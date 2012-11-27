/**
 * @file sn9c20x-usb.c
 * @author Nicolas VIVIEN
 * @date 2008-02-01
 *
 * @brief Driver for SN9C20X USB video camera
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
 *
 *
 * 2012-11-26: Ported to GNU/Linux Kernel 3.6.x series by Marco A Benatto <benatto at mandriva.com.br>
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/kref.h>
#include <linux/stat.h>
#include <linux/usb.h>
#include <media/v4l2-common.h>

#ifdef CONFIG_SN9C20X_EVDEV
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/usb/input.h>
#endif

#include "sn9c20x.h"
#include "sn9c20x-bridge.h"
#include "micron.h"
#include "omnivision.h"

/**
 * @var fps
 *   Module parameter to set frame per second
 */
static __u8 fps = 25;

/**
 * @var bulk
 *  Module parameter to enable/disable bulk transfers
 */
static __u8 bulk;

/**
 * @var jpeg
 *  Module parameter to enable/disable JPEG format
 */
__u8 jpeg = 2;

/**
 * @var bandwidth
 *  Module parameter to set the available bandwidth via the alternate setting
 */
static __u8 bandwidth = 8;

/**
 * @var hflip
 *  Module parameter to enable/disable the horizontal flip process
 */
static __u8 hflip;

/**
 * @var flip_detect
 *  Module parameter to enable/disable vflip detection
 */
static __u8 flip_detect;

/**
 * @var vflip
 *   Module parameter to enable/disable the vertical flip process
 */
static __u8 vflip;

/**
 * @var brightness
 *   Module parameter to set the brightness
 */
static __u16 brightness = SN9C20X_PERCENT(50, 0xFF);

/**
 * @var gamma
 *   Module parameter to set the gamma
 */
static __u16 gamma = SN9C20X_PERCENT(20, 0xFF);

/**
 * @var saturation
 *   Module parameter to set the gamma
 */
static __u16 saturation = SN9C20X_PERCENT(50, 0xFF);

/**
 * @var hue
 *   Module parameter to set the gamma
 */
static __s16 hue;

/**
 * @var contrast
 *   Module parameter to set the contrast
 */
static __u16 contrast = SN9C20X_PERCENT(50, 0xFF);

/**
 * @var exposure
 *   Module parameter to set the exposure
 */
static __u16 exposure = SN9C20X_PERCENT(20, 0xFF);

/**
 * @var gain
 *   Module parameter to set the gain
 */
static __u16 gain = SN9C20X_PERCENT(20, 0xFF);

/**
 * @var sharpness
 *   Module parameter to set the sharpness
 */
static __u16 sharpness = SN9C20X_PERCENT(50, 0x3F);

/**
 * @var red_gain
 *   Module parameter to set the red gain
 */
static __u16 red_gain = SN9C20X_PERCENT(25, 0x7F);

/**
 * @var blue_gain
 *   Module parameter to set the blue gain
 */
static __u16 blue_gain = SN9C20X_PERCENT(25, 0x7F);

/**
 * @var min_buffers
 *   Module parameter to set the minimum number of image buffers
 */
static __u8 min_buffers = 2;

/**
 * @var max_buffers
 *   Module parameter to set the maximum number of image buffers
 */
static __u8 max_buffers = 5;

/**
 * @var auto_exposure
 *   Module parameter to set the exposure
 */
static __u8 auto_exposure = 1;

/**
 * @var auto_gain
 *   Module parameter to set the gain
 */
static __u8 auto_gain;


/**
 * @var auto_whitebalance
 *   Module parameter to set the auto-whitebalance
 */
static __u8 auto_whitebalance = 1;

/**
 * @var log_level
 *   Module parameter to set the log level
 */
__u8 log_level = 5;


/**
 * @var sn9c20x_table
 * Define all the hotplug supported devices by this driver
 */
