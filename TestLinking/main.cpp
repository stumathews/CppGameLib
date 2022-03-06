#include <iostream>


#include "audio/AudioManager.h"
#include "common/static_config.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/sdl_graphics_manager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"


#pragma comment(lib, "cppgamelib")
using namespace std;
using namespace gamelib;

int main(int argc, char **argv)
{
	const SettingsManager& settings_admin(new gamelib::SettingsManager);
	const shared_ptr<gamelib::EventManager> event_admin(new gamelib::EventManager(settings_admin));
		event_admin->initialize();
	const shared_ptr<gamelib::FontManager> font_admin(new gamelib::FontManager);		
	const shared_ptr<gamelib::AudioManager> audio_admin(new gamelib::AudioManager);
	const shared_ptr<gamelib::sdl_graphics_manager> graphics_admin(new gamelib::sdl_graphics_manager(event_admin));
		graphics_admin->initialize();
	const shared_ptr<gamelib::ResourceManager> resource_admin(new gamelib::ResourceManager(settings_admin, graphics_admin, font_admin, audio_admin ));
		resource_admin->initialize(event_admin);
	const shared_ptr<gamelib::SceneManager> scene_admin(new gamelib::SceneManager(event_admin, settings_admin, resource_admin));
		scene_admin->initialize();
	
	cout << "Hello World" << endl;
	int i;
	cin >> i;
	return 0;
}