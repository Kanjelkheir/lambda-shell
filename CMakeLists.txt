cmake_minimum_required(VERSION 3.13)

project(lambda-shell LANGUAGES C)

# Set C standard (fallback to C17 if C23 not supported)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compile flags for better debugging and warnings
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Shell executable
add_executable(shell
    src/main.c
)

# Set include directories for shell target
target_include_directories(shell PRIVATE src)

# Try to find readline library (optional)
find_library(READLINE_LIBRARY readline)
if(READLINE_LIBRARY)
    target_link_libraries(shell PRIVATE ${READLINE_LIBRARY})
    message(STATUS "Found readline library: ${READLINE_LIBRARY}")
else()
    message(WARNING "readline library not found - building without readline support")
endif()

# Tests executable (optional - only if CUnit is available)
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS AND EXISTS "${CMAKE_SOURCE_DIR}/src/tests.c")
    # Try to find CUnit
    find_library(CUNIT_LIBRARY cunit)
    find_path(CUNIT_INCLUDE_DIR CUnit/CUnit.h)

    if(CUNIT_LIBRARY AND CUNIT_INCLUDE_DIR)
        add_executable(tests
            src/tests.c
        )

        target_include_directories(tests PRIVATE src ${CUNIT_INCLUDE_DIR})
        target_link_libraries(tests PRIVATE ${CUNIT_LIBRARY})

        if(READLINE_LIBRARY)
            target_link_libraries(tests PRIVATE ${READLINE_LIBRARY})
        endif()

        message(STATUS "CUnit found - tests will be built")
        message(STATUS "CUnit library: ${CUNIT_LIBRARY}")
        message(STATUS "CUnit include: ${CUNIT_INCLUDE_DIR}")
    else()
        message(WARNING "CUnit library not found - tests will not be built")
        message(STATUS "To install CUnit on Ubuntu/Debian: sudo apt-get install libcunit1-dev")
        message(STATUS "To install CUnit on CentOS/RHEL: sudo yum install CUnit-devel")
        message(STATUS "To disable tests: cmake -DBUILD_TESTS=OFF .")
    endif()
endif()

# Set output directory to bin/ subdirectory
set_target_properties(shell PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(TARGET tests)
    set_target_properties(tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Custom target to run just the shell
add_custom_target(run-shell
    COMMAND ${CMAKE_BINARY_DIR}/bin/shell
    DEPENDS shell
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Custom target to run tests
if(TARGET tests)
    add_custom_target(run-tests
        COMMAND ${CMAKE_BINARY_DIR}/bin/tests
        DEPENDS tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Build tests: ${BUILD_TESTS}")

# Installation rules
install(TARGETS shell
    RUNTIME DESTINATION bin
)

if(TARGET tests)
    install(TARGETS tests
        RUNTIME DESTINATION bin
    )
endif()
