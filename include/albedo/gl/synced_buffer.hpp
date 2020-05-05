#pragma once

#include <albedo/gl/fence.hpp>
#include <albedo/gl/buffer.hpp>

namespace abd::gl {

class synced_buffer_handle;

/**
	Upon request, returns synced buffer handles to chunks that
	are guaranteed not to be in use by the GPU.
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
	inline gl::buffer &get_buffer();
	inline GLsizeiptr get_chunk_size() const;
	inline int get_chunk_count() const;
	inline void *get_map_ptr();

private:
	GLsizeiptr m_chunk_size;
	int m_chunk_count;
	int m_current_chunk = 0;

	gl::buffer m_buffer;
	std::vector<gl::fence_sync> m_fences;
	void* m_map_ptr = nullptr;
};

gl::buffer &synced_buffer::get_buffer()
{
	return m_buffer;
}

GLsizeiptr synced_buffer::get_chunk_size() const
{
	return m_chunk_size;
}

int synced_buffer::get_chunk_count() const
{
	return m_chunk_count;
}

void *synced_buffer::get_map_ptr()
{
	return m_map_ptr;
}

/**
	Represents a handle to a buffer chunk.
*/
class synced_buffer_handle
{
	friend class synced_buffer;

public:
	inline GLsizeiptr get_chunk_size() const;
	inline GLsizeiptr get_chunk_offset() const;
	inline int get_chunk_index() const;

	inline void *get_chunk_ptr();
	inline gl::synced_buffer &get_synced_buffer();
	inline gl::fence_sync &get_fence();

	inline void fence();

private:
	inline synced_buffer_handle(GLsizeiptr chunk_size, int chunk_id, void *map, gl::synced_buffer *buf, gl::fence_sync *fence);

	GLsizeiptr m_chunk_size;
	int m_chunk_index;
	void *m_map_ptr;
	gl::synced_buffer *m_buffer;
	gl::fence_sync *m_fence;
};

synced_buffer_handle::synced_buffer_handle(GLsizeiptr chunk_size, int chunk_id, void *map, gl::synced_buffer *buf, gl::fence_sync *fence) :
	m_chunk_size(chunk_size),
	m_chunk_index(chunk_id),
	m_map_ptr(map),
	m_buffer(buf),
	m_fence(fence)
{
}

GLsizeiptr synced_buffer_handle::get_chunk_size() const
{
	return m_chunk_size;
}

GLsizeiptr synced_buffer_handle::get_chunk_offset() const
{
	return m_chunk_size * m_chunk_index;
}

int synced_buffer_handle::get_chunk_index() const
{
	return m_chunk_index;
}

void *synced_buffer_handle::get_chunk_ptr()
{
	return static_cast<char*>(m_map_ptr) + get_chunk_offset();
}

gl::synced_buffer &synced_buffer_handle::get_synced_buffer()
{
	return *m_buffer;
}

gl::fence_sync &synced_buffer_handle::get_fence()
{
	return *m_fence;
}

void synced_buffer_handle::fence()
{
	m_fence->fence();
}

}