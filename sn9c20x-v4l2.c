/**
 * @file sn9c20x-v4l2.c
 * @author Nicolas VIVIEN
 * @date 2008-02-01
 *
 * @brief V4L2 interface and functions
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
#include <linux/vmalloc.h>
#include <linux/usb.h>
#include <linux/mm.h>

#include "sn9c20x.h"
#include "sn9c20x-bridge.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <media/v4l2-ioctl.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
static struct file_operations v4l_sn9c20x_fops;
#else
static struct v4l2_file_operations v4l_sn9c20x_fops;
#endif
/**
 * @var sn9c20x_controls
 *   List of all V4Lv2 controls supported by the driver
 */
static struct v4l2_queryctrl sn9c20x_controls[] = {
	{
		.id      = V4L2_CID_BRIGHTNESS,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Brightness",
		.minimum = 0,
		.maximum = 0xff,
		.step    = 1,
	},
	{
		.id      = V4L2_CID_GAMMA,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Gamma",
		.minimum = 0,
		.maximum = 0xff,
		.step    = 1,
	},

	{
		.id      = V4L2_CID_SATURATION,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Saturation",
		.minimum = 0,
		.maximum = 0xff,
		.step    = 1,
	},

	{
		.id      = V4L2_CID_HUE,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Hue",
		.minimum = -180,
		.maximum = 180,
		.step    = 1,
	},

	{
		.id      = V4L2_CID_CONTRAST,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Contrast",
		.minimum = 0,
		.maximum = 0xff,
		.step    = 1,
	},
	{
		.id	 = V4L2_CID_EXPOSURE,
		.type	 = V4L2_CTRL_TYPE_INTEGER,
		.name	 = "Exposure",
		.minimum = 0,
		.maximum = 0xff,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_GAIN,
		.type	 = V4L2_CTRL_TYPE_INTEGER,
		.name	 = "Gain",
		.minimum = 0,
		.maximum = 0xff,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_HFLIP,
		.type	 = V4L2_CTRL_TYPE_BOOLEAN,
		.name	 = "Horizontal flip",
		.minimum = 0,
		.maximum = 1,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_VFLIP,
		.type	 = V4L2_CTRL_TYPE_BOOLEAN,
		.name	 = "Vertical flip",
		.minimum = 0,
		.maximum = 1,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_SHARPNESS,
		.type	 = V4L2_CTRL_TYPE_INTEGER,
		.name	 = "Sharpness",
		.minimum = 0,
		.maximum = 0x3f,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_RED_BALANCE,
		.type	 = V4L2_CTRL_TYPE_INTEGER,
		.name	 = "Red Balance",
		.minimum = 0,
		.maximum = 0x7f,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_BLUE_BALANCE,
		.type	 = V4L2_CTRL_TYPE_INTEGER,
		.name	 = "Blue Balance",
		.minimum = 0,
		.maximum = 0x7f,
		.step	 = 1,
	},
/* According to v4l2 specs auto exposure should be a 4 step value.
 * This make little since for webcams however so a boolean is used
 * instead.
 */
	{
		.id	 = V4L2_CID_EXPOSURE_AUTO,
		.type	 = V4L2_CTRL_TYPE_BOOLEAN,
		.name	 = "Automatic exposure control",
		.minimum = 0,
		.maximum = 1,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_AUTOGAIN,
		.type	 = V4L2_CTRL_TYPE_BOOLEAN,
		.name	 = "Automatic gain control",
		.minimum = 0,
		.maximum = 1,
		.step	 = 1,
	},
	{
		.id	 = V4L2_CID_AUTO_WHITE_BALANCE,
		.type	 = V4L2_CTRL_TYPE_BOOLEAN,
		.name	 = "Automatic whitbalance control",
		.minimum = 0,
		.maximum = 1,
		.step	 = 1,
	},
};

void v4l2_set_control_default(struct usb_sn9c20x *dev, __u32 ctrl, __u16 value)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(sn9c20x_controls); i++) {
		if (sn9c20x_controls[i].id == ctrl) {
			sn9c20x_controls[i].default_value = value;
			sn9c20x_set_camera_control(dev,
						   ctrl,
						   value);
			break;
		}
	}
}

