#version 450 core

layout(set=0, binding=0) uniform sampler2D image;

layout(location = 0) in struct {
  vec4 color;
  vec2 uv;
} fromVert;

layout(location = 0) out vec4 outColor;

// ImGui does not do sRGB->Linear transformation for vertex 
// colors even though these colors are given as UNorm to shader.
// This leads incorrect rendering result especially for sRGB 
// formatted framebuffers.
// To mitigate that, we apply sRGB to Linear color conversion here.
float sRGBtoLinear(float c1) 
{
  float c2 = c1 * c1;
  float c3 = c2 * c1;
  return 0.012522878 * c1 + 0.682171111 * c2 + 0.305306011 * c3; 
} 

vec4 sRGBtoLinear(vec4 v)
{
  return vec4(
    sRGBtoLinear(v[0]),
    sRGBtoLinear(v[1]),
    sRGBtoLinear(v[2]),
    v[3]);
}

void main()
{
  outColor = sRGBtoLinear(fromVert.color) * texture(image, fromVert.uv.st);
}
