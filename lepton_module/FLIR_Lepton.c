/* FLIR_Lepton.c
   Main source file for the FLIR Lepton VoSPI driver
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/spi/spi.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>
#include <media/videobuf2-memops.h>

#include "FLIR_Lepton.h"
#include "lepton_vospi_funcs.h"

enum lepton_model {
	FLIR_LEPTON2	= 2,
	FLIR_LEPTON3	= 3,
};

struct spare_spi_buffer {
	unsigned	len;
	void		*rx_buf;
	dma_addr_t	rx_dma;
};

struct lepton {
	struct mutex mutex;
	spinlock_t lock;
	bool started;
	bool synced;
	bool telemetry_enabled;
	struct list_head unfilled_bufs; /* waiting to be filled with data */
	struct spare_spi_buffer spare_buf; /* when not using unfilled_bufs */
	struct v4l2_device *v4l2_dev;
	struct video_device *vid_dev;
	struct vb2_queue *q;
	struct spi_device *spi_dev;
	unsigned int vsync_count;
	unsigned int discard_count;
	lepton_vospi_info lep_vospi_info;
	struct lepton_buffer *current_lep_buf;
	struct spi_transfer *spi_xfer;
	struct spi_message *spi_msg;
	struct timespec last_spi_done_ts;
};

struct lepton_buffer {
	struct vb2_buffer buf;
	struct list_head list;
};

/*
 * interface to userspace apps: V4L2 video device
 */

static int lepton_querycap(struct file *file, void *priv,
			struct v4l2_capability *cap)
{
	strlcpy(cap->driver, LEPTON_MODULE_NAME, sizeof(cap->driver));
	strlcpy(cap->card, "FLIR Lepton", sizeof(cap->driver));
	snprintf(cap->bus_info, sizeof(cap->bus_info), "platform:%s", LEPTON_MODULE_NAME);
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int lepton_enum_input(struct file *file, void *priv,
			 struct v4l2_input *inp)
{
	if (inp->index > 0)
		return -EINVAL;

	inp->type = V4L2_INPUT_TYPE_CAMERA;
	snprintf(inp->name, sizeof(inp->name), LEPTON_MODULE_NAME);
	inp->capabilities = 0;
	return 0;
}

static int lepton_s_input(struct file *file, void *priv, unsigned int i)
{
	// only 1 input
	if (i != 0)
		return -EINVAL;
	return 0;
}
static int lepton_g_input(struct file *file, void *priv, unsigned int *i)
{
	// only 1 input
	*i = 0;
	return 0;
}
static int lepton_querystd(struct file *file, void *fh, v4l2_std_id *std)
{
	// nothing to say about the video standard
	return -ENODATA;
}
static int lepton_s_std(struct file *file, void *fh, v4l2_std_id std)
{
	return -ENODATA;
}
static int lepton_g_std(struct file *file, void *fh, v4l2_std_id *std)
{
	return -ENODATA;
}
static int lepton_enum_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;
	f->pixelformat = V4L2_PIX_FMT_Y16;
	return 0;
}
static int lepton_set_fmt_fields(struct lepton *lep, struct v4l2_format *f)
{
	struct v4l2_pix_format *pix = NULL;

	pix = &f->fmt.pix;

	pix->width = lep->lep_vospi_info.pixel_width;
	pix->height = lep->lep_vospi_info.line_count;
	pix->pixelformat = V4L2_PIX_FMT_Y16;  // 16-bit grayscale
	pix->colorspace = V4L2_COLORSPACE_RAW;
	pix->bytesperline = lep->lep_vospi_info.pixel_width * 2;
	pix->sizeimage = lep->lep_vospi_info.total_data_byte_size;
	return 0;
}
static int lepton_s_parm(struct file *file, void *priv,
			     struct v4l2_streamparm *parm)
{
	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 30;
	parm->parm.capture.readbuffers  = 1;

