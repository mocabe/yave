//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>

namespace yave::compiler {

  auto init_pipeline() -> pipeline
  {
    pipeline pipe;
    pipe.add_data("msg_map", message_map());
    return pipe;
  }
}