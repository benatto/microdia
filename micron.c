/**
 * @file micron.c
 * @author Comer352l
 * @date 2008-04-25
 *
 * @brief Common functions and data for Micron sensors.
 *
 * @note Copyright (C) Comer352l
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

#include <linux/delay.h>
#include "sn9c20x.h"
#include "sn9c20x-bridge.h"
#include "micron.h"

struct sn9c20x_i2c_regs mt9v112_init[] = {
	{0x0d, 0x0021}, {0x0d, 0x0020}, {0xf0, 0x0000},
	{0x34, 0xc019}, {0x0a, 0x0011}, {0x0b, 0x000b},
	{0x20, 0x0703}, {0x35, 0x2022}, {0xf0, 0x0001},
	{0x05, 0x0000}, {0x06, 0x340c}, {0x3b, 0x042a},
	{0x3c, 0x0400}, {0xf0, 0x0002}, {0x2e, 0x0c58},
	{0x5b, 0x0001}, {0xc8, 0x9f0b}, {0xf0, 0x0001},
	{0x9b, 0x5300}, {0xf0, 0x0000}, {0x2b, 0x0020},
	{0x2c, 0x002a}, {0x2d, 0x0032}, {0x2e, 0x0020},
	{0x09, 0x01dc}, {0x01, 0x000c}, {0x02, 0x0020},
	{0x03, 0x01e0}, {0x04, 0x0280}, {0x06, 0x000c},
	{0x05, 0x0098}, {0x20, 0x0703}, {0x09, 0x01f2},
	{0x2b, 0x00a0}, {0x2c, 0x00a0}, {0x2d, 0x00a0},
	{0x2e, 0x00a0}, {0x01, 0x000c}, {0x02, 0x0020},
	{0x03, 0x01e0}, {0x04, 0x0280}, {0x06, 0x000c},
	{0x05, 0x0098}, {0x09, 0x01c1}, {0x2b, 0x00ae},
	{0x2c, 0x00ae}, {0x2d, 0x00ae}, {0x2e, 0x00ae},
	{0xff, 0xffff}
};

struct sn9c20x_i2c_regs mt9v111_init[] = {
	{0x01, 0x0004},	/* select sensor address space */
	{0x0d, 0x0001}, /* Resets chip */
	{0x0d, 0x0000},
	{0x01, 0x0001},	/* select IFP address space */
	{0x02, 0x0016},	/* ??? */
		/* 0x03: Color Correction Register 3 */
		/* 0x04: Color Correction Register 4 */
	{0x03, 0x01e1},	/* ??? */
	{0x04, 0x0281},	/* ??? */
		/* 0x05: Aperture Correction (Sharpening) */
	{0x05, 0x0004},	/* 100% sharpening,
		no automatic sharpness reduction at low light: no sharpening */
		/* 0x07: IFP Soft Reset */
		/* 0x08: Output Format Control */
	{0x07, 0x3002},	/* reset */
	{0x21, 0x0000},
	{0x25, 0x4024},
	{0x26, 0xff03},
	{0x27, 0xff10},
	{0x2b, 0x7828},
	{0x2c, 0xb43c},
	{0x2d, 0xf0a0},
	{0x2e, 0x0c64},
	{0x2f, 0x0064},
	{0x67, 0x4010},
	{0x06, 0x301c},	/* stop AWB at the current values,
		no auto exposure */
	{0x08, 0x0480},	/* bypass entire image processing,
				raw 8+2 Bayer data output directly */
	{0x01, 0x0004},	/* select sensor address space */
	{0x02, 0x0016},	/* start with column 22 */
		/* 0x03: Window Height */
		/* 0x04: Window Width */
	{0x03, 0x01e6},	/* 486 */
	{0x04, 0x0286},	/* 646 */
		/* 0x05: Horizontal Blanking */
		/* 0x06: Vertical Blanking */
	{0x05, 0x0004},	/* 4 columns (pixel clocks) */
	{0x06, 0x0000},	/* 0 rows */
		/* 0x07: Output Control */
		/* 0x08: Row Start */
	{0x07, 0x3002},	/* normal operation + chip enable*/
	{0x08, 0x0008},	/* row 8 */
		/* 0x0c: Shutter Delay */
		/* 0x0d: Reset (Soft) */
	{0x0c, 0x0000},	/* 0 master clocks */
	{0x0d, 0x0000},	/* return to normal operation */
	{0x0e, 0x0000},
	{0x0f, 0x0000},
	{0x10, 0x0000},
	{0x11, 0x0000},
		/* 0x12: 2X Zoom Col Start
			=> 0x1e bit 0 must be set to activate zoom */
		/* 0x13: 2X Zoom Row Start
			=> 0x1e bit 0 must be set to activate zoom */
	{0x12, 0x00b0},	/* column 176 */
	{0x13, 0x007c},	/* row 124 */
	{0x14, 0x0000},
	{0x15, 0x0000},
	{0x16, 0x0000},
	{0x17, 0x0000},
	{0x18, 0x0000},
	{0x19, 0x0000},
	{0x1a, 0x0000},
	{0x1b, 0x0000},
	{0x1c, 0x0000},
	{0x1d, 0x0000},
	{0x30, 0x0000},
	{0x30, 0x0005},
	{0x31, 0x0000},
		/* 0x02: Column Start */
		/* 0x03: Window Height */
	{0x02, 0x0016},	/* coulmn 22 */
	{0x03, 0x01e1},	/* 481 */
		/* 0x04: Window Width */
		/* 0x05: Horizontal Blanking */
	{0x04, 0x0281},	/* 641 */
	{0x05, 0x0004},	/* 4 columns (pixel clocks) */
		/* 0x06: Vertical Blanking */
		/* 0x07: Output Control */
	{0x06, 0x0000},	/* 0 rows */
	{0x07, 0x3002},	/* normal operation + chip enable */
		/* 0x06: Vertical Blanking */
	{0x06, 0x002d},	/* 45 rows */
		/* 0x05: Horizontal Blanking */
	{0x05, 0x0004},	/* 4 columns (pixel clocks) */
		/* 0x09: Shutter Width */
	{0x09, 0x0064},	/* integration of 100 rows */
		/* 0x2b: Green 1 Gain */
		/* 0x2c: Blue Gain */
	{0x2b, 0x00a0},	/* 32*0.03125*2 = 2 */
	{0x2c, 0x00a0},	/* 32*0.03125*2 = 2 */
		/* 0x2d: Red Gain */
		/* 0x2e: Green 2 Gain */
	{0x2d, 0x00a0},	/* 32*0.03125*2 = 2 */
	{0x2e, 0x00a0},	/* 32*0.03125*2 = 2 */
		/* 0x02: Column Start */
		/* 0x03: Window Hight */
	{0x02, 0x0016},	/* coulmn 22 */
	{0x03, 0x01e1},	/* 481 */
		/* 0x04: Window Width */
		/* 0x05: Horizontal Blanking */
	{0x04, 0x0281},	/* 641 */
	{0x05, 0x0004},	/* 4 columns (pixel clocks) */
		/* 0x06: Vertical Blanking */
		/* 0x07: Output Control */
	{0x06, 0x002d},	/* 45 rows */
	{0x07, 0x3002},	/* RESERVED options */
		/* Writes to0x0e: UNDOCUMENTED */
	{0x0e, 0x0008},
		/* 0x06: Vertical Blanking */
	{0x06, 0x002d},	/* 45 rows */
		/* 0x05: Horizontal Blanking */
	{0x05, 0x0004},	/* 4 columns (pixel clocks) */
	{0xff, 0xffff},
};

