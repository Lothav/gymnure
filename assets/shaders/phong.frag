#version 420

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 2) uniform m_Pos{
	vec4 lightPos;
	vec4 cameraPos;
} pos;

layout (binding = 3) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inFragWorldPos;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

struct LightData
{
	vec3 pos;
	vec3 color;
};

const LightData[2] lights = LightData[2](
	LightData(vec3(10,  0, 0), vec3(1, 0, 0)),
	LightData(vec3( 0, 10, 0), vec3(0, 1, 0)));

const float inv_pi = 0.318309886;

void main()
{
	int LIGHT_COUNT = 2;

	vec4 diffuse_color = texture(samplerColor, inUV, 0.0);
	vec3 radiance = vec3(0, 0, 0);

	float Ka = 0.2;
	float Kd = 100.0;
	float Ks = 40.0;

	for(int i = 0; i < lights.length(); i++)
	{
		vec3 light_dir = lights[i].pos - inFragWorldPos;
		vec3 light_color = lights[i].color;
		float distance = length(light_dir);

		vec3 L = light_dir / distance;
		vec3 N = normalize(inNormal);
		vec3 V = normalize(pos.cameraPos.xyz - inFragWorldPos);
		vec3 R = normalize(reflect(-L, N));
		vec3 H = normalize(L + V);

		distance *= distance;

		float n_dot_l = max(dot(N, L), 0.0);
		float r_dot_v = max(dot(R, V), 0.0);
		float n_dot_h = max(dot(N, H), 0.0);

		float diffuse  = Kd * inv_pi * n_dot_l / distance;
		float specular = Ks * inv_pi * pow(n_dot_h, 15.0) / distance;

		radiance += diffuse_color.rgb * Ka +
					diffuse_color.rgb * diffuse +
					light_color.rgb   * specular;
	}

	outFragColor = vec4(radiance, diffuse_color.a);
}