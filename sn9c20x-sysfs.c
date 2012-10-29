/**
 * @file sn9c20x-sysfs.c
 * @author Nicolas VIVIEN
 * @date 2008-02-01
 *
 * @brief Sysfs interface for image settings
 *
 * @note Copyright (C) Nicolas VIVIEN
 *
 * @par Licences
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/kref.h>
#include <linux/device.h>

#include <linux/usb.h>
#include <media/v4l2-common.h>

#include "sn9c20x.h"
#include "sn9c20x-bridge.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res)
{
	char *tail;
	unsigned long val;
	size_t len;

	*res = 0;
	len = strlen(cp);
	if (len == 0)
		return -EINVAL;

	val = simple_strtoul(cp, &tail, base);
	if ((*tail == '\0') ||
		((len == (size_t)(tail - cp) + 1) && (*tail == '\n'))) {
		*res = val;
		return 0;
	}

	return -EINVAL;
}

int strict_strtol(const char *cp, unsigned int base, long *res)
{
	int ret;
	if (*cp == '-') {
		ret = strict_strtoul(cp + 1, base, (unsigned long *)res);
		if (!ret)
			*res = -(*res);
	} else
		ret = strict_strtoul(cp, base, (unsigned long *)res);

	return ret;
}
#endif

/**
 * @brief show_release
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'release' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_release(struct class_device *class, char *buf)
#else
static ssize_t show_release(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->release);
}


/**
 * @brief show_videostatus
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'videostatus' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_videostatus(struct class_device *class, char *buf)
#else
static ssize_t show_videostatus(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf,
		"Overflow frames    : %d\n"
		"Incomplete frames  : %d\n"
		"Dropped frames     : %d\n",
		dev->vframes_overflow,
		dev->vframes_incomplete,
		dev->vframes_dropped);
}


/**
 * @brief show_information
 *
 * @param class Class device
 * @param attr
 * @retval buf Address of buffer with the 'information' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_information(struct class_device *class, char *buf)
#else
static ssize_t show_information(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	char *pixelfmt = NULL;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	char *palette_bayer = "BAYER - BGGR - 8 bits";
	char *palette_i420 = "SN9C20X_I420 - YUV 4:2:0 - 12 bits";
	char *palette_jpeg = "JPEG - YUV 4:2:2 - 16 bits";


	switch (dev->vsettings.format.pixelformat) {
	case V4L2_PIX_FMT_SBGGR8:
		pixelfmt = palette_bayer;
		break;

	case V4L2_PIX_FMT_JPEG:
		pixelfmt = palette_jpeg;
		break;

	case V4L2_PIX_FMT_SN9C20X_I420:
		pixelfmt = palette_i420;
		break;
	}

	return sprintf(buf,
			"Driver Resolution : %dx%d\n"
			"\n"
			"%s\n"
			"\n"
			"Brightness        : %u\n"
			"Contrast          : %u\n"
			"Hue               : %d\n"
			"Saturation        : %u\n"
			"Gamma             : %u\n"
			"Exposure          : %u\n"
			"Gain              : %u\n"
			"Sharpness         : %u\n"
			"Red Balance       : %u\n"
			"Blue Balance      : %u\n"
			"Horizontal flip   : %d\n"
			"Vertical flip     : %d\n"
			"Auto-exposure     : %d\n"
			"Auto-gain         : %d\n"
			"Auto-whitebalance : %d\n",
			dev->vsettings.format.width,
			dev->vsettings.format.height,
			pixelfmt,
			dev->vsettings.brightness,
			dev->vsettings.contrast,
			dev->vsettings.hue,
			dev->vsettings.colour,
			dev->vsettings.gamma,
			dev->vsettings.exposure,
			dev->vsettings.gain,
			dev->vsettings.sharpness,
			dev->vsettings.red_gain,
			dev->vsettings.blue_gain,
			dev->vsettings.hflip,
			dev->vsettings.vflip,
			dev->vsettings.auto_exposure,
			dev->vsettings.auto_gain,
			dev->vsettings.auto_whitebalance);
}


/**
 * @brief show_fps
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'fps' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_fps(struct class_device *class, char *buf)
#else
static ssize_t show_fps(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->vsettings.fps);
}

/**
 * @brief show_gain
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'gain' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_gain(struct class_device *class, char *buf)
#else
static ssize_t show_gain(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.gain);
}

/**
 * @brief store_gain
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_gain(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_gain(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_GAIN,
				   value);

	return strlen(buf);
}
/**
 * @brief show_exposure
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'exposure' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_exposure(struct class_device *class, char *buf)
#else
static ssize_t show_exposure(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.exposure);
}

/**
 * @brief store_exposure
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_exposure(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_exposure(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_EXPOSURE,
				   value);

	return strlen(buf);
}

/**
 * @brief show_brightness
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'brightness' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_brightness(struct class_device *class, char *buf)
#else
static ssize_t show_brightness(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.brightness);
}

/**
 * @brief store_brightness
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_brightness(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_brightness(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_BRIGHTNESS,
				   value);

	return strlen(buf);
}

/**
 * @brief show_contrast
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'contrast' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_contrast(struct class_device *class, char *buf)
#else
static ssize_t show_contrast(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.contrast);
}


/**
 * @brief store_contrast
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_contrast(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_contrast(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_CONTRAST,
				   value);

	return strlen(buf);
}




/**
 * @brief show_saturation
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'contrast' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_saturation(struct class_device *class, char *buf)
#else
static ssize_t show_saturation(struct device *class,
		struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.colour);
}


/**
 * @brief store_saturation
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_saturation(struct class_device *class,
				const char *buf,
				size_t count)
#else
static ssize_t store_saturation(struct device *class,
				struct device_attribute *attr,
				const char *buf,
				size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_SATURATION,
				   value);

	return strlen(buf);
}



/**
 * @brief show_hue
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'contrast' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_hue(struct class_device *class,
			char *buf)
#else
static ssize_t show_hue(struct device *class,
			struct device_attribute *attr,
			char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->vsettings.hue);
}


/**
 * @brief store_hue
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_hue(struct class_device *class,
			const char *buf,
			size_t count)
#else
static ssize_t store_hue(struct device *class,
			struct device_attribute *attr,
			const char *buf, size_t count)
#endif
{
	long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtol(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 180 || value < -180)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_HUE,
				   value);

	return strlen(buf);
}




/**
 * @brief show_whitebalance
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'whitebalance' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_gamma(struct class_device *class,
			  char *buf)
#else
static ssize_t show_gamma(struct device *class,
			  struct device_attribute *attr,
			  char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.gamma);
}


/**
 * @brief store_gamma
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_gamma(struct class_device *class,
			   const char *buf,
			   size_t count)
#else
static ssize_t store_gamma(struct device *class,
			   struct device_attribute *attr,
			   const char *buf, size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 255)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_GAMMA,
				   value);

	return strlen(buf);
}




/**
 * @brief show_sharpness
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'sharpness' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_sharpness(struct class_device *class, char *buf)
#else
static ssize_t show_sharpness(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%u\n", dev->vsettings.sharpness);
}

/**
 * @brief store_sharpness
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_sharpness(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_sharpness(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;

	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value > 63)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_SHARPNESS,
				   value);

	return strlen(buf);
}


/**
 * @brief show_hflip
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'hflip' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_hflip(struct class_device *class, char *buf)
#else
static ssize_t show_hflip(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->vsettings.hflip);
}


/**
 * @brief store_hflip
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_hflip(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_hflip(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value != 0 && value != 1)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_HFLIP,
				   value);

	return strlen(buf);
}


/**
 * @brief show_vflip
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'vflip' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_vflip(struct class_device *class, char *buf)
#else
static ssize_t show_vflip(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->vsettings.vflip);
}


/**
 * @brief store_vflip
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_vflip(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_vflip(struct device *class, struct device_attribute *attr, const char *buf, size_t count)
#endif
{
	unsigned long value;
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value != 0 && value != 1)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_VFLIP,
				   value);

	return strlen(buf);
}


/**
 * @brief show_autoexposure
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'hflip' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_autoexposure(struct class_device *class, char *buf)
#else
static ssize_t show_autoexposure(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->vsettings.auto_exposure);
}


/**
 * @brief store_autoexposure
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_autoexposure(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_autoexposure(struct device *class, struct device_attribute *attr,
		const char *buf, size_t count)
#endif
{
	unsigned long value;
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value != 0 && value != 1)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_EXPOSURE_AUTO,
				   value);

	return strlen(buf);
}


/**
 * @brief show_autowhitebalance
 *
 * @param class Class device
 * @param attr
 * @retval buf Adress of buffer with the 'vflip' value
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t show_autowhitebalance(struct class_device *class, char *buf)
#else
static ssize_t show_autowhitebalance(struct device *class, struct device_attribute *attr, char *buf)
#endif
{
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	return sprintf(buf, "%d\n", dev->vsettings.auto_whitebalance);
}


/**
 * @brief store_autowhitebalance
 *
 * @param class Class device
 * @param buf Buffer
 * @param count Counter
 * @param attr
 *
 * @returns Size of buffer
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static ssize_t store_autowhitebalance(struct class_device *class, const char *buf, size_t count)
#else
static ssize_t store_autowhitebalance(struct device *class, struct device_attribute *attr, const char *buf, size_t count)
#endif
{
	unsigned long value;
	struct video_device *vdev = to_video_device(class);
	struct usb_sn9c20x *dev = video_get_drvdata(vdev);

	if (strict_strtoul(buf, 10, &value) < 0)
		return -EINVAL;

	if (value != 0 && value != 1)
		return -EINVAL;

	sn9c20x_set_camera_control(dev,
				   V4L2_CID_AUTO_WHITE_BALANCE,
				   value);

	return strlen(buf);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static CLASS_DEVICE_ATTR(release, S_IRUGO, show_release, NULL);							/**< Release value */
