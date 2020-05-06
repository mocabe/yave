//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>

#include <yave-imgui/canvas.hpp>
#include <yave-imgui/render.hpp>
#include <yave/module/std/module.hpp>
#include <yave/module/std/module_loader.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/app/node_compiler_thread.hpp>
#include <yave/support/log.hpp>
#include <yave/lib/image/image_view.hpp>

#include <yave/app/project.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <boost/gil.hpp>

int main()
{
  auto render_result_tex_name     = "render result";
  auto render_background_tex_name = "render bg";

  using namespace yave;

  using flags = vulkan::vulkan_context::init_flags;
  vulkan::vulkan_context vulkan_ctx(
    flags::enable_logging | flags::enable_validation);

  imgui::imgui_context imgui_ctx {vulkan_ctx};

  scene_config config(
    1600, 900, 60, image_format::rgba32f, audio_format::pcm_44100_stereo);

  std::array<float, 4> bg_tex = {0, 0, 0, 1};

  (void)imgui_ctx.add_texture(
    render_background_tex_name,
    {1, 1},
    sizeof(float) * 4,
    vk::Format::eR32G32B32A32Sfloat,
    (const uint8_t*)bg_tex.data());

  modules::_std::module_loader std_loader {vulkan_ctx};
  app::project project {
    {"my project", {modules::_std::module_name}, {}, config}, std_loader};

  auto& graph = project.graph();
  graph.add_group_output_socket(graph.root_group(), "out");

  app::editor_context editor_ctx(project);

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
        static auto* graph  = &editor_ctx.node_graph();
        static auto ftime   = 0.f;
        static bool compile = true;

        ImGui::Checkbox("compile", &compile);
        ImGui::SliderFloat("time", &ftime, 0, 10);

        if (editor_ctx.is_compiling()) {
          ImGui::Text("Compiling...");
        }

        if (!editor_ctx.is_compiling() && compile) {
          // compile
          if (graph != &editor_ctx.node_graph()) {
            graph = &editor_ctx.node_graph();
            editor_ctx.compile();
          }
          // show result
          auto result = editor_ctx.get_compile_result();

          if (result->success) {

            ImGui::Text(
              "Compile time: %ld ms",
              std::chrono::duration_cast<std::chrono::milliseconds>(
                result->end_time - result->bgn_time)
                .count());

            auto bgn_time = std::chrono::high_resolution_clock::now();
            auto t        = time::seconds(ftime);
            auto obj      = result->exe.execute({{t, {}}, fmat4(1.0)});
            auto end_time = std::chrono::high_resolution_clock::now();

            ImGui::Text(
              "Execution time: %ld ms",
              std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - bgn_time)
                .count());

            try {
              ImGui::Text("Result Type: %s", to_string(type_of(obj)).c_str());
            } catch (type_error::type_error& e) {
              ImGui::Text("Erroneous Result Type: %s", e.what());
            }

            if (auto i = value_cast_if<Int>(obj)) {
              ImGui::Text("Result: %d", *i);
            }

            if (auto f = value_cast_if<Float>(obj)) {
              ImGui::Text("Result: %f", *f);
            }

            if (auto t = value_cast_if<FrameTime>(obj)) {
              ImGui::Text("Result: %lf", t->time_point.seconds().count());
            }

            if (auto s = value_cast_if<String>(obj)) {
              ImGui::Text("Result: %s", s->c_str());
            }

            if (auto m = value_cast_if<FMat4>(obj)) {
              ImGui::Text("Result: fmat4");
              for (int i = 0; i < 4; ++i) {
                ImGui::Text(
                  "%s", glm::to_string((glm::transpose(*m))[i]).c_str());
              }
            }

            // upload result frame to GUI renderer
            if (auto f = value_cast_if<FrameBuffer>(obj)) {

              const_image_view view(
                config.width(), config.height(), config.frame_format());

              if (!imgui_ctx.find_texture(render_result_tex_name))
                (void)imgui_ctx.add_texture(
                  render_result_tex_name,
                  vk::Extent2D(view.width(), view.height()),
                  view.byte_size(),
                  vk::Format::eR32G32B32A32Sfloat,
                  (const uint8_t*)f->view().data());
              else
                imgui_ctx.update_texture(
                  render_result_tex_name,
                  (const uint8_t*)f->view().data(),
                  view.byte_size());
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

      ImGui::Begin("render window");
      {
        yave::editor::imgui::draw_render_window(
          imgui_ctx,
          editor_ctx,
          config,
          render_result_tex_name,
          render_background_tex_name);
      }
      ImGui::End();
    }
    imgui_ctx.end_frame();
    imgui_ctx.render();
  }
}
