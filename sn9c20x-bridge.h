/**
 * @file sn9c20x.h
 * @author Dave Neuer
 * @date 2008-03-02
 *
 * @brief Common functions and data for the Sonix SN9C20x webcam bridge chips.
 *
 * @note Copyright (C) Dave Neuer
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

#ifndef SN9C20X_BRIDGE_H
#define SN9C20X_BRIDGE_H

#define SN9C20X_I2C_400KHZ	0x01
#define SN9C20X_I2C_READ	0x02
#define SN9C20X_I2C_READY	0x04
#define SN9C20X_I2C_ERROR	0x08
#define SN9C20X_I2C_2WIRE	0x80

/* registers */
#define SN9C20X_SHARPNESS	0x10f6
#define SN9C20X_RED_GAIN	0x118c
#define SN9C20X_GREEN1_GAIN	0x118d
#define SN9C20X_GREEN2_GAIN	0x118e
#define SN9C20X_BLUE_GAIN	0x118f
#define SN9C20X_SCALE		0x1189

#define SN9C20X_NO_SCALE	0x00
#define SN9C20X_1_2_SCALE	0x10
#define SN9C20X_1_4_SCALE	0x20

int sn9c20x_initialize(struct usb_sn9c20x *dev);
int sn9c20x_reset_device(struct usb_sn9c20x *dev);
int sn9c20x_set_LEDs(struct usb_sn9c20x *dev, int enable);
int sn9c20x_set_camera_control(struct usb_sn9c20x *dev,
				 __u32 control, __s32 value);
int sn9c20x_enable_video(struct usb_sn9c20x *dev, int enable);
int sn9c20x_i2c_initialize(struct usb_sn9c20x *dev);

int sn9c20x_write_i2c_data_ext(struct usb_sn9c20x *dev, __u8 nbytes,
	__u8 address, const __u8 [nbytes], __u8 last_byte);

static inline int sn9c20x_write_i2c_data(struct usb_sn9c20x *dev, __u8 nbytes,
	__u8 address, const __u8 data[nbytes])
{
	return sn9c20x_write_i2c_data_ext(dev, nbytes, address, data,
		0x10);
}

int sn9c20x_read_i2c_data(struct usb_sn9c20x *dev, __u8 nbytes,
	__u8 address, __u8 *result);

int sn9c20x_write_i2c_data16(struct usb_sn9c20x *dev, __u8 datalen,
	__u8 address, const __u16 [datalen]);

int sn9c20x_read_i2c_data16(struct usb_sn9c20x *dev, __u8 datalen,
	__u8 address, __u16 *result);

int sn9c20x_write_i2c_array(struct usb_sn9c20x *dev,
	struct sn9c20x_i2c_regs *regs, int bits16);

int sn9c20x_set_resolution(struct usb_sn9c20x *dev,
	int width, int height);

int sn9c20x_get_closest_resolution(struct usb_sn9c20x *, int *, int *);
int sn9c20x_set_format(struct usb_sn9c20x *, __u32);
void sn9c20x_set_jpeg(struct usb_sn9c20x *);
void sn9c20x_set_raw(struct usb_sn9c20x *);
void sn9c20x_set_yuv420(struct usb_sn9c20x *);
void sn9c20x_set_yuv422(struct usb_sn9c20x *);

#endif
