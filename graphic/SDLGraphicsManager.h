#ifndef SDL_GRAPHICS_MANAGER_H
#define SDL_GRAPHICS_MANAGER_H
#include <vector>
#include "GraphicAsset.h"
#include <tinyxml2.h>
#include <memory>

#include <functional>

#include "Window.h"
#include "events/EventSubscriber.h"
#include "objects/GameObject.h"

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

		// Setup/initialize everything
		bool Initialize(uint width = 800, uint height = 600, const char* windowTitle = nullptr, bool hideWindow = false);

		// Returns the main Window
		std::shared_ptr<Window> GetMainWindow();

		// Returns the Windows's renderer
		static SDL_Renderer* GetSdlRendererFromWindow(SDL_Window* window);

		// Returns a new SDL Window (does not show it)
		static SDL_Window* CreateSdlWindow(int screenWidth, int screenHeight, const char* windowTitle, bool hideWindow = false);

		// Finish up
		static void Unload();

		// Clears the contents of the buffer and draws objects onto it
		void ClearAndDraw(const std::function<void(SDL_Renderer* renderer)>& drawObjects) const;

		[[nodiscard]] uint GetScreenWidth() const;
		[[nodiscard]] uint GetScreenHeight() const;

		std::string GetSubscriberName() override;

		// Casts a standard Asset to a Graphics Asset
		static std::shared_ptr<GraphicAsset> ToGraphicAsset(const std::shared_ptr<Asset>& asset);

		const char* MainWindowName = "main";
	protected:		
		static SdlGraphicsManager* instance;
	private:

		SdlGraphicsManager();	
		std::shared_ptr<Window> mainWindow; //The window we'll be rendering to
		std::map<std::string, std::shared_ptr<Window>> windows;
		ListOfEvents HandleEvent(const std::shared_ptr<Event>& event, unsigned long deltaMs) override;
	};
}
#endif
