# yave::core

yave_add_library(yave-core 
  uuid.cpp
  uid.cpp
  log.cpp
)
 
target_link_libraries(yave-core PUBLIC yave::config)
target_link_libraries(yave-core PRIVATE Boost::random)
target_link_libraries(yave-core PUBLIC spdlog::spdlog)

if(WIN32)
  # <uuid>: BCryptGenRandom, etc.
  target_link_libraries(yave-core PRIVATE Bcrypt)
endif()
