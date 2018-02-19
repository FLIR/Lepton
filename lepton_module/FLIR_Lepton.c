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

enum lepton_model {
	FLIR_LEPTON2	= 2,
	FLIR_LEPTON3	= 3,
};

struct lepton {
	struct mutex mutex;
	struct v4l2_device *v4l2_dev;
	struct video_device *vid_dev;
	struct vb2_queue *q;
	struct spi_device *spi_dev;
	int telemetry_enabled;
	unsigned int width_in_pixels;
	unsigned int height;
	unsigned int sizeimage;
};

/*
 * interface to userspace apps: V4L2 video device
 */

static int lepton_querycap(struct file *file, void *priv,
			struct v4l2_capability *cap)
{
	printk(KERN_INFO "%s: start\n", __func__);
	strlcpy(cap->driver, LEPTON_MODULE_NAME, sizeof(cap->driver));
	strlcpy(cap->card, "FLIR Lepton", sizeof(cap->driver));
	snprintf(cap->bus_info, sizeof(cap->bus_info), "platform:%s", LEPTON_MODULE_NAME);
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}

static int lepton_enum_input(struct file *file, void *priv,
			 struct v4l2_input *inp)
{
	printk(KERN_INFO "%s: start\n", __func__);
	if (inp->index > 0)
		return -EINVAL;

	inp->type = V4L2_INPUT_TYPE_CAMERA;
	snprintf(inp->name, sizeof(inp->name), LEPTON_MODULE_NAME);
	inp->capabilities = 0;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}

