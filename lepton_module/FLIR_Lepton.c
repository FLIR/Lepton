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
#include <asm/uaccess.h>

#include "FLIR_Lepton.h"

struct file_operations lepton_fops = {
    .owner =    THIS_MODULE,
    .llseek =   NULL,
    .read =     lepton_read,
    .write =    NULL,
    .ioctl =    NULL,
    .open =     lepton_open,
    .release =  lepton_release,
};

struct lepton_dev {
    int telemetry_enabled;
    struct cdev cdev;
};

static void lepton_setup_cdev(struct lepton_dev *dev, int index)
{
    int err, devno = MKDEV(lepton_major, lepton_minor + index);

    cdev_init(&dev->cdev, &lepton_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &lepton_fops;
    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding lepton%d", err, index);
    }
}

int lepton_open(struct inode *inode, struct file *filp)
{
    struct lepton_dev *dev;

    dev = container_of(inode->i_cdev, struct lepton_dev, cdev);
    filp->private_data = dev;
    if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
        return -EINVAL;
    }
    return 0;
}

int lepton_release(struct inode *inode, struct file *filp)
{
    return 0;
}

MODULE_AUTHOR("Ron Lockwood-Childs, VCT Labs, Inc.");
MODULE_DESCRIPTION("VoSPI driver for FLIR lepton 2.x/3.x");
MODULE_VERSION(VERSION);

