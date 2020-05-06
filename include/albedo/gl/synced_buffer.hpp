#pragma once

#include <albedo/gl/fence.hpp>
#include <albedo/gl/buffer.hpp>

namespace abd::gl {

class synced_buffer_handle;

/**
	Upon request, returns handles to buffer chunks that
	are guaranteed not to be in use by the GPU.

	This class is optimized for writing the mapped memory.
*/
class synced_buffer
{
public:
	synced_buffer(GLsizeiptr chunk_size, GLbitfield flags, int chunk_count = 3);
	~synced_buffer();

	synced_buffer(const synced_buffer &) = delete;
	synced_buffer &operator=(const synced_buffer &) = delete;

	synced_buffer(synced_buffer &&) = default;
	synced_buffer &operator=(synced_buffer &&) = default;

	synced_buffer_handle get_chunk();
	inline GLsizeiptr get_chunk_size() const;
	inline int get_chunk_count() const;

	inline gl::fence_sync &get_fence(int index = 0);
	inline gl::buffer &get_buffer(int index = 0);
	inline void *get_map_ptr(int index = 0);

private:
	GLsizeiptr m_chunk_size;
	int m_chunk_count;
	int m_current_chunk = 0;

	gl::buffer m_buffer;
	std::vector<gl::fence_sync> m_fences;
	void* m_map_ptr = nullptr;
};

gl::buffer &synced_buffer::get_buffer(int index)
{
	return m_buffer;
}

gl::fence_sync &synced_buffer::get_fence(int index)
{
	return m_fences.at(index);
}

GLsizeiptr synced_buffer::get_chunk_size() const
{
	return m_chunk_size;
}

int synced_buffer::get_chunk_count() const
{
	return m_chunk_count;
}

/**
	Returns a pointer to mapped buffer memory
*/
void *synced_buffer::get_map_ptr(int index)
{
	return m_map_ptr;
}

/**
	Represents a handle to a buffer chunk.

	This may be a handle to a separate buffer object, if the
	client does not support ARB_buffer_storage.
*/
class synced_buffer_handle
{
	friend class synced_buffer;

public:
	inline GLsizeiptr get_size() const;
	inline GLsizeiptr get_offset() const;
	inline int get_index() const;

	inline void *get_ptr();
	inline gl::synced_buffer &get_synced_buffer();
	inline gl::fence_sync &get_fence();
	inline gl::buffer &get_buffer();

	inline void flush();
	inline void fence();

private:
	inline synced_buffer_handle(gl::synced_buffer *buf, int index);

	int m_index;
	gl::synced_buffer *m_synced_buffer;
};

synced_buffer_handle::synced_buffer_handle(gl::synced_buffer *buf, int index) :
	m_index(index),
	m_synced_buffer(buf)
{
}

GLsizeiptr synced_buffer_handle::get_size() const
{
	return m_synced_buffer->get_chunk_size();
}

/**
	Reurns offset relative to the beginning of the buffer
	returned by get_buffer() (and the buffer's BASE map pointer)
*/
GLsizeiptr synced_buffer_handle::get_offset() const
{
	return get_size() * m_index;
}

/**
	Returns chunk index within synced_buffer
*/
int synced_buffer_handle::get_index() const
{
	return m_index;
}

/**
	Returns pointer to the mapped buffer data (not the buffer beginning)
*/
void *synced_buffer_handle::get_ptr()
{
	return static_cast<char*>(m_synced_buffer->get_map_ptr()) + get_offset();
}

gl::synced_buffer &synced_buffer_handle::get_synced_buffer()
{
	return *m_synced_buffer;
}

/**
	Returns a reference to the OpenGL buffer this handle
	is associated with
*/
gl::buffer &synced_buffer_handle::get_buffer()
{
	return m_synced_buffer->get_buffer(m_index);
}

gl::fence_sync &synced_buffer_handle::get_fence()
{
	return m_synced_buffer->get_fence(m_index);
}

/**
	Flushes mapped memory region after writing
*/
void synced_buffer_handle::flush()
{
	glFlushMappedNamedBufferRange(get_buffer(), get_offset(), get_size());
}

void synced_buffer_handle::fence()
{
	get_fence().fence();
}

}