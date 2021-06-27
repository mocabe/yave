# ------------------------------------------
# Testing

set(YAVE_TEST_COMPILE_FLAGS ${YAVE_COMPILE_FLAGS})
set(YAVE_TEST_LINK_FLAGS ${YAVE_LINK_FLAGS})

# Catch2 lib
add_library(yave-Catch2 catch.cpp)
target_link_libraries(yave-Catch2 PUBLIC Catch2::Catch2)
target_compile_options(yave-Catch2 PRIVATE ${YAVE_TEST_COMPILE_FLAGS})
target_link_options(yave-Catch2 PRIVATE ${YAVE_TEST_LINK_FLAGS})
set_target_properties(yave-Catch2 PROPERTIES FOLDER "test")

# add test
function (yave_add_test NAME LABEL)
  set(TARGET test-${LABEL}-${NAME})
  add_executable(${TARGET} ${NAME}.cpp)
  add_test(NAME ${TARGET} COMMAND ${TARGET})
  set_tests_properties(${TARGET} PROPERTIES LABELS ${LABEL})
  target_include_directories(${TARGET} PRIVATE "${YAVE_INCLUDE_DIR}")
  target_compile_options(${TARGET} PRIVATE ${YAVE_TEST_COMPILE_FLAGS})
  target_link_options(${TARGET} PRIVATE ${YAVE_TEST_LINK_FLAGS})
  target_link_libraries(${TARGET} PRIVATE yave-Catch2)
  target_link_libraries(${TARGET} PRIVATE yave::core)
  foreach(lib ${ARGN})
    target_link_libraries(${TARGET} PRIVATE ${lib})
  endforeach()
  string(REPLACE "-" "/" _test_folder "test-${LABEL}")
  set_target_properties(${TARGET} PROPERTIES FOLDER ${_test_folder})
endfunction()

# ------------------------------------------
# Tests

add_subdirectory(lib)
add_subdirectory(data)
add_subdirectory(rts)
add_subdirectory(node)
add_subdirectory(editor)
add_subdirectory(core)
add_subdirectory(media)