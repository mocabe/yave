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
      /// argument to apply update
      object_ptr<NodeArgument> arg;
      /// new data to set
      object_ptr<const Object> data;

      /// apply update from this data
      void apply()
      {
        assert(arg && data);
        arg->set_data(data);
      }
    };

    /// queue data update
    void push_update(update_data data);
    /// execute all updates
    [[nodiscard]] auto consume_updates() -> std::vector<update_data>;
  };

} // namespace yave::editor