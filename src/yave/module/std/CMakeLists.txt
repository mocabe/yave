# yave::module::std

yave_add_library(yave-module-std
  color/color.cpp
  vec/vec.cpp
  mat/mat.cpp
  list/list.cpp
  list/algorithm.cpp
  maybe/maybe.cpp
  function/apply.cpp
  function/fix.cpp
  logic/if.cpp
  logic/ops.cpp
  primitive/primitive.cpp
  num/num.cpp
  num/conversion.cpp
  string/string.cpp
  frame/frame.cpp
  frame/blend.cpp
  time/retime.cpp 
  time/time.cpp
  time/ops.cpp
  shape/shape.cpp
  shape/draw.cpp
  shape/transform.cpp
  shape/merge.cpp
  ops/ops.cpp
  math/ops.cpp
  math/trigonometric.cpp
  random/random.cpp
  animation/animation.cpp
  module.cpp
  module_loader.cpp
)

target_link_libraries(yave-module-std PRIVATE yave::core)
target_link_libraries(yave-module-std PUBLIC yave::node::core)
target_link_libraries(yave-module-std PRIVATE yave::data::frame_buffer)
target_link_libraries(yave-module-std PRIVATE yave::data::node)
target_link_libraries(yave-module-std PRIVATE yave::lib::scene)
target_link_libraries(yave-module-std PRIVATE yave::lib::vulkan)
target_link_libraries(yave-module-std PRIVATE yave::lib::image)
target_link_libraries(yave-module-std PRIVATE yave::lib::shape)
