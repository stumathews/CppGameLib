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
		explicit Component(std::string name);		
		std::string GetName() const;
		void SetName(const std::string& name);
		std::map<std::string, std::string> Strings;
	private:
		std::string name;
	};

	
}

