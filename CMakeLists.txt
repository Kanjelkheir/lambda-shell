cmake_minimum_required(VERSION 3.13)

project(c-shell)

file(GLOB_RECURSE SHELL_SOURCES src/main.c) # Only main.c for the shell executable
file(GLOB_RECURSE TEST_SOURCES src/tests.c src/utils/utils.h) # tests.c and utils.h for the test executable

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

add_executable(shell ${SHELL_SOURCES})
target_link_libraries(shell PRIVATE readline)

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE readline) # Link readline for tests if needed, though not strictly for current tests.c
