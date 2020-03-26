//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/module.hpp>
#include <yave/module/module_loader.hpp>

namespace yave::modules::_std {

  class module_loader : public yave::module_loader
  {
  public:
    module_loader(vulkan::vulkan_context& vulkan_ctx);

  public:
    bool load(const std::vector<std::string>& modules) override;
    void unload(const std::vector<std::string>& modules) override;
    auto get() const -> std::vector<std::shared_ptr<module>> override;
    auto last_errors() const -> error_list override;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave::modules::_std