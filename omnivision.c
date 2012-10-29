/**
 * @file omnivision.c
 * @date 2008-10-27
 *
 * @brief Common control functions for Omnivision Image Sensors.
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
#include "omnivision.h"

struct sn9c20x_i2c_regs ov7660_init[] = {
	/* System CLK selection, to get a higher Frame Rate */
	{OV7660_CTL_COM5, 0x80},
	/* OV7660 Wakeup */
	/* COM4 is Reserved : using default value 0x40 OR windows driver value 0x08 */
	{OV7660_CTL_COM4, 0x08},
	/* Enable HREF at optical black, Use optical black line as BLC signal
	    Reset all timing when format changes, Enable ADBLC option */
	{OV7660_CTL_COM6, 0xc3},
	/* windows 0x00, default 0x00, trying 0x60 to enable CCIR656 format */
	{OV7660_CTL_COM1, 0xc3},
	/* default is 0x40, windows sets it to 0x00 */
	{OV7660_CTL_AECH, 0x40},
	/* default is 0x00, windows sets it to 0x40 */
	{OV7660_CTL_CLKRC, 0x40},
	/* Set O/P format - RGB Selection, Set O/P format Raw RGB */
	{OV7660_CTL_COM7, 0x05},
	/* default is 0x8f, windows used 0xf8 */
	/* Enable fast AGC/AEC algorithm, AEC - Step size limit = 1/16 x AEC */
	/* Banding & Reserved are disabled. AGC, AEC enabled, 0x85 */
	{OV7660_CTL_COM8, 0xba},
	/* video appears jagged w/o these  ADC writes */
	{OV7660_CTL_ADC, 0x0f},
	{OV7660_CTL_ACOM, 0x02},
	{OV7660_CTL_OFON, 0x43},
	/* video appears jagged w/o this write */
	/* Default 0x0c sets format to uYvY, Windows driver 0x00 sets format to YuYv */
	{OV7660_CTL_TSLB, 0x00},
	/* Not doing this write makes the video look green */
	/* Manual Banding Filter MSB , set B & R channel pre-gain */
	{OV7660_CTL_HV, 0x90},
	/*  No video stream w/o these ADVFL/ADVFH write totally black */
	{OV7660_CTL_ADVFL, 0xf6},
	{OV7660_CTL_ADVFH, 0x0b},
	/* Setting BLUE to 0x78; RED to 0x78 to get natural colors in artificial light */
	{OV7660_CTL_BLUE, 0x78},
	/* Setting RED to 0x50 to get natural colors in natural light */
	{OV7660_CTL_RED,  0x50},
	{0xff, 0xff},
};

