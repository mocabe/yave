cmake_minimum_required(VERSION 3.13)

project(yave CXX)

# ------------------------------------------
# Directories

set(YAVE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YAVE_INCLUDE_DIR ${YAVE_DIR}/include)
set(YAVE_SOURCE_DIR ${YAVE_DIR}/src)
set(YAVE_EXTERNAL_DIR ${YAVE_DIR}/external)
set(YAVE_TEST_DIR ${YAVE_DIR}/test)
set(YAVE_TOOL_DIR ${YAVE_DIR}/tools)

# ------------------------------------------
# Options

option(YAVE_BUILD_TESTS "Build tests" ON)

# ------------------------------------------
# Build type

set(BUILD_SHARED_LIBS OFF)

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

  include(${YAVE_EXTERNAL_DIR}/external.cmake)

# ------------------------------------------
# Compiler Flags

if(YAVE_COMPILER_MSVC)
  set(YAVE_COMPILE_FLAGS /W4 /Zi /EHsc /permissive- /w34716 /d2FH4)
else()
  set(YAVE_COMPILE_FLAGS -O0 -Wall -Wextra -g -pedantic)
endif()

if(YAVE_COMPILER_LLVM)
  set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -Wshadow)
  if(${CMAKE_MAKE_PROGRAM} MATCHES "ninja" )
    set(YAVE_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS} -fcolor-diagnostics)
    set(YAVE_LINK_FLAGS ${YAVE_LINK_FLAGS} -latomic)
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
add_subdirectory(${YAVE_TOOL_DIR})

if(YAVE_BUILD_TESTS)
  include (CTest)
  enable_testing()
  add_subdirectory(${YAVE_TEST_DIR})
endif()