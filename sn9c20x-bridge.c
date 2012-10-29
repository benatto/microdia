/**
 * @file sn9c20x-bridge.c
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

#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/string.h>
#include "sn9c20x.h"
#include "sn9c20x-bridge.h"



/**
 * @var RX
 *   Coordinate X aray for eliptic HSV corrections, conditionaly Red pixel
 */
static const int RX[] = {
	41,  44,  46,  48,  50,  52,  54,  56,
	58,  60,  62,  64,  66,  68,  70,  72,
	74,  76,  78,  80,  81,  83,  85,  87,
	88,  90,  92,  93,  95,  97,  98, 100,
	101, 102, 104, 105, 107, 108, 109, 110,
	112, 113, 114, 115, 116, 117, 118, 119,
	120, 121, 122, 123, 123, 124, 125, 125,
	126, 127, 127, 128, 128, 129, 129, 129,
	130, 130, 130, 130, 131, 131, 131, 131,
	131, 131, 131, 131, 130, 130, 130, 130,
	129, 129, 129, 128, 128, 127, 127, 126,
	125, 125, 124, 123, 122, 122, 121, 120,
	119, 118, 117, 116, 115, 114, 112, 111,
	110, 109, 107, 106, 105, 103, 102, 101,
	99,  98,  96,  94,  93,  91,  90,  88,
	86,  84,  83,  81,  79,  77,  75,  74,
	72,  70,  68,  66,  64,  62,  60,  58,
	56,  54,  52,  49,  47,  45,  43,  41,
	39,  36,  34,  32,  30,  28,  25,  23,
	21,  19,  16,  14,  12,   9,   7,   5,
	3,   0,  -1,  -3,  -6,  -8, -10, -12,
	-15, -17, -19, -22, -24, -26, -28, -30,
	-33, -35, -37, -39, -41, -44, -46, -48,
	-50, -52, -54, -56, -58, -60, -62, -64,
	-66, -68, -70, -72, -74, -76, -78, -80,
	-81, -83, -85, -87, -88, -90, -92, -93,
	-95, -97, -98, -100, -101, -102, -104, -105,
	-107, -108, -109, -110, -112, -113, -114, -115,
	-116, -117, -118, -119, -120, -121, -122, -123,
	-123, -124, -125, -125, -126, -127, -127, -128,
	-128, -128, -128, -128, -128, -128, -128, -128,
	-128, -128, -128, -128, -128, -128, -128, -128,
	-128, -128, -128, -128, -128, -128, -128, -128,
	-128, -127, -127, -126, -125, -125, -124, -123,
	-122, -122, -121, -120, -119, -118, -117, -116,
	-115, -114, -112, -111, -110, -109, -107, -106,
	-105, -103, -102, -101, -99, -98, -96, -94,
	-93, -91, -90, -88, -86, -84, -83, -81,
	-79, -77, -75, -74, -72, -70, -68, -66,
	-64, -62, -60, -58, -56, -54, -52, -49,
	-47, -45, -43, -41, -39, -36, -34, -32,
	-30, -28, -25, -23, -21, -19, -16, -14,
	-12,  -9,  -7,  -5,  -3,   0,   1,   3,
	6,   8,  10,  12,  15,  17,  19,  22,
	24,  26,  28,  30,  33,  35,  37,  39, 41
};
/**
 * @var RY
 *   Coordinate Y aray for eliptic HSV corrections, conditionaly Red pixel
 */
static const int RY[] = {
	82,  80,  78,  76,  74,  73,  71,  69,
	67,  65,  63,  61,  58,  56,  54,  52,
	50,  48,  46,  44,  41,  39,  37,  35,
	32,  30,  28,  26,  23,  21,  19,  16,
	14,  12,  10,   7,   5,   3,   0,  -1,
	-3,  -6,  -8, -10, -13, -15, -17, -19,
	-22, -24, -26, -29, -31, -33, -35, -38,
	-40, -42, -44, -46, -48, -51, -53, -55,
	-57, -59, -61, -63, -65, -67, -69, -71,
	-73, -75, -77, -79, -81, -82, -84, -86,
	-88, -89, -91, -93, -94, -96, -98, -99,
	-101, -102, -104, -105, -106, -108, -109, -110,
	-112, -113, -114, -115, -116, -117, -119, -120,
	-120, -121, -122, -123, -124, -125, -126, -126,
	-127, -128, -128, -128, -128, -128, -128, -128,
	-128, -128, -128, -128, -128, -128, -128, -128,
	-128, -128, -128, -128, -128, -128, -128, -128,
	-128, -128, -128, -128, -128, -128, -128, -128,
	-127, -127, -126, -125, -125, -124, -123, -122,
	-121, -120, -119, -118, -117, -116, -115, -114,
	-113, -111, -110, -109, -107, -106, -105, -103,
	-102, -100, -99, -97, -96, -94, -92, -91,
	-89, -87, -85, -84, -82, -80, -78, -76,
	-74, -73, -71, -69, -67, -65, -63, -61,
	-58, -56, -54, -52, -50, -48, -46, -44,
	-41, -39, -37, -35, -32, -30, -28, -26,
	-23, -21, -19, -16, -14, -12, -10,  -7,
	-5,  -3,   0,   1,   3,   6,   8,  10,
	13,  15,  17,  19,  22,  24,  26,  29,
	31,  33,  35,  38,  40,  42,  44,  46,
	48,  51,  53,  55,  57,  59,  61,  63,
	65,  67,  69,  71,  73,  75,  77,  79,
	81,  82,  84,  86,  88,  89,  91,  93,
	94,  96,  98,  99, 101, 102, 104, 105,
	106, 108, 109, 110, 112, 113, 114, 115,
	116, 117, 119, 120, 120, 121, 122, 123,
	124, 125, 126, 126, 127, 128, 128, 129,
	129, 130, 130, 131, 131, 131, 131, 132,
	132, 132, 132, 132, 132, 132, 132, 132,
	132, 132, 132, 131, 131, 131, 130, 130,
	130, 129, 129, 128, 127, 127, 126, 125,
	125, 124, 123, 122, 121, 120, 119, 118,
	117, 116, 115, 114, 113, 111, 110, 109,
	107, 106, 105, 103, 102, 100,  99,  97,
	96, 94, 92, 91, 89, 87, 85, 84, 82
};

/**
 * @var GX
 *   Coordinate X aray for eliptic HSV corrections, conditionaly Green pixel
 */
