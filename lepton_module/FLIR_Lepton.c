/* FLIR_Lepton.c
   Main source file for the FLIR Lepton VoSPI driver
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <asm/uaccess.h>

#include "FLIR_Lepton.h"

enum lepton_model {
	FLIR_LEPTON2	= 2,
	FLIR_LEPTON3	= 3,
};

ssize_t lepton_read(struct file *, char __user *, size_t, loff_t *);
int lepton_open(struct inode *inode, struct file *filp);
int lepton_release(struct inode *inode, struct file *filp);
static int lepton_probe(struct spi_device *spi);
static int lepton_remove(struct spi_device *spi);

static struct file_operations lepton_fops = {
    .owner =    THIS_MODULE,
    .llseek =   NULL,
    .read =     lepton_read,
    .write =    NULL,
    .unlocked_ioctl =    NULL,
    .open =     lepton_open,
    .release =  lepton_release,
};

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

static struct spi_driver lepton_spi_driver = {
	.driver = {
		.name	= "lepton",
		.of_match_table	= lepton_of_match,
	},
	.id_table	= lepton_id_table,
	.probe		= lepton_probe,
	.remove		= lepton_remove,
};

struct _lepton_dev {
    int telemetry_enabled;
} lepton_dev;

static int lepton_init(void) {
    int status;

    status = register_chrdev(lepton_major, "lepton", &lepton_fops);
    if (status < 0)
        return status;
    status = spi_register_driver(&lepton_spi_driver);
    if (status < 0)
    {
        unregister_chrdev(lepton_major, lepton_spi_driver.driver.name);
    }
    return status;
}
module_init(lepton_init);

static void __exit lepton_exit(void)
{
    spi_unregister_driver(&lepton_spi_driver);
    unregister_chrdev(lepton_major, lepton_spi_driver.driver.name);
}
module_exit(lepton_exit);

int lepton_open(struct inode *inode, struct file *filp)
{

    filp->private_data = &lepton_dev;
    if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		// This device isn't writable
        return -EINVAL;
    }
    return 0;
}

ssize_t lepton_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct lepton_dev *leptondev;
    leptondev = filp->private_data;
    return 0;
}

int lepton_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static int lepton_probe(struct spi_device *spi)
{
	/* @@ set up character device, configure spi, dma buffers, assign interrupt handler */
	return 0;
}

static int lepton_remove(struct spi_device *spi)
{
	/* @@ tear down the things in probe */
	return 0;
}

MODULE_AUTHOR("Ron Lockwood-Childs, VCT Labs, Inc.");
MODULE_DESCRIPTION("VoSPI driver for FLIR lepton 2.x/3.x");
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");

