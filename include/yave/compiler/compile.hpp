//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/compiler/pipeline.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/node/core/node_definition_store.hpp>

namespace yave::compiler {

  /// Init pipelie.
  /// output:
  /// | 'msg_map' as message_map
  [[nodiscard]] auto init_pipeline() -> pipeline;

  /// Initialize program input
  void input(
    pipeline& pipe,
    structured_node_graph ng,
    socket_handle os,
    node_definition_store defs);

  /// Parse program input.
  /// input:
  /// | 'msg_map' as message_map
  /// | 'ng'      as structured_node_graph
  /// | 'os'      as socket_handle
  void parse(pipeline& pipe);

  /// Verify semantics of parsed program, generate executable apply graph.
  /// This stage includes:
  ///  + desugar
  ///  + typecheck
  ///  + executable graph gen
  /// input:
  /// | 'msg_map' as message_map
  /// | 'ng'      as structured_node_graph
  /// | 'os'      as socket_handle
  /// | 'defs'    as node_definition_store
  /// output:
  /// | 'exe'     as executable
  /// comsumes:
  /// | 'ng', 'os', 'defs'
  void sema(pipeline& pipe);

  /// Post sema verification.
  /// input:
  /// | 'msg_map' as message_map
  /// | 'exe' as executable
  void verify(pipeline& pipe);

  /// Optimize executable.
  /// input:
  /// | 'msg_map' as message_map
  /// | 'exe' as executable
  void optimize(pipeline& pipe);
}