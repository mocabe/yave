//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/shader.hpp>

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(glslang)

namespace yave::vulkan {

  namespace {

    auto compileShader(const char* code, EShLanguage stage)
      -> std::vector<uint32_t>
    {
      init_logger();

      EShMessages msg = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

      std::vector<uint32_t> ret;

      glslang::InitializeProcess();
      {
        glslang::TShader shader {stage};
        shader.setStrings(&code, 1);
        if (!shader.parse(&glslang::DefaultTBuiltInResource, 110, false, msg)) {
          Error(g_logger, "{}", shader.getInfoLog());
          Error(g_logger, "{}", shader.getInfoDebugLog());
          throw std::runtime_error("Failed to compile shader: parse failed");
        }

        glslang::TProgram prog;
        prog.addShader(&shader);
        if (!prog.link(msg)) {
          Error(g_logger, "{}", prog.getInfoLog());
          Error(g_logger, "{}", shader.getInfoDebugLog());
          throw std::runtime_error("Failed to compile shader: link failed");
        }

        glslang::GlslangToSpv(*prog.getIntermediate(stage), ret);
      }
      glslang::FinalizeProcess();

      return ret;
    }

  } // namespace

  auto compileVertShader(const char* code) -> std::vector<uint32_t>
  {
    return compileShader(code, EShLangVertex);
  }

  auto compileFragShader(const char* code) -> std::vector<uint32_t>
  {
    return compileShader(code, EShLangFragment);
  }
} // namespace yave::vulkan