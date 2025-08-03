#include "FontAsset.h"

using namespace std;

namespace gamelib
{
	FontAsset::FontAsset(const int inUid, const string& inName, const string& inPath, const string& inType,
	                     const int inScene) : Asset(inUid, inName, inPath, inType, inScene)
	{
		AssetType = AssetType::Font;
	}
	
	TTF_Font* FontAsset::GetFont() const
	{
		return font;
	}
	
	void FontAsset::Load()
	{
		font =  TTF_OpenFont(FilePath.c_str(), 28);
	}
	
	bool FontAsset::Unload()
	{
		TTF_CloseFont(font);
	    font = nullptr;
		return true;
	}
}