static struct usb_device_id sn9c20x_table[] = {
	/* SN9C201 + MI1300 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6240, MT9M001_SENSOR, 0x5d)},
	/* SN9C201 + MI1310 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6242, MT9M111_SENSOR, 0x5d)},
	/* SN9C201 + S5K4AAFX */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6243, 0, 0)},
	/* SN9C201 + OV9655 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6248, OV9655_SENSOR, 0x30)},
	/* SN9C201 + CX1332 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x624B, 0, 0)},
	/* SN9C201 + MI1320 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x624C, 0, 0)},
	/* SN9C201 + SOI968 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x624E, SOI968_SENSOR, 0x30)},
	/* SN9C201 + OV9650 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x624F, OV9650_SENSOR, 0x30)},
	/* SN9C201 + OV9650 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6251, OV9650_SENSOR, 0x30)},
	/* SN9C201 + OV9650 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6253, OV9650_SENSOR, 0x30)},
	/* SN9C201 + OV7670ISP */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6260, OV7670_SENSOR, 0x21)},
	/* SN9C201 + OM6802 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6262, 0, 0)},
	/* SN9C201 + MI0360/MT9V111 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6270, 0, 0)},
	/* SN9C201 + S5K53BEB */
	{SN9C20X_USB_DEVICE(0x0C45, 0x627A, 0, 0)},
	/* SN9C201 + OV7660 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x627B, OV7660_SENSOR, 0x21)},
	/* SN9C201 + HV7131R */
	{SN9C20X_USB_DEVICE(0x0C45, 0x627C, HV7131R_SENSOR, 0x11)},
	/* EEPROM */
	{SN9C20X_USB_DEVICE(0x0C45, 0x627F, OV9650_SENSOR, 0x30)},
	/* SN9C202 + MI1300 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6280, MT9M001_SENSOR, 0x5d)},
	/* SN9C202 + MI1310 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6282, MT9M111_SENSOR, 0x5d)},
	/* SN9C202 + S5K4AAFX */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6283, 0, 0)},
	/* SN9C202 + OV9655 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x6288, OV9655_SENSOR, 0x30)},
	/* SN9C202 + ICM107 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x628A, 0, 0)},
	/* SN9C202 + CX1332 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x628B, 0, 0)},
	/* SN9C202 + MI1320 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x628C, 0, 0)},
	/* SN9C202 + SOI968 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x628E, SOI968_SENSOR, 0x30)},
	/* SN9C202 + OV9650 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x628F, OV9650_SENSOR, 0x30)},
	/* SN9C202 + OV7670ISP */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62A0, OV7670_SENSOR, 0x21)},
	/* SN9C202 + OM6802 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62A2, 0, 0)},
	/* SN9C202 + MI0360/MT9V111 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62B0, 0, 0)},
	/* SN9C202 + OV9655 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62B3, OV9655_SENSOR, 0x30)},
	/* SN9C202 + S5K53BEB */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62BA, 0, 0)},
	/* SN9C202 + OV7660 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62BB, OV7660_SENSOR, 0x21)},
	/* SN9C202 + HV7131R */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62BC, HV7131R_SENSOR, 0x11)},
	/* SN9C202 + OV7663 */
	{SN9C20X_USB_DEVICE(0x0C45, 0x62BE, 0, 0)},
	/* => 628f (SN9C202 + OV9650) */
	{SN9C20X_USB_DEVICE(0x045E, 0x00F4, OV9650_SENSOR, 0x30)},
	/* => 627b (SN9C201 + OV7660) */
	{SN9C20X_USB_DEVICE(0x145F, 0x013D, OV7660_SENSOR, 0x21)},
	/* => 62be (SN9C202 + OV7663 + EEPROM) */
	{SN9C20X_USB_DEVICE(0x04F2, 0xA128, 0, 0)},
	/* => 627c (SN9C201 + HV7131R) */
	{SN9C20X_USB_DEVICE(0x0458, 0x7029, HV7131R_SENSOR, 0x11)},
	/* SN9C201 + HV7131R (Dino Lite - Digital Microscope) */
	{SN9C20X_USB_DEVICE(0xA168, 0x0610, HV7131R_SENSOR, 0x11)},
	{SN9C20X_USB_DEVICE(0xA168, 0x0611, HV7131R_SENSOR, 0x11)},
	{SN9C20X_USB_DEVICE(0xA168, 0x0613, HV7131R_SENSOR, 0x11)},
	{SN9C20X_USB_DEVICE(0xA168, 0x0618, HV7131R_SENSOR, 0x11)},
	/* SN9C201 + MI1310 (Dino Lite Pro - Digital Microscope) */
	{SN9C20X_USB_DEVICE(0xA168, 0x0614, MT9M111_SENSOR, 0x5d)},
	{SN9C20X_USB_DEVICE(0xA168, 0x0615, MT9M111_SENSOR, 0x5d)},
	{SN9C20X_USB_DEVICE(0xA168, 0x0617, MT9M111_SENSOR, 0x5d)},
	{SN9C20X_USB_DEVICE(0x0C45, 0x63EE, MT9M111_SENSOR, 0X5d)},
	{}
};


MODULE_DEVICE_TABLE(usb, sn9c20x_table);		/**< Define the supported devices */

DEFINE_MUTEX(open_lock); /**< Define global mutex */

struct usb_endpoint_descriptor *find_endpoint(struct usb_host_interface *alts,
		__u8 epaddr)
{
	unsigned long i;
	struct usb_endpoint_descriptor *ep;

	for (i = 0; i < alts->desc.bNumEndpoints; ++i) {
		ep = &alts->endpoint[i].desc;
		if ((ep->bEndpointAddress & 0xf) == epaddr) {
			UDIA_DEBUG("Found Endpoint 0x%X\n", epaddr);
			return ep;
		}
	}

	return NULL;
}
/**
 * @param dev Device structure
 * @param ep Usb endpoint structure
 *
 * @returns 0 if all is OK
 *
 * @brief Initilize an isochronous pipe.
 *
 * This function permits to initialize an URB transfert (or isochronous pipe).
 */
int usb_sn9c20x_isoc_init(struct usb_sn9c20x *dev,
	struct usb_endpoint_descriptor *ep)
{
	int i, j;
	__u16 iso_max_frame_size;
	struct urb *urb;
	struct usb_device *udev;

	udev = dev->udev;

	UDIA_DEBUG("usb_sn9c20x_isoc_init()\n");