struct sn9c20x_i2c_regs mt9v011_init[] = {
	/* 0x07: Output Control */
	{0x07, 0x0002},	/*  chip enable, normal operation */
	/* 0x0d: Soft Reset */
	{0x0d, 0x0001},	/*  reset */
	/* 0x0d: Soft Reset */
	{0x0d, 0x0000},	/*  resume operation */
	/* 0x01: Row start */
	/* 0x02: Column Start */
	{0x01, 0x0008},	/*  start with row 8 */
	{0x02, 0x0016},	/*  start with column 22 */
	/* 0x03: Window Height */
	/* 0x04: Window Width */
	{0x03, 0x01e1},	/*  481 */
	{0x04, 0x0281},	/*  641 */
	/* 0x05: Horizontal Blanking */
	/* 0x06: Vertical Blanking */
	{0x05, 0x0083},	/*  131 columns (pixel clocks) */
	{0x06, 0x0006},	/*  6 rows */
	/* 0x0d: Soft Reset */
	{0x0d, 0x0002},	/*  UNKNOWN */
	/* 0x0a: Pixel Clock Speed */
	/* 0x0b: Frame Restart */
	{0x0a, 0x0000},	/*  default */
	{0x0b, 0x0000},	/*  (has no effect/no restart) */
	/* 0x0c: Shutter Delay */
	/* 0x0d: Soft Reset */
	{0x0c, 0x0000},	/*  0 master clocks */
	{0x0d, 0x0000},	/*  resume operation */
	{0x0e, 0x0000},
	{0x0f, 0x0000},
	{0x10, 0x0000},
	{0x11, 0x0000},
	/* 0x12: 2X Zoom Column Start (from MT9V111 datasheet) */
	/* 0x13: 2X Zoom Row Start (from MT9V111 datasheet) */
	{0x12, 0x0000},
	/*  column 0 => bit0 of reg 0x1e must be set to activate zoom */
	{0x13, 0x0000},
	/*  row 0    => bit0 of reg 0x1e must be set to activate zoom */
	{0x14, 0x0000},
	{0x15, 0x0000},
	{0x16, 0x0000},
	{0x17, 0x0000},
	{0x18, 0x0000},
	{0x19, 0x0000},
	{0x1a, 0x0000},
	{0x1b, 0x0000},
	{0x1c, 0x0000},
	{0x1d, 0x0000},
	{0x32, 0x0000},
	/* 0x20: Read Mode */
	{0x20, 0x1101},	/*  output all frames (including bad frames) */
	{0x21, 0x0000},
	{0x22, 0x0000},
	{0x23, 0x0000},
	{0x24, 0x0000},
	{0x25, 0x0000},
	{0x26, 0x0000},
	{0x27, 0x0024},
	{0x2f, 0xf7b0},
	{0x30, 0x0005},
	{0x31, 0x0000},
	{0x32, 0x0000},
	{0x33, 0x0000},
	{0x34, 0x0100},
	{0x3d, 0x068f},
	{0x40, 0x01e0},
	{0x41, 0x00d1},
	{0x44, 0x0082},
	{0x5a, 0x0000},
	{0x5b, 0x0000},
	{0x5c, 0x0000},
	{0x5d, 0x0000},
	{0x5e, 0x0000},
	{0x5f, 0xa31d},
	{0x62, 0x0611},
	/* 0x0a: Pixel Clock Speed */
	{0x0a, 0x0000},	/*  default */
	/* 0x06: Vertical Blanking */
	{0x06, 0x0029},	/*  41 rows */
	/* 0x05: Horizontal Blanking */
	{0x05, 0x0009},	/*  9 columns (pixel clocks) */
	/* 0x20: Read Mode */
	{0x20, 0x1101},	/*  output all frames (including bad ones) */
	/* 0x20: Read Mode */
	{0x20, 0x1101},	/*  output all frames (including bad ones) */
	/* 0x09: Shutter Width */
	{0x09, 0x0064},	/*  integration of 100 rows */
	/* 0x07: Output Control */
	{0x07, 0x0003},
	/*  dont update changes until bit0=0, chip enable, normal operation */
	/* 0x2b: Green 1 Gain */
	/* 0x2c: Blue Gain */
	{0x2b, 0x0033},	/*  51*0.03125*1 = 1.59375 */
	{0x2c, 0x00a0},	/*  32*0.03125*2 = 2 */
	/* 0x2d: Red Gain */
	/* 0x2e: Green 2 Gain */
	{0x2d, 0x00a0},	/*  32*0.03125*2 = 2 */
	{0x2e, 0x0033},	/*  51*0.03125*1 = 1.59375 */
	/* 0x07: Output Control */
	{0x07, 0x0002},	/*  chip enable, normal operation */
	/* 0x0a: Pixel Clock Speed */
	{0x06, 0x0000},	/*  default */
	/* 0x06: Vertical Blanking */
	{0x06, 0x0029},	/*  41 rows */
	/* 0x05: Horizontal Blanking */
	{0x05, 0x0009},	/*  9 columns (pixel clocks) */
	{0xff, 0xffff},
};

