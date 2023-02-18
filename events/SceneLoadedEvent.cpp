#include "SceneLoadedEvent.h"
#include "Event.h"
#include "Events.h"
using namespace std;
namespace gamelib
{
	SceneLoadedEvent::SceneLoadedEvent(const int sceneId): Event(SceneLoadedEventId), SceneId(sceneId)
	{
	}

	string SceneLoadedEvent::ToString()
	{
		return "SceneLoadedEvent";
	}
}