struct sn9c20x_i2c_regs ov7670_init[] = {
	/* Phase 1 */
	{OV7670_CTL_COM7, 0x80},
	{OV7670_CTL_CLKRC, 0x80},
	{OV7670_CTL_TSLB, 0x04},
	{OV7670_CTL_COM7, 0x01},
	{OV7670_CTL_HREF, 0xb6},
	{OV7670_CTL_VREF, 0x0a},
	{OV7670_CTL_COM3, 0x00},
	{OV7670_CTL_COM14, 0x00},
	{OV7670_CTL_SCALING_XSC, 0x3a},
	{OV7670_CTL_SCALING_YSC, 0x35},
	{OV7670_CTL_SCALING_DCWCTR, 0x11},
	{OV7670_CTL_SCALING_PCLK_DIV, 0xf0},
	{OV7670_CTL_SCALING_PCLK_DELAY, 0x02},
	{OV7670_CTL_COM8, 0xe0},
	{OV7670_CTL_GAIN, 0x00},
	{OV7670_CTL_AECH, 0x00},
	{OV7670_CTL_COM4, 0x40},
	{OV7670_CTL_COM9, 0x08},
	{OV7670_CTL_BD50MAX, 0x05},
	{OV7670_CTL_BD60MAX, 0x07},
	{OV7670_CTL_AEW, 0x95},
	{OV7670_CTL_AEB, 0x33},
	{OV7670_CTL_VPT, 0xe3},
	{OV7670_CTL_HAECC1, 0x75},
	{OV7670_CTL_HAECC2, 0x65},
	{0xa1, 0x0b},
	{OV7670_CTL_HAECC3, 0xd8},
	{OV7670_CTL_HAECC4, 0xd8},
	{OV7670_CTL_HAECC5, 0xf0},
	{OV7670_CTL_HAECC6, 0x90},
	{OV7670_CTL_HAECC7, 0x94},
	{OV7670_CTL_COM8, 0xe5},
	{OV7670_CTL_COM5, 0x61},
	{OV7670_CTL_COM6, 0x4b},
	{0x16, 0x02},
	{OV7670_CTL_MVFP, 0x27},
	{OV7670_CTL_ADCCTR1, 0x02},
	{OV7670_CTL_ADCCTR2, 0x91},
	{0x29, 0x07},
	{OV7670_CTL_CHLF, 0x0b},
	{0x35, 0x0b},
	{OV7670_CTL_ADC, 0x1d},
	{OV7670_CTL_ACOM, 0x71},
	{OV7670_CTL_OFON, 0x2a},
	{OV7670_CTL_COM12, 0x78},
	{0x4d, 0x40},
	{0x4e, 0x20},
	{OV7670_CTL_GFIX, 0x00},
	{OV7670_CTL_REG74, 0x19},
	{0x8d, 0x4f},
	{0x8e, 0x00},
	{0x8f, 0x00},
	{0x90, 0x00},
	{0x91, 0x00},
	{0x96, 0x00},
	{0x9a, 0x80},
	{0xb0, 0x84},
	{OV7670_CTL_ABLC1, 0x0c},
	{0xb2, 0x0e},
	{OV7670_CTL_THL_ST, 0x82},
	{0xb8, 0x0a},
	{OV7670_CTL_AWBC1, 0x0a},
	{OV7670_CTL_AWBC2, 0xf0},
	{OV7670_CTL_AWBC3, 0x20},
	{OV7670_CTL_AWBC4, 0x7d},
	{OV7670_CTL_AWBC5, 0x29},
	{OV7670_CTL_AWBC6, 0x4a},
	{0x59, 0x8c},
	{0x5a, 0xa5},
	{0x5b, 0xde},
	{0x5c, 0x96},
	{0x5d, 0x66},
	{0x5e, 0x10},
	{OV7670_CTL_AWBCTR3, 0x0a},
	{OV7670_CTL_AWBCTR2, 0x55},
	{OV7670_CTL_AWBCTR1, 0x11},
	{OV7670_CTL_AWBCTR0, 0x9e},
	{OV7670_CTL_GGAIN, 0x40},
	{OV7670_CTL_BLUE, 0x40},
	{OV7670_CTL_RED, 0x40},
	{OV7670_CTL_COM8, 0xe7},
	{OV7670_CTL_MTX1, 0x6e},
	{OV7670_CTL_MTX2, 0x70},
	{OV7670_CTL_MTX3, 0x02},
	{OV7670_CTL_MTX4, 0x1d},
	{OV7670_CTL_MTX5, 0x56},
	{OV7670_CTL_MTX6, 0x73},
	{OV7670_CTL_BRIGHT, 0x0a},
	{OV7670_CTL_CONTRAS, 0x55},
	{OV7670_CTL_CONTRAS_CENTER, 0x80},
	{OV7670_CTL_MTXS, 0x9e},
	{OV7670_CTL_COM16, 0x08},
	{OV7670_CTL_EDGE, 0x02},
	{OV7670_CTL_REG75, 0x03},
	{OV7670_CTL_REG76, 0x63},
	{OV7670_CTL_DNSTH, 0x04},
	{OV7670_CTL_REG77, 0x06},
	{OV7670_CTL_COM13, 0x02},
	{OV7670_CTL_REG4B, 0x09},
	{OV7670_CTL_SATCTR, 0x30},
	{OV7670_CTL_COM16, 0x08},
	{OV7670_CTL_CONTRAS, 0x48},
	{OV7670_CTL_ARBLM, 0x11},
	{OV7670_CTL_NT_CTRL, 0x88},
	{0x96, 0x00},
	{0x97, 0x30},
	{0x98, 0x20},
	{0x99, 0x30},
	{0x9a, 0x84},
	{0x9b, 0x29},
	{0x9c, 0x03},
	{OV7670_CTL_BD50ST, 0x99},
	{OV7670_CTL_BD60ST, 0x7f},
	{0x78, 0x04},
	{0x79, 0x01},
	{0xc8, 0xf0},
	{0x79, 0x0f},
	{0xc8, 0x00},
	{0x79, 0x10},
	{0xc8, 0x7e},
	{0x79, 0x0a},
	{0xc8, 0x80},
	{0x79, 0x0b},
	{0xc8, 0x01},
	{0x79, 0x0c},
	{0xc8, 0x0f},
	{0x79, 0x0d},
	{0xc8, 0x20},
	{0x79, 0x09},
	{0xc8, 0x80},
	{0x79, 0x02},
	{0xc8, 0xc0},
	{0x79, 0x03},
	{0xc8, 0x40},
	{0x79, 0x05},
	{0xc8, 0x30},
	{0x79, 0x26},
	{OV7670_CTL_LCC1, 0x20},
	{OV7670_CTL_LCC2, 0x00},
	{OV7670_CTL_LCC3, 0x06},
	{OV7670_CTL_LCC4, 0x00},
	{OV7670_CTL_LCC5, 0x05},
	{OV7670_CTL_LCC6, 0x05},
	{OV7670_CTL_LCC7, 0x0a},
	{OV7670_CTL_HSTART, 0x13},
	{OV7670_CTL_HSTOP, 0x01},
	{OV7670_CTL_VSTRT, 0x02},
	{OV7670_CTL_VSTOP, 0x7a},
	{OV7670_CTL_AWBC4, 0x59},
	{OV7670_CTL_AWBC5, 0x30},
	{OV7670_CTL_MTXS, 0x9a},
	{0x59, 0x84},
	{0x5a, 0x91},
	{0x5b, 0x57},
	{0x5c, 0x75},
	{0x5d, 0x6d},
	{0x5e, 0x13},
	{OV7670_CTL_LCC3, 0x07},
	{OV7670_CTL_LCC6, 0x07},
	{OV7670_CTL_LCC7, 0x0d},
	{OV7670_CTL_HAECC3, 0xdf},
	{OV7670_CTL_HAECC4, 0xdf},
	{OV7670_CTL_AWBC6, 0x4d},
	{OV7670_CTL_MTX3, 0x00},
	/* Phase 2 */
	{OV7670_CTL_DBLV, 0x0a},
	{OV7670_CTL_CLKRC, 0x80},
	{OV7670_CTL_EXHCH, 0x00},
	{OV7670_CTL_EXHCL, 0x00},
	{OV7670_CTL_DM_LNL, 0x00},
	{OV7670_CTL_DM_LNH, 0x00},
	{OV7670_CTL_BRIGHT, 0x0a},
	{OV7670_CTL_CONTRAS, 0x60},
	{OV7670_CTL_MTX1, 0x6e},
	{OV7670_CTL_MTX1 + 1, 0x70},
	{OV7670_CTL_MTX1 + 2, 0x00},
	{OV7670_CTL_MTX1 + 3, 0x1d},
	{OV7670_CTL_MTX5, 0x56},
	{OV7670_CTL_MTX5 + 1, 0x73},
	{OV7670_CTL_MTXS, 0x9a},
	{OV7670_CTL_MTX1, 0x6e},
	{OV7670_CTL_MTX1 + 1, 0x70},
	{OV7670_CTL_MTX1 + 2, 0x00},
	{OV7670_CTL_MTX1 + 3, 0x1d},
	{OV7670_CTL_MTX5, 0x56},
	{OV7670_CTL_MTX5 + 1, 0x73},
	{OV7670_CTL_MTXS, 0x9a},
	{OV7670_CTL_EDGE, 0x01},
	{OV7670_CTL_GAM1, 0x03},
	{OV7670_CTL_GAM1 + 1, 0x09},
	{OV7670_CTL_GAM1 + 2, 0x16},
	{OV7670_CTL_GAM1 + 3, 0x38},
	{OV7670_CTL_GAM5, 0x47},
	{OV7670_CTL_GAM5 + 1, 0x53},
	{OV7670_CTL_GAM5 + 2, 0x5e},
	{OV7670_CTL_GAM5 + 3, 0x6a},
	{OV7670_CTL_GAM9, 0x74},
	{OV7670_CTL_GAM9 + 1, 0x80},
	{OV7670_CTL_GAM9 + 2, 0x8c},
	{OV7670_CTL_GAM9 + 3, 0x9b},
	{OV7670_CTL_GAM13, 0xb2},
	{OV7670_CTL_GAM13 + 1, 0xcc},
	{OV7670_CTL_GAM13 + 2, 0xe5},
	{OV7670_CTL_SLOP, 0x24},
	{OV7670_CTL_COM11, 0x00},
	{OV7670_CTL_HAECC1, 0x76},
	{OV7670_CTL_HAECC1 + 1, 0x65},
	{OV7670_CTL_COM8, 0xe2},
	/* Phase 3 */
	{OV7670_CTL_DBLV, 0x0a},
	{OV7670_CTL_CLKRC, 0x80},
	{OV7670_CTL_EXHCH, 0x00},
	{OV7670_CTL_EXHCL, 0x00},
	{OV7670_CTL_DM_LNL, 0x00},
	{OV7670_CTL_DM_LNH, 0x00},
	{0xff, 0xff},
};

