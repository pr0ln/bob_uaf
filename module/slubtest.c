#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "drv.h"


static int device_open(struct inode *, struct file *);
static long device_ioctl(struct file *, unsigned int, unsigned long);
static int device_release(struct inode *, struct file *f);

static int major_no;


struct slubtest {
	unsigned long u64_value;
	char buf[1024];
};

#define SLUB_MAXLIST 1024
struct slubtest * pslublist[SLUB_MAXLIST];

static struct kmem_cache *slubtest_cachep;

static struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.unlocked_ioctl = device_ioctl
};

static int device_release(struct inode *i, struct file *f) {
	printk(KERN_INFO "device_release() called\n");
	return 0;
}

static int device_open(struct inode *i, struct file *f) {
	return 0;
}

static long slub_alloc(unsigned int index) {

	if(index >= SLUB_MAXLIST)
		return -EPERM;

	if(pslublist[index] != NULL)
		return -EPERM;

	pslublist[index] = kmem_cache_zalloc(slubtest_cachep, GFP_ATOMIC);

	return index;
}

static long slub_free(unsigned int index) {

	if(index >= SLUB_MAXLIST)
		return -EPERM;

	if(pslublist[index] == NULL)
		return -EPERM;

	kmem_cache_free(slubtest_cachep, pslublist[index]);
//	pslublist[index] = NULL;
//	VULN : make dangling pointer
	return 0;
}


static long slub_read64(unsigned int index) {
	if(index >= SLUB_MAXLIST)
                return -EPERM;

        if(pslublist[index] == NULL)
                return -EPERM;

	return pslublist[index]->u64_value;
}

static long slub_freeall(void) {

	unsigned long i;

	for(i=0; i<SLUB_MAXLIST; i++)
	{
		if(pslublist[i] != NULL)
			kmem_cache_free(slubtest_cachep, pslublist[i]);
	}
	return 0;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long args) {
	unsigned int req = (unsigned int)args;
	switch(cmd) {
		case IOCTL_ALLOC:
			return slub_alloc(req);
		case IOCTL_FREE:
			return slub_free(req);
		case IOCTL_FREEALL:
			return slub_freeall();
		case IOCTL_READ64:
			return slub_read64(req);
		default:
			break;
	}

	return 0;
}
static struct class *class;

static int __init load(void) {

	int retval = -ENOMEM;

	memset((void *)pslublist, 0, sizeof(pslublist));

	slubtest_cachep = kmem_cache_create("slubtest", sizeof(struct slubtest), 0, 0, NULL);

	if (!slubtest_cachep)
		return retval;

	printk(KERN_INFO "Driver loaded\n");
	major_no = register_chrdev(0, DEVICE_NAME, &fops);
	printk(KERN_INFO "major_no = %d\n", major_no);
	class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(class, NULL, MKDEV(major_no, 0), NULL, DEVICE_NAME);

	return 0;
}

static void __exit unload(void) {
	slub_freeall();
	kmem_cache_destroy(slubtest_cachep);
	device_destroy(class, MKDEV(major_no, 0));
	class_unregister(class);
	class_destroy(class);
	unregister_chrdev(major_no, DEVICE_NAME);
	printk(KERN_INFO "Driver unloaded\n");
}

module_init(load);
module_exit(unload);

MODULE_LICENSE("GPL");
