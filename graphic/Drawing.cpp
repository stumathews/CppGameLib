#include "Drawing.h"
#include <SDL.h>
#include <iostream>
#include <SDL_image.h>

namespace gamelib
{
	Drawing::Drawing()
	{
	}

	void Drawing::DrawRectangle(const int m_xPos, const int m_yPos, const int w, const int h, SDL_Renderer * onRenderer, int r, int g, int b)
	{

		// Define a rectangle
		SDL_Rect fillRect = { m_xPos == 0 ? 3 : m_xPos , m_yPos == 0 ? 4 : m_yPos , w, h };
		
		// Set draw colour on renderer
		SDL_SetRenderDrawColor(onRenderer, r, g, b, 0xFF);

		// Send to senderer
		SDL_RenderFillRect(onRenderer, &fillRect);
	}


	Drawing::~Drawing()
	{
	}



	SDL_Texture* MakeTexture(char* texturePath, SDL_Renderer* renderer)
	{
		SDL_Texture* newTexture = NULL;
		SDL_Surface* imageSurface = IMG_Load(texturePath);

		//if(imageSurface == NULL)
		//{
		//	std::cout << "SDL could not load image: " << (char*)IMG_GetError() << std::endl;
		//}

		//auto optimisedSurface = SDL_ConvertSurface(imageSurface, g_pGameWorldData->pWindowImageSurface->format, NULL);
		//
		//if(optimisedSurface == NULL)
		//{
		//	std::cout << "Unable to optimize image " <<  texturePath << " SDL Error: " << SDL_GetError() << std::endl;
		//}
		//
		////Create texture from surface pixels
		//newTexture = SDL_CreateTextureFromSurface(renderer, optimisedSurface);	
		//if(newTexture == NULL)
		//{
		//	std::cout << "Unable to create texture: " << (char*)IMG_GetError() << std::endl;
		//}

		//Get rid of old loaded surface
		SDL_FreeSurface(imageSurface);
		return newTexture;
	}


	void DrawTextureTopLeft(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, SDL_Texture* texture)
	{
		// SDL_RenderCopy(g_pGameWorldData->pWindowRenderer, texture, NULL, NULL);
	}

	void drawVerticalLineOfDots(const int SCREEN_HEIGHT, const int SCREEN_WIDTH)
	{
		//Draw vertical line of yellow dots
		/*
		SDL_SetRenderDrawColor(g_pGameWorldData->pWindowRenderer, 0x00, 0x00, 0xFF, 0x00);
		for (int i = 0; i < SCREEN_HEIGHT; i += 4) {
			SDL_RenderDrawPoint(g_pGameWorldData->pWindowRenderer, SCREEN_WIDTH / 2, i);
		}
		*/
	}


	SDL_Texture* TryMakeTexture(char* path, SDL_Renderer* windowRenderer)
	{	
		if(path == NULL)
		{
			std::cout << "Texture path cant be empty!" << std::endl;
		}

		SDL_Texture* outTexture = MakeTexture(path, windowRenderer);	

		if(outTexture == NULL)
		{
			std::cout << "Could not load textture" << std::endl;
		}
		return outTexture;
	}

	void renderLine(SDL_Renderer* toRenderer, unsigned int screen_height, unsigned int screenWidth)
	{
		 //Draw blue horizontal line
		SDL_SetRenderDrawColor( toRenderer, 0x00, 0x00, 0xFF, 0xFF );
		SDL_RenderDrawLine( toRenderer, 0, screen_height / 2, screenWidth, screen_height / 2 );
	}
}