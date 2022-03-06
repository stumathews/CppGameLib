#pragma once
#include "SDL_ttf.h"
#include "asset/asset.h"

namespace gamelib
{
	class font_resource final : public Asset
	{
	public:
		font_resource(int uid, std::string name, std::string path, std::string type, int scene);

		TTF_Font* get_font() const;

		void Load() override;
		bool Unload() override;
	private:
		TTF_Font* font = nullptr;
	};
}