void v4l_add_jpegheader(struct usb_sn9c20x *dev, __u8 *buffer,
	__u32 buffer_size)
{
	static __u8 jpeg_header[589] = {
		0xff, 0xd8, 0xff, 0xdb, 0x00, 0x84, 0x00, 0x06, 0x04, 0x05,
		0x06, 0x05, 0x04, 0x06, 0x06, 0x05, 0x06, 0x07, 0x07, 0x06,
		0x08, 0x0a, 0x10, 0x0a, 0x0a, 0x09, 0x09, 0x0a, 0x14, 0x0e,
		0x0f, 0x0c, 0x10, 0x17, 0x14, 0x18, 0x18, 0x17, 0x14, 0x16,
		0x16, 0x1a, 0x1d, 0x25, 0x1f, 0x1a, 0x1b, 0x23, 0x1c, 0x16,
		0x16, 0x20, 0x2c, 0x20, 0x23, 0x26, 0x27, 0x29, 0x2a, 0x29,
		0x19, 0x1f, 0x2d, 0x30, 0x2d, 0x28, 0x30, 0x25, 0x28, 0x29,
		0x28, 0x01, 0x07, 0x07, 0x07, 0x0a, 0x08, 0x0a, 0x13, 0x0a,
		0x0a, 0x13, 0x28, 0x1a, 0x16, 0x1a, 0x28, 0x28, 0x28, 0x28,
		0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
		0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
		0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
		0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
		0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0xff, 0xc4, 0x01, 0xa2,
		0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
		0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x01,
		0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x00,
		0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04,
		0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04,
		0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61,
		0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23,
		0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62,
		0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25,
		0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
		0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64,
		0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
		0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
		0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
		0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa,
		0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
		0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3,
		0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3,
		0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3,
		0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0x11, 0x00, 0x02,
		0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04,
		0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
		0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
		0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1,
		0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1,
		0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19,
		0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
		0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64,
		0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
		0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
		0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
		0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
		0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
		0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3,
		0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4,
		0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xc0, 0x00, 0x11,
		0x08, 0x01, 0xe0, 0x02, 0x80, 0x03, 0x01, 0x21, 0x00, 0x02,
		0x11, 0x01, 0x03, 0x11, 0x01, 0xff, 0xda, 0x00, 0x0c, 0x03,
		0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00
	};
	static __u8 qtable1[128] = {
		0x0d, 0x08, 0x08, 0x0d, 0x08, 0x08, 0x0d, 0x0d,
		0x0d, 0x0d, 0x11, 0x0d, 0x0d, 0x11, 0x15, 0x21,
		0x15, 0x15, 0x11, 0x11, 0x15, 0x2a, 0x1d, 0x1d,
		0x19, 0x21, 0x32, 0x2a, 0x32, 0x32, 0x2e, 0x2a,
		0x2e, 0x2e, 0x36, 0x3a, 0x4b, 0x43, 0x36, 0x3a,
		0x47, 0x3a, 0x2e, 0x2e, 0x43, 0x5c, 0x43, 0x47,
		0x4f, 0x54, 0x58, 0x58, 0x58, 0x32, 0x3f, 0x60,
		0x64, 0x5c, 0x54, 0x64, 0x4b, 0x54, 0x58, 0x54,
		0x0d, 0x11, 0x11, 0x15, 0x11, 0x15, 0x26, 0x15,
		0x15, 0x26, 0x54, 0x36, 0x2e, 0x36, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54
	};

	jpeg_header[6] = 0x00;
	jpeg_header[71] = 0x01;
	memcpy(jpeg_header + 7, qtable1, 64);
	memcpy(jpeg_header + 8 + 64, qtable1+64, 64);
	jpeg_header[564] = dev->vsettings.format.width & 0xFF;
	jpeg_header[563] = (dev->vsettings.format.width >> 8) & 0xFF;
	jpeg_header[562] = dev->vsettings.format.height & 0xFF;
	jpeg_header[561] = (dev->vsettings.format.height >> 8) & 0xFF;
	jpeg_header[567] = 0x21;

	memmove(buffer+589, buffer, buffer_size);
	memcpy(buffer, jpeg_header, 589);
}
/**
 * @brief Get V4L privileges
 *
 * @param file
 *
 * @return 0 or negative error code
 *
 */
int v4l_get_privileges(struct file *file)
{
	struct usb_sn9c20x *dev;
	int ret = 0;

	dev = video_get_drvdata(video_devdata(file));

	if (dev->owner == file)
		return 0;

	mutex_lock(&open_lock);
	if (dev->owner != NULL) {
		ret = -EBUSY;
		goto done;
	}
	dev->owner = file;
done:
	mutex_unlock(&open_lock);
	return ret;
}

/**
 * @brief Check whether there are V4L privileges
 *
 * @param file
 *
 * @return 0 or 1
 *
 */
int v4l_has_privileges(struct file *file)
{
	struct usb_sn9c20x *dev;
	int ret = 0;

	dev = video_get_drvdata(video_devdata(file));

	if (dev->owner == file)
		ret = 1;

	return ret;
}

/**
 * @brief Drop V4L privileges
 *
 * @param file
 *
 */
void v4l_drop_privileges(struct file *file)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(video_devdata(file));

	if (dev->owner == file)
		dev->owner = NULL;
}

/**
 * @brief Enable video stream
 *
 * @param dev Pointer to device structure
 * @param mode Mode for video stream
 *
 * @returns 0 or negative error value
 *
 */
int v4l2_enable_video(struct usb_sn9c20x *dev, int mode)
{
	int ret;

	if (mode == SN9C20X_MODE_IDLE) {
		sn9c20x_enable_video(dev, 0);
		usb_sn9c20x_uninit_urbs(dev, 1);
		sn9c20x_queue_enable(&dev->queue, 0);
		dev->mode = mode;
		return 0;
	}

	if (dev->mode != SN9C20X_MODE_IDLE)
		return -EBUSY;

	if (sn9c20x_queue_enable(&dev->queue, 1) < 0)
		return -EBUSY;

	ret = usb_sn9c20x_init_urbs(dev);

	if (ret)
		return ret;

	sn9c20x_enable_video(dev, 1);
	dev->mode = mode;

	if (dev->vsettings.format.pixelformat == V4L2_PIX_FMT_JPEG)
		dev->queue.flags &= ~SN9C20X_QUEUE_DROP_INCOMPLETE;
	else
		dev->queue.flags |= SN9C20X_QUEUE_DROP_INCOMPLETE;

	return 0;
}

