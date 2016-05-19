cmake_minimum_required (VERSION 2.6) 
project (fluxsh) 

find_package(BISON)
find_package(FLEX)

BISON_TARGET(SHELL_PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
FLEX_TARGET(SHELL_LEXER scanner.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(SHELL_LEXER SHELL_PARSER)

include(../cmake/usefluxlibc.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (FLUXSH_SOURCES
     ${BISON_SHELL_PARSER_OUTPUTS}
     ${FLEX_SHELL_LEXER_OUTPUTS}
)

build_executable(fluxsh "${FLUXSH_SOURCES}" )
