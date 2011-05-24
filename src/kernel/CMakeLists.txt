cmake_minimum_required (VERSION 2.6) 
project (kernel)

SET(UMF_LINUX_X86_64 ON)
SET(UMF ON)

SET(CMAKE_C_FLAGS "-nostdlib -nostdinc -Wall -fno-builtin -fno-exceptions -fno-threadsafe-statics -fno-rtti -g" )
SET(CMAKE_CXX_FLAGS "-nostdlib -nostdinc -Wall -fno-builtin -fno-exceptions -fno-threadsafe-statics -fno-rtti -g" )

include_directories(include/)
include_directories(include/lib/libc)
include_directories(include/lib/koof)


include_directories(include/arch/umf/umf-linux-x86_64)

include(arch/umf/umf.cmake)

include(core/core.cmake)
include(drivers/drivers.cmake)
include(filesystem/filesystem.cmake)
include(lib/libc/libc.cmake)
include(task/task.cmake)

set(KERNEL_SRCS ${KERNEL_SRCS}
    main.cpp
)

add_executable(vmflux ${KERNEL_SRCS})
