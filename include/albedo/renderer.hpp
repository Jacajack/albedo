#pragma once

#include <albedo/gl/framebuffer.hpp>
#include <albedo/gl/texture.hpp>
#include <albedo/gl/program.hpp>
#include <albedo/scene.hpp>
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
		Determines the light volume drawn during shading
	*/
	enum class light_volume_type
	{
		GLOBAL    = 0,  //!< Affects entire screen area
		SPHERICAL = 1,  //!< Affects a spherical region 
		MESH      = 2,  //!< Affects a region determined by provided mesh  
	};

	light_volume_type volume;
	glm::vec3 position;
	glm::vec3 direction;
	float attenuation;
	float angle;

	glm::vec3 color;

	std::shared_ptr<abd::mesh> volume_mesh_ptr;

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
	Deferred renderer.

	\todo optimize G-buffer layout

	Standard MRT layout for the deferred renderer is:
	layout (location = 0) out vec3 f_color;
	layout (location = 1) out vec3 f_pos;
	layout (location = 2) out vec3 f_normal;
	layout (location = 3) out vec3 f_diffuse;
	layout (location = 4) out vec3 f_specular;

*/
class deferred_renderer
{
public:
	/**
		Light data passed to the shaders in UBO.
	*/
	struct ubo_light_data
	{
		GLint light_type;   //!< Determines light type (not the volume type)
		GLfloat attenuation;
		GLfloat angle;
		GLfloat dummy;
		glm::vec4 color;
		glm::vec4 position;
		glm::vec4 direction;
	};

	deferred_renderer(int width, int height);

	void render_geometry(abd::draw_task_list draw_tasks, const abd::camera &camera);
	void shading_pass(abd::draw_task_list draw_tasks, GLuint output_fbo);

	const abd::gl::framebuffer &get_fbo() const {return m_fbo;}

private:
	static const int max_light_count = 128;

	/**
		Contains a quad used for blitting and postprocessing
	*/
	abd::gl::buffer m_blit_quad;
	
	/**
		Contains information about all the lights to be processed
	*/
	abd::gl::buffer m_lights_buffer;

	/**
		The main VAO - input stage for the geomatry pass shaders
	*/
	abd::fixed_vao m_vao{abd::standard_vao_layout};

	/**
		The depth buffer (32F)
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_depth_buffer;

	/**
		The actual color buffer that we later output HDR image to
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_color_buffer;

	/**
		Contains three floating-point components that contain screen-space
		coordinates of fragments.

		\todo reconstruct position from depth!
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_position_buffer;

	/**
		Three 16-bit floating point components - screen-space normals
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_normal_buffer;

	/**
		Three 8-bit RGB components of diffuse material color
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_diffuse_buffer;

	/**
		R - specular intensity
		G - specular exponent
		B - material roughness
	*/
	gl::texture<gl::texture_target::TEXTURE_2D> m_specular_buffer;

	// More buffers to come
 
	// Framebuffer
	int m_fbo_width;
	int m_fbo_height;
	gl::framebuffer m_fbo;

	std::unique_ptr<gl::program> m_geometry_program;
	std::unique_ptr<gl::program> m_shading_program;
	std::unique_ptr<gl::program> m_blit_program;
};

/**
	The default rendering pipeline 
*/
using renderer = deferred_renderer;

}