//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/backend/backend.hpp>
#include <yave/backend/external_backend_info.hpp>

namespace yave {

  /// Backend instance controller.
  /// This class manages communication between backends.
  class external_backend : public backend
  {
  public:
    external_backend(const object_ptr<const ExternalBackendInfo>& info);
    ~external_backend() override;

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

    auto backend_id() const -> uuid override;

  private:
    object_ptr<const ExternalBackendInfo> m_backend_info;

  private:
    bool m_initialized;
    uid m_instance_id;
  };

  namespace backend_tags {
    class external_backend
    {
    };
  }; // namespace backend_tags

  template <>
  struct backend_traits<backend_tags::external_backend>
  {
    auto get_backend(const object_ptr<ExternalBackendInfo>& info)
    {
      return std::make_unique<external_backend>(info);
    }
  };
}