struct sn9c20x_i2c_regs mt9m001_init[] = {
	{0x07, 0x0000},
	{0x07, 0x0002},
	{0x0d, 0x0001},
	{0x0d, 0x0000},
	{0x01, 0x000e},
	{0x02, 0x0014},
	{0x03, 0x03c1},
	{0x04, 0x0501},
	{0x05, 0x0083},
	{0x06, 0x0006},
	{0x0d, 0x0002},
	{0x09, 0x0000},
	{0x0a, 0x0000},
	{0x0b, 0x0000},
	{0x0c, 0x0000},
	{0x11, 0x0000},
	{0x1e, 0x8000},
	{0x20, 0x1105},
	{0x2b, 0x0008},
	{0x2c, 0x0010},
	{0x2d, 0x0014},
	{0x2e, 0x0008},
	{0x5f, 0x8904},
	{0x60, 0x0000},
	{0x61, 0x0000},
	{0x62, 0x0498},
	{0x63, 0x0000},
	{0x68, 0x0000},
	{0x20, 0x111d},
	{0x06, 0x00f2},
	{0x05, 0x0013},
	{0x20, 0x511d},
	{0x20, 0xd11d},
	{0x09, 0x03e8},
	{0x07, 0x0003},
	{0x2b, 0x0010},
	{0x2c, 0x0010},
	{0x2d, 0x0010},
	{0x2e, 0x0010},
	{0x07, 0x0002},
	{0x06, 0x00f2},
	{0x05, 0x0013},
	{0x09, 0x043c},
	{0x07, 0x0003},
	{0x2b, 0x0023},
	{0x2c, 0x003f},
	{0x2d, 0x003a},
	{0x2e, 0x0023},
	{0x07, 0x0002},
	{0x09, 0x0879},
	{0x07, 0x0003},
	{0x2b, 0x0027},
	{0x2c, 0x003f},
	{0x2d, 0x0039},
	{0x2e, 0x0027},
	{0x07, 0x0002},
	{0x09, 0x0b4c},
	{0x07, 0x0003},
	{0x2b, 0x0029},
	{0x2c, 0x003f},
	{0x2d, 0x0035},
	{0x2e, 0x0029},
	{0x07, 0x0002},
	{0x09, 0x0e1f},
	{0x07, 0x0003},
	{0x2d, 0x0030},
	{0x07, 0x0002},
	{0x09, 0x103e},
	{0x07, 0x0003},
	{0x2d, 0x002b},
	{0x07, 0x0002},
	{0x09, 0x10f2},
	{0x07, 0x0003},
	{0x2b, 0x002a},
	{0x2c, 0x003f},
	{0x2d, 0x002a},
	{0x2c, 0x002a},
	{0x07, 0x0002},
	{0x07, 0x0003},
	{0x07, 0x0002},
	{0x07, 0x0003},
	{0x07, 0x0002},
	{0x07, 0x0003},
	{0x07, 0x0002},
	{0x07, 0x0000},
};

