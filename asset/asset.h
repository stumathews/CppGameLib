#pragma once
#include <string>

namespace gamelib
{
	/**
	 Meta data about a resource (uid, name, path, type, scene etc)
	 */
	class asset
	{
	public:	
		
		asset(int uid, std::string& name, const std::string& path, const std::string& type, int scene);

		const int uid;
		const std::string name;
		const std::string path;
		const std::string type;
		const int scene;

		bool is_loaded;
		
		// All resources can load themselves
		virtual void load() = 0;

		// All resources can unload themselves
		virtual bool unload() = 0;
		
		virtual ~asset() = default;
	};
}
