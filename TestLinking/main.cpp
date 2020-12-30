#include <iostream>


#include "audio/AudioManager.h"
#include "common/static_config.h"
#include "events/event_manager.h"
#include "font/font_manager.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/resource_manager.h"
#include "scene/scene_manager.h"


#pragma comment(lib, "cppgamelib")
using namespace std;
using namespace gamelib;

int main(int argc, char **argv)
{
	const shared_ptr<settings_manager> settings_admin(new gamelib::settings_manager);
	const shared_ptr<gamelib::event_manager> event_admin(new gamelib::event_manager(settings_admin));
		event_admin->initialize();
	const shared_ptr<gamelib::font_manager> font_admin(new gamelib::font_manager);		
	const shared_ptr<gamelib::audio_manager> audio_admin(new gamelib::audio_manager);
	const shared_ptr<gamelib::sdl_graphics_manager> graphics_admin(new gamelib::sdl_graphics_manager(event_admin));
		graphics_admin->initialize();
	const shared_ptr<gamelib::resource_manager> resource_admin(new gamelib::resource_manager(settings_admin, graphics_admin, font_admin, audio_admin ));
		resource_admin->initialize(event_admin);
	const shared_ptr<gamelib::scene_manager> scene_admin(new gamelib::scene_manager(event_admin, settings_admin, resource_admin));
		scene_admin->initialize();
	
	cout << "Hello World" << endl;
	int i;
	cin >> i;
	return 0;
}