/**
 * @var ov9650_init
 * @brief Addresses and values for the initialization of ov965x sensors
 *
 */
struct sn9c20x_i2c_regs ov9650_init[] = {
	{OV965X_CTL_COM7, OV965X_COM7_SCCB_RESET},
	{OV965X_CTL_GAIN, 0x00},
	{OV965X_CTL_BLUE, 0x78},
	{OV965X_CTL_RED, 0x78},
	{OV965X_CTL_VREF, OV965X_VREF_VSTOP_LOW3(0x06) |
			  OV965X_VREF_VSTART_LOW3(0x06)},
	{OV965X_CTL_COM1, 0x03},
	{OV965X_CTL_BAVE, 0x00}, /* default */
	{OV965X_CTL_GEAVE, 0x00}, /* default */
	{OV965X_CTL_RAVE, 0x00}, /* default */
	{OV965X_CTL_COM2, OV965X_COM2_OUTPUT_DRIVE_CAP_2X},
	{OV965X_CTL_COM3, 0x00},
	{OV965X_CTL_COM4, 0x00},
	{OV965X_CTL_COM5, OV965X_COM5_15FPS_48MHZ_RGB | 0x20},
	{OV965X_CTL_COM6, OV965X_COM6_TIMING_RESET_ON_FMT_CHANGE | 0x50},
	{OV965X_CTL_AECH, 0x7c},
	{OV965X_CTL_CLKRC, OV965X_CLKRC_DBL_CLK_ENABLE},
	{OV965X_CTL_COM7, OV965X_COM7_OUTPUT_VGA | OV965X_COM7_OUTPUT_RAW_RGB},
	{OV965X_CTL_COM8, OV965X_COM8_FAST_AGC_AEC |
			  OV965X_COM8_AEC_STEP_SIZE_NOLIMIT |
			  OV965X_COM8_AWB_ENABLE},
	{OV965X_CTL_COM9, OV965X_COM9_MAX_AGC_4X |
			  OV965X_COM9_RELAX_EXPOSURE_TIMING |
			  OV965X_COM9_DROP_VSYNC_ON_FRAME_DROP |
			  OV965X_COM9_DROP_FRAME_ON_BIG_AEC},
	{OV965X_CTL_COM10, 0x00},
	{0x16, 0x07}, /* reserved */
	{OV965X_CTL_HSTART, 0x24},
	{OV965X_CTL_HSTOP, 0xc5},
	{OV965X_CTL_VSTRT, 0x00},
	{OV965X_CTL_VSTOP, 0x3c},
	{OV965X_CTL_PSHIFT, 0x00}, /* default */
	{OV965X_CTL_MVFP, 0x04},
	{OV965X_CTL_LAEC, 0x00}, /* reserved */
	{OV965X_CTL_AEW, 0x78}, /* default */
	{OV965X_CTL_AEB, 0x68}, /* default */
	{OV965X_CTL_VPT, 0xd4}, /* default */
	{OV965X_CTL_BBIAS, OV965X_BIAS_SUBTRACT}, /* default */
	{OV965X_CTL_GbBIAS, OV965X_BIAS_SUBTRACT}, /* default */
	{OV965X_CTL_Gr_COM, OV965X_Gr_COM_BYPASS_ANALOG_BLC |
			    OV965X_Gr_COM_BYPASS_REGULATOR},
	{OV965X_CTL_EXHCH, 0x00}, /* default */
	{OV965X_CTL_EXHCL, 0x00}, /* default */
	{OV965X_CTL_RBIAS, OV965X_BIAS_SUBTRACT}, /* default */
	{OV965X_CTL_ADVFL, 0x00}, /* default */
	{OV965X_CTL_ADVFH, 0x00}, /* default */
	{OV965X_CTL_YAVE, 0x00}, /* default */
	{OV965X_CTL_HSYST, 0x08}, /* default */
	{OV965X_CTL_HSYEN, 0x30}, /* default */
	{OV965X_CTL_HREF, OV965X_HREF_EDGE_OFT_TO_DATA_OUT(2) |
			  OV965X_HREF_HSTOP_LOW3(0) |
			  OV965X_HREF_HSTART_LOW3(4)},
	{OV965X_CTL_CHLF, 0xe2}, /* reserved */
	{OV965X_CTL_ARBLM, 0xbf}, /* reserved */
	{0x35, 0x81}, /* reserved */
	{0x36, 0xf9}, /* reserved */
	{OV965X_CTL_ADC, 0x00}, /* reserved */
	{OV965X_CTL_ACOM, 0x93}, /* reserved */
	{OV965X_CTL_OFON, 0x50},
	{OV965X_CTL_TSLB, OV965X_TSLB_DIGITAL_BLC_ENABLE},
	{OV965X_CTL_COM11, OV965X_COM11_MANUAL_BANDING_FILTER},
	{OV965X_CTL_COM12, 0x73},
	{OV965X_CTL_COM13, OV965X_COM13_ENABLE_COLOR_MATRIX |
			   OV965X_COM13_DELAY_Y_CHANNEL |
			   OV965X_COM13_OUTPUT_DELAY(1)},
	{OV965X_CTL_COM14, OV965X_COM14_YUV_EDGE_ENHANCE |
			   OV965X_COM14_EDGE_ENHANCE_FACTOR_DBL | 0x0b},
	{OV965X_CTL_EDGE, OV965X_EDGE_EDGE_ENHANCE_LOW4(8) |
			  OV965X_EDGE_EDGE_ENHANCE_FACTOR(8)},
	{OV965X_CTL_COM15, OV965X_COM15_OUTPUT_RANGE_O0_TO_FF | 0x01},
	{OV965X_CTL_COM16, 0x00},
	{OV965X_CTL_COM17, 0x08},
	{OV965X_CTL_MANU, 0x80}, /* default */
	{OV965X_CTL_MANV, 0x80}, /* default */
	{OV965X_CTL_HV, 0x40},
	{OV965X_CTL_MBD, 0x00}, /* default */
	{OV965X_CTL_DBLV, 0x0a}, /* reserved */
	{OV965X_CTL_COM21, 0x06}, /* reserved */
	{OV965X_CTL_COM22, 0x20},
	{OV965X_CTL_COM23, 0x00}, /* default */
	{OV965X_CTL_COM24, 0x00}, /* reserved */
	{OV965X_CTL_DBLC1, 0xdf},
	{OV965X_CTL_DM_LNL, 0x00}, /* default */
	{OV965X_CTL_DM_LNH, 0x00}, /* default */
	{0x94, 0x88}, /* reserved */
	{0x95, 0x88}, /* reserved */
	{0x96, 0x04}, /* reserved */
	{OV965X_CTL_AECHM, 0x00},
	{OV965X_CTL_COM26, 0x80}, /* reserved */
	{0xa8, 0x80}, /* reserved */
	{0xa9, 0xb8}, /* reserved */
	{0xaa, 0x92}, /* reserved */
	{0xab, 0x0a}, /* reserved */
	{0xff, 0xff},
};

