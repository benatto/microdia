/**
 * @file omnivision.h
 *
 * @author Neekhil
 *
 * @author Tiago Madeira
 *
 * @author Dave Neuer
 *
 * @date 2008-03
 *
 * @brief Register names, Hex Addresses and common functions for the
 * Omnivision Image Sensor chips.
 *
 * @note Copyright (C) Neekhil (OV7660)
 *
 * @note Copyright (C) Tiago Madeira (OV7670)
 *
 * @note Copyright (C) Dave Neuer (OV965x)
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

#ifndef OMNIVISION_H
#define OMNIVISION_H

#include "sn9c20x.h"

/* Register names & Hex Addresses for Omnivision OV7660 Image Sensor. */

#define OV7660_CTL_GAIN			0x00
#define OV7660_CTL_BLUE			0x01
#define OV7660_CTL_RED			0x02
#define OV7660_CTL_VREF			0x03
#define OV7660_CTL_COM1			0x04
#define OV7660_CTL_BAVE			0x05
#define OV7660_CTL_GEAVE		0x06
#define OV7660_CTL_AECHH		0x07
#define OV7660_CTL_RAVE			0x08
#define OV7660_CTL_COM2			0x09
#define OV7660_CTL_PID			0x0a
#define OV7660_CTL_VER			0x0b
#define OV7660_CTL_COM3			0x0c
#define OV7660_CTL_COM4			0x0d
#define OV7660_CTL_COM5			0x0e
#define OV7660_CTL_COM6			0x0f
#define OV7660_CTL_AECH			0x10
#define OV7660_CTL_CLKRC		0x11
#define OV7660_CTL_COM7			0x12
#define OV7660_CTL_COM8			0x13
#define OV7660_CTL_COM9			0x14
#define OV7660_CTL_COM10		0x15
/* RSVD 0x16 is Reserved */
#define OV7660_CTL_HSTART		0x17
#define OV7660_CTL_HSTOP		0x18
#define OV7660_CTL_VSTRT		0x19
#define OV7660_CTL_VSTOP		0x1a
#define OV7660_CTL_PSHFT		0x1b
#define OV7660_CTL_MIDH			0x1c
#define OV7660_CTL_MIDL			0x1d
#define OV7660_CTL_MVFP			0x1e
#define OV7660_CTL_LAEC			0x1f
#define OV7660_CTL_BOS			0x20
#define OV7660_CTL_GBOS			0x21
#define OV7660_CTL_GROS			0x22
#define OV7660_CTL_ROS			0x23
#define OV7660_CTL_AEW			0x24
#define OV7660_CTL_AEB			0x25
#define OV7660_CTL_VPT			0x26
#define OV7660_CTL_BBIAS		0x27
#define OV7660_CTL_GbBIAS		0x28
/* RSVD 0x29 is Reserved */
#define OV7660_CTL_EXHCH		0x2a
#define OV7660_CTL_EXHCL		0x2b
#define OV7660_CTL_RBIAS		0x2c
#define OV7660_CTL_ADVFL		0x2d
#define OV7660_CTL_ADVFH		0x2e
#define OV7660_CTL_YAVE			0x2f
#define OV7660_CTL_HSYST		0x30
#define OV7660_CTL_HSYEN		0x31
#define OV7660_CTL_HREF			0x32
#define OV7660_CTL_CHLF			0x33
#define OV7660_CTL_ARBLM		0x34
/* RSVD 0x35 is Reserved */
/* RSVD 0x36 is Reserved */
#define OV7660_CTL_ADC			0x37
#define OV7660_CTL_ACOM			0x38
#define OV7660_CTL_OFON			0x39
#define OV7660_CTL_TSLB			0x3a
#define OV7660_CTL_COM11		0x3b
#define OV7660_CTL_COM12		0x3c
#define OV7660_CTL_COM13		0x3d
#define OV7660_CTL_COM14		0x3e
#define OV7660_CTL_EDGE			0x3f
#define OV7660_CTL_COM15		0x40
#define OV7660_CTL_COM16		0x41
#define OV7660_CTL_COM17		0x42
/* RSVD 0x43 is Reserved */
/* RSVD 0x44 is Reserved */
/* RSVD 0x45 is Reserved */
/* RSVD 0x46 is Reserved */
/* RSVD 0x47 is Reserved */
/* RSVD 0x48 is Reserved */
/* RSVD 0x49 is Reserved */
/* RSVD 0x4a is Reserved */
/* RSVD 0x4b is Reserved */
/* RSVD 0x4c is Reserved */
/* RSVD 0x4d is Reserved */
/* RSVD 0x4e is Reserved */
#define OV7660_CTL_MTX1			0x4f
#define OV7660_CTL_MTX2			0x50
#define OV7660_CTL_MTX3			0x51
#define OV7660_CTL_MTX4			0x52
#define OV7660_CTL_MTX5			0x53
#define OV7660_CTL_MTX6			0x54
#define OV7660_CTL_MTX7			0x55
#define OV7660_CTL_MTX8			0x56
#define OV7660_CTL_MTX9			0x57
#define OV7660_CTL_MTXS			0x58
/* RSVD 0x59 is Reserved */
/* RSVD 0x60 is Reserved */
/* RSVD 0x61 is Reserved */
#define OV7660_CTL_LCC1			0x62
#define OV7660_CTL_LCC2			0x63
#define OV7660_CTL_LCC3			0x64
#define OV7660_CTL_LCC4			0x65
#define OV7660_CTL_LCC5			0x66
#define OV7660_CTL_MANU			0x67
#define OV7660_CTL_MANV			0x68
#define OV7660_CTL_HV			0x69
#define OV7660_CTL_GGAIN		0x6a
#define OV7660_CTL_DBLV			0x6b
/* 6c-7b GSP */
/* 7c-8a GST */
/* RSVD 0x8b is Reserved */
/* RSVD 0x8c is Reserved */
/* RSVD 0x8d is Reserved */
/* RSVD 0x8e is Reserved */
/* RSVD 0x8f is Reserved */
/* RSVD 0x90 is Reserved */
/* RSVD 0x91 is Reserved */
#define OV7660_CTL_DM_LNL		0x92
#define OV7660_CTL_DM_LNH		0x93
/* RSVD 0x94 is Reserved */
/* RSVD 0x95 is Reserved */
/* RSVD 0x96 is Reserved */
/* RSVD 0x97 is Reserved */
/* RSVD 0x98 is Reserved */
/* RSVD 0x99 is Reserved */
/* RSVD 0x9a is Reserved */
/* RSVD 0x9b is Reserved */
/* RSVD 0x9c is Reserved */
#define OV7660_CTL_BD50ST		0x9d
#define OV7660_CTL_BD60ST		0x9e
/* RSVD 0x9f is Reserved */
/* a0 DSPC2 */
/* RSVD 0xa1 is Reserved */
/* RSVD 0xa2 is Reserved */
/* RSVD 0xa3 is Reserved */
/* RSVD 0xa4 is Reserved */
/* RSVD 0xa5 is Reserved */
/* All other are factory reserved ! */