static int lepton_s_input(struct file *file, void *priv, unsigned int i)
{
	printk(KERN_INFO "%s: start\n", __func__);
	// only 1 input
	if (i != 0)
		return -EINVAL;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_g_input(struct file *file, void *priv, unsigned int *i)
{
	printk(KERN_INFO "%s: start\n", __func__);
	// only 1 input
	*i = 0;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_querystd(struct file *file, void *fh, v4l2_std_id *std)
{
	printk(KERN_INFO "%s: stub\n", __func__);
	// nothing to say about the video standard
	return -ENODATA;
}
static int lepton_s_std(struct file *file, void *fh, v4l2_std_id std)
{
	printk(KERN_INFO "%s: stub\n", __func__);
	return -ENODATA;
}
static int lepton_g_std(struct file *file, void *fh, v4l2_std_id *std)
{
	printk(KERN_INFO "%s: stub\n", __func__);
	return -ENODATA;
}
static int lepton_enum_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_fmtdesc *f)
{
	printk(KERN_INFO "%s: start\n", __func__);
	if (f->index != 0)
		return -EINVAL;
	f->pixelformat = V4L2_PIX_FMT_Y16;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static unsigned int lepton_get_width_in_pixels(void)
{
	unsigned int frame_width;

	// @@@ determine frame_width from lepton type
	frame_width = FRAME_WIDTH_LEPTON_2;
	return frame_width;
}
static unsigned int lepton_get_height(void)
{
	unsigned int frame_height;

	// @@@ determine frame_width from lepton type
	frame_height = FRAME_HEIGHT_LEPTON_2;
	return frame_height;
}
static int lepton_set_fmt_fields(struct lepton *dev, struct v4l2_format *f)
{
	struct v4l2_pix_format *pix = NULL;

	printk(KERN_INFO "%s: start\n", __func__);
	pix = &f->fmt.pix;

	pix->width = dev->width_in_pixels;
	pix->height = dev->height;
	pix->pixelformat = V4L2_PIX_FMT_Y16;  // 16-bit grayscale
	pix->colorspace = V4L2_COLORSPACE_RAW;
	pix->bytesperline = dev->width_in_pixels * 2;
	pix->sizeimage = dev->sizeimage;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_s_parm(struct file *file, void *priv,
			     struct v4l2_streamparm *parm)
{
	printk(KERN_INFO "%s: start\n", __func__);
	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 30;
	parm->parm.capture.readbuffers  = 1;
	printk(KERN_INFO "%s: done\n", __func__);

	return 0;
}
static int lepton_g_parm(struct file *file, void *priv,
			     struct v4l2_streamparm *parm)
{
	printk(KERN_INFO "%s: start\n", __func__);
	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 30;
	parm->parm.capture.readbuffers  = 1;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_g_fmt_vid_cap(struct file *file, void *priv,
			     struct v4l2_format *f)
{
	struct lepton *dev = NULL;

	printk(KERN_INFO "%s: start\n", __func__);
	dev = video_drvdata(file);

	lepton_set_fmt_fields(dev, f);
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_try_fmt_vid_cap(struct file *file, void *priv,
			       struct v4l2_format *f)
{
	struct lepton *dev = NULL;

	printk(KERN_INFO "%s: start\n", __func__);
	dev = video_drvdata(file);
	// we only ever have one format, so always send it back.
	lepton_set_fmt_fields(dev, f);
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_s_fmt_vid_cap(struct file *file, void *priv,
			     struct v4l2_format *f)
{
	struct lepton *dev = NULL;

	printk(KERN_INFO "%s: start\n", __func__);
	dev = video_drvdata(file);
	// we only ever have one format, so always send it back.
	lepton_set_fmt_fields(dev, f);
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_enum_frameintervals(struct file *file, void *priv,
				   struct v4l2_frmivalenum *f)
{
	printk(KERN_INFO "%s: start\n", __func__);
	if (f->index != 0)
		return -EINVAL;
	f->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	// always runs at 30Hz
	f->discrete.numerator = 1;
	f->discrete.denominator = 30;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
static int lepton_enum_framesizes(struct file *file, void *priv,
			       struct v4l2_frmsizeenum *f)
{
	struct lepton *dev = NULL;

	printk(KERN_INFO "%s: start\n", __func__);
	dev = video_drvdata(file);
	if (f->index != 0)
		return -EINVAL;
	f->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	f->discrete.width = dev->width_in_pixels;
	f->discrete.height = dev->height;
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}

#if 0
static int lepton_release(struct file *file)
{
	struct video_device *vdev = NULL;
	struct mutex *lock = NULL;
	int ret = -1;

	printk(KERN_INFO "%s: start\n", __func__);
	vdev = video_devdata(file);
	printk(KERN_INFO "%s: vdev points to %p\n", __func__, vdev);
	printk(KERN_INFO "%s: vdev->queue points to %p\n", __func__, vdev->queue);
	printk(KERN_INFO "%s: vdev->queue->lock points to %p\n", __func__, vdev->queue->lock);
	printk(KERN_INFO "%s: vdev->lock points to %p\n", __func__, vdev->lock);
	lock = vdev->queue->lock ? vdev->queue->lock : vdev->lock;
	printk(KERN_INFO "%s: mutex points to %p\n", __func__, lock);
	// ret = _vb2_fop_release(file, lock);
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}
#endif

static struct v4l2_file_operations lepton_fops = {
	.owner =    THIS_MODULE,
	.open =     v4l2_fh_open,
	.release =  vb2_fop_release,
//	.release =  lepton_release,
	.read =     vb2_fop_read,    
	.poll =     vb2_fop_poll,
	.mmap =     vb2_fop_mmap,
	.unlocked_ioctl = video_ioctl2,
};

static const struct v4l2_ioctl_ops lepton_ioctl_ops = {
	.vidioc_s_parm			= lepton_s_parm,
	.vidioc_g_parm			= lepton_g_parm,
#if 0 // How much of this is mandatory?
	.vidioc_g_selection	= lepton_g_selection,
	.vidioc_s_selection	= lepton_s_selection,
#endif
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
	struct lepton *dev = NULL;
	unsigned int size = -1;

	printk(KERN_INFO "%s: start\n", __func__);
	dev = vb2_get_drv_priv(vq);
	size = dev->sizeimage;
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
	printk(KERN_INFO LEPTON_MODULE_NAME " %s: get %d buffers, each holding %d bytes.\n", __func__, *nbuffers, sizes[0]);
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}

int lepton_buf_prepare(struct vb2_buffer *vb)
{
	struct lepton *dev = NULL;

	printk(KERN_INFO "%s: start\n", __func__);
	dev = vb2_get_drv_priv(vb->vb2_queue);
	if (vb2_plane_size(vb, 0) < dev->sizeimage)
	{
		pr_debug("%s: data will not fit into buf size %ld\n", __func__, vb2_plane_size(vb, 0));
		printk(KERN_INFO LEPTON_MODULE_NAME " %s: data will not fit into buf size %ld\n", __func__, vb2_plane_size(vb,0));
		return -EINVAL;
	}
	printk(KERN_INFO "%s: done\n", __func__);
	return 0;
}

void lepton_buf_queue(struct vb2_buffer *vb)
{
	printk(KERN_INFO "%s: stub\n", __func__);
}

int lepton_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	printk(KERN_INFO "%s: stub\n", __func__);
	return -EINVAL;	//@@@@ NYI
}

void lepton_stop_streaming(struct vb2_queue *vq)
{
	printk(KERN_INFO "%s: stub\n", __func__);
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

static irqreturn_t lepton_vsync_handler(int irq, void *data)
{
	static int vsync_count = 0;

	if (printk_ratelimit())
		pr_debug("VSYNC %d", vsync_count);
	vsync_count++;

	return 0;
}

static int lepton_probe(struct spi_device *spi)
{
	struct lepton *lep = NULL;
	struct device *dev = NULL;
	struct device_node *of_node = NULL;
	struct v4l2_device *v4l2_dev = NULL;
	struct video_device *vid_dev = NULL;
	struct vb2_queue *q = NULL;
	int ret, irq = -1;

	dev = &spi->dev;
	of_node = dev->of_node;

	printk(KERN_INFO LEPTON_MODULE_NAME ": dma mask\n");
	/* set 32-bit DMA mask for allocation of video buffers
	 */
	of_dma_configure(dev, of_node); 
	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(dev, "failed to set DMA mask, err %d", ret);
		return -EINVAL;
	}

	printk(KERN_INFO LEPTON_MODULE_NAME ": Map IRQ from device tree\n");
	/* set up interrupt handler for lepton VSYNC (frame ready signal) 
	 */

	if (!of_node) {
		dev_err(dev, "missing device tree entry");
		return -EINVAL;
	}

	irq = irq_of_parse_and_map(of_node, 0);
	if (irq < 0) {
		dev_err(dev, "failed to map irq");
		return -EINVAL;
	}

	printk(KERN_INFO LEPTON_MODULE_NAME ": Register IRQ handler\n");
	ret = devm_request_irq(dev, irq, lepton_vsync_handler, 0, dev_name(dev), spi);
	if (ret) {
		dev_err(dev, "failed to register irq");
		return ret;
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

	printk(KERN_INFO LEPTON_MODULE_NAME ": Allocate, initialize, register video device\n");
	vid_dev = video_device_alloc();
	if (vid_dev == NULL) {
		dev_err(dev, "failed to allocate video struct");
		return -ENOMEM;
	}

	*vid_dev = lepton_videodev_template;
	vid_dev->v4l2_dev = v4l2_dev;
	ret = video_register_device(vid_dev, VFL_TYPE_GRABBER, -1);
	if (ret) {
		/* now have non-devm (i.e. not automatically released when
		   owning device struct is gone) resources to free */
		goto err_vid_dev;
	}

	/* initialize vb2_queue -- used to manage buffers for
	 * capturing video data 
	 */

	printk(KERN_INFO LEPTON_MODULE_NAME ": Allocate, initialize, register vb2 queue\n");
	q = devm_kzalloc(dev, sizeof(*q), GFP_KERNEL);
	if (q == NULL) {
		/* now have non-devm (i.e. not automatically released when
		   owning device struct is gone) resources to free */
		dev_err(dev, "failed to allocate queue struct");
		ret = -ENOMEM;
		goto err_queue;
	}

	q->dev = dev;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//	q->io_modes = VB2_MMAP | VB2_DMABUF | VB2_READ; //@@@ is DMABUF freebie with vb2 boilerplate?
	q->io_modes = VB2_MMAP | VB2_READ;
	q->buf_struct_size = sizeof(struct vb2_buffer); //@@@ update if custom buffer is needed
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
		goto err_queue;
	}

	printk(KERN_INFO LEPTON_MODULE_NAME ": Final initialization\n");
    /* set up data pointers to be able to find any of the core structs
	 * when only one is passed into a callback function 
	 */

	lep->v4l2_dev = v4l2_dev;
	lep->vid_dev = vid_dev;
	lep->q = q;
	lep->spi_dev = spi;

	dev_set_drvdata(dev, lep);
	video_set_drvdata(vid_dev, lep);
	vid_dev->queue = q;
	q->drv_priv = lep;

	/* initialize private data */
	lep->width_in_pixels = lepton_get_width_in_pixels();
	lep->height = lepton_get_height();
	lep->sizeimage = (lep->width_in_pixels*2) * lep->height;

	printk(KERN_INFO LEPTON_MODULE_NAME ": Probe complete\n");
	return 0;

err_vid_dev:
//@@@ v4l_unregister_device, video_device_release
err_queue:
//@@@ video_unregister_device 
	return ret;
}

static int lepton_remove(struct spi_device *spi)
{
    struct lepton *lep = dev_get_drvdata(&spi->dev);

	/* @@ tear down the things in probe */
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

