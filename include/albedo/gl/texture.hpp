#pragma once 

#include <albedo/gl/gl_object.hpp>
#include <albedo/gl/buffer.hpp>

namespace abd::gl {

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

template <texture_target Ttarget>
struct texture_target_traits;

template <>
struct texture_target_traits<texture_target::TEXTURE_2D>
{
	static constexpr bool is_multisample = false;
	static constexpr bool is_layered = false;
	static constexpr int dimensions = 2;
	static constexpr int storage_dimensions = dimensions + is_layered;
};



enum class texture_format
{
	UNDEFINED = 0,

	RGBA32F = GL_RGBA32F,
	RGB32F = GL_RGB32F,
	RG32F = GL_RG32F,
	R32F = GL_R32F,

	RGBA16F = GL_RGBA16F,
	RGB16F = GL_RGB16F,
	RG16F = GL_RG16F,
	R16F = GL_R16F,
	
	RGBA8 = GL_RGBA8,
	RGB8 = GL_RGB8,
	RG8 = GL_RG8,
	R8 = GL_R8,

	DEPTH_32F = GL_DEPTH_COMPONENT32F,
	DEPTH_32 = GL_DEPTH_COMPONENT32
};

/**
	Represents any type of OpenGL texture.
*/
template <texture_target Ttarget>
class texture : public gl_object<gl_object_type::TEXTURE>
{
public:
	texture();

	void bind_texture(int unit);
	void bind_image(GLuint unit, GLint level, GLenum access);
	void bind_image(GLuint unit, GLint level, GLint layer, GLenum access);


	inline texture_target get_target() const;

	void attach_buffer(const abd::gl::buffer &buffer, GLenum internalformat);

	void storage_1d(abd::gl::texture_format internalformat, GLsizei width, GLsizei levels = 1);
	void storage_2d(abd::gl::texture_format internalformat, GLsizei width, GLsizei height, GLsizei levels = 1);
	void storage_3d(abd::gl::texture_format internalformat, GLsizei width, GLsizei height, GLsizei depth, GLsizei levels = 1);
	void storage_2d_multisample(abd::gl::texture_format internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations, GLsizei levels = 1);

	void subimage_1d(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
	void subimage_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	void subimage_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);

	void set_min_filter(GLenum type);
	void set_mag_filter(GLenum type);

	void set_base_level(GLint level);
	void set_max_level(GLint level);
	void generate_mipmap();

