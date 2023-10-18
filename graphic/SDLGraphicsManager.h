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

	class SdlGraphicsManager final : public EventSubscriber
	{
		
	public:	
		static SdlGraphicsManager* Get();
		
		SdlGraphicsManager(SdlGraphicsManager const&)  = delete;		
		SdlGraphicsManager(SdlGraphicsManager &&other) = delete;	
		SdlGraphicsManager & operator=(SdlGraphicsManager &&other) = delete;
		SdlGraphicsManager& operator=(SdlGraphicsManager const&)  = delete;
		~SdlGraphicsManager() override;
			
		bool Initialize(uint width = 800, uint height = 600, const char* windowTitle = nullptr);
		static void Unload();

		void ClearAndDraw(const std::function<void(SDL_Renderer* renderer)>& drawObjects) const;
		void ShowOnly() const;

		[[nodiscard]] uint GetScreenWidth() const { return screenWidth;}
		[[nodiscard]] uint GetScreenHeight() const { return screenHeight;}
		
	    std::string GetSubscriberName() override;
		

		static std::shared_ptr<GraphicAsset> ToGraphicAsset(const std::shared_ptr<Asset>& asset);

		SDL_Renderer* WindowRenderer = nullptr; //The window renderer
	protected:		
		static SdlGraphicsManager* instance;
	private:		
		SdlGraphicsManager();
		uint screenWidth = 0;
		uint screenHeight = 0;		
		SDL_Window* window = nullptr; //The window we'll be rendering to
		SDL_Surface* windowSurface = nullptr; 
		ListOfEvents HandleEvent(std::shared_ptr<Event> theEvent, unsigned long deltaMs) override;
	};
}
#endif