#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//layout (location = 0) in sampler2D tex;
//layout (location = 1) in vec4      color;

//varying vec2 texpos;

layout(location = 0) out vec4 outColor;

void main(void) {
    outColor = vec4(1, 1, 1, 1); //texture2D(tex, texpos).a) * color;
}
