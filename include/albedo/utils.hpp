#pragma once

namespace abd {

/**
	A noncopyable base class for enforcing move-only semantics
*/
class noncopy
{
protected:
	noncopy() = default;

	noncopy(const noncopy &) = delete;
	noncopy(noncopy &&) = default;

	noncopy &operator=(const noncopy &) = delete;
	noncopy &operator=(noncopy &&) = default;

	~noncopy() = default;
};


}