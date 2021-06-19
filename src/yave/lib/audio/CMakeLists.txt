# yave::lib::audio

yave_add_library(yave-lib-audio
  audio_format.cpp)

target_link_libraries(yave-lib-audio PUBLIC yave::config)
target_link_libraries(yave-lib-audio PRIVATE fmt::fmt)