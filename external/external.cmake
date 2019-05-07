function(init_lib_cmake NAME)
  message(STATUS "Initializing submodule: ${NAME}")
  execute_process(COMMAND git submodule update --init -- external/${NAME}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(init_and_add_lib_cmake NAME)
  init_lib_cmake(${NAME})
  message(STATUS "Adding library target: ${NAME}")
  add_subdirectory(external/${NAME})
endfunction()

# fmt
init_and_add_lib_cmake(fmt)

# spdlog 
init_and_add_lib_cmake(spdlog)

# Cathc2
init_and_add_lib_cmake(Catch2)

# boost
message(STATUS "Initializing submodule: boost")
execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)

message(STATUS "Building boost library")

# bootstrap
execute_process(COMMAND sh bootstrap.sh WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)

# Build Boost library
if(MINGW)
  # Use default "mgw" toolset. which uses gcc.
  # Use workaround for clang to link to gcc binary.
  execute_process(COMMAND ./b2 -j 8 link=static address-model=64 WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
elseif(YAVE_COMPILER_MSVC)
  execute_process(COMMAND ./b2 -j 8 toolset=msvc link=static address-model=64 WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
else()
  execute_process(COMMAND ./b2 -j 8 toolset=gcc link=static address-model=64 WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
endif()

# MinGW clang workaround
if(MINGW)
  # get current gcc version
  execute_process(COMMAND gcc -dumpversion OUTPUT_VARIABLE GCC_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.[0-9]+)?" "\\1" 
    GCC_VERSION_MAJOR "${GCC_VERSION}")
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.[0-9]+)?" "\\2" 
    GCC_VERSION_MINOR "${GCC_VERSION}")

  # set compiler version
  set(GCC_VERSION_TAG "${GCC_VERSION_MAJOR}${GCC_VERSION_MINOR}")
  set(Boost_COMPILER "-mgw${GCC_VERSION_TAG}")
endif()

message(STATUS "Adding local package: boost")
set(BOOST_ROOT "${YAVE_EXTERNAL_DIR}/boost" CACHE PATH "" FORCE)
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib" CACHE PATH "" FORCE)
set (Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost COMPONENTS random REQUIRED)