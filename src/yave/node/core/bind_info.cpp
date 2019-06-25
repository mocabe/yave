//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/bind_info.hpp>
#include <yave/data/obj/primitive.hpp>
#include <yave/core/rts/eval.hpp>

#include <yave/data/obj/frame.hpp>

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

  bind_info::bind_info(
    std::string name,
    std::vector<std::string> input_sockets,
    std::string output_socket,
    object_ptr<const Object> get_instance_func,
    std::string description,
    bool is_const)
    : m_name {std::move(name)}
    , m_input_sockets {std::move(input_sockets)}
    , m_output_socket {std::move(output_socket)}
    , m_get_instance_func {std::move(get_instance_func)}
    , m_description {std::move(description)}
    , m_is_const {is_const}
  {
    // check names
    if (!has_unique_name(m_input_sockets)) {
      throw std::invalid_argument("Input socket names should be unique");
    }

    // null
    if (!m_get_instance_func) {
      throw std::invalid_argument("get_instance_func is null");
    }

    // test type
    try {
      auto prim = make_object<PrimitiveContainer>();
      auto app  = m_get_instance_func << prim;
      auto tp   = type_of(app);

      auto flat = flatten(tp);

      if (flat.size() != m_input_sockets.size() + 2)
        throw std::invalid_argument(
          "get_instance_func may return closure with invalid number of "
          "arguments");

      if (!same_type(flat[m_input_sockets.size()], object_type<Frame>()))
        throw std::invalid_argument(
          "get_instance_func may return closure with invalid argument type");

    } catch (type_error::type_error& e) {
      throw std::invalid_argument(
        std::string("get_instance_func has invalid type: ") + e.what());
    }
  }

  const std::string& bind_info::name() const
  {
    return m_name;
  }

  const std::vector<std::string>& bind_info::input_sockets() const
  {
    return m_input_sockets;
  }

  void bind_info::set_input_sockets(const std::vector<std::string>& sockets)
  {
    auto tmp = sockets;
    if (!has_unique_name(tmp)) {
      throw std::invalid_argument("Input socket names should be unique");
    }
    std::swap(tmp, m_input_sockets);
  }

  const std::string& bind_info::output_socket() const
  {
    return m_output_socket;
  }

  void bind_info::set_output_socket(const std::string& socket)
  {
    m_output_socket = socket;
  }

  const object_ptr<const Object>& bind_info::get_instance_func() const
  {
    return m_get_instance_func;
  }

  void bind_info::set_instance_func(const object_ptr<const Object>& func)
  {
    // null check
    if (!func) {
      throw std::invalid_argument("func is null");
    }

    // test primitive apply
    try {
      auto prim = make_object<PrimitiveContainer>();
      auto app  = func << prim;
      auto tp   = type_of(app);

      auto flat = flatten(tp);
      if (
        flat.size() != m_input_sockets.size() + 2 ||
        !same_type(flat[m_input_sockets.size()], object_type<Frame>()))
        throw std::invalid_argument(
          "Invalid closure type retuend from instance getter function");

    } catch (type_error::type_error& e) {
      throw std::invalid_argument(
        std::string("Invalid type in instance getter function: ") + e.what());
    }
    m_get_instance_func = func;
  }

  bool bind_info::is_const() const
  {
    return m_is_const;
  }

  void bind_info::set_const(bool b)
  {
    m_is_const = b;
  }

  const std::string& bind_info::description() const
  {
    return m_description;
  }

  void bind_info::set_description(const std::string& d)
  {
    m_description = d;
  }

  object_ptr<const Object> bind_info::get_instance(
    const object_ptr<const PrimitiveContainer>& prim) const
  {
    auto app = m_get_instance_func << prim;
    return eval(app);
  }

  bool bind_info::is_bind_of(const node_info& info) const
  {
    return is_bind_of(info.name(), info.input_sockets(), info.output_sockets());
  }

  bool bind_info::is_bind_of(
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

    // check inupt sockets
    size_t bgn = 0;
    for (auto&& s : m_input_sockets) {
      auto found = [&] {
        for (auto i = bgn; i < input_sockets.size(); ++i) {
          if (s == input_sockets[i]) {
            bgn = i + 1;
            return true;
          }
        }
        return false;
      }();
      if (!found)
        return false;
    }
    return true;
  }
} // namespace yave