/**
 * @file sn9c20x.h
 * @author Nicolas VIVIEN
 * @date 2008-02-01
 *
 * @brief Driver for SN9C20X USB video cameras
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

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/usb.h>
/**for kzalloc**/
#include <linux/slab.h>
#ifdef CONFIG_SN9C20X_EVDEV
#include <linux/input.h>
#endif
#include <media/v4l2-common.h>

#ifndef SN9C20X_H
#define SN9C20X_H

#define DRIVER_NAME		"sn9c20x"			/**< Name of this driver */
#define DRIVER_VERSION		"v2014.07"			/**< Version of this driver */
#define DRIVER_VERSION_NUM	0x201407			/**< Version numerical of this driver */
#define DRIVER_DESC		"SN9C20x USB 2.0 Webcam Driver"	/**< Short description of this driver */
#define DRIVER_AUTHOR		"Nicolas VIVIEN and microdia project" \
				" <microdia@googlegroups.com>"	/**< Author of this driver */
#define PREFIX			DRIVER_NAME ": "		/**< Prefix use for the SN9C20X "printk" */

#define SN9C20X_USB_DEVICE(vend, prod, sensor, address)		\
	.match_flags = USB_DEVICE_ID_MATCH_DEVICE |		\
		       USB_DEVICE_ID_MATCH_INT_CLASS,		\
	.idVendor = (vend),					\
	.idProduct = (prod),					\
	.bInterfaceClass = 0xff,				\
	.driver_info = ((address << 8) | sensor)

/** V4L2-CONTROLS for compatibility with older kernels: */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 25)
#define V4L2_CID_SHARPNESS		(V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_EXPOSURE_AUTO		(V4L2_CID_PRIVATE_BASE + 1)
#endif

#ifndef V4L2_PIX_FMT_SN9C20X_I420
#define V4L2_PIX_FMT_SN9C20X_I420  v4l2_fourcc('S', '9', '2', '0')
#endif

/**
 * @def MAX_URBS
 *   Number maximal of URBS
 *
 * @def ISO_FRAMES_PER_DESC
 *   Number frames per ISOC descriptor
 */
#define MAX_URBS				10
#define ISO_FRAMES_PER_DESC			10

/**
 * @def hb_multiplier(wMaxPacketSize)
 *   USB endpoint high bandwidth multiplier
 *
 * @def max_packet_sz(wMaxPacketSize)
 *   USB endpoint maximum packet size
 *
 * These values are both encoded within the wMaxPacketSize field of the usb_endpoint_descriptor structure.
 * The 11(10:0) lowest bits hold the MaxPacketSize(according to the usb specs a value over 1024 is undefined),
 * and the bits 12:11 will give the high bandwidth multiplier(this should be a value of 0-2).
 */

#define hb_multiplier(wMaxPacketSize) (((wMaxPacketSize >> 11) & 0x03) + 1)
#define max_packet_sz(wMaxPacketSize) ((wMaxPacketSize) & 0x07ff)

/**
 * @def DRIVER_SUPPORT
 *
 * List of supported devices
 */
#define DRIVER_SUPPORT		"SN9C20X USB Camera"

#define SN9C20X_INFO		(1 << 0)
#define SN9C20X_WARNING		(1 << 1)
#define SN9C20X_ERROR		(1 << 2)
#define SN9C20X_DEBUG		(1 << 3)
#define SN9C20X_STREAM		(1 << 4)

/**
 * @def UDIA_INFO(str, args...)
 *   Print information message.
 *   @a Use this function like the function printf.
 *
 * @def UDIA_ERROR(str, args...)
 *   Print error message.
 *   @a Use this function like the function printf.
 *
 * @def UDIA_WARNING(str, args...)
 *   Print warning message.
 *   @a Use this function like the function printf.
 *
 * @def UDIA_DEBUG(str, args...)
 *   Print debug message.
 *   @a Use this function like the function printf.
 *
 * @def UDIA_STREAM(str, args...)
 *   Print stream debug message.
 *   @a Use this function like the function printf.
 */
extern __u8 log_level;

