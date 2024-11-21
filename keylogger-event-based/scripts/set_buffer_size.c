#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/keylogger"
#define IOCTL_SET_BUFFER_SIZE _IOR('k', 3, int)

int main()
{
	int fd;
	int new_size;

	fd = open(DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		perror("Unable to open keylogger device");
		return errno;
	}

	printf("Enter new buffer size: ");
	scanf("%d", &new_size);

	if (ioctl(fd, IOCTL_SET_BUFFER_SIZE, &new_size) < 0) {
		perror("Failed to set buffer size (IOCTL_SET_BUFFER_SIZE)");
		close(fd);
		return errno;
	}

	printf("Buffer size set to %d\n", new_size);

	close(fd);
	return 0;
}