# yave::core

yave_add_library(yave-core 
  uuid.cpp
)
 
target_link_libraries(yave-core PRIVATE yave::config)
target_link_libraries(yave-core PRIVATE Boost::random)

if(WIN32)
  # <uuid>: BCryptGenRandom, etc.
  target_link_libraries(yave-core PRIVATE Bcrypt)
endif()
