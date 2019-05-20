#version 420

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UBO_m {
    mat4 data;
} m;

layout (binding = 1) uniform UBO_vp {
    mat4 data;
} vp;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outFragWorldPos;
layout (location = 2) out vec3 outNormal;

void main()
{
   	outUV           = inUV;
    outFragWorldPos = (/*m.data **/ vec4(inPos, 1.0)).xyz;
	gl_Position     = vp.data * vec4(outFragWorldPos, 1.0);

    vec4 tNormal    = vec4(/*inverse(transpose(m.data)) * */vec4(inNormal, 1.0));
    outNormal       = tNormal.xyz / tNormal.w;
}