//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/type_gen.hpp>
#include <yave/rts/function.hpp>
#include <yave/rts/eval.hpp>

namespace yave {

  namespace detail {
    class Fix_X;
  } // namespace detail

  /// Implementation of Y using self referencing apply node.
  // fix: (a -> a) -> a.
  // fix f = let x = f x in x
  struct Fix
    : Function<Fix, closure<detail::Fix_X, detail::Fix_X>, detail::Fix_X>
  {
    return_type code() const
    {
      // reduce argument closure
      auto f  = eval_arg<0>();
      auto cf = reinterpret_cast<const Closure<>*>(f.get());

      assert(has_arrow_type(f));

      // check arity for safety
      if (unlikely(cf->arity == 0))
        throw eval_error::bad_fix();

      auto ret = [&]() -> object_ptr<const Object> {
        // create return value
        auto pap   = clone(f);
        auto cpap  = reinterpret_cast<const Closure<>*>(pap.get());
        auto cthis = reinterpret_cast<const Closure<>*>(this);

        auto& app = cthis->vertebrae(0);

        // build self-referencing closure
        auto arity             = --cpap->arity;
        cpap->vertebrae(arity) = app;

        // avoid memory leak
        _get_storage(app).decrement_refcount();

        // eval
        if (unlikely(arity == 0))
          return cpap->call();

        return pap;
      }();

      // return
      return static_object_cast<const VarValueProxy<detail::Fix_X>>(
        std::move(ret));
    }

    auto _self_update(object_ptr<const Object> result) const noexcept
      -> object_ptr<const Object>
    {
      // set cache
      auto cthis = reinterpret_cast<const Closure<>*>(this);
      auto& app  = cthis->vertebrae(0);
      _get_storage(*app).set_result(std::move(result));

      // return @
      return app;
    }
  };
} // namespace yave