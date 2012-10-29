/**
 * @file hv7131r.c
 * @author Josua Grawitter
 * @date 2008-12-17
 *
 * @brief Common control functions the HV7131R Image Sensor
 *
 * @note Copyright (C) Josua Grawitter
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

#include "sn9c20x.h"
#include "sn9c20x-bridge.h"

struct sn9c20x_i2c_regs hv7131r_init[] = {
	{0x02, 0x08},
	{0x02, 0x00},
	{0x01, 0x08},
	{0x02, 0x00},
	{0x20, 0x00},
	{0x21, 0xd0},
	{0x22, 0x00},
	{0x23, 0x09},
	{0x01, 0x08},
	{0x01, 0x08},
	{0x01, 0x08},
	{0x25, 0x07},
	{0x26, 0xc3},
	{0x27, 0x50},
	{0x30, 0x62},
	{0x31, 0x10},
	{0x32, 0x06},
	{0x33, 0x10},
	{0x20, 0x00},
	{0x21, 0xd0},
	{0x22, 0x00},
	{0x23, 0x09},
	{0x01, 0x08},
	{0xff, 0xff},
};


int hv7131r_set_exposure(struct usb_sn9c20x *dev)
{
	__u8 buf[3];
	int ret;
	__u32 val;

	val = ((dev->vsettings.exposure << 8) * 0xffffff) / 0xffff;

	buf[0] = (val >> 16) & 0xff;
	buf[1] = (val >> 8) & 0xff;
	buf[2] = val & 0xff;

	ret = sn9c20x_write_i2c_data_ext(dev, 3, 0x25, buf, 0x1e);

	return ret;
}

int hv7131r_set_gain(struct usb_sn9c20x *dev)
{
	__u8 buf;
	int ret;

	buf = dev->vsettings.gain;

	ret = sn9c20x_write_i2c_data_ext(dev, 1, 0x30, &buf, 0x1d);

	return ret;
}

int hv7131r_set_hvflip(struct usb_sn9c20x *dev)
{
	__u8 buf;
	int ret;

	ret = sn9c20x_read_i2c_data(dev, 1, 0x01, &buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.hflip)
		buf |= 0x02;
	else
		buf &= ~0x02;

	if (dev->vsettings.vflip)
		buf |= 0x01;
	else
		buf &= ~0x01;

	ret = sn9c20x_write_i2c_data(dev, 1, 0x01, &buf);

	return ret;
}
