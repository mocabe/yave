# yave::lib::audio

add_library(yave-lib-audio
  audio_format.cpp)

add_library(yave::lib::audio ALIAS yave-lib-audio)

target_link_libraries(yave-lib-audio PUBLIC yave::config)
target_link_libraries(yave-lib-audio PRIVATE fmt::fmt)