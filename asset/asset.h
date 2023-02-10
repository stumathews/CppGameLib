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

		/// <summary>
		/// Describes the type of asset
		/// </summary>
		enum class AssetType
		{
			Undefined,
			Graphic,
			Sprite,
			Audio,
			Font,
		};

				
		/// <summary>
		/// A generic asset
		/// </summary>
		Asset(int uid, std::string name, std::string path, std::string type, int scene);
		Asset(const Asset& other) = delete;

		virtual ~Asset() = default;

		/// <summary>
		/// Asset UID
		/// </summary>
		const int uid;

		/// <summary>
		/// Asset Name
		/// </summary>
		const std::string name;

		/// <summary>
		/// Asset Path
		/// </summary>
		const std::string path;

		/// <summary>
		/// Asset type
		/// </summary>
		const std::string type;

		/// <summary>
		/// Asset type
		/// </summary>
		AssetType assetType;		

		/// <summary>
		/// Asset scene/scope
		/// </summary>
		const int scene;

		/// <summary>
		/// If Assset is in memory currently
		/// </summary>
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
		/// Properties
		/// </summary>
		std::map<std::string, std::string> Properties;

	};
}
