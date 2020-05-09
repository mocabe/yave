# boost

message(STATUS "Find package: boost")

set(YAVE_BOOST_COMPONENTS random program_options filesystem)
set(YAVE_BOOST_VERSION 1.73.0)
set (Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)

set(BOOST_ROOT "${YAVE_EXTERNAL_DIR}/boost")
find_package(Boost ${YAVE_BOOST_VERSION} COMPONENTS ${YAVE_BOOST_COMPONENTS})

if(NOT Boost_FOUND)

  message(STATUS "Initializing submodule: boost")
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive ${YAVE_EXTERNAL_DIR}/boost)
  
  message(STATUS "Building boost library")
  
  # bootstrap
  file(GLOB b2_bin ${YAVE_EXTERNAL_DIR}/boost/b2*)

  if(NOT b2_bin)
    message(STATUS "No b2/bjam executables. Run bootstrap.")
    execute_process(COMMAND sh bootstrap.sh WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
  endif()
  
  # Build Boost library
  if(MINGW)
    # Use default "mgw" toolset. which uses gcc.
    # Use workaround for clang to link to gcc binary.
    execute_process(COMMAND ./b2 -j 8 link=static address-model=64 cxxstd=17
                    WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
  elseif(YAVE_COMPILER_MSVC)
    # Should link Boost::disable_autolinking on all targets to avoid link errors.
    execute_process(COMMAND ./b2 -j 8 toolset=msvc link=static address-model=64 cxxstd=17
                    WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
  else()
    execute_process(COMMAND ./b2 -j 8 toolset=gcc link=static address-model=64 cxxstd=17
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
  set(BOOST_ROOT "${YAVE_EXTERNAL_DIR}/boost")
  set(Boost_NO_SYSTEM_PATHS TRUE)
  find_package(Boost ${YAVE_BOOST_VERSION} COMPONENTS ${YAVE_BOOST_COMPONENTS} REQUIRED)

endif()