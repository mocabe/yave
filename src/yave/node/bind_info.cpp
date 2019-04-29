//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/bind_info.hpp>
#include <yave/obj/primitive.hpp>
#include <yave/rts/eval.hpp>

#include <algorithm>

namespace yave {

  namespace {
    auto has_unique_name = [](auto& names) {
      // sort
      std::sort(names.begin(), names.end());
      // unique
      auto end = std::unique(names.begin(), names.end());
      return end == names.end();
    };
  }

  BindInfo::BindInfo(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::string& output_socket,
    const object_ptr<const Object> get_instance_func,
    const std::string& description,
    bool is_const)
    : m_name {name}
    , m_input_sockets {input_sockets}
    , m_output_socket {output_socket}
    , m_is_const {is_const}
    , m_get_instance_func {get_instance_func}
    , m_description {description}
  {
    // check names
    if (!has_unique_name(m_input_sockets)) {
      throw std::invalid_argument("Input socket names should be unique");
    }

    // null
    if (!get_instance_func) {
      throw std::invalid_argument("get_instance_func is null");
    }

    // test primitive apply
    try {
      auto prim = make_object<Primitive>();
      auto app  = m_get_instance_func << prim;
      auto tp   = type_of(app);
    } catch (type_error::type_error& e) {
      throw std::invalid_argument("get_instance_func has invalid type");
    }
  }

  const std::string& BindInfo::name() const
  {
    return m_name;
  }

  const std::vector<std::string>& BindInfo::input_sockets() const
  {
    return m_input_sockets;
  }

  void BindInfo::set_input_sockets(const std::vector<std::string>& sockets)
  {
    auto tmp = sockets;
    if (!has_unique_name(tmp)) {
      throw std::invalid_argument("Input socket names should be unique");
    }
    std::swap(tmp, m_input_sockets);
  }

  const std::string& BindInfo::output_socket() const
  {
    return m_output_socket;
  }

  void BindInfo::set_output_socket(const std::string& socket)
  {
    m_output_socket = socket;
  }

  const object_ptr<const Object>& BindInfo::get_instance_func() const
  {
    return m_get_instance_func;
  }

  void BindInfo::set_instance_func(const object_ptr<const Object>& func)
  {
    // null check
    if (!func) {
      throw std::invalid_argument("func is null");
    }

    // test primitive apply
    try {
      auto prim = make_object<Primitive>();
      auto app  = func << prim;
      auto tp   = type_of(app);
    } catch (type_error::type_error& e) {
      throw std::invalid_argument("get_instance_func has invalid type");
    }
    m_get_instance_func = func;
  }

  bool BindInfo::is_const() const
  {
    return m_is_const;
  }

  void BindInfo::set_const(bool b)
  {
    m_is_const = b;
  }

  const std::string& BindInfo::description() const
  {
    return m_description;
  }

  void BindInfo::set_description(const std::string& d)
  {
    m_description = d;
  }

  object_ptr<const Object> BindInfo::get_instance(const primitive_t& prim) const
  {
    return eval(m_get_instance_func << make_object<Primitive>(prim));
  }

  [[nodiscard]] bool BindInfo::matches(const NodeInfo& info) const
  {
    return matches(info.name(), info.input_sockets(), info.output_sockets());
  }

  [[nodiscard]] bool BindInfo::matches(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::vector<std::string>& output_sockets) const
  {
    // check name
    if (m_name != name)
      return false;

    // check output socket
    auto match_output = [&] {
      for (auto&& s : output_sockets) {
        if (s == m_output_socket)
          return true;
      }
      return false;
    }();

    if (!match_output)
      return false;

    // check input sockets
    for (auto&& s : m_input_sockets) {
      auto found = [&] {
        for (auto&& t : input_sockets) {
          if (s == t)
            return true;
        }
        return false;
      }();
      if (!found)
        return false;
    }
    return true;
  }

} // namespace yave