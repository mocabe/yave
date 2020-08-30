//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/view_context.hpp>
#include <yave/wm/viewport.hpp>
#include <yave/wm/root_window.hpp>
#include <yave/wm/float_layout.hpp>
#include <yave/wm/render_context.hpp>
#include <yave/wm/draw_event.hpp>
#include <yave/wm/draw.hpp>

#include <catch2/catch.hpp>
#include <iostream>

using namespace yave;

class test_window : public wm::window
{
  bool m_hovered = false;
  bool m_pressed = false;
  bool m_draggin = false;

  glm::vec2 m_drag_relpos;
  glm::vec4 m_col;

public:
  test_window(glm::vec2 pos, glm::vec2 size, glm::vec4 col)
    : window("test_window", pos, size)
    , m_col {col}
  {
  }

  void update(editor::data_context& data_ctx, editor::view_context& view_ctx)
    override
  {
    auto& wm = view_ctx.window_manager();

    if (m_draggin) {
      auto vp = wm.get_viewport(this)->as<wm::viewport>();
      set_pos(vp->io().mouse_pos() + m_drag_relpos);
    }
    m_draggin = false;
  }

  void draw(
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const override
  {
    assert(false);
  }

  void on_mouse_move(
    wm::events::mouse_move& e,
    const editor::data_context&,
    const editor::view_context& vctx) const override
  {
    e.accept();

    auto& wm    = vctx.window_manager();
    auto relpos = *wm.screen_pos(this) - e.pos();

    vctx.push(editor::make_window_view_command(*this, [relpos](auto& self) {
      if (self.m_pressed) {
        self.m_draggin     = true;
        self.m_drag_relpos = relpos;
      }
    }));
  }

  void on_mouse_enter(
    wm::events::mouse_enter& e,
    const editor::data_context&,
    const editor::view_context& vctx) const override
  {
    vctx.push(editor::make_window_view_command(
      *this, [](auto& self) { self.m_hovered = true; }));
  }

  void on_mouse_leave(
    wm::events::mouse_leave& e,
    const editor::data_context&,
    const editor::view_context& vctx) const override
  {
    vctx.push(editor::make_window_view_command(*this, [](auto& self) {
      self.m_hovered = false;
      self.m_pressed = false;
    }));
  }

  void on_mouse_press(
    wm::events::mouse_press& e,
    const editor::data_context&,
    const editor::view_context& vctx) const
  {
    e.accept();

    vctx.push(editor::make_window_view_command(
      *this, [&wm = vctx.window_manager()](auto& self) {
        self.m_pressed = true;
        self.parent()
          ->template as<wm::float_layout>() //
          ->move_window_back(self.id());    //
      }));
  }

  void on_mouse_release(
    wm::events::mouse_release& e,
    const editor::data_context&,
    const editor::view_context& vctx) const
  {
    e.accept();

    vctx.push(editor::make_window_view_command(
      *this, [](auto& self) { self.m_pressed = false; }));
  }

  void on_draw(
    wm::events::draw& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const override
  {
    auto& ctx = e.render_context();
    auto& wm  = view_ctx.window_manager();

    auto screen_pos = *wm.screen_pos(this);

    wm::draw_list dl;

    wm::dl_fill_rounded_rect(
      dl,
      screen_pos,
      screen_pos + size(),
      5.f,
      {m_col.x, m_col.y, m_col.z, m_pressed ? 0.8f : 0.3f},
      {screen_pos, size()},
      ctx.default_tex());

    if (m_hovered) {
      wm::dl_draw_rounded_rect(
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
  editor::data_context dctx;
  editor::view_context vctx {dctx};

  glfw::glfw_context glfw;
  vulkan::vulkan_context vk;

  auto& wm = vctx.window_manager();

  auto root = wm.root();
  auto vp   = root
              ->add_viewport(std::make_unique<wm::viewport>(
                wm, vk, glfw.create_window(1280, 720, "🎆")))
              ->as<wm::viewport>();

  wm.set_key_focus(vp);

  REQUIRE(wm.get_key_focus() == vp);
  REQUIRE(wm.get_window(root->id()) == root);
  REQUIRE(wm.get_window(vp->id()) == vp);
  REQUIRE(!wm.get_window(uid()));
  REQUIRE(!wm.get_viewport(root->id()));
  REQUIRE(!wm.get_viewport(uid()));
  REQUIRE(wm.get_viewport(vp->id()) == vp);
  REQUIRE(wm.is_child(vp, root));
  REQUIRE(wm.is_parent(root, vp));
  REQUIRE(!wm.is_child(nullptr, root));
  REQUIRE(!wm.is_parent(nullptr, vp));
  REQUIRE(!vp->layout());
  REQUIRE(vp->modals().empty());
  REQUIRE(!vp->modal(uid()));

  auto layout = vp->set_layout(std::make_unique<wm::float_layout>());
  auto testw  = layout->add_window(std::make_unique<test_window>(
    glm::vec2(0, 0), glm::vec2(300, 300), glm::vec4(1, 0, 0, 0)));

  auto testw1 = layout->add_window(std::make_unique<test_window>(
    glm::vec2(200, 200), glm::vec2(400, 250), glm::vec4(0, 1, 0, 0)));

  auto testw2 = layout->add_window(std::make_unique<test_window>(
    glm::vec2(150, 150), glm::vec2(200, 200), glm::vec4(0, 0, 1, 0)));

  REQUIRE(wm.get_viewport(testw->id()));
  REQUIRE(wm.is_child(testw, root));
  REQUIRE(wm.is_parent(root, testw));

  vctx.run();
}