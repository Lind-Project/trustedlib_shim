#include "shim_t.h"
#include "types.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "sgx_error.h"
#include "sgx_file.h"
#include <string.h>

// // /** fd **/
// // int ocall_open(int *p_fd, const char *pathname, int flags, bool has_mode,
// //            unsigned mode);

int printf1(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
    return 0;
}

int open(const char *pathname, int flags, ...)
{
    int fd;
    bool has_mode = false;
    mode_t mode = 0;

    if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
        has_mode = true;
    }

    if (SGX_SUCCESS != ocall_open(&fd, pathname, flags, has_mode, mode)) {
        return -1;
    }

    return fd;
}

int open64(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
        return open(pathname, flags | O_LARGEFILE, mode);
    } else {
        return open(pathname, flags | O_LARGEFILE);
    }
}

int stat(const char *pathname, struct stat *statbuf)
{
    printf1("inside stat\n");
    int ret;

    if (statbuf == NULL)
        return -1;

    if (ocall_stat(&ret, pathname, (void *)statbuf, sizeof(struct stat))
        != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}

int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf)
{
    int ret;

    if (statxbuf == NULL)
        return -1;

    if (ocall_statx(&ret, dirfd, pathname, flags, mask, (void *)statxbuf, sizeof(struct statx))
        != SGX_SUCCESS) {
        return -1;
    }

        // Print detailed info from statx
        // printf1("File Information:\n");
        // printf1("  stx_mode: %u\n", statxbuf->stx_mode);
        // printf1("  stx_ino: %llu\n", statxbuf->stx_ino);
        // printf1("  stx_size: %llu bytes\n", statxbuf->stx_size);
        // printf1("  stx_blocks: %llu\n", statxbuf->stx_blocks);
        // printf1("  stx_attributes: %u\n", statxbuf->stx_attributes);

        // printf1("Access Time: %ld.%09u\n", statxbuf->stx_atime.tv_sec, statxbuf->stx_atime.tv_nsec);
        // printf1("Modification Time: %ld.%09u\n", statxbuf->stx_mtime.tv_sec, statxbuf->stx_mtime.tv_nsec);
        // printf1("Creation Time: %ld.%09u\n", statxbuf->stx_btime.tv_sec, statxbuf->stx_btime.tv_nsec);

    return ret;
}

ssize_t getrandom(void* buf, size_t buflen, unsigned int flags) {

    int ret;

    if (buf == NULL)
        return -1;

    if (ocall_getrandom(&ret, buf, buflen, flags)
        != SGX_SUCCESS) {
        return -1;
    }

    return ret;

}

int fstat(int fd, struct stat *statbuf)
{
    printf1("inside stat\n");
    int ret;

    if (statbuf == NULL)
        return -1;

    if (ocall_fstat(&ret, fd, (void *)statbuf, sizeof(struct stat))
        != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}

off_t lseek(int fd, off_t offset, int whence)
{
    printf1("inside stat\n");
    off_t ret;

    if (ocall_lseek(&ret, fd, (long)offset, whence) != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}

off_t lseek64(int fd, off_t offset, int whence) 
{
    return lseek(fd, offset, whence);
}

ssize_t read(int fd, void *buf, size_t size)
{
    printf1("inside stat\n");
    ssize_t ret;
    int size_read_max = 2048, size_read, total_size_read = 0, count, i;
    char *p = (char *)buf;

    if (buf == NULL) {
        return -1;
    }

    count = (size + size_read_max - 1) / size_read_max;
    for (i = 0; i < count; i++) {
        size_read = (i < count - 1) ? size_read_max : size - size_read_max * i;

        if (ocall_read(&ret, fd, p, size_read) != SGX_SUCCESS) {
            return -1;
        }

        p += ret;
        total_size_read += ret;

        if (ret < size_read)
            /* end of file */
            break;
    }
    return total_size_read;
}

ssize_t write(int fd, const void *buf, size_t count) 
{
    printf1("inside stat\n");
    ssize_t ret;
    
    if (ocall_write(&ret, fd, buf, count) != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}

int close(int fd)
{
    int ret;

    if (ocall_close(&ret, fd) != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}

char * strcpy(char *dest, const char *src)
{
    const unsigned char *s = reinterpret_cast<const unsigned char*>(src);
    unsigned char *d = reinterpret_cast<unsigned char*>(dest);

    while ((*d++ = *s++)) {
    }
    return dest;
}

char * realpath(const char *path, char *resolved_path)
{
    printf1("inside realpath\n");
    int ret;
    char buf[PATH_MAX] = { 0 };

    if (resolved_path == NULL) {
        printf1("broken\n");
        resolved_path = (char*)malloc(PATH_MAX);
    }

    if (ocall_realpath(&ret, path, buf, PATH_MAX) != SGX_SUCCESS) {
        return (char *)NULL;
    }
    printf1("inside realpath success\n");

    if (ret != 0)
        return (char *)NULL;

    printf1("inside realpath %s\n", buf);

    if (resolved_path != NULL) {
        printf1("not null resolved_path\n");
        strcpy(resolved_path, buf);
    }

    printf1("inside realpath result %s\n", resolved_path);

    return resolved_path;
}

int
fcntl(int fd, int cmd, ... /* arg */)
{
    int ret;
    va_list args;

    switch (cmd) {
        case F_GETFD:
        case F_GETFL:
            if (ocall_fcntl(&ret, fd, cmd) != SGX_SUCCESS) {
                return -1;
            }
            break;

        case F_DUPFD:
        case F_SETFD:
        case F_SETFL:
            va_start(args, cmd);
            long arg_1 = (long)va_arg(args, long);
            if (ocall_fcntl_long(&ret, fd, cmd, arg_1) != SGX_SUCCESS) {
                va_end(args);
                return -1;
            }
            va_end(args);
            break;
    }

    return ret;
}

int ftruncate(int fd, off_t length)
{
    int ret;

    if (ocall_ftruncate(&ret, fd, length) != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}

int unlinkat(int dirfd, const char *pathname, int flags)
{
    int ret;

    if (ocall_unlinkat(&ret, dirfd, pathname, flags) != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}