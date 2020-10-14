//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>
#include <yave/compiler/executable.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>

namespace yave::compiler {

  void verify(pipeline& pipe)
  {
    assert(pipe.get_data_if<message_map>("msg_map"));
    assert(pipe.get_data_if<executable>("exe"));

    auto& msg_map = pipe.get_data<message_map>("msg_map");
    auto& exe     = pipe.get_data<executable>("exe");

    // check output type
    auto out_type = object_type<node_closure<FrameBuffer>>();
    if (!same_type(exe.type(), out_type)) {
      msg_map.add(invalid_output_type(out_type, exe.type()));
      pipe.set_failed();
    }

    // verbose type check
    try {
      if (same_type(type_of(exe.object()), exe.type()))
        return;
    } catch (...) {
    }
    msg_map.add(internal_compile_error("Verbose type check failed"));
    pipe.set_failed();
  }
}