if ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
  SET(CMAKE_C_FLAGS "-m32 -nostdlib -nostdinc -Wall -fno-builtin -g" )
  SET(CMAKE_CXX_FLAGS "-m32 -nostdlib -nostdinc -Wall -fno-builtin -g" )

elseif ((BUILD_TARGET MATCHES "UMM_LINUX_X86_64"))
  SET(CMAKE_C_FLAGS "-nostdlib -nostdinc -Wall -fno-builtin -g" )
  SET(CMAKE_CXX_FLAGS "-nostdlib -nostdinc -Wall -fno-builtin -g") 

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

endif ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))

include_directories(../libc/include)

macro(build_executable name sources)
    if ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
        add_executable (${name} ../crt0/crt0.i386.c ${sources})
    elseif (BUILD_TARGET MATCHES "UMM_LINUX_X86_64")
        add_executable (${name} ../crt0/crt0.x86_64.c ${sources})

    elseif (BUILD_TARGET MATCHES "VERSATILE_ARM")
        add_executable (${name} ../crt0/crt0.arm.c ${sources})

    elseif (BUILD_TARGET MATCHES "MIPS_24K")
        add_executable (${name} ../crt0/crt0.mips.S ${sources})
        SET_TARGET_PROPERTIES(${name} PROPERTIES LINK_FLAGS "-Ttext=0x84100000 -Wl,--build-id=none" )

    endif ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))

    target_link_libraries(${name} libc libgcc.a)
endmacro(build_executable)

