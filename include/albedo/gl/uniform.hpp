#pragma once

#include <albedo/gl/gl_object.hpp>

namespace abd {
namespace gl {

// Forward declare program class
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

	inline std::size_t get_field_offset(int column, int row = 0) const;

	inline operator GLint() const;

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
std::size_t uniform::get_field_offset(int column, int row) const
{
	if (!m_is_row_major)
		return m_offset + row * m_array_stride + column * m_matrix_stride;
	else
		return m_offset + column * m_array_stride + row * m_matrix_stride;
}

/**
	Converts to GLint representing location
*/
uniform::operator GLint() const
{
	return get_location();
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

}
}