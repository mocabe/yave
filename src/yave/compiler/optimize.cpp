//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>
#include <yave/compiler/executable.hpp>

namespace yave::compiler {

  void optimize(pipeline& pipe)
  {
    assert(pipe.get_data_if<message_map>("msg_map"));
    assert(pipe.get_data_if<executable>("exe"));

    // TODO
  }
} 