cmake_minimum_required(VERSION 3.13)

project(yave CXX)

# ------------------------------------------
# Directories

set(YAVE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YAVE_INCLUDE_DIR ${YAVE_DIR}/include)
set(YAVE_SOURCE_DIR ${YAVE_DIR}/src)
set(YAVE_EXTERNAL_DIR ${YAVE_DIR}/external)
set(YAVE_TEST_DIR ${YAVE_DIR}/test)

# ------------------------------------------
# Options

option(YAVE_BUILD_TESTS "Build tests" ON)

# ------------------------------------------
# Build type

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

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

set(CMAKE_CXX_STANDARD 17)

# ------------------------------------------
# External

if(YAVE_BUILD_TESTS)
  include(${YAVE_EXTERNAL_DIR}/external.cmake)
endif()

# ------------------------------------------
# Compiler Flags

if(YAVE_COMPILER_MSVC)
  set(YAVE_COMPILE_FLAGS /W4 /Zi /EHsc /permissive- /w34716 /d2FH4)
else()
  set(YAVE_COMPILE_FLAGS -O0 -Wall -Wextra -Wshadow -g -pedantic)
endif()

if(UNIX AND YAVE_COMPILER_LLVM)
  message(STATUS "UB Sanitizer is enabled")
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=undefined)
  set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=undefined)
endif()

if(UNIX AND YAVE_COMPILER_GNU)
  message(STATUS "UB Sanitizer is enabled")
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fsanitize=undefined -fuse-ld=gold)
  set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -fsanitize=undefined -fuse-ld=gold)
endif()

# ------------------------------------------
# Valgrind Target

if(UNIX AND YAVE_BUILD_TESTS)
  add_custom_target(yave-valgrind-tests
      COMMAND ${CMAKE_CTEST_COMMAND} 
          --force-new-ctest-process --test-action memcheck
  COMMAND cat "${CMAKE_BINARY_DIR}/Testing/Temporary/MemoryChecker.*.log")
endif()

# ------------------------------------------
# Subdirectories

add_subdirectory(${YAVE_SOURCE_DIR})

if(YAVE_BUILD_TESTS)
  include (CTest)
  enable_testing()
  add_subdirectory(${YAVE_TEST_DIR})
endif()