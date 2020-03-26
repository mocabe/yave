//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/editor_context.hpp>
#include <yave/app/node_data_thread.hpp>
#include <yave/app/node_compiler_thread.hpp>

#include <yave/support/log.hpp>
#include <yave/lib/vec/vec.hpp>

#include <algorithm>
#include <range/v3/view.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/strategies/centroid.hpp>
#include <boost/geometry/strategies/envelope.hpp>
#include <boost/geometry/geometries/register/point.hpp>

// clang-format off
namespace boost::geometry::traits {

  BOOST_GEOMETRY_DETAIL_SPECIALIZE_POINT_TRAITS(
    yave::fvec2,
    2,
    float,
    cs::cartesian)

  template<> 
  struct access<yave::fvec2, 0> {
    static inline float get(const yave::fvec2& p) { return p[0]; }
    static inline void  set(yave::fvec2& p, float v) { p[0] = v; }
  };
  template<> 
  struct access<yave::fvec2, 1> {
    static inline float get(const yave::fvec2& p) { return p[1]; }
    static inline void  set(yave::fvec2& p, float v) { p[1] = v; }
  };
}
// clang-format on

YAVE_DECL_G_LOGGER(editor_context)

namespace yave::app {

  namespace rng = ranges;
  namespace bg  = boost::geometry;

  // editor data for frontend
  class editor_context_front
  {
  public:
    // current editor state
    editor_state state = editor_state::neutral;

  public: /* hovered */
    node_handle n_hovered       = {};
    socket_handle s_hovered     = {};
    connection_handle c_hovered = {};

  public: /* selected */
    std::vector<node_handle> n_selected       = {};
    std::vector<socket_handle> s_selected     = {};
    std::vector<connection_handle> c_selected = {};

  public:
    // current node group to look at
    node_handle group;

  public:
    // current scroll position
    fvec2 scroll_pos = {};
    // last drag source position
    fvec2 drag_source_pos = {};

  public:
    editor_context_front(const managed_node_graph& graph)
      : group {graph.root_group()}
    {
    }

    // set new group
    void switch_group(const node_handle& new_group, const managed_node_graph& g)
    {
      if (!new_group)
        return;

      assert(g.is_group(new_group));

      if (group == new_group)
        return;

      group      = new_group;
      scroll_pos = {};
    }

    // update handles to make it valid in new graph instance
    void refresh_nodes(const managed_node_graph& g)
    {
      auto _update = [&](auto& h) {
        using ht = std::decay_t<decltype(h)>;
        if (!h)
          return;
        if constexpr (std::is_same_v<ht, node_handle>)
          h = g.node(h.id());
        if constexpr (std::is_same_v<ht, socket_handle>)
          h = g.socket(h.id());
        if constexpr (std::is_same_v<ht, connection_handle>)
          h = g.connection(h.id());
      };

      auto _clean = [&](auto& hs) {
        using ht = typename std::decay_t<decltype(hs)>::value_type;
        auto it  = std::remove(hs.begin(), hs.end(), ht());
        hs.erase(it, hs.end());
      };

      _update(n_hovered);
      _update(s_hovered);
      _update(c_hovered);

      for (auto&& n : n_selected)
        _update(n);
      for (auto&& s : s_selected)
        _update(s);
      for (auto&& c : c_selected)
        _update(c);

      _clean(n_selected);
      _clean(s_selected);
      _clean(c_selected);

      _update(group);

      if (!group)
        switch_group(g.root_group(), g);
    }

    void begin_frame(const managed_node_graph& g)
    {
      // reset hovered node every frame
      n_hovered = {};
      s_hovered = {};
      c_hovered = {};

      // refresh handles
      refresh_nodes(g);
    }

    void end_frame()
    {
    }

    static constexpr auto _sel = [](auto&& handle, auto* _this) -> auto&
    {
      using ht = std::decay_t<decltype(handle)>;
      if constexpr (std::is_same_v<ht, node_handle>)
        return _this->n_selected;
      if constexpr (std::is_same_v<ht, socket_handle>)
        return _this->s_selected;
      if constexpr (std::is_same_v<ht, connection_handle>)
        return _this->c_selected;
    };

