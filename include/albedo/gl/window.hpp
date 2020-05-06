#pragma once

#include <string>
#include <memory>
#include <utility>
#include <functional>
#include <initializer_list>
#include <albedo/gl/gl.hpp>
#include <map>

namespace abd::gl {

class window_builder;

/**
	\todo Implement mouse and keyboard inputs as separate classes
	iterfacing with window::control_block
*/

/**
    A wrapper for GLFWwindow
*/
class window
{
public:
	class keyboard_handler;
	class mouse_handler;
	struct control_block;
	using hint = std::pair<int, int>;

	// Static functions - GLFW callback handlers
	static void keyboard_callback(GLFWwindow *win, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow *win, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow *win, double x, double y);

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

	// Provide access to the mouse and keyboard handlers
	keyboard_handler &get_keyboard_handler();
	mouse_handler &get_mouse_handler();

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
	std::unique_ptr<control_block> m_control_block;
};

/**
	Handles GLFW keyboard events and makes handling
	keyboard more convenient
*/
class window::keyboard_handler
{
	friend class window;

public:
	struct key_status
	{
		int action;
		int mods;
		bool is_pressed;
	};

	const key_status &get_key_by_scancode(int scancode) const
	{
		const static key_status def = {0, 0, 0};

		try
		{
			return m_keyboard_status.at(scancode);
		}
		catch(const std::out_of_range &ex)
		{
			return def;
		}
		
	}

	const key_status &get_key(int key) const
	{
		return get_key_by_scancode(glfwGetKeyScancode(key));
	}

private:
	void callback(int key, int scancode, int action, int mods);

	//! Last action registered for each scancode
	std::map<int, key_status> m_keyboard_status;
};

/**
	Handles GLFW mouse events and makes handling mouse
	stuff more convenient
*/
class window::mouse_handler
{
	friend class window;

public:
	glm::vec2 get_position() const
	{
		return {m_x, m_y};
	}

	void set_button_func(const std::function<void(int button, int action, int mods)> &f)
	{
		m_button_func = f;
	}

private:
	void button_callback(int button, int action, int mods);
	void position_callback(double x, double y);

	std::function<void(int button, int action, int mods)> m_button_func;

	double m_x, m_y;
};

/**
	Contains all the window resources that need to be referenced
	through GLFW window's user pointer and hence need to be immovable.
*/
struct window::control_block
{
	window::mouse_handler mouse;
	window::keyboard_handler keyboard;
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

} // namespace abd::gl