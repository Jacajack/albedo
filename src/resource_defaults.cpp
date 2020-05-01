#include <albedo/resource_defaults.hpp>

std::unique_ptr<abd::default_resource_engine_type> abd::main_resource_engine::m_engine;

abd::default_resource_engine_type &abd::main_resource_engine::get()
{
	if (m_engine)
		return *m_engine;
	else
		return *(m_engine = std::make_unique<abd::default_resource_engine_type>());
}