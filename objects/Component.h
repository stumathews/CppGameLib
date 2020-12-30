#pragma once
#include <string>

namespace gamelib
{

	class component
	{	
		std::string name;
	public:
		explicit component(const std::string name);		
		std::string get_name() const;
		void set_name(std::string name);
	};

	
}

