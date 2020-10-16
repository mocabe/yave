//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/update_channel.hpp>

#include <vector>

namespace yave::editor {

  class node_argument_update_channel::impl
  {
    std::vector<update_data> data_list;

  public:
    void push(update_data d)
    {
      data_list.push_back(std::move(d));
    }

    auto consume()
    {
      return std::move(data_list);
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
    m_pimpl->push(std::move(data));
  }

  auto node_argument_update_channel::consume_updates()
    -> std::vector<update_data>
  {
    return m_pimpl->consume();
  }
}