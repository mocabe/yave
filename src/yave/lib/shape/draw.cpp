//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/draw.hpp>

#include <blend2d.h>

namespace yave {

  auto draw_shape_bgra8(
    const shape& s,
    const shape_draw_style& style,
    uint32_t width,
    uint32_t height) -> image
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

    BLPath p;

    auto apply_transform = [&](const glm::fvec2& v) {
      return glm::fvec2(s.transform() * glm::fvec3(v, 1.f));
    };

    // build path
    for (auto&& path : s.paths()) {
      for (size_t i = 0; i < path.size(); ++i) {
        auto vec = apply_transform(path.points()[i]);
        auto cmd = path.commands()[i];

        if (cmd == path_cmd::move)
          p.moveTo(vec.x, vec.y);

        if (cmd == path_cmd::close)
          p.close();

        if (cmd == path_cmd::line)
          p.lineTo(vec.x, vec.y);

        if (cmd == path_cmd::quad) {
          assert(path.commands().at(i + 1) == path_cmd::line);
          auto end = apply_transform(path.points()[++i]);
          p.quadTo(vec.x, vec.y, end.x, end.y);
        }

        if (cmd == path_cmd::cubic) {
          assert(path.commands().at(i + 1) == path_cmd::cubic);
          assert(path.commands().at(i + 2) == path_cmd::line);
          auto cp2 = apply_transform(path.points()[++i]);
          auto end = apply_transform(path.points()[++i]);
          p.cubicTo(vec.x, vec.y, cp2.x, cp2.y, end.x, end.y);
        }
      }
    }

    // fill
    ctx.setFillStyle(BLRgba(
      style.fill_color.r,
      style.fill_color.g,
      style.fill_color.b,
      style.fill_color.a));
    ctx.fillPath(p);

    // stroke
    ctx.setStrokeWidth(style.stroke_width);
    ctx.setStrokeStyle(BLRgba(
      style.stroke_color.r,
      style.stroke_color.g,
      style.stroke_color.b,
      style.stroke_color.a));
    ctx.strokePath(p);

    ctx.end();

    return ret;
  }

} // namespace yave