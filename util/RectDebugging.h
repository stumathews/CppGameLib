#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <memory>

#include "font/FontAsset.h"
#include "resource/ResourceManager.h"

namespace gamelib
{
	class RectDebugging
	{
	public:
		static void PrintInRect(SDL_Renderer* renderer, const std::string& text, const SDL_Rect* bounds, const SDL_Color color)
		{
			SDL_Surface * surface = TTF_RenderText_Solid(std::static_pointer_cast<FontAsset>(
				ResourceManager::Get()->GetAssetInfo("kenvector_future2.ttf"))->GetFont(), 
				text.c_str(), 
				color);

			SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);	
			SDL_RenderCopy(renderer, texture, nullptr, bounds);
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}

	};
}

