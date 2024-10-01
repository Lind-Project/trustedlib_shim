#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
// #include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int pid_t;
typedef unsigned int socklen_t;
typedef unsigned long nfds_t;
typedef int key_t;

typedef int clockid_t;
typedef long time_t;
typedef unsigned int mode_t;


#define MAP_FAILED ((void *) -1)

typedef uintptr_t _Unwind_Ptr;


typedef struct _Unwind_Context {
    // Add any necessary fields here
    void *dummy;
} _Unwind_Context;



#define FD_SETSIZE 1024

typedef struct {
    unsigned int fds_bits[FD_SETSIZE / (8 * sizeof(unsigned int))];
} fd_set;

#define FD_ZERO(set) do { \
    int __i; \
    for (__i = 0; __i < (int)(sizeof(fd_set) / sizeof(unsigned int)); __i++) { \
        (set)->fds_bits[__i] = 0; \
    } \
} while (0)

#define FD_SET(fd, set) ((set)->fds_bits[(fd) / (8 * sizeof(unsigned int))] |= (1U << ((fd) % (8 * sizeof(unsigned int)))))

#define FD_CLR(fd, set) ((set)->fds_bits[(fd) / (8 * sizeof(unsigned int))] &= ~(1U << ((fd) % (8 * sizeof(unsigned int)))))

#define FD_ISSET(fd, set) ((set)->fds_bits[(fd) / (8 * sizeof(unsigned int))] & (1U << ((fd) % (8 * sizeof(unsigned int)))))

typedef struct {
    unsigned long __bits[1024 / (8 * sizeof(long))];
} cpu_set_t;

// Define struct statx manually
#ifndef _STATX_STRUCT_DEFINED
#define _STATX_STRUCT_DEFINED

#define STATX_TYPE              0x00000001U
#define STATX_MODE              0x00000002U
#define STATX_NLINK             0x00000004U
#define STATX_UID               0x00000008U
#define STATX_GID               0x00000010U
#define STATX_ATIME             0x00000020U
#define STATX_MTIME             0x00000040U
#define STATX_CTIME             0x00000080U
#define STATX_INO               0x00000100U
#define STATX_SIZE              0x00000200U
#define STATX_BLOCKS            0x00000400U
#define STATX_BASIC_STATS       0x000007ffU

struct statx_timestamp {
    int64_t tv_sec;   // seconds
    uint32_t tv_nsec; // nanoseconds
    int32_t __reserved;
};

struct statx {
    uint32_t stx_mask;
    uint32_t stx_blksize;
    uint64_t stx_attributes;
    uint32_t stx_nlink;
    uint32_t stx_uid;
    uint32_t stx_gid;
    uint16_t stx_mode;
    uint16_t __spare0[1];
    uint64_t stx_ino;
    uint64_t stx_size;
    uint64_t stx_blocks;
    uint64_t stx_attributes_mask;
    struct statx_timestamp stx_atime;
    struct statx_timestamp stx_btime;
    struct statx_timestamp stx_ctime;
    struct statx_timestamp stx_mtime;
    uint32_t stx_rdev_major;
    uint32_t stx_rdev_minor;
    uint32_t stx_dev_major;
    uint32_t stx_dev_minor;
    uint64_t stx_mnt_id;
    uint64_t __spare2;
    uint64_t __spare3[12];
};

#endif

#define SYS_statx                       332
#define SYS_getrandom                   318

#define LIND_safe_fs_access             2
#define LIND_safe_fs_unlink             4
#define LIND_safe_fs_link               5
#define LIND_safe_fs_rename             6

#define LIND_safe_fs_xstat              9
#define LIND_safe_fs_open               10
#define LIND_safe_fs_close              11
#define LIND_safe_fs_read               12
#define LIND_safe_fs_write              13
#define LIND_safe_fs_lseek              14
#define LIND_safe_fs_ioctl              15
#define LIND_safe_fs_truncate           16
#define LIND_safe_fs_fxstat             17
#define LIND_safe_fs_ftruncate          18
#define LIND_safe_fs_fstatfs            19
#define LIND_safe_fs_mmap               21
#define LIND_safe_fs_munmap             22
#define LIND_safe_fs_getdents           23
#define LIND_safe_fs_dup                24
#define LIND_safe_fs_dup2               25
#define LIND_safe_fs_statfs             26
#define LIND_safe_fs_fcntl              28

#define LIND_safe_sys_getppid           29
#define LIND_safe_sys_exit              30
#define LIND_safe_sys_getpid            31

#define LIND_safe_net_bind              33
#define LIND_safe_net_send              34
#define LIND_safe_net_sendto            35
#define LIND_safe_net_recv              36
#define LIND_safe_net_recvfrom          37
#define LIND_safe_net_connect           38
#define LIND_safe_net_listen            39
#define LIND_safe_net_accept            40