static const int GX[] = {
	-124, -124, -125, -125, -125, -125, -125, -125,
	-125, -126, -126, -125, -125, -125, -125, -125,
	-125, -124, -124, -124, -123, -123, -122, -122,
	-121, -121, -120, -120, -119, -118, -117, -117,
	-116, -115, -114, -113, -112, -111, -110, -109,
	-108, -107, -105, -104, -103, -102, -100, -99,
	-98, -96, -95, -93, -92, -91, -89, -87,
	-86, -84, -83, -81, -79, -77, -76, -74,
	-72, -70, -69, -67, -65, -63, -61, -59,
	-57, -55, -53, -51, -49, -47, -45, -43,
	-41, -39, -37, -35, -33, -30, -28, -26,
	-24, -22, -20, -18, -15, -13, -11,  -9,
	-7,  -4,  -2,   0,   1,   3,   6,   8,
	10,  12,  14,  17,  19,  21,  23,  25,
	27,  29,  32,  34,  36,  38,  40,  42,
	44,  46,  48,  50,  52,  54,  56,  58,
	60,  62,  64,  66,  68,  70,  71,  73,
	75,  77,  78,  80,  82,  83,  85,  87,
	88,  90,  91,  93,  94,  96,  97,  98,
	100, 101, 102, 104, 105, 106, 107, 108,
	109, 111, 112, 113, 113, 114, 115, 116,
	117, 118, 118, 119, 120, 120, 121, 122,
	122, 123, 123, 124, 124, 124, 125, 125,
	125, 125, 125, 125, 125, 126, 126, 125,
	125, 125, 125, 125, 125, 124, 124, 124,
	123, 123, 122, 122, 121, 121, 120, 120,
	119, 118, 117, 117, 116, 115, 114, 113,
	112, 111, 110, 109, 108, 107, 105, 104,
	103, 102, 100,  99,  98,  96,  95,  93,
	92,  91,  89,  87,  86,  84,  83,  81,
	79,  77,  76,  74,  72,  70,  69,  67,
	65,  63,  61,  59,  57,  55,  53,  51,
	49,  47,  45,  43,  41,  39,  37,  35,
	33,  30,  28,  26,  24,  22,  20,  18,
	15,  13,  11,   9,   7,   4,   2,   0,
	-1,  -3,  -6,  -8, -10, -12, -14, -17,
	-19, -21, -23, -25, -27, -29, -32, -34,
	-36, -38, -40, -42, -44, -46, -48, -50,
	-52, -54, -56, -58, -60, -62, -64, -66,
	-68, -70, -71, -73, -75, -77, -78, -80,
	-82, -83, -85, -87, -88, -90, -91, -93,
	-94, -96, -97, -98, -100, -101, -102, -104,
	-105, -106, -107, -108, -109, -111, -112, -113,
	-113, -114, -115, -116, -117, -118, -118, -119,
	-120, -120, -121, -122, -122, -123, -123, -124, -124
};

/**
 * @var GY
 *   Coordinate Y aray for eliptic HSV corrections, conditionaly Green pixel
 */
static const int GY[] = {
	-100, -99, -98, -97, -95, -94, -93, -91,
	-90, -89, -87, -86, -84, -83, -81, -80,
	-78, -76, -75, -73, -71, -70, -68, -66,
	-64, -63, -61, -59, -57, -55, -53, -51,
	-49, -48, -46, -44, -42, -40, -38, -36,
	-34, -32, -30, -27, -25, -23, -21, -19,
	-17, -15, -13, -11,  -9,  -7,  -4,  -2,
	0,   1,   3,   5,   7,   9,  11,  14,
	16,  18,  20,  22,  24,  26,  28,  30,
	32,  34,  36,  38,  40,  42,  44,  46,
	48,  50,  52,  54,  56,  58,  59,  61,
	63,  65,  67,  68,  70,  72,  74,  75,
	77,  78,  80,  82,  83,  85,  86,  88,
	89,  90,  92,  93,  95,  96,  97,  98,
	100, 101, 102, 103, 104, 105, 106, 107,
	108, 109, 110, 111, 112, 112, 113, 114,
	115, 115, 116, 116, 117, 117, 118, 118,
	119, 119, 119, 120, 120, 120, 120, 120,
	121, 121, 121, 121, 121, 121, 120, 120,
	120, 120, 120, 119, 119, 119, 118, 118,
	117, 117, 116, 116, 115, 114, 114, 113,
	112, 111, 111, 110, 109, 108, 107, 106,
	105, 104, 103, 102, 100,  99,  98,  97,
	95,  94,  93,  91,  90,  89,  87,  86,
	84,  83,  81,  80,  78,  76,  75,  73,
	71,  70,  68,  66,  64,  63,  61,  59,
	57,  55,  53,  51,  49,  48,  46,  44,
	42,  40,  38,  36,  34,  32,  30,  27,
	25,  23,  21,  19,  17,  15,  13,  11,
	9,   7,   4,   2,   0,  -1,  -3,  -5,
	-7,  -9, -11, -14, -16, -18, -20, -22,
	-24, -26, -28, -30, -32, -34, -36, -38,
	-40, -42, -44, -46, -48, -50, -52, -54,
	-56, -58, -59, -61, -63, -65, -67, -68,
	-70, -72, -74, -75, -77, -78, -80, -82,
	-83, -85, -86, -88, -89, -90, -92, -93,
	-95, -96, -97, -98, -100, -101, -102, -103,
	-104, -105, -106, -107, -108, -109, -110, -111,
	-112, -112, -113, -114, -115, -115, -116, -116,
	-117, -117, -118, -118, -119, -119, -119, -120,
	-120, -120, -120, -120, -121, -121, -121, -121,
	-121, -121, -120, -120, -120, -120, -120, -119,
	-119, -119, -118, -118, -117, -117, -116, -116,
	-115, -114, -114, -113, -112, -111, -111, -110,
	-109, -108, -107, -106, -105, -104, -103, -102, -100
};

/**
 * @var BX
 *   Coordinate X aray for eliptic HSV corrections, conditionaly Blue pixel
 */
