#include <albedo/renderer.hpp>
#include <albedo/exception.hpp>
#include <albedo/simple_loaders.hpp>
#include <iostream>

using abd::deferred_renderer;

deferred_renderer::deferred_renderer(int width, int height) :
	m_fbo_width(width),
	m_fbo_height(height)
{
	// Load shaders
	try
	{
		m_geometry_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/geometry_pass"));
		m_shading_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/shading"));
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw abd::exception("deferred_renderer could not load essential shaders");
	}

	// Create depth texture
	m_depth_buffer.storage_2d(gl::texture_format::DEPTH_32F, width, height);
	m_depth_buffer.set_min_filter(GL_LINEAR);
	m_depth_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_DEPTH_BUFFER, m_depth_buffer);

	// Create color texture
	m_color_buffer.storage_2d(gl::texture_format::RGB16F, width, height);
	m_color_buffer.set_min_filter(GL_LINEAR);
	m_color_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT0, m_color_buffer);

	// Create position texture
	m_position_buffer.storage_2d(gl::texture_format::RGB32F, width, height);
	m_position_buffer.set_min_filter(GL_LINEAR);
	m_position_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT1, m_position_buffer);

	// Normal texture
	m_normal_buffer.storage_2d(gl::texture_format::RGB16F, width, height);
	m_normal_buffer.set_min_filter(GL_LINEAR);
	m_normal_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT2, m_normal_buffer);

	// Diffuse color buffer
	m_diffuse_buffer.storage_2d(gl::texture_format::RGB8, width, height);
	m_diffuse_buffer.set_min_filter(GL_LINEAR);
	m_diffuse_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT3, m_diffuse_buffer);

	// Specular info buffer
	m_specular_buffer.storage_2d(gl::texture_format::RGB8, width, height);
	m_specular_buffer.set_min_filter(GL_LINEAR);
	m_specular_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_COLOR_ATTACHMENT4, m_specular_buffer);

	if (!m_fbo.is_complete())
		throw abd::exception("deferred_renderer's FBO is incomplete!");

	// Specify draw buffers
	m_fbo.set_draw_buffers({
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
	});
}


void deferred_renderer::render_geometry(abd::draw_task_list draw_tasks, const abd::camera &camera)
{
	// Beginning of the geometry pass - bind MRT FBO, VAO and proper program
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	m_vao.bind();
	m_geometry_program->use();

	// Some uniforms
	// auto &uni_mat_model = m_geometry_program->get_uniform("mat_model");
	// auto &uni_mat_view = m_geometry_program->get_uniform("mat_view");
	// auto &uni_mat_proj = m_geometry_program->get_uniform("mat_proj");
	// auto &uni_mat_vp = m_geometry_program->get_uniform("mat_vp");

	// Pass view and projection matrices to the shader
	// TODO
	// glUniformMatrix4fv(uni_mat_view, 1, GL_FALSE, &camera.get_matrix()[0][0]);

	//! \todo sort mesh draw_tasks to minimize context-changes
	// Execute every draw task
	for (const auto &task : draw_tasks.mesh_draw_tasks)
	{
		auto &mesh = *task.mesh_ptr;
		auto &mesh_data = mesh.get_data();
		auto &mesh_buffers = mesh.get_buffers();

		mesh_buffers.bind_index_buffer();
		mesh_buffers.bind_to_vao(m_vao);
		// glUniformMatrix4fv(uni_mat_model, 1, GL_FALSE, &task.transform[0][0]); // FIXME
		// TODO pass MVP matrix

		//! \todo replace with glMutliDraw*()
		// Draw all sub-meshes one by one
		for (unsigned int i = 0; i < mesh_data.base_indices.size(); i++)
		{
			glDrawElementsBaseVertex(
				GL_TRIANGLES,
				mesh_data.draw_sizes[i],
				GL_UNSIGNED_INT,            // FIXME
				nullptr,
				mesh_data.base_indices[i]
				);
		}
	}


	//! \todo shading pass (maybe in a separate funcion)
}