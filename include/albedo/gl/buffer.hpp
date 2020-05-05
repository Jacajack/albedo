#pragma once

#include <albedo/gl/gl_object.hpp>
#include <memory>
#include <vector>

namespace abd::gl {

/**
	Represents an OpenGL buffer
*/
class buffer : public gl_object<gl_object_type::BUFFER>
{
public:
	buffer(GLsizeiptr size, const void *data, GLbitfield flags);

	void bind(GLenum target) const;

	void write(GLintptr offset, GLsizeiptr size, const void *data);
	void read(GLintptr offset, GLsizeiptr size, void *data) const;
};

template <typename T>
static std::unique_ptr<abd::gl::buffer> vector_to_buffer(const std::vector<T> &v, GLbitfield flags)
{
	return std::make_unique<abd::gl::buffer>(v.size() * sizeof(T), v.data(), flags);
}

}