	iso_max_frame_size =
		max_packet_sz(le16_to_cpu(ep->wMaxPacketSize)) *
		hb_multiplier(le16_to_cpu(ep->wMaxPacketSize));
	for (i = 0; i < MAX_URBS; i++) {
		urb = usb_alloc_urb(ISO_FRAMES_PER_DESC, GFP_KERNEL);

		if (urb == NULL) {
			UDIA_ERROR("Failed to allocate URB %d\n", i);
			usb_sn9c20x_uninit_urbs(dev, 1);
			return -ENOMEM;
		}

		urb->interval = 1;
		urb->dev = udev;
		urb->pipe = usb_rcvisocpipe(udev, ep->bEndpointAddress);
		urb->transfer_flags = URB_ISO_ASAP;
		urb->transfer_buffer_length = iso_max_frame_size * ISO_FRAMES_PER_DESC;
		urb->complete = usb_sn9c20x_completion_handler;
		urb->context = dev;
		urb->start_frame = 0;
		urb->number_of_packets = ISO_FRAMES_PER_DESC;

		for (j = 0; j < ISO_FRAMES_PER_DESC; j++) {
			urb->iso_frame_desc[j].offset = j * iso_max_frame_size;
			urb->iso_frame_desc[j].length = iso_max_frame_size;
		}

		if (dev->urbs[i].data == NULL) {
			dev->urbs[i].data = kzalloc(urb->transfer_buffer_length,
						    GFP_KERNEL);
			if (dev->urbs[i].data == NULL) {
				usb_sn9c20x_uninit_urbs(dev, 1);
				return -ENOMEM;
			}
		}
		urb->transfer_buffer = dev->urbs[i].data;
		dev->urbs[i].urb = urb;
	}

	return 0;
}

int usb_sn9c20x_bulk_init(struct usb_sn9c20x *dev,
	struct usb_endpoint_descriptor *ep)
{
	struct urb *urb;
	unsigned int pipe, i;
	__u16 psize;
	__u32 size;
	psize = max_packet_sz(le16_to_cpu(ep->wMaxPacketSize));
	size = psize * ISO_FRAMES_PER_DESC;
	pipe = usb_rcvbulkpipe(dev->udev, ep->bEndpointAddress);

	for (i = 0; i < MAX_URBS; ++i) {
		urb = usb_alloc_urb(0, GFP_KERNEL);
		if (urb == NULL) {
			usb_sn9c20x_uninit_urbs(dev, 1);
			return -ENOMEM;
		}
		if (dev->urbs[i].data == NULL) {
			dev->urbs[i].data = kzalloc(size, GFP_KERNEL);
			if (dev->urbs[i].data == NULL) {
				usb_sn9c20x_uninit_urbs(dev, 1);
				return -ENOMEM;
			}
		}

		usb_fill_bulk_urb(urb, dev->udev, pipe,
				  dev->urbs[i].data, size,
				  usb_sn9c20x_completion_handler,
				  dev);

		dev->urbs[i].urb = urb;
	}

	return 0;
}

int usb_sn9c20x_init_urbs(struct usb_sn9c20x *dev)
{
	int ret, i;
	__u8 value;
	struct usb_endpoint_descriptor *ep;
	struct usb_interface *intf = dev->interface;

	ret = usb_sn9c20x_control_read(dev, 0x1061, &value, 1);
	if (ret < 0)
		return ret;

	if (!bulk) {
		if (bandwidth > 8)
			return -EINVAL;

		ep = find_endpoint(usb_altnum_to_altsetting(intf, bandwidth), SN9C20X_VID_ISOC);
		if (ep == NULL)
			return -EIO;

		ret = usb_set_interface(dev->udev, 0, bandwidth);
		if (ret < 0)
			return ret;

		value |= 0x01;
		ret = usb_sn9c20x_control_write(dev, 0x1061, &value, 1);
		if (ret < 0)
			return ret;

		ret = usb_sn9c20x_isoc_init(dev, ep);
	} else {
		ep = find_endpoint(usb_altnum_to_altsetting(intf, 0), SN9C20X_BULK);
		if (ep == NULL)
			return -EIO;

		ret = usb_set_interface(dev->udev, 0, 0);
		if (ret < 0)
			return ret;

		value &= ~0x01;
		ret = usb_sn9c20x_control_write(dev, 0x1061, &value, 1);
		if (ret < 0)
			return ret;

		ret = usb_sn9c20x_bulk_init(dev, ep);
	}

	if (ret < 0)
		return ret;

	for (i = 0; i < MAX_URBS; i++) {
		ret = usb_submit_urb(dev->urbs[i].urb, GFP_KERNEL);
		if (ret)
			UDIA_ERROR("isoc_init() submit_urb %d failed with error %d\n", i, ret);
	}

	return 0;
}

/**
 * @param dev Device structure
 *
 * @brief Clean-up all the ISOC buffers
 *
 * This function permits to clean-up all the ISOC buffers.
 */
void usb_sn9c20x_uninit_urbs(struct usb_sn9c20x *dev, int free_buffers)
{
	int i;
	struct urb *urb;

	UDIA_DEBUG("Isoc cleanup\n");

	if (dev == NULL)
		return;

	for (i = 0; i < MAX_URBS; i++) {
		urb = dev->urbs[i].urb;
		if (urb == NULL)
			continue;
		usb_kill_urb(urb);
		usb_free_urb(urb);
		dev->urbs[i].urb = NULL;
		if (free_buffers) {
			kfree(dev->urbs[i].data);
			dev->urbs[i].data = NULL;
		}
	}
}

int usb_sn9c20x_detect_frame(unsigned char *buf, unsigned int buf_length)
{
	static unsigned char frame_header[] = {0xff, 0xff, 0x00, 0xc4, 0xc4, 0x96};
	int index;
	if (buf_length < 64)
		return -1;
	if (buf_length == 64) {
		if (memcmp(buf, frame_header, 6) == 0)
			return 0;
		else
			return -1;
	}
	if (bulk) {
		for (index = 0; index < buf_length - 63; index++) {
			if (memcmp(buf + index, frame_header, 6) == 0) {
				UDIA_DEBUG("Found Header at %d\n", index);
				return index;
			}
		}
	}
	return -1;
}

