# yave::support::log

add_library(yave-support-log log.cpp)
add_library(yave::support::log ALIAS yave-support-log)

target_include_directories(yave-support-log PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-support-log PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-support-log PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-support-log PUBLIC spdlog)

# yave::support::id

add_library(yave-support-id id.cpp)
add_library(yave::support::id ALIAS yave-support-id)

target_include_directories(yave-support-id PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-support-id PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-support-id PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-support-id PRIVATE Boost::boost)
target_link_libraries(yave-support-id PRIVATE Boost::random)
target_link_libraries(yave-support-id PRIVATE Boost::disable_autolinking)

# yave::support::uuid

add_library(yave-support-uuid uuid.cpp)
add_library(yave::support::uuid ALIAS yave-support-uuid)

target_include_directories(yave-support-uuid PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-support-uuid PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-support-uuid PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-support-uuid PRIVATE Boost::boost)

# yave::support::error

add_library(yave-support-error error.cpp)
add_library(yave::support::error ALIAS yave-support-error)

target_include_directories(yave-support-error PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-support-error PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-support-error PRIVATE ${YAVE_LINK_FLAGS})

# yave::support

add_library(yave-support INTERFACE)
add_library(yave::support ALIAS yave-support)

target_link_libraries(yave-support INTERFACE yave-support-log)
target_link_libraries(yave-support INTERFACE yave-support-id)
target_link_libraries(yave-support INTERFACE yave-support-uuid)
target_link_libraries(yave-support INTERFACE yave-support-error)