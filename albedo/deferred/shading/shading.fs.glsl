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

/**
	\todo this is just handy but is also extremely stupid and needs to be replaced
*/
float light_attenuation(in float dist)
{
	#define LIGHT_ATTENUATION_FACTOR 2
	return 1 / pow(dist / LIGHT_ATTENUATION_FACTOR, 2);
}

float point_light_attenuation(in float dist, in float max_dist)
{
	// FIXME
	if (max_dist <= 0 || dist < max_dist)
		return light_attenuation(dist);
	else
		return 0;
}

float spot_light_attenuation(in float dist, in float max_dist, in float cone_angle, in float angle, in float blend)
{
	// FIXME
	if (max_dist <= 0 || dist < max_dist)
	{
		// Power correction for conical shape
		// float power = 2 / (1 - cos(cone_angle));
		float power = 1;

		// Inverse square attenuation
		float attenuation = light_attenuation(dist);

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

/**
	Returns light source's contribution to fragment lighting calculated
	based on Phong model.
*/
vec3 phong(in vec3 N, in vec3 L, in vec3 V, in vec3 diffuse, in vec3 specular, in float specular_exponent)
{
	vec3 R = reflect(-L, N);
	return diffuse * clamp(dot(N, -L), 0, 1) + specular * pow(clamp(dot(R, V), 0, 1), specular_exponent);
}

/**
	Trowbridge-Reitz GGX normal distribution function
	a - roughness
*/
float trowbridge_reitz_ggx(in vec3 N, in vec3 H, in float a)
{
	float N_dot_H = max(dot(N, H), 0);
	float a_sq = a * a;
	float tmp = N_dot_H * N_dot_H * (a_sq - 1) + 1;
	return a_sq / (tmp * tmp * M_PI);
}

/**
	Schlick GGX geometry function
	k - roughness scaled
		k_direct = (a+1)^2/8
		k_IBL    = a^2/2
*/
float schlick_ggx(in vec3 N, in vec3 V, in float k)
{
	float tmp = max(dot(N, V), 0);
	return tmp / (tmp * (1 - k) + k);
}

/**
	Geometry function taking into account both geometry obstruction and geometry shadowing.
	Based on Schlick GGX.

	\todo This can be further optimized by passing only dot(N, V) to the schlick_ggx
*/
float smith_schlick(in vec3 N, in vec3 V, in vec3 L, in float k)
{
	return schlick_ggx(N, V, k) * schlick_ggx(N, L, k);
}

/**
	Fresnel-Schlick approximation
*/
vec3 fresnel_schlick(in vec3 H, in vec3 V, in vec3 F0)
{
	return F0 + (1 - F0) * pow(1 - dot(H, V), 5);
}

/**
	PBR lighting model (Cook-Torrance)
*/
vec3 pbr(in vec3 N, in vec3 L, in vec3 V, in vec3 albedo, in float roughness, in float metallic, in vec3 radiance)
{
	vec3 H = normalize(L + V);
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	// Calculate a and k based on roughness
	float a = roughness;
	float k = pow(a + 1, 2) / 8;

	// Normal distribution function, geometry function and Frensel equation
	float NDF = trowbridge_reitz_ggx(N, H, a);
	float GF  = smith_schlick(N, V, L, k);
	vec3 F = fresnel_schlick(H, V, F0);

	// Specular and diffuse term intensities
	vec3 k_s = F;
	vec3 k_d = (1 - k_s) * (1 - metallic);

	// Specular term
	vec3 specular = NDF * GF * F / max(4 * max(dot(N, V), 0) * max(dot(N, L), 0), 0.001);

	// Difuse term
	vec3 diffuse = (vec3(1) - k_s) * albedo / M_PI;

	//return F;
	return (specular + diffuse) * radiance * max(dot(N, L), 0);
}

void main()
{
	vec3 f_pos      = texture(tex_position, vs_out.v_uv.xy).xyz;
	vec3 f_normal   = texture(tex_normal, vs_out.v_uv.xy).xyz;
	vec3 f_diffuse  = texture(tex_diffuse, vs_out.v_uv.xy).xyz;
	vec3 f_specular = texture(tex_specular, vs_out.v_uv.xy).xyz;

	float f_specular_amount = f_specular.r;
	float f_roughness = f_specular.g;
	float f_specular_tint = f_specular.b;
	float f_specular_exponent = 1 + 99 * pow(1 - f_roughness, 2);

	// Accumulated lighting
	vec3 f_lighting = vec3(0);

	vec3 N = f_normal;
	vec3 V = normalize(-f_pos); // Fragment -> Camera

	// Iterate over light sources
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

		vec3  light_to_frag = l_pos - f_pos;
		float dist = length(light_to_frag);
		vec3  L = normalize(light_to_frag); // Fragment -> Light
		
		/*
			General light intensity. This value determines light source contribution
			to lighting.

			Attenuation and cone characteristics are taken into account here.
		*/
		float attenuation;
		if (l_type == LIGHT_TYPE_POINT)
			attenuation = point_light_attenuation(dist, l_max_dist);
		else if (l_type == LIGHT_TYPE_SPOT)
			attenuation = spot_light_attenuation(dist, l_max_dist, l_angle, acos(clamp(dot(l_dir, -L), 0, 1)), l_blend);
		else if (l_type == LIGHT_TYPE_SUN)
		{
			// All light comes from one direction and has
			// equal power
			L = -l_dir;
			attenuation = 1;
		}

		// f_lighting += attenuation * l_color * phong(N, L, V, f_diffuse, f_specular_amount * f_diffuse, f_specular_exponent);
		f_lighting += pbr(N, L, V, f_diffuse, 0.1, 0.1, attenuation * l_color);
	}

	f_color = f_lighting;
}