struct sn9c20x_i2c_regs mt9m111_init[] = {
	{0xf0, 0x0000},
	/* Reset sensor */
	{0x0d, 0x0008}, {0x0d, 0x0009}, {0x0d, 0x0008},
	/* Select Page map 0x01
	 * This means  all new writes now have the address prefix 0x1.
	 * Example: 0x3a becomes 0x13a. */
	{0xf0, 0x0001},
	/* change pixel format of both contexts */
	{0x3a, 0x4300}, {0x9b, 0x4300},
	/* setup AE, AWB, etc */
	{0x06, 0x708e},
	/* Switch back to Page map 0x00 */
	{0xf0, 0x0000},
	{0xff, 0xffff},
};

int mt9m111_set_exposure(struct usb_sn9c20x *dev)
{
	int ret;
	__u16 exposure, page;

	exposure = dev->vsettings.exposure << 8;

	ret = 0;
	page = 0;
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0xF0, &page);
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0x09, &exposure);

	return ret;
}

int mt9m111_set_autoexposure(struct usb_sn9c20x *dev)
{
	int ret;
	__u16 data, page;

	ret = 0;
	page = 1;
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0xF0, &page);
	ret |= sn9c20x_read_i2c_data16(dev, 1, 0x06, &data);


	if (dev->vsettings.auto_exposure == 1) {
		data |= 0x7000;
	} else if (dev->vsettings.auto_exposure == 0) {
		data &= ~0x7000;
	} else
		return -EINVAL;

	ret |= sn9c20x_write_i2c_data16(dev, 1, 0x06, &data);
	page = 0;
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0xF0, &page);

	if (ret < 0) {
		UDIA_ERROR("Error: setting of auto exposure failed: "
			"error while writing to IFP-register 0x06\n");
		return ret;
	}
	return 0;
}

