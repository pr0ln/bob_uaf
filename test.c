#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <sys/ioctl.h>
#include "./module/drv.h"

void pin_cpu(int cpu)
{
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(cpu, &set);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &set)) {
		printf("error\n");
		exit(-1);
	}
}

int main() {
        int fd;
        long res;
        fd = open(DEVICE_PATH, O_RDONLY);

        if (fd == -1) {
                perror("open");
                return -1;
        }
        res = ioctl(fd, IOCTL_ALLOC, 10);
	res = ioctl(fd, IOCTL_FREE, 10);
        close(fd);
}
