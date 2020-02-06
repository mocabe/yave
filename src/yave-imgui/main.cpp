//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>

#include <yave-imgui/editor/canvas.hpp>
#include <yave/module/std/core/module_definition.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>

int main()
{
  using namespace yave;

  imgui::imgui_context imgui_ctx {};

  auto graph = std::make_shared<managed_node_graph>();

  auto core = modules::_std::core::module();
  node_definition_store defs;

  graph->register_node_decl(core.get_node_declarations());
  defs.add(core.get_node_definitions());

  graph->add_group_output_socket(graph->root_group(), "out");

  app::node_data_thread data_thread(graph);
  app::editor_context editor_ctx(data_thread);

  node_parser parser;
  node_compiler compiler;

  data_thread.start();

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
        static auto snapshot                 = data_thread.snapshot();
        static std::optional<executable> exe = std::nullopt;
        static std::string msg               = "";
        static auto ftime                    = 0.f;
        static auto result                   = object_ptr<const Object>();

        if (snapshot != data_thread.snapshot()) {

          snapshot = data_thread.snapshot();

          auto parsed = parser.parse(*graph);

          if (!parsed) {
            msg = "Could to parse graph!";
            exe = std::nullopt;
          } else {
            auto e = compiler.compile(std::move(*parsed), defs);

            if (!e) {
              msg = "Could not compile!";
              exe = std::nullopt;
            } else {
              msg = "Compiled!";
              exe = e;
            }
          }
        }

        ImGui::Text("%s", msg.c_str());
        if (exe) {
          ImGui::Text("Result Type: %s", to_string(exe->type()).c_str());
        }
        ImGui::SliderFloat("time", &ftime, 0, 10);

        if (exe) {

          auto t = time::seconds(ftime);
          result = exe->execute(frame_time {t});

          ImGui::Text("Result Type: %s", to_string(get_type(result)).c_str());

          if (auto i = value_cast_if<Int>(result)) {
            ImGui::Text("Result: %d", *i);
          }

          if (auto f = value_cast_if<Float>(result)) {
            ImGui::Text("Result: %f", *f);
          }

          if (auto t = value_cast_if<FrameTime>(result)) {
            ImGui::Text("Result: %lf", t->time_point.seconds());
          }
        }
      }
      ImGui::End();
    }
    imgui_ctx.end_frame();

    imgui_ctx.render();
  }

  data_thread.stop();
}
