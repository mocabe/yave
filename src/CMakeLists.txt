# ------------------------------------------
# Souces

# Glob include files from current directory
function(yave_glob_include_files dst_list)
  # Get include directory for current source directory
  string(REPLACE "src" "include" _include_dir ${CMAKE_CURRENT_LIST_DIR})
  # Glob files
  file(GLOB _public_inc_files "${_include_dir}/*.hpp")
  file(GLOB _private_inc_files "*.hpp")
  # set result
  set(${dst_list} ${_public_inc_files} ${_private_inc_files} PARENT_SCOPE)
endfunction()

# Add library target
function(yave_add_library NAME)

  # source files
  set(SOURCES ${ARGN})
  list(FILTER SOURCES INCLUDE REGEX ".+\\..+")
  source_group("src" FILES ${SOURCES})

  yave_glob_include_files(_include_files)

  # Set source group for include files
  source_group("include" FILES ${_include_files})

  # Merge include files into source list
  set(SOURCES ${SOURCES} ${_include_files})

  # add library target
  add_library(${NAME} ${SOURCES})

  # convert library name to alias
  string(REPLACE "-" "::" _alias_lib_name ${NAME}) 
  add_library(${_alias_lib_name} ALIAS ${NAME})

  # set target folder
  string(REPLACE "-" "/" _lib_folder ${NAME})
  set_target_properties(${NAME} PROPERTIES FOLDER ${_lib_folder})

endfunction()

# Add interface library target
function(yave_add_interface_library NAME)

  # add library target
  add_library(${NAME} INTERFACE ${ARGN})

  # convert library name to alias
  string(REPLACE "-" "::" _alias_lib_name ${NAME}) 
  add_library(${_alias_lib_name} ALIAS ${NAME})

endfunction()

add_subdirectory(yave)
add_subdirectory(yave-imgui)
