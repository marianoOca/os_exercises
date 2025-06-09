#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

static char* DEVICE_NAME = "nulo";


static ssize_t nulo_read(struct file *filp, char __user *data, size_t size, loff_t* off)
{
	return 0;
}

static ssize_t nulo_write(struct file *filp, const char __user *data, size_t s, loff_t* off)
{
  return 0;
}


static struct file_operations nulo_operaciones = {
  .owner = THIS_MODULE,
  .read = nulo_read,
  .write = nulo_write,
};

static struct cdev nulo_device;
static dev_t device_number;
static int device_count = 1;
static struct class *nulo_class;

static int nulo_init(void)
{
  int first_minor = 0;

  //init char device
  cdev_init(&nulo_device, &nulo_operaciones);

  //asignar numero al char device
  alloc_chrdev_region(&device_number, first_minor, device_count, DEVICE_NAME);

  //register char device
  cdev_add(&nulo_device, device_number, device_count);

  //create nodes para /dev/{DEVICE_NAME}
  nulo_class = class_create(THIS_MODULE, DEVICE_NAME);
  device_create(nulo_class, NULL, device_number, NULL, DEVICE_NAME);

  //Log mensaje carga del modulo
  printk(KERN_ALERT "cargando mod nulo...\n");
  return 0;
}

static void __exit nulo_exit(void)
{
  //eliminar nodos
  device_destroy(nulo_class, device_number);
  class_destroy(nulo_class);

  //eliminar nro char device
  unregister_chrdev_region(device_number, device_count);
  //eliminar char device
  cdev_del(&nulo_device);

  //log mensaje de que sacamos el modulo
  printk(KERN_ALERT "eliminado modulo NULO\n");
}

module_init(nulo_init);
module_exit(nulo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("NULL DEV");
