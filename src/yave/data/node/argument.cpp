//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/node/argument.hpp>

#include <range/v3/view.hpp>
#include <regex>

namespace yave {

  namespace rn = ranges;
  namespace rv = ranges::views;

  namespace {

    auto split_path(const std::string& path) -> std::vector<std::string>
    {
      static const auto re = std::regex(node_argument_object_value::path_regex);

      if (!std::regex_match(path, re))
        return {};

      return path //
             | rv::split('/')
             | rv::transform([](auto&& r) { return r | rn::to<std::string>; })
             | rn::to_vector;
    }
  } // namespace

  // ----------------------------------------
  // node_argument_variable_object_value

  node_argument_variable_object_value::node_argument_variable_object_value(
    const node_argument_nvp& var,
    const std::vector<node_argument_nvp>& props)
    : m_value {var}
    , m_properties {props}
    , m_members {}
  {
  }

  node_argument_variable_object_value::node_argument_variable_object_value(
    const std::string& name,
    const object_ptr<const Type>& type,
    const std::vector<object_ptr<NodeArgumentVariable>>& members)
    : m_value {.name = data::string(name), .value = nullptr}
    , m_properties {{.name = u8"type", .value = type}}
    , m_members {std::move(members)}
  {
  }

  bool node_argument_variable_object_value::is_value() const
  {
    return static_cast<bool>(m_value.value);
  }

  auto node_argument_variable_object_value::name() const -> std::string
  {
    return m_value.name;
  }

  auto node_argument_variable_object_value::value() const
    -> object_ptr<const Object>
  {
    if (is_value())
      return m_value.value;
    return nullptr;
  }

  void node_argument_variable_object_value::set_value(
    const object_ptr<const Object> v)
  {
    if (is_value() && same_type(get_type(m_value.value), get_type(v)))
      m_value.value = std::move(v);
  }

  auto node_argument_variable_object_value::properties() const
    -> std::vector<node_argument_nvp>
  {
    if (is_value())
      return m_properties;
    return {};
  }

  auto node_argument_variable_object_value::type() const
    -> object_ptr<const Type>
  {
    if (is_value())
      return get_type(m_value.value);

    assert(m_properties[0].name == u8"type");
    return value_cast<const Type>(m_properties[0].value);
  }

  auto node_argument_variable_object_value::members() const
    -> std::vector<object_ptr<NodeArgumentVariable>>
  {
    if (!is_value()) {
      assert(!m_members.empty());
      return m_members;
    }
    return {};
  }

  auto node_argument_variable_object_value::clone() const
    -> object_ptr<NodeArgumentVariable>
  {
    if (is_value()) {

      auto nvp = node_argument_nvp {
        .name = m_value.name, .value = m_value.value.clone()};

      auto props = m_properties;

      return make_object<NodeArgumentVariable>(
        std::move(nvp), std::move(props));
    }

    auto ns = name();
    auto ty = type();
    auto ms = m_members;

    for (auto& m : ms)
      m = m->clone();

    return make_object<NodeArgumentVariable>(
      std::move(ns), std::move(ty), std::move(ms));
  }

  // ----------------------------------------
  // node_argument_object_value

  node_argument_object_value::node_argument_object_value(
    const object_ptr<const Type> type,
    std::vector<object_ptr<NodeArgumentVariable>> vars,
    object_ptr<const Object> func)
    : m_value {make_object<NodeArgumentVariable>("", type, std::move(vars))}
    , m_func {std::move(func)}
  {
  }

  auto node_argument_object_value::get_type() const -> object_ptr<const Type>
  {
    assert(!m_value->is_value());
    return m_value->type();
  }

  auto node_argument_object_value::_get_node(const std::string& path) const
    -> object_ptr<NodeArgumentVariable>
  {
    object_ptr<NodeArgumentVariable> node = m_value;

    for (auto&& name : split_path(path)) {

      if (node->is_value())
        return nullptr;

      node = [&]() -> object_ptr<NodeArgumentVariable> {
        for (auto&& m : node->members()) {
          if (m->name() == name) {
            return m;
          }
        }
        return nullptr;
      }();

      if (!node)
        break;
    }
    return node;
  }

  auto node_argument_object_value::get_value(const std::string& path) const
    -> object_ptr<const Object>
  {
    if (auto n = _get_node(path))
      return n->value();
    return nullptr;
  }

  void node_argument_object_value::set_value(
    const std::string& path,
    object_ptr<const Object> val)
  {
    if (auto n = _get_node(path))
      n->set_value(std::move(val));
  }

  auto node_argument_object_value::get_properties(const std::string& path) const
    -> std::vector<node_argument_nvp>
  {
    if (auto n = _get_node(path))
      return n->properties();
    return {};
  }

  auto node_argument_object_value::get_type(const std::string& path) const
    -> object_ptr<const Type>
  {
    if (auto n = _get_node(path))
      return n->type();
    return nullptr;
  }

  auto node_argument_object_value::clone() const -> object_ptr<NodeArgument>
  {
    return make_object<NodeArgument>(
      m_value->type(), m_value->clone()->members(), m_func);
  }

  auto node_argument_object_value::generate(
    object_ptr<const NodeArgument> self) const -> object_ptr<const Object>
  {
    return m_func << std::move(self);
  }
} // namespace yave