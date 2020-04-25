#pragma once 

#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/buffer.hpp>
#include <memory>
#include <list>
#include <vector>
#include <mutex>

namespace abd {
namespace gl {


/**
	A wrapper for OpenGL's Vertex Array Object. All member functions correspond to OpenGL VAO functions
*/
class vertex_array : public abd::gl::gl_object<abd::gl::gl_object_type::VERTEX_ARRAY>
{
public:
	void bind_buffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
	void bind_buffers(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
	void set_attribute_format(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
	void set_attribute_binding(GLuint attribindex, GLuint bindingindex);
	void set_attribute_enable(GLuint attribindex, bool enable);
	void bind() const;
};


/**
	Contains a VAO wrapper and a list of currently used attributes
*/
struct vao_control_block
{
	std::vector<GLuint> used_attributes;
	vertex_array vao;
};

/**
	Represents all parameters needed to fully set up a VAO attribute
*/
struct vao_attribute_config
{
	GLuint bindingindex;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLuint relativeoffset;
};

/**
	Determines data layout inside a buffer
*/
struct buffer_data_layout
{
	GLintptr offset;
	GLsizei stride;
};

/**
	Represents a VAO attribute. Follows RAII principles
*/
class vao_attribute : abd::noncopy
{
	friend class vao;

public:
	vao_attribute(vao_attribute &&rhs);
	vao_attribute &operator=(vao_attribute &&rhs);
	~vao_attribute();

	void configure(const vao_attribute_config &config);
	void set_format(GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
	void set_binding(GLuint bindingindex);
	bool set_enable(bool enable);
	
	GLuint id() const
	{
		return m_index;
	}

	operator GLuint() const
	{
		return id();
	}

private:
	explicit vao_attribute(std::weak_ptr<vao_control_block> cont, GLuint index);

	GLuint m_index;
	GLuint m_binding = 0;
	std::weak_ptr<vao_control_block> m_control_block_ptr;
};

/**
	This class provides further abstraction from the OpenGL's VAO.
	All bindings and attributes are RAII-conformant.
*/
class vao : abd::noncopy
{
public:
	vao();

	vao_attribute get_attribute(GLuint index);
	void bind_buffer(GLuint bindingindex, const gl::buffer &buffer, GLintptr offset, GLsizei stride);
	void bind_buffer(GLuint bindingindex, const gl::buffer &buffer, const buffer_data_layout &data_layout);

	void bind() const
	{
		m_control_block_ptr->vao.bind();
	}

	GLuint id() const
	{
		return m_control_block_ptr->vao.id();
	}

	operator GLuint() const
	{
		return id();
	}

private:
	std::shared_ptr<vao_control_block> m_control_block_ptr;
};


}
}