#define LIND_safe_net_getsockopt        43
#define LIND_safe_net_setsockopt        44
#define LIND_safe_net_shutdown          45
#define LIND_safe_net_select            46
#define LIND_safe_fs_getcwd             47
#define LIND_safe_net_poll              48
#define LIND_safe_net_socketpair        49
#define LIND_safe_sys_getuid            50
#define LIND_safe_sys_geteuid           51
#define LIND_safe_sys_getgid            52
#define LIND_safe_sys_getegid           53
#define LIND_safe_fs_flock              54

#define LIND_safe_net_epoll_create      56
#define LIND_safe_net_epoll_ctl         57
#define LIND_safe_net_epoll_wait        58

#define LIND_safe_fs_shmget             62
#define LIND_safe_fs_shmat              63
#define LIND_safe_fs_shmdt              64
#define LIND_safe_fs_shmctl             65

#define LIND_safe_fs_pipe               66
#define LIND_safe_fs_pipe2              67
#define LIND_safe_fs_fork               68
#define LIND_safe_fs_exec               69

#define LIND_safe_mutex_create          70
#define LIND_safe_mutex_destroy         71
#define LIND_safe_mutex_lock            72
#define LIND_safe_mutex_trylock         73
#define LIND_safe_mutex_unlock          74
#define LIND_safe_cond_create           75
#define LIND_safe_cond_destroy          76
#define LIND_safe_cond_wait             77
#define LIND_safe_cond_broadcast        78
#define LIND_safe_cond_signal           79
#define LIND_safe_cond_timedwait        80

#define LIND_safe_sem_init               91
#define LIND_safe_sem_wait               92
#define LIND_safe_sem_trywait            93
#define LIND_safe_sem_timedwait          94
#define LIND_safe_sem_post               95
#define LIND_safe_sem_destroy            96
#define LIND_safe_sem_getvalue           97

#define LIND_safe_net_gethostname       125

#define LIND_safe_fs_pread              126
#define LIND_safe_fs_pwrite             127
#define LIND_safe_fs_chdir              130
#define LIND_safe_fs_mkdir              131
#define LIND_safe_fs_rmdir              132
#define LIND_safe_fs_chmod              133
#define LIND_safe_fs_fchmod             134

#define LIND_safe_net_socket            136

#define LIND_safe_net_getsockname       144
#define LIND_safe_net_getpeername       145
#define LIND_safe_net_getifaddrs        146
#define LIND_safe_sys_sigaction		    147
#define LIND_safe_sys_kill		        148
#define LIND_safe_sys_sigprocmask	    149
#define LIND_safe_sys_lindsetitimer	    150


#define LIND_safe_fs_fchdir             161
#define LIND_safe_fs_fsync              162
#define LIND_safe_fs_fdatasync          163
#define LIND_safe_fs_sync_file_range    164

#define FAKE_DEV_ZERO_FD	99
#define PAGE_SIZE 			((uint64_t)0x1000) 	// 4096 Bytes
#define PROT_NONE			0x0
#define PROT_READ			0x1
#define PROT_WRITE			0x2
#define MAP_ANON			0x20
#define MAP_PRIVATE 		0x02

#define MADV_DONTDUMP		16
#define MAP_FAILED			(void *) -1

struct MmapInfo
{
	MmapInfo(void* malockAddr, size_t length) : m_malloc_addr(malockAddr), m_length(length) {}

	void* m_malloc_addr;
	size_t m_length;
};


pid_t getpid(void);
long syscall(long number, ...);
int munmap(void *addr, size_t length);
int fstat64(int fd, struct stat64 *statbuf);
ssize_t write(int fd, const void *buf, size_t count);
long sysconf(int name);
char *getcwd(char *buf, size_t size);
int ftruncate64(int fd, off_t length);
//int bcmp(const void *s1, const void *s2, size_t n);
int unlink(const char *pathname);
int stat64(const char *pathname, struct stat64 *statbuf);
//int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
// int pthread_key_delete(pthread_key_t key);
// int pthread_setspecific(pthread_key_t key, const void *value);
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
void *mremap(void *old_address, size_t old_size, size_t new_size, int flags, ...);
int shutdown(int sockfd, int how);
// int memcmp(const void *s1, const void *s2, size_t n);
_Unwind_Ptr _Unwind_GetIPInfo(_Unwind_Context *context, int *ip_before_insn);
// int pthread_mutex_destroy(pthread_mutex_t *mutex);
// int pthread_cond_destroy(pthread_cond_t *cond);
int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);
char *__xpg_strerror_r(int errnum, char *buf, size_t buflen);
char *getenv(const char *name);
void *mmap64(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int puts(const char *s);
int pthread_kill(pthread_t thread);


#ifdef __cplusplus
}
#endif

#endif // TYPES_H
