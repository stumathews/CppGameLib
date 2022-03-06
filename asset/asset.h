#pragma once
#include <string>

namespace gamelib
{
	// Meta data about a resource (uid, name, path, type, scene etc)
	class Asset
	{
	public:	
		
		Asset(int uid, std::string& name, const std::string& path, const std::string& type, int scene);

		const int uid;
		const std::string name;
		const std::string path;
		const std::string type;
		const int scene;

		bool isLoadedInMemory;
		
		// All resources can load themselves
		virtual void Load() = 0;

		// All resources can unload themselves
		virtual bool Unload() = 0;
		
		virtual ~Asset() = default;
	};
}