	void set_wrap_s(GLenum wrap);
	void set_wrap_t(GLenum wrap);
	void set_wrap_r(GLenum wrap);


private:
	texture_format m_format = texture_format::UNDEFINED;
	texture_target m_target;
	GLuint m_layers = 0;
};

template <texture_target Ttarget>
texture_target texture<Ttarget>::get_target() const
{
	return m_target;
}

template <texture_target Ttarget>
texture<Ttarget>::texture() :
	gl_object<gl_object_type::TEXTURE>(static_cast<GLenum>(Ttarget)),
	m_target(Ttarget)
{
}

template <texture_target Ttarget>
void texture<Ttarget>::bind_texture(int unit)
{
	glBindTextureUnit(unit, *this);
}

/**
	For non-layered textures - always binds layer 0.
	For layered textures - binds all layers
*/
template <texture_target Ttarget>
void texture<Ttarget>::bind_image(GLuint unit, GLint level, GLenum access)
{
	glBindTextureImage(unit, *this, level, texture_target_traits<Ttarget>::is_layered, 0, access, m_format);
}

/**
	Binds one layer of a layered texture
*/
template <texture_target Ttarget>
void texture<Ttarget>::bind_image(GLuint unit, GLint level, GLint layer, GLenum access)
{
	static_assert(texture_target_traits<Ttarget>::is_layered, "Cannot use this on non-layered texture!");
	glBindTextureImage(unit, *this, level, GL_FALSE, layer, access, m_format);
}

template <texture_target Ttarget>
void texture<Ttarget>::attach_buffer(const abd::gl::buffer &buffer, GLenum internalforamt)
{
	static_assert(Ttarget == texture_target::TEXTURE_BUFFER, "Not a buffer texture target!");
	glTextureBuffer(*this, internalforamt, buffer);
}

template <texture_target Ttarget>
void texture<Ttarget>::storage_1d(abd::gl::texture_format internalformat, GLsizei width, GLsizei levels)
{
	static_assert(texture_target_traits<Ttarget>::storage_dimensions == 1, "Cannot create texture storage with this function!");
	glTextureStorage1D(*this, levels, static_cast<GLenum>(internalformat), width);
}

template <texture_target Ttarget>
void texture<Ttarget>::storage_2d(abd::gl::texture_format internalformat, GLsizei width, GLsizei height, GLsizei levels)
{
	static_assert(texture_target_traits<Ttarget>::storage_dimensions == 2, "Cannot create texture storage with this function!");
	glTextureStorage2D(*this, levels, static_cast<GLenum>(internalformat), width, height);
}

template <texture_target Ttarget>
void texture<Ttarget>::storage_3d(abd::gl::texture_format internalformat, GLsizei width, GLsizei height, GLsizei depth, GLsizei levels)
{
	static_assert(texture_target_traits<Ttarget>::storage_dimensions == 3, "Cannot create texture storage with this function!");
	glTextureStorage3D(*this, levels, static_cast<GLenum>(internalformat), width, height, depth);
}

template <texture_target Ttarget>
void texture<Ttarget>::storage_2d_multisample(abd::gl::texture_format internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations, GLsizei levels)
{
	static_assert(Ttarget == texture_target::TEXTURE_2D_MULTISAMPLE, "Target is not a multisample texture!");
	glTextureStorage2DMultisample(*this, levels, static_cast<GLenum>(internalformat), width, height, fixedsamplelocations);
}

template <texture_target Ttarget>
void texture<Ttarget>::subimage_1d(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels)
{
	static_assert(texture_target_traits<Ttarget>::storage_dimensions == 1, "Cannot provide 1D data for this texture!");
	glTextureSubImage1D(*this, level, xoffset, width, format, type, pixels);
}

template <texture_target Ttarget>
void texture<Ttarget>::subimage_2d(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
	static_assert(texture_target_traits<Ttarget>::storage_dimensions == 2, "Cannot provide 2D data for this texture!");
	glTextureSubImage2D(*this, level, xoffset, yoffset, width, height, format, type, pixels);
}

template <texture_target Ttarget>
void texture<Ttarget>::subimage_3d(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	static_assert(texture_target_traits<Ttarget>::storage_dimensions == 3, "Cannot provide 3D data for this texture!");
	glTextureSubImage3D(*this, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_min_filter(GLenum type)
{
	set_parameter<GLint>(GL_TEXTURE_MIN_FILTER, type);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_mag_filter(GLenum type)
{
	set_parameter<GLint>(GL_TEXTURE_MAG_FILTER, type);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_base_level(GLint level)
{
	set_parameter<GLint>(GL_TEXTURE_BASE_LEVEL, level);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_max_level(GLint level)
{
	set_parameter<GLint>(GL_TEXTURE_MAX_LEVEL, level);
}

template <texture_target Ttarget>
void texture<Ttarget>::generate_mipmap()
{
	glGenerateTextureMipmap(*this);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_wrap_s(GLenum wrap)
{
	set_parameter<GLint>(GL_TEXTURE_WRAP_S, wrap);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_wrap_t(GLenum wrap)
{
	static_assert(texture_target_traits<Ttarget>::dimensions >= 2, "T wrapping is not available for this texture type");
	set_parameter<GLint>(GL_TEXTURE_WRAP_T, wrap);
}

template <texture_target Ttarget>
void texture<Ttarget>::set_wrap_r(GLenum wrap)
{
	static_assert(texture_target_traits<Ttarget>::dimensions == 3, "T wrapping is not available for this texture type");
	set_parameter<GLint>(GL_TEXTURE_WRAP_R, wrap);
}



} // namespace abd::gl