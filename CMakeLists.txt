cmake_minimum_required(VERSION 2.8)

set (CMAKE_VERBOSE_MAKEFILE 1)

project (ele C)
set (serial "1.0.0")
set (soserial "1")

set (CMAKE_BUILD_TYPE Debug)

set (TARGET_NAME libhello)
file (GLOB SRCS src/*.c)
set (INCLUDES
    include
)
include_directories (${INCLUDES})
set (CMAKE_C_FLAGS_RELEASE "-Wall -O2")
set (CMAKE_C_FLAGS_DEBUG "-g")
add_library (${TARGET_NAME} SHARED 
    ${SRCS}
)
set_target_properties (${TARGET_NAME} PROPERTIES 
    VERSION ${serial} SOVERSION ${soserial}
)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing ()
    include_directories (tests)
    set (test_libraries ${libraries} pcunit)
    macro (add_unit_test args)
        set (target ${ARGV0})
        add_executable (${ARGV})
        target_link_libraries (${target} ${test_libraries})
        add_test (
            ${target} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}
            "--log_level=warning"
        )
    endmacro (add_unit_test)
    include (unittests.cmake)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
