#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>

#define DEVICE_NAME "azar"

static struct cdev dev;
static struct class* mi_class;
static dev_t num; //El major
static int high = 0;

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	unsigned int rand;
	get_random_bytes(&rand, 4);
	rand %= high;

	void* b = kmalloc(length, GFP_KERNEL);
	int w = snprintf(b, length, "%i\n", rand);

	copy_to_user(buffer, b, length);
	kfree(b);

	return w;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset) {
	char* b = (char*)kmalloc(length + 1, GFP_KERNEL);
	copy_from_user(b, buffer, length);
	b[length] = '\0';

	if(kstrtoint(b, 10, &high) != 0) {
		kfree(b);
		printk(KERN_ALERT "Numero rechazado\n");
		return -EPERM;
	}

	printk(KERN_ALERT "Nuevo numerico\n");

	kfree(b);
	return length;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write
};

static int __init hello_init(void) {
	printk(KERN_ALERT "Hola, Sistemas Operativos!\n");

	cdev_init(&dev, &fops);

	alloc_chrdev_region(&num, 0, 1, DEVICE_NAME);

	cdev_add(&dev, num, 1);

	mi_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(mi_class, NULL, num, NULL, DEVICE_NAME);

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