static CLASS_DEVICE_ATTR(videostatus, S_IRUGO, show_videostatus, NULL);						/**< Video status */
static CLASS_DEVICE_ATTR(information, S_IRUGO, show_information, NULL);						/**< Information */
static CLASS_DEVICE_ATTR(fps, S_IRUGO, show_fps, NULL);								/**< FPS value */
static CLASS_DEVICE_ATTR(brightness, S_IRUGO | S_IWUGO, show_brightness, store_brightness);			/**< Brightness value */
static CLASS_DEVICE_ATTR(exposure, S_IRUGO | S_IWUGO, show_exposure, store_exposure);				/**< Exposure value */
static CLASS_DEVICE_ATTR(gain, S_IRUGO | S_IWUGO, show_gain, store_gain);					/**< Gain value */
static CLASS_DEVICE_ATTR(saturation, S_IRUGO | S_IWUGO, show_saturation, store_saturation);			/**< Saturation value */
static CLASS_DEVICE_ATTR(hue, S_IRUGO | S_IWUGO, show_hue, store_hue);						/**< Hue value */
static CLASS_DEVICE_ATTR(contrast, S_IRUGO | S_IWUGO, show_contrast, store_contrast);				/**< Contrast value */
static CLASS_DEVICE_ATTR(gamma, S_IRUGO | S_IWUGO, show_gamma, store_gamma); /**< Gamma value */
static CLASS_DEVICE_ATTR(sharpness, S_IRUGO | S_IWUGO, show_sharpness, store_sharpness);			/**< Sharpness value */
static CLASS_DEVICE_ATTR(hflip, S_IRUGO | S_IWUGO, show_hflip, store_hflip);					/**< Horizontal flip value */
static CLASS_DEVICE_ATTR(vflip, S_IRUGO | S_IWUGO, show_vflip, store_vflip);					/**< Vertical flip value */
static CLASS_DEVICE_ATTR(auto_exposure, S_IRUGO | S_IWUGO, show_autoexposure, store_autoexposure);		/**< Automatic exposure control value */
static CLASS_DEVICE_ATTR(auto_whitebalance, S_IRUGO | S_IWUGO, show_autowhitebalance, store_autowhitebalance);	/**< Automatic whitebalance control value */
#else
static DEVICE_ATTR(release, S_IRUGO, show_release, NULL);							/**< Release value */
static DEVICE_ATTR(videostatus, S_IRUGO, show_videostatus, NULL);						/**< Video status */
static DEVICE_ATTR(information, S_IRUGO, show_information, NULL);						/**< Information */
static DEVICE_ATTR(fps, S_IRUGO, show_fps, NULL);								/**< FPS value */
static DEVICE_ATTR(brightness, S_IRUGO | S_IWUGO, show_brightness, store_brightness);				/**< Brightness value */
static DEVICE_ATTR(exposure, S_IRUGO | S_IWUGO, show_exposure, store_exposure);					/**< Exposure value */
static DEVICE_ATTR(gain, S_IRUGO | S_IWUGO, show_gain, store_gain);						/**< Gain value */
static DEVICE_ATTR(contrast, S_IRUGO | S_IWUGO, show_contrast, store_contrast);					/**< Contrast value */
static DEVICE_ATTR(saturation, S_IRUGO | S_IWUGO, show_saturation, store_saturation);				/**< Saturation value */
static DEVICE_ATTR(hue, S_IRUGO | S_IWUGO, show_hue, store_hue);						/**< Hue value */
static DEVICE_ATTR(gamma, S_IRUGO | S_IWUGO, show_gamma, store_gamma); 						/**< Gamma value */
static DEVICE_ATTR(sharpness, S_IRUGO | S_IWUGO, show_sharpness, store_sharpness);				/**< Sharpness value */
static DEVICE_ATTR(hflip, S_IRUGO | S_IWUGO, show_hflip, store_hflip);						/**< Horizontal flip value */
static DEVICE_ATTR(vflip, S_IRUGO | S_IWUGO, show_vflip, store_vflip);						/**< Vertical flip value */
static DEVICE_ATTR(auto_exposure, S_IRUGO | S_IWUGO, show_autoexposure, store_autoexposure);			/**< Automatic exposure control value */
static DEVICE_ATTR(auto_whitebalance, S_IRUGO | S_IWUGO, show_autowhitebalance, store_autowhitebalance);	/**< Automatic whitebalance control value */
#endif


