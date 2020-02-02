//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/module/module.hpp>
#include <yave/module/ext/external_module_info.hpp>

namespace yave {

  /// Module instance controller.
  /// This class manages communication between modules.
  class external_module : public module
  {
  public:
    external_module(const object_ptr<const ExternalModuleInfo>& info);
    ~external_module() override;

    void init(const scene_config& config) override;

    void deinit() override;

    void update(const scene_config& config) override;

    bool initialized() const override;

    auto get_node_declarations() const
      -> std::vector<node_declaration> override;

    auto get_node_definitions() const -> std::vector<node_definition> override;

    auto get_scene_config() const -> scene_config override;

    auto instance_id() const -> uid override;

    auto name() const -> std::string override;

    auto module_id() const -> uuid override;

  private:
    object_ptr<const ExternalModuleInfo> m_module_info;

  private:
    bool m_initialized;
    uid m_instance_id;
  };

  namespace module_tags {
    class external_module
    {
    };
  }; // namespace module_tags

  template <>
  struct module_traits<module_tags::external_module>
  {
    auto get_module(const object_ptr<ExternalModuleInfo>& info)
    {
      return std::make_unique<external_module>(info);
    }
  };
}