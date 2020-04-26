#pragma once

#include <string>
#include <memory>
#include <utility>
#include <functional>
#include <initializer_list>
#include <albedo/gl/gl.hpp>

namespace abd {
namespace gl {

class window_builder;

/**
    A wrapper for GLFWwindow
*/
class window
{
public:
	using hint = std::pair<int, int>;

	static inline void set_global_hint(const hint &h)
	{
		glfwWindowHint(h.first, h.second);
	}

	window(int resx, int resy, const std::string &title, const window_builder &builder);
	virtual ~window() = default;

	// Copy ctor and assignment operator
	window(const window &) = delete;
	window &operator=(const window &) = delete;

	// Move semantics
	window(window &&) = default;
	window &operator=(window &&) = default;

	//! Provides access to the actual pointer
	GLFWwindow *get()
	{
		return m_window.get();
	}

	//! Runs provided function in a loop until the window is closed. Does buffer swapping and event polling
	void run_main_loop(std::function<void(double dt)> f)
	{
		double t = 0.0, tlast = 0.0;
		while (!should_close())
		{
			if (f) f(t - tlast);
			glfwPollEvents();
			glfwSwapBuffers(this->get());
			tlast = t;
			t = glfwGetTime();
		}
	}

	void make_current() const;
	bool should_close() const;
	void swap_buffers();

private:
	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)> m_window;
};

/**
	A builder class for the window
*/
class window_builder
{
public:
	window_builder &context_version(int major, int minor)
	{
		m_minor = minor;
		m_major = major;
		return *this;
	}

	window_builder &debug(bool enable)
	{
		m_debug = enable;
		return *this;
	}

	window_builder &forward_compat(bool enable)
	{
		m_forward_compat = enable;
		return *this;
	}

	window_builder &profile(int profile)
	{
		m_profile = profile;
		return *this;
	}

	window_builder &samples(int n)
	{
		m_samples = n;
		return *this;
	}

	window_builder &monitor(GLFWmonitor *mon)
	{
		m_monitor = mon;
		return *this;
	}

	GLFWmonitor *get_monitor() const
	{
		return m_monitor;
	}

	window_builder &parent_window(GLFWwindow *win)
	{
		m_parent = win;
		return *this;
	}

	GLFWwindow *get_parent_window() const
	{
		return m_parent;
	}

	std::vector<window::hint> to_hints() const;

private:
	int m_major = 4;
	int m_minor = 5;
	bool m_forward_compat = true;
	bool m_debug = false;
	int m_samples = 0;
	int m_profile = GLFW_OPENGL_CORE_PROFILE;
	GLFWmonitor *m_monitor = nullptr;
	GLFWwindow *m_parent = nullptr;
};

}
}