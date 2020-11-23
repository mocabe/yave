//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <fmt/format.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

auto check_files(const std::vector<const char*>& files)
{
  std::vector<std::string> sss;

  for (auto&& file : files) {
    auto path = fs::absolute(fs::path(file));

    if (path.extension() != ".frag" && path.extension() != ".vert") {
      throw std::runtime_error(fmt::format(
        "{}: unsupported file format {}",
        path.string(),
        path.extension().string()));
    }

    if (!fs::exists(path))
      throw std::runtime_error(
        fmt::format("{}: file not found", path.string()));

    auto ifs = std::ifstream(path.string());
    std::stringstream buff;
    buff << ifs.rdbuf();
    sss.push_back(buff.str());
  }
  return sss;
}

auto compile_files(
  const std::vector<const char*>& files,
  const std::vector<std::string>& texts)
{
  std::vector<std::vector<uint32_t>> codes;

  for (size_t i = 0; i < files.size(); ++i) {

    auto&& file = fs::path(files[i]);
    auto&& text = texts[i];

    auto stage = [&] {
      if (file.extension() == ".vert")
        return EShLangVertex;
      if (file.extension() == ".frag")
        return EShLangFragment;
      throw std::runtime_error("invalid extension!");
    }();

    auto msg = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
    std::vector<uint32_t> code;

    glslang::InitializeProcess();
    {
      glslang::TShader shader {stage};
      auto cstr = text.c_str();
      shader.setStrings(&cstr, 1);
      if (!shader.parse(&glslang::DefaultTBuiltInResource, 110, false, msg)) {
        throw std::runtime_error(
          fmt::format("Failed to compile shader: {}", shader.getInfoLog()));
      }

      glslang::TProgram prog;
      prog.addShader(&shader);
      if (!prog.link(msg)) {
        throw std::runtime_error(
          fmt::format("Failed to compile shader: {}", prog.getInfoLog()));
      }

      glslang::GlslangToSpv(*prog.getIntermediate(stage), code);
    }
    glslang::FinalizeProcess();

    codes.push_back(std::move(code));
  }
  return codes;
}

void print_files(
  const std::vector<const char*>& files,
  const std::vector<std::string>& texts,
  const std::vector<std::vector<uint32_t>>& codes)
{
  for (size_t i = 0; i < files.size(); ++i) {
    auto&& file = fs::path(files[i]);
    auto&& text = texts[i];
    auto&& code = codes[i];

    // print source
    {
      fmt::print("// // file: {}\n//\n", file.filename().string());

      auto ss = std::stringstream(text);
      auto s  = std::string();

      while (std::getline(ss, s)) {
        fmt::print("// {}\n", s);
      }
    }

    // print code as array
    {
      auto name = file.filename().string();
      std::replace(name.begin(), name.end(), '.', '_');

      fmt::print("constexpr uint32_t {}_spv[] = {{", name);

      int i = 0;
      for (auto&& c : code) {
        if (i % 6 == 0)
          fmt::print("\n  ");
        fmt::print("0x{:08X}, ", c);
        ++i;
      }
      fmt::print("\n}};\n");
    }
    fmt::print("\n");
  }
}

int main(int argc, char** argv)
{
  if (argc <= 1)
    return 0;

  try {

    auto files = std::vector<const char*>(argv + 1, argv + argc);
    auto texts = check_files(files);
    auto codes = compile_files(files, texts);

    print_files(files, texts, codes);

    return 0;

  } catch (std::exception& e) {
    fmt::print(e.what());
  }
  return -1;
}