int mt9m111_set_autowhitebalance(struct usb_sn9c20x *dev)
{
	int ret;
	__u16 data, page;

	ret = 0;
	page = 1;
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0xF0, &page);
	ret |= sn9c20x_read_i2c_data16(dev, 1, 0x06, &data);

	if (dev->vsettings.auto_whitebalance == 1) {
		data |= 0x0002;
	} else if (dev->vsettings.auto_whitebalance == 0) {
		data &= ~0x0002;
	}

	ret |= sn9c20x_write_i2c_data16(dev, 1, 0x06, &data);
	page = 0;
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0xF0, &page);

	return ret;
}

int mt9v111_select_address_space(struct usb_sn9c20x *dev, __u8 address_space)
{
	__u8 buf[2];
	int retI2C;
	int k;

	/* check if selection is valid: */
	if ((address_space != MT9V111_ADDRESSSPACE_IFP) &&
		(address_space != MT9V111_ADDRESSSPACE_SENSOR)) {
			UDIA_WARNING("invalid register address space "
				"selection for sensor MT9V111/MI0360SOC !\n");
			return -1;
	}
	/* read address space slection register: */
	k = 0;
	retI2C = -1;
	while ((k < 3) && (retI2C != 0)) {
		retI2C = sn9c20x_read_i2c_data(dev, 2, 0x01, buf);
		if (retI2C != 0 && k < 2)
			udelay(1000);
		k++;
	}
	if (retI2C != 0) {
		UDIA_ERROR("MT9V111/MI0360SOC (I2C-slave 0x5c): "
			"read of reg0x01 (address space selection) failed !\n");
		return -1;
	}
	/* check current address space: */
	if ((buf[0] != 0x00) || (buf[1] != address_space)) {
		k = 0;
		retI2C = -1;
		while ((k < 3) && (retI2C != 0)) {
			/* switch address space: */
			buf[0] = 0x00; buf[1] = address_space;
			retI2C = sn9c20x_write_i2c_data(dev, 2, 0x01, buf);
			if (retI2C != 0 && k < 2)
				udelay(1000);
			k++;
		}
		if (retI2C != 0) {
			if (address_space == MT9V111_ADDRESSSPACE_IFP)
				UDIA_ERROR("MT9V111/MI0360SOC "
					"(I2C-slave 0x5c): switch to IFP "
					"address space failed !\n");
			else
				UDIA_ERROR("MT9V111/MI0360SOC "
					"(I2C-slave 0x5c): switch to sensor "
					"core address space failed !\n");
			return -1;
		}
	}
	return 0;
}

int mt9v011_probe(struct usb_sn9c20x *dev)
{
	__u16 buf;
	int ret = -EINVAL;

	dev->camera.address = 0x5d;
	ret = sn9c20x_read_i2c_data16(dev, 1, 0xff, &buf);
	if ((ret == 0) && (buf == 0x8243))
		ret = MT9V011_SENSOR;

	return ret;
}

int mt9v111_probe(struct usb_sn9c20x *dev)
{
	__u16 buf;
	int ret = -EINVAL;

	dev->camera.address = 0x5c;
	/* Select address-space: sensor */
	buf = 0x04;
	ret = sn9c20x_write_i2c_data16(dev, 1, 0x01, &buf);
	if (ret != 0)
		return ret;

	ret = sn9c20x_read_i2c_data16(dev, 1, 0xff, &buf);
	if ((ret == 0) && (buf == 0x823a))
		ret = MT9V111_SENSOR;

	return ret;
}

