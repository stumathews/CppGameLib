#include <iostream>


#include "audio/AudioManager.h"
#include "common/StaticConfig.h"
#include "events/EventManager.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include "resource/ResourceManager.h"
#include "scene/SceneManager.h"


#pragma comment(lib, "cppgamelib")
using namespace std;
using namespace gamelib;

int main(int argc, char **argv)
{
	SettingsManager settings_admin;
	Logger logger;
	shared_ptr<gamelib::EventManager> event_admin(new gamelib::EventManager(settings_admin, logger));
	event_admin->Initialize();
	
	shared_ptr<gamelib::FontManager> font_admin(new gamelib::FontManager);		
	shared_ptr<gamelib::AudioManager> audio_admin(new gamelib::AudioManager);
	shared_ptr<gamelib::SDLGraphicsManager> graphics_admin(new gamelib::SDLGraphicsManager(*event_admin, logger));
	
	graphics_admin->Initialize();
	
	shared_ptr<gamelib::ResourceManager> resource_admin(new gamelib::ResourceManager(settings_admin, *graphics_admin, *font_admin, *audio_admin, logger ));
		
	resource_admin->Initialize(*event_admin);
	
	GameWorldData world;
	shared_ptr<gamelib::SceneManager> scene_admin(new gamelib::SceneManager(*event_admin, settings_admin, *resource_admin, world, logger));
    scene_admin->Initialize("game/");
	
	cout << "Hello World" << endl;
	int i;
	cin >> i;
	return 0;
}