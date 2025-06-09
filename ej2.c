#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>

static char* DEVICE_NAME = "azar";
static int last_number;
static bool init = false;


static ssize_t azar_read(struct file *filp, char __user *data, size_t size, loff_t* off)
{
	unsigned int rd;
	char* buffer;

	if (!init) return -EPERM;
	get_random_bytes(&rd, sizeof(rd));
	rd %= last_number;

	buffer = kmalloc(size, GFP_KERNEL);
	snprintf(&buffer,"%d\n",rd);

	copy_to_user(data, buffer, size);

	kfree(buffer);
	return 0;  //??
}

static ssize_t azar_write(struct file *filp, const char __user *data, size_t size, loff_t* off)
{
	char* buffer;
	int res;

	buffer = kmalloc(size+1, GFP_KERNEL);
	copy_from_user(buffer, data, size);  //(sizeof(char) es un byte)
	buffer[size] = '\0';

	//interpretar como un entero
	if (kstrtoint (buffer, 0, &res) != 0)
	{
		//la conversion falló
		kfree(buffer);
		return -EPERM;
	}

	init = true;
	last_number = res;

	kfree(buffer);
	return 0;
}


static struct file_operations azar_operaciones = {
  .owner = THIS_MODULE,
  .read = azar_read,
  .write = azar_write,
};

static struct cdev azar_device;
static dev_t device_number;
static int device_count = 1;
static struct class *azar_class;

static int azar_init(void)
{
  int first_minor = 0;

  //init char device
  cdev_init(&azar_device, &azar_operaciones);

  //asignar numero al char device
  alloc_chrdev_region(&device_number, first_minor, device_count, DEVICE_NAME);

  //register char device
  cdev_add(&azar_device, device_number, device_count);

  //create nodes para /dev/{DEVICE_NAME}
  azar_class = class_create(THIS_MODULE, DEVICE_NAME);
  device_create(azar_class, NULL, device_number, NULL, DEVICE_NAME);

  //Log mensaje carga del modulo
  printk(KERN_ALERT "cargando mod azar...\n");
  return 0;
}

static void __exit azar_exit(void)
{
  //eliminar nodos
  device_destroy(azar_class, device_number);
  class_destroy(azar_class);

  //eliminar nro char device
  unregister_chrdev_region(device_number, device_count);
  //eliminar char device
  cdev_del(&azar_device);

  //log mensaje de que sacamos el modulo
  printk(KERN_ALERT "eliminado modulo azar\n");
}

module_init(azar_init);
module_exit(azar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("NULL DEV");