static const int BX[] = {
	112, 113, 114, 114, 115, 116, 117, 117,
	118, 118, 119, 119, 120, 120, 120, 121,
	121, 121, 122, 122, 122, 122, 122, 122,
	122, 122, 122, 122, 122, 122, 121, 121,
	121, 120, 120, 120, 119, 119, 118, 118,
	117, 116, 116, 115, 114, 113, 113, 112,
	111, 110, 109, 108, 107, 106, 105, 104,
	103, 102, 100,  99,  98,  97,  95,  94,
	93,  91,  90,  88,  87,  85,  84,  82,
	80,  79,  77,  76,  74,  72,  70,  69,
	67,  65,  63,  61,  60,  58,  56,  54,
	52,  50,  48,  46,  44,  42,  40,  38,
	36,  34,  32,  30,  28,  26,  24,  22,
	19,  17,  15,  13,  11,   9,   7,   5,
	2,   0,  -1,  -3,  -5,  -7,  -9, -12,
	-14, -16, -18, -20, -22, -24, -26, -28,
	-31, -33, -35, -37, -39, -41, -43, -45,
	-47, -49, -51, -53, -54, -56, -58, -60,
	-62, -64, -66, -67, -69, -71, -73, -74,
	-76, -78, -79, -81, -83, -84, -86, -87,
	-89, -90, -92, -93, -94, -96, -97, -98,
	-99, -101, -102, -103, -104, -105, -106, -107,
	-108, -109, -110, -111, -112, -113, -114, -114,
	-115, -116, -117, -117, -118, -118, -119, -119,
	-120, -120, -120, -121, -121, -121, -122, -122,
	-122, -122, -122, -122, -122, -122, -122, -122,
	-122, -122, -121, -121, -121, -120, -120, -120,
	-119, -119, -118, -118, -117, -116, -116, -115,
	-114, -113, -113, -112, -111, -110, -109, -108,
	-107, -106, -105, -104, -103, -102, -100, -99,
	-98, -97, -95, -94, -93, -91, -90, -88,
	-87, -85, -84, -82, -80, -79, -77, -76,
	-74, -72, -70, -69, -67, -65, -63, -61,
	-60, -58, -56, -54, -52, -50, -48, -46,
	-44, -42, -40, -38, -36, -34, -32, -30,
	-28, -26, -24, -22, -19, -17, -15, -13,
	-11,  -9,  -7,  -5,  -2,   0,   1,   3,
	5,   7,   9,  12,  14,  16,  18,  20,
	22,  24,  26,  28,  31,  33,  35,  37,
	39,  41,  43,  45,  47,  49,  51,  53,
	54,  56,  58,  60,  62,  64,  66,  67,
	69,  71,  73,  74,  76,  78,  79,  81,
	83,  84,  86,  87,  89,  90,  92,  93,
	94,  96,  97,  98,  99, 101, 102, 103,
	104, 105, 106, 107, 108, 109, 110, 111, 112
};

/**
 * @var BY
 *   Coordinate Y aray for eliptic HSV corrections, conditionaly Blue pixel
 */
static const int BY[] = {
	-11, -13, -15, -17, -19, -21, -23, -25,
	-27, -29, -31, -33, -35, -37, -39, -41,
	-43, -45, -46, -48, -50, -52, -54, -55,
	-57, -59, -61, -62, -64, -66, -67, -69,
	-71, -72, -74, -75, -77, -78, -80, -81,
	-83, -84, -86, -87, -88, -90, -91, -92,
	-93, -95, -96, -97, -98, -99, -100, -101,
	-102, -103, -104, -105, -106, -106, -107, -108,
	-109, -109, -110, -111, -111, -112, -112, -113,
	-113, -114, -114, -114, -115, -115, -115, -115,
	-116, -116, -116, -116, -116, -116, -116, -116,
	-116, -115, -115, -115, -115, -114, -114, -114,
	-113, -113, -112, -112, -111, -111, -110, -110,
	-109, -108, -108, -107, -106, -105, -104, -103,
	-102, -101, -100, -99, -98, -97, -96, -95,
	-94, -93, -91, -90, -89, -88, -86, -85,
	-84, -82, -81, -79, -78, -76, -75, -73,
	-71, -70, -68, -67, -65, -63, -62, -60,
	-58, -56, -55, -53, -51, -49, -47, -45,
	-44, -42, -40, -38, -36, -34, -32, -30,
	-28, -26, -24, -22, -20, -18, -16, -14,
	-12, -10,  -8,  -6,  -4,  -2,   0,   1,
	3,   5,   7,   9,  11,  13,  15,  17,
	19,  21,  23,  25,  27,  29,  31,  33,
	35,  37,  39,  41,  43,  45,  46,  48,
	50,  52,  54,  55,  57,  59,  61,  62,
	64,  66,  67,  69,  71,  72,  74,  75,
	77,  78,  80,  81,  83,  84,  86,  87,
	88,  90,  91,  92,  93,  95,  96,  97,
	98,  99, 100, 101, 102, 103, 104, 105,
	106, 106, 107, 108, 109, 109, 110, 111,
	111, 112, 112, 113, 113, 114, 114, 114,
	115, 115, 115, 115, 116, 116, 116, 116,
	116, 116, 116, 116, 116, 115, 115, 115,
	115, 114, 114, 114, 113, 113, 112, 112,
	111, 111, 110, 110, 109, 108, 108, 107,
	106, 105, 104, 103, 102, 101, 100,  99,
	98,  97,  96,  95,  94,  93,  91,  90,
	89,  88,  86,  85,  84,  82,  81,  79,
	78,  76,  75,  73,  71,  70,  68,  67,
	65,  63,  62,  60,  58,  56,  55,  53,
	51,  49,  47,  45,  44,  42,  40,  38,
	36,  34,  32,  30,  28,  26,  24,  22,
	20,  18,  16,  14,  12,  10,   8,   6,
	4,   2,   0,  -1,  -3,  -5,  -7,  -9, -11
};


