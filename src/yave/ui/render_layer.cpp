//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/render_layer.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/render_scope.hpp>

#include <limits>

namespace {

  using namespace yave::ui;

  auto unbounded_clip_rect()
  {
    auto max = std::numeric_limits<f64>::max();
    auto min = std::numeric_limits<f64>::lowest();
    return rect(vec(min, min), size(max, max));
  }

  auto to_draw_scissor(const rect& cr)
  {
    return draw_scissor {
      .offset = {cr.pos().x, cr.pos().y}, .extent = {cr.size().w, cr.size().h}};
  }

  auto to_vec2(const vec& v)
  {
    return glm::vec2(v.x, v.y);
  }

  auto to_vec4(const color& col)
  {
    return glm::vec4(col.r, col.g, col.b, col.a);
  }

} // namespace

namespace yave::ui {

  class render_layer::priv
  {
  public:
    static auto& current_draw_list(render_layer& self)
    {
      if (self.m_lists.lists().empty()) {
        self.m_lists.append_list(draw_list());
      }
      return self.m_lists.lists().back();
    }
  };

  render_layer::render_layer(const render_scope& scope)
    : m_rctx {scope.render_ctx()}
    , m_clip_rects {unbounded_clip_rect()}
  {
    rebind_scope(scope);
  }

  render_layer::render_layer(render_layer&& other) noexcept
    : m_rctx {other.m_rctx}
    , m_lists {std::move(other.m_lists)}
    , m_clip_rects {std::move(other.m_clip_rects)}
  {
  }

  render_layer& render_layer::operator=(render_layer&& other) noexcept
  {
    auto tmp = std::move(other);
    swap(tmp);
    return *this;
  }

  void render_layer::swap(render_layer& other) noexcept
  {
    assert(std::addressof(m_rctx) == std::addressof(other.m_rctx));
    m_lists.swap(other.m_lists);
    m_clip_rects.swap(other.m_clip_rects);
  }

  void render_layer::rebind_scope(const render_scope& scope)
  {
    m_cursor_pos = scope.window_pos();
    m_clip_rects.clear();
    m_clip_rects.emplace_back(scope.window_pos(), scope.window_size());
  }

  auto render_layer::cursor_pos() const -> vec
  {
    return m_cursor_pos;
  }

  void render_layer::set_cursor_pos(const vec& cursor_pos)
  {
    m_cursor_pos = cursor_pos;
  }

  void render_layer::push_clip_rect(const rect& rec)
  {
    assert(!m_clip_rects.empty());
    const auto& clip = m_clip_rects.front();

    if (auto isect = rect::intersect(clip, rec))
      m_clip_rects.push_back(*isect);
    else
      m_clip_rects.emplace_back();
  }

  void render_layer::pop_clip_rect()
  {
    if (m_clip_rects.size() > 1)
      m_clip_rects.pop_back();
  }

  auto render_layer::clip_rect() const -> rect
  {
    assert(!m_clip_rects.empty());
    return m_clip_rects.front();
  }

  void render_layer::append(render_layer&& other)
  {
    m_lists.reserve_lists(
      m_lists.lists().size() + other.m_lists.lists().size());

    for (auto&& list : other.m_lists.lists())
      m_lists.append_list(std::move(list));
  }

  void render_layer::prepend(render_layer&& other)
  {
    m_lists.reserve_lists(
      m_lists.lists().size() + other.m_lists.lists().size());

    for (auto&& list : other.m_lists.lists())
      m_lists.prepend_list(std::move(list));
  }

  void render_layer::draw_line(
    const vec& p1,
    const vec& p2,
    const f32& width,
    const color& col)
  {
    auto& list = priv::current_draw_list(*this);
    auto clip  = clip_rect();

    list.draw_line(
      to_vec2(m_cursor_pos + p1),
      to_vec2(m_cursor_pos + p2),
      std::max(0.f, width),
      to_vec4(col),
      to_draw_scissor(clip),
      m_rctx.default_texture());
  }

  void render_layer::draw_rect(
    const rect& rect,
    const f32& width,
    const color& col)
  {
    auto& list = priv::current_draw_list(*this);
    auto clip  = clip_rect();

    auto p1 = rect.pos();
    auto p2 = p1 + rect.size().vec();

    list.draw_rect(
      to_vec2(m_cursor_pos + p1),
      to_vec2(m_cursor_pos + p2),
      std::max(0.f, width),
      to_vec4(col),
      to_draw_scissor(clip),
      m_rctx.default_texture());
  }

  auto render_layer::draw_lists(passkey<viewport_renderer>) const
    -> const ui::draw_lists&
  {
    return m_lists;
  }

} // namespace yave::ui