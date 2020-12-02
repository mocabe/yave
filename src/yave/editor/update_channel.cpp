//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/update_channel.hpp>
#include <vector>

#include <range/v3/view.hpp>

namespace yave::editor {

  namespace rn = ranges;
  namespace rv = ranges::views;

  class node_argument_update_channel::impl
  {
    std::vector<update_data> updates;

  public:
    void push_update(update_data d)
    {
      assert(d.arg && d.data);
      for (auto&& u : updates) {
        if (u.arg == d.arg) {
          u.data = std::move(d.data);
          return;
        }
      }
      updates.push_back(std::move(d));
    }

    auto apply_updates()
    {
      for (auto&& u : updates) {
        assert(u.arg && u.data);
        u.arg->set_value(u.data);
      }
      updates.clear();
    }

    auto find_value(const object_ptr<NodeArgumentPropNode>& p) const
      -> object_ptr<const Object>
    {
      for (auto&& u : updates) {
        if (u.arg == p) {
          return u.data;
        }
      }
      return nullptr;
    }

    auto rebuild_prop_tree(const object_ptr<NodeArgumentPropNode>& p) const
      -> object_ptr<NodeArgumentPropNode>
    {
      if (p->is_value()) {
        if (auto v = find_value(p)) {
          auto r = p->clone();
          r->set_value(v);
          return r;
        }
        return p;
      }

      auto cs = p->children();

      auto newcs =
        cs //
        | rv::transform([&](auto&& c) { return rebuild_prop_tree(c); })
        | rn::to_vector;

      return make_object<NodeArgumentPropNode>(p->name(), p->type(), newcs);
    }

    auto get_current_value(const object_ptr<NodeArgumentPropNode>& arg) const
      -> object_ptr<const NodeArgumentPropNode>
    {
      return rebuild_prop_tree(arg);
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

  void node_argument_update_channel::apply_updates()
  {
    return m_pimpl->apply_updates();
  }

  auto node_argument_update_channel::get_current_value(
    const object_ptr<NodeArgumentPropNode>& arg) const
    -> object_ptr<const NodeArgumentPropNode>
  {
    return m_pimpl->get_current_value(arg);
  }
} // namespace yave::editor