#version 450 core
    
layout(set=0, binding=0) uniform sampler2D image;
    
layout(location = 0) in vec2 uv;
    
layout(location = 0) out vec4 outColor;
    
void main()
{
  outColor = texture(image, uv.st);
}