#pragma once

#include <albedo/gl/gl_object.hpp>
#include <memory>
#include <vector>

namespace abd {
namespace gl {

/**
	Represents a buffer mapped to memory region
*/
class buffer_mapping : abd::noncopy
{
	friend class buffer;

public:
	~buffer_mapping();

	void *data()
	{
		return m_data;
	}

private:
	buffer_mapping(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
	void unmap();

	GLuint m_id;
	void *m_data;
};

/**
	Represents an OpenGL buffer
	\todo data copying
*/
class buffer : public gl_object<gl_object_type::BUFFER>
{
public:
	buffer(GLsizeiptr size, const void *data, GLbitfield flags);
	virtual ~buffer();

	void bind(GLenum target) const;

	void write(GLintptr offset, GLsizeiptr size, const void *data);
	void read(GLintptr offset, GLsizeiptr size, void *data) const;

	std::shared_ptr<buffer_mapping> map(GLintptr offset, GLsizeiptr length, GLbitfield access);

private:
	std::weak_ptr<buffer_mapping> m_mapping_ptr;
};

template <typename T>
static std::unique_ptr<abd::gl::buffer> vector_to_buffer(const std::vector<T> &v, GLbitfield flags)
{
	return std::make_unique<abd::gl::buffer>(v.size() * sizeof(T), v.data(), flags);
}


}
}