# ------------------------------------------
# Testing

# Catch2 lib
add_library(yave_Catch2 catch.cpp)
target_link_libraries(yave_Catch2 PRIVATE Catch2::Catch2)

# add test
function (YAVE_Test NAME LABEL)
  set(TARGET test_${NAME})
  add_executable(${TARGET} ${NAME}.cpp ${YAVE_TEST_DIR}/catch.cpp)
  target_link_libraries(${TARGET} PRIVATE spdlog)
  target_link_libraries(${TARGET} PRIVATE Catch2::Catch2)
  target_link_libraries(${TARGET} PRIVATE yave_Catch2)
  target_include_directories(${TARGET} PRIVATE "${YAVE_INCLUDE_DIR}")
  target_compile_features(${TARGET} PRIVATE cxx_std_17)
  add_test(NAME ${TARGET} COMMAND ${TARGET})
  set_tests_properties(${TARGET} PROPERTIES LABELS ${LABEL})
endfunction()

# ------------------------------------------
# Tests

add_subdirectory(graph)