    static constexpr auto _hov = [](auto&& handle, auto* _this) -> auto&
    {
      using ht = std::decay_t<decltype(handle)>;
      if constexpr (std::is_same_v<ht, node_handle>)
        return _this->n_hovered;
      if constexpr (std::is_same_v<ht, socket_handle>)
        return _this->s_hovered;
      if constexpr (std::is_same_v<ht, connection_handle>)
        return _this->c_hovered;
    };

    template <class Handle>
    bool is_selected() const
    {
      auto& sel = _sel(Handle(), this);
      return !sel.empty();
    }

    template <class Handle>
    bool is_selected(const Handle& h) const
    {
      auto& sel = _sel(Handle(), this);
      auto it   = std::find(sel.begin(), sel.end(), h);
      return it != sel.end();
    }

    template <class Handle>
    auto& get_selected() const
    {
      return _sel(Handle(), this);
    }

    template <class Handle>
    void add_selected(const Handle& h)
    {
      if (!h)
        return;

      auto& sel = _sel(Handle(), this);
      auto it   = std::find(sel.begin(), sel.end(), h);
      if (it == sel.end())
        sel.push_back(h);
    }

    template <class Handle>
    void remove_selected(const Handle& h)
    {
      if (!h)
        return;

      auto& sel = _sel(Handle(), this);
      auto it   = std::remove(sel.begin(), sel.end(), h);
      sel.erase(it, sel.end());
    }

    template <class Handle>
    void clear_selected()
    {
      auto& sel = _sel(Handle(), this);
      sel       = {};
    }

    template <class Handle>
    bool is_hovered() const
    {
      auto& hov = _hov(Handle(), this);
      return hov.has_value();
    }

    template <class Handle>
    bool is_hovered(const Handle& h) const
    {
      auto& hov = _hov(Handle(), this);
      return hov && h == hov;
    }

    template <class Handle>
    auto& get_hovered() const
    {
      return _hov(Handle(), this);
    }

    template <class Handle>
    void set_hovered(const Handle& h)
    {
      if (!h)
        return;

      auto& hov = _hov(Handle(), this);
      hov       = h;
    }

    template <class Handle>
    void clear_hovered()
    {
      set_hovered(Handle());
    }

    void begin_background_drag(const tvec2<float>& pos)
    {
      Info(
        g_logger,
        "state: {} -> {}",
        to_string(state),
        to_string(editor_state::background));

      if (state != editor_state::neutral) {
        Warning(g_logger, "state != editor_state::neutral");
        return;
      }

      state           = editor_state::background;
      drag_source_pos = pos;
    }

    void begin_node_drag(const tvec2<float>& pos)
    {
      Info(
        g_logger,
        "state: {} -> {}",
        to_string(state),
        to_string(editor_state::node));

      if (state != editor_state::neutral) {
        Warning(g_logger, "state != editor_state::neutral");
        return;
      }

      state           = editor_state::node;
      drag_source_pos = pos;
    }

    void begin_socket_drag(const tvec2<float>& pos)
    {
      Info(
        g_logger,
        "state: {} -> {}, {},{}",
        to_string(state),
        to_string(editor_state::socket),
        pos.x,
        pos.y);

      if (state != editor_state::neutral) {
        Warning(g_logger, "state != editor_state::neutral");
        return;
      }

      state           = editor_state::socket;
      drag_source_pos = pos;
    }

    void end_background_drag()
    {
      Info(
        g_logger,
        "state: {} -> {}",
        to_string(state),
        to_string(editor_state::neutral));

      if (state != editor_state::background) {
        Warning(g_logger, "state != editor_state::background");
        return;
      }

      state           = editor_state::neutral;
      drag_source_pos = {};
    }

    void end_node_drag()
    {
      Info(
        g_logger,
        "state: {} -> {}",
        to_string(state),
        to_string(editor_state::neutral));

      if (state != editor_state::node) {
        Warning(g_logger, "state != editor_state::node");
        return;
      }

      state           = editor_state::neutral;
      drag_source_pos = {};
    }

    void end_socket_drag()
    {
      Info(
        g_logger,
        "state: {} -> {}",
        to_string(state),
        to_string(editor_state::neutral));

      if (state != editor_state::socket) {
        Warning(g_logger, "state != editor_state::socket");
        return;
      }

      state           = editor_state::neutral;
      drag_source_pos = {};
    }
  };

  class editor_context::impl
  {
  public:
    app::project& project;

