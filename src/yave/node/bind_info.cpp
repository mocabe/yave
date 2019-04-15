//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/bind_info.hpp>

#include <algorithm>

namespace yave {

  BindInfo::BindInfo(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::string& output_socket,
    const std::function<object_ptr<>(primitive_t)>& get_instance_func,
    const std::string& description,
    bool is_const)
    : m_name {name}
    , m_input_sockets {input_sockets}
    , m_output_socket {output_socket}
    , m_is_const {is_const}
    , m_get_instance_func {get_instance_func}
    , m_description {description}
  {
    // sort
    std::sort(m_input_sockets.begin(), m_input_sockets.end());
    // check
    validate();
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
    m_input_sockets = sockets;
  }

  const std::string& BindInfo::output_socket() const
  {
    return m_output_socket;
  }

  void BindInfo::set_output_socket(const std::string& socket)
  {
    m_output_socket = socket;
  }

  const std::function<object_ptr<>(primitive_t)>&
    BindInfo::get_instance_func() const
  {
    return m_get_instance_func;
  }

  void BindInfo::set_instance_func(
    const std::function<object_ptr<>(primitive_t)>& func)
  {
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

  object_ptr<> BindInfo::get_instance(const primitive_t& prim) const
  {
    return m_get_instance_func(prim);
  }

  void BindInfo::validate()
  {
    auto _has_unique_names = [](auto& names) {
      // unique
      auto end = std::unique(names.begin(), names.end());
      return end == names.end();
    };
    if (!_has_unique_names(m_input_sockets))
      throw std::invalid_argument("Input sockets should be unique");
    if (m_get_instance_func == nullptr)
      throw std::invalid_argument("get_instance should not be null");
  }
} // namespace yave