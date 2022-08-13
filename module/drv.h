#define DEVICE_NAME "slubtest"
#define DEVICE_PATH "/dev/slubtest"

#define IOCTL_SLUBTEST 100
#define IOCTL_ALLOC    _IOW(IOCTL_SLUBTEST, 0, unsigned int)
#define IOCTL_FREE _IOW(IOCTL_SLUBTEST, 1, unsigned int)
#define IOCTL_FREEALL _IO(IOCTL_SLUBTEST, 2)
#define IOCTL_READ64 _IOW(IOCTL_SLUBTEST, 3, unsigned int)