/**
 * @var soi968_init
 * @brief Addresses and values for the initialization of SOI968 sensors
 * @author Josua Grawitter
 *
 */
struct sn9c20x_i2c_regs soi968_init[] = {
	/* reset all registers */
	{0x12, 0x80},
	/* stop resetting */
	{0x12, 0x00},
	/* snapshot mode: off */
	{0x0c, 0x00},
	/* enable offset adjustment,
	 * full data range,
	 * analogue2digital control black-level calibration
	 */
	{0x0f, 0x1f},
	/* Clock: internal PLL on */
	{0x11, 0x80},
	/* Analoge Black-level calibration off , no anaolgue gain */
	{0x38, 0x52},
	/* reserved */
	{0x1e, 0x00},
	/* special system settings (voltage, analogue2digital, ...) */
	{0x33, 0x08},
	{0x35, 0x8c},
	{0x36, 0x0c},
	{0x37, 0x04},
	/*  next 7 are unknown/reserved */
	{0x45, 0x04},
	{0x47, 0xff},
	{0x3e, 0x00},
	{0x3f, 0x00},
	{0x3b, 0x20},
	{0x3a, 0x96},
	{0x3d, 0x0a},
	/* disable banding filter in dark environment,
	 * VSYNC is dropped when framerate is dropped,
	 * drop frmaes when exposure out of tolerance,
	 * unfreeze exposure and gain values
	 */
	{0x14, 0x4e},
	/* AEC, AGC, AWB disabled; fast AEC */
	{0x13, 0x8a},
	/* output: VGA, master mode */
	{0x12, 0x40},
	/* set HSTART, HSTOP, VSTART and VSTOP */
	{0x17, 0x13},
	{0x18, 0x63},
	{0x19, 0x01},
	{0x1a, 0x79},
	{0x32, 0x24}, /* LSB for all four */
	/* AWB update threshold,
	 * blue and red gain LSB
	 */
	{0x03, 0x00},
	/* CLock: internal PLL off */
	{0x11, 0x40},
	/* Line interval adjustment */
	{0x2a, 0x10},
	{0x2b, 0xe0},
	/* AEC MSB */
	{0x10, 0x32},
	/* gain - default*/
	{0x00, 0x00},
	/* blue and red gain - default*/
	{0x01, 0x80},
	{0x02, 0x80},
	{0xff, 0xff},
};