	return 0;
}
static int lepton_g_parm(struct file *file, void *priv,
			     struct v4l2_streamparm *parm)
{
	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 30;
	parm->parm.capture.readbuffers  = 1;
	return 0;
}
static int lepton_g_fmt_vid_cap(struct file *file, void *priv,
			     struct v4l2_format *f)
{
	struct lepton *lep = NULL;

	lep = video_drvdata(file);

	lepton_set_fmt_fields(lep, f);
	return 0;
}
static int lepton_try_fmt_vid_cap(struct file *file, void *priv,
			       struct v4l2_format *f)
{
	struct lepton *lep = NULL;

	lep = video_drvdata(file);
	// we only ever have one format, so always send it back.
	lepton_set_fmt_fields(lep, f);
	return 0;
}
static int lepton_s_fmt_vid_cap(struct file *file, void *priv,
			     struct v4l2_format *f)
{
	struct lepton *lep = NULL;

	lep = video_drvdata(file);
	// we only ever have one format, so always send it back.
	lepton_set_fmt_fields(lep, f);
	return 0;
}
static int lepton_enum_frameintervals(struct file *file, void *priv,
				   struct v4l2_frmivalenum *f)
{
	if (f->index != 0)
		return -EINVAL;
	f->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	// always runs at 30Hz
	f->discrete.numerator = 1;
	f->discrete.denominator = 30;
	return 0;
}
static int lepton_enum_framesizes(struct file *file, void *priv,
			       struct v4l2_frmsizeenum *f)
{
	struct lepton *lep = NULL;

	lep = video_drvdata(file);
	if (f->index != 0)
		return -EINVAL;
	f->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	f->discrete.width = lep->lep_vospi_info.pixel_width;
	f->discrete.height = lep->lep_vospi_info.line_count;
	return 0;
}

static struct v4l2_file_operations lepton_fops = {
	.owner =    THIS_MODULE,
	.open =     v4l2_fh_open,
	.release =  vb2_fop_release,
	.read =     vb2_fop_read,    
	.poll =     vb2_fop_poll,
	.mmap =     vb2_fop_mmap,
	.unlocked_ioctl = video_ioctl2,
};

static const struct v4l2_ioctl_ops lepton_ioctl_ops = {
	.vidioc_s_parm			= lepton_s_parm,
	.vidioc_g_parm			= lepton_g_parm,
	.vidioc_querycap	= lepton_querycap,
	.vidioc_enum_input	= lepton_enum_input,
	.vidioc_g_input		= lepton_g_input,
	.vidioc_s_input		= lepton_s_input,

	.vidioc_querystd	= lepton_querystd,
	.vidioc_g_std		= lepton_g_std,
	.vidioc_s_std		= lepton_s_std,

	.vidioc_enum_fmt_vid_cap = lepton_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap	= lepton_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap	= lepton_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap	= lepton_s_fmt_vid_cap,
	.vidioc_enum_frameintervals	= lepton_enum_frameintervals,
	.vidioc_enum_framesizes		= lepton_enum_framesizes,

	.vidioc_reqbufs		= vb2_ioctl_reqbufs,
	.vidioc_create_bufs	= vb2_ioctl_create_bufs,
	.vidioc_prepare_buf	= vb2_ioctl_prepare_buf,
	.vidioc_querybuf	= vb2_ioctl_querybuf,
	.vidioc_qbuf		= vb2_ioctl_qbuf,
	.vidioc_dqbuf		= vb2_ioctl_dqbuf,
	.vidioc_expbuf		= vb2_ioctl_expbuf,

	.vidioc_streamon	= vb2_ioctl_streamon,
	.vidioc_streamoff	= vb2_ioctl_streamoff,

	.vidioc_log_status	= v4l2_ctrl_log_status,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};

static struct video_device lepton_videodev_template = {
	.name		= LEPTON_MODULE_NAME,
	.fops		= &lepton_fops,
	.ioctl_ops	= &lepton_ioctl_ops,
	.minor		= -1,
	.release	= video_device_release,
};

