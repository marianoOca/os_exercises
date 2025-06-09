#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEVICE_NAME "nulo"

static struct cdev nulo_device;
static struct class* mi_class;
static dev_t num; //El major


static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "No voy a leer tus bytes\n");
	return 0;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "Se tiraron a la mierda algunos bytes!\n");
	return length;
}

static struct file_operations fops = {
.owner = THIS_MODULE,
.read = device_read,
.write = device_write
};

static int __init hello_init(void) {
	printk(KERN_ALERT "Hola, Sistemas Operativos!\n");

	cdev_init(&nulo_device, &fops);

	alloc_chrdev_region(&num, 0, 1, DEVICE_NAME);

	cdev_add(&nulo_device, num, 1);

	mi_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(mi_class, NULL, num, NULL, DEVICE_NAME);

	return 0;
}

static void __exit hello_exit(void) {
	printk(KERN_ALERT "Adios, mundo cruel...\n");
	device_destroy(mi_class, num);
	class_destroy(mi_class);
	unregister_chrdev_region(num, 1);
	cdev_del(&nulo_device);
}
	

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Una suerte de 'Hola, mundo'");