/* Common functions and data for the Omnivision OV7670 sensor. */

#define OV7670_CTL_GAIN			0x00
#define OV7670_CTL_BLUE			0x01
#define OV7670_CTL_RED			0x02
#define OV7670_CTL_VREF			0x03
#define OV7670_CTL_COM1			0x04
#define OV7670_CTL_BAVE			0x05
#define OV7670_CTL_GbAVE		0x06
#define OV7670_CTL_AECHH		0x07
#define OV7670_CTL_RAVE			0x08
#define OV7670_CTL_COM2			0x09
#define OV7670_CTL_PID			0x0a
#define OV7670_CTL_VER			0x0b
#define OV7670_CTL_COM3			0x0c
#define OV7670_CTL_COM4			0x0d
#define OV7670_CTL_COM5			0x0e
#define OV7670_CTL_COM6			0x0f
#define OV7670_CTL_AECH			0x10
#define OV7670_CTL_CLKRC		0x11
#define OV7670_CTL_COM7			0x12
#define OV7670_CTL_COM8			0x13
#define OV7670_CTL_COM9			0x14
#define OV7670_CTL_COM10		0x15
#define OV7670_CTL_HSTART		0x17
#define OV7670_CTL_HSTOP		0x18
#define OV7670_CTL_VSTRT		0x19
#define OV7670_CTL_VSTOP		0x1a
#define OV7670_CTL_PSHFT		0x1b
#define OV7670_CTL_MIDH			0x1c
#define OV7670_CTL_MIDL			0x1d
#define OV7670_CTL_MVFP			0x1e
#define OV7670_VFLIP_BIT		0x10
#define OV7670_CTL_LAEC			0x1f
#define OV7670_CTL_ADCCTR0		0x20
#define OV7670_CTL_ADCCTR1		0x21
#define OV7670_CTL_ADCCTR2		0x22
#define OV7670_CTL_ADCCTR3		0x23
#define OV7670_CTL_AEW			0x24
#define OV7670_CTL_AEB			0x25
#define OV7670_CTL_VPT			0x26
#define OV7670_CTL_BBIAS		0x27
#define OV7670_CTL_GbBIAS		0x28
#define OV7670_CTL_EXHCH		0x2a
#define OV7670_CTL_EXHCL		0x2b
#define OV7670_CTL_RBIAS		0x2c
#define OV7670_CTL_ADVFL		0x2d
#define OV7670_CTL_ADVFH		0x2e
#define OV7670_CTL_YAVE			0x2f
#define OV7670_CTL_HSYST		0x30
#define OV7670_CTL_HSYEN		0x31
#define OV7670_CTL_HREF			0x32
#define OV7670_CTL_CHLF			0x33
#define OV7670_CTL_ARBLM		0x34
#define OV7670_CTL_ADC			0x37
#define OV7670_CTL_ACOM			0x38
#define OV7670_CTL_OFON			0x39
#define OV7670_CTL_TSLB			0x3a
#define OV7670_CTL_COM11		0x3b
#define OV7670_CTL_COM12		0x3c
#define OV7670_CTL_COM13		0x3d
#define OV7670_CTL_COM14		0x3e
#define OV7670_CTL_EDGE			0x3f
#define OV7670_CTL_COM15		0x40
#define OV7670_CTL_COM16		0x41
#define OV7670_CTL_COM17		0x42
#define OV7670_CTL_AWBC1		0x43
#define OV7670_CTL_AWBC2		0x44
#define OV7670_CTL_AWBC3		0x45
#define OV7670_CTL_AWBC4		0x46
#define OV7670_CTL_AWBC5		0x47
#define OV7670_CTL_AWBC6		0x48
#define OV7670_CTL_REG4B		0x4b
#define OV7670_CTL_DNSTH		0x4c
#define OV7670_CTL_MTX1			0x4f
#define OV7670_CTL_MTX2			0x50
#define OV7670_CTL_MTX3			0x51
#define OV7670_CTL_MTX4			0x52
#define OV7670_CTL_MTX5			0x53
#define OV7670_CTL_MTX6			0x54
#define OV7670_CTL_BRIGHT		0x55
#define OV7670_CTL_CONTRAS		0x56
#define OV7670_CTL_CONTRAS_CENTER	0x57
#define OV7670_CTL_MTXS			0x58
#define OV7670_CTL_LCC1			0x62
#define OV7670_CTL_LCC2			0x63
#define OV7670_CTL_LCC3			0x64
#define OV7670_CTL_LCC4			0x65
#define OV7670_CTL_LCC5			0x66
#define OV7670_CTL_MANU			0x67
#define OV7670_CTL_MANV			0x68
#define OV7670_CTL_GFIX			0x69
#define OV7670_CTL_GGAIN		0x6a
#define OV7670_CTL_DBLV			0x6b
#define OV7670_CTL_AWBCTR3		0x6c
#define OV7670_CTL_AWBCTR2		0x6d
#define OV7670_CTL_AWBCTR1		0x6e
#define OV7670_CTL_AWBCTR0		0x6f
#define OV7670_CTL_SCALING_XSC		0x70
#define OV7670_CTL_SCALING_YSC		0x71
#define OV7670_CTL_SCALING_DCWCTR	0x72
#define OV7670_CTL_SCALING_PCLK_DIV	0x73
#define OV7670_CTL_REG74		0x74
#define OV7670_CTL_REG75		0x75
#define OV7670_CTL_REG76		0x76
#define OV7670_CTL_REG77		0x77
#define OV7670_CTL_SLOP			0x7a
#define OV7670_CTL_GAM1			0x7b
#define OV7670_CTL_GAM2			0x7c
#define OV7670_CTL_GAM3			0x7d
#define OV7670_CTL_GAM4			0x7e
#define OV7670_CTL_GAM5			0x7f
#define OV7670_CTL_GAM6			0x80
#define OV7670_CTL_GAM7			0x81
#define OV7670_CTL_GAM8			0x82
#define OV7670_CTL_GAM9			0x83
#define OV7670_CTL_GAM10		0x84
#define OV7670_CTL_GAM11		0x85
#define OV7670_CTL_GAM12		0x86
#define OV7670_CTL_GAM13		0x87
#define OV7670_CTL_GAM14		0x88
#define OV7670_CTL_GAM15		0x89
#define OV7670_CTL_RGB444		0x8c
#define OV7670_CTL_DM_LNL		0x92
#define OV7670_CTL_DM_LNH		0x93
#define OV7670_CTL_LCC6			0x94
#define OV7670_CTL_LCC7			0x95
#define OV7670_CTL_BD50ST		0x9d
#define OV7670_CTL_BD60ST		0x9e
#define OV7670_CTL_HAECC1		0x9f
#define OV7670_CTL_HAECC2		0xa0
#define OV7670_CTL_SCALING_PCLK_DELAY	0xa2
#define OV7670_CTL_NT_CTRL		0xa4
#define OV7670_CTL_BD50MAX		0xa5
#define OV7670_CTL_HAECC3		0xa6
#define OV7670_CTL_HAECC4		0xa7
#define OV7670_CTL_HAECC5		0xa8
#define OV7670_CTL_HAECC6		0xa9
#define OV7670_CTL_HAECC7		0xaa
#define OV7670_CTL_BD60MAX		0xab
#define OV7670_CTL_STR_OPT		0xac
#define OV7670_CTL_STR_R		0xad
#define OV7670_CTL_STR_G		0xae
#define OV7670_CTL_STR_B		0xaf
#define OV7670_CTL_ABLC1		0xb1
#define OV7670_CTL_THL_ST		0xb3
#define OV7670_CTL_THL_DLT		0xb5
#define OV7670_CTL_AD_CHB		0xbe
#define OV7670_CTL_AH_CHR		0xbf
#define OV7670_CTL_AD_CHGb		0xc0
#define OV7670_CTL_AD_CHGr		0xc1
#define OV7670_CTL_SATCTR		0xc9

