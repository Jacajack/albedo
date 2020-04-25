#include <albedo/gl/vertex_array.hpp>
#include <albedo/exception.hpp>
#include <algorithm>

using abd::gl::vertex_array;
using abd::gl::vao_attribute;
using abd::gl::vao;


void vertex_array::bind_buffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
{
	glVertexArrayVertexBuffer(*this, bindingindex, buffer, offset, stride);
}

void vertex_array::bind_buffers(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides)
{
	glVertexArrayVertexBuffers(*this, first, count, buffers, offsets, strides);
}

void vertex_array::set_attribute_format(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	glVertexArrayAttribFormat(*this, attribindex, size, type, normalized, relativeoffset);
}

void vertex_array::set_attribute_binding(GLuint attribindex, GLuint bindingindex)
{
	glVertexArrayAttribBinding(*this, attribindex, bindingindex);
}

void vertex_array::set_attribute_enable(GLuint attribindex, bool enable)
{
	if (enable)
		glEnableVertexArrayAttrib(*this, attribindex);
	else
		glDisableVertexArrayAttrib(*this, attribindex);
}

void vertex_array::bind() const
{
	glBindVertexArray(*this);
}

/**
	Creates a new VAO attribute. Initially, a buffer binding with the same index is used
	and the attribute is enabled.
*/
vao_attribute::vao_attribute(std::weak_ptr<vao_control_block> cont, GLuint index) :
	m_index(index),
	m_control_block_ptr(cont)
{
	// Try to get a shared pointer
	std::shared_ptr control_block{m_control_block_ptr};

	// Check if the attribute is taken
	auto &attribs = control_block->used_attributes;
	auto it = std::find(attribs.begin(), attribs.end(), index);
	
	// Fine, register attribute
	if (it == attribs.end())
	{
		attribs.push_back(index);
	}
	else
	{
		throw abd::exception("attempted to create aliasing vao_attribute");
	}

	// Set default binding and enable
	set_binding(index);
	set_enable(true);
}

/**
	Move constructor - invalidates source
*/
vao_attribute::vao_attribute(vao_attribute &&rhs) :
	m_index(rhs.m_index),
	m_binding(rhs.m_binding),
	m_control_block_ptr(std::move(rhs.m_control_block_ptr))
{
	// Invalidate source's pointer
	rhs.m_control_block_ptr.reset();
}

/**
	Move assignment operator - invalidates source
*/
vao_attribute &vao_attribute::operator=(vao_attribute &&rhs)
{
	if (this == &rhs) return *this;
	m_index = rhs.m_index;
	m_binding = rhs.m_binding;
	m_control_block_ptr = std::move(rhs.m_control_block_ptr);
	rhs.m_control_block_ptr.reset();
	return *this;
}

/**
	Creates a new VAO attribute. Initially, a buffer binding with the same index is used
	and the attribute is enabled.
*/
vao_attribute::~vao_attribute()
{
	// Try to get the shared pointer
	std::shared_ptr control_block = m_control_block_ptr.lock();
	if (!control_block) return;

	// Check if the attribute is taken
	auto &attribs = control_block->used_attributes;
	auto it = std::find(attribs.begin(), attribs.end(), id());
	
	// Unregister the attribute
	if (it != attribs.end())
	{
		*it = std::move(attribs.back());
		attribs.pop_back();
	}
}

/**
	Performs all-in-one VAO attribute configuration.
	Does not affect enable status.
*/
void vao_attribute::configure(const vao_attribute_config &config)
{
	set_binding(config.bindingindex);
	set_format(config.size, config.type, config.normalized, config.relativeoffset);
}

void vao_attribute::set_format(GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	// Try to acquire a shared pointer
	std::shared_ptr control_block{m_control_block_ptr};
	control_block->vao.set_attribute_format(id(), size, type, normalized, relativeoffset);
}

void vao_attribute::set_binding(GLuint bindingindex)
{
	// Try to acquire a shared pointer
	std::shared_ptr control_block{m_control_block_ptr};
	control_block->vao.set_attribute_binding(id(), bindingindex);
}

bool vao_attribute::set_enable(bool enable)
{
	// Try to acquire a shared pointer
	std::shared_ptr control_block{m_control_block_ptr};
	control_block->vao.set_attribute_enable(id(), enable);
	return enable;
}


/**
	Initializes a new VAO object
*/
vao::vao() :
	m_control_block_ptr(new vao_control_block)
{
}

/**
	Returns a new attribute handle
*/
vao_attribute vao::get_attribute(GLuint index)
{
	return vao_attribute(m_control_block_ptr, index);
}

/**
	Binds a buffer to the VAO
*/
void vao::bind_buffer(GLuint bindingindex, const gl::buffer &buffer, GLintptr offset, GLsizei stride)
{
	m_control_block_ptr->vao.bind_buffer(bindingindex, buffer, offset, stride);
}

void vao::bind_buffer(GLuint bindingindex, const gl::buffer &buffer, const buffer_data_layout &data_layout)
{
	this->bind_buffer(bindingindex, buffer, data_layout.offset, data_layout.stride);
}