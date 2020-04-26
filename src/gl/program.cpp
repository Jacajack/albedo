#include <albedo/gl/program.hpp>

using abd::gl::uniform;
using abd::gl::uniform_block;
using abd::gl::program;

/**
*/
uniform::uniform(
		const gl::program &program,
		GLint location,
		GLenum type,
		GLint size,
		GLint block_index,
		GLint offset,
		GLint array_stride,
		GLint matrix_stride,
		GLint is_row_major) :
	m_program(program),
	m_location(location),
	m_type(type),
	m_size(size),
	m_block_index(block_index),
	m_offset(offset),
	m_array_stride(array_stride),
	m_matrix_stride(matrix_stride),
	m_is_row_major(is_row_major)
{
}




uniform_block::uniform_block(const gl::program &program, GLuint index) :
	m_program(program),
	m_index(index)
{

}


std::string program::get_link_log() const
{
	GLint length = this->get_parameter<GLint>(GL_INFO_LOG_LENGTH);
	if (length > 0)
	{
		char arr[length + 1];
		glGetProgramInfoLog(*this, length, NULL, arr);
		return std::string{arr};
	}
	else
		return {};
}