/* Common functions and data for the Omnivision OV965x sensor series. */

#define OV965X_CTL_GAIN		0x00
#define OV965X_CTL_BLUE		0x01
#define OV965X_CTL_RED		0x02
#define OV965X_CTL_VREF		0x03
#define OV965X_CTL_COM1		0x04
#define OV965X_CTL_BAVE		0x05
#define OV965X_CTL_GEAVE	0x06
#define OV965X_CTL_RAVE		0x08
#define OV965X_CTL_COM2		0x09
#define OV965X_CTL_PID		0x0a
#define OV965X_CTL_VER		0x0b
#define OV965X_CTL_COM3		0x0c
#define OV965X_CTL_COM4		0x0d
#define OV965X_CTL_COM5		0x0e
#define OV965X_CTL_COM6		0x0f
#define OV965X_CTL_AECH		0x10
#define OV965X_CTL_CLKRC	0x11
#define OV965X_CTL_COM7		0x12
#define OV965X_CTL_COM8		0x13
#define OV965X_CTL_COM9		0x14
#define OV965X_CTL_COM10	0x15
#define OV965X_CTL_HSTART	0x17
#define OV965X_CTL_HSTOP	0x18
#define OV965X_CTL_VSTRT	0x19
#define OV965X_CTL_VSTOP	0x1a
#define OV965X_CTL_PSHIFT	0x1b
#define OV965X_CTL_MIDH		0x1c
#define OV965X_CTL_MIDL		0x1d
#define OV965X_CTL_MVFP		0x1e
#define OV965X_CTL_LAEC		0x1f
#define OV965X_CTL_BOS		0x20
#define OV965X_CTL_GBOS		0x21
#define OV965X_CTL_GROS		0x22
#define OV965X_CTL_ROS		0x23
#define OV965X_CTL_AEW		0x24
#define OV965X_CTL_AEB		0x25
#define OV965X_CTL_VPT		0x26
#define OV965X_CTL_BBIAS	0x27
#define OV965X_CTL_GbBIAS	0x28
#define OV965X_CTL_Gr_COM	0x29
#define OV965X_CTL_EXHCH	0x2a
#define OV965X_CTL_EXHCL	0x2b
#define OV965X_CTL_RBIAS	0x2c
#define OV965X_CTL_ADVFL	0x2d
#define OV965X_CTL_ADVFH	0x2e
#define OV965X_CTL_YAVE		0x2f
#define OV965X_CTL_HSYST	0x30
#define OV965X_CTL_HSYEN	0x31
#define OV965X_CTL_HREF		0x32
#define OV965X_CTL_CHLF		0x33
#define OV965X_CTL_ARBLM	0x34
#define OV965X_CTL_ADC		0x37
#define OV965X_CTL_ACOM		0x38
#define OV965X_CTL_OFON		0x39
#define OV965X_CTL_TSLB		0x3a
#define OV965X_CTL_COM11	0x3b
#define OV965X_CTL_COM12	0x3c
#define OV965X_CTL_COM13	0x3d
#define OV965X_CTL_COM14	0x3e
#define OV965X_CTL_EDGE		0x3f
#define OV965X_CTL_COM15	0x40
#define OV965X_CTL_COM16	0x41
#define OV965X_CTL_COM17	0x42
#define OV965X_CTL_MTX1		0x4f
#define OV965X_CTL_MTX2		0x50
#define OV965X_CTL_MTX3		0x51
#define OV965X_CTL_MTX4		0x52
#define OV965X_CTL_MTX5		0x53
#define OV965X_CTL_MTX6		0x54
#define OV965X_CTL_MTX7		0x55
#define OV965X_CTL_MTX8		0x56
#define OV965X_CTL_MTX9		0x57
#define OV965X_CTL_LCC1		0x62
#define OV965X_CTL_LCC2		0x63
#define OV965X_CTL_LCC3		0x64
#define OV965X_CTL_LCC4		0x65
#define OV965X_CTL_LCC5		0x66
#define OV965X_CTL_MANU		0x67
#define OV965X_CTL_MANV		0x68
#define OV965X_CTL_HV		0x69
#define OV965X_CTL_MBD		0x6a
#define OV965X_CTL_DBLV		0x6b
#define OV965X_CTL_COM21	0x8b
#define OV965X_CTL_COM22	0x8c
#define OV965X_CTL_COM23	0x8d
#define OV965X_CTL_COM24	0x8e
#define OV965X_CTL_DBLC1	0x8f
#define OV965X_CTL_DBLC_B	0x90
#define OV965X_CTL_DBLC_R	0x91
#define OV965X_CTL_DM_LNL	0x92
#define OV965X_CTL_DM_LNH	0x93
#define OV965X_CTL_LCCFB	0x9d
#define OV965X_CTL_LCCFR	0x9e
#define OV965X_CTL_DBLC_Gb	0x9f
#define OV965X_CTL_DBLC_Gr	0xa0
#define OV965X_CTL_AECHM	0xa1
#define OV965X_CTL_COM25	0xa4 /* all bits reserved */
#define OV965X_CTL_COM26	0xa5 /* all bits reserved */
#define OV965X_CTL_G_GAIN	0xa6 /* reserved */
#define OV965X_CTL_VGA_ST	0xa7 /* reserved */
#define OV965X_CTL_ACOM2	0xa8 /* reserved */

