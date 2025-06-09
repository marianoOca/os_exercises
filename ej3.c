#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>

static char* DEVICE_NAME = "letras123";


static ssize_t letras123_read(struct file *filp, char __user *data, size_t size, loff_t* off)
{
	return 0;
}

static ssize_t letras123_write(struct file *filp, const char __user *data, size_t size, loff_t* off)
{
	return 0;
}

static int letras123_open(struct inode *inode, struct file *file)
{

}

static int letras123_release(struct inode *inode, struct file *file)
{

}


static struct file_operations letras123_operaciones = {
  .owner = THIS_MODULE,
  .read = letras123_read,
  .write = letras123_write,
	.open = letras123_open,
	.release = letras123_release,
};

static struct cdev letras123_device;
static dev_t device_number;
static int device_count = 1;
static struct class *letras123_class;

static int letras123_init(void)
{
  int first_minor = 0;

  //init char device
  cdev_init(&letras123_device, &letras123_operaciones);

  //asignar numero al char device
  alloc_chrdev_region(&device_number, first_minor, device_count, DEVICE_NAME);

  //register char device
  cdev_add(&letras123_device, device_number, device_count);

  //create nodes para /dev/{DEVICE_NAME}
  letras123_class = class_create(THIS_MODULE, DEVICE_NAME);
  device_create(letras123_class, NULL, device_number, NULL, DEVICE_NAME);

  //Log mensaje carga del modulo
  printk(KERN_ALERT "cargando mod letras123...\n");
  return 0;
}

static void __exit letras123_exit(void)
{
  //eliminar nodos
  device_destroy(letras123_class, device_number);
  class_destroy(letras123_class);

  //eliminar nro char device
  unregister_chrdev_region(device_number, device_count);
  //eliminar char device
  cdev_del(&letras123_device);

  //log mensaje de que sacamos el modulo
  printk(KERN_ALERT "eliminado modulo letras123\n");
}

module_init(letras123_init);
module_exit(letras123_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("NULL DEV");
