#include "FontAsset.h"

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create Font asset details
	/// </summary>
	FontAsset::FontAsset(const int uid, string name, string path, string type, int scene) : Asset(uid, name, path, type, scene)
	{
		assetType = AssetType::Font;
	}

	/// <summary>
	/// Get Font data
	/// </summary>
	TTF_Font* FontAsset::GetFont() const
	{
		return font;
	}

	/// <summary>
	/// Load font data into memory
	/// </summary>
	void FontAsset::Load()
	{
		font =  TTF_OpenFont(path.c_str(), 28);
	}

	/// <summary>
	/// Unload font data from memory
	/// </summary>
	bool FontAsset::Unload()
	{
		TTF_CloseFont(font);
	    font = nullptr;
		return true;
	}
}