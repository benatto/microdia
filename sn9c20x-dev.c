/**
 * @file sn9c20x-dev.c
 * @author Nicolas VIVIEN
 * @date 2008-02-01
 *
 * @brief Device specific functions
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

#include <linux/errno.h>
#include <linux/string.h>
#include <stdarg.h>

#include "sn9c20x.h"
#include "sn9c20x-bridge.h"
#include "omnivision.h"
#include "micron.h"

int (*sn_probes[])(struct usb_sn9c20x *dev) = {
	mt9v011_probe,
	mt9v111_probe,
	mt9v112_probe,
};

struct sn9c20x_video_mode sn9c20x_modes[SN9C20X_N_MODES] = {
	{
		.width = 128,
		.height = 96,
		.scale = SN9C20X_1_4_SCALE,
		.window = {65, 48, 1024, 768}
	},
	{
		.width = 160,
		.height = 120,
		.scale = SN9C20X_1_4_SCALE,
		.window = {0, 0, 640, 480}
	},
	{
		.width = 176,
		.height = 144,
		.scale = SN9C20X_1_2_SCALE,
		.window = {145, 96, 704, 576}
	},
	{
		.width = 320,
		.height = 240,
		.scale = SN9C20X_1_2_SCALE,
		.window = {0, 0, 640, 480}
	},
	{
		.width = 352,
		.height = 288,
		.scale = SN9C20X_NO_SCALE,
		.window = {145, 96, 704, 576}
	},
	{
		.width = 640,
		.height = 480,
		.scale = SN9C20X_NO_SCALE,
		.window = {0, 0, 640, 480}
	},
	{
		.width = 1280,
		.height = 1024,
		.scale = SN9C20X_NO_SCALE,
		.window = {0, 0, 1280, 1024}
	},
};

struct sn9c20x_video_format sn9c20x_fmts[SN9C20X_N_FMTS] = {
	{
		.pix_fmt = V4L2_PIX_FMT_SN9C20X_I420,
		.desc = "SN9C20X I420 (YUV 4:2:0)",
		.depth = 12,
		.set_format = sn9c20x_set_yuv420,
	},
	{
		.pix_fmt = V4L2_PIX_FMT_SBGGR8,
		.desc = "Bayer 8bit (BGGR)",
		.depth = 8,
		.set_format = sn9c20x_set_raw,
	},
/*
	{
		.pix_fmt = V4L2_PIX_FMT_YUYV,
		.desc = "YUYV (YUV 4:2:0)",
		.depth = 16,
		.set_format = sn9c20x_set_yuv422,
	},
*/
	{
		.pix_fmt = V4L2_PIX_FMT_JPEG,
		.desc = "JPEG (YUV 4:2:2)",
		.depth = 16,
		.set_format = sn9c20x_set_jpeg,
	},
};

