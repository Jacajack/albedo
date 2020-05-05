#pragma once

#include <albedo/gl/gl.hpp>
#include <albedo/utils.hpp>
#include <albedo/exception.hpp>

namespace abd::gl {

/**
	Wraps up a GLsync object.
*/
class fence_sync : abd::noncopy
{
public:
	fence_sync() = default;
	fence_sync(const fence_sync &) = delete;
	fence_sync &operator=(const fence_sync &) = delete; 
	inline fence_sync(fence_sync &&rhs) noexcept;
	inline fence_sync &operator=(fence_sync &&rhs) noexcept;
	inline ~fence_sync();

	inline void fence();
	inline void destroy();

	inline bool is_signaled() const;
	inline void wait(GLuint64 timeout = 0.1e9) const;

private:
	mutable GLsync m_sync = nullptr;
};

fence_sync::fence_sync(fence_sync &&rhs) noexcept :
	m_sync(rhs.m_sync)
{
	rhs.m_sync = nullptr;
}

fence_sync &fence_sync::operator=(fence_sync &&rhs) noexcept
{
	if (this != &rhs)
		std::swap(*this, rhs);
	return *this;
}

fence_sync::~fence_sync()
{
	destroy();
}

void fence_sync::fence()
{
	if (m_sync) throw abd::exception("gl::fence_sync fenced twice");
	m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void fence_sync::destroy()
{
	glDeleteSync(m_sync);
	m_sync = nullptr;
}

/**
	Returns whether the fence has signaled.

	Inactive fences are considered signaled, because they
	do not cause blocking.
*/
bool fence_sync::is_signaled() const
{
	if (m_sync == nullptr)
		return true;

	GLint result;
	glGetSynciv(m_sync, GL_SYNC_STATUS, 1, nullptr, &result);
	return result == GL_SIGNALED;
}

/**
	Waits for sync and then destroys it (since destroyed fences are equivalent to signaled)
*/
void fence_sync::wait(GLuint64 timeout) const
{
	if (m_sync != nullptr)
	{
		glClientWaitSync(m_sync, 0, timeout);
		glDeleteSync(m_sync);
		m_sync = nullptr;
	}
}

};