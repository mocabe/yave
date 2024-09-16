#version 450 core
      
layout(set=0, binding=0) uniform sampler2D image;
      
layout(location = 0) in struct {
  vec4 color;
  vec2 uv;
} vertOut;
      
layout(location = 0) out vec4 outColor;
      
void main()
{
  outColor = vertOut.color * texture(image, vertOut.uv.st);
}