void usb_sn9c20x_assemble_video(struct usb_sn9c20x *dev,
	unsigned char *transfer, unsigned int transfer_length,
	struct sn9c20x_buffer **buffer)
{
	void *mem = NULL;
	unsigned char *header;
	int header_index;
	int yavg;
	struct sn9c20x_buffer *buf = *buffer;
	struct sn9c20x_video_queue *queue = &dev->queue;

	if (buf->state != SN9C20X_BUF_STATE_ACTIVE)
		buf->state = SN9C20X_BUF_STATE_ACTIVE;

	header_index = usb_sn9c20x_detect_frame(transfer, transfer_length);
	if (header_index >= 0) {
		if (header_index + buf->buf.bytesused > buf->buf.length) {
			UDIA_WARNING("Frame Buffer overflow!\n");
			dev->vframes_overflow++;
			buf->state = SN9C20X_BUF_STATE_DONE;
		}
		header_index = min(buf->buf.length - buf->buf.bytesused,
					(unsigned int)header_index);
		mem = queue->mem + buf->buf.m.offset + buf->buf.bytesused;
		memcpy(mem, transfer, header_index);
		buf->buf.bytesused += header_index;
		header = transfer+header_index;
/*		UDIA_INFO("color window: %dx%d\n",
			    header[0x3a] << 4,
			    header[0x3b] << 3);
		UDIA_INFO("Frame Size: %d\n", buf->buf.bytesused);*/
		yavg = ((header[35] >> 2) & 3)|(header[20] << 2)|(header[19] << 10);
		yavg += ((header[35] >> 4) & 3)|(header[22] << 2)|(header[21] << 10);
		yavg += ((header[35] >> 6) & 3)|(header[24] << 2)|(header[23] << 10);
		yavg += (header[36] & 3)|(header[26] << 2)|(header[25] << 10);
		yavg += ((header[36] >> 2) & 3)|(header[28] << 2)|(header[27] << 10);
		yavg += ((header[36] >> 4) & 3)|(header[30] << 2)|(header[29] << 10);
		yavg += ((header[36] >> 6) & 3)|(header[32] << 2)|(header[31] << 10);
		yavg += ((header[44] >> 4) & 3)|(header[34] << 2)|(header[33] << 10);
		UDIA_DEBUG("AVGY Total: %d (%d)\n", yavg, yavg >> 9);
		yavg >>= 9;
		atomic_set(&dev->camera.yavg, yavg);

		if (buf->buf.bytesused != 0)
			buf->state = SN9C20X_BUF_STATE_DONE;
	} else {
		if (transfer_length + buf->buf.bytesused > buf->buf.length) {
			UDIA_WARNING("Frame Buffer overflow!\n");
			dev->vframes_overflow++;
			buf->state = SN9C20X_BUF_STATE_DONE;
		}
		transfer_length = min(buf->buf.length - buf->buf.bytesused,
				      transfer_length);
		mem = queue->mem + buf->buf.m.offset + buf->buf.bytesused;
		memcpy(mem, transfer, transfer_length);
		buf->buf.bytesused += transfer_length;
	}
	if (buf->state == SN9C20X_BUF_STATE_DONE ||
	    buf->state == SN9C20X_BUF_STATE_ERROR) {
		buf = sn9c20x_queue_next_buffer(queue, buf);
		*buffer = buf;
		if (buf == NULL) {
			dev->vframes_dropped++;
		} else {
			if (header_index + 64 < transfer_length) {
				memcpy(queue->mem + buf->buf.m.offset,
				       transfer + header_index + 64,
				       transfer_length - (header_index + 64));
				buf->buf.bytesused +=
				       transfer_length - (header_index + 64);
			}
		}
	}
}
/**
 * @param urb URB structure
 *
 * @brief ISOC handler
 *
 * This function is called as an URB transfert is complete (Isochronous pipe).
 * So, the traitement is done in interrupt time, so it has be fast, not crash,
 * and not stall. Neat.
 */
void usb_sn9c20x_completion_handler(struct urb *urb)
{
	int i;
	int ret;
	unsigned long flags;

	unsigned char *transfer = NULL;
	unsigned int transfer_length;

	struct sn9c20x_buffer *buf = NULL;
	struct usb_sn9c20x *dev = urb->context;
	struct sn9c20x_video_queue *queue = &dev->queue;

	UDIA_STREAM("Isoc handler\n");

	switch (urb->status) {
	case 0:
		break;

	default:
		UDIA_WARNING("Non-zero status (%d) in video "
			"completion handler.\n", urb->status);

	case -ENOENT:		/* usb_kill_urb() called. */
		if (dev->frozen)
			return;

	case -ECONNRESET:	/* usb_unlink_urb() called. */
	case -ESHUTDOWN:	/* The endpoint is being disabled. */
		sn9c20x_queue_cancel(queue, urb->status == -ESHUTDOWN);
		return;
	}

	spin_lock_irqsave(&queue->irqlock, flags);
	if (!list_empty(&queue->irqqueue))
		buf = list_first_entry(&queue->irqqueue, struct sn9c20x_buffer,
				       queue);
	spin_unlock_irqrestore(&queue->irqlock, flags);
	if (!bulk) {
		for (i = 0; i < urb->number_of_packets; i++) {
			if (urb->iso_frame_desc[i].status != 0) {
				/*UDIA_ERROR("Iso frame %d of USB has error %d\n",
					   i, urb->iso_frame_desc[i].status);*/
				mdelay(1);
				continue;
			}
			transfer_length = urb->iso_frame_desc[i].actual_length;
			transfer = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
			if (buf == NULL)
				continue;

			usb_sn9c20x_assemble_video(dev, transfer, transfer_length, &buf);
		}
	} else {
		if (buf != NULL) {
			usb_sn9c20x_assemble_video(dev, urb->transfer_buffer,
						    urb->actual_length, &buf);
		}
	}
	ret = usb_submit_urb(urb, GFP_ATOMIC);

	if (ret != 0) {
		UDIA_ERROR("Error (%d) re-submitting urb in "
			   "sn9c20x_isoc_handler.\n", ret);
	}
}

