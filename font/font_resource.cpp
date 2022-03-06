#include "font_resource.h"

using namespace std;

namespace gamelib
{
	font_resource::font_resource(const int uid, string name, string path, string type, int scene) : Asset(uid, name, path, type, scene)
	{
		// ctor used for initialization only
	}

	TTF_Font* font_resource::get_font() const
	{
		return font;
	}

	void font_resource::Load()
	{
		font =  TTF_OpenFont( path.c_str(), 28 );
	}

	bool font_resource::Unload()
	{
		TTF_CloseFont(font);
	    font = nullptr;
		return true;
	}
}