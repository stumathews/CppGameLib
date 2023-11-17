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
			const auto font = std::static_pointer_cast<FontAsset>(ResourceManager::Get()->GetAssetInfo("kenvector_future2.ttf"))->GetFont();

			// text -> surface
			SDL_Surface * surface = TTF_RenderText_Blended(font, text.c_str(), color);
			
			// surface -> Texture
			SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

			// texture -> renderer
			SDL_RenderCopy(renderer, texture, nullptr, bounds);

			// Cleanup
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}

	};
}

