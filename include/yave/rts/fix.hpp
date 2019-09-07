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
      auto f = eval_arg<0>();
      auto c = reinterpret_cast<const Closure<>*>(_get_storage(f).get());

      if (unlikely(c->arity == 0)) {
        throw eval_error::bad_fix();
      }

      // self referencing apply node; app=@(f, app)
      auto app = make_object<Apply>(std::move(f), /*app*/ nullptr);
      _get_storage(*app).arg() = app;

      // avoid memory leak
      _get_storage(app).decrement_refcount();

      return static_object_cast<const VarValueProxy<detail::Fix_X>>(
        std::move(app));
    }
  };
} // namespace yave