/**
 * @brief Create the 'sys' entries.
 *
 * This function permits to create all the entries in the 'sys' filesystem.
 *
 * @param vdev Video device structure
 *
 * @returns 0 if all is OK
 */
int sn9c20x_create_sysfs_files(struct video_device *vdev)
{
	int ret;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
	ret = video_device_create_file(vdev, &class_device_attr_release);
	ret = video_device_create_file(vdev, &class_device_attr_videostatus);
	ret = video_device_create_file(vdev, &class_device_attr_information);
	ret = video_device_create_file(vdev, &class_device_attr_fps);
	ret = video_device_create_file(vdev, &class_device_attr_brightness);
	ret = video_device_create_file(vdev, &class_device_attr_exposure);
	ret = video_device_create_file(vdev, &class_device_attr_gain);
	ret = video_device_create_file(vdev, &class_device_attr_contrast);
	ret = video_device_create_file(vdev, &class_device_attr_saturation);
	ret = video_device_create_file(vdev, &class_device_attr_hue);
	ret = video_device_create_file(vdev, &class_device_attr_gamma);
	ret = video_device_create_file(vdev, &class_device_attr_sharpness);
	ret = video_device_create_file(vdev, &class_device_attr_hflip);
	ret = video_device_create_file(vdev, &class_device_attr_vflip);
	ret = video_device_create_file(vdev, &class_device_attr_auto_exposure);
	ret = video_device_create_file(vdev, &class_device_attr_auto_whitebalance);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
	ret = video_device_create_file(vdev, &dev_attr_release);
	ret = video_device_create_file(vdev, &dev_attr_videostatus);
	ret = video_device_create_file(vdev, &dev_attr_information);
	ret = video_device_create_file(vdev, &dev_attr_fps);
	ret = video_device_create_file(vdev, &dev_attr_brightness);
	ret = video_device_create_file(vdev, &dev_attr_exposure);
	ret = video_device_create_file(vdev, &dev_attr_gain);
	ret = video_device_create_file(vdev, &dev_attr_contrast);
	ret = video_device_create_file(vdev, &dev_attr_saturation);
	ret = video_device_create_file(vdev, &dev_attr_hue);
	ret = video_device_create_file(vdev, &dev_attr_gamma);
	ret = video_device_create_file(vdev, &dev_attr_sharpness);
	ret = video_device_create_file(vdev, &dev_attr_hflip);
	ret = video_device_create_file(vdev, &dev_attr_vflip);
	ret = video_device_create_file(vdev, &dev_attr_auto_exposure);
	ret = video_device_create_file(vdev, &dev_attr_auto_whitebalance);
#else
	ret = device_create_file(&vdev->dev, &dev_attr_release);
	ret = device_create_file(&vdev->dev, &dev_attr_videostatus);
	ret = device_create_file(&vdev->dev, &dev_attr_information);
	ret = device_create_file(&vdev->dev, &dev_attr_fps);
	ret = device_create_file(&vdev->dev, &dev_attr_brightness);
	ret = device_create_file(&vdev->dev, &dev_attr_exposure);
	ret = device_create_file(&vdev->dev, &dev_attr_gain);
	ret = device_create_file(&vdev->dev, &dev_attr_contrast);
	ret = device_create_file(&vdev->dev, &dev_attr_saturation);
	ret = device_create_file(&vdev->dev, &dev_attr_hue);
	ret = device_create_file(&vdev->dev, &dev_attr_gamma);
	ret = device_create_file(&vdev->dev, &dev_attr_sharpness);
	ret = device_create_file(&vdev->dev, &dev_attr_hflip);
	ret = device_create_file(&vdev->dev, &dev_attr_vflip);
	ret = device_create_file(&vdev->dev, &dev_attr_auto_exposure);
	ret = device_create_file(&vdev->dev, &dev_attr_auto_whitebalance);
#endif
	return ret;
}


