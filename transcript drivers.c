mutex mutex;
semaforo semaforo;
bool esperando;


void handler()
{
  if(IN(LOC_STATUS) == READY)
  {
    semaforo.signal();
  }
}


int driver_init()
{
  semaforo = semaforo_create(1);
  mutex = mutex_create(0);
  esperando = false;
  request_irq(CHINBOT_INT, handler);
}


int driver_write(void* data)
{
  int codigo;
  copy_from_user(&codigo, data, sizeof(int));

  mutex.lock();
  OUT(LOC_TARGET, codigo);
  OUT(LOC_CTRL, START);
  esperando = true;
  semaforo.wait();
  int resultado = IN(LOC_CTRL);
  mutex.unlock();

  if (resultado == JOYA)
  {
    return 1;
  }
  else if(resultado==BAJON)
  {
    return 0;
  }

  return -1;
}

//==================================================//
//MODULO

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

static char* DEVICE_NAME = "saludador";

static int j = 0;
static char* HOLA = "HOLA!\n'";

static ssize_t saludador_read(struct file *filp, char __user *data, size_t size, loff_t* off)
{
  unsigned char* buffer;
  unsigned int i;

  //reservar memoria en area del kernel
  buffer = kmalloc(size, GFP_KERNEL);
  i = 0;
  //escribir el buffer
  for(i = 0; i < size; i++)
  {
    buffer[i] = HOLA[j];
    j = (j+1)%6;
  }

  //copiamos el buffer en la memoria de usuario
  size = size - copy_to_user(data, buffer, size);
  //liberamos memoria del kernel
  kfree(buffer);

  //retorna cantidad escrita
  return size;
}


static ssize_t saludador_write(struct file *filp, const char __user *data, size_t s, loff_t* off)
{
  //retorno error de permiso
  return -EPERM;
}


static struct file_operations saludador_operaciones = {
  .owner = THIS_MODULE,
  .read = saludador_read,
  .write = saludador_write,
};

static struct cdev saludador_device;
static dev_t device_number;
static int device_count = 1;
static struct class *saludador_class;

static int saludador_init(void)
{
  int first_minor = 0;

  //init char device
  cdev_init(&saludador_device, &saludador_operaciones);

  //asignar numero al char device
  alloc_chrdev_region(&device_number, first_minor, device_count, DEVICE_NAME);

  //register char device
  cdev_add(&saludador_device, device_number, device_count);

  //create nodes para /dev/{DEVICE_NAME}
  saludador_class = class_create(THIS_MODULE, DEVICE_NAME);
  device_create(saludador_class, NULL, device_number, NULL, DEVICE_NAME);

  //Log mensaje carga del modulo
  printk(KERN_ALERT "cargando mod saludador...");
  return 0;
}

static void __exit saludador_exit(void)
{
  //eliminar nodos
  device_destroy(saludador_class, device_number);
  class_destroy(saludador_class);

  //eliminar nro char device
  unregister_chrdev_region(device_number, device_count);
  //eliminar char device
  cdev_del(&saludador_device);

  //log mensaje de que sacamos el modulo
  printk(KERN_ALERT "eliminado modulo saludador");
}

module_init(saludador_init);
module_exit(saludador_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("saludador");