int mt9v112_probe(struct usb_sn9c20x *dev)
{
	int ret;
	__u16 buf;
	dev->camera.address = 0x5d;
	/* Select address-space: sensor */
	buf = 0x00;
	ret = sn9c20x_write_i2c_data16(dev, 1, 0xf0, &buf);
	if (ret < 0) {
		dev->camera.address = 0x48;
		ret = sn9c20x_write_i2c_data16(dev, 1, 0xf0, &buf);
		if (ret < 0)
			return ret;
	}


	ret = sn9c20x_read_i2c_data16(dev, 1, 0x00, &buf);
	if ((ret == 0) && (buf == 0x1229))
		return MT9V112_SENSOR;

	return -EINVAL;
}

int mt9v011_set_exposure(struct usb_sn9c20x *dev)
{
	int ret = 0;
	__u16 exp;

	exp = (dev->vsettings.exposure << 4);
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0x09, &exp);
	/* Maybe we have to disable AE/AWB/flicker avoidence (currently not
	 * used) for MT9V111 sensor, because IFP controls this register if
	 * one of them is enabled. */
	return ret;
}

int mt9v111_set_exposure(struct usb_sn9c20x *dev)
{
	int ret = 0;
	__u16 exp;


	ret = mt9v111_select_address_space(dev, MT9V111_ADDRESSSPACE_SENSOR);

	if (ret < 0)
		return -11;	/* -EAGAIN */

	exp = (dev->vsettings.exposure << 4);
	ret |= sn9c20x_write_i2c_data16(dev, 1, 0x09, &exp);
	/* Maybe we have to disable AE/AWB/flicker avoidence (currently not
	 * used) for MT9V111 sensor, because IFP controls this register if
	 * one of them is enabled. */
	return ret;
}

int mt9v011_set_hvflip(struct usb_sn9c20x *dev)
{
	int ret = 0;
	__u8 buf[2];

	if ((dev->vsettings.hflip > 1) || (dev->vsettings.hflip < 0))
		return -EINVAL;
	if ((dev->vsettings.vflip > 1) || (dev->vsettings.vflip < 0))
		return -EINVAL;

	ret = sn9c20x_read_i2c_data(dev, 2, 0x20, buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.hflip) {
		buf[0] |= 0x80;
		/* (MSB) set bit 15: read out from
		 * bottom to top (upside down) */
		buf[1] |= 0x80;
		/* (LSB) set bit 7: readout starting 1 row later */
	} else {
		buf[0] &= 0x7f;	/* (MSB) unset bit 15: normal readout */
		buf[1] &= 0x7f;	/* (LSB) unset bit 7: normal readout */
	}
	if (dev->vsettings.vflip) {
		buf[0] |= 0x40;
		/* (MSB) set bit 14: read out from right to left (mirrored) */
		buf[1] |= 0x20;
		/* (LSB) set bit 5: readout starting 1 column later */
	} else {
		buf[0] &= 0xbf;	/* (MSB) unset bit 14: normal readout */
		buf[1] &= 0xdf;	/* (LSB) unset bit 5: normal readout */
	}

	ret = sn9c20x_write_i2c_data(dev, 2, 0x20, buf);
	return ret;
}

int mt9v111_set_hvflip(struct usb_sn9c20x *dev)
{
	int ret = 0;
	__u8 buf[2];

	if ((dev->vsettings.hflip > 1) || (dev->vsettings.hflip < 0))
		return -EINVAL;
	if ((dev->vsettings.vflip > 1) || (dev->vsettings.vflip < 0))
		return -EINVAL;


	ret = mt9v111_select_address_space(dev, MT9V111_ADDRESSSPACE_SENSOR);
	if (ret < 0)
		return -11;	/* -EAGAIN */

	ret = sn9c20x_read_i2c_data(dev, 2, 0x20, buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.hflip) {
		buf[0] |= 0x80;
		/* (MSB) set bit 15: read out from
		 * bottom to top (upside down) */
		buf[1] |= 0x80;
		/* (LSB) set bit 7: readout starting 1 row later */
	} else {
		buf[0] &= 0x7f;	/* (MSB) unset bit 15: normal readout */
		buf[1] &= 0x7f;	/* (LSB) unset bit 7: normal readout */
	}
	if (dev->vsettings.vflip) {
		buf[0] |= 0x40;
		/* (MSB) set bit 14: read out from right to left (mirrored) */
		buf[1] |= 0x20;
		/* (LSB) set bit 5: readout starting 1 column later */
	} else {
		buf[0] &= 0xbf;	/* (MSB) unset bit 14: normal readout */
		buf[1] &= 0xdf;	/* (LSB) unset bit 5: normal readout */
	}

	ret = sn9c20x_write_i2c_data(dev, 2, 0x20, buf);
	return ret;
}