  public:
    // data thread
    node_data_thread data_thread;
    // compiler thread
    node_compiler_thread compiler_thread;

  public:
    // current snapshot
    std::shared_ptr<const node_data_snapshot> snapshot;

  public:
    // frontend context
    editor_context_front context_front;

  public:
    // per-frame command queue
    std::vector<std::function<void(void)>> command_queue;

  public:
    // for debug
    [[maybe_unused]] bool in_frame = false;

  public:
    impl(app::project& prj)
      : project {prj}
      , data_thread {project.graph()}
      , compiler_thread {}
      , snapshot {data_thread.snapshot()}
      , context_front {project.graph()}
    {
      init_logger();

      data_thread.start();
      compiler_thread.start();
    }

    ~impl() noexcept
    {
      compiler_thread.stop();
      data_thread.stop();
    }

    void begin_frame()
    {
      assert(!in_frame);
      // update snapshot
      snapshot = data_thread.snapshot();

      auto& g = snapshot->graph;

      context_front.begin_frame(g);

      in_frame = true;
    }

    void end_frame()
    {
      assert(in_frame);
      // execute commands eunqueued
      for (auto&& cmd : command_queue) {
        cmd();
      }
      command_queue.clear();

      context_front.end_frame();

      in_frame = false;
    }

    auto& graph() const
    {
      assert(in_frame);
      return snapshot->graph;
    }

    // get refreshed handle
    static constexpr auto refh =
      [](const auto& h, const managed_node_graph& g) {
        using ht = std::decay_t<decltype(h)>;
        if (!h)
          return h;
        if constexpr (std::is_same_v<ht, node_handle>)
          return g.node(h.id());
        if constexpr (std::is_same_v<ht, socket_handle>)
          return g.socket(h.id());
        if constexpr (std::is_same_v<ht, connection_handle>)
          return g.connection(h.id());
      };

    void create_node(
      const std::string& name,
      const node_handle& group,
      const fvec2& pos)
    {
      assert(in_frame);
      data_thread.send([=](managed_node_graph& g) {
        // find decl from name
        auto decl = project.node_declarations().find(name);
        if (!decl) {
          Error(
            g_logger,
            "Could not create node: Node name {} does not exist",
            name);
          return;
        }
        // create new node
        auto h = g.create(refh(group, g), *decl);
        g.set_pos(h, pos);
      });
    }

    void destroy_node(const node_handle& node)
    {
      assert(in_frame);
      data_thread.send(
        [=](managed_node_graph& g) { g.destroy(refh(node, g)); });
    }

    void connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket)
    {
      assert(in_frame);
      data_thread.send([=](managed_node_graph& g) {
        (void)g.connect(refh(src_socket, g), refh(dst_socket, g));
      });
    }

    void disconnect(const connection_handle& c)
    {
      assert(in_frame);
      data_thread.send(
        [=](managed_node_graph& g) { g.disconnect(refh(c, g)); });
    }

    void group(const std::vector<node_handle>& nodes)
    {
      assert(in_frame);
      data_thread.send([=](managed_node_graph& g) {
        // get new handle
        auto p  = refh(context_front.group, g);
        auto ns = nodes //
                  | rng::views::transform([&](auto&& n) { return refh(n, g); })
                  | rng::to_vector;

        // get polygon
        bg::model::polygon<fvec2> poly;
        for (auto&& n : ns) {
          if (auto pos = g.get_pos(n))
            bg::append(poly.outer(), *pos);
        }

        // make group
        auto group = g.group(p, ns);

        // set new position
        if (group) {
          g.set_pos(group, bg::return_centroid<fvec2>(poly) / (float)ns.size());
        }
      });
    }

    void ungroup(const node_handle& group)
    {
      assert(in_frame);
      data_thread.send([=](managed_node_graph& g) {
        // group to remove
        auto rm_g = refh(group, g);
        // current group
        auto cr_g = context_front.group;

        if (rm_g == cr_g) {
          auto parent = g.get_parent_group(cr_g);
          context_front.switch_group(parent, g);
        }
        g.ungroup(rm_g);
      });
    }

    void set_data(const socket_handle& socket, const object_ptr<Object>& data)
    {
      assert(in_frame);
      data_thread.send(
        [=](managed_node_graph& g) { g.set_data(refh(socket, g), data); });
    }

    auto get_state() const
    {
      return context_front.state;
    }

