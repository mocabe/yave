//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/window_traverser.hpp>
#include <yave/wm/render_context.hpp>

namespace yave::wm {

  namespace events {

    /// draw event
    class draw final : public event
    {
      wm::render_context& m_render_ctx;
      glm::vec2 m_clip_pos, m_clip_ext;

      using event::accept;
      using event::accepted;

    public:
      draw(wm::render_context& rctx, glm::vec2 clip_pos, glm::vec2 clip_ext)
        : event::event()
        , m_render_ctx {rctx}
        , m_clip_pos {clip_pos}
        , m_clip_ext {clip_ext}
      {
      }

      /// window clip rect pos
      auto& clip_pos() const
      {
        return m_clip_pos;
      }

      /// window clip rect extent
      auto& clip_ext() const
      {
        return m_clip_ext;
      }

      auto& render_context()
      {
        return m_render_ctx;
      }
    };

  } // namespace events

  class draw_event_dispatcher : event_dispatcher
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    draw_event_dispatcher(
      /*       */ render_context& rctx,
      const editor::data_context& dctx,
      const editor::view_context& vctx);

    ~draw_event_dispatcher() noexcept;

    void dispatch(window* root) override;
  };
}