int sn9c20x_set_camera_control(struct usb_sn9c20x *dev,
	__u32 control, __s32 value)
{
	int ret = -EINVAL;
	switch (control) {
	case V4L2_CID_CONTRAST:
		if (dev->camera.set_contrast) {
			dev->vsettings.contrast = value;
			ret = dev->camera.set_contrast(dev);
		}
		break;
	case V4L2_CID_BRIGHTNESS:
		if (dev->camera.set_brightness) {
			dev->vsettings.brightness = value;
			ret = dev->camera.set_brightness(dev);
		}
		break;
	case V4L2_CID_GAMMA:
		if (dev->camera.set_gamma) {
			dev->vsettings.gamma = value;
			ret = dev->camera.set_gamma(dev);
		}
		break;
	case V4L2_CID_SATURATION:
		if (dev->camera.set_saturation) {
			dev->vsettings.colour = value;
			ret = dev->camera.set_saturation(dev);
		}
		break;
	case V4L2_CID_HUE:
		if (dev->camera.set_hue) {
			dev->vsettings.hue = value;
			ret = dev->camera.set_hue(dev);
		}
		break;
	case V4L2_CID_SHARPNESS:
		if (dev->camera.set_sharpness) {
			dev->vsettings.sharpness = value;
			ret = dev->camera.set_sharpness(dev);
		}
		break;
	case V4L2_CID_RED_BALANCE:
		if (dev->camera.set_red_gain) {
			dev->vsettings.red_gain = value & 0x7f;
			ret = dev->camera.set_red_gain(dev);
		}
		break;
	case V4L2_CID_BLUE_BALANCE:
		if (dev->camera.set_blue_gain) {
			dev->vsettings.blue_gain = value & 0x7f;
			ret = dev->camera.set_blue_gain(dev);
		}
		break;
	case V4L2_CID_HFLIP:
		if (dev->camera.set_hvflip) {
			dev->vsettings.hflip = value;
			ret = dev->camera.set_hvflip(dev);
		}
		break;
	case V4L2_CID_VFLIP:
		if (dev->camera.set_hvflip) {
			dev->vsettings.vflip = value;
			ret = dev->camera.set_hvflip(dev);
		}
		break;
	case V4L2_CID_AUTOGAIN:
		if (dev->camera.set_auto_gain) {
			dev->vsettings.auto_gain = value;
			ret = dev->camera.set_auto_gain(dev);
		}
		break;
	case V4L2_CID_EXPOSURE:
		if (dev->camera.set_exposure) {
			dev->vsettings.exposure = value;
			ret = dev->camera.set_exposure(dev);
		}
		break;
	case V4L2_CID_GAIN:
		if (dev->camera.set_gain) {
			dev->vsettings.gain = value;
			ret = dev->camera.set_gain(dev);
		}
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		if (dev->camera.set_auto_whitebalance) {
			dev->vsettings.auto_whitebalance = value;
			ret = dev->camera.set_auto_whitebalance(dev);
		}
		break;
	case V4L2_CID_EXPOSURE_AUTO:
		dev->vsettings.auto_exposure = value;
		ret = 0;
		if (dev->camera.set_auto_exposure)
			ret = dev->camera.set_auto_exposure(dev);
		break;
	}
	return ret;
}

/**
 * @brief Switch Video stream on and off
 *
 * @param dev Pointer to device structure
 * @param enable On or off
 *
 * @author Brian Johnson
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_enable_video(struct usb_sn9c20x *dev, int enable)
{
	__u8 value;
	int ret;

	ret = usb_sn9c20x_control_read(dev, 0x1061, &value, 1);
	if (ret < 0)
		return ret;

	if (enable) {
		value |= 0x02;
		ret = usb_sn9c20x_control_write(dev, 0x1061, &value, 1);
	} else {
		value &= ~0x02;
		ret = usb_sn9c20x_control_write(dev, 0x1061, &value, 1);
	}

	sn9c20x_set_LEDs(dev, enable);

	return ret;
}

/**
 * @brief Switch LEDs on and off
 *
 * @param dev Pointer to device structure
 * @param enable On or off
 *
 * @author Josua Grawitter
 *
 * @return 0 or negative error code
 *
 */
int sn9c20x_set_LEDs(struct usb_sn9c20x *dev, int enable)
{
	int ret;
	__u8 led[2];

	int sensor = dev->camera.sensor;

	if (enable) {
		switch (sensor) {
		case MT9M001_SENSOR:
		case HV7131R_SENSOR:
		case SOI968_SENSOR:
			led[0] = 0x80;
			led[1] = 0xa0;
			break;
		case MT9V011_SENSOR:
		case OV7660_SENSOR:
			led[0] = 0x40;
			led[1] = 0x60;
			break;
		case MT9M111_SENSOR:
			led[0] = 0x00;
			led[1] = 0x60;
			break;
		case MT9V111_SENSOR:
			led[0] = 0xc0;
			led[1] = 0xe0;
			break;
		default:
			led[0] = 0x00;
			led[1] = 0x20;
		}
	} else {
		switch (sensor) {
		case HV7131R_SENSOR:
		case MT9M001_SENSOR:
		case MT9V011_SENSOR:
			led[0] = 0xa0;
			led[1] = 0xa0;
			break;
		case MT9M111_SENSOR:
		case OV7660_SENSOR:
			led[0] = 0x20;
			led[1] = 0x60;
			break;
		case MT9V111_SENSOR:
			led[0] = 0xa0;
			led[1] = 0xe0;
			break;
		default:
			led[0] = 0x20;
			led[1] = 0x20;
		}
	}

	ret = usb_sn9c20x_control_write(dev, 0x1006, led, 2);

	return ret;
}

/**
 * @brief Initializes Micro-Controller's I2C interface
 *
 * @author Neekhil
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_i2c_initialize(struct usb_sn9c20x *dev)
{
	__u8 buf[9];
	int ret;

	dev->camera.i2c_flags = SN9C20X_I2C_2WIRE;

	buf[0] = dev->camera.i2c_flags;
	buf[1] = dev->camera.address;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x00;
	buf[5] = 0x00;
	buf[6] = 0x00;
	buf[7] = 0x00;
	buf[8] = 0x03;
	/*  Initialize I2C registers to avoid getting no ACK at first I2C operation: */
	/* Get green diagonal bands w/o this dummy write, Bridge does not know sensor address ? */
	ret = usb_sn9c20x_control_write(dev, 0x10c0, buf, 9);
	if (ret < 0)
		return ret;
	else
		return 0;
}

/**
 * @brief Wait until the I2C slave is ready for the next operation
 *
 * @param dev Pointer to the device
 * @param highspeed
 * @param slave_error
 *
 * @return Zero for success or a negative error value
 *
 */
int sn9c20x_i2c_ack_wait(struct usb_sn9c20x *dev, bool highspeed, bool *slave_error)
{
	int ret, i;
	__u8 readbuf;
	int delay = highspeed ? 100 : 400;

	for (i = 0; i < 5; i++) {
		ret = usb_sn9c20x_control_read(dev, 0x10c0, &readbuf, 1);

		if (unlikely(ret < 0))
			return ret;
		else if (readbuf & SN9C20X_I2C_ERROR) {
			*slave_error = 1;
			/* probably should come up w/ an error value and
			 * return it via the error return */
			return 0;
		} else if (readbuf & SN9C20X_I2C_READY)
			return 0;
		else
			udelay(delay);
	}
	return -EBUSY;
}

/**
 * @brief Read up to 5 bytes of data from an I2C slave
 *
 * @param dev Pointer to the device
 * @param nbytes Number of bytes to read
 * @param address The address of the register on the slave to read
 * @param result A pointer to the location at which the result should be stored
 *
 * @return Zero for success or a negative error value
 *
 */
