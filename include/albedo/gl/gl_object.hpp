#pragma once

#include <type_traits>
#include <string>
#include <albedo/gl/gl.hpp>

namespace abd
{
namespace gl
{

/**
    OpenGL object types. There are 11 values, so 4 bits are enough to store them
    (it's important when creating hashes).
*/
enum class gl_object_type
{
	BUFFER,
	TEXTURE,
	SAMPLER,
	VERTEX_ARRAY,
	FRAMEBUFFER,
	RENDERBUFFER,
	QUERY,
	SHADER,
	PROGRAM,
	PROGRAM_PIPELINE,
	TRANSFORM_FEEDBACK
};

/**
    Associates OpenGL object types (local enums) with their traits.
    Specializations are at the bottom of the file.
*/
template <gl_object_type T>
struct gl_object_traits
{
};

/**
    \brief Serves as a RAII wrapper for OpenGL objects
    \note This class provides no OpenGL error checking - this is up to derived classes.
    \template T is object type - see gl_object_type
*/
template <gl_object_type T>
class gl_object
{
public:
	inline gl_object();
	inline explicit gl_object(GLenum target);

	inline ~gl_object();

	// Deleted copy constructor and copy assignment operator
	gl_object(const gl_object &src) = delete;
	gl_object &operator=(const gl_object &rhs) = delete;

	// Move semantics with source invalidation
	gl_object(gl_object &&src);
	gl_object &operator=(gl_object &&rhs);

	//! Allows retrieveing object's parameters
	template <typename Tv>
	inline Tv get_parameter(GLenum parameter);

	//! Allows retrieveing object's parameters
	template <typename Tv>
	inline void set_parameter(GLenum parameter, Tv val);

	//! Adds a label to the object
	void label(const std::string &name);

	//! Removes object's label
	void label();

	//! Allows casting to object's ID (GLuint)
	inline operator GLuint() const noexcept;

	//! Returns object's ID
	inline GLuint id() const noexcept;

