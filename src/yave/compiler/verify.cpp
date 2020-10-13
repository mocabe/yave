//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>
#include <yave/compiler/executable.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>

namespace yave::compiler {

  void verify(pipeline& pipe)
  {
    assert(pipe.get_data_if<message_map>("msg_map"));
    assert(pipe.get_data_if<executable>("exe"));

    auto& msg_map = pipe.get_data<message_map>("msg_map");
    auto& exe     = pipe.get_data<executable>("exe");

    if (!same_type(exe.type(), object_type<FrameBuffer>())) {
      msg_map.add(invalid_output_type(object_type<FrameBuffer>(), exe.type()));
      pipe.set_failed();
    }
  }
}