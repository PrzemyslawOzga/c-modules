#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/keylogger"
#define IOCTL_CLEAR_BUFFER_DATA _IO('k', 4)

int main()
{
	int fd;

	fd = open(DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		perror("Unable to open keylogger device");
		return errno;
	}

	if (ioctl(fd, IOCTL_CLEAR_BUFFER_DATA, NULL) < 0) {
		perror("Failed to clear buffer data (IOCTL_CLEAR_BUFFER_DATA)");
		close(fd);
		return errno;
	}

	printf("Buffer data cleared\n");

	close(fd);
	return 0;
}