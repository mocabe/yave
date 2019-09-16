//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/scene/layered_node_graph.hpp>
#include <yave/node/scene/scene_config.hpp>
#include <yave/node/scene/layer_composition.hpp>

namespace yave {

  /// scene_graph layer type
  enum class layer_type : uint32_t
  {
    compos = 1,
    image  = 2,
  };

  /// Scene graph class. Manages all frontend node controls, including layer
  /// system management.
  class scene_graph
  {

  public:
    /// Initialize scene graph
    scene_graph();
    /// Dtor
    ~scene_graph() noexcept;
    /// Move scene graph
    scene_graph(scene_graph&&) noexcept;
    /// Move assign scene graph
    scene_graph& operator=(scene_graph&&) noexcept;

  public:
    scene_graph(const scene_graph&) = delete;
    scene_graph& operator=(const scene_graph&) = delete;

  public:
    /// Get root layer handle
    [[nodiscard]] auto root() const -> layer_handle;

    /// exists?
    [[nodiscard]] bool exists(const layer_handle& layer) const;

    /// exists?
    [[nodiscard]] bool exists(const layer_resource_handle& node) const;

    /// is resource exists in specific layer?
    [[nodiscard]] bool exists(
      const layer_resource_handle& node,
      const layer_handle& layer) const;

    /// Create new image layer under specified layer.
    [[nodiscard]] auto add_layer(const layer_handle& target, layer_type type)
      -> layer_handle;

    /// Remove layer and resources
    void remove_layer(const layer_handle& handle);

  public:
    /// Movable into target layer?
    [[nodiscard]] bool movable_into(
      const layer_handle& frmo,
      const layer_handle& to) const;

    /// Movable into below target layer?
    [[nodiscard]] bool movable_below(
      const layer_handle& from,
      const layer_handle& to) const;

    /// Movable above target layer?
    [[nodiscard]] bool movable_above(
      const layer_handle& from,
      const layer_handle& to) const;

    /// Move layer as sublayer of target layer
    void move_into(const layer_handle& from, const layer_handle& to);

    /// Move layer below specific layer
    void move_below(const layer_handle& from, const layer_handle& to);

    /// Move layer above specific layer
    void move_above(const layer_handle& from, const layer_handle& to);

  public:
    /// Compos layer?
    [[nodiscard]] bool is_compos_layer(const layer_handle& layer) const;

    /// Image  layer
    [[nodiscard]] bool is_image_layer(const layer_handle& layer) const;

    /// visible?
    [[nodiscard]] bool is_visible(const layer_handle& layer) const;

    /// Get name of layer
    [[nodiscard]] auto get_name(const layer_handle& layer) const
      -> std::optional<std::string>;

    /// set new visibility state
    void set_visibility(const layer_handle& layer, bool visibility);

    /// Set name of layer
    void set_name(const layer_handle& layer, const std::string& name);

  public:
    /// Get parent layer
    [[nodiscard]] auto get_parent(const layer_handle& layer) const
      -> layer_handle;

    /// Get list of sublayers
    [[nodiscard]] auto get_sublayers(const layer_handle& layer) const
      -> std::vector<layer_handle>;

    /// Get list of managed resources.
    [[nodiscard]] auto get_resources(const layer_handle& layer) const
      -> std::vector<layer_resource_handle>;

    /// Get list of owning resources
    [[nodiscard]] auto get_resources_owning(const layer_handle& layer) const
      -> std::vector<layer_resource_handle>;

    /// Get list of inherited resources
    [[nodiscard]] auto get_resources_reference(const layer_handle& layer) const
      -> std::vector<layer_resource_handle>;

    /// Get composition list
    [[nodiscard]] auto get_compositions(const layer_handle& layr) const
      -> std::vector<layer_composition>;

    /// Get image outout node (node included in resources)
    [[nodiscard]] auto get_image_output(const layer_handle& layer) const
      -> layer_resource_handle;

  public: /* node_graph interface */
    /// node_graph and meta info of scene
    struct node_graph_output
    {
      /// node_graph
      node_graph graph;
      /// image output root
      node_handle image_root;
    };

    /// Get copy of current node graph.
    [[nodiscard]] auto get_node_graph() const -> node_graph_output;

  private:
    struct layer_attribute;
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;
    [[nodiscard]] auto _add_layer_attribute(
      const layer_handle& layer,
      layer_type) -> layer_attribute*;
    [[nodiscard]] auto _get_layer_attribute(const layer_handle& layer)
      -> layer_attribute*;
    [[nodiscard]] auto _get_layer_attribute(const layer_handle& layer) const
      -> const layer_attribute*;
    [[nodiscard]] auto _add_layer(const layer_handle& layer, layer_type type)
      -> layer_handle;
    void _remove_layer(const layer_handle& layer);

  private: /* underlying layer manager */
    layered_node_graph m_graph;

  private: /* global resources */
    layer_resource_handle m_image_output;
    layer_resource_handle m_audio_output;
    layer_resource_handle m_empty_frame_buffer;

  private:
    connection_handle m_c_fb_root;
    connection_handle m_c_root_io;

  private: /* additional layer info */
    std::map<layer_handle, layer_attribute> m_layer_attributes;

  private: /* mutex */
    mutable std::mutex m_mtx;
  };

} // namespace yave