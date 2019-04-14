function(init_lib_cmake NAME)
  message(STATUS "init ${NAME}")
  execute_process(COMMAND git submodule update --init -- external/${NAME}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(init_and_add_lib_cmake NAME)
  init_lib_cmake(${NAME})
  add_subdirectory(external/${NAME})
endfunction()

# fmt
init_and_add_lib_cmake(fmt)

# spdlog 
init_and_add_lib_cmake(spdlog)

# Cathc2
init_and_add_lib_cmake(Catch2)