#define OV965X_COM7_SCCB_RESET		0x80
#define OV965X_COM7_OUTPUT_VGA		0x40
#define OV965X_COM7_OUTPUT_CIF		0x20
#define OV965X_COM7_OUTPUT_QVGA		0x10
#define OV965X_COM7_OUTPUT_QCIF		0x08
#define OV965X_COM7_OUTPUT_RGB		0x04
#define OV965X_COM7_OUTPUT_RAW_RGB	0x05

#define OV965X_VREF_ADC_HIGH2(x) 	((x) << 6)
#define OV965X_VREF_VSTOP_LOW3(x) 	(0x38 & ((x) << 3))
#define OV965X_VREF_VSTART_LOW3(x) 	(0x07 & (x))

#define OV965X_COM2_OUTPUT_DRIVE_CAP_1X		0x00
#define OV965X_COM2_OUTPUT_DRIVE_CAP_2X		0x01
#define OV965X_COM2_OUTPUT_DRIVE_CAP_3X		0x02
#define OV965X_COM2_OUTPUT_DRIVE_CAP_4X		0x03
#define OV965X_COM2_SOFT_SLEEP			0x10

#define OV965X_COM5_15FPS_48MHZ_RGB		0x80
#define OV965X_COM5_SLAM_MODE			0x10

