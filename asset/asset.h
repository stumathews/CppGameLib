#pragma once
#include <string>
#include <map>

namespace gamelib
{
	/// <summary>
	/// A loadable asset
	/// </summary>
	class Asset
	{
	public:

		enum class AssetType
		{
			Undefined,
			Graphic,
			Sprite,
			Audio,
			Font,
		};
		
		Asset(int uid, std::string name, std::string path, std::string type, int scene);
		Asset(const Asset& other) = delete; // copy constructor
		Asset& operator=(const Asset& other) = delete; // assignment operator
		Asset(const Asset&& other) = delete; // move constructor
		Asset& operator=(const Asset&& other) = delete; // move assignment operator

		virtual ~Asset() = default;
				
		const int Uid;
		const std::string Name;
		const std::string FilePath;
		const std::string Type;
		AssetType AssetType;		

		const int SceneId;
		bool IsLoadedInMemory;
		
		/// <summary>
		/// All resources can be allowed to load themselves
		/// </summary>
		virtual void Load() = 0;

		/// <summary>
		/// All assets can be allowed to unload themselves
		/// </summary>
		/// <returns></returns>
		virtual bool Unload() = 0;

		/// <summary>
		/// An asset can have misc properties attached to it
		/// </summary>
		std::map<std::string, std::string> Properties;

	};
}
