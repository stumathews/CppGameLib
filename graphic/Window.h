#pragma once
#include <functional>
#include "common/TypeAliases.h"
//#include "events/EventSubscriber.h"

struct SDL_Renderer;
struct SDL_Window;

namespace gamelib
{
	class Window
	{
	public:
		Window(const Window &) = delete;
		Window(Window &&) = delete;
		Window& operator=(Window const&)  = delete;
		Window& operator=(Window &&) = delete;

		// Construct the window
		explicit Window(const char* windowName, uint width = 800, uint height = 600, const char* windowTitle = nullptr, bool hideWindow = false);

		// Called on window destruction
		~Window();

		// Get the Window ready
		void Initialize();

		// Clear what was drawn on buffer and re-draw new contents
		void ClearAndDraw(const std::function<void(SDL_Renderer* renderer)>& drawObjectsFn) const;

		// Get Window's renderer
		[[nodiscard]] SDL_Renderer* GetRenderer() const;

		// Get Window height
		[[nodiscard]] uint Height() const;

		// Get window width
		[[nodiscard]] uint Width() const;

		// Get Window name
		[[nodiscard]] const char* GetName() const;

	private:
		// Only show what is in the draw buffer right now
		void PresentOnly() const;

		//The window we'll be rendering to
		SDL_Window* window = nullptr;

		uint width;
		uint height;

		// The title that is shown on the window
		const char* windowTitle;

		// An internal identifier for this window
		const char* windowName;

		SDL_Renderer* windowRenderer;

		// Skip drawing operations on this window?
		const bool hideWindow;
	};
}
