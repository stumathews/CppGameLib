#include "GameObjectEvent.h"

#include "Events.h"
using namespace std;

namespace gamelib
{

	GameObjectEvent::GameObjectEvent(shared_ptr<GameObject> gameObject, const GameObjectEventContext context)
		: Event(GameObjectTypeEventId), Context(context), Object(std::move(gameObject))
	{
	}
}
