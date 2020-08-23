//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/draw_event.hpp>

#include <vector>
#include <glm/glm.hpp>

namespace yave::wm {

  namespace {

    struct clip_rect
    {
      glm::vec2 pos;
      glm::vec2 ext;
    };
  } // namespace

  class draw_event_dispatcher::impl
  {
  public:
    // contexts
    /*       */ render_context& rctx;
    const editor::data_context& dctx;
    const editor::view_context& vctx;

    // clip rect pos
    std::vector<clip_rect> clip_stack;
    // window pos stack
    std::vector<glm::vec2> pos_stack;

  public:
    impl(
      /*       */ render_context& rctx,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
      : rctx {rctx}
      , dctx {dctx}
      , vctx {vctx}
    {
    }

    void dispatch(window* root)
    {
      auto rec = [&](auto&& self, const window* w) -> void {
        // root window
        if (clip_stack.empty()) {
          clip_stack.push_back({.pos = {0, 0}, .ext = w->size()});
          pos_stack.push_back({0, 0});
        }
        // child window
        else {
          pos_stack.push_back(pos_stack.back() + w->pos());
          auto pos = max(pos_stack.back(), clip_stack.back().pos);
          auto ext = min(pos_stack.back() + w->size(), clip_stack.back().ext);
          clip_stack.push_back({.pos = pos, .ext = ext});
        }

        // event
        {
          auto e =
            events::draw(rctx, clip_stack.back().pos, clip_stack.back().ext);
          w->event(e, dctx, vctx);
        }

        // children
        for (auto&& c : w->children()) {
          self(c);
        }

        pos_stack.pop_back();
        clip_stack.pop_back();
      };

      auto impl = fix_lambda(rec);
      impl(root);
    }
  };

  draw_event_dispatcher::draw_event_dispatcher(
    /*       */ render_context& rctx,
    const editor::data_context& dctx,
    const editor::view_context& vctx)
    : m_pimpl {std::make_unique<impl>(rctx, dctx, vctx)}
  {
  }

  draw_event_dispatcher::~draw_event_dispatcher() noexcept = default;

  void draw_event_dispatcher::dispatch(window* w)
  {
    return m_pimpl->dispatch(w);
  }
} // namespace yave::wm