int sn9c20x_read_i2c_data(struct usb_sn9c20x *dev, __u8 nbytes,
	__u8 address, __u8 *result)
{
	int ret, i, j;
	__u8 row[5];

	if (!dev || nbytes > 4)
		return -EINVAL;

	/* first, we must do a dummy write of just the address */
	ret = sn9c20x_write_i2c_data(dev, 0, address, NULL);
	if (unlikely(ret < 0))
		return ret;

	memset(row, 0, 5);
	/* now we issue the same command but with the read bit set
	 * and no slave register address */
	dev->camera.i2c_flags |= SN9C20X_I2C_READ;
	ret = sn9c20x_write_i2c_data(dev, nbytes - 1, 0, row);
	dev->camera.i2c_flags &= ~SN9C20X_I2C_READ;
	if (unlikely(ret < 0))
		return ret;

	/* finally, ask the bridge for the data */
	ret = usb_sn9c20x_control_read(dev, 0x10c2, row, 5);
	if (unlikely(ret < 0))
		return ret;

	UDIA_DEBUG("I2C read: %02x %02x %02x %02x %02x %02x\n",
		address, row[0], row[1], row[2], row[3], row[4]);

	for (i = 0, j = 5 - nbytes; i < nbytes; i++, j++)
		result[i] = row[j];

	return 0;
}

/**
 * @brief Read up to 4 bytes of data from an I2C slave an return them as 16bit values
 *
 * @param dev Pointer to the device
 * @param datalen Number of 16bit values to read
 * @param address The address of the register on the slave to read
 * @param result A pointer to the location at which the result should be stored
 *
 * @return Zero for success or a negative error value
 *
 */
int sn9c20x_read_i2c_data16(struct usb_sn9c20x *dev, __u8 datalen,
	__u8 address, __u16 *result)
{
	__u8 result8[4];
	__u8 k;
	int ret;

	if (datalen > 2)
		return -EINVAL;
	ret = sn9c20x_read_i2c_data(dev, 2*datalen, address, result8);
	for (k = 0; k < datalen; k++)
		result[k] = (result8[k*2] << 8) | result8[k*2+1];
	return ret;
}

static const char *wasread = "read from";
static const char *waswrite = "write to";

/**
 * @brief Write up to 5 bytes of data to an I2C slave
 *
 * @param dev Pointer to the device
 * @param nbytes The number of bytes of data
 * @param address The address of the register on the slave to write
 * @param data An array containing the data to write
 * @param last_byte The byte to be sent as last byte of control sequence
 *
 * @return Zero for success or a negative error value
 *
 */
int sn9c20x_write_i2c_data_ext(struct usb_sn9c20x *dev, __u8 nbytes,
	__u8 address, const __u8 data[nbytes], __u8 last_byte)
{
	int ret, i;
	__u8 row[8];
	bool slave_error = 0;

	if (!dev || (nbytes > 0 && !data) || nbytes > 4)
		return -EINVAL;

	/* from the point of view of the bridge, the length
	 * includes the address */
	row[0] = dev->camera.i2c_flags | ((nbytes + 1) << 4);
	row[1] = dev->camera.address;
	row[2] = address;
	row[7] = last_byte;

	for (i = 0; i < 4; i++)
		row[i + 3] = i < nbytes ? data[i] : 0;

	UDIA_DEBUG("I2C %s %02x: %02x %02x %02x %02x %02x %02x %02x %02x\n",
		(dev->camera.i2c_flags & SN9C20X_I2C_READ ? wasread : waswrite),
		address, row[0], row[1], row[2], row[3], row[4], row[5], row[6],
		row[7]);

	ret = usb_sn9c20x_control_write(dev, 0x10c0, row, 8);
	if (likely(ret >= 0))
		ret = sn9c20x_i2c_ack_wait(dev,
			dev->camera.i2c_flags & SN9C20X_I2C_400KHZ,
			&slave_error);

	if (slave_error) {
		UDIA_ERROR("I2C slave 0x%02x returned error during %s address 0x%02x\n",
			dev->camera.address, (dev->camera.i2c_flags &
			SN9C20X_I2C_READ ? wasread : waswrite), address);
		return -1000;
		/* there should be no interference with USB errors */
	}

	if (ret < 0) {
		/* we got no ack */
		UDIA_ERROR("No ack from I2C slave 0x%02x for %s address 0x%02x\n",
			dev->camera.address, (dev->camera.i2c_flags &
			SN9C20X_I2C_READ ? wasread : waswrite), address);
		return ret;
	}

	return 0;
}

/**
 * @brief Write up to 2 16bit values als single bytes to an I2C slave
 *
 * @param dev Pointer to the device
 * @param datalen The number of 16bit data values to write
 * @param address The address of the register on the slave to write
 * @param data An array containing the data to write
 *
 * @return Zero for success or a negative error value
 *
 */
int sn9c20x_write_i2c_data16(struct usb_sn9c20x *dev, __u8 datalen,
	__u8 address, const __u16 data[datalen])
{
	__u8 data8[4];
	__u8 k;
	int ret;

	if (datalen > 2)
		return -EINVAL;
	for (k = 0; k < datalen; k++) {
		data8[k*2] = data[k] >> 8;
		data8[k*2+1] = data[k] & 0xff;
	}
	ret = sn9c20x_write_i2c_data(dev, 2*datalen, address, data8);
	return ret;
}

int sn9c20x_write_i2c_array(struct usb_sn9c20x *dev,
	struct sn9c20x_i2c_regs *regs, int bits16)
{
	int i;
	int ret = 0;
	__u16 value16;
	__u8 value8;
	for (i = 0;; i++) {
		if (regs[i].address == 0xff)
			break;
		if (bits16) {
			value16 = regs[i].value;
			ret = sn9c20x_write_i2c_data16(dev, 1,
				regs[i].address, &value16);
		} else {
			value8 = (__u8)regs[i].value;
			ret = sn9c20x_write_i2c_data(dev, 1,
				regs[i].address, &value8);
		}
		if (unlikely(ret < 0))
			return ret;
	}
	return ret;
}