/**
 * @param inode Pointer on an inode
 * @param fp File pointer
 *
 * @returns 0 if all is OK
 *
 * @brief Open the video device
 *
 * This function permits to open a video device (/dev/videoX)
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
static int v4l_sn9c20x_open(struct inode *inode, struct file *fp)
#else
static int v4l_sn9c20x_open(struct file *fp)
#endif
{
	int ret = 0;

	struct usb_sn9c20x *dev;
	struct video_device *vdev;

	mutex_lock(&open_lock);

	vdev = video_devdata(fp);
	dev = video_get_drvdata(video_devdata(fp));

	fp->private_data = vdev;

	kref_get(&dev->vopen);

	mutex_unlock(&open_lock);
	return ret;
}


/**
 * @param inode Pointer on inode
 * @param fp File pointer
 *
 * @returns 0 if all is OK
 *
 * @brief Release an opened file.
 *
 * This function permits to release an opened file with the 'open' method.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
static int v4l_sn9c20x_release(struct inode *inode, struct file *fp)
#else
static int v4l_sn9c20x_release(struct file *fp)
#endif
{
	struct usb_sn9c20x *dev;
	struct video_device *vdev;

	mutex_lock(&open_lock);

	vdev = video_devdata(fp);
	dev = video_get_drvdata(video_devdata(fp));

	if (v4l_has_privileges(fp)) {
		v4l2_enable_video(dev, SN9C20X_MODE_IDLE);

		mutex_lock(&dev->queue.mutex);
		sn9c20x_free_buffers(&dev->queue);
		mutex_unlock(&dev->queue.mutex);
	}

	v4l_drop_privileges(fp);

	kref_put(&dev->vopen, usb_sn9c20x_delete);

	mutex_unlock(&open_lock);
	return 0;
}


/**
 * @param fp File pointer
 *
 * @retval buf Buffer in user space
 * @retval count
 * @retval f_pos
 *
 * @returns Count value
 *
 * @brief Read the video device
 *
 * This function is called by the application is reading the video device.
 */
static ssize_t v4l_sn9c20x_read(struct file *fp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	int i, ret;
	int nbuffers;
	struct v4l2_buffer buffer;
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(video_devdata(fp));

	ret = v4l_get_privileges(fp);
	if (unlikely(ret < 0))
		return ret;

	if (unlikely(dev->mode != SN9C20X_MODE_IDLE &&
	    dev->mode != SN9C20X_MODE_READ))
		return -EBUSY;

	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	if (dev->mode == SN9C20X_MODE_IDLE) {
		nbuffers = sn9c20x_alloc_buffers(&dev->queue, 2,
					     dev->vsettings.format.sizeimage);
		if (nbuffers < 0)
			return nbuffers;

		for (i = 0; i < nbuffers; i++) {
			buffer = dev->queue.buffer[i].buf;
			sn9c20x_queue_buffer(&dev->queue, &buffer);
		}

		ret = v4l2_enable_video(dev, SN9C20X_MODE_READ);
		if (ret < 0)
			return ret;
	}

	dev_sn9c20x_call_constantly(dev);

	if (dev->queue.read_buffer == NULL) {
		ret = sn9c20x_dequeue_buffer(&dev->queue, &buffer,
					      fp->f_flags & O_NONBLOCK);
		if (ret < 0)
			return ret;

		if (dev->vsettings.format.pixelformat == V4L2_PIX_FMT_JPEG) {
			UDIA_DEBUG("Adding JPEG Header\n");
			v4l_add_jpegheader(dev, dev->queue.mem + buffer.m.offset,
					   buffer.bytesused);
			buffer.bytesused += 589;
		}

		dev->queue.read_buffer = &dev->queue.buffer[buffer.index];
	} else {
		buffer = dev->queue.read_buffer->buf;
	}

	count = min((size_t)(buffer.bytesused - *f_pos), count);
	if (copy_to_user(buf, dev->queue.mem + buffer.m.offset + *f_pos, count))
		return -EFAULT;

	*f_pos += count;
	if (*f_pos >= buffer.bytesused) {
		dev->queue.read_buffer = NULL;
		sn9c20x_queue_buffer(&dev->queue, &buffer);
		*f_pos = 0;
	}
	return count;
}


/**
 * @param fp File pointer
 * @param wait
 *
 * @returns 0 if all is OK
 *
 * @brief Polling function
 */
static unsigned int v4l_sn9c20x_poll(struct file *fp, poll_table *wait)
{
	struct usb_sn9c20x *dev;
	struct video_device *vdev;

	vdev = video_devdata(fp);
	dev = video_get_drvdata(video_devdata(fp));

	UDIA_STREAM("Poll\n");

	if (vdev == NULL || dev == NULL)
		return -EFAULT;

	return sn9c20x_queue_poll(&dev->queue, fp, wait);
}

