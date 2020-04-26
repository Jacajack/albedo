#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/shader.hpp>
#include <initializer_list>
#include <string>
#include <map>

namespace abd {
namespace gl {

class program;

/**
    Represents an OpenGL program uniform.
    \warning Lifetime of an uniform class may not exceed lifetime of the program class it was retrieved from.

    \todo add type checking and functions for providing uniform values
*/
class uniform : abd::noncopy
{
	friend class program;

public:
	inline GLuint get_location() const;
	inline GLenum get_type() const;
	inline GLint get_size() const;
	inline GLint get_block_index() const;

	inline std::size_t get_field_offset(int column, int row = 0);

private:
	uniform(
		const gl::program &program,
		GLint location,
		GLenum type,
		GLint size,
		GLint block_index,
		GLint offset,
		GLint array_stride,
		GLint matrix_stride,
		GLint is_row_major);

	GLuint m_program;
	GLint m_location;
	GLenum m_type;
	GLint m_size;
	GLint m_block_index;
	GLint m_offset;
	GLint m_array_stride;
	GLint m_matrix_stride;
	bool m_is_row_major;
};

/**
    Returns the uniform location
*/
GLuint uniform::get_location() const
{
	return m_location;
}

/**
    Returns offset of an array/matrix field
*/
std::size_t uniform::get_field_offset(int column, int row)
{
	if (!m_is_row_major)
		return m_offset + row * m_array_stride + column * m_matrix_stride;
	else
		return m_offset + column * m_array_stride + row * m_matrix_stride;
}

/**
    Represents an OpenGL program uniform block
    \warning Lifetime of an uniform_block class may not exceed lifetime of the program class it was retrieved from.
*/
class uniform_block : abd::noncopy
{
	friend class program;

public:
	GLint get_location() const;

private:
	uniform_block(const gl::program &program, GLuint location);

	GLuint m_program;
	GLuint m_index;
};

/**
    Represents RAII wrapper for an OpenGL program object
*/
class program : public abd::gl::gl_object<abd::gl::gl_object_type::PROGRAM>
{
public:
	template <typename Tcont>
	explicit program(const Tcont &shaders);

	std::string get_link_log() const;

	uniform &get_uniform(const std::string &name) const
	{
		return m_uniforms.at(name);
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
}