#define OV965X_COM6_BLACK_LINE_HREF_ENABLE	0x80
#define OV965X_COM6_ADBLC_BIAS_ENABLE		0x08
#define OV965X_COM6_ADBLC_OFFSET_2_CHANNEL	0x04 /* else 4-channel */
#define OV965X_COM6_TIMING_RESET_ON_FMT_CHANGE	0x02
#define OV965X_COM6_ADBLC_ENABLE		0x01

#define OV965X_CLKRC_DBL_CLK_ENABLE		0x80
#define OV965X_CLKRC_INPUT_CLK_NO_SCALE		0x40

#define OV965X_COM8_FAST_AGC_AEC		0x80
#define OV965X_COM8_AEC_STEP_SIZE_NOLIMIT	0x40 /* otherwise VSYNC */
#define OV965X_COM8_BANDING_FILTER_ON		0x20
#define OV965X_COM8_AGC_ENABLE			0x04
#define OV965X_COM8_AWB_ENABLE			0x02
#define OV965X_COM8_AEC_ENABLE			0x01

#define OV965X_COM9_MAX_AGC_128X		(0x06 << 4)
#define OV965X_COM9_MAX_AGC_64X			(0x05 << 4)
#define OV965X_COM9_MAX_AGC_32X			(0x04 << 4)
#define OV965X_COM9_MAX_AGC_16X			(0x03 << 4)
#define OV965X_COM9_MAX_AGC_8X			(0x02 << 4)
#define OV965X_COM9_MAX_AGC_4X			(0x01 << 4)
#define OV965X_COM9_RELAX_EXPOSURE_TIMING	0x08
#define OV965X_COM9_DROP_VSYNC_ON_FRAME_DROP	0x04
#define OV965X_COM9_DROP_FRAME_ON_BIG_AEC	0x02
#define OV965X_COM9_FREEZE_AGC_AEC		0x01

