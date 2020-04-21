#include <albedo/gl/texture.hpp>

using abd::gl::texture;
using abd::gl::texture_1d;
using abd::gl::texture_2d;
using abd::gl::texture_3d;

texture::texture(texture_target target) :
	gl_object<gl_object_type::TEXTURE>(static_cast<GLenum>(target)),
	m_target(target)
{
}

void texture::storage_1d(GLsizei levels, GLenum internalformat, GLsizei width)
{
	glTextureStorage1D(*this, levels, internalformat, width);
}

void texture::storage_2d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
	glTextureStorage2D(*this, levels, internalformat, width, height);
}

void texture::storage_2d_multisample(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
{
	glTextureStorage2DMultisample(*this, levels, internalformat, width, height, fixedsamplelocations);
}

void texture::storage_3d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
	glTextureStorage3D(*this, levels, internalformat, width, height, depth);
}

void texture::subimage_1d(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels)
{
	glTextureSubImage1D(*this, level, xoffset, width, format, type, pixels);
}

void texture::subimage_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
	glTextureSubImage2D(*this, level, xoffset, yoffset, width, height, format, type, pixels);
}

void texture::subimage_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	glTextureSubImage3D(*this, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}


texture_1d::texture_1d(texture_target_1d target, GLsizei levels, GLenum internalformat, GLsizei width) :
	texture(static_cast<texture_target>(target))
{
	storage_1d(levels, internalformat, width);
}

texture_2d::texture_2d(texture_target_2d target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) :
	texture(static_cast<texture_target>(target))
{
	storage_2d(levels, internalformat, width, height);
}

texture_3d::texture_3d(texture_target_3d target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) :
	texture(static_cast<texture_target>(target))
{
	storage_3d(levels, internalformat, width, height, depth);
}