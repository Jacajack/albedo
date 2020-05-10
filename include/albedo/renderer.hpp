#pragma once

#include <albedo/gl/synced_buffer.hpp>
#include <albedo/gl/framebuffer.hpp>
#include <albedo/gl/texture.hpp>
#include <albedo/gl/program.hpp>
#include <albedo/mesh.hpp>
#include <albedo/camera.hpp>
#include <memory>

namespace abd {

/**
	Contains all information required to draw a mesh
*/
struct mesh_draw_task
{
	glm::mat4 transform;
	std::shared_ptr<abd::mesh> mesh_ptr;
};


/**
	Contains all information required to perform a shading operation.

	If the camera is inside the light volume, the task has to be dispatched with
	the volume type set to GLOBAL, so that the entire screen is shaded. Whether
	that is the case should be determined by the scene (or whatever issues draw tasks).

	The pointer volume_mesh_ptr is only checked if volume type is set to MESH.
*/
struct light_draw_task
{
	/**
		Determines the light volume drawn during shading. This does not directly
		determine the type of the light, though.
	*/
	enum class light_volume_type
	{
		GLOBAL    = 0,  //!< Affects entire screen area
		SPHERICAL = 1,  //!< Affects a spherical region 
		MESH      = 2,  //!< Affects a region determined by provided mesh  
	};

	/**
		Determines physical characteristics of the light source
	*/
	enum class light_type
	{
		POINT = 0,   //!< A point light source
		SPOT  = 1,   //!< Directional light source
		SUN   = 2,   //!< Light source in infinity (all rays are parallel)
	};

	//! Determines physical model
	light_type type;

	//! Determines what mesh should be drawn during shading
	light_volume_type volume;
	std::shared_ptr<abd::mesh> volume_mesh_ptr;

	//! Determines light source position in space (point and spot only)
	glm::vec3 position;
	
	//! Determines light direction (spot and sun only)
	glm::vec3 direction;

	//! Light color
	glm::vec3 color;

	//! Light power
	float power;

	//! Distance at which the light attenuates completely. 0 means no attenuation at all.
	float distance;

	//! Cone angle for spot lights
	float angle;

	//! Determines roll off characteristics of the spot light (0 - sharp, 1 - smooth)
	float blend;

	//! Specular contribution of the light
	float specular;

	//! Necessary for sorting lights
	bool operator<(const light_draw_task &rhs) const;
};


/**
	Groups together different types draws tasks that
	will later to be executed in different phases of
	the rendering process
*/
struct draw_task_list
{
	std::vector<mesh_draw_task> mesh_draw_tasks;
	std::vector<light_draw_task> light_draw_tasks;
	// transparent/translucent draw tasks
	// light_draw_tasks
	// special draw tasks (grass and stuff)
};

/**
	Deferred renderer's geometry buffer

	Standard MRT layout is:
	layout (location = 0) out vec3 f_color;
	layout (location = 1) out vec3 f_pos;
	layout (location = 2) out vec3 f_normal;
	layout (location = 3) out vec3 f_diffuse;
	layout (location = 4) out vec3 f_specular;

	\todo optimize layout
*/
struct standard_gbuffer
{
	/**
		The depth buffer (32F)
		\note FBO depth attachment
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> depth;

	/**
		Contains three floating-point components that contain screen-space
		coordinates of fragments.
		\note FBO color attachment 1

		\todo reconstruct position from depth!
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> position;

	/**
		Three 16-bit floating point components - screen-space normals
		\note FBO color attachment 2
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> normal;

	/**
		Three 8-bit RGB components of diffuse material color
		\note FBO color attachment 3
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> diffuse;

	/**
		R - specular intensity
		G - specular exponent
		B - material roughness
		\note FBO color attachment 4
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> specular;
};

/**
	Deferred renderer.
*/
class deferred_renderer
{
public:
	struct gbuffer;
	struct ubo_light_data;
	struct ubo_material_data;

	deferred_renderer(int width, int height);

	void render(abd::draw_task_list draw_tasks, const abd::camera &camer, GLuint output_fbo);

	const abd::gl::framebuffer &get_fbo() const {return m_fbo;}

private:
	static const int max_light_count = 128;

	void prepare_lights_data(std::vector<light_draw_task> &light_tasks, gl::synced_buffer_handle &lights_buffer_chunk);
	
	void geometry_pass(std::vector<mesh_draw_task> &mesh_tasks, const abd::camera &camera);
	void lighting_pass(std::vector<light_draw_task> &light_tasks, gl::synced_buffer_handle &lights_buffer_chunk, const abd::camera &camera);
	void postprocess_to_output(GLuint output_fbo);

	/**
		The actual color buffer that we later output HDR image to
		\note FBO color attachment 0
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_color_buffer;

	//! The G-buffer
	standard_gbuffer m_gbuffer;

	/**
		Contains a quad used for blitting and postprocessing
	*/
	abd::gl::buffer m_blit_quad;
	
	/**
		Contains information about all the lights to be processed
	*/
	abd::gl::synced_buffer m_lights_buffer;

	/**
		The main VAO - input stage for the geomatry pass shaders
	*/
	abd::fixed_vao m_vao{abd::standard_vao_layout};


	// Framebuffer
	int m_fbo_width;
	int m_fbo_height;
	gl::framebuffer m_fbo;

	std::unique_ptr<gl::program> m_geometry_program;
	std::unique_ptr<gl::program> m_shading_program;
	std::unique_ptr<gl::program> m_postprocess_program;
};

/**
	Light data passed to the shaders in UBO.
	Data in this struct corresponds to the data light_draw_task
	but is more packed.
*/
struct deferred_renderer::ubo_light_data
{
	GLint light_type;   //!< Determines light type (not the volume type)
	GLfloat blend;
	GLfloat dummy1;
	GLfloat dummy2;
	glm::vec4 color_specular;
	glm::vec4 position_distance;
	glm::vec4 direction_angle;
};

/**
	Material data as passed to the geometry pass shader.
*/
struct deferred_renderer::ubo_material_data
{
	glm::vec4 diffuse;
	std::uint64_t diffuse_tex; // sampler2D

	float specular;
	float roughness;
	float specular_tint;
};

/**
	The default rendering pipeline 
*/
using renderer = deferred_renderer;

}