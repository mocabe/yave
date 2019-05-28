//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/primitive.hpp>
#include <yave/node/objects/constructor.hpp>
#include <yave/core/rts/eval.hpp>
#include <yave/core/objects/string.hpp>

namespace yave {

  object_ptr<const Type> get_primitive_type(const primitive_t& v)
  {
    return std::visit(
      overloaded {[](const auto& a) {
        return object_type<Constructor<Box<std::decay_t<decltype(a)>>>>();
      }},
      v);
  }

  node_info get_primitive_info(const primitive_t& v)
  {
    return {{get_primitive_name(v)}, {}, {"value"}, true};
  }

  namespace {

    template <class T>
    struct PrimitiveGetter
      : Function<PrimitiveGetter<T>, PrimitiveContainer, Constructor<Box<T>>>
    {
      typename PrimitiveGetter::return_type code() const
      {
        auto container = PrimitiveGetter::template eval_arg<0>();
        auto prim      = container->get();
        if (auto v = std::get_if<T>(&prim)) {
          return make_object<Constructor<Box<T>>>(container);
        } else {
          return make_object<Constructor<Box<T>>>();
        }
      }
    };

  } // namespace

  bind_info get_primitive_bind_info(const primitive_t& v)
  {
    auto info = get_primitive_info(v);
    assert(info.output_sockets().size() == 1);

    return std::visit(
      overloaded {[&](const auto& p) {
        using prim_type = std::decay_t<decltype(p)>;
        return bind_info {info.name(),
                          {},
                          {info.output_sockets().front()},
                          make_object<PrimitiveGetter<prim_type>>(),
                          {info.name()},
                          true};
      }},
      v);
  }

  namespace {
    template <size_t N, class P, class R, class F>
    void primitive_list_gen(R& result, F& func)
    {
      result.emplace_back(
        func(make_primitive<std::variant_alternative_t<N, P>>()));
      if constexpr (N == 0) {
        return;
      } else {
        return primitive_list_gen<N - 1, P>(result, func);
      }
    }
  } // namespace

  std::vector<std::string> get_primitive_name_list()
  {
    std::vector<std::string> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_name);
    return ret;
  }

  std::vector<node_info> get_primitive_info_list()
  {
    std::vector<node_info> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_info);
    return ret;
  }

  std::vector<bind_info> get_primitive_bind_info_list()
  {
    std::vector<bind_info> ret;
    primitive_list_gen<std::variant_size_v<primitive_t> - 1, primitive_t>(
      ret, get_primitive_bind_info);
    return ret;
  }

} // namespace yave