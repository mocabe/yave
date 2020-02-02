//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>

#include <yave-imgui/editor/canvas.hpp>
#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/module/std/core/decl/list.hpp>

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>

int main()
{
  using namespace yave;

  imgui::imgui_context imgui_ctx {};

  auto graph = std::make_shared<managed_node_graph>();

  graph->register_node_decl(get_node_declaration<node::Int>());
  graph->register_node_decl(get_node_declaration<node::Float>());
  graph->register_node_decl(get_node_declaration<node::Bool>());
  graph->register_node_decl(get_node_declaration<node::ListCons>());

  graph->add_group_output_socket(graph->root_group(), "out");

  app::node_data_thread data_thread(graph);
  app::editor_context editor_ctx(data_thread);

  node_parser parser;

  node_definition_store defs;
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
        if (ImGui::Button("parse")) {

          auto parsed = parser.parse(*graph);

          if (!parsed)
            Info("Could not parse!");
          else {
            Info("parsed!");

            auto exe = compiler.compile(std::move(*parsed), defs);

            if (!exe)
              Info("Could not compile!");
            else
              Info("compiled!");
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
