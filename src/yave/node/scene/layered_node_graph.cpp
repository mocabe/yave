//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/layered_node_graph.hpp>
#include <yave/support/log.hpp>

namespace {

  static std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("layered_node_graph");
      return 1;
    }();
  }

} // namespace

namespace yave {

  /// Internal layer struct
  struct node_layer
  {
    struct resource
    {
      std::string name;
      node_handle handle;
      layer_resource_scope scope;
    };

    node_layer(layer_handle prnt, const std::string& nm = "New Layer")
      : parent {prnt}
      , name {nm}
    {
      init_logger();
      id = uid::random_generate();
    }

    ~node_layer() noexcept
    {
      Info(g_logger, "Destroying layer '{}'#{}", name, id.data);
    }

    uid id;
    layer_handle parent;
    std::string name;
    std::vector<resource> resources;
    std::vector<std::unique_ptr<node_layer>> sublayers;
  }; // namespace structnode_layer

  /* Private helper functions */

  auto layered_node_graph::_get_handle(
    const std::unique_ptr<node_layer>& ptr) const -> layer_handle
  {
    assert(ptr);
    return layer_handle(ptr.get(), ptr->id);
  }

  auto layered_node_graph::_access(const layer_handle& layer) -> node_layer&
  {
    assert(layer);
    return *layer.descriptor();
  }

  auto layered_node_graph::_access(const layer_handle& layer) const
    -> const node_layer&
  {
    assert(layer);
    return *layer.descriptor();
  }

  auto layered_node_graph::_access_parent(const layer_handle& layer)
    -> node_layer&
  {
    return _access(_access(layer).parent);
  }

  bool layered_node_graph::_exists(const layer_handle& layer) const
  {
    if (!layer)
      return false;

    // traverse tree with DFS.

    std::vector<layer_handle> stack;

    stack.push_back(_get_handle(m_root));

    while (!stack.empty()) {
      auto back = stack.back();
      stack.pop_back();

      if (back == layer)
        return true;

      for (auto&& sub : _access(back).sublayers) {
        stack.push_back(_get_handle(sub));
      }
    }

    return false;
  }

  bool layered_node_graph::_is_child(
    const layer_handle& parent,
    const layer_handle& child) const
  {
    if (!parent || !child)
      return false;

    if (parent == child)
      return false;

    auto dfs =
      [&](const layer_handle& root, const layer_handle& target) -> bool {
      std::vector<layer_handle> stack;

      stack.push_back(root);

      while (!stack.empty()) {
        auto back = stack.back();
        stack.pop_back();

        if (back == target)
          return true;

        for (auto&& sub : _access(back).sublayers) {
          stack.push_back(_get_handle(sub));
        }
      }
      return false;
    };

    // find child
    if (dfs(_get_handle(m_root), parent)) {
      return dfs(parent, child);
    }
    return false;
  }

  bool layered_node_graph::_movable_below(
    const layer_handle& from,
    const layer_handle& to) const
  {
    if (!_exists(from) || !_exists(to))
      return false;

    // cannot move root layer
    if (_get_handle(m_root) == from)
      return false;

    // cannot move below root layer
    if (_get_handle(m_root) == to)
      return false;

    // cannot move below itself
    if (from == to)
      return false;

    // cannot move layer below it's child
    if (_is_child(from, to))
      return false;

    return true;
  }

  bool layered_node_graph::_movable_above(
    const layer_handle& from,
    const layer_handle& to) const
  {
    return _movable_below(from, to);
  }

  bool layered_node_graph::_movable_into(
    const layer_handle& from,
    const layer_handle& to) const
  {

    if (!_exists(from) || !_exists(to))
      return false;

    // cannot move root layer
    if (_get_handle(m_root) == from)
      return false;

    // cannot move into itself
    if (from == to)
      return false;

    // cannot move layer into it's child
    if (_is_child(from, to))
      return false;

    return true;
  }

