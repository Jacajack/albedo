#include <albedo/gl/uniform.hpp>
#include <albedo/gl/program.hpp>

using abd::gl::uniform;
using abd::gl::uniform_block;

/**
	Creates a null uniform
*/
uniform::uniform() :
	m_program(0),
	m_location(-1),
	m_type(0),
	m_size(0),
	m_block_index(0),
	m_offset(0),
	m_array_stride(0),
	m_matrix_stride(0),
	m_is_row_major(false)
{
}

/**
	Creates a uniform reference based on data provided by program class
*/
uniform::uniform(
		GLuint program,
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