/*
 * video buffer queue management
 */

int lepton_queue_setup(struct vb2_queue *vq,
			   unsigned int *nbuffers, unsigned int *nplanes,
			   unsigned int sizes[], struct device *alloc_devs[])
{
	struct lepton *lep = NULL;
	unsigned int size = -1;

	lep = vb2_get_drv_priv(vq);
	size = lep->lep_vospi_info.total_data_byte_size;
	if (*nplanes)
	{
		// only allow 1 plane per buffer, and verify size is large enough
		if ((*nplanes != 1) || (sizes[0] < size))
			return -EINVAL;
		size = sizes[0];
	}
	if (vq->num_buffers + *nbuffers < 2)
		*nbuffers = 2 - vq->num_buffers;
	*nplanes = 1;
	sizes[0] = size;
	pr_debug("get %d buffers, each holding %d bytes.\n", *nbuffers, sizes[0]);
	return 0;
}

int lepton_buf_prepare(struct vb2_buffer *vb)
{
	struct lepton *lep = NULL;

	lep = vb2_get_drv_priv(vb->vb2_queue);
	if (vb2_plane_size(vb, 0) < lep->lep_vospi_info.total_data_byte_size)
	{
		pr_debug("%s: data will not fit into buf size %ld\n", __func__, vb2_plane_size(vb, 0));
		return -EINVAL;
	}

	/* amount of data that will be filled in this buffer,
	 * which will get passed to userspace client in buffer descriptor */
	vb2_set_plane_payload(vb, 0, lep->lep_vospi_info.total_data_byte_size);
	return 0;
}

void lepton_buf_queue(struct vb2_buffer *vb)
{
	struct lepton_buffer *buf = container_of(vb, struct lepton_buffer, buf);
	struct lepton *lep = vb2_get_drv_priv(vb->vb2_queue);
	unsigned long flags;

	spin_lock_irqsave(&lep->lock, flags);
	list_add_tail(&buf->list, &lep->unfilled_bufs);
	spin_unlock_irqrestore(&lep->lock, flags);
}

int lepton_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct lepton *lep = vb2_get_drv_priv(vq);
	unsigned long flags;

	spin_lock_irqsave(&lep->lock, flags);
	lep->started = 1;
	spin_unlock_irqrestore(&lep->lock, flags);
	return 0;
}

void lepton_stop_streaming(struct vb2_queue *vq)
{
	struct lepton *lep = vb2_get_drv_priv(vq);
	struct lepton_buffer *lep_buf = NULL;
	struct list_head *pos, *q;
	unsigned long flags;

	spin_lock_irqsave(&lep->lock, flags);
	list_for_each_safe(pos, q, &lep->unfilled_bufs) {
		lep_buf = list_entry(pos, struct lepton_buffer, list);
		vb2_buffer_done(&lep_buf->buf, VB2_BUF_STATE_ERROR);
		list_del(&lep_buf->list);
	}

	lep->started = 0;
	spin_unlock_irqrestore(&lep->lock, flags);
	vb2_wait_for_all_buffers(lep->q);
}

static const struct vb2_ops lepton_video_qops = {
	.queue_setup     = lepton_queue_setup,
	.buf_prepare     = lepton_buf_prepare,
	.buf_queue       = lepton_buf_queue,
	.start_streaming = lepton_start_streaming,
	.stop_streaming	 = lepton_stop_streaming,
	.wait_prepare    = vb2_ops_wait_prepare,
	.wait_finish     = vb2_ops_wait_finish,
};

/*
 * tables to find matching device-tree entries
 *
 * loaded device-tree will be searched for matching "compatible" strings,
 * and driver probe function will be called on any matched nodes
 */

