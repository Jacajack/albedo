#include <albedo/gl/buffer.hpp>
#include <albedo/exception.hpp>

using abd::gl::buffer_mapping;
using abd::gl::buffer;

/**
	Maps the buffer with glMapNamedBufferRange
*/
buffer_mapping::buffer_mapping(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) :
	m_id(buffer),
	m_data(nullptr)
{
	m_data = glMapNamedBufferRange(buffer, offset, length, access);
}

/**
	Try to unmap on destruction
*/
buffer_mapping::~buffer_mapping()
{
	unmap();
}

/**
	Unmaps the mapping - calls either when mapping is destroyed
	or the buffer is destroyed
*/
void buffer_mapping::unmap()
{
	if (m_id)
	{
		glUnmapBuffer(m_id);
		m_id = 0;
	}
}

/**
    Creates a buffer with predefined storage size
*/
buffer::buffer(GLsizeiptr size, const void *data, GLbitfield flags)
{
	glNamedBufferStorage(*this, size, data, flags);
}

/**
    Performs normal destruction but also makes sure that
    the buffer is unmapped first
*/
buffer::~buffer()
{
	auto mapping = m_mapping_ptr.lock();
	if (mapping)
		mapping->unmap();
}

/**
    Binds the buffer to specified target
*/
void buffer::bind(GLenum target) const
{
	glBindBuffer(target, *this);
}

/**
    Writes data to the buffer
*/
void buffer::write(GLintptr offset, GLsizeiptr size, const void *data)
{
	glNamedBufferSubData(*this, offset, size, data);
}

/**
    Reads data from the buffer
*/
void buffer::read(GLintptr offset, GLsizeiptr size, void *data) const
{
	glGetBufferSubData(*this, offset, size, data);
}

/**
    Map the buffer to a pointer
*/
std::shared_ptr<buffer_mapping> buffer::map(GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	auto mapping = m_mapping_ptr.lock();
	if (mapping)
		throw abd::exception("abd::gl::buffer::map() - already mapped");

	mapping = std::shared_ptr<buffer_mapping>{new buffer_mapping{*this, offset, length, access}};
	m_mapping_ptr = mapping;
	return mapping;
}

