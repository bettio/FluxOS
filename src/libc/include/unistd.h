#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>
#include <sys/stat.h>

#ifndef _SIZE_T_TYPE
#define _SIZE_T_TYPE
	typedef unsigned int size_t;
#endif

#if __cplusplus
extern "C"
{
#endif

extern int access(const char *pathname, int mode);
#define X_OK 1

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
