#include "Window.h"

#include "SDLGraphicsManager.h"
#include "exceptions/EngineException.h"
#include <SDL_render.h>
#include <SDL_video.h>

namespace gamelib
{	
	Window::Window(const char *windowName, const uint width, const uint height, const char *windowTitle,
	               const bool hideWindow) : width(width), height(height),
	                                        windowTitle(windowTitle), windowName(windowName), windowRenderer(nullptr),
	                                        hideWindow(hideWindow)
	{
		// Empty
	}

	Window::~Window()
	{
		SDL_DestroyRenderer(windowRenderer);
		SDL_DestroyWindow(window);
	}

	void Window::Initialize()
	{
		// Create a new Window
		window = SdlGraphicsManager::CreateSdlWindow(static_cast<int>(width), static_cast<int>(height), windowTitle, hideWindow);

		// Get the Window's renderer
		windowRenderer = SdlGraphicsManager::GetSdlRendererFromWindow(window);

		if(windowRenderer == nullptr)
		{
			std::stringstream message;
			message << "Could not get Window Renderer: " << SDL_GetError();
			THROW(12, message.str(), "SDLGraphicsManager");
		}

		// Set drawing colour to Grey background
		SDL_SetRenderDrawColor(windowRenderer, 192,192,192, 0);
	}

	void Window::ClearAndDraw(const std::function<void(SDL_Renderer* renderer)> &drawObjectsFn) const
	{
		if (hideWindow)
		{
			// Window is hidden, don't draw
			return;
		}

		// Backup current render color
		SDL_Color oldColour = {0, 0, 0, 0};
		SDL_GetRenderDrawColor(windowRenderer, &oldColour.r, &oldColour.g, &oldColour.b, &oldColour.a);
		
		// Clear
		SDL_RenderClear(windowRenderer);

		// Draw everything
		drawObjectsFn(windowRenderer);
		
		// Set draw colour
		SDL_SetRenderDrawColor(windowRenderer, oldColour.r, oldColour.g, oldColour.b, oldColour.a); // nb: restore whatever the render routine set as the draw color to
		
		// Show what we've drawn
		PresentOnly();
	}

	SDL_Renderer * Window::GetRenderer() const
	{
		return windowRenderer;
	}

	uint Window::Height() const
	{
		return height;
	}

	uint Window::Width() const
	{
		return width;
	}

	const char * Window::GetName() const
	{
		return windowName;
	}

	void Window::PresentOnly() const
	{
		SDL_RenderPresent(windowRenderer);
	}

}