/**
 * @param dev Device structure
 * @param value register to write to
 * @param data
 * @param length number of bytes
 *
 * @returns 0 if all is OK
 *
 * @brief Write a 16-bit value to a 16-bit register
 *
 * This function permits to write a 16-bit value to a 16-bit register on the USB bus.
 */
int usb_sn9c20x_control_write(struct usb_sn9c20x *dev, __u16 value, __u8 *data, __u16 length)
{
	int result;
	struct usb_device *udev = dev->udev;

	result = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			0x08,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE,
			value,
			0x00,
			data,
			length,
			500);

	if (unlikely(result < 0)) {
		UDIA_ERROR("Write register failed index = 0x%02X\n", value);
		return result;
	}

	return 0;
}

/**
 * @param dev Device structure
 * @param index register to read from
 * @param data
 * @param length number of bytes
 *
 * @returns 0 if all is OK
 *
 * @brief Read a 16-bit value from a 16-bit register
 *
 * This function permits to read a 16-bit value from a 16-bit register on the USB bus.
 */
int usb_sn9c20x_control_read(struct usb_sn9c20x *dev, __u16 index, __u8 *data, __u16 length)
{
	int result;

	struct usb_device *udev = dev->udev;

	*data = 0;

	result = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			0x00,
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE,
			index,
			0x00,
			data,
			length,
			500);

	if (unlikely(result < 0)) {
		UDIA_ERROR("Read register failed 0x%02X\n", index);
		return result;
	}

	return 0;
}


/**
 * @param dev
 *
 * @returns 0 if all is OK
 *
 * @brief Set the default value about the video settings.
 *
 * This function permits to set the video settings for each video camera model.
 *
 */
static int usb_sn9c20x_default_settings(struct usb_sn9c20x *dev)
{
	dev->vframes_overflow = 0;
	dev->vframes_incomplete = 0;
	dev->vframes_dropped = 0;

	dev->queue.min_buffers = min_buffers;
	dev->queue.max_buffers = max_buffers;

	dev->vsettings.fps = fps;

	v4l2_set_control_default(dev, V4L2_CID_HFLIP, hflip);
	v4l2_set_control_default(dev, V4L2_CID_VFLIP, vflip);
	v4l2_set_control_default(dev, V4L2_CID_GAIN, gain);
	v4l2_set_control_default(dev, V4L2_CID_BRIGHTNESS, brightness);
	v4l2_set_control_default(dev, V4L2_CID_CONTRAST, contrast);
	v4l2_set_control_default(dev, V4L2_CID_GAMMA, gamma);
	v4l2_set_control_default(dev, V4L2_CID_SATURATION, saturation);
	v4l2_set_control_default(dev, V4L2_CID_HUE, hue);
	v4l2_set_control_default(dev, V4L2_CID_SHARPNESS, sharpness);
	v4l2_set_control_default(dev, V4L2_CID_RED_BALANCE, red_gain);
	v4l2_set_control_default(dev, V4L2_CID_BLUE_BALANCE, blue_gain);
	v4l2_set_control_default(dev, V4L2_CID_EXPOSURE_AUTO, auto_exposure);
	v4l2_set_control_default(dev, V4L2_CID_AUTOGAIN, auto_gain);
	v4l2_set_control_default(dev, V4L2_CID_AUTO_WHITE_BALANCE, auto_whitebalance);
	v4l2_set_control_default(dev, V4L2_CID_EXPOSURE, exposure);

	if (jpeg == 2) {
		if (dev->udev->speed == USB_SPEED_HIGH && bandwidth == 8)
			dev->jpeg = 0;
		else
			dev->jpeg = 1;
	} else
		dev->jpeg = jpeg;

	sn9c20x_set_resolution(dev, 640, 480);
	sn9c20x_set_format(dev, sn9c20x_fmts[0].pix_fmt);

	return 0;
}


static int input_thread(void *data)
{
	struct usb_sn9c20x *dev = (struct usb_sn9c20x *)data;
	__u8 gpio;
	int ret, i;

	DECLARE_WAIT_QUEUE_HEAD(wait);
	set_freezable();
	for (;;) {
		if (kthread_should_stop())
			break;

		ret = usb_sn9c20x_control_read(dev, 0x1005, &gpio, 1);
		if (ret < 0)
			continue;

		for (i = 0; i < 8; i++) {
			if (!(dev->input_gpio & (1 << i)))
				continue;

			input_report_key(dev->input_dev,
					 BTN_0 + i,
					 gpio & (1 << i));
			input_sync(dev->input_dev);
		}

		wait_event_freezable_timeout(wait,
					     kthread_should_stop(),
					     msecs_to_jiffies(100));
	}
	return 0;
}


