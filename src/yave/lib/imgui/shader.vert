#version 450 core

layout(location = 0) in vec2 vertPos;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec4 vertColor;

layout(push_constant) uniform PushConstant {
  vec2 scale;
  vec2 translate;
} pc;

out gl_PerVertex {
  vec4 gl_Position;
};

layout(location = 0) out struct {
  vec4 color;
  vec2 uv;
} toFrag;

void main()
{
  toFrag.color = vertColor;
  toFrag.uv    = vertUV;
  gl_Position  = vec4(vertPos * pc.scale + pc.translate, 0, 1);
}
