cmake_minimum_required(VERSION 3.20)
cmake_policy(VERSION 2.30)

# ------------------------------------------
# Directories

set(YAVE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YAVE_INCLUDE_DIR ${YAVE_DIR}/include)
set(YAVE_SOURCE_DIR ${YAVE_DIR}/src)
set(YAVE_EXTERNAL_DIR ${YAVE_DIR}/external)
set(YAVE_TEST_DIR ${YAVE_DIR}/test)
set(YAVE_TOOL_DIR ${YAVE_DIR}/tools)

# ------------------------------------------
# Build type

if(NOT (CMAKE_BUILD_TYPE MATCHES Debug|Release|RelWithDebInfo|MinSizeRel))
  message(FATAL_ERROR "Invalid build type!")
endif()

set(BUILD_SHARED_LIBS OFF)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ------------------------------------------
# Git

find_package(Git REQUIRED)

# ------------------------------------------
# Package manager

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init ${VCPKG_SOURCE_DIR})
set(VCPKG_SOURCE_DIR ${YAVE_EXTERNAL_DIR}/vcpkg)

# select appropriate triplet and toolchain
if(WIN32)
  # dynamic CRT, static libs
  set(VCPKG_TARGET_TRIPLET x64-windows-static-md CACHE STRING "")
endif()

# set toolchain
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

# ------------------------------------------
# Project

project(yave CXX)

# ------------------------------------------
# Options

option(YAVE_BUILD_TESTS "Build tests" ON)
option(YAVE_ADDRESS_SANITIZER "Enable address sanitizer" OFF)
option(YAVE_THREAD_SANITIZER "Enable thread sanitizer" OFF)
option(YAVE_UB_SANITIZER "Enable undefined behaviour sanitizer" OFF)
option(YAVE_MEMORY_SANITIZER "Enable meomry sanitizer" OFF)

# ------------------------------------------
# Compilers

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}, Version: ${CMAKE_CXX_COMPILER_VERSION}")

if(NOT (CMAKE_CXX_COMPILER_ID MATCHES MSVC|Clang|GNU))
  message(FATAL_ERROR "Unsupported compiler!")
endif()

# ------------------------------------------
# Export compile command database

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------
# Default C++ standard

set(CMAKE_CXX_STANDARD 20)

# ------------------------------------------
# External

include(${YAVE_EXTERNAL_DIR}/external.cmake)

# ------------------------------------------
# Compiler Flags

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  # Remove /RTC1 from default set of compile flags.
  STRING (REGEX REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang|GNU)
  # Use -O2
  STRING (REGEX REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Common compiler flags
set(YAVE_COMPILE_FLAGS
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /Zi /EHsc /permissive- /w34716 /d2FH4 /Zc:__cplusplus>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -g -pedantic -Wshadow>
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -g -pedantic -Wshadow-compatible-local>
)

# Common linker flags
set(YAVE_LINK_FLAGS
  $<$<CXX_COMPILER_ID:Clang>:-latomic>
)

# Sanitizer options
if(YAVE_ADDRESS_SANITIZER)
  set(YAVE_SANITIZER_STRING "address")
elseif(YAVE_THREAD_SANITIZER)
  set(YAVE_SANITIZER_STRING "thread")
elseif(YAVE_UB_SANITIZER)
  set(YAVE_SANITIZER_STRING "undefined")
elseif(YAVE_MEMORY_SANITIZER)
  set(YAVE_SANITIZER_STRING "memory")
endif()

# Apply sanitizer options
if(YAVE_SANITIZER_STRING AND (CMAKE_CXX_COMPILER_ID MATCHES Clang|GNU))
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=${YAVE_SANITIZER_STRING} -fno-omit-frame-pointer)
  set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=${YAVE_SANITIZER_STRING})
endif()

# for Ninja
if(${CMAKE_MAKE_PROGRAM} MATCHES "ninja" )
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} 
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics> 
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>)
endif()

# ------------------------------------------
# Configuration

add_library(yave-config INTERFACE)
add_library(yave::config ALIAS yave-config)
target_include_directories(yave-config INTERFACE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-config INTERFACE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-config INTERFACE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-config INTERFACE Boost::boost)
target_link_libraries(yave-config INTERFACE Boost::disable_autolinking)
target_link_libraries(yave-config INTERFACE range-v3)
target_link_libraries(yave-config INTERFACE fmt::fmt)

# ------------------------------------------
# Subdirectories

add_subdirectory(${YAVE_SOURCE_DIR})
add_subdirectory(${YAVE_TOOL_DIR})

if(YAVE_BUILD_TESTS)
  include (CTest)
  enable_testing()
  add_subdirectory(${YAVE_TEST_DIR})
endif()