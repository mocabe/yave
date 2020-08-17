//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/support/id.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <typeinfo>

// fwd
namespace yave::editor {
  class data_context;
  class view_context;
} // namespace yave::editor

// fwd
namespace yave::wm::events {
  class resize;
  class move;
  class draw;
  class mouse_click;
  class mouse_double_click;
  class mouse_press;
  class mouse_release;
  class mouse_hover;
  class key_press;
  class key_release;
  class key_char;
} // namespace yave::wm::events

namespace yave::wm {

  class window_manager;
  class root_window;

  /// Base class of GUI window and widgets
  class window
  {
    /// unique id for this window
    uid m_id;
    /// pointer to parent
    window* m_parent;
    /// child windows
    std::vector<std::unique_ptr<const window>> m_children;
    /// name
    std::string m_name;
    /// window size
    glm::vec2 m_pos, m_size;

  public:
    /// ctor (legacy)
    window(std::string name);
    /// ctor
    window(std::string name, glm::vec2 pos, glm::vec2 size);
    /// dtor
    virtual ~window() noexcept;

  protected:
    /// utility function to add new window
    /// \param idx position of insertion
    /// \param win window to insert
    void add_any_window(size_t idx, std::unique_ptr<window>&& win);

    /// utility function to detach child window
    auto detach_any_window(uid id) -> std::unique_ptr<window>;

    /// utility function to remove child window
    void remove_any_window(uid id);

  public:
    /// update view model
    virtual void update(
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) = 0;
    /// draw content
    virtual void draw(
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const = 0;

  public:
    /// get mutable child window pointer
    auto as_mut_child(const std::unique_ptr<const window>& win)
    {
      return const_cast<window*>(win.get());
    }

  public: /* public accessors */
    auto parent() const -> window*
    {
      return m_parent;
    }

    auto& id() const
    {
      return m_id;
    }

    auto& name() const
    {
      return m_name;
    }

    auto& children() const
    {
      return m_children;
    }

    auto& pos() const
    {
      return m_pos;
    }

    auto& size() const
    {
      return m_size;
    }

    void set_name(std::string name)
    {
      m_name = std::move(name);
    }

    void set_pos(glm::vec2 pos)
    {
      m_pos = pos;
    }

    void set_size(glm::vec2 size)
    {
      m_size = size;
    }

  public:
    /// dynamic casting
    template <class Derived>
    auto as()
    {
      static_assert(std::is_base_of_v<window, Derived>);
      assert(typeid(*this) == typeid(Derived));
      return static_cast<Derived*>(this);
    }

    /// dynamic casting
    template <class Derived>
    auto as() const
    {
      static_assert(std::is_base_of_v<window, Derived>);
      assert(typeid(*this) == typeid(Derived));
      return static_cast<const Derived*>(this);
    }

  public: /* event handlers */
    /// handle events
    void event(
      wm::event& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// resize
    virtual void on_resize(
      wm::events::resize& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// move
    virtual void on_move(
      wm::events::move& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// draw
    virtual void on_draw(
      wm::events::draw& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// button press
    virtual void on_mouse_click(
      wm::events::mouse_click& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// button double press
    virtual void on_mouse_double_click(
      wm::events::mouse_double_click& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// button press
    virtual void on_mouse_press(
      wm::events::mouse_press& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// button release
    virtual void on_mouse_release(
      wm::events::mouse_release& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// hover
    virtual void on_mouse_hover(
      wm::events::mouse_hover& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// key press
    virtual void on_key_press(
      wm::events::key_press& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// key release
    virtual void on_key_release(
      wm::events::key_release& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// key char input
    virtual void on_key_char(
      wm::events::key_char& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;

    /// custom event
    virtual void on_custom_event(
      wm::event& e,
      const editor::data_context& data_ctx,
      const editor::view_context& view_ctx) const;
  };

} // namespace yave::wm