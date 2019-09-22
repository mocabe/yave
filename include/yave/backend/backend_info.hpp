//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/string/string.hpp>
#include <yave/lib/vector/vector.hpp>
#include <yave/obj/scene/scene_config.hpp>
#include <yave/node/core/bind_info.hpp>
#include <yave/rts/box.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// bind_info for backend interface.
  /// Only for communication with backends.
  /// Keep data types as binary compatible as possible.
  struct backend_bind_info
  {
    backend_bind_info(const bind_info& info)
    {
      // we need some code for conversion from std::vector<string> to
      // vector<string>
      std::vector<string> inputs_tmp;
      inputs_tmp.reserve(info.input_sockets().size());

      for (auto&& name : info.input_sockets())
        inputs_tmp.push_back(name);

      m_name          = info.name();
      m_inputs        = inputs_tmp;
      m_output        = info.output_socket();
      m_instanec_func = info.get_instance_func();
      m_description   = info.description();
      m_is_const      = info.is_const();
    }

    [[nodiscard]] auto name() const -> std::string
    {
      return m_name;
    }

    [[nodiscard]] auto input_sockets() const -> std::vector<std::string>
    {
      std::vector<std::string> tmp;
      tmp.reserve(m_inputs.size());

      for (auto&& s : m_inputs)
        tmp.emplace_back(s);

      return tmp;
    }

    [[nodiscard]] auto output_socket() const -> std::string
    {
      return m_output;
    }

    [[nodiscard]] auto description() const -> std::string
    {
      return m_description;
    }

    [[nodiscard]] auto instance_func() const -> const object_ptr<const Object>&
    {
      return m_instanec_func;
    }

    [[nodiscard]] bool is_const() const
    {
      return m_is_const;
    }

    [[nodiscard]] bind_info bind_info() const
    {
      return {name(),
              input_sockets(),
              output_socket(),
              instance_func(),
              description(),
              is_const()};
    }

  private:
    string m_name;                            // 16 byte
    vector<string> m_inputs;                  // 16
    string m_output;                          // 16
    string m_description;                     // 16
    object_ptr<const Object> m_instanec_func; // 8
    uint64_t m_is_const;                      // 8 (including padding)
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
    // clang-format off
    backend_info(
      const string& name,
      const uuid& backend_id,
      void* handle,
      auto (*fp_init)(void* handle, const scene_config& config) noexcept->uid,
      void (*fp_deinit)(void* handle, uid instance) noexcept,
      bool (*fp_update)(void* handle, uid instance, const scene_config& config) noexcept,
      auto (*fp_get_config)(void* handle, uid instance) noexcept ->object_ptr<SceneConfig>,
      auto (*fp_get_binds)(void* handle, uid instance) noexcept ->object_ptr<BackendBindInfoList>)
      : m_name {name}
      , m_backend_id {backend_id}
      , m_handle {handle}
      , m_fp_init {fp_init}
      , m_fp_deinit {fp_deinit}
      , m_fp_update {fp_update}
      , m_fp_get_config {fp_get_config}
      , m_fp_get_binds {fp_get_binds}
    {
    }
    // clang-format on

    [[nodiscard]] auto init(const scene_config& config) const noexcept -> uid
    {
      return m_fp_init(m_handle, config);
    }

    void deinit(uid instance) const noexcept
    {
      return m_fp_deinit(m_handle, instance);
    }

    [[nodiscard]] bool update(uid instance, const scene_config& config) const
      noexcept
    {
      return m_fp_update(m_handle, instance, config);
    }

    [[nodiscard]] auto get_config(uid instance) const noexcept
      -> object_ptr<SceneConfig>
    {
      return m_fp_get_config(m_handle, instance);
    }

    [[nodiscard]] auto get_binds(uid instance) const noexcept
      -> object_ptr<BackendBindInfoList>
    {
      return m_fp_get_binds(m_handle, instance);
    }

    [[nodiscard]] auto name() const -> std::string
    {
      return m_name;
    }

    [[nodiscard]] auto backend_id() const -> const uuid&
    {
      return m_backend_id;
    }

  private:
    string m_name;
    uuid m_backend_id;

  private:
    void* m_handle;
    // clang-format off
    auto (*m_fp_init      )(void* handle, const scene_config& config) noexcept -> uid;
    void (*m_fp_deinit    )(void* handle, uid instance) noexcept;
    bool (*m_fp_update    )(void* handle, uid instance, const scene_config& config) noexcept;
    auto (*m_fp_get_config)(void* handle, uid instance) noexcept -> object_ptr<SceneConfig>;
    auto (*m_fp_get_binds )(void* handle, uid instance) noexcept -> object_ptr<BackendBindInfoList>;
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