static const struct spi_device_id lepton_id_table[] = {
	{
		.name		= "lepton2",
		.driver_data	= (kernel_ulong_t)FLIR_LEPTON2,
	},
	{
		.name		= "lepton3",
		.driver_data	= (kernel_ulong_t)FLIR_LEPTON3,
	},
	{ }
};
MODULE_DEVICE_TABLE(spi, lepton_id_table);

static const struct of_device_id lepton_of_match[] = {
	{
		.compatible	= "flir,lepton2",
		.data		= (void *)FLIR_LEPTON2,
	},
	{
		.compatible	= "flir,lepton3",
		.data		= (void *)FLIR_LEPTON3,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, lepton_of_match);

/*
 * toplevel module setup and teardown
 */

static void lepton_spi_done_callback(void *context)
{
	struct lepton *lep = (struct lepton *)context;
	unsigned long flags;
	unsigned short *subframe_data = NULL;
	struct lepton_buffer *lep_buf = NULL;
	struct timespec now;
	bool subframe_is_good = false;

	ktime_get_ts(&now);

	spin_lock_irqsave(&lep->lock, flags);
	lep->last_spi_done_ts.tv_sec = now.tv_sec;
	lep->last_spi_done_ts.tv_nsec = now.tv_nsec;

	/* current_lep_buf will already be NULL if spare buffer is in use;
	 * non-NULL if instead data was transferred to a V4L-allocated buffer
	 * for consumption by userspace
	 */
	lep_buf = lep->current_lep_buf;
	lep->current_lep_buf = NULL;

	/* analyze data to decide if data is synced up into proper subframes yet,
	 * so that data can be sent to userspace when V4L buffers are available
	 */
	subframe_data = lep->spi_xfer->rx_buf;
    subframe_is_good = is_subframe_line_counter_valid(&lep->lep_vospi_info, subframe_data);

	if (subframe_is_good) {
		lep->synced = true;
		lep->discard_count = 0;
	}
	else {
		lep->synced = false;
		lep->discard_count++;
	}

	spin_unlock_irqrestore(&lep->lock, flags);

	/* V4L buffers need to be dispatched back to userspace,
	 * marking it as good if validation check passed and otherwise
	 * noting an error (userspace will thus be informed when 
	 * synced video unexpectedly goes out of sync)
	 */
	if (lep_buf) {
		if (subframe_is_good) {
			vb2_buffer_done(&lep_buf->buf, VB2_BUF_STATE_DONE); 
		}
		else {
			pr_debug("Lost frame sync!\n");
			vb2_buffer_done(&lep_buf->buf, VB2_BUF_STATE_ERROR); 
		}
	}
}

/* kick off a SPI transfer in interrupt context */
static void lepton_start_transfer(struct lepton *lep, void *rx_buf, dma_addr_t rx_dma, size_t rx_len)
{
	unsigned long flags;
	/* SPI message consists of one or more transfers,
	 * in this case only one */

	spin_lock_irqsave(&lep->lock, flags);
	spi_message_init(lep->spi_msg);
	lep->spi_msg->complete = lepton_spi_done_callback;
	lep->spi_msg->context = (void *)lep;
	lep->spi_msg->is_dma_mapped = 1;

	lep->spi_xfer->rx_buf = rx_buf;
	lep->spi_xfer->rx_dma = rx_dma;
	lep->spi_xfer->len = rx_len;
	memset(rx_buf, 0, rx_len);

	/* assign this one transfer to message and send it to controller */
	spi_message_add_tail(lep->spi_xfer, lep->spi_msg);
	spin_unlock_irqrestore(&lep->lock, flags);
	spi_async(lep->spi_dev, lep->spi_msg);
}

static int lepton_timing_ok(struct lepton *lep, struct timespec *now)
{
	struct timespec delta;
	int timing_ok = 1;

	if (lep->last_spi_done_ts.tv_sec == 0) {
		pr_debug("VSYNC %d miss!\n", lep->vsync_count);
		timing_ok = 0;
	}
	else {
		delta = timespec_sub(*now, lep->last_spi_done_ts);
		if (delta.tv_nsec < MINIMUM_SPI_TRANSFER_QUIET_TIME) {
			pr_debug("VSYNC warning!\n");
		}
	}
	return timing_ok;
}

static irqreturn_t lepton_vsync_handler(int irq, void *data)
{
	struct spi_device *spi = (struct spi_device *)data;
	struct device *dev = NULL;
	struct lepton *lep = NULL;
	struct lepton_buffer *lep_buf = NULL;
	unsigned long flags;
	unsigned long *vaddr = NULL;
	dma_addr_t dma_addr;
	int synced = 0;
	unsigned rx_len;
	struct timespec now;

	ktime_get_ts(&now); /* time at beginning of IRQ handler */
	dev = &spi->dev;
	lep = dev_get_drvdata(dev);

#if 0
	if (printk_ratelimit()) {
		pr_debug("VSYNC %d", lep->vsync_count);
		// printk(KERN_INFO "spi=%p dev=%p lep=%p\n", spi, dev, lep);
	}
#endif

	spin_lock_irqsave(&lep->lock, flags);
	lep->vsync_count++;

	/* Do not kick off another DMA if the previous has not
	 * finished (which is a SERIOUS problem, since missing subframes
	 * can knock lepton into a bad state that requires hardware reset 
	 */
	if (!lepton_timing_ok(lep, &now)) {
		spin_unlock_irqrestore(&lep->lock, flags);
		return IRQ_HANDLED;
	}

	/* If video is synced and there are V4L buffers available,
	 * take the first one to fill with data
	 */
	if (lep->synced && !list_empty(&lep->unfilled_bufs)) {
		lep_buf = list_first_entry(&lep->unfilled_bufs, struct lepton_buffer, list);
		list_del(&lep_buf->list);
		lep->current_lep_buf = lep_buf;
	}
	synced = lep->synced;  /* cache this for use outside spinlock */
	lep->last_spi_done_ts.tv_sec = 0; /* reset timer for upcoming spi transfer */
	spin_unlock_irqrestore(&lep->lock, flags);

	/* driver provides a spare buffer for two purposes:
	 * - achieving sync of video frames
	 * - place to stash SPI data when no V4L buffers are available
	 *
	 * Achieving sync:
	 *   When video streaming starts up, there can be some extra lines
	 * of data before the real start of frame (marked by line counter=0).
	 * In that case driver will "catch up" by using the spare buffer,
	 * which has one extra line worth of space. Reading out full frame
	 * plus a line will gradually drain out the extra data until a frame 
	 * starts with line 0 at the beginning. On that first synced frame,
	 * the extra transferred line will be a "discard packet" which can 
	 * just be ignored.
	 *
	 * Fallback when no V4L buffers are available:
	 *   After the extra lines have been cleared out, frame-sized
	 * buffers allocated by V4L layer can be used to receive data
	 * that will then be passed to userspace. However, it is up
	 * to userspace to allocate V4L buffers and to keep returning
	 * them to the driver. The lepton is most stable if data
	 * is *always* clocked out in a timely manner, so the spare buffer
	 * is used to clock out data when no V4L buffer is available.
	 */
	if (lep_buf && synced) {
		/* kick off spi read to V4L buffer */
		vaddr = vb2_plane_vaddr(&lep_buf->buf, 0);
		dma_addr = vb2_dma_contig_plane_dma_addr(&lep_buf->buf, 0);
		// printk(KERN_INFO "SPI read into %p (%u)\n", vaddr, dma_addr);
		rx_len = lep->lep_vospi_info.subframe_data_byte_size;
		memset(vaddr, 0xa5, rx_len); //@@@@@
		lepton_start_transfer(lep, vaddr, dma_addr, rx_len);
	}
	else {
		/* kick off spi read to spare buffer, which has an extra line */
		rx_len = lep->spare_buf.len;
		lepton_start_transfer(lep, lep->spare_buf.rx_buf, lep->spare_buf.rx_dma, rx_len);
	}

	return IRQ_HANDLED;
}

static int lepton_probe(struct spi_device *spi)
{
	struct lepton *lep = NULL;
	struct device *dev = NULL;
	struct device_node *of_node = NULL;
	struct v4l2_device *v4l2_dev = NULL;
	struct video_device *vid_dev = NULL;
	struct vb2_queue *q = NULL;
	struct spi_transfer *spi_xfer = NULL;
	struct spi_message *spi_msg = NULL;
	int ret, irq = -1;

	dev = &spi->dev;
	of_node = dev->of_node;
	if (!of_node) {
		dev_err(dev, "missing device tree entry");
		return -EINVAL;
	}


	/* set 32-bit DMA mask for allocation of video buffers
	 */
	of_dma_configure(dev, of_node); 
	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(dev, "failed to set DMA mask, err %d", ret);
		return -EINVAL;
	}

	/* lepton struct keeps track of both video and spi-related structs,
	 * and will be available in driver callbacks via private data pointers
	 */
	printk(KERN_INFO LEPTON_MODULE_NAME ": Allocate struct lepton, init mutex\n");
	lep = devm_kzalloc(dev, sizeof(*lep), GFP_KERNEL);
	if (lep == NULL) {
		dev_err(dev, "failed to allocate lepton struct");
		return -ENOMEM;
	}
	mutex_init(&lep->mutex);
	spin_lock_init(&lep->lock);

	/* initialize frame dimensions
	 */

	// @@@ need lepton version and telemetry module parameters
	init_lepton_info(&lep->lep_vospi_info, LEPTON_VERSION_2X, TELEMETRY_OFF);

	/* initialize v4l2_device -- used for tracking relationships among 
	 * video-related hardware managed by the V4L2 subsystem 
	 */

	printk(KERN_INFO LEPTON_MODULE_NAME ": Allocate struct v4l2_dev, register V4L2 device\n");
	v4l2_dev = devm_kzalloc(dev, sizeof(*v4l2_dev), GFP_KERNEL);
	if (v4l2_dev == NULL) {
		dev_err(dev, "failed to allocate v4l2 struct");
		return -ENOMEM;
	}
    ret = v4l2_device_register(dev, v4l2_dev);
	if (ret) {
		dev_err(dev, "failed to register v4l2");
		return ret;
	}

	/* initialize video_device -- used for managing device file
	 * (e.g. /dev/videoN) owned by parent v4l2_device 
	 */

	vid_dev = video_device_alloc();
	if (vid_dev == NULL) {
		dev_err(dev, "failed to allocate video struct");
		ret = -ENOMEM;
		goto unreg_v4l2_device;
	}

	*vid_dev = lepton_videodev_template;
	vid_dev->v4l2_dev = v4l2_dev;
	ret = video_register_device(vid_dev, VFL_TYPE_GRABBER, -1);
	if (ret) {
		/* now have non-devm (i.e. not automatically released when
		   owning device struct is gone) resources to free */
		goto unreg_v4l2_device;
	}

	/* initialize vb2_queue -- used to manage buffers for
	 * capturing video data 
	 */

	q = devm_kzalloc(dev, sizeof(*q), GFP_KERNEL);
	if (q == NULL) {
		/* now have non-devm (i.e. not automatically released when
		   owning device struct is gone) resources to free */
		dev_err(dev, "failed to allocate queue struct");
		ret = -ENOMEM;
		goto unreg_video_and_v4l_device;
	}

	q->dev = dev;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//	q->io_modes = VB2_MMAP | VB2_DMABUF | VB2_READ; //@@@ is DMABUF freebie with vb2 boilerplate?
	q->io_modes = VB2_MMAP | VB2_READ;
	q->buf_struct_size = sizeof(struct lepton_buffer);
	q->gfp_flags = GFP_DMA32;
	q->ops = &lepton_video_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &lep->mutex;
	q->min_buffers_needed = 3;

	ret = vb2_queue_init(q);
	if (ret) {
		/* now have non-devm (i.e. not automatically released when
		   owning device struct is gone) resources to free */
		dev_err(dev, "failed to init queue struct");
		goto unreg_video_and_v4l_device;
	}

	/* initialize spi descriptors and transmit buffer
	 */
	spi_msg = devm_kzalloc(dev, sizeof(*spi_msg), GFP_KERNEL);
	spi_xfer = devm_kzalloc(dev, sizeof(*spi_xfer), GFP_KERNEL);
	if (spi_xfer == NULL || spi_msg == NULL) {
		dev_err(dev, "failed to allocate SPI message/transfer structs");
		ret = -ENOMEM;
		goto unreg_video_and_v4l_device;
	}
	/* spare rx buffer when not using allocated V4L buf is subframe size + 1 line 
	 * so that eventually we will sync up if we start out in middle of a subframe */
	lep->spare_buf.len = lep->lep_vospi_info.subframe_data_byte_size + LEPTON_SUBFRAME_LINE_BYTE_WIDTH;
	lep->spare_buf.rx_buf = dma_zalloc_coherent(dev, lep->spare_buf.len, &lep->spare_buf.rx_dma, GFP_KERNEL);
	if (lep->spare_buf.rx_buf == NULL) {
		dev_err(dev, "failed to allocate SPI rx buffer");
		ret = -ENOMEM;
		goto unreg_video_and_v4l_device;
	}

    /* set up data pointers to be able to find any of the core structs
	 * when only one is passed into a callback function 
	 */

	lep->v4l2_dev = v4l2_dev;
	lep->vid_dev = vid_dev;
	lep->q = q;
	lep->spi_dev = spi;
	lep->spi_xfer = spi_xfer;
	lep->spi_msg = spi_msg;
	lep->current_lep_buf = NULL;

	dev_set_drvdata(dev, lep);
	video_set_drvdata(vid_dev, lep);
	vid_dev->queue = q;
	q->drv_priv = lep;

	lep->last_spi_done_ts.tv_sec = -1; /* no spi transfer has been started yet */
	lep->last_spi_done_ts.tv_nsec = 0;

	INIT_LIST_HEAD(&lep->unfilled_bufs);

	/* set up interrupt handler for lepton VSYNC (frame ready signal) 
	 */

	irq = irq_of_parse_and_map(of_node, 0);
	if (irq < 0) {
		dev_err(dev, "failed to map irq");
		goto unreg_video_and_v4l_device;
	}

	ret = devm_request_irq(dev, irq, lepton_vsync_handler, 0, dev_name(dev), spi);
	if (ret) {
		dev_err(dev, "failed to register irq");
		goto unreg_video_and_v4l_device;
	}

	printk(KERN_INFO LEPTON_MODULE_NAME ": Probe complete\n");
	return 0;

unreg_video_and_v4l_device:
	video_unregister_device(lep->vid_dev);
unreg_v4l2_device:
	v4l2_device_unregister(lep->v4l2_dev);

	return ret;
}

static int lepton_remove(struct spi_device *spi)
{
    struct lepton *lep = dev_get_drvdata(&spi->dev);

	/* tear down the things that are not "devm" (device-managed) */
	video_unregister_device(lep->vid_dev);
	v4l2_device_unregister(lep->v4l2_dev);

	return 0;
}

static struct spi_driver lepton_spi_driver = {
	.driver = {
		.name	= LEPTON_MODULE_NAME,
		.of_match_table	= lepton_of_match,
	},
	.id_table	= lepton_id_table,
	.probe		= lepton_probe,
	.remove		= lepton_remove,
};

module_spi_driver(lepton_spi_driver);

MODULE_AUTHOR("Team Lockwood-Childs, VCT Labs, Inc.");
MODULE_DESCRIPTION("VoSPI driver for FLIR lepton 2.x/3.x");
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");

