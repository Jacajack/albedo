#pragma once

#include <string>
#include <memory>
#include <utility>
#include <functional>
#include <initializer_list>
#include <albedo/gl/gl.hpp>

namespace abd {
namespace gl {

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

	window(int resx, int resy, const std::string &title, GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr, std::initializer_list<hint> hints = {});
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
	void run_main_loop(std::function<void()> f)
	{
		while (!should_close())
		{
			if (f) f();
			glfwPollEvents();
			glfwSwapBuffers(this->get());
		}
	}

	void make_current() const;
	bool should_close() const;
	void swap_buffers();

private:
	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)> m_window;
};

}
}