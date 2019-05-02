#version 420

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 2) uniform sampler2D samplerColor;

layout (binding = 3) uniform m_Pos{
	vec4 lightPos;
	vec4 cameraPos;
} pos;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inFragWorldPos;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

void main()
{
	vec4 color = texture(samplerColor, inUV, 0.0);

	vec3 N = normalize(inNormal);
	vec3 L = normalize(pos.lightPos.xyz  - inFragWorldPos);
	vec3 V = normalize(pos.cameraPos.xyz - inFragWorldPos);
	vec3 R = reflect(-L, N);
	float diffuse = max(dot(N, L), 0.0);
	float specular = pow(max(dot(R, V), 0.0), 5.0);

	outFragColor = vec4(diffuse * color.rgb + specular, 1.0);
}