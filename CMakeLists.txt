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
# Git

find_package(Git REQUIRED)

# ------------------------------------------
# Package manager

# bootstrap vcpkg (internal)
function(_bootstrap_vcpkg)
  message(STATUS "bootstraping vcpkg")
  if(WIN32)
    execute_process(COMMAND ${VCPKG_SOURCE_DIR}/bootstrap-vcpkg.bat)
  else()
    execute_process(COMMAND ${VCPKG_SOURCE_DIR}/bootstrap-vcpkg.sh)
  endif()
endfunction()

# select appropriate triplet and toolchain
function(_select_vcpkg_triplet_and_toolchain) 
  if(WIN32)
    # dynamic CRT, static libs
    set(VCPKG_TARGET_TRIPLET x64-windows-static-md CACHE STRING "")
  endif()
  set(CMAKE_TOOLCHAIN_FILE ${VCPKG_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
endfunction()

# configure vcpkg
function(vcpkg_configure)
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init ${VCPKG_SOURCE_DIR})
  if(NOT VCPKG_EXECUTABLE)
    _bootstrap_vcpkg()
  endif()
  find_program(VCPKG_EXECUTABLE vcpkg PATHS ${VCPKG_SOURCE_DIR} REQUIRED)
  _select_vcpkg_triplet_and_toolchain()
  message(STATUS "configured vcpkg toolchain")
endfunction()

# do configure
set(VCPKG_SOURCE_DIR ${YAVE_EXTERNAL_DIR}/vcpkg)
vcpkg_configure()

# ------------------------------------------
# Project

project(yave CXX)

# ------------------------------------------
# Options

option(YAVE_BUILD_TESTS "Build tests" ON)
option(YAVE_CLANG_TIDY "Enabe clang-tidy" OFF)

# ------------------------------------------
# Build type

set(CMAKE_BUILD_TYPE "Debug")
set(BUILD_SHARED_LIBS OFF)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

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
  set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od" CACHE STRING "" FORCE)
  set(YAVE_COMPILE_FLAGS /W4 /Zi /EHsc /permissive- /w34716 /d2FH4)
else()
  set(YAVE_COMPILE_FLAGS -O0 -Wall -Wextra -g -pedantic)
endif()

if(YAVE_COMPILER_LLVM)
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -Wshadow)
  set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -latomic)
  if(${CMAKE_MAKE_PROGRAM} MATCHES "ninja" )
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fcolor-diagnostics)
  endif()
  if(UNIX AND (NOT CYGWIN))
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=address)
    set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=address)
  endif()
endif()

if(YAVE_COMPILER_GNU)
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -Wshadow-compatible-local)
  if(${CMAKE_MAKE_PROGRAM} MATCHES "ninja")
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fdiagnostics-color=always)
  endif()
  if(UNIX AND (NOT CYGWIN))
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=address -fuse-ld=gold)
    set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=address -fuse-ld=gold)
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