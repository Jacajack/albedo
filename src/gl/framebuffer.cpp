#include <albedo/gl/framebuffer.hpp>

using abd::gl::framebuffer;

void framebuffer::attach_texture(GLenum attachment, gl::texture<gl::texture_target::TEXTURE_2D> &texture, GLint level)
{
	glNamedFramebufferTexture(id(), attachment, texture, level);
}

GLenum framebuffer::get_status(GLenum target) const
{
	return glCheckNamedFramebufferStatus(id(), target);
}

bool framebuffer::is_complete(GLenum target) const
{
	return get_status() == GL_FRAMEBUFFER_COMPLETE;
}

void framebuffer::set_draw_buffers(GLsizei n, const GLenum *buffers)
{
	glNamedFramebufferDrawBuffers(id(), n, buffers);
}

void framebuffer::set_draw_buffers(std::initializer_list<GLenum> buffers)
{
	GLenum arr[buffers.size()];
	std::copy(buffers.begin(), buffers.end(), arr);
	set_draw_buffers(buffers.size(), arr);
}
