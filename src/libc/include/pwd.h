#ifndef _PWD_H_
#define _PWD_H_

#include <sys/types.h>
#include <string.h>

struct passwd {
    char    *pw_name;
    char    *pw_passwd;
    uid_t    pw_uid;
    gid_t    pw_gid;
    char    *pw_gecos;
    char    *pw_dir;
    char    *pw_shell;
};

int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result);
int getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result);

#endif
