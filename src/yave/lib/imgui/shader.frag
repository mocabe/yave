#version 450 core

layout(set=0, binding=0) uniform sampler2D image;

layout(location = 0) in struct {
  vec4 color;
  vec2 uv;
} fromVert;

layout(location = 0) out vec4 outColor;

void main()
{
  outColor = fromVert.color * texture(image, fromVert.uv.st);
}
