cmake_minimum_required (VERSION 2.6) 
project (coreutils) 
 
include(../cmake/usefluxlibc.cmake)

build_executable(basename basename.c)
build_executable(cat cat.c)
build_executable(chmod chmod.c)
build_executable(chown chown.c)
build_executable(dirname dirname.c)
build_executable(domainname domainname.c)
build_executable(false false.c)
build_executable(hostname hostname.c)
build_executable(ln ln.c)
build_executable(ls ls.c)
build_executable(mkdir mkdir.c)
build_executable(mknod mknod.c)
build_executable(mount mount.c)
build_executable(mv mv.c)
build_executable(ps ps.c)
build_executable(pwd pwd.c)
build_executable(readlink readlink.c)
build_executable(reboot reboot.c)
build_executable(rm rm.c)
build_executable(rmdir rmdir.c)
build_executable(true true.c)
build_executable(umount umount.c)
build_executable(uname uname.c)
build_executable(touch touch.c)
