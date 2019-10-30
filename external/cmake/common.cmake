function(init_submodule NAME)
  message(STATUS "Initializing submodule: ${NAME}")
  execute_process(COMMAND git submodule update --init --jobs 4 -- ${YAVE_EXTERNAL_DIR}/${NAME})
endfunction()

function(init_add_submodule NAME)
  init_submodule(${NAME})
  message(STATUS "Adding library target: ${NAME}")
  add_subdirectory(${YAVE_EXTERNAL_DIR}/${NAME})
endfunction()

function(build_submodule NAME)

  set(SUBMODULE_DIR ${YAVE_EXTERNAL_DIR}/${NAME})
  set(SUBMODULE_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/${NAME})
  set(SUBMODULE_INSTALL_DIR ${SUBMODULE_BUILD_DIR}/install)

  if(MSVC) 
    # Use CMAKE_GENERATOR_PLATFORM
    execute_process(
      COMMAND 
      ${CMAKE_COMMAND} 
        -G ${CMAKE_GENERATOR} 
        -A ${CMAKE_GENERATOR_PLATFORM}
        -S ${SUBMODULE_DIR}
        -B ${SUBMODULE_BUILD_DIR} 
        -DCMAKE_INSTALL_PREFIX=${SUBMODULE_INSTALL_DIR}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        ${ARGN})
  else()
    execute_process(
      COMMAND 
      ${CMAKE_COMMAND} 
        -G ${CMAKE_GENERATOR} 
        -S ${SUBMODULE_DIR}
        -B ${SUBMODULE_BUILD_DIR} 
        -DCMAKE_INSTALL_PREFIX=${SUBMODULE_INSTALL_DIR} 
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        ${ARGN})
  endif()

  execute_process(
    COMMAND 
    ${CMAKE_COMMAND} 
      --build ${SUBMODULE_BUILD_DIR} 
      --config ${CMAKE_BUILD_TYPE}
  )

  execute_process(
    COMMAND 
    ${CMAKE_COMMAND} 
      --build ${SUBMODULE_BUILD_DIR} 
      --target install 
      --config ${CMAKE_BUILD_TYPE}
  )

  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SUBMODULE_INSTALL_DIR} PARENT_SCOPE)
endfunction()