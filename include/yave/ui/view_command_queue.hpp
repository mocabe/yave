//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/view_context.hpp>

#include <thread>
#include <mutex>
#include <queue>
#include <array>
#include <optional>

namespace yave::ui {

  /// View command queue
  class view_command_queue
  {
    view_context& m_vctx;
    std::mutex cmd_mtx;
    std::condition_variable cmd_cond;
    std::array<std::queue<view_command>, 2> cmd_queues;
    size_t front_queue_idx = 0;

    auto lock_queues()
    {
      return std::unique_lock(cmd_mtx);
    }

    auto& cmd_queue()
    {
      return cmd_queues[front_queue_idx];
    }

    auto& cmd_queue_delay()
    {
      return cmd_queues[(front_queue_idx + 1) % cmd_queues.size()];
    }

    auto swap_queue()
    {
      assert(cmd_queue().empty());
      front_queue_idx = (front_queue_idx + 1) % cmd_queues.size();
    }

    auto pop_cmd() -> std::optional<view_command>
    {
      auto lck = lock_queues();
      if (!cmd_queue().empty()) {
        auto ret = std::move(cmd_queue().front());
        cmd_queue().pop();
        return ret;
      }
      return std::nullopt;
    }

    auto pop_cmd_or_swap() -> std::optional<view_command>
    {
      auto lck = lock_queues();
      if (!cmd_queue().empty()) {
        auto ret = std::move(cmd_queue().front());
        cmd_queue().pop();
        return ret;
      }
      swap_queue();
      return std::nullopt;
    }

  public:
    /// Init command queue
    view_command_queue(view_context& vctx)
      : m_vctx {vctx}
    {
    }

    /// Execute all commands in queue
    void exec_all()
    {
      while (auto cmd = pop_cmd()) {
        cmd->exec(m_vctx);
      }
    }

    /// Execute all commands in queue, then swap command buffer
    void exec_all_then_swap()
    {
      while (auto cmd = pop_cmd_or_swap()) {
        cmd->exec(m_vctx);
      }
    }

    /// Post command to primary buffer
    void post(view_command op)
    {
      auto lck = lock_queues();
      cmd_queue().push(std::move(op));
      cmd_cond.notify_one();
    }

    /// Post command to secondary buffer
    void post_delay(view_command op)
    {
      auto lck = lock_queues();
      cmd_queue_delay().push(std::move(op));
      cmd_cond.notify_one();
    }

    /// Post empty command
    void wake()
    {
      post(view_command([](auto&) {}));
    }

    /// Wait next command
    void wait()
    {
      auto lck = lock_queues();
      cmd_cond.wait(lck, [&] { return !cmd_queue().empty(); });
    }
  };

} // namespace yave::ui