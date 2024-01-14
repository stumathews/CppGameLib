#pragma once
#include <functional>
#include <SDL_render.h>
#include <SDL_video.h>

#include "common/TypeAliases.h"
#include "events/EventSubscriber.h"

namespace gamelib
{
	class Window
	{
	public:
		explicit Window(const char* windowName, uint width = 800, uint height = 600, const char* windowTitle = nullptr);
		Window(const Window &) = delete;
		Window(Window &&) = delete;
	    Window& operator=(Window const&)  = delete;
		Window& operator=(Window &&) = delete;
		~Window();
		void Initialize();
		void ClearAndDraw(const std::function<void(SDL_Renderer* renderer)>& drawObjectsFn) const;
		void PresentOnly() const;
		[[nodiscard]] SDL_Renderer* GetRenderer() const { return windowRenderer; }
		[[nodiscard]] uint Height() const { return height;}
		[[nodiscard]] uint Width() const { return width;}
		[[nodiscard]] const char* GetName() const { return windowName; }
	private:
		SDL_Window* window = nullptr; //The window we'll be rendering to
		uint width;
		uint height;
		const char* windowTitle;
		SDL_Renderer* windowRenderer;
		const char* windowName;
	};
}