/**
 * @brief Calculate and set hue/sat
 *
 * @author Comer352l, Boris Borisov
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_set_color(struct usb_sn9c20x *dev)
{
	__s16 value;
	long tmp_coordinate;
	__u8 *cmatrix = dev->vsettings.cmatrix;

	if (dev->vsettings.hue < -179)
		dev->vsettings.hue = -180;

	if (dev->vsettings.hue > 179)
		dev->vsettings.hue = 180;

	value = 180 + dev->vsettings.hue;

	/* Calculate HUE and SATURATION */
	tmp_coordinate = (long)(RX[value]);
	tmp_coordinate = (tmp_coordinate * dev->vsettings.colour) >> 8;
	cmatrix[6] = (unsigned char)(tmp_coordinate & 0xff);
	cmatrix[7] = (unsigned char)((tmp_coordinate >> 8) & 0x0f);

	tmp_coordinate = (long)(RY[value]);
	tmp_coordinate = (tmp_coordinate * dev->vsettings.colour) >> 8;
	cmatrix[8] = (unsigned char)(tmp_coordinate & 0xff);
	cmatrix[9] = (unsigned char)((tmp_coordinate >> 8) & 0x0f);

	tmp_coordinate = (long)(GX[value]);
	tmp_coordinate = (tmp_coordinate * dev->vsettings.colour) >> 8;
	cmatrix[10] = (unsigned char)(tmp_coordinate & 0xff);
	cmatrix[11] = (unsigned char)((tmp_coordinate >> 8) & 0x0f);

	tmp_coordinate = (long)(GY[value]);
	tmp_coordinate = (tmp_coordinate * dev->vsettings.colour) >> 8;
	cmatrix[12] = (unsigned char)(tmp_coordinate & 0xff);
	cmatrix[13] = (unsigned char)((tmp_coordinate >> 8) & 0x0f);

	tmp_coordinate = (long)(BX[value]);
	tmp_coordinate = (tmp_coordinate * dev->vsettings.colour) >> 8;
	cmatrix[14] = (unsigned char)(tmp_coordinate & 0xff);
	cmatrix[15] = (unsigned char)((tmp_coordinate >> 8) & 0x0f);

	tmp_coordinate = (long)(BY[value]);
	tmp_coordinate = (tmp_coordinate * dev->vsettings.colour) >> 8;
	cmatrix[16] = (unsigned char)(tmp_coordinate & 0xff);
	cmatrix[17] = (unsigned char)((tmp_coordinate >> 8) & 0x0f);


	return usb_sn9c20x_control_write(dev, 0x10e1, cmatrix, 21);
}

/**
 * @brief Set contrast inside sn9c20x chip
 *
 * @author Comer352l
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_set_contrast(struct usb_sn9c20x *dev)
{
	__u8 *cmatrix = dev->vsettings.cmatrix;
	__u8 contrast_val = (dev->vsettings.contrast) * 0x25 / 0x100;

	contrast_val += 0x26;
	cmatrix[2] = contrast_val;
	cmatrix[0] = 0x13 + (cmatrix[2] - 0x26) * 0x13 / 0x25;
	cmatrix[4] = 0x07 + (cmatrix[2] - 0x26) * 0x07 / 0x25;

	return usb_sn9c20x_control_write(dev, 0x10e1, cmatrix, 21);
}

/**
 * @brief Set brightness inside sn9c20x chip
 *
 * @author Comer352l
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 * Wrapper for sn9c20x_set_contrast
 *
 */
int sn9c20x_set_brightness(struct usb_sn9c20x *dev)
{
	__u8 *cmatrix = dev->vsettings.cmatrix;

	cmatrix[18] = dev->vsettings.brightness - 0x80;

	return usb_sn9c20x_control_write(dev, 0x10e1, cmatrix, 21);
}

/**
 * @brief Set gamma inside sn9c20x chip
 *
 * @author Comer352l
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_set_gamma(struct usb_sn9c20x *dev)
{
	int value = (dev->vsettings.gamma) * 0xb8 / 0x100;
	int r = 0;

	__u8 gamma_val[17] = {0x0a, 0x13, 0x25, 0x37, 0x45, 0x55, 0x65, 0x74,
			0x83, 0x92, 0xa1, 0xb0, 0xbf, 0xce, 0xdf, 0xea, 0xf5};

	gamma_val[0] = 0x0a;
	gamma_val[1] = 0x13 + (value * (0xcb - 0x13) / 0xb8);
	gamma_val[2] = 0x25 + (value * (0xee - 0x25) / 0xb8);
	gamma_val[3] = 0x37 + (value * (0xfa - 0x37) / 0xb8);
	gamma_val[4] = 0x45 + (value * (0xfc - 0x45) / 0xb8);
	gamma_val[5] = 0x55 + (value * (0xfb - 0x55) / 0xb8);
	gamma_val[6] = 0x65 + (value * (0xfc - 0x65) / 0xb8);
	gamma_val[7] = 0x74 + (value * (0xfd - 0x74) / 0xb8);
	gamma_val[8] = 0x83 + (value * (0xfe - 0x83) / 0xb8);
	gamma_val[9] = 0x92 + (value * (0xfc - 0x92) / 0xb8);
	gamma_val[10] = 0xa1 + (value * (0xfc - 0xa1) / 0xb8);
	gamma_val[11] = 0xb0 + (value * (0xfc - 0xb0) / 0xb8);
	gamma_val[12] = 0xbf + (value * (0xfb - 0xbf) / 0xb8);
	gamma_val[13] = 0xce + (value * (0xfb - 0xce) / 0xb8);
	gamma_val[14] = 0xdf + (value * (0xfd - 0xdf) / 0xb8);
	gamma_val[15] = 0xea + (value * (0xf9 - 0xea) / 0xb8);
	gamma_val[16] = 0xf5;

	r = usb_sn9c20x_control_write(dev, 0x1190, gamma_val, 17);

	return r;
}

/**
 * @brief Set sharpness inside sn9c20x chip
 *
 * @author Comer352l
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_set_sharpness(struct usb_sn9c20x *dev)
{
	__u8 val[1];
	int ret;

	ret = usb_sn9c20x_control_read(dev, SN9C20X_SHARPNESS, val, 1);
	if (ret < 0)
		return ret;
	val[0] = (val[0] & 0xc0) | (dev->vsettings.sharpness & 0x3f);
	ret = usb_sn9c20x_control_write(dev, SN9C20X_SHARPNESS, val, 1);
	if (ret < 0)
		return ret;
	else
		return 0;
}

/**
 * @brief Set red gain inside sn9c20x chip
 *
 * @author Brian Johnson
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_set_red_gain(struct usb_sn9c20x *dev)
{
	__u8 val;
	int ret;

	val = dev->vsettings.red_gain;
	ret = usb_sn9c20x_control_write(dev, SN9C20X_RED_GAIN, &val, 1);
	if (ret < 0)
		return ret;
	else
		return 0;
}

/**
 * @brief Set blue gain inside sn9c20x chip
 *
 * @author Brian Johnson
 *
 * @param dev Pointer to the device
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int sn9c20x_set_blue_gain(struct usb_sn9c20x *dev)
{
	__u8 val;
	int ret;

	val = dev->vsettings.blue_gain;
	ret = usb_sn9c20x_control_write(dev, SN9C20X_BLUE_GAIN, &val, 1);
	if (ret < 0)
		return ret;
	else
		return 0;
}


/**
 * @brief Calculate closest resolution to input from application
 *
 * @author Brian Johnson
 *
 * @param dev Pointer to the device structure
 * @param width Requested width of video stream
 * @param height Requested height of video stream
 *
 * @retval width Closest possible width of video stream
 * @retval height Closest possible height of video stream
 *
 * @return Number of the
 *
 */
