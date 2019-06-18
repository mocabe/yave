//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

/* Fragment shader for ImGui Vulkan renderer */

#version 450 core

// image sampler from descriptor set.
layout(set=0, binding=0) uniform sampler2D image;

// should be matched with vertex shader output
layout(location = 0) in struct {
  vec4 color;
  vec2 uv;
} fromVert;

layout(location = 0) out vec4 outColor;

void main()
{
  outColor = fromVert.color * texture(image, fromVert.uv.st);
}
