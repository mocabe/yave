//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/draw.hpp>
#include <yave/core/overloaded.hpp>

#include <blend2d.h>

namespace yave {

  auto draw_shape_bgra8(const shape& s, uint32_t width, uint32_t height)
    -> image
  {
    // image data
    auto ret = image(width, height, image_format::rgba8);

    // non-owning BGRA image
    BLImage img;
    img.createFromData(
      width,
      height,
      BL_FORMAT_PRGB32,
      ret.data(),
      ret.stride(),
      nullptr,
      nullptr);

    BLContext ctx(img);

    std::vector<BLPath> ps;
    ps.reserve(s.paths().size());

    // build path
    for (auto&& path : s.paths()) {

      auto& p = ps.emplace_back();

      for (size_t i = 0; i < path.size(); ++i) {
        auto vec = path.points()[i];
        auto cmd = path.commands()[i];

        if (cmd == path_cmd::move)
          p.moveTo(vec.x, vec.y);

        if (cmd == path_cmd::close)
          p.close();

        if (cmd == path_cmd::line)
          p.lineTo(vec.x, vec.y);

        if (cmd == path_cmd::quad) {
          assert(path.commands().at(i + 1) == path_cmd::line);
          auto end = path.points()[++i];
          p.quadTo(vec.x, vec.y, end.x, end.y);
        }

        if (cmd == path_cmd::cubic) {
          assert(path.commands().at(i + 1) == path_cmd::cubic);
          assert(path.commands().at(i + 2) == path_cmd::line);
          auto cp2 = path.points()[++i];
          auto end = path.points()[++i];
          p.cubicTo(vec.x, vec.y, cp2.x, cp2.y, end.x, end.y);
        }
      }
    }

    // draw paths
    for (auto&& cmd : s.commands()) {

      auto& p = ps[cmd.path_idx];

      ctx.save();

      std::visit(
        overloaded {
          // fill
          [&](const shape_op_fill& fill) {
            ctx.setFillStyle(
              BLRgba(fill.color.r, fill.color.g, fill.color.b, fill.color.a));
            ctx.fillPath(p);
          },
          // stroke
          [&](const shape_op_stroke& stroke) {
            ctx.setStrokeStyle(BLRgba(
              stroke.color.r, stroke.color.g, stroke.color.b, stroke.color.a));
            ctx.setStrokeWidth(stroke.width);
            ctx.strokePath(p);
          }},
        cmd.op);

      ctx.restore();
    }

    ctx.end();

    return ret;
  }

} // namespace yave