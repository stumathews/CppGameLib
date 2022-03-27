#pragma once
#include <string>

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

	private:
		std::string name;
	};

	
}

