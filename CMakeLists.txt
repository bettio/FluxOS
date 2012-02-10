cmake_minimum_required (VERSION 2.6) 
project (fluxos) 

add_subdirectory(src/kernel)
add_subdirectory(src/libc)
add_subdirectory(src/init)
add_subdirectory(src/coreutils)
add_subdirectory(src/fluxsh)
