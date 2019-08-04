//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/scene_graph.hpp>

#include <yave/node/obj/function.hpp>
#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/frame.hpp>
#include <yave/node/obj/frame_buffer.hpp>
#include <yave/node/obj/image_output.hpp>
#include <yave/node/obj/compositor.hpp>
#include <yave/node/obj/blend_op.hpp>
#include <yave/node/obj/control_flow.hpp>
#include <yave/node/obj/keyframe.hpp>
#include <yave/support/log.hpp>
#include <yave/support/id.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("scene_graph");
      return 1;
    }();
  }
} // namespace

namespace yave {

  /// layer_attribute manages all per-layer resources, internal connections
  /// between private resources, and inter-layer connections of sublayers.
  /// There're two types of layer; image layer and composition layer. Image
  /// layer uses it's image output for rendering content on the layer.
  /// Composition layer is used for blending all sublayers and inheritable
  /// resources for sublayer nodes. All layers which have sublayer(s) are
  /// composition layer, otherwise it's image layer.
  /// Each layer has it's own layer compositor and blending function to
  /// represent how it will be blended with other layers.
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
      , m_blend_func_info {get_blend_op_node_info(blend_operation::over)}
    {
      Info("Initializing layer attribute for layer {}", m_layer.id().data);

      // layer is visible by default
      m_is_visible = true;

      // layer blending is alpha-blending by default
      m_blend_op = blend_operation::over;

      /* init resources */
      {
        auto info      = get_node_info<LayerImageOutput>();
        m_image_output = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_image_output)
          throw std::runtime_error("Failed to create layer image output");
      }
      {
        auto info       = get_node_info<KeyFrame<Bool>>();
        m_visibility_kv = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_visibility_kv)
          throw std::runtime_error("Failed to create layer visibility kv");
      }
      {
        auto info  = get_node_info<FrameBufferConstructor>();
        m_empty_fb = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_empty_fb)
          throw std::runtime_error("Failed to create empty frame buffer");
      }
      {
        auto info  = get_node_info<IfNode>();
        m_blend_if = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_if)
          throw std::runtime_error("Failed to create blend if");
      }
      {
        auto info    = get_blend_op_node_info(m_blend_op);
        m_blend_func = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_func)
          throw std::runtime_error("Failed to create blend func");
      }
      {
        auto info   = get_node_info<BlendOpDst>();
        m_blend_dst = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_dst)
          throw std::runtime_error("Failed to create blend dst");
      }
      {
        auto info    = get_node_info<LayerCompositor>();
        m_compositor = m_graph.add_resource_shared(
          info.name(), m_layer, layer_resource_scope::Private);
        if (!m_compositor)
          throw std::runtime_error("Failed to create compositor");
      }
      {
        m_blend_func_info = get_blend_op_node_info(m_blend_op);
        m_blend_func      = m_graph.add_resource_shared(
          m_blend_func_info.name(), m_layer, layer_resource_scope::Private);
        if (!m_blend_func)
          throw std::runtime_error("Failed to create blend func");
      }

      /* build connections */

      // image output [out] -> compositor[src]
      {
        auto io       = get_node_info<LayerImageOutput>();
        auto compos   = get_node_info<LayerCompositor>();
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
        auto bif      = get_node_info<IfNode>();
        auto compos   = get_node_info<LayerCompositor>();
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
        auto vis          = get_node_info<KeyFrame<Bool>>();
        auto bif          = get_node_info<IfNode>();
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
        auto bif    = get_node_info<IfNode>();
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
        auto dst   = get_node_info<BlendOpDst>();
        auto bif   = get_node_info<IfNode>();
        m_c_dst_if = m_graph.connect(
          m_blend_dst.get(),
          dst.output_sockets()[0],
          m_blend_if.get(),
          bif.input_sockets()[2]);
        if (!m_c_dst_if)
          throw std::runtime_error("Failed to connect nodes");
      }

      // sublayer connection
      {
        connect_sublayers();
      }
    }

    ~layer_attribute()
  {
      Info(
        g_logger, "Destroying layer attribute of layer {} ", m_layer.id().data);

      // destroy connections
  {
        m_graph.disconnect(m_c_io_compos);
        m_graph.disconnect(m_c_if_compos);
        m_graph.disconnect(m_c_visibility_if);
        m_graph.disconnect(m_c_func_if);
        m_graph.disconnect(m_c_dst_if);
        for (auto&& c : m_c_sublayers) {
          m_graph.disconnect(c);
        }
      }

      // No need to remove shared resources
    }

    void connect_sublayers()
    {
      Info("Rebuilding sublayer connections...");

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

        auto compos    = get_node_info<LayerCompositor>();
        auto sublayers = m_graph.get_sublayers(m_layer);

        if (sublayers.empty()) {

          auto fb = get_node_info<FrameBufferConstructor>();
          auto io = get_node_info<LayerImageOutput>();

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

            auto fb = get_node_info<FrameBufferConstructor>();

            // connect empty as first sublayer dst
            auto c = m_graph.connect(
              prev->m_empty_fb.get(),
              fb.output_sockets()[0],
              prev->m_compositor.get(),
              compos.input_sockets()[0]);

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
            auto io     = get_node_info<LayerImageOutput>();

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
      Info("Set new blend operation to layer {}", m_layer.id().data);

      if (m_blend_op == op)
        return;

      m_graph.remove_resource(m_blend_func.get());
      m_blend_func = nullptr;

      // rebuild blend func
      {
        m_blend_func_info = get_blend_op_node_info(op);
        m_blend_func      = m_graph.add_resource_shared(
          m_blend_func_info.name(), m_layer, layer_resource_scope::Private);

        if (!m_blend_func)
          throw std::runtime_error("Failed to create blend func");
    }

      // rebuild connections
    {
        auto bif    = get_node_info<IfNode>();
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
      [visibility] --> |    |        |
      [func] --------> | if | -> [compositor] ---> (out: empty)
      [dst] ---------> |    |        |
                               [layer image output]
    */
    shared_layer_resource_handle m_image_output;  ///< image output node
    shared_layer_resource_handle m_visibility_kv; ///< visibiilty cond
    shared_layer_resource_handle m_empty_fb;      ///< empty frame buffer
    shared_layer_resource_handle m_blend_if;      ///< if node for blend func
    shared_layer_resource_handle m_blend_dst;     ///< blend function (ab-dst)
    shared_layer_resource_handle m_compositor;    ///< compositor node
    shared_layer_resource_handle m_blend_func;    ///< current blend function
    node_info m_blend_func_info;                  ///< current blend func info

  private:
    connection_handle m_c_io_compos;
    connection_handle m_c_if_compos;
    connection_handle m_c_visibility_if;
    connection_handle m_c_func_if;
    connection_handle m_c_dst_if;
    std::vector<connection_handle> m_c_sublayers;

  private:
    /// resource references (list of inherited resources currently used)
    std::vector<shared_layer_resource_handle> m_references;
  };

  scene_graph::scene_graph()
  {
    init_logger();
  }

  scene_graph::~scene_graph() noexcept
  {
  }

  scene_graph::scene_graph(scene_graph&& other) noexcept
  {
    auto lck     = _lock();
    m_graph      = std::move(other.m_graph);
    m_layer_info = std::move(other.m_layer_info);
  }

  scene_graph& scene_graph::operator=(scene_graph&& other) noexcept
  {
    auto lck1    = _lock();
    auto lck2    = other._lock();
    m_graph      = std::move(other.m_graph);
    m_layer_info = std::move(other.m_layer_info);
  }

  // Use when modifying additional information for m_graph
  auto scene_graph::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock {m_mtx};
  }

  auto scene_graph::root() const -> layer_handle
  {
    return m_graph.root();
  }

  bool scene_graph::exists(const layer_handle& layer) const
  {
    return m_graph.exists(layer);
  }

  bool scene_graph::exists(const layer_resource_handle& node) const
  {
    return m_graph.exists(node);
  }

  bool scene_graph::exists(
    const layer_resource_handle& node,
    const layer_handle& layer) const
  {
    return m_graph.exists(node, layer);
  }

  auto scene_graph::add_layer(const layer_handle& layer) -> layer_handle
  {
    auto lck = _lock();

    // build new layer
    auto l = m_graph.add_layer(layer);

    // add composition control

  }

  void scene_graph::remove_layer(const layer_handle& layer)
  {
  }
} // namespace yave