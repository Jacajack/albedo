#pragma once

#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/texture.hpp>

namespace abd {
namespace gl {

/**
	Very simple framebuffer wrapper.

	\todo mulitlayer texture attachments, copying blitting and stuff
*/
class framebuffer : public gl_object<gl_object_type::FRAMEBUFFER>
{
public:
	void attach_texture(GLenum attachment, gl::texture<gl::texture_target::TEXTURE_2D> &texture, GLint level = 0);

	// void attach_color_buffer();
	// void attach_depth_buffer();
	// void attach_stencil_buffer();

	GLenum get_status(GLenum target = GL_DRAW_FRAMEBUFFER) const;
	bool is_complete(GLenum target = GL_DRAW_FRAMEBUFFER) const;

	void set_draw_buffers(GLsizei n, const GLenum *buffers);
	void set_draw_buffers(std::initializer_list<GLenum> buffers);
};

}
}