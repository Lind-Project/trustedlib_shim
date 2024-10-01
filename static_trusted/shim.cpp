#include "shim_t.h"  /* print_string */
#include "types.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "sgx_file.h"
#include <map>
#include <sgx_spinlock.h>
#include <string>

static sgx_spinlock_t addr_info_map_lock = SGX_SPINLOCK_INITIALIZER;

static std::map<void*, MmapInfo*> addr_info_map;

int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

int ecall_shim_sample()
{
  return -25;
}

pid_t getpid(){
  return 0;
}

long syscall(long number, ...) {
    va_list args;
    va_start(args, number);
    long result = -1;

    printf("Unsupported syscall number --- \n");

    switch (number) {
        case 0: {
            printf("read\n");
            int fd = va_arg(args, int);
            void *buf = va_arg(args, void *);
            size_t count = va_arg(args, size_t);
            result = read(fd, buf, count);
            break;
        }
        case 2: {
            printf("open\n");
            const char *pathname = va_arg(args, const char *);
            int flags = va_arg(args, int);
            mode_t mode = va_arg(args, mode_t);
            result = open(pathname, flags, mode);
            break;
        }
        case SYS_statx: {
            printf("stat\n");
            int dirfd = va_arg(args, int);
            const char *pathname = va_arg(args, const char *);
            int flags = va_arg(args, int);
            unsigned int mask = va_arg(args, unsigned int);
            struct statx *statxbuf = va_arg(args, struct statx *);
            result = statx(dirfd, pathname, flags, mask, statxbuf);
            break;
        }
        case SYS_getrandom: {
            void *buf = va_arg(args, void *);
            size_t buflen = va_arg(args, size_t);
            unsigned int flags = va_arg(args, unsigned int);
            result = getrandom(buf, buflen, flags);
            break;
        }
        //Add other cases here for other syscalls
        default:
            printf("Unsupported syscall number: %ld\n", number);
            errno = ENOSYS;
            result = -1;
            break;
    }

    va_end(args);
    return result;
}

static void mmap_free(std::map<void*, MmapInfo*>::iterator& it)
{
	// NOTE: Caller function acquired the addr_info_map_lock before calling to the mmap_free() function.

	MmapInfo * addr_info = it->second;

	// free the memory allocation
	free(addr_info->m_malloc_addr);

	// Delete the aaddr_info
	delete addr_info;

	// delete the mmap entry
	addr_info_map.erase(it);
}

int munmap(void *addr, size_t len)
{

	// Find and remove the address info from the map
	sgx_spin_lock(&addr_info_map_lock);

	std::map<void*, MmapInfo*>::iterator it = addr_info_map.find(addr);
	if ( it == addr_info_map.end() || it->second == NULL) {
		sgx_spin_unlock(&addr_info_map_lock);
		// On error -1 is returned and errno is set to EINVAL
		//SGX_REPORT_ERR(SET_ERRNO);
		//FEND;
		return -1;
	}

	// Implemented mmap support is limited for the existing OpenSSL usage.
	// Verify that the usage haven't changed.
	MmapInfo* addr_info = it->second;
	if (addr_info->m_length != len) {
		sgx_spin_unlock(&addr_info_map_lock);
		
		//SGX_UNREACHABLE_CODE(SET_ERRNO); // we cannot free only part of the memory, so we will have a memory leak
		
		//FEND;
		return -1;
	}

	mmap_free(it);
	sgx_spin_unlock(&addr_info_map_lock);

	//FEND;
	return 0;
}

int fstat64(int fd, struct stat64 *statbuf) {
    return fstat(fd, (struct stat *) statbuf);
}

// ssize_t write(int fd, const void *buf, size_t count) {
//     printf("Custom write shim\n");
//     return -1;
// }

int sched_yield(void) {
    printf("Custom sched_yield shim\n");
    return -1;
}

long sysconf(int name) {
    printf("Custom sysconf shim\n");
    return -1;
}

char *getcwd(char *buf, size_t size) {
    printf("Custom getcwd shim\n");
    return NULL;
}

int ftruncate64(int fd, off_t length)
{
    return ftruncate(fd, length);
}

// ssize_t read(int fd, void *buf, size_t count) {
//     printf("Custom read shim\n");
//     return -1;
// }

// int bcmp(const void *s1, const void *s2, size_t n) {
//     //return std::bcmp(s1, s2, n);
//     printf("Custom bcmp shim\n");
//     const unsigned char *p1 = (const unsigned char *)s1;
//     const unsigned char *p2 = (const unsigned char *)s2;
//     for (size_t i = 0; i < n; i++) {
//         if (p1[i] != p2[i]) {
//             return 1;
//         }
//     }
//     return 0;
// }

// int open64(const char *pathname, int flags, ...) {
//     printf("Custom open64 shim\n");
//     return -1;
// }

// int open(const char *pathname, int flags, ...) {
//     printf("Custom open shim\n");
//     return -1;
// }

