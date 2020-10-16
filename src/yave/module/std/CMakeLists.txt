# yave::module::std

add_library(yave-module-std
  color/color.cpp
  vec/vec.cpp
  mat/mat.cpp
  list/list.cpp
  list/algorithm.cpp
  function/apply.cpp
  logic/if.cpp
  primitive/primitive.cpp
  num/num.cpp
  num/conversion.cpp
  string/string.cpp
  bool/bool.cpp
  frame/frame.cpp
  frame/blend.cpp
  frame/fill.cpp
  time/retime.cpp 
  time/time.cpp
  shape/circle.cpp
  shape/rect.cpp
  shape/draw.cpp
  shape/transform.cpp
  ops/ops.cpp
  math/ops.cpp
  module.cpp
  module_loader.cpp
)

add_library(yave::module::std ALIAS yave-module-std)

target_link_libraries(yave-module-std PUBLIC yave::config)
target_link_libraries(yave-module-std PUBLIC yave::node::core)
target_link_libraries(yave-module-std PRIVATE yave::support::log)
target_link_libraries(yave-module-std PRIVATE yave::support::error)
target_link_libraries(yave-module-std PRIVATE yave::data::frame_buffer)
target_link_libraries(yave-module-std PRIVATE yave::lib::scene)
target_link_libraries(yave-module-std PRIVATE yave::lib::vulkan)
target_link_libraries(yave-module-std PRIVATE yave::lib::image)
target_link_libraries(yave-module-std PRIVATE yave::lib::shape)