int sn9c20x_initialize_sensor(struct usb_sn9c20x *dev)
{
	int ret = 0;
	int i;

	dev->camera.min_yavg = 80;
	dev->camera.max_yavg = 130;
	dev->camera.old_step = 0;
	dev->camera.older_step = 0;
	dev->camera.exposure_step = 16;

	/* Probe sensor first if sensor set to probe*/
	if (dev->camera.sensor == PROBE_SENSOR) {
		for (i = 0; i < ARRAY_SIZE(sn_probes); i++) {
			ret = sn_probes[i](dev);
			dev->camera.sensor = ret;
			if (ret > 0)
				break;
		}
	}

	switch (dev->camera.sensor) {
	case SOI968_SENSOR:
		sn9c20x_write_i2c_array(dev, soi968_init, 0);
		dev->camera.set_sxga_mode = ov965x_set_sxga_mode;
		dev->camera.set_exposure = soi968_set_exposure;
		dev->camera.set_auto_exposure = soi968_set_autoexposure;
		dev->camera.set_gain = soi968_set_gain;
		dev->camera.set_auto_gain = ov_set_autogain;
		dev->camera.set_auto_whitebalance = soi968_set_autowhitebalance;
		dev->camera.hstart = 60;
		dev->camera.vstart = 11;
		UDIA_INFO("Detected SOI968 Sensor.\n");
		break;
	case OV9650_SENSOR:
		sn9c20x_write_i2c_array(dev, ov9650_init, 0);
		dev->camera.hstart = 1;
		dev->camera.vstart = 7;
		dev->camera.set_sxga_mode = ov965x_set_sxga_mode;
		dev->camera.set_hvflip = ov965x_set_hvflip;
		dev->camera.set_exposure = ov_set_exposure;
		dev->camera.set_auto_gain = ov_set_autogain;
		dev->camera.set_gain = ov9650_set_gain;
		dev->camera.flip_detect = ov965x_flip_detect;
		UDIA_INFO("Detected OV9650 Sensor.\n");
		break;
	case OV9655_SENSOR:
		sn9c20x_write_i2c_array(dev, ov9655_init, 0);
		dev->camera.set_sxga_mode = ov965x_set_sxga_mode;
		dev->camera.set_exposure = ov_set_exposure;
		dev->camera.set_auto_gain = ov_set_autogain;
		dev->camera.hstart = 0;
		dev->camera.vstart = 7;
		UDIA_INFO("Detected OV9655 Sensor.\n");
		break;
	case OV7670_SENSOR:
		sn9c20x_write_i2c_array(dev, ov7670_init, 0);
		dev->camera.set_exposure = ov_set_exposure;
		dev->camera.set_auto_gain = ov_set_autogain;
		dev->camera.flip_detect = ov7670_flip_detect;
		dev->camera.hstart = 0;
		dev->camera.vstart = 1;
		UDIA_INFO("Detected OV7670 Sensor.\n");
		break;
	case OV7660_SENSOR:
		sn9c20x_write_i2c_array(dev, ov7660_init, 0);
		dev->camera.set_exposure = ov_set_exposure;
		dev->camera.set_auto_gain = ov_set_autogain;
		dev->camera.set_gain = ov9650_set_gain;
		dev->camera.hstart = 1;
		dev->camera.vstart = 1;
		UDIA_INFO("Detected OV7660 Sensor.\n");
		break;
	case MT9V111_SENSOR:
		sn9c20x_write_i2c_array(dev, mt9v111_init, 1);
		dev->camera.set_hvflip = mt9v111_set_hvflip;
		dev->camera.set_exposure = mt9v111_set_exposure;
		dev->camera.set_auto_exposure = mt9v111_set_autoexposure;
		dev->camera.set_auto_whitebalance = mt9v111_set_autowhitebalance;
		dev->camera.hstart = 2;
		dev->camera.vstart = 2;
		UDIA_INFO("Detected MT9V111 Sensor.\n");
		break;
	case MT9V112_SENSOR:
		sn9c20x_write_i2c_array(dev, mt9v112_init, 1);
		dev->camera.set_hvflip = mt9v112_set_hvflip;
		dev->camera.hstart = 6;
		dev->camera.vstart = 2;
		UDIA_INFO("Detected MT9V112 Sensor.\n");
		break;
	case MT9M111_SENSOR:
		sn9c20x_write_i2c_array(dev, mt9m111_init, 1);
		dev->camera.set_exposure = mt9m111_set_exposure;
		dev->camera.set_auto_exposure = mt9m111_set_autoexposure;
		dev->camera.set_auto_whitebalance = mt9m111_set_autowhitebalance;
		dev->camera.hstart = 0;
		dev->camera.vstart = 2;
		UDIA_INFO("Detected MT9M111 Sensor.\n");
		break;
	case MT9V011_SENSOR:
		sn9c20x_write_i2c_array(dev, mt9v011_init, 1);
		dev->camera.set_hvflip = mt9v011_set_hvflip;
		dev->camera.set_exposure = mt9v011_set_exposure;
		dev->camera.hstart = 2;
		dev->camera.vstart = 2;
		UDIA_INFO("Detected MT9V011 Sensor.\n");
		break;
	case MT9M001_SENSOR:
		sn9c20x_write_i2c_array(dev, mt9m001_init, 1);
		dev->camera.hstart = 2;
		dev->camera.vstart = 2;
		UDIA_INFO("Detected MT9M001 Sensor.\n");
		break;
	case HV7131R_SENSOR:
		dev->camera.i2c_flags |= SN9C20X_I2C_400KHZ;
		sn9c20x_write_i2c_array(dev, hv7131r_init, 0);
		dev->camera.set_hvflip = hv7131r_set_hvflip;
		dev->camera.set_gain = hv7131r_set_gain;
		dev->camera.set_exposure = hv7131r_set_exposure;
		dev->camera.hstart = 0;
		dev->camera.vstart = 1;
		UDIA_INFO("Detected HV7131R Sensor.\n");
		break;
	default:
		ret = -EINVAL;
		UDIA_INFO("Unsupported sensor.\n");
	}
	return ret;
}


