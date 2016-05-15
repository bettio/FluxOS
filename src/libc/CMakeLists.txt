cmake_minimum_required (VERSION 2.6) 
project (libc)

include(../cmake/usercflags.cmake)

include_directories(include)

if ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
    SET(ARCH_SOURCES arch/ia32/syscalls.c)
elseif (BUILD_TARGET MATCHES "UMM_LINUX_X86_64")
    SET(ARCH_SOURCES arch/x86_64/syscalls.c)
elseif (BUILD_TARGET MATCHES "VERSATILE_ARM")
    SET(ARCH_SOURCES arch/arm/syscalls.c)
    target_link_libraries(${name} libgcc.a)
elseif (BUILD_TARGET MATCHES "MIPS_24K")
    SET(ARCH_SOURCES arch/mips/syscalls.c)
endif ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))

SET(LIBC_SOURCES
    ${ARCH_SOURCES}
    inet.c
    dirent.c
    malloc.c
    stdlib.c
    ctype.c
    posix/grp/grp.c
    posix/pwd/pwd.c
    string/memmove.c
    string.c
    stdio.c
    math.c
    libgen.c
    getopt-susv3.c
    syserrmsgs.c
    unistd.c
)

add_library(c-static STATIC ${LIBC_SOURCES})
add_library(c SHARED ${LIBC_SOURCES})