static int sn9c20x_input_init(struct usb_sn9c20x *dev)
{
	int ret;

	dev->input_dev = input_allocate_device();
	if (!dev->input_dev)
		return -ENOMEM;

	dev->input_dev->name = "SN9C20X Webcam";

	dev->input_dev->phys = kasprintf(GFP_KERNEL, "usb-%s-%s",
					 dev->udev->bus->bus_name,
					 dev->udev->devpath);

	if (!dev->input_dev->phys)
		return -ENOMEM;

	usb_to_input_id(dev->udev, &dev->input_dev->id);
	dev->input_dev->dev.parent = &dev->udev->dev;

	set_bit(EV_KEY, dev->input_dev->evbit);
	set_bit(BTN_0, dev->input_dev->keybit);
	set_bit(BTN_1, dev->input_dev->keybit);
	set_bit(BTN_2, dev->input_dev->keybit);
	set_bit(BTN_3, dev->input_dev->keybit);
	set_bit(BTN_4, dev->input_dev->keybit);
	set_bit(BTN_5, dev->input_dev->keybit);
	set_bit(BTN_6, dev->input_dev->keybit);
	set_bit(BTN_7, dev->input_dev->keybit);

	ret = input_register_device(dev->input_dev);
	if (ret)
		return ret;

	dev->input_task = kthread_run(input_thread, dev, "sn9c20x/%d",
				      dev->vdev->minor);

	if (IS_ERR(dev->input_task))
		return -EINVAL;

	return 0;
}

static void sn9c20x_input_cleanup(struct usb_sn9c20x *dev)
{
	if (!IS_ERR(dev->input_task))
		kthread_stop(dev->input_task);

	if (dev->input_dev != NULL) {
		input_unregister_device(dev->input_dev);
		kfree(dev->input_dev->phys);
		input_free_device(dev->input_dev);
		dev->input_dev = NULL;
	}
}

/**
 * @brief Load the driver
 *
 * @param interface
 * @param id
 *
 * @returns 0 if all is OK
 *
 * This function detects the device and allocate the buffers for the device
 * and the video interface.
 */
static int usb_sn9c20x_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int ret;

	int vendor_id;
	int product_id;
	int bNumInterfaces;

	struct usb_sn9c20x *dev = NULL;
	struct usb_device *udev = interface_to_usbdev(interface);

	/* Get USB VendorID and ProductID */
	vendor_id = le16_to_cpu(udev->descriptor.idVendor);
	product_id = le16_to_cpu(udev->descriptor.idProduct);

	/* Check if we can handle this device */
	UDIA_DEBUG("Probe function called with VendorID=%04X, ProductID=%04X and InterfaceNumber=%d\n",
			vendor_id, product_id, interface->cur_altsetting->desc.bInterfaceNumber);

	UDIA_INFO("SN9C20X USB 2.0 Webcam - %04X:%04X plugged-in.\n",
		  vendor_id, product_id);


	/*
	// Allocate structure, initialize pointers, mutexes, etc.
	// and link it to the usb_device
	*/
	dev = kzalloc(sizeof(struct usb_sn9c20x), GFP_KERNEL);

	if (dev == NULL) {
		UDIA_ERROR("Out of memory !\n");
		ret = -ENOMEM;
		goto error;
	}

	kref_init(&dev->vopen);

	dev->udev = udev;
	dev->interface = interface;

	/* Read the product release */
	dev->release = le16_to_cpu(udev->descriptor.bcdDevice);
	UDIA_DEBUG("Release: %04x\n", dev->release);

	/* How many interfaces (1 or 3) ? */
	bNumInterfaces = udev->config->desc.bNumInterfaces;
	UDIA_DEBUG("Number of interfaces : %d\n", bNumInterfaces);

	dev->camera.sensor = id->driver_info & 0xFF;
	dev->camera.address = (id->driver_info >> 8) & 0xFF;

	/* Initialize the camera */
	ret = sn9c20x_initialize(dev);
	if (ret < 0)
		goto error;

	/* Initialize the video device */
	dev->vdev = video_device_alloc();

	if (!dev->vdev) {
		ret = -ENOMEM;
		goto free_dev;
	}

	/* Register the video device */
	ret = v4l_sn9c20x_register_video_device(dev);

	if (ret)
		goto free_dev;

	/* Create the entries in the sys filesystem */
	sn9c20x_create_sysfs_files(dev->vdev);

	sn9c20x_create_debugfs_files(dev);

#ifdef CONFIG_SN9C20X_EVDEV
	ret = sn9c20x_input_init(dev);
	if (ret < 0)
		goto free_dev;
#endif

	/* Save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	usb_sn9c20x_default_settings(dev);


	return 0;

free_dev:
	kref_put(&dev->vopen, usb_sn9c20x_delete);
error:
	return ret;
}

void usb_sn9c20x_delete(struct kref *kref)
{
	struct usb_sn9c20x *dev;
	dev = container_of(kref, struct usb_sn9c20x, vopen);

	if (dev->vdev != NULL) {
		sn9c20x_remove_sysfs_files(dev->vdev);
		sn9c20x_remove_debugfs_files(dev);
		v4l_sn9c20x_unregister_video_device(dev);
	}
#ifdef CONFIG_SN9C20X_EVDEV
	sn9c20x_input_cleanup(dev);
#endif
	kfree(dev);
}

/**
 * @param interface
 *
 * @brief This function is called when the device is disconnected
 *   or when the kernel module is unloaded.
 */
static void usb_sn9c20x_disconnect(struct usb_interface *interface)
{
	struct usb_sn9c20x *dev = usb_get_intfdata(interface);

	UDIA_INFO("SN9C20X USB 2.0 Webcam unplugged\n");

	usb_set_intfdata(interface, NULL);

	mutex_lock(&open_lock);
	kref_put(&dev->vopen, usb_sn9c20x_delete);
	mutex_unlock(&open_lock);
}

