#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>

#define DEVICE_NAME "letras123"

static struct cdev dev;
static struct class* mi_class;
static dev_t num; //El major
static int slots = 0;
static spinlock_t mutex;

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	if (filp->private_data == (char*)1) {
		return -EPERM;
	}

	char* b = (char*)kmalloc(length, GFP_KERNEL);
	int i;
	for (i = 0; i < length; i++) {
		b[i] = (char)filp->private_data;
	}

	copy_to_user(buffer, b, length);
	kfree(b);

	return length;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset) {
	if (filp->private_data == (char*)1) {
		filp->private_data = buffer[0];
	}
	return length;
}

static ssize_t device_open(struct inode* in, struct file* filp) {
	spin_lock(&mutex);
	if (slots < 3) {
		// asignar slot
		filp->private_data = (char*)1;
		slots++;
		spin_unlock(&mutex);
		return 0;
	}
	spin_unlock(&mutex);

	return -EPERM;
}

static ssize_t device_release(struct inode* in, struct file* filp) {
	if (filp->private_data == NULL) {
		return -EPERM;
	} else {
		spin_lock(&mutex);
		slots--;
		spin_unlock(&mutex);
		filp->private_data = NULL;
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int __init hello_init(void) {
	printk(KERN_ALERT "Hola, Sistemas Operativos!\n");

	cdev_init(&dev, &fops);

	alloc_chrdev_region(&num, 0, 1, DEVICE_NAME);

	cdev_add(&dev, num, 1);

	mi_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(mi_class, NULL, num, NULL, DEVICE_NAME);

	spin_lock_init(&mutex);

	return 0;
}

static void __exit hello_exit(void) {
	printk(KERN_ALERT "Adios, mundo cruel...\n");
	device_destroy(mi_class, num);
	class_destroy(mi_class);
	unregister_chrdev_region(num, 1);
	cdev_del(&dev);
}
	

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Una suerte de 'Hola, mundo'");
