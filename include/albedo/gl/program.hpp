#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/shader.hpp>
#include <albedo/gl/uniform.hpp>
#include <initializer_list>
#include <string>
#include <map>

namespace abd::gl {

/**
    Represents RAII wrapper for an OpenGL program object. The RAII here isn't quite perfect, because
	you have to take care to only use uniforms and uniform_blocks acquired from this program when it is
	used itself.
*/
class program : public abd::gl::gl_object<abd::gl::gl_object_type::PROGRAM>
{
public:
	template <typename Tcont>
	explicit program(const Tcont &shaders);

	std::string get_link_log() const;

	uniform &get_uniform(const std::string &name) const
	{
		try
		{
			return m_uniforms.at(name);
		}
		catch (const std::out_of_range &ex)
		{
			return abd::gl::null_uniform;
		}
	}

	uniform_block &get_uniform_block(const std::string &name) const
	{
		return m_uniform_blocks.at(name);
	}

	void use() const
	{
		glUseProgram(*this);
	}

	const std::map<std::string, uniform> &get_uniforms() const
	{
		return m_uniforms;
	}

	const std::map<std::string, uniform_block> &get_uniform_blocks() const
	{
		return m_uniform_blocks;
	}

private:
	mutable std::map<std::string, uniform> m_uniforms;
	mutable std::map<std::string, uniform_block> m_uniform_blocks;
};

/**
    This constructor allows program creation based on any container
    of shaders
*/
template <typename Tcont>
program::program(const Tcont &shaders)
{
	// Attach all shaders
	for (const auto &s : shaders)
		glAttachShader(*this, s);

	// Link the program
	glLinkProgram(*this);

	// Detach all shaders
	for (const auto &s : shaders)
		glDetachShader(*this, s);

	// Check linking result
	if (this->get_parameter<GLint>(GL_LINK_STATUS) == GL_FALSE)
	{
		throw abd::gl::shader_exception(get_link_log(), abd::gl::program_link_error{});
	}

	// Get uniform count
	GLint uniform_count = this->get_parameter<GLint>(GL_ACTIVE_UNIFORMS);

	// Reserve some space for uniform info (SOA style)
	std::vector<GLuint> uniform_indices(uniform_count);
	std::vector<GLint> uniform_block_indices(uniform_count);
	std::vector<GLint> uniform_offsets(uniform_count);
	std::vector<GLint> uniform_array_strides(uniform_count);
	std::vector<GLint> uniform_matrix_strides(uniform_count);
	std::vector<GLint> uniform_is_row_major(uniform_count);

	// Generate indices
	for (GLint i = 0; i < uniform_count; i++)
		uniform_indices.at(i) = i;

	// Get uniform data
	glGetActiveUniformsiv(*this, uniform_count, uniform_indices.data(), GL_UNIFORM_BLOCK_INDEX, uniform_block_indices.data());
	glGetActiveUniformsiv(*this, uniform_count, uniform_indices.data(), GL_UNIFORM_OFFSET, uniform_offsets.data());
	glGetActiveUniformsiv(*this, uniform_count, uniform_indices.data(), GL_UNIFORM_ARRAY_STRIDE, uniform_array_strides.data());
	glGetActiveUniformsiv(*this, uniform_count, uniform_indices.data(), GL_UNIFORM_MATRIX_STRIDE, uniform_matrix_strides.data());
	glGetActiveUniformsiv(*this, uniform_count, uniform_indices.data(), GL_UNIFORM_IS_ROW_MAJOR, uniform_is_row_major.data());

	// Construct uniforms (AOS style)
	for (GLint i = 0; i < uniform_count; i++)
	{
		char name[1024];
		GLenum type;
		GLint size;
		GLint location;
		glGetActiveUniform(*this, i, sizeof(name), nullptr, &size, &type, name);
		location = glGetUniformLocation(*this, name);

		m_uniforms.emplace(name, uniform{
			*this,
			location,
			type,
			size,
			uniform_block_indices.at(i),
			uniform_offsets.at(i),
			uniform_array_strides.at(i),
			uniform_matrix_strides.at(i),
			uniform_is_row_major.at(i)
		});
	}

	// Retrieve uniform block information
	GLint uniform_block_count = this->get_parameter<GLint>(GL_ACTIVE_UNIFORM_BLOCKS);
	for (GLint i = 0; i < uniform_block_count; i++)
	{
		char name[1024];
		glGetActiveUniformBlockName(*this, i, sizeof(name), nullptr, name);
		m_uniform_blocks.emplace(name, uniform_block{*this, static_cast<GLuint>(i)});

		// Set binding number to block index number
		glUniformBlockBinding(*this, i, i);
	}
}


}