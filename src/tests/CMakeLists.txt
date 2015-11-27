cmake_minimum_required (VERSION 2.6) 
project (tests) 
 
include(../cmake/usefluxlibc.cmake)
 
build_executable(mmap mmap.c)
