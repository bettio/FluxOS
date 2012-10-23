include(filesystem/devfs/devfs.cmake)
include(filesystem/ext2/ext2.cmake)
include(filesystem/procfs/procfs.cmake)
include(filesystem/tmpfs/tmpfs.cmake)

set(KERNEL_SRCS ${KERNEL_SRCS}
    filesystem/filedescriptor.cpp
    filesystem/fscalls.cpp
    filesystem/pipe.cpp
    filesystem/vfs.cpp
    filesystem/vnodemanager.cpp
)
