#version 450 core

#define M_PI 3.141592653589793238462643383279502884

// FIXME
#define MAX_LIGHT_COUNT 128

// Light types corresponding to abd::light_draw_task::light_type
#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_SPOT  1
#define LIGHT_TYPE_SUN   2

layout (location = 0) out vec3 f_color;

// Standard G-buffer layout
uniform sampler2D tex_position;
uniform sampler2D tex_normal;
uniform sampler2D tex_diffuse;
uniform sampler2D tex_specular;

// All view matrices
uniform mat4 mat_view;
uniform mat4 mat_proj;
uniform mat4 mat_vp;

// Light data (must correspond to ubo_light_data in C++)
struct ubo_light_data
{
	int type;
	float blend;
	vec4 color_specular;
	vec4 position_distance;
	vec4 direction_angle;
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

float point_light_attenuation(in float dist, in float max_dist)
{
	// FIXME
	if (max_dist <= 0 || dist < max_dist)
		return 1 / pow(dist, 2);
	else
		return 0;
}

float spot_light_attenuation(in float dist, in float max_dist, in float cone_angle, in float angle, in float blend)
{
	// FIXME
	if (max_dist <= 0 || dist < max_dist)
	{
		// Power correction for conical shape
		float power = 2 / (1 - cos(cone_angle));

		// Inverse square attenuation
		float attenuation = 1 / pow(dist, 2);

		// Clamped ray angle to cone angle ratio
		// determines attenuation
		float x = clamp(angle / cone_angle, 0, 1);
		float t = 1 - blend;
		float cone_attenuation = x < t ? 1 : cos((x - t) / (1 - t) * M_PI / 2);

		// Compensates power for different blending values
		// due to different light distribution
		// see: https://www.desmos.com/calculator/k1rqxsbzfg
		float blending_correction = 1 / (t * (M_PI / 2 - 1) + 1);

		return power * attenuation * cone_attenuation * blending_correction;
	}
	else
		return 0;
}


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
		// Unpack the data from the UBO
		int   l_type = lights_ubo.lights_data[i].type;
		vec3  l_pos = (mat_view * vec4(lights_ubo.lights_data[i].position_distance.xyz, 1)).xyz;
		float l_max_dist = lights_ubo.lights_data[i].position_distance.w;
		vec3  l_dir = (mat_view * vec4(lights_ubo.lights_data[i].direction_angle.xyz, 0)).xyz;
		float l_angle = lights_ubo.lights_data[i].direction_angle.w;
		vec3  l_color = lights_ubo.lights_data[i].color_specular.xyz;
		float l_specular = lights_ubo.lights_data[i].color_specular.w;
		float l_blend = lights_ubo.lights_data[i].blend;

		vec3  light_to_frag = f_pos - l_pos;
		float dist = length(light_to_frag);
		vec3  L = normalize(light_to_frag); // Light -> Fragment
		vec3  R = reflect(L, N);            // Reflected light
		
		/*
			General light intensity.
			Attenuation and cone characteristics are taken into account here.
		*/
		float intensity;
		if (l_type == LIGHT_TYPE_POINT)
			intensity = point_light_attenuation(dist, l_max_dist);
		else if (l_type == LIGHT_TYPE_SPOT)
			intensity = spot_light_attenuation(dist, l_max_dist, l_angle, acos(clamp(dot(l_dir, L), 0, 1)), l_blend);
		else if (l_type == LIGHT_TYPE_SUN)
		{
			// All light comes from one direction and has
			// equal power
			L = l_dir;
			intensity = 1;
		}

		f_lighting += intensity * f_diffuse * l_color * clamp(dot(N, -L), 0, 1);
		f_lighting += intensity * l_specular * l_color * pow(clamp(dot(R, E), 0, 1), 4);
	}

	f_color = f_lighting;
}