/**
 * @brief Remove the 'sys' entries.
 *
 * This function permits to remove all the entries in the 'sys' filesystem.
 *
 * @param vdev Video device structure
 *
 * @returns 0 if all is OK
 */
void sn9c20x_remove_sysfs_files(struct video_device *vdev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
	video_device_remove_file(vdev, &class_device_attr_release);
	video_device_remove_file(vdev, &class_device_attr_videostatus);
	video_device_remove_file(vdev, &class_device_attr_information);
	video_device_remove_file(vdev, &class_device_attr_fps);
	video_device_remove_file(vdev, &class_device_attr_brightness);
	video_device_remove_file(vdev, &class_device_attr_exposure);
	video_device_remove_file(vdev, &class_device_attr_gain);
	video_device_remove_file(vdev, &class_device_attr_contrast);
	video_device_remove_file(vdev, &class_device_attr_saturation);
	video_device_remove_file(vdev, &class_device_attr_hue);
	video_device_remove_file(vdev, &class_device_attr_gamma);
	video_device_remove_file(vdev, &class_device_attr_sharpness);
	video_device_remove_file(vdev, &class_device_attr_hflip);
	video_device_remove_file(vdev, &class_device_attr_vflip);
	video_device_remove_file(vdev, &class_device_attr_auto_exposure);
	video_device_remove_file(vdev, &class_device_attr_auto_whitebalance);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
	video_device_remove_file(vdev, &dev_attr_release);
	video_device_remove_file(vdev, &dev_attr_videostatus);
	video_device_remove_file(vdev, &dev_attr_information);
	video_device_remove_file(vdev, &dev_attr_fps);
	video_device_remove_file(vdev, &dev_attr_brightness);
	video_device_remove_file(vdev, &dev_attr_exposure);
	video_device_remove_file(vdev, &dev_attr_gain);
	video_device_remove_file(vdev, &dev_attr_contrast);
	video_device_remove_file(vdev, &dev_attr_saturation);
	video_device_remove_file(vdev, &dev_attr_hue);
	video_device_remove_file(vdev, &dev_attr_gamma);
	video_device_remove_file(vdev, &dev_attr_sharpness);
	video_device_remove_file(vdev, &dev_attr_hflip);
	video_device_remove_file(vdev, &dev_attr_vflip);
	video_device_remove_file(vdev, &dev_attr_auto_exposure);
	video_device_remove_file(vdev, &dev_attr_auto_whitebalance);
#else
	device_remove_file(&vdev->dev, &dev_attr_release);
	device_remove_file(&vdev->dev, &dev_attr_videostatus);
	device_remove_file(&vdev->dev, &dev_attr_information);
	device_remove_file(&vdev->dev, &dev_attr_fps);
	device_remove_file(&vdev->dev, &dev_attr_brightness);
	device_remove_file(&vdev->dev, &dev_attr_exposure);
	device_remove_file(&vdev->dev, &dev_attr_gain);
	device_remove_file(&vdev->dev, &dev_attr_contrast);
	device_remove_file(&vdev->dev, &dev_attr_saturation);
	device_remove_file(&vdev->dev, &dev_attr_hue);
	device_remove_file(&vdev->dev, &dev_attr_gamma);
	device_remove_file(&vdev->dev, &dev_attr_sharpness);
	device_remove_file(&vdev->dev, &dev_attr_hflip);
	device_remove_file(&vdev->dev, &dev_attr_vflip);
	device_remove_file(&vdev->dev, &dev_attr_auto_exposure);
	device_remove_file(&vdev->dev, &dev_attr_auto_whitebalance);
#endif
}