int sn9c20x_get_closest_resolution(struct usb_sn9c20x *dev,
	int *width, int *height)
{
	int i;

	if (*width < sn9c20x_modes[0].width)
		*width = sn9c20x_modes[0].width;

	if (*height < sn9c20x_modes[0].height)
		*height = sn9c20x_modes[0].height;

	if (dev->camera.set_sxga_mode == NULL) {
		if (*width > 640)
			*width = 640;

		if (*height > 480)
			*height = 480;
	}

	for (i = SN9C20X_N_MODES - 1; i >= 0; i--) {
		if (*width >= sn9c20x_modes[i].width
		    && *height >= sn9c20x_modes[i].height)
			break;
	}

	*width = sn9c20x_modes[i].width;
	*height = sn9c20x_modes[i].height;
	return i;
}

/**
 * @brief Set resolution inside sn9c20x chip
 *
 * @author Brian Johnson
 *
 * @param dev Pointer to the device
 * @param width Width
 * @param height Height
 *
 * @return 0
 *
 */
int sn9c20x_set_resolution(struct usb_sn9c20x *dev,
	int width, int height)
{
	int ret;
	__u8 scale;
	__u8 window[6];
	__u8 clrwindow[5];
	struct sn9c20x_video_mode *mode;

	ret = sn9c20x_get_closest_resolution(dev, &width, &height);

	mode = &sn9c20x_modes[ret];

	dev->vsettings.format.width = mode->width;
	dev->vsettings.format.height = mode->height;

	clrwindow[0] = 0;
	clrwindow[1] = mode->width  >> 2;
	clrwindow[2] = 0;
	clrwindow[3] = mode->height >> 1;
	clrwindow[4] = ((mode->width >> 10) & 0x01) |
				((mode->height >> 8) & 0x06);

	scale = mode->scale;

	window[0] = (mode->window[0] + dev->camera.hstart) & 0xff;
	window[1] = (mode->window[0] + dev->camera.hstart) >> 8;
	window[2] = (mode->window[1] + dev->camera.vstart) & 0xff;
	window[3] = (mode->window[1] + dev->camera.vstart) >> 8;
	window[4] = mode->window[2] >> 4;
	window[5] = mode->window[3] >> 3;

	if (dev->camera.set_sxga_mode) {
		if (width > 640 && height > 480) {
			dev->camera.set_sxga_mode(dev, true);
			scale |= 0xc0;
			UDIA_DEBUG("Set Sensor to SXGA\n");
		} else {
			dev->camera.set_sxga_mode(dev, false);
			scale |= 0x80;
			UDIA_DEBUG("Set Sensor to VGA\n");
		}
	}

	usb_sn9c20x_control_write(dev, 0x10fb, clrwindow, 5);
	usb_sn9c20x_control_write(dev, 0x1180, window, 6);
	usb_sn9c20x_control_write(dev, SN9C20X_SCALE, &scale, 1);

	UDIA_DEBUG("Set mode [%dx%d]\n", mode->width, mode->height);

	return 0;
}


int sn9c20x_set_format(struct usb_sn9c20x *dev, __u32 format)
{
	int i;
	int ret = -EINVAL;
	for (i = 0; i < SN9C20X_N_FMTS; i++) {
		if (sn9c20x_fmts[i].pix_fmt == format) {
			dev->vsettings.format.bytesperline =
				dev->vsettings.format.width *
				sn9c20x_fmts[i].depth / 8;
			dev->vsettings.format.sizeimage =
				dev->vsettings.format.height *
				dev->vsettings.format.bytesperline;
			dev->vsettings.format.pixelformat = format;
			dev->vsettings.format.colorspace = V4L2_COLORSPACE_SRGB;
			dev->vsettings.format.priv = 0;
			sn9c20x_fmts[i].set_format(dev);
			ret = 0;
			break;
		}
	}
	return ret;
}

void sn9c20x_set_raw(struct usb_sn9c20x *dev)
{
	__u8 value;
	value = 0x2d;
	usb_sn9c20x_control_write(dev, 0x10e0, &value, 1);
	UDIA_INFO("Using raw output format\n");
}

void sn9c20x_set_jpeg(struct usb_sn9c20x *dev)
{
	__u8 value;
	value = 0x2c;
	usb_sn9c20x_control_write(dev, 0x10e0, &value, 1);
	UDIA_INFO("Using jpeg output format\n");
}

void sn9c20x_set_yuv420(struct usb_sn9c20x *dev)
{
	__u8 value;
	value = 0x2f;
	usb_sn9c20x_control_write(dev, 0x10e0, &value, 1);
	UDIA_INFO("Using yuv420 output format\n");
}

void sn9c20x_set_yuv422(struct usb_sn9c20x *dev)
{
	__u8 value;
	value = 0x2e;
	usb_sn9c20x_control_write(dev, 0x10e0, &value, 1);
	UDIA_INFO("Using yuv422 output format\n");
}

/**
 * @brief This function initializes the SN9C20x bridge,
 * these are the bare minimum writes that must be done for
 * the bridge to work correctly.
 *
 * @author Neekhil
 *
 * @param dev Pointer to the device
 *
 * @return Zero for success or error value
 *
 */
