//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/data_types/primitive.hpp>
#include <yave/data/obj/primitive.hpp>
#include <yave/core/rts/object_ptr.hpp>
#include <yave/node/core/node_info.hpp>

#include <vector>
#include <functional>

namespace yave {

  /// typeless bind information
  class bind_info
  {
  public:
    /// Default ctor (deleted)
    bind_info() = delete;
    /// Copy ctor.
    bind_info(const bind_info&) = default;
    /// Mode ctor.
    bind_info(bind_info&&) = default;
    /// operator=
    bind_info& operator=(const bind_info&) = default;
    /// operator=
    bind_info& operator=(bind_info&&) = default;

    /// Create new bind info.
    /// \param Name name of bind (should match to target node_info::name())
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
    bind_info(
      std::string name,
      std::vector<std::string> input_sockets,
      std::string output_socket,
      object_ptr<const Object> instance_func,
      std::string description,
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
      get_instance(const object_ptr<const PrimitiveContainer>& prim) const;

    /// Check if this bind matches to node_info.
    [[nodiscard]] bool is_bind_of(const node_info& info) const;

    /// Check if this bind matches to node info.
    [[nodiscard]] bool is_bind_of(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets) const;

  private:
    /// name of node
    std::string m_name;
    /// input sockets (sorted/unique)
    std::vector<std::string> m_input_sockets;
    /// output sockets
    std::string m_output_socket;
    /// callback to get closure object
    object_ptr<const Object> m_get_instance_func;
    /// description
    std::string m_description;
    /// constness
    bool m_is_const;
  };

} // namespace yave