//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_manager.hpp>
#include <yave/wm/root_window.hpp>

#include<yave/support/log.hpp>

#include <map>
#include <algorithm>

YAVE_DECL_G_LOGGER(window_manager)

namespace yave::wm {

  class window_manager::impl
  {
  public:
    /// window tree
    std::unique_ptr<root_window> root;
    /// window id map
    std::map<uid, window*> id_map;

  public:
    void init();
    impl();
    ~impl() noexcept;

  public:
    bool exists(uid id)
    {
      return id_map.find(id) != id_map.end();
    }

    auto get_window(uid id) -> window*
    {
      auto it = id_map.find(id);

      if (it == id_map.end())
        return nullptr;

      return it->second;
    }

  public:
    auto add_window(uid parent_id, size_t index, std::unique_ptr<window>&& w)
      -> window*
    {
      assert(!w->parent());

      auto parent = get_window(parent_id);

      if (!parent) {
        Info(
          g_logger,
          "Failed to add new window: Invalid parent ID of {}",
          to_string(parent_id));
        return nullptr;
      }

      if (index > parent->m_children.size()) {
        Warning(g_logger, "add_window(): Index out of range, adding to last");
        index = parent->m_children.size();
      }

      Info(
        g_logger,
        "Add new window {} under {}",
        to_string(w->id()),
        to_string(parent->id()));

      auto ret = w.get();

      // add to ID map
      id_map.emplace(w->id(), w.get());
      // create link
      w->m_parent = parent;
      parent->m_children.insert(
        parent->m_children.begin() + index, std::move(w));

      return ret;
    }

    void remove_window(uid window_id)
    {
      auto w = get_window(window_id);

      if (!w) {
        Error(
          g_logger,
          "remove_window(): ID {} does not exists",
          to_string(window_id));
        return;
      }

      auto p = w->parent();

      if (!p) {
        Error(g_logger, "Cannot reomve root window");
        return;
      }

      auto it = std::remove_if(
        p->m_children.begin(), p->m_children.end(), [&](auto& x) {
          return x->id() == window_id;
        });

      Info(
        g_logger, "Removing window {}", to_string(p->id()), to_string(w->id()));

      // delete link
      w->m_parent = nullptr;
      p->m_children.erase(it, p->m_children.end());
      // remove from ID map
      id_map.erase(window_id);
    }

  public:
    void update(editor::data_context& dctx, editor::view_context& vctx)
    {
      root->update(dctx, vctx);
    }

    void dispatch(window_visitor& visitor, window_traverser& traverser)
    {
      traverser.traverse(root.get(), visitor);
    }
  };

  void window_manager::impl::init()
  {
    // add root window
    root = std::make_unique<root_window>();
    // add to ID map
    id_map.emplace(root->id(), root.get());
  }

  window_manager::impl::impl()
  {
    init_logger();
    init();
  }

  window_manager::impl::~impl() noexcept = default;

  window_manager::window_manager()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  window_manager::~window_manager() noexcept = default;

  auto window_manager::add_window(
    uid parent_id,
    size_t index,
    std::unique_ptr<window>&& w) -> window*
  {
    return m_pimpl->add_window(parent_id, index, std::move(w));
  }

  void window_manager::remove_window(uid window_id)
  {
    m_pimpl->remove_window(window_id);
  }

  auto window_manager::root() const -> window*
  {
    return m_pimpl->root.get();
  }

  auto window_manager::get_window(uid id) const -> window*
  {
    return m_pimpl->get_window(id);
  }

  bool window_manager::exists(uid id) const
  {
    return m_pimpl->exists(id);
  }

  void window_manager::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->update(dctx, vctx);
  }

  void window_manager::dispatch(
    window_visitor& visitor,
    window_traverser& traverser)
  {
    return m_pimpl->dispatch(visitor, traverser);
  }

} // namespace yave::wm