/**
 * @param vma
 *
 */
static void sn9c20x_vm_open(struct vm_area_struct *vma)
{
	struct sn9c20x_buffer *buffer = vma->vm_private_data;
	buffer->vma_use_count++;

}

/**
 * @param vma
 *
 */
static void sn9c20x_vm_close(struct vm_area_struct *vma)
{
	struct sn9c20x_buffer *buffer = vma->vm_private_data;
	buffer->vma_use_count--;
}

struct vm_operations_struct sn9c20x_vm_ops = {
	.open	= sn9c20x_vm_open,
	.close	= sn9c20x_vm_close
};

/**
 * @param fp File pointer
 * @param vma VMA structure
 *
 * @returns 0 if all is OK
 *
 * @brief Memory map
 *
 * This function permits to map a memory space.
 */
static int v4l_sn9c20x_mmap(struct file *fp, struct vm_area_struct *vma)
{
	struct page *page;
	unsigned long addr, start, size;
	unsigned int i;
	int ret = 0;

	struct usb_sn9c20x *dev;
	struct video_device *vdev;
	struct sn9c20x_buffer *buffer = NULL;

	vdev = video_devdata(fp);
	dev = video_get_drvdata(video_devdata(fp));

	UDIA_STREAM("mmap\n");

	start = vma->vm_start;
	size = vma->vm_end - vma->vm_start;

	mutex_lock(&dev->queue.mutex);

	for (i = 0; i < dev->queue.count; ++i) {
		buffer = &dev->queue.buffer[i];
		if ((buffer->buf.m.offset >> PAGE_SHIFT) == vma->vm_pgoff)
			break;
	}

	if (i == dev->queue.count || size != dev->queue.buf_size) {
		ret = -EINVAL;
		goto done;
	}

	vma->vm_flags |= VM_IO;

	addr = (unsigned long)dev->queue.mem + buffer->buf.m.offset;
	while (size > 0) {
		page = vmalloc_to_page((void *)addr);
		ret = vm_insert_page(vma, start, page);
		if (ret < 0)
			goto done;

		start += PAGE_SIZE;
		addr += PAGE_SIZE;
		size -= PAGE_SIZE;
	}

	vma->vm_ops = &sn9c20x_vm_ops;
	vma->vm_private_data = buffer;
	sn9c20x_vm_open(vma);
done:
	mutex_unlock(&dev->queue.mutex);
	return ret;
}

/**
 * @param file
 * @param priv
 * @param cap
 *
 * @return 0
 *
 */
int sn9c20x_vidioc_querycap(struct file *file, void *priv,
	struct v4l2_capability *cap)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_QUERYCAP\n");

	strlcpy(cap->driver, "sn9c20x", sizeof(cap->driver));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING
		| V4L2_CAP_READWRITE;
	cap->version = (__u32) DRIVER_VERSION_NUM,
		strlcpy(cap->card, dev->vdev->name, sizeof(cap->card));

	if (usb_make_path(dev->udev, cap->bus_info, sizeof(cap->bus_info)) < 0)
		strlcpy(cap->bus_info, dev->vdev->name, sizeof(cap->bus_info));
	return 0;
}

/**
 * @param file
 * @param priv
 * @param cap
 *
 * @return 0
 *
 */
int sn9c20x_vidioc_enum_framesizes(struct file *file, void *priv,
	struct v4l2_frmsizeenum *size)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("ENUM_FRAMESIZES\n");

	if (size->index >= SN9C20X_N_MODES)
		return -EINVAL;

	size->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	size->discrete.width = sn9c20x_modes[size->index].width;
	size->discrete.height = sn9c20x_modes[size->index].height;

	if (dev->camera.set_sxga_mode == NULL &&
	   (size->discrete.width > 640 && size->discrete.height > 480))
		return -EINVAL;

	if (size->pixel_format != V4L2_PIX_FMT_SBGGR8 &&
	   (size->discrete.width > 640 && size->discrete.height > 480))
		return -EINVAL;

	UDIA_DEBUG("Framesize: %dx%d, FMT: %X\n", size->discrete.width,
						  size->discrete.height,
						  size->pixel_format);

	return 0;
}

