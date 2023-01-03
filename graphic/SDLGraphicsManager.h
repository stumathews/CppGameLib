#ifndef SDL_GRAPHICS_MANAGER_H
#define SDL_GRAPHICS_MANAGER_H
#include <vector>
#include "GraphicAsset.h"
#include "tinyxml2.h"
#include <memory>
#include "SDL.h"
#include <functional>
#include "events/EventManager.h"
#include "events/EventSubscriber.h"


namespace gamelib
{
	class SceneManager;

	class SDLGraphicsManager final : public EventSubscriber
	{
		
	public:	
		static SDLGraphicsManager* Get();
		
		SDLGraphicsManager(SDLGraphicsManager const&)  = delete;		
		SDLGraphicsManager(SDLGraphicsManager &&other) = delete;	
		SDLGraphicsManager & operator=(SDLGraphicsManager &&other) = delete;
		SDLGraphicsManager& operator=(SDLGraphicsManager const&)  = delete;
		~SDLGraphicsManager() override;
			
		
		SDL_Window* Window = nullptr; //The window we'll be rendering to
		SDL_Renderer* WindowRenderer = nullptr; //The window renderer
		SDL_Surface* WindowSurface = nullptr; 
		bool Initialize(uint width = 800, uint height = 600, const char* windowTitle = nullptr);

		[[nodiscard]] uint GetScreenWidth() const { return screenWidth;}
		[[nodiscard]] uint GetScreenHeight() const { return screenHeight;}
		
	    std::string GetSubscriberName() override;

		static std::shared_ptr<GraphicAsset> ToGraphicAsset(const std::shared_ptr<Asset>& asset);
		void ClearAndDraw(const std::function<void(SDL_Renderer* renderer)>& drawObjects) const;
		void ShowOnly() const;
	protected:		
		static SDLGraphicsManager* Instance;
	private:		
		SDLGraphicsManager();
		uint screenWidth = 0;
		uint screenHeight = 0;		
		ListOfEvents HandleEvent(std::shared_ptr<Event> theEvent, unsigned long deltaMs) override;
	};
}
#endif