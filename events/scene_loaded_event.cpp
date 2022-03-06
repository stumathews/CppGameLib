#include "scene_loaded_event.h"
#include "Event.h"
using namespace std;
namespace gamelib
{
	scene_loaded_event::scene_loaded_event(int scene_id): Event(EventType::scene_loaded), scene_id_(scene_id)
	{
	}

	string scene_loaded_event::to_str()
	{
		return "scene_loaded_event";
	}
}