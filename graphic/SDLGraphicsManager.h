#ifndef SDL_GRAPHICS_MANAGER_H
#define SDL_GRAPHICS_MANAGER_H
#include <vector>
#include "graphic_resource.h"
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
		EventManager& event_admin;
		Logger& the_logger;
	public:
		
		SDLGraphicsManager(EventManager& event_admin, Logger& the_logger);
		SDLGraphicsManager(SDLGraphicsManager const&)  = delete;		
		SDLGraphicsManager(SDLGraphicsManager &&other) = delete;	
		SDLGraphicsManager & operator=(SDLGraphicsManager &&other) = delete;
		SDLGraphicsManager& operator=(SDLGraphicsManager const&)  = delete;
		~SDLGraphicsManager();
			
		
		SDL_Window* window = nullptr; //The window we'll be rendering to
		SDL_Renderer* window_renderer = nullptr; //The window renderer
		SDL_Surface* window_surface = nullptr; 

		std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * asset_xml_element, SettingsManager& config);
		
		bool initialize(const uint width = 800, uint height = 600, const char* window_title = nullptr);	

		void DrawCurrentScene(SceneManager& scene_admin) const;
		uint get_screen_width() const { return screen_width;}
		uint get_screen_height() const { return screen_height;}
		
	    std::string get_subscriber_name() override;

		static std::shared_ptr<graphic_resource> to_resource(const std::shared_ptr<asset>& asset);
		
	private:
		uint screen_width = 0;
		uint screen_height = 0;
		void clear_draw_present(std::function<void(SDL_Renderer* renderer)> &render_routine) const;
		
		events handle_event(const std::shared_ptr<event> the_event) override;
		
	};
}
#endif