#define OV965X_MVFP_MIRROR			0x20
#define OV965X_MVFP_VFLIP			0x10

#define OV965X_Gr_COM_BYPASS_ANALOG_BLC		0x20
#define OV965X_Gr_COM_BYPASS_REGULATOR		0x10

#define OV965X_BIAS_SUBTRACT			0x80
#define OV965X_BIAS_VALUE(x)			((~0x80) & (x))

#define OV965X_HREF_EDGE_OFT_TO_DATA_OUT(x)	((x) << 6)
#define OV965X_HREF_HSTOP_LOW3(x)		((~0xc0) & ((x) << 3))
#define OV965X_HREF_HSTART_LOW3(x)		((~0xf8) & (x))

#define OV965X_TSLB_BITWISE_REVERSE		0x20
#define OV965X_TSLB_FIXED_UV_OUTPUT		0x10
#define OV965X_TSLB_OUTPUT_SEQ_YUYV		0x00
#define OV965X_TSLB_OUTPUT_SEQ_YVYU		0x04
#define OV965X_TSLB_OUTPUT_SEQ_VYUY		0x08
#define OV965X_TSLB_OUTPUT_SEQ_UYVY		0x0c
#define OV965X_TSLB_DIGITAL_BLC_ENABLE		0x01

#define OV965X_COM11_NIGHT_MODE_ENABLE		0x80
#define OV965X_COM11_NM_NORMAL_FRAME_RATE	0x00
#define OV965X_COM11_NM_HALF_FRAME_RATE		0x20
#define OV965X_COM11_NM_QUARTER_FRAME_RATE	0x40
#define OV965X_COM11_NM_EIGHTH_FRAME_RATE	0x60
#define OV965X_COM11_FULL_FRAME_CALC_WIND	0x00
#define OV965X_COM11_HALF_FRAME_CALC_WIND	0x08
#define OV965X_COM11_QUARTER_FRAME_CALC_WIND	0x10
#define OV965X_COM11_MANUAL_BANDING_FILTER	0x01

