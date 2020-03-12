//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>

#include <yave-imgui/editor/canvas.hpp>
#include <yave/module/std/module.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/app/node_compiler_thread.hpp>
#include <yave/support/log.hpp>

int main()
{
  using namespace yave;

  using flags = vulkan::vulkan_context::init_flags;
  vulkan::vulkan_context vulkan_ctx(
    flags::enable_logging | flags::enable_validation);

  imgui::imgui_context imgui_ctx {vulkan_ctx};

  scene_config config(1600, 900, 60, 44100, image_format::rgba32f);

  auto _std = modules::_std::module();
  _std.init(config);

  node_definition_store defs;

  graph->register_node_decl(_std.get_node_declarations());
  defs.add(_std.get_node_definitions());

  graph->add_group_output_socket(graph->root_group(), "out");

  app::node_data_thread data_thread(graph);
  app::node_compiler_thread compiler_thread;
  app::editor_context editor_ctx(data_thread);

  data_thread.start();
  compiler_thread.start();

  while (!imgui_ctx.window_context().should_close()) {

    imgui_ctx.begin_frame();
    {
      // demo window
      static bool show_demo_window = true;
      ImGui::ShowDemoWindow(&show_demo_window);
      // editor window

      ImGui::Begin("node canvas");
      {
        editor::imgui::draw_node_canvas(imgui_ctx, editor_ctx);
      }
      ImGui::End();

      ImGui::Begin("parser");
      {
        static auto snapshot = data_thread.snapshot();
        static auto ftime    = 0.f;
        static bool compile  = true;

        ImGui::Checkbox("compile", &compile);
        ImGui::SliderFloat("time", &ftime, 0, 10);

        if (compile) {
          // compile
          if (snapshot != data_thread.snapshot()) {
            snapshot = data_thread.snapshot();
            compiler_thread.compile(snapshot, defs);
          }
          // show result
          auto result = compiler_thread.get_last_result();

          if (result->success) {

            ImGui::Text(
              "Compile time: %ld ms",
              std::chrono::duration_cast<std::chrono::milliseconds>(
                result->end_time - result->bgn_time)
                .count());

            auto t   = time::seconds(ftime);
            auto obj = result->exe.execute(frame_time {t});

            ImGui::Text("Result Type: %s", to_string(type_of(obj)).c_str());

            if (auto i = value_cast_if<Int>(obj)) {
              ImGui::Text("Result: %d", *i);
            }

            if (auto f = value_cast_if<Float>(obj)) {
              ImGui::Text("Result: %f", *f);
            }

            if (auto t = value_cast_if<FrameTime>(obj)) {
              ImGui::Text("Result: %lf", t->time_point.seconds().count());
            }
          } else {
            ImGui::Text("Failed to compile!");
            for (auto&& e : result->parse_errors) {
              ImGui::Text("%s", e.message().c_str());
            }
            for (auto&& e : result->compile_errors) {
              ImGui::Text("%s", e.message().c_str());
            }
          }
        }
      }
      ImGui::End();
    }
    imgui_ctx.end_frame();
    imgui_ctx.render();
  }

  compiler_thread.stop();
  data_thread.stop();
}