#define UDIA_INFO(str, args...)	\
do { \
	if (log_level & SN9C20X_INFO) \
		printk(KERN_INFO PREFIX str, ##args); \
} while (0)

#define UDIA_WARNING(str, args...) \
do { \
	if (log_level & SN9C20X_WARNING) \
		printk(KERN_WARNING PREFIX str, ##args); \
} while (0)

#define UDIA_ERROR(str, args...) \
do { \
	if (log_level & SN9C20X_ERROR) \
		printk(KERN_ERR PREFIX str, ##args); \
} while (0)

#define UDIA_DEBUG(str, args...) \
do { \
	if (log_level & SN9C20X_DEBUG) \
		printk(KERN_DEBUG PREFIX str, ##args); \
} while (0)

#define UDIA_STREAM(str, args...) \
do { \
	if (log_level & SN9C20X_STREAM) \
		printk(KERN_DEBUG PREFIX str, ##args); \
} while (0)


enum sn9c20x_buffer_state {
	SN9C20X_BUF_STATE_IDLE		= 0,
	SN9C20X_BUF_STATE_QUEUED	= 1,
	SN9C20X_BUF_STATE_ACTIVE	= 2,
	SN9C20X_BUF_STATE_DONE		= 3,
	SN9C20X_BUF_STATE_ERROR		= 4,
};

enum sn9c20x_mode {
	SN9C20X_MODE_IDLE	= 0,
	SN9C20X_MODE_READ	= 1,
	SN9C20X_MODE_STREAM	= 2,
};

struct sn9c20x_buffer {
	unsigned long vma_use_count;
	struct list_head stream;

	/* Touched by interrupt handler. */
	struct v4l2_buffer buf;
	struct list_head queue;
	wait_queue_head_t wait;
	enum sn9c20x_buffer_state state;
};

#define SN9C20X_QUEUE_STREAMING	(1 << 0)
#define SN9C20X_QUEUE_DISCONNECTED	(1 << 1)
#define SN9C20X_QUEUE_DROP_INCOMPLETE	(1 << 2)

struct sn9c20x_video_queue {
	void *mem;
	unsigned int flags;
	__u32 sequence;

	unsigned int count;
	unsigned int min_buffers;
	unsigned int max_buffers;
	unsigned int buf_size;

	struct sn9c20x_buffer *buffer;
	struct sn9c20x_buffer *read_buffer;
	struct mutex mutex;	/* protects buffers and mainqueue */
	spinlock_t irqlock;	/* protects irqqueue */

	struct list_head mainqueue;
	struct list_head irqqueue;
};

/**
 * @struct sn9c20x_urb
 */
struct sn9c20x_urb {
	void *data;
	struct urb *urb;
};

/**
 * @struct sn9c20x_video
 */
struct sn9c20x_video {
	struct v4l2_pix_format format;
	__u8 cmatrix[21];

	int fps;			/**< FPS setting */
	int brightness;			/**< Brightness setting */
	int contrast;			/**< Contrast setting */
	int gamma;			/**< Gamma setting */
	int colour;			/**< Colour setting */
	int hue;			/**< Hue setting */
	int hflip;			/**< Horizontal flip */
	int vflip;			/**< Vertical flip */
	int exposure;			/**< Exposure */
	int gain;			/**< Gain */
	int sharpness;			/**< Sharpness */
	int red_gain;			/**< Red Gain Control */
	int blue_gain;			/**< Blue Gain Control */
	int auto_exposure;		/**< Automatic exposure */
	int auto_gain;			/**< Automatic gain */
	int auto_whitebalance;		/**< Automatic whitebalance */
};

/**
 * @struct sn9c20x_debugfs
 */
struct sn9c20x_debugfs {
	struct dentry *dent_device;
	struct dentry *dent_bridge_addr;
	struct dentry *dent_bridge_val;
	struct dentry *dent_bridge_dump;
	struct dentry *dent_sensor_addr;
	struct dentry *dent_sensor_val8;
	struct dentry *dent_sensor_val16;
	struct dentry *dent_sensor_val32;

	__u16 bridge_addr;	/**< Current bridge register address */
	__u8 sensor_addr;	/**< Current sensor register address */
};

enum sn9c20x_endpoints {
	SN9C20X_VID_ISOC = 1,
	SN9C20X_BULK = 2,
	SN9C20X_INT = 3,
	SN9C20X_AUD_ISOC = 4
};

enum sn9c20x_sensors {
	PROBE_SENSOR		= 0,
	OV9650_SENSOR		= 1,
	OV9655_SENSOR		= 2,
	SOI968_SENSOR		= 3,
	OV7660_SENSOR		= 4,
	OV7670_SENSOR		= 5,
	MT9M111_SENSOR		= 6,
	MT9V111_SENSOR		= 7,
	MT9V011_SENSOR		= 8,
	MT9M001_SENSOR		= 9,
	HV7131R_SENSOR		= 10,
	MT9V112_SENSOR		= 11,
};



struct usb_sn9c20x;

struct sn9c20x_video_mode {
	__u16 width;
	__u16 height;
	__u8 scale;
	__u16 window[4];
};

struct sn9c20x_video_format {
	__u32 pix_fmt;
	char desc[32];
	__u8 depth;
	void (*set_format) (struct usb_sn9c20x *dev);
};

struct sn9c20x_i2c_regs {
	__u8 address;
	__u16 value;
};

#define SN9C20X_N_FMTS	3
#define SN9C20X_N_MODES	7

extern struct sn9c20x_video_format sn9c20x_fmts[SN9C20X_N_FMTS];
extern struct sn9c20x_video_mode sn9c20x_modes[SN9C20X_N_MODES];

struct sn9c20x_camera {

/* Sensor ID */
	__u8 sensor;

/* SCCB/I2C interface */
	__u8 i2c_flags;
	__u8 address;

	int min_yavg, max_yavg, old_step, older_step;
	unsigned int exposure_step;

	atomic_t yavg;

	int vstart;
	int hstart;

	int (*flip_detect) (struct usb_sn9c20x *dev);
	int (*set_hvflip) (struct usb_sn9c20x *dev);
	void (*set_sxga_mode) (struct usb_sn9c20x *dev, bool sxga);
/* image quality functions */
	int (*set_exposure) (struct usb_sn9c20x *dev);
	int (*set_gain) (struct usb_sn9c20x *dev);
	int (*set_auto_exposure) (struct usb_sn9c20x *dev);
	int (*set_auto_gain) (struct usb_sn9c20x *dev);
	int (*set_auto_whitebalance) (struct usb_sn9c20x *dev);
	int (*set_contrast) (struct usb_sn9c20x *dev);
	int (*set_brightness) (struct usb_sn9c20x *dev);
	int (*set_gamma) (struct usb_sn9c20x *dev);
	int (*set_saturation) (struct usb_sn9c20x *dev);
	int (*set_sharpness) (struct usb_sn9c20x *dev);
	int (*set_red_gain) (struct usb_sn9c20x *dev);
	int (*set_blue_gain) (struct usb_sn9c20x *dev);
	int (*set_hue) (struct usb_sn9c20x *dev);
};

/**
 * @struct usb_sn9c20x
 */
struct usb_sn9c20x {
#ifdef CONFIG_SN9C20X_EVDEV
	struct input_dev *input_dev;
	__u8 input_gpio;
	struct task_struct *input_task;
#endif
	struct video_device *vdev; 	/**< Pointer on a V4L2 video device */
	struct usb_device *udev;	/**< Pointer on a USB device */
	struct usb_interface *interface;/**< Pointer on a USB interface */

	int release;			/**< Release of the device (bcdDevice) */

	struct sn9c20x_video vsettings;	/**< Video settings (brightness, whiteness...) */
	struct sn9c20x_debugfs debug; 	/**< debugfs information structure */

	struct kref vopen;		/**< Video status (Opened or Closed) */
	struct file *owner;		/**< file handler of stream owner */
	enum sn9c20x_mode mode;		/**< camera mode */

	int vframes_overflow;		/**< Buffer overflow frames */
	int vframes_incomplete;		/**< Incomplete frames */
	int vframes_dropped;		/**< Dropped frames */

	struct sn9c20x_urb urbs[MAX_URBS];

	__u8 jpeg;

	unsigned int frozen:1;
	struct sn9c20x_video_queue queue;
	struct sn9c20x_camera camera;
};

/**
 * @def SN9C20X_PERCENT
 *   Calculate a value from a percent
 */
#define SN9C20X_PERCENT(x, y) (((int)x * (int)y) / 100)


extern struct mutex open_lock;
extern __u8 jpeg;

int usb_sn9c20x_control_write(struct usb_sn9c20x *, __u16, __u8 *, __u16);
int usb_sn9c20x_control_read(struct usb_sn9c20x *, __u16, __u8 *, __u16);

int usb_sn9c20x_isoc_init(struct usb_sn9c20x *,
	struct usb_endpoint_descriptor *);
void usb_sn9c20x_completion_handler(struct urb *);
int usb_sn9c20x_init_urbs(struct usb_sn9c20x *);
void usb_sn9c20x_uninit_urbs(struct usb_sn9c20x *, int);
void usb_sn9c20x_delete(struct kref *);

int sn9c20x_initialize(struct usb_sn9c20x *dev);
int sn9c20x_initialize_sensor(struct usb_sn9c20x *dev);
int sn9c20x_enable_video(struct usb_sn9c20x *dev, int enable);

int dev_sn9c20x_call_constantly(struct usb_sn9c20x *dev);
int dev_sn9c20x_flip_detection(struct usb_sn9c20x *dev);
int dev_sn9c20x_camera_set_exposure(struct usb_sn9c20x *);
int dev_sn9c20x_camera_set_gain(struct usb_sn9c20x *);
int dev_sn9c20x_camera_set_hvflip(struct usb_sn9c20x *);
int dev_sn9c20x_camera_set_auto_exposure(struct usb_sn9c20x *dev);
int dev_sn9c20x_camera_set_auto_gain(struct usb_sn9c20x *dev);
int dev_sn9c20x_camera_set_auto_whitebalance(struct usb_sn9c20x *dev);
int dev_sn9c20x_perform_soft_ae(struct usb_sn9c20x *dev);

void v4l2_set_control_default(struct usb_sn9c20x *, __u32, __u16);
int v4l_sn9c20x_select_video_mode(struct usb_sn9c20x *, int);
int v4l_sn9c20x_register_video_device(struct usb_sn9c20x *);
int v4l_sn9c20x_unregister_video_device(struct usb_sn9c20x *);

int sn9c20x_create_sysfs_files(struct video_device *);
void sn9c20x_remove_sysfs_files(struct video_device *);

#ifdef CONFIG_SN9C20X_DEBUGFS
void sn9c20x_init_debugfs(void);
void sn9c20x_uninit_debugfs(void);
int sn9c20x_create_debugfs_files(struct usb_sn9c20x *);
int sn9c20x_remove_debugfs_files(struct usb_sn9c20x *);
#else
static inline void sn9c20x_init_debugfs(void) {}
static inline void sn9c20x_uninit_debugfs(void) {}
static inline int sn9c20x_create_debugfs_files(struct usb_sn9c20x *dev)
{
	return 0;
}
static inline int sn9c20x_remove_debugfs_files(struct usb_sn9c20x *dev)
{
	return 0;
}
#endif

void sn9c20x_queue_init(struct sn9c20x_video_queue *);
int sn9c20x_alloc_buffers(struct sn9c20x_video_queue *,
	unsigned int, unsigned int);
int sn9c20x_free_buffers(struct sn9c20x_video_queue *);
int sn9c20x_queue_enable(struct sn9c20x_video_queue *, int);
void sn9c20x_queue_cancel(struct sn9c20x_video_queue *, int);
unsigned int sn9c20x_queue_poll(struct sn9c20x_video_queue *,
	struct file *, poll_table *);
int sn9c20x_query_buffer(struct sn9c20x_video_queue *, struct v4l2_buffer *);
int sn9c20x_queue_buffer(struct sn9c20x_video_queue *, struct v4l2_buffer *);
int sn9c20x_dequeue_buffer(struct sn9c20x_video_queue *,
	struct v4l2_buffer *, int);
struct sn9c20x_buffer *sn9c20x_queue_next_buffer(
	struct sn9c20x_video_queue *, struct sn9c20x_buffer *);

static inline int sn9c20x_queue_streaming(struct sn9c20x_video_queue *queue)
{
	return queue->flags & SN9C20X_QUEUE_STREAMING;
}

/* sensor hv7131r has no header-file of its own: */
extern struct sn9c20x_i2c_regs hv7131r_init[];
int hv7131r_initialize(struct usb_sn9c20x *dev);
int hv7131r_set_exposure(struct usb_sn9c20x *dev);
int hv7131r_set_gain(struct usb_sn9c20x *dev);
int hv7131r_set_hvflip(struct usb_sn9c20x *dev);

#endif