int sn9c20x_initialize(struct usb_sn9c20x *dev)
{
	int ret, i;

	__u16 reg;
	__u8 value;

	__u16 regs[][2] = {
		{0x1000, 0x78},
		{0x1001, 0x40},
		{0x1002, 0x1c},
		{0x1020, 0x80},
		{0x1061, 0x01},
		{0x1067, 0x40},
		{0x1068, 0x30},
		{0x1069, 0x20},
		{0x106a, 0x10},
		{0x106b, 0x08},
		{0x1188, 0x87},
		{0x11a1, 0x00},
		{0x11a2, 0x00},
		{0x11a3, 0x6a},
		{0x11a4, 0x50},
		{0x11ab, 0x00},
		{0x11ac, 0x00},
		{0x11ad, 0x50},
		{0x11ae, 0x3c},
		{0x118a, 0x04},
		{0x0395, 0x04},
		{0x11b8, 0x3a},
		{0x118b, 0x0e},
		{0x10f7, 0x05},
		{0x10f8, 0x14},
		{0x10fa, 0xff},
		{0x10f9, 0x00},
		{0x11ba, 0x0a},
		{0x11a5, 0x2d},
		{0x11a6, 0x2d},
		{0x11a7, 0x3a},
		{0x11a8, 0x05},
		{0x11a9, 0x04},
		{0x11aa, 0x3f},
		{0x11af, 0x28},
		{0x11b0, 0xd8},
		{0x11b1, 0x14},
		{0x11b2, 0xec},
		{0x11b3, 0x32},
		{0x11b4, 0xdd},
		{0x11b5, 0x32},
		{0x11b6, 0xdd},
		{0x10e0, 0x2c},
		{0x11bc, 0x40},
		{0x11bd, 0x01},
		{0x11be, 0xf0},
		{0x11bf, 0x00},
		{0x118c, 0x1f},
		{0x118d, 0x1f},
		{0x118e, 0x1f},
		{0x118f, 0x1f},
		{0x1180, 0x01},
		{0x1181, 0x00},
		{0x1182, 0x01},
		{0x1183, 0x00},
		{0x1184, 0x50},
		{0x1185, 0x80},
	};

	__u8 qtable1[64] = {
		0x0d, 0x08, 0x08, 0x0d, 0x08, 0x08, 0x0d, 0x0d,
		0x0d, 0x0d, 0x11, 0x0d, 0x0d, 0x11, 0x15, 0x21,
		0x15, 0x15, 0x11, 0x11, 0x15, 0x2a, 0x1d, 0x1d,
		0x19, 0x21, 0x32, 0x2a, 0x32, 0x32, 0x2e, 0x2a,
		0x2e, 0x2e, 0x36, 0x3a, 0x4b, 0x43, 0x36, 0x3a,
		0x47, 0x3a, 0x2e, 0x2e, 0x43, 0x5c, 0x43, 0x47,
		0x4f, 0x54, 0x58, 0x58, 0x58, 0x32, 0x3f, 0x60,
		0x64, 0x5c, 0x54, 0x64, 0x4b, 0x54, 0x58, 0x54
	};

	__u8 qtable2[64] = {
		0x0d, 0x11, 0x11, 0x15, 0x11, 0x15, 0x26, 0x15,
		0x15, 0x26, 0x54, 0x36, 0x2e, 0x36, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
		0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54
	};

	for (i = 0; i < ARRAY_SIZE(regs); i++) {
		reg = regs[i][0];
		value = regs[i][1];
		ret = usb_sn9c20x_control_write(dev, reg, &value, 1);
		if (unlikely(ret < 0)) {
			UDIA_INFO("Bridge Init Error (%d). line %d\n", ret, i);
			goto err;
		}
	}

	ret = usb_sn9c20x_control_write(dev, 0x1100, qtable1, 64);
	if (ret < 0)
		goto err;

	ret = usb_sn9c20x_control_write(dev, 0x1140, qtable2, 64);
	if (ret < 0)
		goto err;

#ifdef CONFIG_SN9C20X_EVDEV
	ret = usb_sn9c20x_control_read(dev, 0x1005, &dev->input_gpio, 1);
	if (ret < 0)
		goto err;

	dev->input_gpio = ~dev->input_gpio;
#endif

	dev->camera.set_contrast = sn9c20x_set_contrast;
	dev->camera.set_brightness = sn9c20x_set_brightness;
	dev->camera.set_hue = sn9c20x_set_color;
	dev->camera.set_saturation = sn9c20x_set_color;
	dev->camera.set_gamma = sn9c20x_set_gamma;
	dev->camera.set_sharpness = sn9c20x_set_sharpness;
	dev->camera.set_red_gain = sn9c20x_set_red_gain;
	dev->camera.set_blue_gain = sn9c20x_set_blue_gain;

	ret = sn9c20x_i2c_initialize(dev);
	if (ret < 0)
		goto err;

	ret = sn9c20x_initialize_sensor(dev);
	return ret;

err:
	UDIA_ERROR("Device Init failed (%d)!\n", ret);
	return ret;
}

int sn9c20x_reset_device(struct usb_sn9c20x *dev)
{
	if (sn9c20x_initialize(dev) < 0)
		return -EINVAL;

	sn9c20x_set_camera_control(dev, V4L2_CID_HFLIP,
				   dev->vsettings.hflip);
	sn9c20x_set_camera_control(dev, V4L2_CID_VFLIP,
				   dev->vsettings.vflip);
	sn9c20x_set_camera_control(dev, V4L2_CID_GAIN,
				   dev->vsettings.gain);
	sn9c20x_set_camera_control(dev, V4L2_CID_BRIGHTNESS,
				   dev->vsettings.brightness);
	sn9c20x_set_camera_control(dev, V4L2_CID_CONTRAST,
				   dev->vsettings.contrast);
	sn9c20x_set_camera_control(dev, V4L2_CID_HUE,
				   dev->vsettings.hue);
	sn9c20x_set_camera_control(dev, V4L2_CID_SATURATION,
				   dev->vsettings.colour);
	sn9c20x_set_camera_control(dev, V4L2_CID_GAMMA,
				   dev->vsettings.gamma);
	sn9c20x_set_camera_control(dev, V4L2_CID_SHARPNESS,
				   dev->vsettings.sharpness);
	sn9c20x_set_camera_control(dev, V4L2_CID_RED_BALANCE,
				   dev->vsettings.red_gain);
	sn9c20x_set_camera_control(dev, V4L2_CID_BLUE_BALANCE,
				   dev->vsettings.blue_gain);
	sn9c20x_set_camera_control(dev, V4L2_CID_EXPOSURE_AUTO,
				   dev->vsettings.auto_exposure);
	sn9c20x_set_camera_control(dev, V4L2_CID_AUTOGAIN,
				   dev->vsettings.auto_gain);
	sn9c20x_set_camera_control(dev, V4L2_CID_AUTO_WHITE_BALANCE,
				   dev->vsettings.auto_whitebalance);
	sn9c20x_set_camera_control(dev, V4L2_CID_EXPOSURE,
				   dev->vsettings.exposure);

	sn9c20x_set_resolution(dev, dev->vsettings.format.width,
			       dev->vsettings.format.height);

	sn9c20x_set_format(dev, dev->vsettings.format.pixelformat);

	return 0;
}