struct sn9c20x_i2c_regs ov9655_init[] = {
	{0x12, 0x80}, {0x12, 0x01}, {0x0d, 0x00}, {0x0e, 0x61},
	{0x11, 0x80}, {0x13, 0xba}, {0x14, 0x3e}, {0x16, 0x24},
	{0x1e, 0x04}, {0x1e, 0x04}, {0x1e, 0x04}, {0x27, 0x08},
	{0x28, 0x08}, {0x29, 0x15}, {0x2c, 0x08}, {0x32, 0xbf},
	{0x34, 0x3d}, {0x35, 0x00}, {0x36, 0xf8}, {0x38, 0x12},
	{0x39, 0x57}, {0x3a, 0x00}, {0x3b, 0xcc}, {0x3c, 0x0c},
	{0x3d, 0x19}, {0x3e, 0x0c}, {0x3f, 0x01}, {0x41, 0x40},
	{0x42, 0x80}, {0x45, 0x46}, {0x46, 0x62}, {0x47, 0x2a},
	{0x48, 0x3c}, {0x4a, 0xf0}, {0x4b, 0xdc}, {0x4c, 0xdc},
	{0x4d, 0xdc}, {0x4e, 0xdc}, {0x69, 0x02}, {0x6c, 0x04},
	{0x6f, 0x9e}, {0x70, 0x05}, {0x71, 0x78}, {0x77, 0x02},
	{0x8a, 0x23}, {0x8c, 0x0d}, {0x90, 0x7e}, {0x91, 0x7c},
	{0x9f, 0x6e}, {0xa0, 0x6e}, {0xa5, 0x68}, {0xa6, 0x60},
	{0xa8, 0xc1}, {0xa9, 0xfa}, {0xaa, 0x92}, {0xab, 0x04},
	{0xac, 0x80}, {0xad, 0x80}, {0xae, 0x80}, {0xaf, 0x80},
	{0xb2, 0xf2}, {0xb3, 0x20}, {0xb5, 0x00}, {0xb6, 0xaf},
	{0xbb, 0xae}, {0xbc, 0x44}, {0xbd, 0x44}, {0xbe, 0x3b},
	{0xbf, 0x3a}, {0xc0, 0xe2}, {0xc1, 0xc8}, {0xc2, 0x01},
	{0xc4, 0x00}, {0xc6, 0x85}, {0xc7, 0x81}, {0xc9, 0xe0},
	{0xca, 0xe8}, {0xcc, 0xd8}, {0xcd, 0x93}, {0x12, 0x61},
	{0x36, 0xfa}, {0x8c, 0x8d}, {0xc0, 0xaa}, {0x69, 0x0a},
	{0x03, 0x12}, {0x17, 0x14}, {0x18, 0x00}, {0x19, 0x01},
	{0x1a, 0x3d}, {0x32, 0xbf}, {0x11, 0x80}, {0x2a, 0x10},
	{0x2b, 0x0a}, {0x92, 0x00}, {0x93, 0x00}, {0x1e, 0x04},
	{0x1e, 0x04}, {0x10, 0x7c}, {0x04, 0x03}, {0xa1, 0x00},
	{0x2d, 0x00}, {0x2e, 0x00}, {0x00, 0x00}, {0x01, 0x80},
	{0x02, 0x80}, {0x12, 0x61}, {0x36, 0xfa}, {0x8c, 0x8d},
	{0xc0, 0xaa}, {0x69, 0x0a}, {0x03, 0x12}, {0x17, 0x14},
	{0x18, 0x00}, {0x19, 0x01}, {0x1a, 0x3d}, {0x32, 0xbf},
	{0x11, 0x80}, {0x2a, 0x10}, {0x2b, 0x0a}, {0x92, 0x00},
	{0x93, 0x00}, {0x04, 0x01}, {0x10, 0x1f}, {0xa1, 0x00},
	{0x00, 0x0a}, {0xa1, 0x00}, {0x10, 0x5d}, {0x04, 0x03},
	{0x00, 0x01}, {0xa1, 0x00}, {0x10, 0x7c}, {0x04, 0x03},
	{0x00, 0x03}, {0x00, 0x0a}, {0x00, 0x10}, {0x00, 0x13},
	{0xff, 0xff},
};

