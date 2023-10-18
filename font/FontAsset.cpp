#include "FontAsset.h"

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Create Font asset details
	/// </summary>
	FontAsset::FontAsset(const int inUid, const string& inName, const string& inPath, const string& inType,
	                     const int inScene) : Asset(inUid, inName, inPath, inType, inScene)
	{
		AssetType = AssetType::Font;
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
		font =  TTF_OpenFont(FilePath.c_str(), 28);
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