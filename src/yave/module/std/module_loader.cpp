//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/module_loader.hpp>

namespace yave::modules::_std {

  class module_loader::impl
  {
  public:
    vulkan::vulkan_context& vulkan_ctx;
    error_list errs;
    std::shared_ptr<module> mdl;

  public:
    impl(vulkan::vulkan_context& ctx)
      : vulkan_ctx {ctx}
    {
    }

    bool load(const std::vector<std::string>& modules)
    {
      errs.clear();

      if (
        std::find(modules.begin(), modules.end(), module_name) == modules.end())
        return false;

      if (!mdl)
        mdl = std::make_shared<module>(vulkan_ctx);

      return true;
    }

    void unload(const std::vector<std::string>& modules)
    {
      if (
        std::find(modules.begin(), modules.end(), module_name) != modules.end())
        mdl = nullptr;
    }

    auto get() -> std::vector<std::shared_ptr<module>>
    {
      if (mdl)
        return {mdl};

      return {};
    }

    auto& last_errors()
    {
      return errs;
    }
  };

  module_loader::module_loader(vulkan::vulkan_context& vulkan_ctx)
    : m_pimpl {std::make_unique<impl>(vulkan_ctx)}
  {
  }

  module_loader::~module_loader() noexcept = default;

  bool module_loader::load(const std::vector<std::string>& modules)
  {
    return m_pimpl->load(modules);
  }

  void module_loader::unload(const std::vector<std::string> &modules)
  {
    return m_pimpl->unload(modules);
  }

  auto module_loader::get() const -> std::vector<std::shared_ptr<module>>
  {
    return m_pimpl->get();
  }

  auto module_loader::last_errors() const -> error_list
  {
    return m_pimpl->last_errors().clone();
  }
}