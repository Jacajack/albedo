#version 450 core

layout (location = 0) out vec3 f_out;

uniform sampler2D input_tex;

in struct VS_OUT
{
	vec2 v_uv;
} vs_out;

void main()
{
	f_out = texture(input_tex, vs_out.v_uv.xy).xyz;
}