/**
 * @param file
 * @param priv
 * @param input
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_enum_input(struct file *file, void *priv,
	struct v4l2_input *input)
{
	UDIA_DEBUG("VIDIOC_ENUMINPUT %d\n", input->index);

	if (input->index)
		return -EINVAL;

	strlcpy(input->name, "Webcam", sizeof(input->name));
	input->type = V4L2_INPUT_TYPE_CAMERA;
	input->std = 0;

	return 0;
}

/**
 * @param file
 * @param priv
 * @param index
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_g_input(struct file *file, void *priv, unsigned int *index)
{
	UDIA_DEBUG("GET INPUT %d\n", *index);

	if (index)
		return -EINVAL;

	return 0;
}

/**
 * @param file
 * @param priv
 * @param index
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_s_input(struct file *file, void *priv, unsigned int index)
{
	UDIA_DEBUG("SET INPUT %d\n", index);

	if (v4l_get_privileges(file) < 0)
		return -EBUSY;

	if (index != 0)
		return -EINVAL;

	return 0;
}

/**
 * @param file
 * @param priv
 * @param ctrl
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_queryctrl(struct file *file, void *priv,
	struct v4l2_queryctrl *ctrl)
{
	int i;
	int ret = 0;
#ifdef V4L2_CTRL_FLAG_NEXT_CTRL
	int min;
	__u32 idnew, idlast;
#endif
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_QUERYCTRL id = %d\n", ctrl->id);

#ifdef V4L2_CTRL_FLAG_NEXT_CTRL
	if (ctrl->id & V4L2_CTRL_FLAG_NEXT_CTRL) {
		min = 0;
		idnew = V4L2_CTRL_FLAG_NEXT_CTRL;
		idlast = ctrl->id & ~V4L2_CTRL_FLAG_NEXT_CTRL;
		for (i = 0; i < ARRAY_SIZE(sn9c20x_controls); i++) {
			if ((sn9c20x_controls[i].id < idnew) &&
				(sn9c20x_controls[i].id > idlast)) {
					idnew = sn9c20x_controls[i].id;
					min = i;
			}
		}
		if (idnew != V4L2_CTRL_FLAG_NEXT_CTRL) {
			UDIA_DEBUG("VIDIOC_QUERYCTRL found\n");
			memcpy(ctrl, &sn9c20x_controls[min],
				sizeof(struct v4l2_queryctrl));
			goto done;
		} else {
			goto error;
		}
	} else
#endif
	{
		for (i = 0; i < ARRAY_SIZE(sn9c20x_controls); i++) {
			if (sn9c20x_controls[i].id == ctrl->id) {
				UDIA_DEBUG("VIDIOC_QUERYCTRL found\n");
				memcpy(ctrl, &sn9c20x_controls[i],
					sizeof(struct v4l2_queryctrl));
				goto done;
			}
		}
	}

error:
	ret = -EINVAL;
done:
	if ((ctrl->id == V4L2_CID_GAIN && dev->vsettings.auto_gain) ||
	    (ctrl->id == V4L2_CID_EXPOSURE && dev->vsettings.auto_exposure) ||
	    ((ctrl->id == V4L2_CID_BLUE_BALANCE ||
	      ctrl->id == V4L2_CID_RED_BALANCE) &&
	     dev->vsettings.auto_whitebalance)) {
		ctrl->flags |= V4L2_CTRL_FLAG_GRABBED;
	}
	return ret;
}

/**
 * @param file
 * @param priv
 * @param ctrl
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_g_ctrl(struct file *file, void *priv,
	struct v4l2_control *ctrl)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("GET CTRL id=%d\n", ctrl->id);

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		ctrl->value = dev->vsettings.brightness;
		break;

	case V4L2_CID_EXPOSURE:
		ctrl->value = dev->vsettings.exposure;
		break;

	case V4L2_CID_GAIN:
		ctrl->value = dev->vsettings.gain;
		break;

	case V4L2_CID_GAMMA:
		ctrl->value = dev->vsettings.gamma;
		break;

	case V4L2_CID_SATURATION:
		ctrl->value = dev->vsettings.colour;
		break;

	case V4L2_CID_HUE:
		ctrl->value = dev->vsettings.hue;
		break;

	case V4L2_CID_CONTRAST:
		ctrl->value = dev->vsettings.contrast;
		break;

	case V4L2_CID_HFLIP:
		ctrl->value = dev->vsettings.hflip;
		break;

	case V4L2_CID_VFLIP:
		ctrl->value = dev->vsettings.vflip;
		break;

	case V4L2_CID_SHARPNESS:
		ctrl->value = dev->vsettings.sharpness;
		break;

	case V4L2_CID_RED_BALANCE:
		ctrl->value = dev->vsettings.red_gain;
		break;

	case V4L2_CID_BLUE_BALANCE:
		ctrl->value = dev->vsettings.blue_gain;
		break;

	case V4L2_CID_EXPOSURE_AUTO:
		ctrl->value = dev->vsettings.auto_exposure;
		break;

	case V4L2_CID_AUTOGAIN:
		ctrl->value = dev->vsettings.auto_gain;
		break;

	case V4L2_CID_AUTO_WHITE_BALANCE:
		ctrl->value = dev->vsettings.auto_whitebalance;
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

/**
 * @brief Apply v4l2 settings on camera
 *
 * @param file
 * @param priv
 * @param ctrl V4L2 control structure
 *
 * @returns 0 or negative error value
 *
 */
int sn9c20x_vidioc_s_ctrl(struct file *file, void *priv,
	struct v4l2_control *ctrl)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("SET CTRL id=%d value=%d\n", ctrl->id, ctrl->value);

	if ((ctrl->id == V4L2_CID_GAIN && dev->vsettings.auto_gain) ||
	    (ctrl->id == V4L2_CID_EXPOSURE && dev->vsettings.auto_exposure)) {
		return -EBUSY;
	}

	return sn9c20x_set_camera_control(dev,
					  ctrl->id,
					  ctrl->value);
}

