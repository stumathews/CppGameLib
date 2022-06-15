#pragma once
#include <string>
#include <map>

namespace gamelib
{
	/// <summary>
	/// A component
	/// </summary>
	class Component
	{			
	public:
		Component(const std::string name);		
		std::string GetName() const;
		void SetName(std::string name);
		std::map<std::string, std::string> strings;
	private:
		std::string name;
	};

	
}

