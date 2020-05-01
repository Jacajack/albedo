#pragma once

#include <boost/filesystem.hpp>
#include <albedo/exception.hpp>
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


/**
	Manges resource loading and lookup (by name).
	Everything loaded from a file is considered a resource.

	\todo dependency resolution
*/
template <typename T>
class resource_manager
{
	static_assert(std::is_base_of_v<abd::resource<T>, T>, "managed resource must be derived from abd::resource");

public:
	using name_type = std::string;
	using path_type = boost::filesystem::path;
	using resource_loader = std::function<std::unique_ptr<T>(const path_type&)>;

	resource_manager(const resource_loader &default_loader);

	std::shared_ptr<T> load(const resource_loader &loader, const path_type &path, const name_type &name = {});
	std::shared_ptr<T> load(const path_type &path, const name_type &name = {});

	std::shared_ptr<T> get_ptr(const name_type &name) const;
	T &get(const name_type &name) const;

	void remove(const name_type &name);

private:
	resource_loader m_default_loader;
	std::map<name_type, std::shared_ptr<T>> m_resources;
};


template <typename T>
resource_manager<T>::resource_manager(const resource_loader &default_loader) :
	m_default_loader(default_loader)
{
}

/**
	Delegates loading to the loader function and stores
	acquired pointer in the manager.
*/
template <typename T>
std::shared_ptr<T> resource_manager<T>::load(const resource_loader &loader, const path_type &path, const name_type &name)
{
	name_type real_name = name;

	// If name is empty, the path becomes the name
	if (real_name.empty())
		real_name = path.string();

	// Check for name collision
	if (m_resources.find(real_name) != m_resources.end())
		throw abd::exception("name collision in resource_manager");
	
	std::shared_ptr<T> res_ptr = loader(path);
	m_resources.emplace(std::move(real_name), res_ptr);

	return std::move(res_ptr);
}

/**
	When no loader is specified, uses default loader
*/
template <typename T>
std::shared_ptr<T> resource_manager<T>::load(const path_type &path, const name_type &name)
{
	return load(m_default_loader, path, name);
}

template <typename T>
std::shared_ptr<T> resource_manager<T>::get_ptr(const name_type &name) const
{
	return m_resources.at(name);
}

template <typename T>
T &resource_manager<T>::get(const name_type &name) const
{
	return *m_resources.at(name);
}

template <typename T>
void resource_manager<T>::remove(const name_type &name)
{
	auto &ptr = m_resources.at(name);
	if (ptr.use_count() > 1)
		throw abd::exception("cannot remove resource in use");
	m_resources.erase(name);
}

}