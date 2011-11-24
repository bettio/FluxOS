cmake_minimum_required (VERSION 2.6) 
project (fluxsh) 
 
include(../cmake/usefluxlibc.cmake)
 
build_executable(fluxsh fluxsh.c)
