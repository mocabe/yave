#include <yave/ui/view_context.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/root_window.hpp>
#include <yave/ui/float_layout.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/draw_event.hpp>
#include <yave/ui/draw.hpp>

#include <glm/glm.hpp>
#include <catch2/catch.hpp>
#include <iostream>

using namespace yave;

class test_window : public ui::window
{
  bool m_hovered = false;
  bool m_pressed = false;
  bool m_draggin = false;

  glm::vec2 m_drag_relpos;
  glm::vec4 m_col;

public:
  test_window(glm::vec4 col)
    : window(u8"test_window")
    , m_col {col}
  {
  }

  void draw(const ui::data_context& data_ctx, const ui::view_context& view_ctx)
    const override
  {
    assert(false);
  }

  void update()
  {
    if (m_draggin) {
      auto& wm = window_manager();
      auto vp  = wm.get_viewport(this);
      set_pos(vp->io().mouse_pos() + m_drag_relpos);
    }
    m_draggin = false;
  }

  void on_update(
    ui::events::update& e,
    const ui::data_context&,
    const ui::view_context& vctx) const override
  {
    auto& wm = vctx.window_manager();

    vctx.pus(
      ui::make_window_view_command(*this, [](auto& self) { self.update(); }));
  }

  void on_mouse_move(
    ui::events::mouse_move& e,
    const ui::data_context&,
    const ui::view_context& vctx) const override
  {
    e.accept();

    auto& ui    = vctx.window_manager();
    auto relpos = *ui.screen_pos(this) - e.pos();

    vctx.push(ui::make_window_view_command(*this, [relpos](auto& self) {
      if (self.m_pressed) {
        self.m_draggin     = true;
        self.m_drag_relpos = relpos;
      }
    }));
  }

  void on_mouse_enter(
    ui::events::mouse_enter& e,
    const ui::data_context&,
    const ui::view_context& vctx) const override
  {
    vctx.push(ui::make_window_view_command(
      *this, [](auto& self) { self.m_hovered = true; }));
  }

  void on_mouse_leave(
    ui::events::mouse_leave& e,
    const ui::data_context&,
    const ui::view_context& vctx) const override
  {
    vctx.push(ui::make_window_view_command(*this, [](auto& self) {
      self.m_hovered = false;
      self.m_pressed = false;
    }));
  }

  void on_mouse_press(
    ui::events::mouse_press& e,
    const ui::data_context&,
    const ui::view_context& vctx) const
  {
    e.accept();

    vctx.push(ui::make_window_view_command(
      *this, [&ui = vctx.window_manager()](auto& self) {
        self.m_pressed = true;
        self.parent()
          ->template as<ui::float_layout>() //
          ->move_window_back(self.id());    //
      }));
  }

  void on_mouse_release(
    ui::events::mouse_release& e,
    const ui::data_context&,
    const ui::view_context& vctx) const
  {
    e.accept();

    vctx.push(ui::make_window_view_command(
      *this, [](auto& self) { self.m_pressed = false; }));
  }

  void on_draw(
    ui::events::draw& e,
    const ui::data_context& data_ctx,
    const ui::view_context& view_ctx) const override
  {
    auto& ctx = e.render_context();
    auto& ui  = view_ctx.window_manager();

    auto screen_pos = *ui.screen_pos(this);

    ui::draw_list dl;

    ui::dl_fill_rounded_rect(
      dl,
      screen_pos,
      screen_pos + size(),
      5.f,
      {m_col.x, m_col.y, m_col.z, m_pressed ? 0.8f : 0.3f},
      {screen_pos, size()},
      ctx.default_tex());

    if (m_hovered) {
      ui::dl_draw_rounded_rect(
        dl,
        screen_pos,
        screen_pos + size(),
        1.f,
        5.f,
        {1.f, 1.f, 1.f, 1.f},
        {screen_pos, size()},
        ctx.default_tex());
    }

    ctx.add_draw_list(std::move(dl));
  }
};

TEST_CASE("viewport")
{
  ui::data_context dctx;
  @ @-25, 7 + 166, 7 @ @TEST_CASE("viewport") auto root = ui.root();
  auto vp   = root
              ->add_viewport(std::make_unique<ui::viewport>(
                ui, vk, glfw.create_window(1280, 720, "test window")))
                ui, vk, glfw.create_window(1280, 720, "🎆")))
              ->as<ui::viewport>();

  ui.set_key_focus(vp);
  @ @-37, 9 + 178,
    27 @ @TEST_CASE("viewport") REQUIRE(!ui.get_viewport(root->id()));
  REQUIRE(!ui.get_viewport(uid()));
  REQUIRE(ui.get_viewport(vp->id()) == vp);
  REQUIRE(ui.is_child(vp, root));
  REQUIRE(ui.is_parent(root, vp));
  REQUIRE(!ui.is_child(nullptr, root));
  REQUIRE(!ui.is_parent(nullptr, vp));
  REQUIRE(!vp->layout());
  REQUIRE(vp->modals().empty());
  REQUIRE(!vp->modal(uid()));

  auto layout = vp->set_layout(std::make_unique<ui::float_layout>());
  auto testw  = layout->add_window(std::make_unique<test_window>(
    glm::vec2(0, 0), glm::vec2(300, 300), glm::vec4(1, 0, 0, 0)));

  auto testw1 = layout->add_window(std::make_unique<test_window>(
    glm::vec2(200, 200), glm::vec2(400, 250), glm::vec4(0, 1, 0, 0)));

  auto testw2 = layout->add_window(std::make_unique<test_window>(
    glm::vec2(150, 150), glm::vec2(200, 200), glm::vec4(0, 0, 1, 0)));

  REQUIRE(ui.get_viewport(testw->id()));
  REQUIRE(ui.is_child(testw, root));
  REQUIRE(ui.is_parent(root, testw));

  vctx.run();
}