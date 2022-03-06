#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <memory>

#include "font/font_resource.h"
#include "resource/ResourceManager.h"
using namespace std;

namespace gamelib
{
	class RectDebugging
	{
	public:
		static void printInRect(SDL_Renderer* renderer, std::string text, SDL_Rect* bounds, ResourceManager& resource_admin)
		{
			SDL_Color color = { 255, 255, 255 };
			SDL_Surface * surface = TTF_RenderText_Solid( static_pointer_cast<font_resource>(resource_admin.GetAssetInfo("arial.ttf"))->get_font() , text.c_str(), color);
			SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);	
			SDL_RenderCopy(renderer, texture, NULL, bounds);
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}

	};
}

