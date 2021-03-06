//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/module.hpp>
#include <yave/module/std/config.hpp>
#include <yave/lib/vulkan/vulkan_context.hpp>

namespace yave {

  namespace modules::_std {

    /// std module
    class module : public yave::module
    {
    public:
      module(vulkan::vulkan_context& vulakn_ctx);
      ~module() noexcept override;
      void init(const scene_config& config) override;
      void deinit() override;
      void update(const scene_config& config) override;
      bool initialized() const override;
      auto get_node_declarations() const
        -> std::vector<node_declaration> override;
      auto get_node_definitions() const
        -> std::vector<node_definition> override;
      auto get_scene_config() const -> scene_config override;
      auto instance_id() const -> uid override;
      auto name() const -> std::string override;
      auto module_id() const -> uuid override;

    private:
      struct impl;
      std::unique_ptr<impl> m_pimpl;
    };
  } // namespace modules::_std
} // namespace yave