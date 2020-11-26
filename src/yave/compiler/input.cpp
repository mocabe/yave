//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>

namespace yave::compiler {

  void input(
    pipeline& pipe,
    structured_node_graph ng,
    socket_handle os,
    node_declaration_map decls,
    node_definition_map defs)
  {
    assert(pipe.get_data_if<message_map>("msg_map"));

    if (!ng.exists(os)) {
      auto& msg_map = pipe.get_data<message_map>("msg_map");
      msg_map.add(internal_compile_error("Invalid program input"));
      pipe.set_failed();
      return;
    }

    pipe.add_data("ng", std::move(ng));
    pipe.add_data("os", std::move(os));
    pipe.add_data("decls", std::move(decls));
    pipe.add_data("defs", std::move(defs));
  }
} // namespace yave::compiler