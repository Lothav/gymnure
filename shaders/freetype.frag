#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) varying vec2      texpos;
layout (location = 1) uniform sampler2D tex;
layout (location = 2) uniform vec4      color;

void main(void) {
  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
}
