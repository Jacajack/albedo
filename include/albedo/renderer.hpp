#pragma once

#include <albedo/gl/framebuffer.hpp>
#include <albedo/gl/texture.hpp>
#include <albedo/gl/program.hpp>
#include <albedo/scene.hpp>
#include <memory>

namespace abd {

/**
	Deferred renderer.
*/
class deferred_renderer
{
public:
	deferred_renderer(int width, int height);

	/**
		Renders entire scene
	*/
	void render_scene(const abd::scene &scene);

	void begin_geometry_pass();
	void begin_shading_pass();

private:

	

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
};

/**
	The default rendering pipeline 
*/
using renderer = deferred_renderer;

}