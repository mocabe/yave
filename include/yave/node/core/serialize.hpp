//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/structured_node_graph.hpp>

#include <tuple>

namespace yave {

  /// save subgraph into archive
  /// \param archive archive
  /// \param root root of created subgraph to save
  /// \param ng node graph
  template <class Archive>
  void save_user_node_graph(
    Archive& archive,
    const node_handle& root,
    const structured_node_graph& ng);

  /// load subgraph from archive
  /// \param archive archive
  /// \param root ref to result root handle
  /// \param ng ref to result node graph
  template <class Archive>
  void load_user_node_graph(
    Archive& archive,
    node_handle& root,
    structured_node_graph& ng);

} // namespace yave