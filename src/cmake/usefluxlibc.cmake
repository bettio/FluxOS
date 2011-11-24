if ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
  SET(CMAKE_C_FLAGS "-m32 -nostdlib -nostdinc -Wall -fno-builtin -g" )
  SET(CMAKE_CXX_FLAGS "-m32 -nostdlib -nostdinc -Wall -fno-builtin -g" )
endif ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))

include_directories(../libc/include)

macro(build_executable name sources)
    if ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
        add_executable (${name} ../crt0/crt0.i386.c ${sources})
    endif ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))

    target_link_libraries(${name} libc libgcc.a)
endmacro(build_executable)