	//! Returns unique object hash
	std::uint64_t hash() const noexcept;

protected:
	//! The object ID
	GLuint m_id;

private:
	//! Incremented on each glCreate*() call for certain object type
	static std::uint64_t m_creation_counter;
};

template <gl_object_type T>
std::uint64_t gl_object<T>::m_creation_counter = 0;

//! Move constructor with source invalidation
template <gl_object_type T>
gl_object<T>::gl_object(gl_object<T> &&src) :
	m_id(src.m_id)
{
	src.m_id = 0;
}

//! Move assignment operator with source invalidation
template <gl_object_type T>
gl_object<T> &gl_object<T>::operator=(gl_object<T> &&rhs)
{
	// Prevent self-move
	if (this != &rhs)
	{
		m_id = rhs.m_id;
		rhs.m_id = 0;
	}

	return *this;
}

// Adds a label
template <gl_object_type T>
void gl_object<T>::label(const std::string &name)
{
	glObjectLabel(gl_object_traits<T>::gl_type, m_id, -1, name.c_str());
}

// Removes label
template <gl_object_type T>
void gl_object<T>::label()
{
	glObjectLabel(gl_object_traits<T>::gl_type, m_id, 0, nullptr);
}

// Allows implicit cast to GLuint
template <gl_object_type T>
inline gl_object<T>::operator GLuint() const noexcept
{
	return m_id;
}

// Used for acquiring object's ID
template <gl_object_type T>
inline GLuint gl_object<T>::id() const noexcept
{
	return m_id;
}

// Computes a hash from creation counter and object type ID
template <gl_object_type T>
std::uint64_t gl_object<T>::hash() const noexcept
{
	return (static_cast<std::uint64_t>(T) << 60) | m_creation_counter;
}


/////////////////////////// Specializations for GL_BUFFER ///////////////////////////

template <>
inline gl_object<gl_object_type::BUFFER>::gl_object()
{
	glCreateBuffers(1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::BUFFER>::~gl_object()
{
	glDeleteBuffers(1, &m_id);
}

template <>
template <>
inline GLint gl_object<gl_object_type::BUFFER>::get_parameter<GLint>(GLenum parameter)
{
	GLint val;
	glGetNamedBufferParameteriv(m_id, parameter, &val);
	return val;
}

template <>
template <>
inline GLint64 gl_object<gl_object_type::BUFFER>::get_parameter<GLint64>(GLenum parameter)
{
	GLint64 val;
	glGetNamedBufferParameteri64v(m_id, parameter, &val);
	return val;
}



/////////////////////////// Specializations for GL_TEXTURE ///////////////////////////

template <>
inline gl_object<gl_object_type::TEXTURE>::gl_object(GLenum target)
{
	glCreateTextures(target, 1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::TEXTURE>::~gl_object()
{
	glDeleteTextures(1, &m_id);
}

template <>
template <>
inline GLint gl_object<gl_object_type::TEXTURE>::get_parameter<GLint>(GLenum parameter)
{
	GLint val;
	glGetTextureParameteriv(m_id, parameter, &val);
	return val;
}

template <>
template <>
inline GLfloat gl_object<gl_object_type::TEXTURE>::get_parameter<GLfloat>(GLenum parameter)
{
	GLfloat val;
	glGetTextureParameterfv(m_id, parameter, &val);
	return val;
}

template <>
template <>
inline void gl_object<gl_object_type::TEXTURE>::set_parameter<GLint>(GLenum parameter, GLint val)
{
	glTextureParameteri(m_id, parameter, val);
}

template <>
template <>
inline void gl_object<gl_object_type::TEXTURE>::set_parameter<GLfloat>(GLenum parameter, GLfloat val)
{
	glTextureParameterf(m_id, parameter, val);
}

template <>
template <>
inline void gl_object<gl_object_type::TEXTURE>::set_parameter<const GLint *>(GLenum parameter, const GLint *val)
{
	glTextureParameteriv(m_id, parameter, val);
}

template <>
template <>
inline void gl_object<gl_object_type::TEXTURE>::set_parameter<const GLfloat *>(GLenum parameter, const GLfloat *val)
{
	glTextureParameterfv(m_id, parameter, val);
}



/////////////////////////// Specializations for GL_SAMPLER ///////////////////////////

template <>
inline gl_object<gl_object_type::SAMPLER>::gl_object()
{
	glCreateSamplers(1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::SAMPLER>::~gl_object()
{
	glDeleteSamplers(1, &m_id);
}



/////////////////////////// Specializations for GL_VERTEX_ARRAY ///////////////////////////

template <>
inline gl_object<gl_object_type::VERTEX_ARRAY>::gl_object()
{
	m_creation_counter++;
	glCreateVertexArrays(1, &m_id);
}

template <>
inline gl_object<gl_object_type::VERTEX_ARRAY>::~gl_object()
{
	glDeleteVertexArrays(1, &m_id);
}



/////////////////////////// Specializations for GL_FRAMEBUFFER ///////////////////////////

template <>
inline gl_object<gl_object_type::FRAMEBUFFER>::gl_object()
{
	glCreateFramebuffers(1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::FRAMEBUFFER>::~gl_object()
{
	glDeleteFramebuffers(1, &m_id);
}

template <>
template <>
inline GLint gl_object<gl_object_type::FRAMEBUFFER>::get_parameter<GLint>(GLenum parameter)
{
	GLint val;
	glGetNamedFramebufferParameteriv(m_id, parameter, &val);
	return val;
}

template <>
template <>
inline void gl_object<gl_object_type::FRAMEBUFFER>::set_parameter<GLint>(GLenum parameter, GLint val)
{
	glNamedFramebufferParameteri(m_id, parameter, val);
}



/////////////////////////// Specializations for GL_RENDERBUFFER ///////////////////////////

template <>
inline gl_object<gl_object_type::RENDERBUFFER>::gl_object()
{
	glCreateRenderbuffers(1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::RENDERBUFFER>::~gl_object()
{
	glDeleteRenderbuffers(1, &m_id);
}



/////////////////////////// Specializations for GL_QUERY ///////////////////////////

template <>
inline gl_object<gl_object_type::QUERY>::gl_object(GLenum target)
{
	glCreateQueries(target, 1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::QUERY>::~gl_object()
{
	glDeleteQueries(1, &m_id);
}



/////////////////////////// Specializations for GL_SHADER ///////////////////////////

template <>
inline gl_object<gl_object_type::SHADER>::gl_object(GLenum type)
{
	m_id = glCreateShader(type);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::SHADER>::~gl_object()
{
	glDeleteShader(m_id);
}

template <>
template <>
inline GLint gl_object<gl_object_type::SHADER>::get_parameter<GLint>(GLenum parameter)
{
	GLint val;
	glGetShaderiv(m_id, parameter, &val);
	return val;
}



/////////////////////////// Specializations for GL_PROGRAM ///////////////////////////

template <>
inline gl_object<gl_object_type::PROGRAM>::gl_object()
{
	m_id = glCreateProgram();
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::PROGRAM>::~gl_object()
{
	glDeleteProgram(m_id);
}

template <>
template <>
inline GLint gl_object<gl_object_type::PROGRAM>::get_parameter<GLint>(GLenum parameter)
{
	GLint val;
	glGetProgramiv(m_id, parameter, &val);
	return val;
}



/////////////////////////// Specializations for GL_PROGRAM_PIPELINE ///////////////////////////

template <>
inline gl_object<gl_object_type::PROGRAM_PIPELINE>::gl_object()
{
	glCreateProgramPipelines(1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::PROGRAM_PIPELINE>::~gl_object()
{
	glDeleteProgramPipelines(1, &m_id);
}



/////////////////////////// Specializations for GL_TRANSFORM_FEEBACK ///////////////////////////

template <>
inline gl_object<gl_object_type::TRANSFORM_FEEDBACK>::gl_object()
{
	glCreateTransformFeedbacks(1, &m_id);
	m_creation_counter++;
}

template <>
inline gl_object<gl_object_type::TRANSFORM_FEEDBACK>::~gl_object()
{
	glDeleteTransformFeedbacks(1, &m_id);
}



/////////////////////////// Specializations for gl_object_traits ///////////////////////////

template <>
struct gl_object_traits<gl_object_type::BUFFER>
{
	const static GLenum gl_type = GL_BUFFER;
};

template <>
struct gl_object_traits<gl_object_type::TEXTURE>
{
	const static GLenum gl_type = GL_TEXTURE;
};

template <>
struct gl_object_traits<gl_object_type::SAMPLER>
{
	const static GLenum gl_type = GL_SAMPLER;
};

template <>
struct gl_object_traits<gl_object_type::VERTEX_ARRAY>
{
	const static GLenum gl_type = GL_VERTEX_ARRAY;
};

template <>
struct gl_object_traits<gl_object_type::FRAMEBUFFER>
{
	const static GLenum gl_type = GL_FRAMEBUFFER;
};

template <>
struct gl_object_traits<gl_object_type::RENDERBUFFER>
{
	const static GLenum gl_type = GL_RENDERBUFFER;
};

template <>
struct gl_object_traits<gl_object_type::QUERY>
{
	const static GLenum gl_type = GL_QUERY;
};

template <>
struct gl_object_traits<gl_object_type::SHADER>
{
	const static GLenum gl_type = GL_SHADER;
};

template <>
struct gl_object_traits<gl_object_type::PROGRAM>
{
	const static GLenum gl_type = GL_PROGRAM;
};

template <>
struct gl_object_traits<gl_object_type::PROGRAM_PIPELINE>
{
	const static GLenum gl_type = GL_PROGRAM_PIPELINE;
};

template <>
struct gl_object_traits<gl_object_type::TRANSFORM_FEEDBACK>
{
	const static GLenum gl_type = GL_TRANSFORM_FEEDBACK;
};


}
}