/**
 * @brief OV7670 Auto-Flip
 *
 * @param dev Pointer to the device
 * @param vflip Flag to indicate whether or not Camera is currently flipped
 *
 * @return Zero (success) or negative (USB-error value)
 *
 */
int ov7670_auto_flip(struct usb_sn9c20x *dev, __u8 vflip)
{
	int ret;
	__u8 buf[2];

	ret = sn9c20x_read_i2c_data(dev, 1,
			OV7670_CTL_MVFP, buf);
	if (ret < 0)
		return ret;

	if (vflip == 0)
		buf[0] = buf[0] & (0xff ^ OV7670_VFLIP_BIT);
	else
		buf[0] = buf[0] | OV7670_VFLIP_BIT;
	ret = sn9c20x_write_i2c_data(dev, 1,
			OV7670_CTL_MVFP, buf);

	return ret;
}

void ov965x_set_sxga_mode(struct usb_sn9c20x *dev, bool sxga)
{
	__u8 value;
	__u8 sxga_hstart[4] = {0x1b, 0xbc, 0x01, 0x82};
	__u8 vga_hstart[4] = {0x24, 0xC5, 0x00, 0x3C};

	if (sxga) {
		sn9c20x_write_i2c_data(dev, 4, 0x17, sxga_hstart);
		sn9c20x_read_i2c_data(dev, 1, 0x12, &value);
		value = value & 0x7;
		sn9c20x_write_i2c_data(dev, 1, 0x12, &value);
	} else {
		sn9c20x_write_i2c_data(dev, 4, 0x17, vga_hstart);
		sn9c20x_read_i2c_data(dev, 1, 0x12, &value);
		value = (value & 0x7) | 0x40;
		sn9c20x_write_i2c_data(dev, 1, 0x12, &value);
	}
}

