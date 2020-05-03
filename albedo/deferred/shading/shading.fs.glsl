#version 450 core

#define MAX_LIGHT_COUNT 128

layout (location = 0) out vec3 f_color;

// Standard G-buffer layout
uniform sampler2D tex_position;
uniform sampler2D tex_normal;
uniform sampler2D tex_diffuse;
uniform sampler2D tex_specular;

// Light data (must correspond to ubo_light_data in C++)
struct ubo_light_data
{
	int light_type;
	float attenuation;
	float angle;
	vec3 color;
	vec3 position;
	vec3 direction;
};

// UBO with lights data
uniform int base_light_index;
uniform int light_count;
layout (std140) uniform LIGHTS_UBO
{
	ubo_light_data lights_data[MAX_LIGHT_COUNT];
} lights_ubo;

in struct VS_OUT
{
	vec2 v_uv;
} vs_out;

void main()
{
	vec3 f_pos     = texture(tex_position, vs_out.v_uv.xy).xyz;
	vec3 f_normal  = texture(tex_normal, vs_out.v_uv.xy).xyz;
	vec3 f_diffuse = texture(tex_diffuse, vs_out.v_uv.xy).xyz;
	// vec3 f_diffuse = texture(tex_diffuse, vs_out.v_uv.xy).xyz;

	// Accumulated lighting
	vec3 f_lighting = vec3(0);

	vec3 N = f_normal;
	vec3 E = normalize(-f_pos); // Fragment -> Camera

	// Iterate over lights
	for (int i = base_light_index; i < base_light_index + light_count; i++)
	{
		vec3 l_pos = lights_ubo.lights_data[i].position;
		vec3 l_color = lights_ubo.lights_data[i].color;

		vec3 L = normalize(f_pos - l_pos); // Light -> Fragment
		vec3 R = reflect(L, N);            // Reflected light

		f_lighting += f_diffuse * l_color * clamp(dot(N, -L), 0, 1);
	}

	f_color = f_lighting;
}