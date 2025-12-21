#include "Component.h"

namespace gamelib
{	
	
	Component::Component(std::string name) 
		: name(std::move(name))
	{
	}

	std::string Component::GetName() const
	{
		return name;
	}


	void Component::SetName(const std::string& name)
	{
		this->name = name;
	}
}
