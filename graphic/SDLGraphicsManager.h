#ifndef SDL_GRAPHICS_MANAGER_H
#define SDL_GRAPHICS_MANAGER_H
#include <vector>
#include "GraphicAsset.h"
#include "file/tinyxml2.h"
#include <memory>
#include "SDL.h"
#include <functional>

#include "Window.h"
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
		std::shared_ptr<Window> GetMainWindow();

		static SDL_Window* CreateSdlWindow(int screenWidth, int screenHeight, const char* windowTitle);
		static SDL_Renderer* GetSdlRendererFromWindow(SDL_Window* window);
		bool Initialize(uint width = 800, uint height = 600, const char* windowTitle = nullptr);
		static void Unload();
		
		void ClearAndDraw(const std::function<void(SDL_Renderer* renderer)>& drawObjects) const;

		[[nodiscard]] uint GetScreenWidth() const { return mainWindow->Width();}
		[[nodiscard]] uint GetScreenHeight() const { return mainWindow->Height();}
		
	    std::string GetSubscriberName() override;
		

		static std::shared_ptr<GraphicAsset> ToGraphicAsset(const std::shared_ptr<Asset>& asset);
		const char* MainWindowName = "main";
	protected:		
		static SdlGraphicsManager* instance;
	private:		
		SdlGraphicsManager();	
		std::shared_ptr<Window> mainWindow; //The window we'll be rendering to
		std::map<std::string, std::shared_ptr<Window>> windows;
		ListOfEvents HandleEvent(std::shared_ptr<Event> event, unsigned long deltaMs) override;
	};
}
#endif