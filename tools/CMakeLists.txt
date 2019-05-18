add_executable(UUID UUID.cpp)

target_link_libraries(UUID PRIVATE Boost::boost)
target_link_libraries(UUID PRIVATE Boost::program_options)
target_link_libraries(UUID PRIVATE Boost::disable_autolinking)
target_link_libraries(UUID PRIVATE fmt::fmt)

if(WIN32)
  # BCryptGenRandom, etc.
  target_link_libraries(UUID PRIVATE Bcrypt)
endif()