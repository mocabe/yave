//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/string/string.hpp>
#include <yave/lib/vector/vector.hpp>
#include <yave/lib/scene/scene_config.hpp>
#include <yave/rts/box.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// bind_info for backend interface.
  /// Only for communication with backends.
  /// Keep data types as binary compatible as possible.
  struct backend_bind_info
  {
    string name;                                // 16 byte
    vector<string> inputs;                      // 16
    vector<string> outputs;                     // 16
    string description;                         // 16
    object_ptr<const Object> get_instance_func; // 8
    uint64_t _padding_;                         // 8
  };

  static_assert(sizeof(backend_bind_info) == 80);

  /// BackendBindInfo
  using BackendBindInfo = Box<backend_bind_info>;

  /// List of bind info
  struct backend_bind_info_list
  {
    vector<object_ptr<BackendBindInfo>> info_list; // 16
  };

  static_assert(sizeof(backend_bind_info_list) == 16);

  /// BackendBindInfoList
  using BackendBindInfoList = Box<backend_bind_info_list>;

  /// backend info
  struct backend_info
  {
    void* handle;

    // clang-format off
    auto (*fp_init      )(void* handle, const scene_config& config) noexcept -> uid;
    void (*fp_deinit    )(void* handle, uid instance) noexcept;
    bool (*fp_update    )(void* handle, uid instance, const scene_config& config) noexcept;
    auto (*fp_get_config)(void* handle, uid instance) noexcept -> object_ptr<SceneConfig>;
    auto (*fp_get_binds )(void* handle, uid instance) noexcept -> object_ptr<BackendBindInfoList>;
    // clang-format on
  };

  /// BackendInfo
  using BackendInfo = Box<backend_info>;

} // namespace yave

// clang-format off
YAVE_DECL_TYPE(yave::BackendBindInfo,     "a292b66b-e8c2-4cae-a02e-3de38fa38cb7");
YAVE_DECL_TYPE(yave::BackendBindInfoList, "e6bdef71-b50f-4591-a6fc-ee99aa78ba4c");
YAVE_DECL_TYPE(yave::BackendInfo,         "11a91dd5-c9e1-4c41-b864-0175f2283647");
// clang-format on