  auto layered_node_graph::_find_layer(const node_handle& resource) const
    -> layer_handle
  {
    // TODO: performance optimization

    if (!m_node_graph.exists(resource))
      return nullptr;

    std::vector<layer_handle> stack;

    stack.push_back(_get_handle(m_root));

    while (!stack.empty()) {
      auto back = stack.back();
      stack.pop_back();

      for (auto&& res : _access(back).resources) {
        if (res.handle == resource)
          return back;
      }

      for (auto&& sub : _access(back).sublayers) {
        stack.push_back(_get_handle(sub));
      }
    }
    return nullptr;
  }

  auto layered_node_graph::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

  /* Public functions */

  layered_node_graph::layered_node_graph()
    : m_node_graph {}
  {
    init_logger();
    m_root = std::make_unique<node_layer>(nullptr, "root");
  }

  layered_node_graph::~layered_node_graph()
  {
  }

  bool layered_node_graph::exists(const layer_handle& layer) const
  {
    auto lck = _lock();
    return _exists(layer);
  }

  auto layered_node_graph::root() const -> layer_handle
  {
    auto lck = _lock();
    return _get_handle(m_root);
  }

  auto layered_node_graph::layers() const -> std::vector<layer_handle>
  {
    auto lck = _lock();

    std::vector<layer_handle> ret;
    std::vector<layer_handle> stack;

    stack.push_back(_get_handle(m_root));

    while (!stack.empty()) {
      auto back = stack.back();
      stack.pop_back();

      ret.push_back(back);

      for (auto&& sub : _access(back).sublayers) {
        stack.push_back(_get_handle(sub));
      }
    }
    return ret;
  }

  auto layered_node_graph::get_info(const layer_handle& layer) const
    -> std::optional<layer_info>
  {
    // TODO: perf optimization

    auto lck = _lock();

    if (!_exists(layer))
      return std::nullopt;

    const auto& l = _access(layer);

    // sublayers
    std::vector<layer_handle> sublayers;
    {
      sublayers.reserve(l.sublayers.size());
      for (auto&& sub : l.sublayers) {
        sublayers.push_back(_get_handle(sub));
      }
    }
    // resources
    std::vector<layer_resource> resources;
    {
      resources.reserve(l.resources.size());
      for (auto&& res : l.resources) {
        resources.push_back({res.name, res.handle, res.scope});
      }
    }
    return layer_info(
      l.name, l.parent, std::move(sublayers), std::move(resources));
  }

  auto layered_node_graph::connect(
    const node_handle& src_n,
    const std::string& src_s,
    const node_handle& dst_n,
    const std::string& dst_s) -> connection_handle
  {
    return m_node_graph.connect(src_n, src_s, dst_n, dst_s);
  }

  void layered_node_graph::disconnect(const connection_handle& handle)
  {
    return m_node_graph.disconnect(handle);
  }

  void layered_node_graph::clear()
  {
    auto lck = _lock();

    m_root = nullptr;
    m_node_graph.clear();
    // re-initialize root layer
    m_root = std::make_unique<node_layer>(nullptr);
  }

  auto layered_node_graph::add_layer(const layer_handle& target) -> layer_handle
  {
    auto lck = _lock();

    if (!_exists(target))
      return nullptr;

    auto sub      = std::make_unique<node_layer>(target);
    const auto& l = _access(target).sublayers.emplace_back(std::move(sub));

    Info(
      g_logger,
      "Add new layer '{}'#{} under '{}'#{}",
      l->name,
      l->id.data,
      _access(target).name,
      target.id().data);

    return _get_handle(l);
  }

  void layered_node_graph::remove_layer(const layer_handle& layer)
  {
    auto lck = _lock();

    if (!_exists(layer))
      return;

    if (_get_handle(m_root) == layer) {
      Warning(g_logger, "Tried to remove root layer, ignored.");
      return;
    }

    auto& parent = _access_parent(layer);

    Info(
      g_logger,
      "Removing layer {}#{} and sublayers",
      _access(layer).name,
      layer.id().data);

    auto rm = std::remove_if(
      parent.sublayers.begin(), parent.sublayers.end(), [&](auto& sub) {
        return _get_handle(sub) == layer;
      });
    parent.sublayers.erase(rm, parent.sublayers.end());
  }

