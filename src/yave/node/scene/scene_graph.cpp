
#include <yave/node/scene/scene_graph.hpp>

#include <yave/support/log.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_undued]] static auto init = [] {
      yave::add_logger("scene_graph");
      return 1;
    }();
  }
} // namespace

namespace yave {

  struct scene_layer_composition
  {
    layer_resource_handle compositor;
    layer_resource_handle sublayer_image_output;
  };

  struct scene_layer_info
  {
    scene_layer_info(layered_node_graph& g, const layer_handle& l)
      : m_graph {g}
      , m_layer {l}
    {
    }

    ~scene_layer_info()
    {
    }

    void set_visibility(bool visibility)
    {
      m_is_visible = true;
    }

    void make_custom()
    {
    }
  
  public:
    bool is_custom() const
    {
      return m_is_custom;
    }

    bool is_visible() const
    {
      return m_is_visible;
    }

  private:
    layered_node_graph& m_graph;
    layer_handle m_layer;
    bool m_is_custom  = false;
    bool m_is_visible = true;
    std::vector<scene_layer_composition> m_compositions;
  };

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