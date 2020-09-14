//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/shape/path.hpp>
#include <yave/lib/shape/shape.hpp>
#include <yave/obj/shape/shape_op.hpp>

namespace yave::data {

  /// shape command data
  struct shape_cmd
  {
    /// index of path
    uint64_t path_idx = 0;
    /// shape op
    object_ptr<ShapeOp> op;

  public:
    shape_cmd(const yave::shape_cmd& cmd)
      : path_idx {cmd.path_idx}
      , op {make_object<ShapeOp>(cmd.op)}
    {
    }

    operator yave::shape_cmd() const
    {
      assert(op);
      return {path_idx, yave::shape_op(*op)};
    }
  };

} // namespace yave::data