/**
 * @param file
 * @param priv
 * @param fmt
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_enum_fmt_cap(struct file *file, void *priv,
	struct v4l2_fmtdesc *fmt)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_ENUM_FMT %d\n", fmt->index);

	if (fmt->index >= SN9C20X_N_FMTS)
		return -EINVAL;

	fmt->flags = 0;
	fmt->pixelformat = sn9c20x_fmts[fmt->index].pix_fmt;

	if (fmt->pixelformat == V4L2_PIX_FMT_JPEG && dev->jpeg == 0)
		return -EINVAL;

	memcpy(fmt->description, sn9c20x_fmts[fmt->index].desc, 32);

	return 0;
}

/**
 * @param file
 * @param priv
 * @param fmt
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_try_fmt_cap(struct file *file, void *priv,
	struct v4l2_format *fmt)
{
	int index;
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);
	UDIA_DEBUG("TRY FMT %d\n", fmt->type);

/*	when this code is used prevents mplayer from setting outfmt
	if(fmt->fmt.pix.field != V4L2_FIELD_NONE)
		return -EINVAL;
*/
	if (fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_JPEG && dev->jpeg == 0)
		return -EINVAL;

	sn9c20x_get_closest_resolution(dev, &fmt->fmt.pix.width,
				       &fmt->fmt.pix.height);

	if (fmt->fmt.pix.pixelformat != V4L2_PIX_FMT_SBGGR8 &&
	   (fmt->fmt.pix.width > 640 && fmt->fmt.pix.height > 480))
		fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;

	for (index = 0; index < SN9C20X_N_FMTS; index++)
		if (sn9c20x_fmts[index].pix_fmt == fmt->fmt.pix.pixelformat)
			break;

	if (index >= SN9C20X_N_FMTS)
		return -EINVAL;

	fmt->fmt.pix.bytesperline = fmt->fmt.pix.width *
		sn9c20x_fmts[index].depth / 8;

	fmt->fmt.pix.sizeimage = fmt->fmt.pix.height *
			fmt->fmt.pix.bytesperline;

	fmt->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
	fmt->fmt.pix.priv = index;

	return 0;
}

/**
 * @param file
 * @param priv
 * @param fmt
 *
 * @return 0
 *
 */
int sn9c20x_vidioc_g_fmt_cap(struct file *file, void *priv,
	struct v4l2_format *fmt)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("GET FMT %d\n", fmt->type);

	memcpy(&(fmt->fmt.pix), &(dev->vsettings.format), sizeof(fmt->fmt.pix));


	return 0;
}

