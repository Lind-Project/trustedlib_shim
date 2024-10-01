#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sched.h>
#include <poll.h>
#include <errno.h>
#include <sys/random.h>

int ocall_open(const char *pathname, int flags, bool has_mode, unsigned mode)
{
    if (has_mode) {
        return open(pathname, flags, (mode_t)mode);
    }
    else {
        return open(pathname, flags);
    }
}

int ocall_stat(const char *pathname, void *buf, unsigned int buf_len)
{
    printf("inside stat -- \n");
    return stat(pathname, (struct stat *)buf);
}

int ocall_statx(int dirfd, const char *pathname, int flags, unsigned int mask, void *buf, unsigned int buf_len)
{
    // Ensure that buf is large enough to hold struct statx
    // if (buf_len < sizeof(struct statx)) {
    //     printf("Buffer is too small to hold struct statx\n");
    //     return -1; // or appropriate error code
    // }

    return statx(dirfd, pathname, flags, mask, (struct statx *)buf);
}

int ocall_getrandom(void* buf, size_t buflen, unsigned int flags) 
{
    return getrandom(buf, buflen, flags);
}

int ocall_fstat(int fd, void *buf, unsigned int buf_len)
{
    return fstat(fd, (struct stat *)buf);
}

off_t ocall_lseek(int fd, off_t offset, int whence)
{
    return lseek(fd, offset, whence);
}

ssize_t ocall_read(int fd, void *buf, size_t read_size)
{
    if (buf != NULL) {
        return read(fd, buf, read_size);
    }
    else {
        return -1;
    }
}

int ocall_close(int fd)
{
    return close(fd);
}

int ocall_sched_getaffinity(pid_t pid, size_t cpusetsize, void *mask)
{
    return sched_getaffinity(pid, cpusetsize, (cpu_set_t *) mask);
}

int ocall_realpath(const char *path, char *buf, unsigned int buf_len)
{
    printf("inside real path\n");
    char *val = NULL;
    val = realpath(path, buf);
    if (val != NULL) {
        return 0;
    }
    return -1;
}

int ocall_write(int fd, const void *buf, size_t count) 
{
    return write(fd, buf, count);
}

int ocall_fcntl(int fd, int cmd)
{
    return fcntl(fd, cmd);
}

int ocall_fcntl_long(int fd, int cmd, long arg)
{
    return fcntl(fd, cmd, arg);
}

int ocall_ftruncate(int fd, off_t length)
{
    return ftruncate(fd, length);
}

int ocall_unlinkat(int dirfd, const char *pathname, int flags)
{
    return unlinkat(dirfd, pathname, flags);
}