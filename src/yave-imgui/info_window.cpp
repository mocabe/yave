//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/info_window.hpp>
#include <yave-imgui/data_commands.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/editor/data_command.hpp>

namespace yave::editor::imgui {

  info_window::info_window()
    : window("info_window")
  {
  }

  void info_window::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    auto lck       = dctx.get_data<editor_data>();
    auto& scene    = lck.ref().scene_config();
    auto& executor = lck.ref().executor_data();

    m_arg_time     = executor.arg_time();
    m_arg_time_min = executor.loop_range_min();
    m_arg_time_max = executor.loop_range_max();
    m_continuous   = executor.continuous_execution();

    m_width  = scene.width();
    m_height = scene.height();
    m_fps    = scene.frame_rate();
    m_format = scene.frame_format();
  }

  void info_window::draw(
    const editor::data_context& dctx,
    const editor::view_context& vctx) const
  {
    ImGui::Begin(name().c_str());
    {
      ImGui::Text("t=%lf", m_arg_time.seconds().count());
      ImGui::Text("scene: w=%d, h=%d", m_width, m_height);
      ImGui::Text("format: %s", to_string(m_format).c_str());
      ImGui::Text("fps: %d", m_fps);

      ImGui::Separator();

      // play
      auto cont = m_continuous;
      if (ImGui::Checkbox("play", &cont)) {

        dctx.cmd(make_data_command([=](data_context& ctx) {
          auto lck = ctx.get_data<editor_data>();
          lck.ref().executor_data().set_continuous_execution(cont);
        }));

        if (cont) {
          dctx.cmd(std::make_unique<dcmd_notify_execute>());
        }
      }

      ImGui::SameLine();

      static bool loop = false;
      if (ImGui::Checkbox("loop", &loop)) {
        dctx.cmd(make_data_command([=](data_context& ctx) {
          auto lck = ctx.get_data<editor_data>();
          lck.ref().executor_data().set_loop_execution(loop);
        }));
      }

      if (loop) {
        auto fmin = static_cast<float>(m_arg_time_min.seconds().count());
        auto fmax = static_cast<float>(m_arg_time_max.seconds().count());

        auto fmin_input = fmin;
        auto fmax_input = fmax;
        ImGui::DragFloat("##min", &fmin_input, 0.5, 0, 0, "loop min: %.3f");
        ImGui::DragFloat("##max", &fmax_input, 0.5, 0, 0, "loop max: %.3f");

        if (fmin_input != fmin || fmax_input != fmax)
          dctx.cmd(make_data_command([=](data_context& ctx) {
            auto lck = ctx.get_data<editor_data>();
            lck.ref().executor_data().set_loop_range(
              media::time::seconds(fmin_input), media::time::seconds(fmax_input));
          }));
      }
    }
    ImGui::End();
  }

} // namespace yave::editor::imgui
