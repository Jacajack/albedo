#include <albedo/gl/synced_buffer.hpp>

using abd::gl::synced_buffer;

synced_buffer::synced_buffer(GLsizeiptr chunk_size, GLbitfield flags, int chunk_count) :
	m_chunk_size(chunk_size),
	m_chunk_count(chunk_count),
	m_buffer(chunk_size * chunk_count, nullptr, flags | GL_MAP_PERSISTENT_BIT),
	m_fences(chunk_count)
{
	if (flags & GL_MAP_WRITE_BIT) flags |= GL_MAP_FLUSH_EXPLICIT_BIT;
	m_map_ptr = glMapNamedBufferRange(m_buffer, 0, chunk_size * chunk_count, flags | GL_MAP_PERSISTENT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	if (m_map_ptr == nullptr)
		throw abd::exception("abd::gl::synced_buffer failed to map the buffer");
}

synced_buffer::~synced_buffer()
{
	glUnmapNamedBuffer(m_buffer);
}

abd::gl::synced_buffer_handle synced_buffer::get_chunk()
{
	// Wait for the fence (and implicitly destroy it)
	m_fences[m_current_chunk].wait();

	// Create a handle
	synced_buffer_handle handle(this, m_current_chunk);

	// Increment chunk id
	m_current_chunk = (m_current_chunk + 1) % m_chunk_count;

	return handle;
}