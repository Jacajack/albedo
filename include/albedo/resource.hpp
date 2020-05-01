#pragma once 

#include <functional>
#include <boost/filesystem.hpp>
#include <albedo/utils.hpp>

namespace abd {

/**
	A base class for any resource that is to be managed by resource_manager.
	Provides separate resource indexing per resource type.
*/
template <typename T>
class resource : abd::noncopy
{
public:
	using index_type = std::uint64_t;
	using path_type = boost::filesystem::path;
	using resource_loader = std::function<std::unique_ptr<T>(const path_type&)>;
	using default_loader_type = const resource_loader;

	static inline default_loader_type default_loader;

	resource() :
		m_id(m_id_counter++)
	{
	}

	const index_type get_id() const
	{
		return m_id;
	}

private:
	static index_type m_id_counter;
	index_type m_id;
};

// ID counter initialization
template <typename T>
typename resource<T>::index_type resource<T>::m_id_counter = 0;

}