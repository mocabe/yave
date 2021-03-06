//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <functional>
#include <type_traits>
#include <cassert>
#include <concepts>

namespace yave::ui {

  /// Non-copyable function wrapper which operates over data/view context.
  template <class Context>
  class command
  {
    // Command is designed to be non-copyable, so we want to store non-copyable
    // callables too (so we can store lambdas which captures promise<T>, etc.),
    // but since std::function requires copyable type, we need to use fake
    // wrapper class which looks copyable.
    template <class F>
    struct fake_copyable
    {
      F f;

      fake_copyable() = default;

      fake_copyable(fake_copyable&&) noexcept = default;

      fake_copyable(F&& f)
        : f {std::forward<F>(f)}
      {
      }

      fake_copyable(const fake_copyable& other)
        : f {[&] {
          assert(false);
          // this will not be actually called, but without this F needs to be
          // default constructible (maybe use union instead?).
          return std::move(const_cast<F&>(other.f));
        }()}
      {
      }

      template <typename... Args>
      void operator()(Args&&... args)
      {
        std::invoke(f, std::forward<Args>(args)...);
      }
    };

    std::function<void(Context&)> m_fun;

  public:
    command()                   = delete;
    command(const command&)     = delete;
    command(command&&) noexcept = default;
    command& operator=(const command&) = delete;
    command& operator=(command&&) noexcept = default;

  public:
    template <std::invocable<Context&> F>
    command(F&& f)
      : m_fun {fake_copyable(std::forward<F>(f))}
    {
    }

    void exec(Context& ctx)
    {
      m_fun(ctx);
    }
  };

} // namespace yave::ui