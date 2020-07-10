# yave::module::std

add_library(yave-module-std
  color/color.cpp
  filesystem/path.cpp
  geometry/rect.cpp
  geometry/vec.cpp
  geometry/mat.cpp
  image/image.cpp
  list/list.cpp
  list/algorithm.cpp
  logic/apply.cpp
  logic/if.cpp
  primitive/primitive.cpp
  render/frame.cpp
  render/mix.cpp
  time/retime.cpp 
  time/time.cpp
  transform/rotate.cpp
  transform/translate.cpp
  shape/circle.cpp
  shape/draw.cpp
  shape/transform.cpp
  module.cpp
  module_loader.cpp
)

add_library(yave::module::std ALIAS yave-module-std)

target_link_libraries(yave-module-std PUBLIC yave::config)
target_link_libraries(yave-module-std PUBLIC yave::node::core)
target_link_libraries(yave-module-std PRIVATE yave::support::log)
target_link_libraries(yave-module-std PRIVATE yave::support::error)
target_link_libraries(yave-module-std PRIVATE yave::lib::image_buffer)
target_link_libraries(yave-module-std PRIVATE yave::lib::frame_buffer)
target_link_libraries(yave-module-std PRIVATE yave::lib::scene)
target_link_libraries(yave-module-std PRIVATE yave::lib::vulkan)
target_link_libraries(yave-module-std PRIVATE yave::lib::image)
target_link_libraries(yave-module-std PRIVATE yave::lib::shape)
