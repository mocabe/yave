//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/backend/default/render/backend.hpp>

#include <yave/backend/default/render/blend_op.hpp>
#include <yave/backend/default/render/compositor.hpp>
#include <yave/backend/default/render/frame_buffer.hpp>
#include <yave/backend/default/render/image_io.hpp>
#include <yave/backend/default/render/image_output.hpp>
#include <yave/backend/default/render/keyframe.hpp>

#include <yave/support/log.hpp>

#include <optional>

YAVE_DECL_G_LOGGER(yave::backends::default_render)

namespace yave::backends::default_render {

  struct backend::impl
  {
    uid id;
    bool initialized;
    std::optional<scene_config> config;
    std::optional<frame_buffer_manager> fb_manager;
  };

  backend::backend()
  {
    init_logger();

    m_pimpl     = std::make_unique<impl>();
    m_pimpl->id = uid::random_generate();
  }

  backend::~backend() noexcept
  {
  }

  void backend::init(const scene_config& config)
  {
    if (m_pimpl->initialized) {
      Error(g_logger, "Failed to initialize backend: Already initialized");
      throw std::runtime_error("backend: Already initialized");
    }

    m_pimpl->config = config;

    m_pimpl->fb_manager = frame_buffer_manager(
      config.width(), config.height(), config.frame_buffer_format());

    m_pimpl->initialized = true;
  }

  void backend::deinit()
  {
    // release resources
    m_pimpl->config     = std::nullopt;
    m_pimpl->fb_manager = std::nullopt;

    m_pimpl->initialized = false;
  }

  void backend::update(const scene_config& config)
  {
    m_pimpl->config = config;
  }

  bool backend::initialized() const
  {
    return true;
  }

  auto backend::get_node_declarations() const -> std::vector<node_declaration>
  {
    std::vector<node_declaration> ret;

    {
      ret.push_back(get_node_declaration<node::BlendOpSrc>());
      ret.push_back(get_node_declaration<node::BlendOpDst>());
      ret.push_back(get_node_declaration<node::BlendOpOver>());
      ret.push_back(get_node_declaration<node::BlendOpIn>());
      ret.push_back(get_node_declaration<node::BlendOpOut>());
      ret.push_back(get_node_declaration<node::BlendOpAdd>());
      ret.push_back(get_node_declaration<node::LayerCompositor>());
      ret.push_back(get_node_declaration<node::FrameBuffer>());
      ret.push_back(get_node_declaration<node::LoadImage>());
      ret.push_back(get_node_declaration<node::LayerImageOutput>());
      ret.push_back(get_node_declaration<node::KeyframeEvaluatorInt>());
      ret.push_back(get_node_declaration<node::KeyframeEvaluatorFloat>());
      ret.push_back(get_node_declaration<node::KeyframeEvaluatorBool>());
    }

    return ret;
  }

  auto backend::get_node_definitions() const -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;

    auto def = [&](auto&& defs) {
      for (auto&& def : defs)
        ret.push_back(def);
    };

    { // clang-format off

      def(yave::get_node_definitions<node::BlendOpSrc, backend_tags::default_render>());
      def(yave::get_node_definitions<node::BlendOpDst, backend_tags::default_render>());
      def(yave::get_node_definitions<node::BlendOpOver, backend_tags::default_render>());
      def(yave::get_node_definitions<node::BlendOpIn, backend_tags::default_render>());
      def(yave::get_node_definitions<node::BlendOpOut, backend_tags::default_render>());
      def(yave::get_node_definitions<node::BlendOpAdd, backend_tags::default_render>());
      def(yave::get_node_definitions<node::LayerCompositor, backend_tags::default_render>());
      def(yave::get_node_definitions<node::FrameBuffer, backend_tags::default_render>(&(*m_pimpl->fb_manager)));
      def(yave::get_node_definitions<node::LoadImage, backend_tags::default_render>());
      def(yave::get_node_definitions<node::LayerImageOutput, backend_tags::default_render>());
      def(yave::get_node_definitions<node::KeyframeEvaluatorInt, backend_tags::default_render>());
      def(yave::get_node_definitions<node::KeyframeEvaluatorFloat, backend_tags::default_render>());
      def(yave::get_node_definitions<node::KeyframeEvaluatorBool, backend_tags::default_render>());

    } // clang-format on

    return ret;
  }

  auto backend::get_scene_config() const -> scene_config
  {
    return *m_pimpl->config;
  }

  auto backend::instance_id() const -> uid
  {
    return m_pimpl->id;
  }

  auto backend::name() const -> std::string
  {
    return "default_render";
  }

  auto backend::backend_id() const -> uuid
  {
    return namespace_uuid;
  }

} // namespace yave::backends::default_render