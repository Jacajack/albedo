#include <albedo/gl/buffer.hpp>
#include <albedo/exception.hpp>

using abd::gl::buffer;

/**
    Creates a buffer with predefined storage size
*/
buffer::buffer(GLsizeiptr size, const void *data, GLbitfield flags)
{
	glNamedBufferStorage(*this, size, data, flags);
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
