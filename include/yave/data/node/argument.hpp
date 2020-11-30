//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/node/core/function.hpp>

#include <yave/data/vector/vector.hpp>
#include <yave/data/string/string.hpp>

namespace yave {

  // name/value pair
  struct node_argument_nvp
  {
    /// name of variable
    data::string name;
    /// variable which contains value.
    /// possible type: Int, Float, Bool, String and Type.
    object_ptr<const Object> value;
  };

  // fwd
  class node_argument_variable_object_value;
  using NodeArgumentVariable = Box<node_argument_variable_object_value>;

  /// value node of argument property tree
  class node_argument_variable_object_value
  {
    /// name of this value and value.
    /// when value.value is nullptr, !members.empty().
    node_argument_nvp m_value;

    /// properties of value
    /// when value.value is nullptr, properties may contain "type" variable
    /// which can be used from GUI otherwise empty.
    data::vector<node_argument_nvp> m_properties;

    /// members of value
    /// this is not empty iff value.value is nullptr.
    data::vector<object_ptr<NodeArgumentVariable>> m_members;

  public:
    node_argument_variable_object_value(
      const node_argument_nvp& var,
      const std::vector<node_argument_nvp>& props);

    node_argument_variable_object_value(
      const std::string& name,
      const object_ptr<const Type>& type,
      const std::vector<object_ptr<NodeArgumentVariable>>& members);

    bool is_value() const;
    auto name() const -> std::string;
    auto value() const -> object_ptr<const Object>;
    void set_value(const object_ptr<const Object> v);
    auto properties() const -> std::vector<node_argument_nvp>;
    auto type() const -> object_ptr<const Type>;
    auto members() const -> std::vector<object_ptr<NodeArgumentVariable>>;
    auto clone() const -> object_ptr<NodeArgumentVariable>;
  };

  class node_argument_variable_tree_object_value;
  using NodeArgumentVariableTree =
    Box<node_argument_variable_tree_object_value>;

  // fwd
  class node_argument_object_value;
  using NodeArgument = Box<node_argument_object_value>;

  class node_argument_object_value
  {
    /// property tree
    object_ptr<NodeArgumentVariable> m_value;
    /// generator function of (NodeArgument -> T)
    object_ptr<const Object> m_func;

    auto _get_node(const std::string& p) const
      -> object_ptr<NodeArgumentVariable>;

  public:
    static constexpr auto path_regex = R"(^(\w+\/)*\w$)";

    /// \param type root type of this argument
    /// \param vars property variables
    /// \param func generator function
    node_argument_object_value(
      const object_ptr<const Type> type,
      std::vector<object_ptr<NodeArgumentVariable>> vars,
      object_ptr<const Object> func);

    // clang-format off
    auto get_type() const -> object_ptr<const Type>;
    auto get_value(const std::string& p) const -> object_ptr<const Object>;
    void set_value(const std::string& p, object_ptr<const Object> v);
    auto get_properties(const std::string& p) const -> std::vector<node_argument_nvp>;
    auto get_type(const std::string& p) const -> object_ptr<const Type>;
    auto clone() const -> object_ptr<NodeArgument>;
    auto generate(object_ptr<const NodeArgument> self) const -> object_ptr<const Object>;
    // clang-format on
  };

  /// traits of node argument types
  template <class T>
  struct node_argument_traits
  {
    /// create new value of node argument.
    template <class... Ts>
    static auto create(Ts&&...)
    {
      static_assert(false_v<Ts...>, "Not implemented, or missing include");
    }

    template <class... Ts>
    static auto create_variable(Ts&&...)
    {
      static_assert(false_v<Ts...>, "Not implemented, or missing include");
    }
  };

  /// Create new node argument
  template <class T, class... Args>
  [[nodiscard]] auto make_node_argument(Args&&... args)
    -> object_ptr<NodeArgument>
  {
    return node_argument_traits<T>::create(std::forward<Args>(args)...);
  }

  /// Create new node argument variable
  template <class T, class... Ts>
  [[nodiscard]] auto make_node_argument_variable(Ts&&... args)
    -> object_ptr<NodeArgumentVariable>
  {
    return node_argument_traits<T>::create_variable(std::forward<Ts>(args)...);
  }

} // namespace yave