cmake_minimum_required (VERSION 2.6) 
project (init) 
 
include(../cmake/usefluxlibc.cmake)
 
build_executable(init init.c)

target_link_libraries(init libgcc.a)
