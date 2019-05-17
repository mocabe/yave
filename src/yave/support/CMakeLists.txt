add_library(yave-tools-log log.cpp)
add_library(yave::tools::log ALIAS yave-tools-log)

target_include_directories(yave-tools-log PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-tools-log PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-tools-log PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-tools-log PUBLIC spdlog)

add_library(yave-tools-uid id.cpp)
add_library(yave::tools::uid ALIAS yave-tools-uid)

target_include_directories(yave-tools-uid PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-tools-uid PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-tools-uid PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-tools-uid PRIVATE Boost::boost)
target_link_libraries(yave-tools-uid PRIVATE Boost::random)
target_link_libraries(yave-tools-uid PRIVATE Boost::disable_autolinking)

add_library(yave-tools-uuid uuid.cpp)
add_library(yave::tools::uuid ALIAS yave-tools-uuid)
target_include_directories(yave-tools-uuid PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-tools-uuid PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-tools-uuid PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-tools-uuid PRIVATE Boost::boost)

add_library(yave-tools-error error.cpp)
add_library(yave::tools::error ALIAS yave-tools-error)

target_include_directories(yave-tools-error PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-tools-error PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-tools-error PRIVATE ${YAVE_LINK_FLAGS})