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
    auto code() const -> return_type
    {
      auto f     = eval_arg<0>();
      auto cthis = reinterpret_cast<const Closure<>*>(this);

      [[maybe_unused]] auto cf = reinterpret_cast<const Closure<>*>(f.get());
      assert(cf->arity != 0);

      auto& app = cthis->vertebrae(0);

      // build self referencing apply node
      _get_storage(*app).app() = std::move(f);
      _get_storage(*app).arg() = app;

      // avoid memory leak
      _get_storage(app).decrement_refcount();

      return static_object_cast<const VarValueProxy<detail::Fix_X>>(
        object_ptr<const Object>(app));
    }

    auto _self_update(object_ptr<const Object> result) const noexcept
      -> object_ptr<const Object>
    {
      // bypass
      return result;
    }
  };
} // namespace yave