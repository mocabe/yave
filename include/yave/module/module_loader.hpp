//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/module/module.hpp>

namespace yave {

  /// Module loader interface.
  class module_loader
  {
  public:
    virtual ~module_loader() noexcept = default;

  public:
    /// Load modules.
    /// When one or more modules specified in argument are not found, returns
    /// false. Modules found are still loaded and can be accessed from get().
    virtual bool load(const std::vector<std::string>& modules) = 0;
    /// Unloads specific module from this loader. This function affects next
    /// get(). This function will not call deinit() on modules.
    virtual void unload(const std::vector<std::string>& modules) = 0;
    /// Get list of module pointers.
    /// Modules are usually managed in shared_ptr, so will not be actually
    /// unloaded until all references are dead.
    virtual auto get() const -> std::vector<std::shared_ptr<module>> = 0;
  };
} // namespace yave