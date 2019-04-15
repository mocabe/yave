//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/primitive_definition.hpp>
#include <yave/rts/object_ptr.hpp>
#include <functional>

namespace yave {

  /// typeless bind information
  class BindInfo
  {
  public:
    BindInfo()                = delete;
    BindInfo(const BindInfo&) = default;
    BindInfo(BindInfo&&)      = default;
    BindInfo& operator=(const BindInfo&) = default;
    BindInfo& operator=(BindInfo&&) = default;
    BindInfo(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::string& output_socket,
      const std::function<object_ptr<>(primitive_t)>& get_instance_func,
      const std::string& description,
      bool is_const = true);

    /// Get name.
    [[nodiscard]] const std::string& name() const;

    /// Set name.
    void set_name(const std::string& name);

    /// Get input sockets.
    [[nodiscard]] const std::vector<std::string>& input_sockets() const;

    /// Set input sockets.
    void set_input_sockets(const std::vector<std::string>& sockets);

    /// Get output sockets.
    [[nodiscard]] const std::string& output_socket() const;

    /// Set output socket.
    void set_output_socket(const std::string& socket);

    /// Get instance function.
    [[nodiscard]] const std::function<object_ptr<>(primitive_t)>&
      get_instance_func() const;

    /// Set instance function.
    void
      set_instance_func(const std::function<object_ptr<>(primitive_t)>& func);

    /// Check constness.
    /// \notes const attribute is currently not used
    [[nodiscard]] bool is_const() const;

    /// Set constness.
    void set_const(bool b);

    /// Get description.
    [[nodiscard]] const std::string& description() const;

    /// Set description.
    void set_description(const std::string& d);

    /// Get instance by calling instance function.
    [[nodiscard]] object_ptr<> get_instance(const primitive_t& prim) const;

  private:
    void validate();

  private:
    /// name of node
    std::string m_name;
    /// input sockets (sorted/unique)
    std::vector<std::string> m_input_sockets;
    /// output sockets
    std::string m_output_socket;
    /// constness
    bool m_is_const;
    /// callback to get closure object
    std::function<object_ptr<>(primitive_t)> m_get_instance_func;
    /// description
    std::string m_description;
  };

} // namespace yave