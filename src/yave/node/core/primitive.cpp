//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/primitive.hpp>
#include <yave/node/objects/constructor.hpp>
#include <yave/core/rts/eval.hpp>
#include <yave/core/objects/string.hpp>

namespace yave {

  primitive_container::primitive_container(const primitive_t& prim)
    : m_prim {prim}
  {
  }

  void primitive_container::set(const primitive_t& prim)
  {
    std::lock_guard lck {m_mtx};
    m_prim = prim;
  }

  primitive_t primitive_container::get() const
  {
    std::lock_guard lck {m_mtx};
    return m_prim;
  }

    object_ptr<> get_primitive_constructor(const primitive_t& v)
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

  node_info get_primitive_info(const primitive_t& v)
  {
    return {{get_primitive_name(v)}, {}, {"value"}, true};
  }

  namespace {
    // Primitive -> closure<Frame, T>
    struct PrimitiveGetterFunc
      : Function<PrimitiveGetterFunc, Primitive, Object>
    {
      PrimitiveGetterFunc(primitive_t refv)
        : reference_value {refv}
      {
      }

      return_type code() const
      {
        auto prim = *eval_arg<0>();
        if (prim.index() == reference_value.index())
          return get_primitive_constructor(*eval_arg<0>());
        else
          return get_primitive_constructor(reference_value);
      }

      primitive_t reference_value;
    };
  } // namespace

  bind_info get_primitive_bind_info(const primitive_t& v)
  {
    auto info = get_primitive_info(v);
    assert(info.output_sockets().size() == 1);
    return bind_info {info.name(),
                      {},
                      {info.output_sockets().front()},
                      make_object<PrimitiveGetterFunc>(v),
                      {info.name()},
                      true};
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