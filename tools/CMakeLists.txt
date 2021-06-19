add_executable(UUID UUID.cpp)

target_link_libraries(UUID PRIVATE Boost::boost)
target_link_libraries(UUID PRIVATE Boost::program_options)
target_link_libraries(UUID PRIVATE Boost::disable_autolinking)
target_link_libraries(UUID PRIVATE fmt::fmt)
set_target_properties(UUID PROPERTIES FOLDER "tools")

if(WIN32)
  # BCryptGenRandom, etc.
  target_link_libraries(UUID PRIVATE Bcrypt)
endif()

add_executable(shader shader.cpp)
target_link_libraries(shader PRIVATE fmt::fmt)
target_link_libraries(shader PRIVATE glslang)
target_link_libraries(shader PRIVATE SPIRV)
target_link_libraries(shader PRIVATE glslang-default-resource-limits)
set_target_properties(shader PROPERTIES FOLDER "tools")