/**
 * @brief Set hflip and vflip in ov965x sensors
 *
 * @param dev Pointer to device structure
 *
 * @returns 0 or negative error code
 *
 */
int ov965x_set_hvflip(struct usb_sn9c20x *dev)
{
	int ret;
	__u8 value;
	__u8 tslb;
	ret = sn9c20x_read_i2c_data(dev, 1, OV965X_CTL_MVFP, &value);
	if (ret < 0)
		return ret;

	if (dev->vsettings.hflip)
		value |= OV965X_MVFP_MIRROR;
	else
		value &= ~OV965X_MVFP_MIRROR;

	if (dev->vsettings.vflip) {
		value |= OV965X_MVFP_VFLIP;
		tslb = 0x49;
	} else {
		value &= ~OV965X_MVFP_VFLIP;
		tslb = 0x01;
	}

	ret = sn9c20x_write_i2c_data(dev, 1, OV965X_CTL_MVFP, &value);
	ret = sn9c20x_write_i2c_data(dev, 1, OV965X_CTL_TSLB, &tslb);

	return ret;
}

/**
 * @brief Set exposure for omnivision sensors
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * The used registers do not show up the datasheets.
 *
 */
int ov_set_exposure(struct usb_sn9c20x *dev)
{
	int ret = 0;
	__u8 val[2] = {(dev->vsettings.exposure & 0x0F) << 4,
		dev->vsettings.exposure >> 4};

	ret = sn9c20x_write_i2c_data_ext(dev, 2, 0x2d, val, 0x1e);

	return ret;
}

/**
 * @brief Set autoexposure for omnivision sensors
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * @author Josua Grawitter
 *
 * For SOI968 sensor.
 */
int soi968_set_autoexposure(struct usb_sn9c20x *dev)
{
	__u8 buf[1];
	int ret = 0;

	ret = sn9c20x_read_i2c_data(dev, 1, 0x13, buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.auto_exposure) {
		buf[0] |= 0x01;
	} else {
		buf[0] &= ~0x01;
	}

	ret = sn9c20x_write_i2c_data(dev, 1, 0x13, buf);

	return ret;
}

/**
 * @brief Set autowhitebalance for omnivision sensors
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * @author Josua Grawitter
 *
 * For SOI968 sensor.
 */
int soi968_set_autowhitebalance(struct usb_sn9c20x *dev)
{
	__u8 buf[1];
	int ret = 0;

	ret = sn9c20x_read_i2c_data(dev, 1, 0x13, buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.auto_whitebalance)
		buf[0] |= 0x02;
	else
		buf[0] &= ~0x02;

	ret = sn9c20x_write_i2c_data(dev, 1, 0x13, buf);

	return ret;
}

/**
 * @brief Set autogain for omnivision sensors
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * @author Josua Grawitter
 *
 * For all Omnivision sensors and SOI968.
 */
