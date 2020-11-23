#version 450 core

vec2 pos[6] = vec2[](
  vec2(-1, -1), 
  vec2(-1,  1), 
  vec2( 1,  1), 
  vec2(-1, -1), 
  vec2( 1,  1), 
  vec2( 1, -1)
);

vec2 uv[6] = vec2[](
  vec2(0, 0),
  vec2(0, 1),
  vec2(1, 1),
  vec2(0, 0),
  vec2(1, 1),
  vec2(1, 0)
);
    
layout(location = 0) out vec2 outUV;

void main()
{
  outUV       = uv[gl_VertexIndex];
  gl_Position = vec4(pos[gl_VertexIndex], 0, 1);
}