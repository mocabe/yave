//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/scene_graph.hpp>

#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/decl/frame_time.hpp>
#include <yave/node/decl/frame_buffer.hpp>
#include <yave/node/decl/image_output.hpp>
#include <yave/node/decl/compositor.hpp>
#include <yave/node/decl/blend_op.hpp>
#include <yave/node/decl/control_flow.hpp>
#include <yave/node/decl/keyframe.hpp>
#include <yave/support/log.hpp>
#include <yave/support/id.hpp>

YAVE_DECL_G_LOGGER(scene_graph)

namespace yave {

  /// layer_attribute manages all per-layer resources, internal connections
  /// between private resources, and inter-layer connections of sublayers.
  /// There're two types of layer; image layer and composition layer. Image
  /// layer uses it's image output for rendering content on the layer.
  /// Composition layer is used for blending all sublayers and managing
  /// inheritable resources for sublayer nodes. All layers which have
  /// sublayer(s) are composition layer, otherwise it's image layer. Each layer
  /// has it's own layer compositor and blending function to represent how it
  /// will be blended with other layers.
  struct scene_graph::layer_attribute
  {
    layer_attribute(
      layer_type type,
      const layer_handle& layer,
      layered_node_graph& graph,
      const std::map<layer_handle, layer_attribute>& map)
      : m_layer {layer}
      , m_graph {graph}
      , m_map {map}
      , m_layer_type {type}
      , m_blend_op {blend_operation::over} // default blend op
      , m_blend_func_info {get_blend_op_getter_node_info(m_blend_op)}
    {
      Info(
        g_logger,
        "Initializing layer attribute for layer {}",
        to_string(m_layer.id()));

      // layer is visible by default
      m_is_visible = true;

      /* init resources */
      {
        auto info      = get_node_info<node::LayerImageOutput>();
        m_image_output = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_image_output)
          throw std::runtime_error("Failed to create layer image output");
      }
      {
        auto info       = get_node_info<node::KeyframeBool>();
        m_visibility_kv = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_visibility_kv)
          throw std::runtime_error("Failed to create layer visibility kv");
      }
      {
        auto info  = get_node_info<node::FrameBuffer>();
        m_empty_fb = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_empty_fb)
          throw std::runtime_error("Failed to create empty frame buffer");
      }
      {
        auto info  = get_node_info<node::If>();
        m_blend_if = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_if)
          throw std::runtime_error("Failed to create blend if");
      }
      {
        auto info    = get_blend_op_getter_node_info(m_blend_op);
        m_blend_func = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_func)
          throw std::runtime_error("Failed to create blend func");
      }
      {
        auto info   = get_node_info<node::BlendOpSrcGetter>();
        m_blend_dst = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_dst)
          throw std::runtime_error("Failed to create blend dst");
      }
      {
        auto info    = get_node_info<node::LayerCompositor>();
        m_compositor = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_compositor)
          throw std::runtime_error("Failed to create compositor");
      }
      {
        auto info = get_node_info<node::FrameTime>();
        m_frame   = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_frame)
          throw std::runtime_error("Failed to create frame");
      }

      /* build connections */

      // image output [out] -> compositor[src]
      {
        auto io       = get_node_info<node::LayerImageOutput>();
        auto compos   = get_node_info<node::LayerCompositor>();
        m_c_io_compos = m_graph.connect(
          m_image_output.get(),
          io.output_sockets()[0],
          m_compositor.get(),
          compos.input_sockets()[0]);
        if (!m_c_io_compos)
          throw std::runtime_error("Failed to connect nodes");
      }

      // if -> compos [func]
      {
        auto bif      = get_node_info<node::If>();
        auto compos   = get_node_info<node::LayerCompositor>();
        m_c_if_compos = m_graph.connect(
          m_blend_if.get(),
          bif.output_sockets()[0],
          m_compositor.get(),
          compos.input_sockets()[2]);
        if (!m_c_if_compos)
          throw std::runtime_error("Failed to connect nodes");
      }

      // visibility -> if [cond]
      {
        auto vis          = get_node_info<node::KeyframeBool>();
        auto bif          = get_node_info<node::If>();
        m_c_visibility_if = m_graph.connect(
          m_visibility_kv.get(),
          vis.output_sockets()[0],
          m_blend_if.get(),
          bif.input_sockets()[0]);
        if (!m_c_visibility_if)
          throw std::runtime_error("Failed to connect nodes");
      }

      //  func -> if [then]
      {
        auto bif    = get_node_info<node::If>();
        m_c_func_if = m_graph.connect(
          m_blend_func.get(),
          m_blend_func_info.output_sockets()[0],
          m_blend_if.get(),
          bif.input_sockets()[1]);
        if (!m_c_func_if)
          throw std::runtime_error("Failed to connect nodes");
      }

      // blend dst -> if [else]
      {
        auto dst   = get_node_info<node::BlendOpDstGetter>();
        auto bif   = get_node_info<node::If>();
        m_c_dst_if = m_graph.connect(
          m_blend_dst.get(),
          dst.output_sockets()[0],
          m_blend_if.get(),
          bif.input_sockets()[2]);
        if (!m_c_dst_if)
          throw std::runtime_error("Failed to connect nodes");
      }

      // frame -> visibility
      {
        auto vis             = get_node_info<node::KeyframeBool>();
        auto fb              = get_node_info<node::FrameTime>();
        m_c_frame_visibility = m_graph.connect(
          m_frame.get(),
          fb.output_sockets()[0],
          m_visibility_kv.get(),
          vis.input_sockets()[0]);

        if (!m_c_frame_visibility)
          throw std::runtime_error("Failed to connect nodes");
      }

      // image layer: connect empty fb to image out
      if (is_image_layer()) {
        assert(m_c_sublayers.empty());

        auto io = get_node_info<node::LayerImageOutput>();
        auto fb = get_node_info<node::FrameBuffer>();

        auto c = m_graph.connect(
          m_empty_fb.get(),
          fb.output_sockets()[0],
          m_image_output.get(),
          io.input_sockets()[0]);

        if (!c)
          throw std::runtime_error("Failed to initialize empty image layer");

        m_c_resources.push_back(c);
      }

      // compos layer: sublayer connection
      if (is_compos_layer()) {
        rebuild_sublayer_connections();
      }

      // set visible resources
      {
        // register image layer
        if (is_image_layer())
          m_resources.push_back(m_image_output);
      }
    }

    ~layer_attribute()
    {
      Info(
        g_logger,
        "Destroying layer attribute of layer {} ",
        to_string(m_layer.id()));

      // destroy connections
      {
        m_graph.disconnect(m_c_io_compos);
        m_graph.disconnect(m_c_if_compos);
        m_graph.disconnect(m_c_dst_compos);
        m_graph.disconnect(m_c_visibility_if);
        m_graph.disconnect(m_c_func_if);
        m_graph.disconnect(m_c_dst_if);

        // disconnect sublayers
        for (auto&& c : m_c_sublayers) {
          m_graph.disconnect(c);
        }
        // disconnect per-layer resource connections
        for (auto&& c : m_c_resources) {
          m_graph.disconnect(c);
        }
      }

      // No need to remove shared resources
    }

    void rebuild_sublayer_connections()
    {
      Info(
        g_logger,
        "Rebuilding sublayer connections at '{}'#{}",
        *m_graph.get_layer_name(m_layer),
        to_string(m_layer.id()));

      if (is_image_layer()) {
        Warning(
          g_logger, "connect_sublayers() called for image layer. Ignored.");
        return;
      }

      if (is_compos_layer()) {

        // disconnect existing connections

        for (auto&& c : m_c_sublayers) {
          m_graph.disconnect(c);
        }
        m_c_sublayers.clear();

        // connect sublayers

        auto compos    = get_node_info<node::LayerCompositor>();
        auto sublayers = m_graph.get_sublayers(m_layer);

        if (sublayers.empty()) {

          auto fb = get_node_info<node::FrameBuffer>();
          auto io = get_node_info<node::LayerImageOutput>();

          auto c = m_graph.connect(
            m_empty_fb.get(),
            fb.output_sockets()[0],
            m_image_output.get(),
            io.input_sockets()[0]);

          if (!c)
            throw std::runtime_error(
              "Could not connect empty fb to io in empty layer");

          m_c_sublayers.push_back(c);

        } else {

          const layer_attribute* prev;

          // connect first layer

          {
            auto iter = m_map.find(sublayers[0]);
            if (iter != m_map.end())
              prev = &(iter->second);
            else
              throw std::runtime_error("Could not find layer attribute");

            auto fb = get_node_info<node::FrameBuffer>();

            // connect empty as first sublayer dst
            auto c = m_graph.connect(
              prev->m_empty_fb.get(),
              fb.output_sockets()[0],
              prev->m_compositor.get(),
              compos.input_sockets()[1]);

            if (!c)
              throw std::runtime_error(
                "Failed to connect empty fb to lowest layer");

            m_c_sublayers.push_back(c);
          }

          // connect each sublayers

          for (size_t i = 1; i < sublayers.size(); ++i) {
            auto iter = m_map.find(sublayers[i]);
            if (iter != m_map.end()) {
              // prev compos output -> compositor dst
              auto c = m_graph.connect(
                prev->m_compositor.get(),
                compos.output_sockets()[0],
                iter->second.m_compositor.get(),
                compos.input_sockets()[1]);

              if (!c)
                throw std::runtime_error("Failed to connect sublayers");

              m_c_sublayers.push_back(c);

              // update prev
              prev = &(iter->second);
            }
          }

          // connect top layer output to image output of this layer
          {
            auto io = get_node_info<node::LayerImageOutput>();

            // compos out -> image out
            auto c = m_graph.connect(
              prev->m_compositor.get(),
              compos.output_sockets()[0],
              m_image_output.get(),
              io.input_sockets()[0]);

            if (!c)
              throw std::runtime_error(
                "Failed to connect result of sublayer composition to image "
                "output");

            m_c_sublayers.push_back(c);
          }
        }
      }
    }

    void set_blend_op(blend_operation op)
    {
      Info("Set new blend operation to layer {}", to_string(m_layer.id()));

      if (m_blend_op == op)
        return;

      m_graph.remove_resource(m_blend_func.get());
      m_blend_func = nullptr;

      // rebuild blend func
      {
        m_blend_func_info = get_blend_op_getter_node_info(op);
        m_blend_func      = m_graph.add_resource_shared(
          m_blend_func_info.name(), m_layer, layer_resource_scope::Private);

        if (!m_blend_func)
          throw std::runtime_error("Failed to create blend func");
      }

      // rebuild connections
      {
        auto bif    = get_node_info<node::If>();
        m_c_func_if = m_graph.connect(
          m_blend_func.get(),
          m_blend_func_info.output_sockets()[0],
          m_blend_if.get(),
          bif.input_sockets()[0]);

        if (!m_c_func_if)
          throw std::runtime_error("Failed to connect blend func");
      }
    }

    bool is_compos_layer() const
    {
      return m_layer_type == layer_type::compos;
    }

    bool is_image_layer() const
    {
      if (m_layer_type == layer_type::image) {
        assert(m_graph.get_sublayers(m_layer).empty());
        return true;
      }
      return false;
    }

    std::vector<layer_resource_handle> get_resources() const
    {
      std::vector<layer_resource_handle> ret;
      for (auto&& sr : m_resources) {
        ret.push_back(sr.get());
      }
      return ret;
    }

    std::vector<layer_resource_handle> get_references() const
    {
      std::vector<layer_resource_handle> ret;
      for (auto&& sr : m_references) {
        ret.push_back(sr.get());
      }
      return ret;
    }

    layer_resource_handle add_resource(
      const std::string& name,
      layer_resource_scope scope)
    {
      auto res = m_graph.add_resource_shared(name, m_layer, scope);
      if (res) {
        m_resources.push_back(res);
        return res.get();
      }
      return nullptr;
    }

    bool add_reference(const layer_resource_handle& handle)
    {
      auto inherited = m_graph.get_inherited_resources(m_layer);

      // check scope
      if (
        std::find(inherited.begin(), inherited.end(), handle) ==
        inherited.end()) {

        Info(
          g_logger,
          "Tried to add reference to layer resource which is not accesible "
          "from this layer {}. Ignored.",
          to_string(m_layer.id()));

        return false;
      }

      // find shared resource and add to reference
      if (auto layer = m_graph.get_layer(handle)) {
        auto find = m_map.find(layer);
        if (find == m_map.end())
          return false;

        for (auto&& sr : find->second.m_resources) {
          if (sr.get() == handle) {
            m_references.push_back(sr);

            Info(
              g_logger,
              "Added new reference to the resource {} which belongs to the "
              "layer {}",
              to_string(handle.id()),
              to_string(layer.id()));

            return true;
          }
        }
      }

      Warning(
        g_logger, "Could not add resource reference: Resource not found.");

      return false;
    }

    bool is_visible() const
    {
      return m_is_visible;
    }

    void set_visibility(bool visibility)
    {
      if (m_is_visible == visibility)
        return;

      auto dst_info = get_node_info<node::LayerCompositor>();

      if (visibility) {
        assert(m_graph.exists(m_c_dst_compos));
        assert(!m_graph.exists(m_c_if_compos));
        m_graph.disconnect(m_c_dst_compos);
        auto src_info = get_node_info<node::If>();

        m_c_if_compos = m_graph.connect(
          m_blend_if.get(),
          src_info.output_sockets()[0],
          m_compositor.get(),
          dst_info.input_sockets()[2]);

        if (!m_c_if_compos)
          throw std::runtime_error("Failed to change layer visibility");

      } else {
        assert(!m_graph.exists(m_c_dst_compos));
        assert(m_graph.exists(m_c_if_compos));
        m_graph.disconnect(m_c_if_compos);
        auto src_info = get_node_info<node::BlendOpDstGetter>();

        m_c_dst_compos = m_graph.connect(
          m_blend_dst.get(),
          src_info.output_sockets()[0],
          m_compositor.get(),
          dst_info.input_sockets()[2]);

        if (!m_c_dst_compos)
          throw std::runtime_error("Failed to change layer visibility");
      }

      m_is_visible = visibility;
    }

    layer_resource_handle get_compositor() const
    {
      return m_compositor.get();
    }

  private:
    /// layer handle (key)
    layer_handle m_layer;
    /// reference for node graph
    layered_node_graph& m_graph;
    /// reference for attribute map
    const std::map<layer_handle, layer_attribute>& m_map;

  private:
    /// composition layer?
    layer_type m_layer_type;
    /// layer visibility
    bool m_is_visible;
    /// blend operation
    blend_operation m_blend_op;

  private:
    /*
      Per-layer resources:
                                (dst: empty)
    [frame] -> [visibility] -> |             |
               [func] -------> | if| -> [compositor] ---> (out: empty)
               [dst] --------> |             |
                               [layer image output]
    */
    shared_layer_resource_handle m_image_output;  ///< image output node
    shared_layer_resource_handle m_visibility_kv; ///< visibiilty cond
    shared_layer_resource_handle m_empty_fb;      ///< empty frame buffer
    shared_layer_resource_handle m_blend_if;      ///< if node for blend func
    shared_layer_resource_handle m_blend_dst;     ///< blend function (ab-dst)
    shared_layer_resource_handle m_compositor;    ///< compositor node
    shared_layer_resource_handle m_blend_func;    ///< current blend function
    shared_layer_resource_handle m_frame;         ///< frame
    node_info m_blend_func_info;                  ///< current blend func info

  private:
    connection_handle m_c_io_compos;
    connection_handle m_c_if_compos;  // is_visible() == true
    connection_handle m_c_dst_compos; // is_visibel() == false
    connection_handle m_c_visibility_if;
    connection_handle m_c_func_if;
    connection_handle m_c_dst_if;
    connection_handle m_c_frame_visibility;
    std::vector<connection_handle> m_c_sublayers;
    std::vector<connection_handle> m_c_resources;

  private:
    /// list of owning resources which is visible from user side
    std::vector<shared_layer_resource_handle> m_resources;
    /// resource references (list of inherited resources currently used)
    std::vector<shared_layer_resource_handle> m_references;
  };

  scene_graph::scene_graph()
  {
    init_logger();

    // register default node info
    {
      // primitive
      if (!m_graph.register_node_info(get_primitive_node_info_list()))
        throw std::runtime_error("Failed to register primitive node info");

      // frame
      if (!m_graph.register_node_info(get_node_info<node::FrameTime>()))
        throw std::runtime_error("Failed to register frame ctor info");

      // layer io
      if (!m_graph.register_node_info(get_node_info<node::LayerImageOutput>()))
        throw std::runtime_error("Failed to register image io info");

      // layer fb
      if (!m_graph.register_node_info(get_node_info<node::FrameBuffer>()))
        throw std::runtime_error("Failed to register frame buffer info");

      // layer compositor
      if (!m_graph.register_node_info(get_node_info<node::LayerCompositor>()))
        throw std::runtime_error("Failed to register compositor info");

      // blend ops
      if (!m_graph.register_node_info(get_blend_op_node_info_list()))
        throw std::runtime_error("Failed to register blend op info");
      if (!m_graph.register_node_info(get_blend_op_getter_node_info_list()))
        throw std::runtime_error("Failed to register blend op info");

      // keyframes
      if (!m_graph.register_node_info(get_keyframe_node_info_list()))
        throw std::runtime_error("Failed to register keyframe info");

      // control flow
      if (!m_graph.register_node_info(get_control_flow_node_info_list()))
        throw std::runtime_error("Failed to register control flow node info");
    }

    // Add global image output (private)
    {
      auto info = get_node_info<node::LayerImageOutput>();
      auto res  = m_graph.add_resource(
        info.name(), root(), layer_resource_scope::Private);

      if (!res)
        throw std::runtime_error("Failed to create layer output node");

      m_graph.set_resource_name(res, "Global Image Output");
      m_image_output = res;
    }

    // Add global audio output (private)
    {
      // TODO
      m_audio_output = nullptr;
    }

    // Add empty frame buffer (inherit)
    {
      auto info = get_node_info<node::FrameBuffer>();
      auto res  = m_graph.add_resource(
        info.name(), root(), layer_resource_scope::Inherit);

      if (!res)
        throw std::runtime_error("Failed to create frame buffer");

      m_graph.set_resource_name(res, "Empty Frame Buffer");
      m_empty_frame_buffer = res;
    }

    assert(m_graph.layers().size() == 1);

    // Add layer info as empty image layer
    auto root_attr = _add_layer_attribute(root(), layer_type::compos);

    // connect root layer to global out
    {
      Info(g_logger, "Building connections for root layer");
      auto compos_info = get_node_info<node::LayerCompositor>();
      auto io_info     = get_node_info<node::LayerImageOutput>();
      auto fb_info     = get_node_info<node::FrameBuffer>();

      m_c_fb_root = m_graph.connect(
        m_empty_frame_buffer,
        fb_info.output_sockets()[0],
        root_attr->get_compositor(),
        compos_info.input_sockets()[1]);

      m_c_root_io = m_graph.connect(
        root_attr->get_compositor(),
        compos_info.output_sockets()[0],
        m_image_output,
        io_info.input_sockets()[0]);

      if (!m_c_root_io || !m_c_fb_root)
        throw std::runtime_error("Failed to connect root compositor");
    }
  }

  scene_graph::~scene_graph() noexcept
  {
    // don't actually need this, just for consistency.
    m_graph.disconnect(m_c_fb_root);
    m_graph.disconnect(m_c_root_io);
  }

  scene_graph::scene_graph(scene_graph&& other) noexcept
  {
    auto lck           = _lock();
    m_graph            = std::move(other.m_graph);
    m_layer_attributes = std::move(other.m_layer_attributes);
  }

  scene_graph& scene_graph::operator=(scene_graph&& other) noexcept
  {
    auto lck1          = _lock();
    auto lck2          = other._lock();
    m_graph            = std::move(other.m_graph);
    m_layer_attributes = std::move(other.m_layer_attributes);
    return *this;
  }

  // Use when modifying additional information for m_graph
  auto scene_graph::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock {m_mtx};
  }

  // Add new layer info to layer info list
  auto scene_graph::_add_layer_attribute(
    const layer_handle& layer,
    layer_type type) -> layer_attribute*
  {
    // insert layer info
    auto [iter, b] = m_layer_attributes.try_emplace(
      layer, type, layer, m_graph, m_layer_attributes);

    if (!b) {
      Error(
        g_logger,
        "Failed to add new scene layer info: id={}",
        to_string(layer.id()));

      throw std::runtime_error("Failed to add layer info");
    }

    return &iter->second;
  }

  auto scene_graph::_get_layer_attribute(const layer_handle& layer) const
    -> const layer_attribute*
  {
    auto iter = m_layer_attributes.find(layer);

    if (iter == m_layer_attributes.end())
      return nullptr;

    return &iter->second;
  }

  auto scene_graph::_get_layer_attribute(const layer_handle& layer)
    -> layer_attribute*
  {
    auto iter = m_layer_attributes.find(layer);

    if (iter == m_layer_attributes.end())
      return nullptr;

    return &iter->second;
  }

  auto scene_graph::root() const -> layer_handle
  {
    auto lck = _lock();
    return m_graph.root();
  }

  bool scene_graph::exists(const layer_handle& layer) const
  {
    auto lck = _lock();
    return m_graph.exists(layer);
  }

  bool scene_graph::exists(const layer_resource_handle& node) const
  {
    auto lck = _lock();
    return m_graph.exists(node);
  }

  bool scene_graph::exists(
    const layer_resource_handle& node,
    const layer_handle& layer) const
  {
    auto lck = _lock();
    return m_graph.exists(node, layer);
  }

  auto scene_graph::_add_layer(const layer_handle& layer, layer_type type)
    -> layer_handle
  {
    auto l = m_graph.add_layer(layer);

    if (!l)
      throw std::runtime_error("Failed to add layer");

    assert(l == m_graph.get_sublayers(layer).back());

    // add layer attribute
    {
      auto attr = _add_layer_attribute(l, type);
      if (!attr)
        throw std::runtime_error("Failed to add layer attribute");
    }

    // rebuild sublayers of parent
    {
      if (auto attr = _get_layer_attribute(layer))
        attr->rebuild_sublayer_connections();
    }

    return l;
  }

  auto scene_graph::add_layer(const layer_handle& layer, layer_type type)
    -> layer_handle
  {
    auto lck = _lock();

    if (auto attr = _get_layer_attribute(layer)) {
      // only compos layer can have sublayers
      if (attr->is_compos_layer())
        return _add_layer(layer, type);
    }

    return nullptr;
  }

  void scene_graph::_remove_layer(const layer_handle& layer)
  {
    if (!m_graph.exists(layer)) {
      Warning(
        g_logger,
        "remove_layer() is called for invalid layer handle. Ignored.");
      return;
    }

    // recursively remove layers
    for (auto&& sub : m_graph.get_sublayers(layer)) {
      _remove_layer(sub);
    }

    auto itr = m_layer_attributes.find(layer);

    if (itr == m_layer_attributes.end()) {
      Error(g_logger, "Could not find layer attribute for exising layer");
      return;
    }

    // remove attribute
    m_layer_attributes.erase(itr);

    auto parent = m_graph.get_parent(layer);
    m_graph.remove_layer(layer);

    // rebuild sublayers of parent
    if (auto attr = _get_layer_attribute(parent))
      attr->rebuild_sublayer_connections();
  }

  void scene_graph::remove_layer(const layer_handle& layer)
  {
    auto lck = _lock();
    _remove_layer(layer);
  }

  bool scene_graph::movable_into(
    const layer_handle& from,
    const layer_handle& to) const
  {
    auto lck = _lock();

    // Move image layer into different compos layer, or move compos layer and
    // it's child layers into different compos layer.
    if (m_graph.movable_into(from, to)) {
      if (auto attr = _get_layer_attribute(to))
        return attr->is_compos_layer();
    }
    return false;
  }

  void scene_graph::move_into(const layer_handle& from, const layer_handle& to)
  {
    auto lck = _lock();

    auto p = m_graph.get_parent(from);

    // Move layer.
    m_graph.move_into(from, to);

    // Rebuild sublayer connections.
    if (auto attr = _get_layer_attribute(p))
      attr->rebuild_sublayer_connections();
    if (auto attr = _get_layer_attribute(to))
      attr->rebuild_sublayer_connections();
  }

  bool scene_graph::movable_below(
    const layer_handle& from,
    const layer_handle& to) const
  {
    auto lck = _lock();
    return m_graph.movable_below(from, to);
  }

  void scene_graph::move_below(const layer_handle& from, const layer_handle& to)
  {
    auto lck = _lock();

    auto p1 = m_graph.get_parent(from);
    auto p2 = m_graph.get_parent(to);

    m_graph.move_below(from, to);

    // rebuild sublayer connections
    if (auto attr = _get_layer_attribute(p1))
      attr->rebuild_sublayer_connections();
    if (auto attr = _get_layer_attribute(p2))
      attr->rebuild_sublayer_connections();
  }

  bool scene_graph::movable_above(
    const layer_handle& from,
    const layer_handle& to) const
  {
    auto lck = _lock();
    return m_graph.movable_above(from, to);
  }

  void scene_graph::move_above(const layer_handle& from, const layer_handle& to)
  {
    auto lck = _lock();

    auto p1 = m_graph.get_parent(from);
    auto p2 = m_graph.get_parent(to);

    m_graph.move_above(from, to);

    if (auto attr = _get_layer_attribute(p1))
      attr->rebuild_sublayer_connections();
    if (auto attr = _get_layer_attribute(p2))
      attr->rebuild_sublayer_connections();
  }

  bool scene_graph::is_compos_layer(const layer_handle& layer) const
  {
    auto lck = _lock();

    if (auto attr = _get_layer_attribute(layer))
      return attr->is_compos_layer();

    Warning(
      g_logger,
      "is_compos_layer() called for nonexisting layer. Returning false.");

    return false;
  }

  bool scene_graph::is_image_layer(const layer_handle& layer) const
  {
    auto lck = _lock();

    if (auto attr = _get_layer_attribute(layer))
      return attr->is_image_layer();

    Warning(
      g_logger,
      "is_image_layer() called for nonexising layer. Returning false.");

    return false;
  }

  auto scene_graph::get_name(const layer_handle& layer) const
    -> std::optional<std::string>
  {
    auto lck = _lock();
    return m_graph.get_layer_name(layer);
  }

  void scene_graph::set_name(const layer_handle& layer, const std::string& name)
  {
    auto lck = _lock();
    return m_graph.set_layer_name(layer, name);
  }

  bool scene_graph::is_visible(const layer_handle& layer) const
  {
    auto lck = _lock();
    if (auto attr = _get_layer_attribute(layer)) {
      return attr->is_visible();
    }
    return false;
  }

  void scene_graph::set_visibility(const layer_handle& layer, bool visibility)
  {
    auto lck = _lock();
    if (auto attr = _get_layer_attribute(layer)) {
      attr->set_visibility(visibility);
    }
  }

  auto scene_graph::get_parent(const layer_handle& layer) const -> layer_handle
  {
    auto lck = _lock();
    return m_graph.get_parent(layer);
  }

  auto scene_graph::get_sublayers(const layer_handle& layer) const
    -> std::vector<layer_handle>
  {
    auto lck = _lock();
    return m_graph.get_sublayers(layer);
  }

  auto scene_graph::get_resources(const layer_handle& layer) const
    -> std::vector<layer_resource_handle>
  {
    auto lck = _lock();
    if (auto attr = _get_layer_attribute(layer)) {
      auto res = attr->get_resources();
      auto ref = attr->get_references();
      res.insert(res.end(), ref.begin(), ref.end());
      return res;
    }
    return {};
  }

  auto scene_graph::get_resources_owning(const layer_handle& layer) const
    -> std::vector<layer_resource_handle>
  {
    auto lck = _lock();
    if (auto attr = _get_layer_attribute(layer)) {
      return attr->get_resources();
    }
    return {};
  }

  auto scene_graph::get_resources_reference(const layer_handle& layer) const
    -> std::vector<layer_resource_handle>
  {
    auto lck = _lock();
    if (auto attr = _get_layer_attribute(layer)) {
      return attr->get_references();
    }
    return {};
  }

  auto scene_graph::get_node_graph() const -> node_graph_output
  {
    auto lck = _lock();

    auto graph_copy = m_graph.get_managed_node_graph().get_node_graph();

    node_handle new_root;

    for (auto&& n : graph_copy.nodes()) {
      if (n.id() == m_image_output.id()) {
        new_root = n;
        break;
      }
    }

    if (!new_root)
      throw std::runtime_error("Could not find root node in copied node_graph");

    return {std::move(graph_copy), new_root};
  }

} // namespace yave