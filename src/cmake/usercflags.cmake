if ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
  SET(CMAKE_C_FLAGS "-m32 -nostdlib -nostdinc -Wall -fno-builtin -g" )
  SET(CMAKE_CXX_FLAGS "-m32 -nostdlib -nostdinc -Wall -fno-builtin -g" )
endif ((BUILD_TARGET MATCHES "UMM_LINUX_I386") OR (BUILD_TARGET MATCHES "NATIVE_IA32"))
