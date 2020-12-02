//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument.hpp>

#include <memory>

namespace yave::editor {

  /// update channel for node arguments
  class node_argument_update_channel
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    node_argument_update_channel();
    node_argument_update_channel(node_argument_update_channel&&) noexcept;
    ~node_argument_update_channel() noexcept;

  public:
    struct update_data
    {
      /// property node to apply update
      object_ptr<NodeArgumentPropNode> arg;
      /// new data to set
      object_ptr<const Object> data;
    };

    /// queue data update
    void push_update(update_data data);

    /// execute all updates
    void apply_updates();

    /// get current change
    [[nodiscard]] auto get_current_value(
      const object_ptr<NodeArgumentPropNode>& arg) const
      -> object_ptr<const NodeArgumentPropNode>;
  };

} // namespace yave::editor