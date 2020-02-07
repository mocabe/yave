//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/primitive/primitive.hpp>

namespace yave {

  /// Node definition provided by backend.
  class node_definition
  {
  public:
    /// Construct node definition.
    /// \param name name of bind (should match to target node_info::name())
    /// \param output_socket name of output socket
    /// \param description Description of this bind.
    /// \param inst_getter A non-null managed pointer to a closure object
    /// \throws std::invalid_argument when arguments are invalid.
    node_definition(
      std::string name,
      size_t output_socket,
      object_ptr<const Object> inst_getter,
      std::string description)
      : m_name {std::move(name)}
      , m_os {std::move(output_socket)}
      , m_inst_getter {std::move(inst_getter)}
      , m_description {std::move(description)}
    {
      // null
      if (!m_inst_getter) {
        throw std::invalid_argument("get_instance_func is null");
      }

      // check type of getter func
      try {
        auto prim = make_object<Int>();
        auto app  = m_inst_getter << prim;
        auto tp   = type_of(app);
      } catch (type_error::type_error& e) {
        throw std::invalid_argument(
          std::string("get_instance_func has invalid type: ") + e.what());
      }
    }

    /// Get instance by calling instance function.
    [[nodiscard]] auto get_instance(object_ptr<const Object> prim) const
      -> object_ptr<const Object>
    {
      auto app = m_inst_getter << std::move(prim);
      return eval(app);
    }

    [[nodiscard]] auto name() const -> const auto&
    {
      return m_name;
    }

    [[nodiscard]] auto description() const -> const auto&
    {
      return m_description;
    }

    [[nodiscard]] auto instance_getter() const -> const auto&
    {
      return m_inst_getter;
    }

    [[nodiscard]] auto output_socket() const -> const auto&
    {
      return m_os;
    }

  private:
    /// name of name
    std::string m_name;
    /// output socket index
    size_t m_os;
    /// instance getter
    object_ptr<const Object> m_inst_getter;
    /// instance description
    std::string m_description;
  };
}