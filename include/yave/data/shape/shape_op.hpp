//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/shape/path.hpp>
#include <yave/lib/shape/shape.hpp>
#include <yave/obj/shape/path.hpp>

#include <yave/support/overloaded.hpp>

namespace yave::data {

  // fill
  struct shape_op_fill
  {
    /// id
    static constexpr uint64_t id = 1;
    /// color
    vec4 color;
  };

  // stroke
  struct shape_op_stroke
  {
    /// id
    static constexpr uint64_t id = 2;
    /// color
    vec4 color;
    /// width
    double width = 0.f;
  };

  /// shape operation
  struct shape_op
  {
    //// id of current op
    uint64_t id = 0;
    /// data
    union
    {
      shape_op_fill fill;
      shape_op_stroke stroke;
    };

  public:
    shape_op(const yave::shape_op& op)
    {
      std::visit(
        overloaded {
          [&](const yave::shape_op_fill& op) {
            id   = data::shape_op_fill::id;
            fill = {.color = op.color};
          },
          [&](const yave::shape_op_stroke& op) {
            id     = data::shape_op_stroke::id;
            stroke = {.color = op.color, .width = op.width};
          }},
        op);
    }

    operator yave::shape_op() const
    {
      switch (id) {
        case data::shape_op_fill::id:
          return yave::shape_op_fill {.color = fill.color};
        case data::shape_op_stroke::id:
          return yave::shape_op_stroke {
            .color = stroke.color, .width = static_cast<float>(stroke.width)};
      }
      unreachable();
    }
  };
} // namespace yave::data