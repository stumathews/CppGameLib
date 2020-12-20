#include "component.h"
namespace gamelib
{
	component::component(const std::string name): name(name)
	{
	}

	std::string component::get_name() const
	{
		return name;
	}

	void component::set_name(const std::string name)
	{
		this->name = name;
	}
}