/**
 * @param file
 * @param priv
 * @param fmt
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_s_fmt_cap(struct file *file, void *priv,
	struct v4l2_format *fmt)
{
	struct usb_sn9c20x *dev;
	int ret;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("SET FMT %d : %d\n", fmt->type, fmt->fmt.pix.pixelformat);

	if (v4l_get_privileges(file) < 0)
		return -EBUSY;

	if (sn9c20x_queue_streaming(&dev->queue))
		return -EBUSY;

	ret = sn9c20x_vidioc_try_fmt_cap(file, priv, fmt);
	if (ret)
		return -EINVAL;

	sn9c20x_set_resolution(dev, fmt->fmt.pix.width, fmt->fmt.pix.height);
	sn9c20x_set_format(dev, fmt->fmt.pix.pixelformat);
	memcpy(&(dev->vsettings.format), &(fmt->fmt.pix), sizeof(fmt->fmt.pix));

	return 0;
}

/**
 * @param file
 * @param priv
 * @param request
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_reqbufs(struct file *file, void *priv,
	struct v4l2_requestbuffers *request)
{
	int ret = 0;
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	if (v4l_get_privileges(file) < 0) {
		ret = -EBUSY;
		goto done;
	}

	if (request->memory != V4L2_MEMORY_MMAP ||
	    request->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		ret = -EINVAL;
		goto done;
	}

	if (sn9c20x_queue_streaming(&dev->queue)) {
		ret = -EBUSY;
		goto done;
	}

	ret = sn9c20x_alloc_buffers(&dev->queue, request->count,
				     dev->vsettings.format.sizeimage);
	if (ret < 0)
		goto done;

	request->count = ret;
	ret = 0;
	UDIA_DEBUG("Buffers Allocated %d\n", request->count);
done:
	return ret;
}

/**
 * @param file
 * @param priv
 * @param buffer
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_querybuf(struct file *file, void *priv,
	struct v4l2_buffer *buffer)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("QUERY BUFFERS %d %d\n", buffer->index, dev->queue.count);

	if (buffer->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (!v4l_has_privileges(file))
		return -EBUSY;

	return sn9c20x_query_buffer(&dev->queue, buffer);
}

/**
 * @param file
 * @param priv
 * @param buffer
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_qbuf(struct file *file, void *priv,
	struct v4l2_buffer *buffer)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_QBUF\n");

	if (!v4l_has_privileges(file))
		return -EBUSY;

	return sn9c20x_queue_buffer(&dev->queue, buffer);
}

/**
 * @param file
 * @param priv
 * @param buffer
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_dqbuf(struct file *file, void *priv,
	struct v4l2_buffer *buffer)
{
	struct usb_sn9c20x *dev;
	int ret = 0;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_DQBUF\n");

	if (!v4l_has_privileges(file))
		return -EBUSY;

	ret = sn9c20x_dequeue_buffer(&dev->queue, buffer,
			file->f_flags & O_NONBLOCK);
	if (ret < 0)
		return ret;

	if (dev->vsettings.format.pixelformat == V4L2_PIX_FMT_JPEG) {
		UDIA_DEBUG("Adding JPEG Header\n");
		v4l_add_jpegheader(dev, dev->queue.mem + buffer->m.offset,
				   buffer->bytesused);
		buffer->bytesused += 589;
	}

	dev_sn9c20x_call_constantly(dev);

	return ret;
}

/**
 * @param file
 * @param priv
 * @param type
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_streamon(struct file *file, void *priv,
	enum v4l2_buf_type type)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_STREAMON\n");

	if (!v4l_has_privileges(file))
		return -EBUSY;

	if (dev->mode != SN9C20X_MODE_IDLE)
		return -EBUSY;

	return v4l2_enable_video(dev, SN9C20X_MODE_STREAM);
}

/**
 * @param file
 * @param priv
 * @param type
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_streamoff(struct file *file, void *priv,
	enum v4l2_buf_type type)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	UDIA_DEBUG("VIDIOC_STREAMOFF\n");

	if (!v4l_has_privileges(file))
		return -EBUSY;

	return v4l2_enable_video(dev, SN9C20X_MODE_IDLE);
}

/**
 * @param file
 * @param priv
 * @param param
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_g_param(struct file *file, void *priv,
	struct v4l2_streamparm *param)
{
	struct usb_sn9c20x *dev;


	dev = video_get_drvdata(priv);

	if (param->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	param->parm.capture.capability = 0;
	param->parm.capture.capturemode = 0;
	param->parm.capture.timeperframe.numerator = 1;
	param->parm.capture.timeperframe.denominator = 30;
	param->parm.capture.readbuffers = 2;
	param->parm.capture.extendedmode = 0;

	return 0;
}

/**
 * @param file
 * @param priv
 * @param param
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_vidioc_s_param(struct file *file, void *priv,
	struct v4l2_streamparm *param)
{
	struct usb_sn9c20x *dev;

	dev = video_get_drvdata(priv);

	if (v4l_get_privileges(file))
		return -EBUSY;

	if (param->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	return 0;
}

/**
 * @param inode Inode pointer
 * @param fp File pointer
 * @param cmd Command
 * @param arg Arguements of the command
 *
 * @returns 0 if all is OK
 *
 * @brief Manage IOCTL
 *
 * This function permits to manage all the IOCTL from the application.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
static int v4l_sn9c20x_ioctl(struct inode *inode, struct file *fp,
		unsigned int cmd, unsigned long arg)
#else
static long v4l_sn9c20x_ioctl(struct file *fp,
		unsigned int cmd, unsigned long arg)
#endif
{
	int err;
	struct usb_sn9c20x *dev;
	struct video_device *vdev;

	vdev = video_devdata(fp);
	dev = video_get_drvdata(video_devdata(fp));

	UDIA_DEBUG("v4l_sn9c20x_ioctl %02X\n", (unsigned char) cmd);

	if (dev == NULL || vdev == NULL)
		return -EFAULT;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
	switch (cmd) {
	case VIDIOC_ENUM_FRAMESIZES:
	{
		struct v4l2_frmsizeenum size;
		if (copy_from_user(&size, (void __user *)arg, sizeof(size)))
			return -EFAULT;
		err = sn9c20x_vidioc_enum_framesizes(fp,
						     fp->private_data,
						     &size);
		if (copy_to_user((void __user *)arg, &size, sizeof(size)))
			return -EFAULT;
		break;
	}
	default:
		err = video_ioctl2(inode, fp, cmd, arg);
	}
#else
	err = video_ioctl2(fp, cmd, arg);
#endif

	return err;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
static const struct v4l2_ioctl_ops sn9c20x_v4l2_ioctl_ops = {
	.vidioc_querycap            = sn9c20x_vidioc_querycap,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29)
	.vidioc_enum_framesizes     = sn9c20x_vidioc_enum_framesizes,
#endif
	.vidioc_enum_fmt_vid_cap    = sn9c20x_vidioc_enum_fmt_cap,
	.vidioc_try_fmt_vid_cap     = sn9c20x_vidioc_try_fmt_cap,
	.vidioc_s_fmt_vid_cap       = sn9c20x_vidioc_s_fmt_cap,
	.vidioc_g_fmt_vid_cap       = sn9c20x_vidioc_g_fmt_cap,
	.vidioc_enum_input          = sn9c20x_vidioc_enum_input,
	.vidioc_g_input             = sn9c20x_vidioc_g_input,
	.vidioc_s_input             = sn9c20x_vidioc_s_input,
	.vidioc_streamon            = sn9c20x_vidioc_streamon,
	.vidioc_streamoff           = sn9c20x_vidioc_streamoff,
	.vidioc_queryctrl           = sn9c20x_vidioc_queryctrl,
	.vidioc_g_ctrl              = sn9c20x_vidioc_g_ctrl,
	.vidioc_s_ctrl              = sn9c20x_vidioc_s_ctrl,
	.vidioc_g_parm              = sn9c20x_vidioc_g_param,
	.vidioc_s_parm              = sn9c20x_vidioc_s_param,
	.vidioc_reqbufs             = sn9c20x_vidioc_reqbufs,
	.vidioc_qbuf                = sn9c20x_vidioc_qbuf,
	.vidioc_dqbuf               = sn9c20x_vidioc_dqbuf,
	.vidioc_querybuf            = sn9c20x_vidioc_querybuf,
};
#endif

/**
 * @param dev Device structure
 *
 * @returns 0 if all is OK
 *
 * @brief Register the video device
 *
 * This function permits to register the USB device to the video device.
 */
