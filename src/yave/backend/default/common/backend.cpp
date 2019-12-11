//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/backend/default/common/backend.hpp>

#include <yave/backend/default/common/control_flow.hpp>
#include <yave/backend/default/common/list.hpp>
#include <yave/backend/default/common/primitive_constructor.hpp>
#include <yave/backend/default/common/primitive.hpp>
#include <yave/backend/default/common/frame_time.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(yave::backends::default_common)

namespace yave::backends::default_common {

  backend::backend()
  {
    init_logger();
  }

  backend::~backend() noexcept
  {
  }

  void backend::init(const scene_config& config)
  {
    if (m_config) {
      Error(g_logger, "Failed to initialize backend: Already initialized");
      throw std::runtime_error("Already initialized");
    }

    m_config = std::make_unique<scene_config>(config);
    m_id     = uid::random_generate();
  }

  void backend::deinit()
  {
    m_config = nullptr;
  }

  void backend::update(const scene_config& config)
  {
    m_config = std::make_unique<scene_config>(config);
  }

  bool backend::initialized() const
  {
    return m_config != nullptr;
  }

  auto backend::get_node_declarations() const -> std::vector<node_declaration>
  {
    std::vector<node_declaration> ret;

    {
      // control_flow
      ret.push_back(get_node_declaration<node::If>());
      // list
      ret.push_back(get_node_declaration<node::ListNil>());
      ret.push_back(get_node_declaration<node::ListCons>());
      ret.push_back(get_node_declaration<node::ListHead>());
      ret.push_back(get_node_declaration<node::ListTail>());
      // primitive
      for (auto&& decl : get_primitive_node_declarations())
        ret.push_back(decl);
      // frame_time
      ret.push_back(get_node_declaration<node::FrameTime>());
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

      // control_flow
      def(yave::get_node_definitions<node::If, backend_tags::default_common>());
      // list
      def(yave::get_node_definitions<node::ListNil, backend_tags::default_common>());
      def(yave::get_node_definitions<node::ListCons, backend_tags::default_common>());
      def(yave::get_node_definitions<node::ListHead, backend_tags::default_common>());
      def(yave::get_node_definitions<node::ListTail, backend_tags::default_common>());
      // primitive
      def(yave::get_primitive_node_definitions<backend_tags::default_common>());

    } // clang-format on

    return ret;
  }

  auto backend::get_scene_config() const -> scene_config
  {
    return *m_config;
  }

  auto backend::instance_id() const -> uid
  {
    return m_id;
  }

  auto backend::name() const -> std::string
  {
    return "default_common";
  }

  auto backend::backend_id() const -> uuid
  {
    return namespace_uuid;
  }

} // namespace yave::backends::default_common