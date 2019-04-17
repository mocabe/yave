//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/primitive.hpp>
#include <yave/func/constructor.hpp>

namespace yave {

  object_ptr<> get_primitive_instance(const primitive_t& v)
  {
    return std::visit(
      overloaded {[](const auto& a) -> object_ptr<> {
        return make_object<Constructor<Box<std::decay_t<decltype(a)>>>>(a);
      }},
      v);
  }

  object_ptr<const Type> get_primitive_type(const primitive_t& v)
  {
    return std::visit(
      overloaded {[](const auto& a) {
        return object_type<Constructor<Box<std::decay_t<decltype(a)>>>>();
      }},
      v);
  }

  NodeInfo get_primitive_info(const primitive_t& v)
  {
    return {{get_primitive_name(v)}, {}, {"value"}, true};
  }

  BindInfo get_primitive_bind_info(const primitive_t& v)
  {
    auto info = get_primitive_info(v);
    assert(info.output_sockets().size() == 1);
    return BindInfo {info.name(),
                     {},
                     {info.output_sockets().front()},
                     [](auto prim) { return get_primitive_instance(prim); },
                     {info.name()},
                     true};
  }

  std::vector<std::string> get_primitive_name_list()
  {
    std::vector<std::string> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_name);
    return ret;
  }

  std::vector<NodeInfo> get_primitive_info_list()
  {
    std::vector<NodeInfo> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_info);
    return ret;
  }

  std::vector<BindInfo> get_primitive_bind_info_list()
  {
    std::vector<BindInfo> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_bind_info);
    return ret;
  }

} // namespace yave