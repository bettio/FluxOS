cmake_minimum_required (VERSION 2.6) 
project (kernel)

SET(UMF_LINUX_X86_64 ON)
SET(UMF ON)

if (BUILD_TARGET MATCHES "UMM_LINUX_X86_64")
    SET(UMM_LINUX_X86_64 ON)
    SET(UMM ON)


    SET(CMAKE_C_FLAGS "-nostdlib -nostdinc -Wall -fno-builtin -g" )
    SET(CMAKE_CXX_FLAGS "-nostdlib -nostdinc -Wall -fno-builtin -fno-exceptions -fno-threadsafe-statics -fno-rtti -g" )

elseif (BUILD_TARGET MATCHES "NATIVE_IA32")
    SET(NATIVE_IA32 ON)

    SET(CMAKE_ASM_COMPILER "g++" )
    SET(CMAKE_ASM_FLAGS "-m32 -nostdlib -nostdinc -fomit-frame-pointer -Wall -Wno-long-long -march=i386 -fno-builtin -fno-exceptions -fno-rtti -c")
    include(CMakeASMInformation)
    enable_language(ASM)
    set(can_use_assembler TRUE)
    SET(CMAKE_ASM_COMPILER "g++" )
    SET(CMAKE_ASM_FLAGS "-m32 -nostdlib -nostdinc -fomit-frame-pointer -Wall -Wno-long-long -march=i386 -fno-builtin -fno-exceptions -fno-rtti -c")

    SET(CMAKE_C_FLAGS "-m32 -nostdlib -nostdinc -fomit-frame-pointer -Wall -Wno-long-long -march=i386 -fno-builtin" )
    SET(CMAKE_CXX_FLAGS "-m32 -nostdlib -nostdinc -fomit-frame-pointer -Wall -Wno-long-long -march=i386 -fno-builtin -fno-exceptions -fno-rtti" )

elseif (BUILD_TARGET MATCHES "MIPS_24K")
    SET(NATIVE_MIPS ON)
    SET(MIPS_24K ON)

    SET(CMAKE_ASM_COMPILER "${COMPILER_PREFIX}g++" )
    include(CMakeASMInformation)
    enable_language(ASM)
    set(can_use_assembler TRUE)
    SET(CMAKE_ASM_COMPILER "${COMPILER_PREFIX}g++" )

    SET(CMAKE_C_FLAGS "-mips32 -EL -msoft-float -static -nostdlib -nostdinc -fno-stack-protector -Wall -fno-builtin -g -nodefaultlibs -nostartfiles" )
    SET(CMAKE_CXX_FLAGS "-mips32 -EL -msoft-float -static -nostdlib -nostdinc -fno-stack-protector -Wall -fno-builtin -g -nodefaultlibs -nostartfiles -fno-exceptions -fno-threadsafe-statics -fno-rtti" )
    SET(CMAKE_ASM_FLAGS "-mips32 -EL -msoft-float -static -nostdlib -nostdinc -fno-stack-protector -Wall -fno-builtin -g -nodefaultlibs -nostartfiles -fno-exceptions -fno-threadsafe-statics -fno-rtti" )

    SET(CMAKE_C_COMPILER "${COMPILER_PREFIX}gcc" )
    SET(CMAKE_CXX_COMPILER "${COMPILER_PREFIX}g++" )


elseif (BUILD_TARGET MATCHES "VERSATILE_ARM")
    SET(NATIVE_ARM ON)
    SET(VERSATILE_ARM ON)

    SET(CMAKE_ASM_COMPILER "${COMPILER_PREFIX}g++" )
    include(CMakeASMInformation)
    enable_language(ASM)
    set(can_use_assembler TRUE)
    SET(CMAKE_ASM_COMPILER "${COMPILER_PREFIX}g++" )

    SET(CMAKE_C_FLAGS "-mcpu=arm926ej-s -g -fomit-frame-pointer -nostdlib -nostdinc -Wall -fno-builtin -g" )
    SET(CMAKE_CXX_FLAGS "-mcpu=arm926ej-s -g -fomit-frame-pointer -nostdlib -nostdinc -Wall -fno-builtin -fno-exceptions -fno-threadsafe-statics -fno-rtti -g" )
    SET(CMAKE_ASM_FLAGS "-mcpu=arm926ej-s -g  -nostdlib -nostdinc -fomit-frame-pointer -Wall  -fno-builtin -fno-exceptions -fno-rtti -c")

    SET(CMAKE_C_COMPILER "${COMPILER_PREFIX}gcc" )
    SET(CMAKE_CXX_COMPILER "${COMPILER_PREFIX}g++" )
endif (BUILD_TARGET MATCHES "UMM_LINUX_X86_64")

include_directories(include/)
include_directories(include/lib/libc)
include_directories(include/lib/koof)

if (BUILD_TARGET MATCHES "UMM_LINUX_X86_64")
    include_directories(include/arch/umf/umf-linux-x86_64)

elseif (BUILD_TARGET MATCHES "NATIVE_IA32")
    include_directories(include/arch/ia32/ia32)

elseif (BUILD_TARGET MATCHES "MIPS_24K")
    include_directories(include/arch/mips/mips)

elseif (BUILD_TARGET MATCHES "VERSATILE_ARM")
    include_directories(include/arch/arm/versatilearm)

endif (BUILD_TARGET MATCHES "UMM_LINUX_X86_64")

if (UMM)
    include(arch/umf/umf.cmake)

elseif (NATIVE_IA32)
    include(arch/ia32/ia32.cmake)

elseif (MIPS_24K)
    include(arch/mips/mips.cmake)

elseif (VERSATILE_ARM)
    include(arch/arm/arm.cmake)
    include(arch/arm/platforms/versatile/versatile.cmake)
endif (UMM)

include(core/core.cmake)
include(drivers/drivers.cmake)
include(filesystem/filesystem.cmake)
include(lib/libc/libc.cmake)
include(lib/koof/koof.cmake)
include(mm/mm.cmake)
include(task/task.cmake)
include(uapi/uapi.cmake)
include(net/net.cmake)

set(KERNEL_SRCS ${KERNEL_SRCS}
    main.cpp
)

add_executable(vmflux ${KERNEL_SRCS})

if (NATIVE_IA32)
    target_link_libraries(vmflux libgcc.a)
    SET_TARGET_PROPERTIES(vmflux PROPERTIES LINK_FLAGS "-Ttext=0x100000 -Wl,--build-id=none" )

elseif (MIPS_24K)
    target_link_libraries(vmflux libgcc.a)
    SET_TARGET_PROPERTIES(vmflux PROPERTIES LINK_FLAGS "-Ttext=0x80100000 -Wl,--build-id=none" )

elseif (VERSATILE_ARM)
    target_link_libraries(vmflux libgcc.a)
    SET_TARGET_PROPERTIES(vmflux PROPERTIES LINK_FLAGS "-T arch/arm/platforms/versatile/versatile.ld")
endif (NATIVE_IA32)

