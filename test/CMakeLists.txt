# ------------------------------------------
# Testing

if(YAVE_COMPILER_MSVC)
  set(YAVE_TEST_COMPILE_FLAGS /W4 /Zi /EHsc /permissive- /w34716 /d2FH4)
else()
  set(YAVE_TEST_COMPILE_FLAGS -O0 -Wall -Wextra -Wshadow -g -pedantic)
endif()

if(UNIX AND YAVE_COMPILER_LLVM)
  message(STATUS "UB Sanitizer is enabled")
  set(YAVE_TEST_COMPILE_FLAGS ${YAVE_TEST_COMPILE_FLAGS} -fsanitize=undefined)
  set(YAVE_TEST_LINK_FLAGS ${YAVE_TEST_LINK_FLAGS} -fsanitize=undefined)
endif()

if(UNIX AND YAVE_COMPILER_GNU)
  message(STATUS "UB Sanitizer is enabled")
  set(YAVE_TEST_COMPILE_FLAGS ${YAVE_TEST_COMPILE_FLAGS} -fsanitize=undefined -fuse-ld=gold)
  set(YAVE_TEST_LINK_FLAGS ${YAVE_TEST_LINK_FLAGS} -fsanitize=undefined -fuse-ld=gold)
endif()

# Catch2 lib
add_library(yave-Catch2 catch.cpp)
target_link_libraries(yave-Catch2 PUBLIC Catch2::Catch2)

# add test
function (YAVE_Test NAME LABEL)
  set(TARGET test-${LABEL}-${NAME})
  add_executable(${TARGET} ${NAME}.cpp)
  add_test(NAME ${TARGET} COMMAND ${TARGET})
  set_tests_properties(${TARGET} PROPERTIES LABELS ${LABEL})
  target_include_directories(${TARGET} PRIVATE "${YAVE_INCLUDE_DIR}")
  target_compile_options(${TARGET} PRIVATE ${YAVE_TEST_COMPILE_FLAGS})
  target_link_options(${TARGET} PRIVATE ${YAVE_TEST_LINK_FLAGS})
  target_link_libraries(${TARGET} PRIVATE yave-Catch2)
  foreach(lib ${ARGN})
    target_link_libraries(${TARGET} PRIVATE ${lib})
  endforeach()
endfunction()

# ------------------------------------------
# Tests

add_subdirectory(graph)
add_subdirectory(time)
add_subdirectory(rts)
add_subdirectory(obj)
add_subdirectory(func)
add_subdirectory(node)