static int usb_sn9c20x_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct usb_sn9c20x *dev = usb_get_intfdata(intf);

	UDIA_INFO("Suspending interface: %u\n",
		  intf->cur_altsetting->desc.bInterfaceNumber);

	if (dev->interface != intf)
		return -EINVAL;

	if (!sn9c20x_queue_streaming(&dev->queue))
		return 0;

	dev->frozen = 1;
	usb_sn9c20x_uninit_urbs(dev, 0);
	usb_set_interface(dev->udev, 0, 0);
	return 0;
}



static int _usb_sn9c20x_resume(struct usb_interface *intf, int reset)
{
	struct usb_sn9c20x *dev = usb_get_intfdata(intf);

	UDIA_INFO("Resuming interface: %u\n",
		  intf->cur_altsetting->desc.bInterfaceNumber);

	if (dev->interface != intf)
		return -EINVAL;

	if (reset && sn9c20x_reset_device(dev) < 0)
		return -EINVAL;

	if (!sn9c20x_queue_streaming(&dev->queue))
		return 0;

	dev->frozen = 0;
	if (usb_sn9c20x_init_urbs(dev) < 0)
		sn9c20x_queue_enable(&dev->queue, 0);

	sn9c20x_enable_video(dev, 1);
	return 0;
}


static int usb_sn9c20x_resume(struct usb_interface *intf)
{
	UDIA_DEBUG("usb_sn9c20x_resume()\n");
	return _usb_sn9c20x_resume(intf, 0);
}

static int usb_sn9c20x_reset_resume(struct usb_interface *intf)
{
	UDIA_DEBUG("usb_sn9c20x_reset_resume()\n");
	return _usb_sn9c20x_resume(intf, 1);
}

/**
 * @var usb_sn9c20x_driver
 *
 * This variable contains some callback
 */
static struct usb_driver usb_sn9c20x_driver = {
	.name = "sn9c20x",
	.probe = usb_sn9c20x_probe,
	.disconnect = usb_sn9c20x_disconnect,
	.suspend = usb_sn9c20x_suspend,
	.resume	= usb_sn9c20x_resume,
	.reset_resume = usb_sn9c20x_reset_resume,
	.id_table = sn9c20x_table,
};

module_param(fps, byte, 0444);			/**< @brief Module parameter frames per second */
module_param(bulk, byte, 0444);
module_param(jpeg, byte, 0444);
module_param(bandwidth, byte, 0444);
module_param(hflip, byte, 0444);		/**< @brief Module parameter horizontal flip process */
module_param(vflip, byte, 0444);		/**< @brief Module parameter vertical flip process */
module_param(flip_detect, byte, 0444);		/**< @brief Module parameter flip detect */
module_param(auto_exposure, byte, 0444);	/**< @brief Module parameter automatic exposure control */
module_param(auto_gain, byte, 0444);		/**< @brief Module parameter automatic gain control */
module_param(auto_whitebalance, byte, 0444);	/**< @brief Module parameter automatic whitebalance control */
module_param(brightness, ushort, 0444);		/**< @brief Module parameter brightness */
module_param(gamma, ushort, 0444);		/**< @brief Module parameter gamma */
module_param(saturation, ushort, 0444);		/**< @brief Module parameter saturation */
module_param(contrast, ushort, 0444);		/**< @brief Module parameter contrast */
module_param(hue, short, 0444);			/**< @brief Module parameter hue */
module_param(exposure, ushort, 0444);		/**< @brief Module parameter exposure */
module_param(sharpness, ushort, 0444);		/**< @brief Module parameter sharpness */
module_param(red_gain, ushort, 0444);		/**< @brief Module parameter red gain */
module_param(blue_gain, ushort, 0444);		/**< @brief Module parameter blue gain */
module_param(gain, ushort, 0444);		/**< @brief Module parameter gain */

module_param(min_buffers, byte, 0444);
module_param(max_buffers, byte, 0444);

module_param(log_level, byte, 0444);

/**
 * @returns 0 if all is OK
 *
 * @brief Initialize the driver.
 *
 * This function is called at first.
 * This function permits to define the default values from the command line.
 */
