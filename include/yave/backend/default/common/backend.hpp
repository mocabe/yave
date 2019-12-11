//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/backend.hpp>
#include <yave/backend/default/config.hpp>

#include <memory>

namespace yave {

  namespace backends::default_common {

    class backend : public yave::backend
    {
    public:
      backend();
      ~backend() noexcept override;
      void init(const scene_config& config) override;
      void deinit() override;
      void update(const scene_config& config) override;
      bool initialized() const override;
      auto get_node_declarations() const
        -> std::vector<node_declaration> override;
      virtual auto get_node_definitions() const
        -> std::vector<node_definition> override;
      auto get_scene_config() const -> scene_config override;
      auto instance_id() const -> uid override;
      auto name() const -> std::string override;
      auto backend_id() const -> uuid override;

    private:
      std::unique_ptr<scene_config> m_config;
      uid m_id;
    };

  } // namespace backends::default_common

  template <>
  struct backend_traits<backend_tags::default_common>
  {
    static auto get_backend()
    {
      return std::make_unique<backends::default_common::backend>();
    }
  };

} // namespace yave