  void layered_node_graph::set_layer_name(
    const layer_handle& layer,
    const std::string& name)
  {
    auto lck = _lock();

    if (!_exists(layer))
      return;

    _access(layer).name = name;
  }

  auto layered_node_graph::get_layer_name(const layer_handle& layer) const
    -> std::optional<std::string>
  {
    auto lck = _lock();

    if (!_exists(layer))
      return std::nullopt;

    return _access(layer).name;
  }

  auto layered_node_graph::get_sublayers(const layer_handle& layer) const
    -> std::vector<layer_handle>
  {
    auto lck = _lock();

    if (!_exists(layer))
      return {};

    std::vector<layer_handle> ret;
    ret.reserve(_access(layer).sublayers.size());

    for (auto&& sub : _access(layer).sublayers) {
      ret.emplace_back(_get_handle(sub));
    }

    return ret;
  }

  auto layered_node_graph::get_parent(const layer_handle& layer) const
    -> layer_handle
  {
    auto lck = _lock();

    if (!_exists(layer))
      return nullptr;

    return _access(layer).parent;
  }

  bool layered_node_graph::movable_below(
    const layer_handle& from,
    const layer_handle& to) const
  {
    auto lck = _lock();
    return _movable_below(from, to);
  }

  void layered_node_graph::move_below(
    const layer_handle& from,
    const layer_handle& to)
  {
    // TODO: optimization

    auto lck = _lock();

    if (!_movable_below(from, to))
      return;

    Info(
      g_logger,
      "Moving '{}'#{} below '{}'#{}",
      _access(from).name,
      _access(from).id.data,
      _access(to).name,
      _access(to).id.data);

    // detach move
    std::unique_ptr<node_layer> tmp;
    {
      auto& parent = _access_parent(from);
      for (auto iter = parent.sublayers.begin(); iter != parent.sublayers.end();
           ++iter) {
        if (_get_handle(*iter) == from) {
          tmp = std::move(*iter);
          parent.sublayers.erase(iter);
          break;
        }
      }
    }

    // insert move under target.
    {
      auto& parent = _access_parent(to);
      for (auto iter = parent.sublayers.begin(); iter != parent.sublayers.end();
           ++iter) {
        if (_get_handle(*iter) == to) {
          // move layer before target (i.e. below)
          parent.sublayers.emplace(iter, std::move(tmp));
          break;
        }
      }
    }
  }

  bool layered_node_graph::movable_above(
    const layer_handle& from,
    const layer_handle& to) const
  {
    auto lck = _lock();
    return _movable_above(from, to);
  }

  void layered_node_graph::move_above(
    const layer_handle& from,
    const layer_handle& to)
  {
    // TODO: optimization

    auto lck = _lock();

    if (!_movable_above(from, to))
      return;

    Info(
      g_logger,
      "Moving '{}'#{} below '{}'#{}",
      _access(from).name,
      _access(from).id.data,
      _access(to).name,
      _access(to).id.data);

    // detach move
    std::unique_ptr<node_layer> tmp;
    {
      auto& parent = _access_parent(from);
      for (auto iter = parent.sublayers.begin(); iter != parent.sublayers.end();
           ++iter) {
        if (_get_handle(*iter) == from) {
          tmp = std::move(*iter);
          parent.sublayers.erase(iter);
          break;
        }
      }
    }

    // insert move under target.
    {
      auto& parent = _access_parent(to);
      for (auto iter = parent.sublayers.begin(); iter != parent.sublayers.end();
           ++iter) {
        if (_get_handle(*iter) == to) {
          // move layer next of target (i.e. above target)
          parent.sublayers.emplace(std::next(iter), std::move(tmp));
          break;
        }
      }
    }
  }

  bool layered_node_graph::movable_into(
    const layer_handle& from,
    const layer_handle& to) const
  {
    auto lck = _lock();
    return _movable_below(from, to);
  }

