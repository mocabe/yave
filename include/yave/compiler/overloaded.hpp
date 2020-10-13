//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/type_value.hpp>

namespace yave::compiler {

  struct overloaded_object_value
  {
    overloaded_object_value(
      uint64_t id = tvar_type::random_generate().id) noexcept
      : id {id}
      , id_var {make_object<Type>(tvar_type {id})}
    {
    }

    uint64_t id;
    object_ptr<const Type> id_var;
  };

  /// Overloaded node
  using Overloaded = Box<overloaded_object_value>;

} // namespace yave::compiler

YAVE_DECL_TYPE(yave::compiler::Overloaded, "a1d282e3-e68e-49ce-ab8f-afe90cfe7a78");