int ov_set_autogain(struct usb_sn9c20x *dev)
{
	__u8 buf[1];
	int ret = 0;

	ret = sn9c20x_read_i2c_data(dev, 1, 0x13, buf);
	if (ret < 0)
		return ret;

	if (dev->vsettings.auto_gain == 1)
		buf[0] |= 0x04;
	else if (dev->vsettings.auto_gain == 0)
		buf[0] &= ~0x04;
	else
		return -EINVAL;

	ret = sn9c20x_write_i2c_data(dev, 1, 0x13, buf);

	return ret;
}

/**
 * @brief Set exposure for SOI968 sensors
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * @author Josua Grawitter
 *
 * For SOI968 sensors.
 */
int soi968_set_exposure(struct usb_sn9c20x *dev)
{
	int value, ret;
	int exposure = dev->vsettings.exposure;
	__u8 buf1, buf2, buf3;

	/* Read current value of the I2C-register
	 * containing exposure LSB:
	 */
	ret = sn9c20x_read_i2c_data(dev, 1, 0x04, &buf1);
	if (ret < 0) {
		UDIA_ERROR("Error: setting exposure failed: "
			"error while reading from I2C-register 0x04\n");
		return ret;
	}

	value = (exposure  * 0x08) & 0x7ff;
	buf2 = ((__u8) (value & 0x07)) | (buf1 & ~0x07);
	buf3 = (__u8) (value >> 3) & 0xff;

	/* Write new value to I2C-register 0x04: */
	ret = sn9c20x_write_i2c_data(dev, 1, 0x04, &buf2);
	if (ret < 0) {
		UDIA_ERROR("Error: setting exposure failed: "
			"error while writing to I2C-register 0x04\n");
		return ret;
	}

	/* Write new value to I2C-register 0x10: */
	ret = sn9c20x_write_i2c_data(dev, 1, 0x10, &buf3);
	if (ret < 0) {
		UDIA_ERROR("Error: setting exposure failed: "
			"error while writing to I2C-register 0x10\n");
		return ret;
	}

	return 0;
}

int soi968_set_gain(struct usb_sn9c20x *dev)
{
	int ret;
	__u8 buf = (dev->vsettings.gain >> 2) & 0x3f;

	ret = sn9c20x_write_i2c_data(dev, 1, 0x00, &buf);
	return ret;
}

/**
 * @brief Set gain for ov9650 sensor
 *
 * @param dev
 *
 * @returns 0 or negative error value
 *
 * @author Vasily Khoruzhick
 *
 */
int ov9650_set_gain(struct usb_sn9c20x *dev)
{
	__u8 buf[1];
	int ret = 0;

	buf[0] = (dev->vsettings.gain >> 1) & 0x0F;
	switch (dev->vsettings.gain >> 1 & 0xF0) {
	case 0x00:
		break;
	case 0x10:
		buf[0] |= 0x10;
		break;
	case 0x20:
		buf[0] |= 0x30;
		break;
	case 0x30:
		buf[0] |= 0x70;
		break;
	case 0x40:
		buf[0] |= 0xF0;
		break;
	default:
		buf[0] |= 0xFF;
		break;
	}
	ret |= sn9c20x_write_i2c_data(dev, 1, OV965X_CTL_GAIN, buf);

	return ret;
}

/**
 * @brief Detect whether the image for 6260 has to be flipped
 *
 * @param dev Pointer to device structure
 *
 * @returns 0 or negative error code
 *
 */
int ov7670_flip_detect(struct usb_sn9c20x *dev)
{
	const __u8 flip_bit = 0x01;
	int ret = 0;
	__u8 val;
	static __u8 flip_reg = flip_bit;
	__u8 vflip;

	ret = usb_sn9c20x_control_read(dev, 0x1009, &val, 1);
	if (ret < 0)
		return -EAGAIN;
	if (flip_reg != (val & flip_bit)) {
		if (val & flip_bit)
			vflip = 0;
		else
			vflip = 1;
		ret = ov7670_auto_flip(dev, vflip);
		flip_reg = (val & flip_bit);
	}

	return ret;
}

/**
 * @brief Detect whether the image for 624f has to be flipped
 *
 * @param dev Pointer to device structure
 *
 * @returns 0 or negative error code
 *
 */
int ov965x_flip_detect(struct usb_sn9c20x *dev)
{
	int ret = 0;
	__u8 val;

	ret = usb_sn9c20x_control_read(dev, 0x1009, &val, 1);
	if (ret < 0)
		return -EAGAIN;
	if (val & 0x01)
		dev->vsettings.vflip = 1;
	else
		dev->vsettings.vflip = 0;
	return ret;
}