/**
 * @brief Wrapper function to detect hardware states
 *
 * @param dev Pointer to device structure
 *
 * @returns 0
 */
int dev_sn9c20x_call_constantly(struct usb_sn9c20x *dev)
{

	/* Know to be broken, temporarely disabled */
	/*dev_sn9c20x_flip_detection(dev);*/
	if (!dev->camera.set_auto_exposure &&
		dev->vsettings.auto_exposure) {
		dev_sn9c20x_perform_soft_ae(dev);
	}

	return 0;
}

/**
 * @brief Wrapper function to detect a flipped sensor
 *
 * @param dev Pointer to device structure
 *
 * @returns 0 or negative error value
 *
 */
int dev_sn9c20x_flip_detection(struct usb_sn9c20x *dev)
{
	int ret = -ENODEV;
	if (dev && dev->camera.flip_detect)
		ret = dev->camera.flip_detect(dev);
	return ret;
}

/**
 * @brief Perform software autoexposure
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * @author Stefan Krastanov
 */
int dev_sn9c20x_perform_soft_ae(struct usb_sn9c20x *dev)
{
	int yavg, new_exp;
	yavg = -1;

	if (!dev->camera.set_exposure)
		return -1;
	yavg = atomic_read(&dev->camera.yavg);

	UDIA_DEBUG("Sensor YAVG: %d\n", yavg);
	if (yavg < 0) {
		/* Can't get YAVG - we have nothing to do */
		return -1;
	}

	/*some hardcoded values are present
	 *like those for maximal/minimal exposure
	 *and exposure steps*/

	/* image too dark */
	if (yavg < dev->camera.min_yavg) {
		/*worst case - exposure is allready maximal*/
		if (dev->vsettings.exposure > 0xf5)
			return 0;
		/*change exposure just a bit*/
		new_exp = dev->vsettings.exposure + dev->camera.exposure_step;
		if (new_exp > 0xff)
			new_exp = 0xff;
		if (new_exp < 0x1)
			new_exp = 1;
		/*set it*/
		dev->vsettings.exposure = new_exp;
		dev->camera.set_exposure(dev);
		/*note the direction of the change*/
		dev->camera.older_step = dev->camera.old_step;
		dev->camera.old_step = 1; /*it's going up*/

		/*check if it's oscillating and take smaller step if it is*/
		if (dev->camera.old_step ^ dev->camera.older_step)
			dev->camera.exposure_step /= 2;
			/* oscilating - divide the step by 2 */
		else
			dev->camera.exposure_step += 2;
			/* same direction - step must be bigger */
	}
	/*image too light*/
	if (yavg > dev->camera.max_yavg) {
		/*worst case - exposure is allready minimal*/
		if (dev->vsettings.exposure < 0x10)
			return 0;
		/*change exposure just a bit*/
		new_exp = dev->vsettings.exposure - dev->camera.exposure_step;
		if (new_exp > 0xff)
			new_exp = 0xff;
		if (new_exp < 0x1)
			new_exp = 1;
		/*set it*/
		dev->vsettings.exposure = new_exp;
		dev->camera.set_exposure(dev);
		/*note the direction of the change*/
		dev->camera.older_step = dev->camera.old_step;
		dev->camera.old_step = 0; /*it's going down*/

		/*check if it's oscillating and take smaller step if it is*/
		if (dev->camera.old_step ^ dev->camera.older_step)
			dev->camera.exposure_step /= 2;
			/* oscilating - divide the step by 2 */
		else
			dev->camera.exposure_step += 2;
			/* same direction - step must be bigger */
	}

	return 0;
}