  void layered_node_graph::move_into(
    const layer_handle& from,
    const layer_handle& to)
  {
    auto lck = _lock();

    if (!_movable_below(from, to))
      return;

    Info(
      g_logger,
      "Moving '{}'#{} into '{}'#{}",
      _access(from).name,
      _access(from).id.data,
      _access(to).name,
      _access(to).id.data);
  }


  auto layered_node_graph::add_resource(
    const std::string& name,
    const layer_handle& layer,
    layer_resource_scope scope) -> std::optional<layer_resource>
  {
    auto lck = _lock();

    if (!_exists(layer))
      return std::nullopt;

    auto node = m_node_graph.create(name);

    if (!node)
      return std::nullopt;

    node_layer::resource res {name, node, scope};
    _access(layer).resources.push_back(res);
    return layer_resource {res.name, res.handle, res.scope};
  }

  void layered_node_graph::remove_resource(const node_handle& node)
  {
    auto lck = _lock();

    if (auto layer = _find_layer(node)) {

      assert(_exists(layer));

      for (auto iter = _access(layer).resources.begin();
           iter != _access(layer).resources.end();
           ++iter) {
        if (iter->handle == node) {
          _access(layer).resources.erase(iter);
          m_node_graph.destroy(node);
          break;
        }
      }
    }
  }

  auto layered_node_graph::get_owning_resources(const layer_handle& layer) const
    -> std::vector<layer_resource>
  {
    auto lck = _lock();

    if (!_exists(layer))
      return {};

    std::vector<layer_resource> ret;
    ret.reserve(_access(layer).resources.size());

    for (auto&& res : _access(layer).resources) {
      ret.push_back({res.name, res.handle, res.scope});
    }
    return ret;
  }

  auto
    layered_node_graph::get_inherited_resources(const layer_handle& layer) const
    -> std::vector<layer_resource>
  {
    auto lck = _lock();

    if (!_exists(layer))
      return {};

    std::vector<layer_resource> ret;

    auto parent = _access(layer).parent;

    while (parent) {
      for (auto&& res : _access(parent).resources) {
        ret.push_back(layer_resource {res.name, res.handle, res.scope});
        parent = _access(parent).parent;
      }
    }
    return ret;
  }

  void layered_node_graph::set_resource_scope(
    const node_handle& node,
    layer_resource_scope scope)
  {
    auto lck = _lock();

    if (auto layer = _find_layer(node)) {
      assert(_exists(layer));

      for (auto&& res : _access(layer).resources) {
        if (res.handle == node) {
          res.scope = scope;
          break;
        }
      }
    }
  }

  auto layered_node_graph::get_resource_scope(const node_handle& node)
    -> std::optional<layer_resource_scope>
  {
    auto lck = _lock();

    if (auto layer = _find_layer(node)) {
      assert(_exists(layer));

      for (auto&& res : _access(layer).resources) {
        if (res.handle == node) {
          return res.scope;
        }
      }
    }
    return std::nullopt;
  }

  void layered_node_graph::set_resource_name(
    const node_handle& node,
    const std::string& name)
  {
    auto lck = _lock();

    if (auto layer = _find_layer(node)) {
      assert(_exists(layer));

      for (auto&& res : _access(layer).resources) {
        if (res.handle == node) {
          res.name = name;
          return;
        }
      }
    }
  }

  auto layered_node_graph::get_resource_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    auto lck = _lock();

    if (auto layer = _find_layer(node)) {
      assert(_exists(layer));

      for (auto&& res : _access(layer).resources) {
        if (res.handle == node)
          return res.name;
      }
    }
    return std::nullopt;
  }

  bool layered_node_graph::register_node_info(const node_info& info)
  {
    return m_node_graph.register_node_info(info);
  }

  void layered_node_graph::unregister_node_info(const node_info& info)
  {
    return m_node_graph.unregister_node_info(info);
  }

  bool
    layered_node_graph::register_node_info(const std::vector<node_info>& info)
  {
    return m_node_graph.register_node_info(info);
  }

  void
    layered_node_graph::unregister_node_info(const std::vector<node_info>& info)
  {
    return m_node_graph.unregister_node_info(info);
  }

} // namespace yave