#define OV965X_COM13_ONLY_Y_CHANNEL_GAMMA	0x40
#define OV965X_COM13_GAMMA_RAW_PRE_INTERP	0x80
#define OV965X_COM13_ENABLE_COLOR_MATRIX	0x10
#define OV965X_COM13_DELAY_UV_CHANNEL		0x00
#define OV965X_COM13_DELAY_Y_CHANNEL		0x08
#define OV965X_COM13_OUTPUT_DELAY(x)		((~0xf8) & (x))

#define OV965X_COM14_YUV_EDGE_ENHANCE		0x02
#define OV965X_COM14_EDGE_ENHANCE_FACTOR_DBL	0x01

#define OV965X_EDGE_EDGE_ENHANCE_LOW4(x)	((x) << 4)
#define OV965X_EDGE_EDGE_ENHANCE_FACTOR(x)	(0xf0 & (x))

#define OV965X_COM15_OUTPUT_RANGE_10_TO_F0	0x00
#define OV965X_COM15_OUTPUT_RANGE_O1_TO_FE	0x80
#define OV965X_COM15_OUTPUT_RANGE_O0_TO_FF	0xc0
#define OV965X_COM15_OUTPUT_RGB_565		0x10
#define OV965X_COM15_OUTPUT_RGB_555		0x30
#define OV965X_COM15_SWAP_R_AND_B		0x08

extern struct sn9c20x_i2c_regs soi968_init[];
extern struct sn9c20x_i2c_regs ov9650_init[];
extern struct sn9c20x_i2c_regs ov9655_init[];
extern struct sn9c20x_i2c_regs ov7660_init[];
extern struct sn9c20x_i2c_regs ov7670_init[];

int ov7670_auto_flip(struct usb_sn9c20x *, __u8);
int ov7670_flip_detect(struct usb_sn9c20x *dev);

int soi968_set_exposure(struct usb_sn9c20x *dev);
int soi968_set_gain(struct usb_sn9c20x *dev);
int soi968_set_autoexposure(struct usb_sn9c20x *dev);
int soi968_set_autowhitebalance(struct usb_sn9c20x *dev);

void ov965x_set_sxga_mode(struct usb_sn9c20x *dev, bool sxga);
int ov965x_set_hvflip(struct usb_sn9c20x *);
int ov965x_flip_detect(struct usb_sn9c20x *dev);
int ov9650_set_gain(struct usb_sn9c20x *dev);

int ov_set_exposure(struct usb_sn9c20x *);
int ov_set_autogain(struct usb_sn9c20x *dev);
#endif
