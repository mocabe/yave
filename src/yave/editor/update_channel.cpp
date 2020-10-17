//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/update_channel.hpp>

#include <vector>

namespace yave::editor {

  class node_argument_update_channel::impl
  {
    std::vector<update_data> updates;

  public:
    void push_update(update_data d)
    {
      for (auto&& u : updates) {
        if (u.arg == d.arg) {
          u.data = std::move(d.data);
          return;
        }
      }
      updates.push_back(std::move(d));
    }

    auto consume_updates()
    {
      return std::move(updates);
    }

    auto get_current_change(const object_ptr<NodeArgument>& arg) const
      -> object_ptr<const Object>
    {
      for (auto&& u : updates) {
        if (u.arg == arg) {
          return u.data;
        }
      }
      return nullptr;
    }
  };

  node_argument_update_channel::node_argument_update_channel()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  node_argument_update_channel::node_argument_update_channel(
    node_argument_update_channel&&) noexcept = default;

  node_argument_update_channel::~node_argument_update_channel() noexcept =
    default;

  void node_argument_update_channel::push_update(update_data data)
  {
    m_pimpl->push_update(std::move(data));
  }

  auto node_argument_update_channel::consume_updates()
    -> std::vector<update_data>
  {
    return m_pimpl->consume_updates();
  }

  auto node_argument_update_channel::get_current_change(
    const object_ptr<NodeArgument>& arg) const -> object_ptr<const Object>
  {
    return m_pimpl->get_current_change(arg);
  }
}