static int __init usb_sn9c20x_init(void)
{
	int result;

	UDIA_DEBUG(DRIVER_DESC " initializing\n");

	sn9c20x_init_debugfs();

	if (fps < 10 || fps > 30) {
		UDIA_WARNING("Framerate out of bounds [10-30]! Defaulting to 25\n");
		fps = 25;
	}

	if (bandwidth < 1 || bandwidth > 8) {
		UDIA_WARNING("Bandwidth out of bounds [1-8]! Defaulting to 8\n");
		bandwidth = 8;
	}

	if (bulk != 0 && bulk != 1) {
		UDIA_WARNING("Bulk transfer should be 0 or 1! Defaulting to 0\n");
		bulk = 0;
	}

	if (jpeg > 2) {
		UDIA_WARNING("JPEG should be 0 or 1 or 2! Defaulting to 2\n");
		jpeg = 2;
	}

	if (vflip != 0 && vflip != 1) {
		UDIA_WARNING("Vertical flip should be 0 or 1! Defaulting to 0\n");
		vflip = 0;
	}

	if (hflip != 0 && hflip != 1) {
		UDIA_WARNING("Horizontal flip should be 0 or 1! Defaulting to 0\n");
		hflip = 0;
	}

	if (sharpness > 0x3f) {
		UDIA_WARNING("Sharpness should be 0 to 63 ! Defaulting to 31\n");
		sharpness = 0x1f;
	}

	if (red_gain > 0x7f) {
		UDIA_WARNING("Red Gain should be 0 to 127 ! Defaulting to 31\n");
		red_gain = 31;
	}

	if (blue_gain > 0x7f) {
		UDIA_WARNING("Blue Gain should be 0 to 127 ! Defaulting to 31\n");
		blue_gain = 31;
	}

	if (hue > 180 || hue < -180) {
		UDIA_WARNING("Hue should be between -180 and 180 ! Defaulting to 0\n");
		hue = 0;
	}

	if (auto_exposure != 0 && auto_exposure != 1) {
		UDIA_WARNING("Automatic exposure should be 0 or 1! "
			     "Defaulting to 0\n");
		auto_exposure = 1;
	}

	if (auto_gain != 0 && auto_gain != 1) {
		UDIA_WARNING("Automatic gain should be 0 or 1! "
			     "Defaulting to 0\n");
		auto_gain = 0;
	}

	if (auto_whitebalance != 0 && auto_whitebalance != 1) {
		UDIA_WARNING("Automatic whitebalance should be 0 or 1! "
			     "Defaulting to 1\n");
		auto_whitebalance = 1;
	}

	if (min_buffers < 2) {
		UDIA_WARNING("Minimum buffers can't be less then 2! "
			     "Defaulting to 2\n");
		min_buffers = 2;
	}


	if (min_buffers > max_buffers) {
		UDIA_WARNING("Minimum buffers must be less then or equal to "
			     "max buffers! Defaulting to 2, 10\n");
		min_buffers = 2;
		max_buffers = 5;
	}

	/* Register the driver with the USB subsystem */
	result = usb_register(&usb_sn9c20x_driver);

	if (result)
		UDIA_ERROR("usb_register failed ! Error number %d\n", result);

	UDIA_INFO(DRIVER_DESC " " DRIVER_VERSION " loaded\n");

	return result;
}


/**
 * @brief Close the driver
 *
 * This function is called at last when you unload the driver.
 */
static void __exit usb_sn9c20x_exit(void)
{
	UDIA_INFO("usb_sn9c20x_exit: SN9C20X USB 2.0 webcam driver unloaded\n");

	sn9c20x_uninit_debugfs();

	/* Deregister this driver with the USB subsystem */
	usb_deregister(&usb_sn9c20x_driver);
}


module_init(usb_sn9c20x_init);	/**< @brief Module initialize */
module_exit(usb_sn9c20x_exit);	/**< @brief Module exit */


MODULE_PARM_DESC(fps, "Frames per second [10-30]");		/**< @brief Description of 'fps' parameter */
MODULE_PARM_DESC(jpeg, "Enable JPEG support (default is auto-detect)");
MODULE_PARM_DESC(bulk, "Enable Bulk transfer (default is to use ISOC)");
MODULE_PARM_DESC(bandwidth, "Bandwidth Setting (only for ISOC)");
MODULE_PARM_DESC(hflip, "Horizontal image flip");		/**< @brief Description of 'hflip' parameter */
MODULE_PARM_DESC(vflip, "Vertical image flip");			/**< @brief Description of 'vflip' parameter */
MODULE_PARM_DESC(flip_detect, "Image flip detection");		/**< @brief Description of 'vflip_detect' parameter */
MODULE_PARM_DESC(auto_exposure, "Automatic exposure control");	/**< @brief Description of 'auto_exposure' parameter */
MODULE_PARM_DESC(auto_gain, "Automatic gain control");		/**< @brief Description of 'auto_gain' parameter */
MODULE_PARM_DESC(auto_whitebalance, "Automatic whitebalance");	/**< @brief Description of 'auto_whitebalance' parameter */
MODULE_PARM_DESC(brightness, "Brightness setting");		/**< @brief Description of 'brightness' parameter */
MODULE_PARM_DESC(gamma, "Gamma setting");			/**< @brief Description of 'gamma' parameter */
MODULE_PARM_DESC(saturation, "Saturation setting");		/**< @brief Description of 'saturation' parameter */
MODULE_PARM_DESC(hue, "Hue setting");				/**< @brief Description of 'hue' parameter */
MODULE_PARM_DESC(exposure, "Exposure setting");			/**< @brief Description of 'exposure' parameter */
MODULE_PARM_DESC(gain, "Gain setting");				/**< @brief Description of 'gain' parameter */
MODULE_PARM_DESC(contrast, "Contrast setting");			/**< @brief Description of 'contrast' parameter */
MODULE_PARM_DESC(sharpness, "Sharpness setting");		/**< @brief Description of 'sharpness' parameter */
MODULE_PARM_DESC(red_gain, "Red Gain setting"); 		/**< @brief Description of 'Red Gain' parameter */
MODULE_PARM_DESC(blue_gain, "Blue Gain setting"); 		/**< @brief Description of 'Blue Gain' parameter */

MODULE_PARM_DESC(min_buffers, "Minimum number of image buffers");
MODULE_PARM_DESC(max_buffers, "Maximum number of image buffers");
MODULE_PARM_DESC(log_level, " <n>\n"
			    "Driver log level\n"
			    "1  = info (default)\n"
			    "2  = warning\n"
			    "4  = error\n"
			    "8  = debug\n"
			    "16 = stream\n");

MODULE_LICENSE("GPL");					/**< @brief Driver is under licence GPL */
MODULE_AUTHOR(DRIVER_AUTHOR);				/**< @brief Driver is written by Nicolas VIVIEN */
MODULE_DESCRIPTION(DRIVER_DESC);			/**< @brief Define the description of the driver */
MODULE_SUPPORTED_DEVICE(DRIVER_SUPPORT);		/**< @brief List of supported device */


