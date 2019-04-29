//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/primitive_definition.hpp>
#include <yave/rts/object_ptr.hpp>

#include <vector>
#include <functional>

namespace yave {

  /// typeless bind information
  class BindInfo
  {
  public:
    /// Default ctor (deleted)
    BindInfo() = delete;
    /// Copy ctor.
    BindInfo(const BindInfo&) = default;
    /// Mode ctor.
    BindInfo(BindInfo&&) = default;
    /// operator=
    BindInfo& operator=(const BindInfo&) = default;
    /// operator=
    BindInfo& operator=(BindInfo&&) = default;

    /// Create new bind info.
    /// \param Name name of bind (should match to target NodeInfo::name())
    /// \param input_sockets List of input socket names. All names shoule be
    /// unique and sorted. If not sorted, constructor will automatically sort
    /// it.
    /// \param output_socket Output socket name.
    /// \param instance_func A non-null managed pointer to a closure object
    /// which takes sintle Primitive object and returns instance of node object
    /// (closure). Thus `type_of(instance_func
    /// << make_object<Primitive>())` should not throw.
    /// \param description Description of this bind.
    /// \throws std::invalid_argument when arguments are invalid.
    BindInfo(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::string& output_socket,
      const object_ptr<const Object> instance_func,
      const std::string& description,
      bool is_const = true);

    /// Get name.
    [[nodiscard]] const std::string& name() const;

    /// Set name.
    void set_name(const std::string& name);

    /// Get input sockets.
    /// Socket names are sorted.
    [[nodiscard]] const std::vector<std::string>& input_sockets() const;

    /// Set input sockets.
    /// \param sockets Should have unique and sorted names. When not sorted,
    /// this function will automatically sort it.
    void set_input_sockets(const std::vector<std::string>& sockets);

    /// Get output socket.
    [[nodiscard]] const std::string& output_socket() const;

    /// Set output socket.
    void set_output_socket(const std::string& socket);

    /// Get instance function.
    [[nodiscard]] const object_ptr<const Object>& get_instance_func() const;

    /// Set instance function.
    /// \param func A non-null managed pointer to an object (closure) which has
    /// type (Primitive -> T).
    void set_instance_func(const object_ptr<const Object>& func);

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
    [[nodiscard]] object_ptr<const Object>
      get_instance(const primitive_t& prim) const;

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
    object_ptr<const Object> m_get_instance_func;
    /// description
    std::string m_description;
  };

} // namespace yave