    auto get_drag_source_pos()
    {
      assert(context_front.state != editor_state::neutral);
      return context_front.drag_source_pos;
    }

    auto get_info(const node_handle& handle) const
      -> std::optional<editor_node_info>
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      auto n  = refh(handle, g);

      if (!n)
        return std::nullopt;

      auto pos = g.get_pos(n);
      assert(pos);

      return editor_node_info {
        n, context_front.is_selected(n), context_front.is_hovered(n), *pos};
    }

    auto get_info(const socket_handle& handle) const
      -> std::optional<editor_socket_info>
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      auto s  = refh(handle, g);

      if (!s)
        return std::nullopt;

      return editor_socket_info {
        s, context_front.is_selected(s), context_front.is_hovered(s)};
    }

    auto get_info(const connection_handle& handle) const
      -> std::optional<editor_connection_info>
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      auto c  = refh(handle, g);

      if (!c)
        return std::nullopt;

      return editor_connection_info {
        c, context_front.is_selected(c), context_front.is_hovered(c)};
    }

    auto get_group() const
    {
      assert(in_frame);
      return context_front.group;
    }

    void set_group(const node_handle& node)
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      context_front.switch_group(node, g);
    }

    auto get_pos(const node_handle& node) const -> std::optional<tvec2<float>>
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      return g.get_pos(node);
    }

    void set_pos(const node_handle& node, const tvec2<float>& new_pos)
    {
      assert(in_frame);
      data_thread.send(
        [=](managed_node_graph& g) { g.set_pos(refh(node, g), new_pos); });
    }

    auto& get_scroll() const
    {
      assert(in_frame);
      return context_front.scroll_pos;
    }

    void set_scroll(const tvec2<float>& new_pos)
    {
      assert(in_frame);
      command_queue.emplace_back(
        [&, new_pos] { context_front.scroll_pos = new_pos; });
    }

    template <class Handle>
    bool is_selected() const
    {
      assert(in_frame);
      return context_front.is_selected<Handle>();
    }

    template <class Handle>
    bool is_selected(const Handle& h) const
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      return context_front.is_selected(refh(h, g));
    }

    template <class Handle>
    auto& get_selected() const
    {
      assert(in_frame);
      return context_front.get_selected<Handle>();
    }

    template <class Handle>
    void add_selected(const Handle& h)
    {
      assert(in_frame);

      if constexpr (std::is_same_v<Handle, node_handle>)
        data_thread.send(
          [=](managed_node_graph& g) { g.bring_front(refh(h, g)); });

      command_queue.emplace_back([&, h] {
        auto& g = snapshot->graph;
        if (auto hh = refh(h, g))
          context_front.add_selected(hh);
      });
    }

    template <class Handle>
    void remove_selected(const Handle& h)
    {
      assert(in_frame);
      command_queue.emplace_back([&, h] {
        auto& g = snapshot->graph;
        if (auto hh = refh(h, g))
          context_front.remove_selected(hh);
      });
    }

    template <class Handle>
    void clear_selected()
    {
      assert(in_frame);
      command_queue.push_back(
        [&]() { context_front.clear_selected<Handle>(); });
    }

    template <class Handle>
    bool is_hovered() const
    {
      assert(in_frame);
      return context_front.is_hovered<Handle>();
    }

    template <class Handle>
    bool is_hovered(const Handle& h) const
    {
      assert(in_frame);
      auto& g = snapshot->graph;
      return context_front.is_hovered(refh(h, g));
    }

    template <class Handle>
    auto& get_hovered() const
    {
      assert(in_frame);
      return context_front.get_hovered<Handle>();
    }

    template <class Handle>
    void set_hovered(const Handle& handle)
    {
      assert(in_frame);
      command_queue.emplace_back([&, handle] {
        auto& g = snapshot->graph;
        if (auto hh = refh(handle, g))
          context_front.set_hovered(hh);
      });
    }

    template <class Handle>
    void clear_hovered()
    {
      assert(in_frame);
      command_queue.emplace_back(
        [&] { context_front.clear_hovered<Handle>(); });
    }

    void begin_background_drag(const tvec2<float>& pos)
    {
      assert(in_frame);
      command_queue.push_back(
        [&, pos]() { context_front.begin_background_drag(pos); });
    }

    void begin_node_drag(const tvec2<float>& pos)
    {
      assert(in_frame);
      command_queue.push_back(
        [&, pos]() { context_front.begin_node_drag(pos); });
    }

    void begin_socket_drag(const tvec2<float>& pos)
    {
      assert(in_frame);
      command_queue.push_back(
        [&, pos]() { context_front.begin_socket_drag(pos); });
    }

    void end_background_drag()
    {
      assert(in_frame);
      command_queue.push_back([&]() { context_front.end_background_drag(); });
    }

    void end_node_drag()
    {
      assert(in_frame);
      command_queue.push_back([&]() { context_front.end_node_drag(); });
    }

    void end_socket_drag()
    {
      assert(in_frame);
      command_queue.push_back([&]() { context_front.end_socket_drag(); });
    }
  };

  editor_context::editor_context(app::project& project)
    : m_pimpl {std::make_unique<impl>(project)}
  {
  }

  editor_context::~editor_context() noexcept = default;

  void editor_context::begin_frame()
  {
    m_pimpl->begin_frame();
  }

  void editor_context::end_frame()
  {
    m_pimpl->end_frame();
  }

  auto editor_context::node_graph() const -> const managed_node_graph&
  {
    return m_pimpl->graph();
  }

  void editor_context::create(
    const std::string& name,
    const node_handle& group,
    const tvec2<float>& pos)
  {
    m_pimpl->create_node(name, group, pos);
  }

  void editor_context::destroy(const node_handle& node)
  {
    m_pimpl->destroy_node(node);
  }

  void editor_context::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket)
  {
    m_pimpl->connect(src_socket, dst_socket);
  }

  void editor_context::disconnect(const connection_handle& c)
  {
    m_pimpl->disconnect(c);
  }

  void editor_context::group(const std::vector<node_handle>& nodes)
  {
    m_pimpl->group(nodes);
  }

  void editor_context::ungroup(const node_handle& group)
  {
    m_pimpl->ungroup(group);
  }

  void editor_context::set_data(
    const socket_handle& socket,
    const object_ptr<Object>& data)
  {
    m_pimpl->set_data(socket, data);
  }

  auto editor_context::get_editor_info(const node_handle& handle) const
    -> std::optional<editor_node_info>
  {
    return m_pimpl->get_info(handle);
  }

  auto editor_context::get_editor_info(const socket_handle& handle) const
    -> std::optional<editor_socket_info>
  {
    return m_pimpl->get_info(handle);
  }

  auto editor_context::get_editor_info(const connection_handle& handle) const
    -> std::optional<editor_connection_info>
  {
    return m_pimpl->get_info(handle);
  }

  auto editor_context::get_group() const -> node_handle
  {
    return m_pimpl->get_group();
  }

  void editor_context::set_group(const node_handle& node)
  {
    return m_pimpl->set_group(node);
  }

  auto editor_context::get_pos(const node_handle& node) const
    -> std::optional<tvec2<float>>
  {
    return m_pimpl->get_pos(node);
  }

  void editor_context::set_pos(
    const node_handle& node,
    const tvec2<float>& new_pos)
  {
    m_pimpl->set_pos(node, new_pos);
  }

  auto editor_context::get_scroll() const -> tvec2<float>
  {
    return m_pimpl->get_scroll();
  }

  void editor_context::set_scroll(const tvec2<float>& new_pos)
  {
    m_pimpl->set_scroll(new_pos);
  }

  void editor_context::set_hovered(const node_handle& node)
  {
    m_pimpl->set_hovered(node);
  }

  void editor_context::set_hovered(const socket_handle& socket)
  {
    m_pimpl->set_hovered(socket);
  }

  void editor_context::set_hovered(const connection_handle& connection)
  {
    m_pimpl->set_hovered(connection);
  }

  void editor_context::clear_hovered_node()
  {
    m_pimpl->clear_hovered<node_handle>();
  }

  void editor_context::clear_hovered_socket()
  {
    m_pimpl->clear_hovered<socket_handle>();
  }

  void editor_context::clear_hovered_connection()
  {
    m_pimpl->clear_hovered<connection_handle>();
  }

  void editor_context::clear_hovered()
  {
    clear_hovered_node();
    clear_hovered_socket();
    clear_hovered_connection();
  }

  bool editor_context::is_node_hovered() const
  {
    return m_pimpl->is_hovered<node_handle>();
  }

  bool editor_context::is_socket_hovered() const
  {
    return m_pimpl->is_hovered<socket_handle>();
  }

  bool editor_context::is_connection_hovered() const
  {
    return m_pimpl->is_hovered<connection_handle>();
  }

  bool editor_context::is_hovered(const node_handle& node) const
  {
    return m_pimpl->is_hovered(node);
  }

  bool editor_context::is_hovered(const socket_handle& socket) const
  {
    return m_pimpl->is_hovered(socket);
  }

  bool editor_context::is_hovered(const connection_handle& connection) const
  {
    return m_pimpl->is_hovered(connection);
  }

  void editor_context::add_selected(const node_handle& node)
  {
    m_pimpl->add_selected(node);
  }

  void editor_context::add_selected(const socket_handle& socket)
  {
    m_pimpl->add_selected(socket);
  }

  void editor_context::add_selected(const connection_handle& connection)
  {
    m_pimpl->add_selected(connection);
  }

  void editor_context::remove_selected(const node_handle& node)
  {
    m_pimpl->remove_selected(node);
  }

  void editor_context::remove_selected(const socket_handle& socket)
  {
    m_pimpl->remove_selected(socket);
  }

  void editor_context::remove_selected(const connection_handle& connection)
  {
    m_pimpl->remove_selected(connection);
  }

  void editor_context::clear_selected_nodes()
  {
    m_pimpl->clear_selected<node_handle>();
  }

  void editor_context::clear_selected_sockets()
  {
    m_pimpl->clear_selected<socket_handle>();
  }

  void editor_context::clear_selected_connections()
  {
    m_pimpl->clear_selected<connection_handle>();
  }

  void editor_context::clear_selected()
  {
    clear_selected_nodes();
    clear_selected_sockets();
    clear_selected_connections();
  }

  bool editor_context::is_node_selected() const
  {
    return m_pimpl->is_selected<node_handle>();
  }

  bool editor_context::is_socket_selected() const
  {
    return m_pimpl->is_selected<socket_handle>();
  }

  bool editor_context::is_connection_selected() const
  {
    return m_pimpl->is_selected<connection_handle>();
  }

  bool editor_context::is_selected(const node_handle& node) const
  {
    return m_pimpl->is_selected(node);
  }

  bool editor_context::is_selected(const socket_handle& socket) const
  {
    return m_pimpl->is_selected(socket);
  }

  bool editor_context::is_selected(const connection_handle& connection) const
  {
    return m_pimpl->is_selected(connection);
  }

  auto editor_context::get_selected_nodes() const -> std::vector<node_handle>
  {
    return m_pimpl->get_selected<node_handle>();
  }

  auto editor_context::get_selected_sockets() const
    -> std::vector<socket_handle>
  {
    return m_pimpl->get_selected<socket_handle>();
  }

  auto editor_context::get_selected_connections() const
    -> std::vector<connection_handle>
  {
    return m_pimpl->get_selected<connection_handle>();
  }

  auto editor_context::get_hovered_node() const -> node_handle
  {
    return m_pimpl->get_hovered<node_handle>();
  }

  auto editor_context::get_hovered_socket() const -> socket_handle
  {
    return m_pimpl->get_hovered<socket_handle>();
  }

  auto editor_context::get_hovered_connection() const -> connection_handle
  {
    return m_pimpl->get_hovered<connection_handle>();
  }

  auto editor_context::get_state() const -> editor_state
  {
    return m_pimpl->get_state();
  }

  auto editor_context::get_drag_source_pos() const -> tvec2<float>
  {
    return m_pimpl->get_drag_source_pos();
  }

  void editor_context::begin_background_drag(const tvec2<float>& pos)
  {
    return m_pimpl->begin_background_drag(pos);
  }

  void editor_context::begin_node_drag(const tvec2<float>& pos)
  {
    return m_pimpl->begin_node_drag(pos);
  }

  void editor_context::begin_socket_drag(const tvec2<float>& pos)
  {
    return m_pimpl->begin_socket_drag(pos);
  }

  void editor_context::end_background_drag()
  {
    return m_pimpl->end_background_drag();
  }

  void editor_context::end_node_drag()
  {
    return m_pimpl->end_node_drag();
  }

  void editor_context::end_socket_drag()
  {
    return m_pimpl->end_socket_drag();
  }
} // namespace yave::app