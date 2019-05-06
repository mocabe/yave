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
execute_process(COMMAND git submodule update --init --recursive -- external/boost 
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Building boost library")
execute_process(COMMAND ./bootstrap.sh WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
execute_process(COMMAND ./b2 -j 8 WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)

message(STATUS "Adding local package: boost")
set(BOOST_ROOT "${YAVE_EXTERNAL_DIR}/boost" CACHE PATH "Boost submodule" FORCE)
message(STATUS "Set BOOST_ROOT to ${BOOST_ROOT}")
find_package(Boost REQUIRED)