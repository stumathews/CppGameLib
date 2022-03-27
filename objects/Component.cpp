#include "component.h"
namespace gamelib
{	
	
	Component::Component(const std::string name) 
		: name(name)
	{
	}

	std::string Component::GetName() const
	{
		return name;
	}


	void Component::SetName(const std::string name)
	{
		this->name = name;
	}
}
