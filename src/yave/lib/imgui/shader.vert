//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

/* Vertex shader for ImGui Vulkan renderer */

#version 450 core

// vertex attributes
layout(location = 0) in vec2 vertPos;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec4 vertColor;

// push constant
layout(push_constant) uniform PushConstant {
  vec2 scale;
  vec2 translate;
} pc;

out gl_PerVertex {
  vec4 gl_Position;
};

// should be matched with fragment shader input
layout(location = 0) out struct {
  vec4 color;
  vec2 uv;
} toFrag;

void main()
{
  // forward color and UV to fragment shader
  toFrag.color = vertColor;
  toFrag.uv    = vertUV;
  // transform
  gl_Position = vec4(vertPos * pc.scale + pc.translate, 0, 1);
}
