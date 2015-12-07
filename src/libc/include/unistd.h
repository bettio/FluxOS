#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>

#ifndef _SIZE_T_TYPE
#define _SIZE_T_TYPE
	typedef unsigned int size_t;
#endif

#if __cplusplus
extern "C"
{
#endif

extern int write(int fd, const char *buf, size_t count);
extern ssize_t read(int fd, void *buf, unsigned int count);
extern int close(int fd);

extern pid_t fork(void);
extern int execve(const char *filename, char *const argv[], char *const envp[]);

extern pid_t getpid(void);
extern pid_t getppid(void);
extern gid_t getgid(void);
extern uid_t getuid(void);
extern int setgid(gid_t gid);
extern int setuid(uid_t uid);

extern int gethostname(char *name, size_t len);
extern int sethostname(const char *name, size_t len);

extern int getdomainname(char *name, size_t len);
extern int setdomainname(const char *name, size_t len);


extern int brk(void *end_data_segment);
extern void _exit(int status);

extern ssize_t readlink(const char *path, char *buf, size_t bufsiz);

extern char *getcwd(char *buf, size_t size);
extern int chdir(const char *path);

extern int rmdir(const char *path);
extern int unlink(const char *path);
extern int link(const char *oldpath, const char *newpath);
extern int symlink(const char *oldpath, const char *newpath);
extern int rename(const char *oldpath, const char *newpath);

extern int chown(const char *path, uid_t owner, gid_t group);
extern int fchown(int fd, uid_t owner, gid_t group);
extern int lchown(const char *path, uid_t owner, gid_t group);
extern int chmod(const char *path, mode_t mode);
extern int fchmod(int fd, mode_t mode);

#include <time.h>
typedef unsigned long dev_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef unsigned long blksize_t;
typedef unsigned long blkcnt_t;

struct stat {
	unsigned long  st_dev;
	unsigned long  st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned long  st_rdev;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  st_atime_nsec;
	unsigned long  st_mtime;
	unsigned long  st_mtime_nsec;
	unsigned long  st_ctime;
	unsigned long  st_ctime_nsec;
	unsigned long  __unused4;
	unsigned long  __unused5;
};

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001


extern int stat(const char *path, struct stat *buf);
extern int fstat(int filedes, struct stat *buf);
extern int lstat(const char *path, struct stat *buf);

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

typedef size_t intptr_t;
extern void *sbrk(intptr_t increment);

#if __cplusplus
}
#endif

#endif
