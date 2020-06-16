//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window.hpp>

namespace yave::wm {

  window::window(std::string name)
    : m_id {uid::random_generate()}
    , m_parent {nullptr}
    , m_children {}
    , m_name {std::move(name)}
  {
  }

  window::~window() noexcept = default;

  void window::add_any_window(
    typename decltype(m_children)::iterator it,
    std::unique_ptr<window>&& win)
  {
    assert(!win->parent());
    win->set_parent(this);
    children().emplace(it, std::move(win));
  }

  void window::remove_any_window(uid id)
  {
    auto& ws = children();

    auto it = std::remove_if(
      ws.begin(), ws.end(), [&](auto& p) { return p->id() == id; });

    ws.erase(it, ws.end());
  }
}