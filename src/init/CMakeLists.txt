cmake_minimum_required (VERSION 2.6) 
project (init) 
 
include(../cmake/usefluxlibc.cmake)
 
build_executable(init init.cpp libDataStore/StoredObject.cpp libDataStore/bson.c)

target_link_libraries(init libgcc.a)
