add_executable(UUID UUID.cpp)

target_link_libraries(UUID Boost::boost)
target_link_libraries(UUID Boost::program_options)
target_link_libraries(UUID fmt::fmt)

if(WIN32)
  # BCryptGenRandom, etc.
  target_link_libraries(UUID Bcrypt)
endif()