#pragma once 

#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/buffer.hpp>

namespace abd {
namespace gl {

enum class texture_target
{
	TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
	TEXTURE_1D = GL_TEXTURE_1D,
	TEXTURE_2D = GL_TEXTURE_2D,
	TEXTURE_3D = GL_TEXTURE_3D,
	TEXTURE_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
	TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
	TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	TEXTURE_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
	TEXTURE_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
	TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
};

enum class texture_target_1d
{
	TEXTURE_BUFFER = static_cast<int>(texture_target::TEXTURE_BUFFER),
	TEXTURE_1D = static_cast<int>(texture_target::TEXTURE_1D),
};

enum class texture_target_2d
{
	TEXTURE_2D = static_cast<int>(texture_target::TEXTURE_2D),
	TEXTURE_1D_ARRAY = static_cast<int>(texture_target::TEXTURE_1D_ARRAY),
	TEXTURE_2D_MULTISAMPLE = static_cast<int>(texture_target::TEXTURE_2D_MULTISAMPLE),
};

enum class texture_target_3d
{
	TEXTURE_3D = static_cast<int>(texture_target::TEXTURE_3D),
	TEXTURE_2D_ARRAY = static_cast<int>(texture_target::TEXTURE_2D_ARRAY),
	TEXTURE_CUBE_MAP = static_cast<int>(texture_target::TEXTURE_CUBE_MAP),
	TEXTURE_CUBE_MAP_ARRAY = static_cast<int>(texture_target::TEXTURE_CUBE_MAP_ARRAY),
	TEXTURE_2D_MULTISAMPLE_ARRAY = static_cast<int>(texture_target::TEXTURE_2D_MULTISAMPLE_ARRAY),
};

/**
	Represents any type of OpenGL texture.
*/
class texture : public gl_object<gl_object_type::TEXTURE>
{
public:
	texture(texture_target target);

	void bind(int unit);
	void generate_mipmap();

	void attach_buffer(const abd::gl::buffer &buffer, GLenum internalforamt);

	void storage_1d(GLsizei levels, GLenum internalformat, GLsizei width);
	void storage_2d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
	void storage_2d_multisample(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	void storage_3d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

	void subimage_1d(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
	void subimage_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	void subimage_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);

	texture_target get_target() const;

private:
	texture_target m_target;
};

inline texture_target texture::get_target() const
{
	return m_target;
}

/**
	Respresents 1D textures. All incompatible functions are deleted.
	Storage must be initialized during construction.
*/
class texture_1d : public texture
{
public:
	texture_1d(texture_target_1d target, GLsizei levels, GLenum internalformat, GLsizei width);

	// Deleted members
	void storage_2d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) = delete;
	void storage_2d_multisample(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) = delete;
	void storage_3d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) = delete;
	void subimage_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) = delete;
	void subimage_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels) = delete;

private:
	void attach_buffer(const abd::gl::buffer &buffer, GLenum internalforamt);
	void storage_1d(GLsizei levels, GLenum internalformat, GLsizei width);
};

/**
	Respresents 2D textures. All incompatible functions are deleted.
	Storage must be initialized during construction.
*/
class texture_2d : public texture
{
public:
	texture_2d(texture_target_2d target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);

	// Deleted members
	void attach_buffer(const abd::gl::buffer &buffer, GLenum internalforamt) = delete;
	void storage_1d(GLsizei levels, GLenum internalformat, GLsizei width) = delete;
	void storage_2d_multisample(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) = delete;
	void storage_3d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) = delete;
	void subimage_1d(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels) = delete;
	void subimage_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels) = delete;

private:
	void storage_2d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
};

/**
	Respresents 3D textures. All incompatible functions are deleted.
	Storage must be initialized during construction.
*/
class texture_3d : public texture
{
public:
	texture_3d(texture_target_3d target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

	// Deleted members
	void attach_buffer(const abd::gl::buffer &buffer, GLenum internalforamt) = delete;
	void storage_1d(GLsizei levels, GLenum internalformat, GLsizei width) = delete;
	void storage_2d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) = delete;
	void storage_2d_multisample(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) = delete;
	void subimage_1d(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels) = delete;
	void subimage_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) = delete;

private:
	void storage_3d(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
};

}
}