#pragma once

#include <boost/stacktrace.hpp>
#include <optional>
#include <exception>
#include <type_traits>

namespace abd {

/**
	A wrapper class for boost::stacktrace::stacktrace.
	All traced exceptions (independent on their template argument)
	can be casted to this type, hence the trace can be extracted.
*/
class stacktrace_wrapper
{
public:
	virtual ~stacktrace_wrapper() = default;

	const boost::stacktrace::stacktrace &get_trace() const
	{
		return m_trace;
	}

private:
	boost::stacktrace::stacktrace m_trace;
};

/**
    A CRTP class allowing any exception type derived from std::exception
    to contain a stack trace.
*/
template <typename T>
class traced_exception : public T, public stacktrace_wrapper
{
	static_assert(std::is_base_of_v<std::exception, T>, "abd::traced_exception requires T to be derived from std::exception");

public:
	template <typename... Args>
	explicit traced_exception(Args&&... args)
		: T(std::forward<Args>(args)...)
	{}

	virtual ~traced_exception() {}

	traced_exception &operator=(const traced_exception &rhs) = default;
	traced_exception &operator=(traced_exception &&rhs) = default;

};

/**
	This function returns the stack trace if the exception contains it
*/
inline std::optional<boost::stacktrace::stacktrace> trace_from_exception(const std::exception &except)
{
	try
	{
		auto &traced = dynamic_cast<const stacktrace_wrapper&>(except);
		return {traced.get_trace()};
	}
	catch (const std::bad_cast &ex)
	{
		return {};
	}
}

/**
	Whether the exceptions should be traced
*/
inline constexpr bool should_trace_exceptions = true;

/**
	The default exception to use
*/
using exception = std::conditional_t<should_trace_exceptions, traced_exception<std::runtime_error>, std::runtime_error>;

}