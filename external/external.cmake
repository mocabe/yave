function(init_lib_cmake NAME)
  message(STATUS "Initializing submodule: ${NAME}")
  execute_process(COMMAND git submodule update --init --jobs 4 -- external/${NAME}
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
execute_process(COMMAND git submodule update --init --recursive --jobs 8
                WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)

message(STATUS "Building boost library")

# bootstrap
if((NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/bjam.exe OR 
    NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/b2.exe) 
    AND
   (NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/bjam OR
    NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/b2))
  execute_process(COMMAND sh bootstrap.sh 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
endif()

# Build Boost library
if(MINGW)
  # Use default "mgw" toolset. which uses gcc.
  # Use workaround for clang to link to gcc binary.
  execute_process(COMMAND ./b2 -j 8 link=static address-model=64 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
elseif(YAVE_COMPILER_MSVC)
  execute_process(COMMAND ./b2 -j 8 toolset=msvc link=static address-model=64 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
else()
  execute_process(COMMAND ./b2 -j 8 toolset=gcc link=static address-model=64 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
endif()

# MinGW clang workaround
if(MINGW)
  # get current gcc version
  execute_process(COMMAND gcc -dumpversion 
                  OUTPUT_VARIABLE YAVE_GCC_VERSION 
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.[0-9]+)?" "\\1" 
    YAVE_GCC_VERSION_MAJOR "${YAVE_GCC_VERSION}")
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.[0-9]+)?" "\\2" 
    YAVE_GCC_VERSION_MINOR "${YAVE_GCC_VERSION}")

  # set compiler version
  set(YAVE_GCC_VERSION_TAG "${YAVE_GCC_VERSION_MAJOR}${YAVE_GCC_VERSION_MINOR}")
  set(Boost_COMPILER "-mgw${YAVE_GCC_VERSION_TAG}")
endif()

# Let FindBoost create Boost targets
message(STATUS "Find package: boost")
set(BOOST_ROOT "${YAVE_EXTERNAL_DIR}/boost" CACHE PATH "" FORCE)
set (Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost COMPONENTS random program_options REQUIRED)

# Qt 5
message(STATUS "Initializing submodule: Qt5")

execute_process(COMMAND perl init-repository --module-subset=qtbase 
                WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/qt5)

message(STATUS "Building Qt5 modules")
execute_process(COMMAND sh configure -developer-build -opensource -nomake examples -nomake tests -confirm-license
                WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/qt5)
execute_process(COMMAND make -j 8
                WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/qt5)

set(Qt5_DIR ${YAVE_EXTERNAL_DIR}/qt5)

message(STATUS "Find package: Qt5Core, Qt5Widgets")
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)