int v4l_sn9c20x_register_video_device(struct usb_sn9c20x *dev)
{
	int err;

	strcpy(dev->vdev->name, DRIVER_DESC);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
	dev->vdev->dev = &dev->interface->dev;
	dev->vdev->owner = THIS_MODULE;
	dev->vdev->type = VID_TYPE_CAPTURE;
	dev->vdev->current_norm = 0;
#else
	dev->vdev->dev_parent = &dev->interface->dev;
#endif
	dev->vdev->tvnorms = 0;
	dev->vdev->fops = &v4l_sn9c20x_fops;
	dev->vdev->release = video_device_release;
	dev->vdev->minor = -1;

	if (log_level & SN9C20X_DEBUG)
		dev->vdev->debug = V4L2_DEBUG_IOCTL_ARG;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
	dev->vdev->vidioc_querycap        = sn9c20x_vidioc_querycap;
	dev->vdev->vidioc_enum_fmt_cap    = sn9c20x_vidioc_enum_fmt_cap;
	dev->vdev->vidioc_try_fmt_cap     = sn9c20x_vidioc_try_fmt_cap;
	dev->vdev->vidioc_s_fmt_cap       = sn9c20x_vidioc_s_fmt_cap;
	dev->vdev->vidioc_g_fmt_cap       = sn9c20x_vidioc_g_fmt_cap;
	dev->vdev->vidioc_enum_input      = sn9c20x_vidioc_enum_input;
	dev->vdev->vidioc_g_input         = sn9c20x_vidioc_g_input;
	dev->vdev->vidioc_s_input         = sn9c20x_vidioc_s_input;
	dev->vdev->vidioc_streamon        = sn9c20x_vidioc_streamon;
	dev->vdev->vidioc_streamoff       = sn9c20x_vidioc_streamoff;
	dev->vdev->vidioc_queryctrl       = sn9c20x_vidioc_queryctrl;
	dev->vdev->vidioc_g_ctrl          = sn9c20x_vidioc_g_ctrl;
	dev->vdev->vidioc_s_ctrl          = sn9c20x_vidioc_s_ctrl;
	dev->vdev->vidioc_g_parm          = sn9c20x_vidioc_g_param;
	dev->vdev->vidioc_s_parm          = sn9c20x_vidioc_s_param;
	dev->vdev->vidioc_reqbufs         = sn9c20x_vidioc_reqbufs;
	dev->vdev->vidioc_qbuf            = sn9c20x_vidioc_qbuf;
	dev->vdev->vidioc_dqbuf           = sn9c20x_vidioc_dqbuf;
	dev->vdev->vidioc_querybuf        = sn9c20x_vidioc_querybuf;
#else
	dev->vdev->ioctl_ops              = &sn9c20x_v4l2_ioctl_ops;
#endif

	video_set_drvdata(dev->vdev, dev);

	sn9c20x_queue_init(&dev->queue);

	err = video_register_device(dev->vdev, VFL_TYPE_GRABBER, -1);

	if (err)
		UDIA_ERROR("Video register fail !\n");
	else
		UDIA_INFO("Webcam device %04X:%04X is now controlling video "
				"device /dev/video%d\n",
				le16_to_cpu(dev->udev->descriptor.idVendor),
				le16_to_cpu(dev->udev->descriptor.idProduct),
				dev->vdev->minor);

	return err;
}


/**
 * @param dev Device structure
 *
 * @returns 0 if all is OK
 *
 * @brief Unregister the video device
 *
 * This function permits to unregister the video device.
 */
int v4l_sn9c20x_unregister_video_device(struct usb_sn9c20x *dev)
{
	UDIA_INFO("SN9C20X USB 2.0 Webcam releases control of video "
			"device /dev/video%d\n", dev->vdev->minor);

	video_set_drvdata(dev->vdev, NULL);
	video_unregister_device(dev->vdev);

	return 0;
}


/**
 * @var v4l_sn9c20x_fops
 *
 * This variable contains some callback
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
static struct file_operations v4l_sn9c20x_fops = {
#else
static struct v4l2_file_operations v4l_sn9c20x_fops = {
#endif
	.owner = THIS_MODULE,
	.open = v4l_sn9c20x_open,
	.release = v4l_sn9c20x_release,
	.read = v4l_sn9c20x_read,
	.poll = v4l_sn9c20x_poll,
	.mmap = v4l_sn9c20x_mmap,
	.ioctl = v4l_sn9c20x_ioctl,
#if defined(CONFIG_COMPAT) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
	.compat_ioctl = v4l_compat_ioctl32,
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
	.llseek = no_llseek
#endif
};

