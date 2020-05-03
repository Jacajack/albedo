#include <albedo/renderer.hpp>
#include <albedo/exception.hpp>
#include <albedo/simple_loaders.hpp>
#include <albedo/gl/debug.hpp>
#include <iostream>
#include <array>

using abd::deferred_renderer;

deferred_renderer::deferred_renderer(int width, int height) :
	m_blit_quad(6 * 3 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT),
	m_fbo_width(width),
	m_fbo_height(height)
{
	// Load shaders
	try
	{
		m_geometry_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/geometry_pass"));
		m_shading_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/shading"));
		m_blit_program = std::make_unique<gl::program>(abd::simple_load_shader_dir("albedo/deferred/blit"));
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		throw abd::exception("deferred_renderer could not load essential shaders");
	}

	// The blit quad
	std::array<float, 18> quad_data =
	{
		-1, -1, 0,
		1, -1, 0,
		-1, 1, 0,
		
		-1, 1, 0,
		1, -1, 0,
		1, 1, 0,
	};
	m_blit_quad.write(0, quad_data.size() * sizeof(float), quad_data.data());

	// Create depth texture
	m_depth_buffer.storage_2d(gl::texture_format::DEPTH_32F, width, height);
	m_depth_buffer.set_min_filter(GL_LINEAR);
	m_depth_buffer.set_mag_filter(GL_LINEAR);
	m_fbo.attach_texture(GL_DEPTH_ATTACHMENT, m_depth_buffer);

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
	abd::gl::debug_group d(0, "abd::deferred_renderer geometry pass");

	// Beginning of the geometry pass - bind MRT FBO, VAO and proper program
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	m_vao.bind();
	m_geometry_program->use();

	// Clear buffers and enable depth test
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Some uniforms
	auto &uni_mat_model = m_geometry_program->get_uniform("mat_model");
	auto &uni_mat_view = m_geometry_program->get_uniform("mat_view");
	auto &uni_mat_proj = m_geometry_program->get_uniform("mat_proj");
	auto &uni_mat_vp = m_geometry_program->get_uniform("mat_vp");
	auto &uni_mat_mvp = m_geometry_program->get_uniform("mat_mvp");

	// Pass view and projection matrices to the shader
	uni_mat_view = camera.get_view_matrix();
	uni_mat_proj = camera.get_projection_matrix();
	uni_mat_vp = camera;

	//! \todo sort mesh draw_tasks to minimize context-changes
	// Execute every draw task
	for (const auto &task : draw_tasks.mesh_draw_tasks)
	{
		auto &mesh = *task.mesh_ptr;
		auto &mesh_data = mesh.get_data();
		auto &mesh_buffers = mesh.get_buffers();

		mesh_buffers.bind_index_buffer();
		mesh_buffers.bind_to_vao(m_vao);

		// Update model matrix
		uni_mat_model = task.transform;
		uni_mat_mvp = camera.get_matrix() * task.transform;

		//! \todo replace with glMutliDraw*()
		// Draw all sub-meshes one by one
		for (unsigned int i = 0; i < mesh_data.base_indices.size(); i++)
		{
			glDrawElementsBaseVertex(
				GL_TRIANGLES,
				mesh_data.draw_sizes[i],
				GL_UNSIGNED_INT,            //! \todo this should be based on type provided by abd::mesh
				nullptr,
				mesh_data.base_indices[i]
				);
		}
	}
}


void deferred_renderer::shading_pass(GLuint output_fbo)
{
	abd::gl::debug_group d(0, "abd::deferred_renderer shading pass");


	// Attach VAO and the framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	m_vao.bind();


	//! \todo Light processing here

	// Attach the blit quad to the VAO and disable depth test
	m_vao.bind_buffer(0, m_blit_quad, {0, 3 * sizeof(float)});
	glDisable(GL_DEPTH_TEST);


	//! \todo postprocessing here

	// Blit color buffer to the output FBO
	m_blit_program->use();
	m_blit_program->get_uniform("input_tex") = 0;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output_fbo);
	glBindTextureUnit(0, m_color_buffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}