int mt9v111_set_autoexposure(struct usb_sn9c20x *dev)
{
	__u16 buf[1];
	int ret = 0;

	/* Switch to IFP-register address space: */
	ret = mt9v111_select_address_space(dev, MT9V111_ADDRESSSPACE_IFP);
	if (ret < 0)
		return -11;	/* -EAGAIN */
	/* Read current value of IFP-register 0x06: */
	ret = sn9c20x_read_i2c_data16(dev, 1, MT9V111_IFPREG_OPMODECTL, buf);
	if (ret < 0) {
		UDIA_ERROR("Error: setting of auto exposure failed: "
			"error while reading from IFP-register 0x06\n");
		return ret;
	}

	/* Set new value for register 0x06: */

	if (dev->vsettings.auto_exposure) {
		buf[0] |= MT9V111_IFP_OPMODE_AUTOEXPOSURE;
	} else {
		buf[0] &= ~MT9V111_IFP_OPMODE_AUTOEXPOSURE;
	}

	/* Write new value to IFP-register 0x06: */
	ret = sn9c20x_write_i2c_data16(dev, 1, MT9V111_IFPREG_OPMODECTL, buf);
	if (ret < 0) {
		UDIA_ERROR("Error: setting of auto exposure failed: "
			"error while writing to IFP-register 0x06\n");
		return ret;
	}
	return 0;
}

int mt9v111_set_autowhitebalance(struct usb_sn9c20x *dev)
{
	__u16 buf[1];
	int ret = 0;

	/* Switch to IFP-register address space: */
	ret = mt9v111_select_address_space(dev, MT9V111_ADDRESSSPACE_IFP);
	if (ret < 0)
		return -11;	/* -EAGAIN */
	/* Read current value of IFP-register 0x06: */
	ret = sn9c20x_read_i2c_data16(dev, 1,
				MT9V111_IFPREG_OPMODECTL, buf);
	if (ret < 0) {
		UDIA_ERROR("Error: setting of auto whitebalance failed: "
			"error while reading from IFP-register 0x06\n");
		return ret;
	}
	/* Set new value for register 0x06: */
	if (dev->vsettings.auto_whitebalance == 1) {
		/* Enable automatic exposure: */
		buf[0] |= MT9V111_IFP_OPMODE_AUTOWHITEBALANCE;
	} else if (dev->vsettings.auto_whitebalance == 0) {
		/* Disable automatic exposure: */
		buf[0] &= ~MT9V111_IFP_OPMODE_AUTOWHITEBALANCE;
	} else
		return -EINVAL;
	/* Write new value to IFP-register 0x06:*/
	ret = sn9c20x_write_i2c_data16(dev, 1,
					MT9V111_IFPREG_OPMODECTL, buf);
	if (ret < 0) {
		UDIA_ERROR("Error: setting of auto whitebalance failed: "
			"error while writing to IFP-register 0x06\n");
		return ret;
	}
	return 0;
}

int mt9v112_set_hvflip(struct usb_sn9c20x *dev)
{
	int ret;
	__u16 buf;

	/* Select address-space: sensor */
	buf = 0x00;
	sn9c20x_write_i2c_data16(dev, 1, 0xf0, &buf);

	ret = sn9c20x_read_i2c_data16(dev, 1, 0x20, &buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.hflip)
		buf |= 0x0002;
	else
		buf &= ~0x0002;

	if (dev->vsettings.vflip)
		buf |= 0x0001;
	else
		buf &= ~0x0001;

	ret = sn9c20x_write_i2c_data16(dev, 1, 0x20, &buf);

	return ret;
}
