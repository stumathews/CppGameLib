#ifndef SDL_GRAPHICS_MANAGER_H
#define SDL_GRAPHICS_MANAGER_H
#include <vector>
#include "graphic_resource.h"
#include "tinyxml2.h"
#include <memory>
#include "SDL.h"
#include <functional>
#include "events/event_manager.h"
#include "events/IEventSubscriber.h"


namespace gamelib
{
	class scene_manager;

	class sdl_graphics_manager final : public IEventSubscriber, public std::enable_shared_from_this<sdl_graphics_manager>
	{
		std::shared_ptr<event_manager> event_admin;
		std::shared_ptr<logger> the_logger;
	public:
		
		sdl_graphics_manager(std::shared_ptr<event_manager> event_admin, std::shared_ptr<logger> the_logger = std::make_shared<logger>());
		~sdl_graphics_manager() override;
		sdl_graphics_manager(sdl_graphics_manager const&)  = delete;		
		sdl_graphics_manager(sdl_graphics_manager &&other);		
		sdl_graphics_manager & operator=(sdl_graphics_manager &&other); 
		sdl_graphics_manager& operator=(sdl_graphics_manager const&)  = delete;
			
		
		SDL_Window* window = nullptr; //The window we'll be rendering to
		SDL_Renderer* window_renderer = nullptr; //The window renderer
		SDL_Surface* window_surface = nullptr; 

		std::shared_ptr<asset> create_asset(tinyxml2::XMLElement * asset_xml_element, std::shared_ptr<settings_manager> config);
		
		bool initialize(const uint width = 800, uint height = 600, const char* window_title = nullptr);	

		void draw_current_scene(std::shared_ptr<scene_manager> scene_admin) const;
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