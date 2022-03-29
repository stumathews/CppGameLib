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
		~SDLGraphicsManager();
			
		
		SDL_Window* window = nullptr; //The window we'll be rendering to
		SDL_Renderer* windowRenderer = nullptr; //The window renderer
		SDL_Surface* windowSurface = nullptr; 
		bool Initialize(const uint width = 800, uint height = 600, const char* window_title = nullptr);	

		uint get_screen_width() const { return screenWidth;}
		uint get_screen_height() const { return screenHeight;}
		
	    std::string GetSubscriberName() override;

		static std::shared_ptr<GraphicAsset> ToGraphicAsset(const std::shared_ptr<Asset>& asset);
		void ClearAndDraw(std::function<void(SDL_Renderer* renderer)>& drawObjects) const;
	protected:		
		static SDLGraphicsManager* Instance;
	private:		
		SDLGraphicsManager();
		uint screenWidth = 0;
		uint screenHeight = 0;
		
		bool beVerbose = false;
		events HandleEvent(const std::shared_ptr<Event> the_event) override;		
	};
}
#endif