// int close(int fd)
// {
//     printf("Custom close shim\n");
//     return -1;    
// }

int unlink(const char *pathname) {

    return unlinkat(AT_FDCWD, pathname, 0);

}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
    printf("Custom readlink shim\n");
    return -1;
}

int stat64(const char *pathname, struct stat64 *statbuf) {
    return stat(pathname, (struct stat *)statbuf);
}

// char *realpath(const char *path, char *resolved_path) {
//     printf("Custom realpath shim\n");
//     return NULL;
// }

// int clock_gettime(clockid_t clk_id, struct timespec *tp) {
//     printf("Custom clock_gettime shim\n");
//     return -1;
// }

// int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
//     printf("Custom pthread_key_create shim\n");
//     return -1;
// }

// int pthread_key_delete(pthread_key_t key) {
//     printf("Custom pthread_key_delete shim\n");
//     return -1;
// }

// int pthread_setspecific(pthread_key_t key, const void *value) {
//     printf("Custom pthread_setspecific shim\n");
//     return -1;
// }

static void * mmap_alloc (size_t length)
{
	MmapInfo * info = NULL;
	// Allocate memory range greater than required by a page size.
	// This is needed to make the return address aligned to the page size.

	void* malloc_addr = malloc(length + PAGE_SIZE);
	if ( malloc_addr == NULL ) {
        printf("failed alloc\n");
		return NULL;
	}

	// The return address must be a multiple of the page size
	uint64_t addr = (uint64_t) malloc_addr;
	addr += PAGE_SIZE;
	addr &= ~(PAGE_SIZE - 1);
	void* ret_addr = (void *)addr;

	try {
		info = new MmapInfo(malloc_addr, length);
	}
	catch (std::bad_alloc e) {
		(void)e; // remove warning
		free(malloc_addr);
		return NULL;
	}
	// Add the address info into the map
	sgx_spin_lock(&addr_info_map_lock);
	addr_info_map[ret_addr] = info;
	sgx_spin_unlock(&addr_info_map_lock);

	return ret_addr;
}

void * mmap (void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{

	if (addr != NULL)
		// prot != (PROT_READ | PROT_WRITE) ||
		// (flags != (MAP_ANON | MAP_PRIVATE)  &&  fd != -1) ||
		// (flags != MAP_PRIVATE  &&  fd != FAKE_DEV_ZERO_FD) ||
		// offset != 0 
        //)
	{
        printf("failed mmap 1\n");
		// On error, the value MAP_FAILED (that is, (void *) -1) is returned
		return MAP_FAILED;
	}

	void * mem_addr = mmap_alloc(len);
	if (mem_addr == NULL) {
        printf("failed mmap\n");
		// On error, the value MAP_FAILED (that is, (void *) -1) is returned
		return MAP_FAILED;
	}

	// Memory allocated with MAP_ANON flag should be initialized to 0
	memset(mem_addr, 0, len);

	return mem_addr;

}

void *mremap(void *old_address, size_t old_size, size_t new_size, int flags, ...) {
    printf("Custom mremap shim\n");
    return MAP_FAILED;
}

// int fcntl(int fd, int cmd, ...) {
//     printf("Custom fcntl shim\n");
//     return -1;
// }

int shutdown(int sockfd, int how) {
    printf("Custom shutdown shim\n");
    return -1;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    printf("Custom writev shim\n");
    return -1;
}

// int memcmp(const void *s1, const void *s2, size_t n) {
//     printf("Custom memcmp shim\n");
//     return -1;
// }

_Unwind_Ptr _Unwind_GetIPInfo(_Unwind_Context *context, int *ip_before_insn) {
    printf("Custom _Unwind_GetIPInfo shim\n");
    return 0;
}

// int pthread_mutex_destroy(pthread_mutex_t *mutex) {
//     printf("Custom pthread_mutex_destroy shim\n");
//     return -1;
// }

// int pthread_cond_destroy(pthread_cond_t *cond) {
//     printf("Custom pthread_cond_destroy shim\n");
//     return -1;
// }

int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
    
    int ret;

    if (ocall_sched_getaffinity(&ret, pid, cpusetsize, (void *) mask) != SGX_SUCCESS) {
        return -1;
    }

    return ret; 
}

char *__xpg_strerror_r(int errnum, char *buf, size_t buflen) {
    printf("Custom __xpg_strerror_r shim\n");
    return NULL;
}

char *getenv(const char *name) {
    printf("Custom getenv shim %s\n", name);
    return NULL;
}

void *mmap64(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    printf("Custom mmap64 shim\n");
    return MAP_FAILED;
}

int puts(const char *s)
{
    printf(s);
    return 0;
}

int pthread_kill(pthread_t thread)
{
    printf("custom pthread_kill\n");
}

// off_t lseek64(int fd, off_t offset, int whence) 
// {
//     printf("Custom lseek64 shim\n");
//     return -1;
// }