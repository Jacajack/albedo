#version 450 core

layout (location = 0) out vec3 f_out;

uniform sampler2D input_tex;

in struct VS_OUT
{
	vec2 v_uv;
} vs_out;

void main()
{
	vec3 f_color = texture(input_tex, vs_out.v_uv.xy).xyz;

	// Reinard tonemapping
	f_color = f_color / (f_color + vec3(1));

	// Gamma correction
	f_color = pow(f_color, vec3(1 / 2.2));

	f_out = f_color;
}