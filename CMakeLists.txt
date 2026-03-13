cmake_minimum_required(VERSION 3.22)
set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_NO_CYCLES TRUE)

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")
PROJECT(quadruple VERSION "${CMAKE_PROJECT_VERSION}" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

message (STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message (STATUS "CXX_STANDARD = ${CMAKE_CXX_STANDARD}")
message(STATUS "ARCH : ${CMAKE_SYSTEM_NAME}")

option(ENABLE_IMPLICIT_CASTS "enable implicit casts to and from quadruple" OFF)
option(ENABLE_TESTS "use tests" ON)
option(ENABLE_ASAN "use address sanitizer" OFF)
option(ENABLE_BENCHMARKS "use benchmarks" ON)

if (ENABLE_IMPLICIT_CASTS)
    message("Implicit casts are allowed")
    add_definitions(-DIMPLICIT_CASTS)
else ()
    message("Implicit casts are disabled")
endif()

if (ENABLE_TESTS)
    message("DEV_MOD ON")
    add_definitions(-DENABLE_TESTS)
    enable_testing()
endif()

if (ENABLE_BENCHMARKS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
endif()

if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_compile_options(
            -Werror

            -Wall
            -Wextra
            -Wpedantic

            -Wcast-align
            -Wcast-qual
            -Wconversion
            -Wctor-dtor-privacy
            -Wenum-compare
            -Wfloat-equal
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wredundant-decls
            -Wsign-conversion
            -Wsign-promo
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4 /WX)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (ENABLE_ASAN)
    find_package(Sanitizers REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

message(STATUS "DCMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_FOUND})
    set(ENV{CCACHE_COMMENTS} "1")
endif(CCACHE_FOUND)

find_package(Catch2 3.13.0 REQUIRED)

include_directories(source)
add_subdirectory(source)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()