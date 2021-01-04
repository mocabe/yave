cmake_minimum_required(VERSION 3.13)

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

if (NOT CMAKE_BUILD_TYPE) 
  set(CMAKE_BUILD_TYPE "Debug")
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

# ------------------------------------------
# Compilers

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Detected compiler: LLVM Clang")
  set(YAVE_COMPILER_LLVM true)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(STATUS "Detected compiler: GNU GCC")
  set(YAVE_COMPILER_GNU true)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  message(STATU "Detected compiler: Intel ICC")
  set(YAVE_COMPILER_INTEL true)
elseif(MSVC)
  message(STATUS "Detected compiler: Microsoft MSVC")
  set(YAVE_COMPILER_MSVC true)
else()
  message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

# ------------------------------------------
# Default C++ standard

set(CMAKE_CXX_STANDARD 20)

# ------------------------------------------
# External

include(${YAVE_EXTERNAL_DIR}/external.cmake)

# ------------------------------------------
# Compiler Flags

if(YAVE_COMPILER_MSVC)
  # Remove /RTC1 from default set of compile flags.
  STRING (REGEX REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  set(YAVE_COMPILE_FLAGS /W4 /Zi /EHsc /permissive- /w34716 /d2FH4 /Zc:__cplusplus)
  set(YAVE_LINK_FLAGS )
else()
  STRING (REGEX REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  set(YAVE_COMPILE_FLAGS -Wall -Wextra -g -pedantic)
  set(YAVE_LINK_FLAGS )
endif()

if(YAVE_COMPILER_LLVM)
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -Wshadow)
  set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -latomic)
  if(${CMAKE_MAKE_PROGRAM} MATCHES "ninja" )
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fcolor-diagnostics)
  endif()
  if(UNIX AND (NOT CYGWIN))
    if(YAVE_ADDRESS_SANITIZER)
      set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=address)
      set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=address)
    elseif(YAVE_THREAD_SANITIZER)
      set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=thread)
      set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=thread)
    endif()
  endif()
endif()

if(YAVE_COMPILER_GNU)
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -Wshadow-compatible-local)
  if(${CMAKE_MAKE_PROGRAM} MATCHES "ninja")
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fdiagnostics-color=always)
  endif()
  if(UNIX AND (NOT CYGWIN))
    if(YAVE_ADDRESS_SANITIZER)
      set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=address -fuse-ld=gold)
      set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=address -fuse-ld=gold)
    elseif(YAVE_THREAD_SANITIZER)
      set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=thread -fuse-ld=gold)
      set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=thread -fuse-ld=gold)
    endif()
  endif()
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