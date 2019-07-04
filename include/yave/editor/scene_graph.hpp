//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/core/managed_node_graph.hpp>
#include <yave/editor/layer_resource.hpp>
#include <yave/editor/layer_composition.hpp>
#include <yave/editor/scene_config.hpp>

namespace yave {

  using layer_handle = descriptor_handle<class scene_graph::layer*>;

  /// Scene graph class. Manages all frontend node controls, including layer
  /// system management.
  class scene_graph
  {
  public:
    scene_graph();
    ~scene_graph() noexcept;
    scene_graph(scene_graph&&) noexcept;
    scene_graph& operator=(scene_graph&&) noexcept;

  private:
    scene_graph(const scene_graph&) = delete;
    scene_graph& operator=(const scene_graph&) = delete;

  public:
    /// Get root layer
    layer_handle root() const;

    /// exists?
    bool exists(const layer_handle& layer) const;

    /// exists?
    bool exists(const node_handle& node) const;

    /// exists?
    bool exists(const node_handle& node, const layer_handle& layer) const;
  
    /// Create new sublayer under specified layer
    layer_handle add_layer(const layer_handle& target);

    /// Remove layer and resources
    void remove_layer(const layer_handle& handle);

    /// Get name of layer
    const std::string& get_name() const;
    /// Set name of layer
    void set_name(const std::string& name);

    /// visible?
    bool is_visible(const layer_handle& layer) const;

    /// set new visibility state
    void set_visibility(bool visibility);

    /// Custom layer?
    bool is_custom(const layer_handle& layer) const;

    /// Convert managed layer to custom layer
    void make_custom(const layer_handle& layer);

    /// Get list of sublayers
    [[nodiscard]] std::vector<layer_handle>
      get_sublayers(const layer_handle& layer) const;

    /// Get list of resources
    [[nodiscard]] std::vector<layer_resource>
      get_resources(const layer_handle& layer, layer_resource_scope scpe) const;

    /// Get composition list
    [[nodiscard]] std::vector<layer_composition>
      get_compositions(const layer_handle& layr) const;

    /// Get image outout node (node included in resources)
    [[nodiscard]] node_handle get_image_output(const layer_handle& layer) const;

  private: /* nodes */
    managed_node_graph m_node_graph;

  private: /* layers */
    struct layer_container;
    std::vector<layer_container> m_layers;

  private: /* root layer */
    struct layer;
    std::shared_ptr<layer> m_root;
  };
} // namespace yave