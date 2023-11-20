#include "UpdateAllGameObjectsEvent.h"

using namespace std;

namespace gamelib
{
	UpdateAllGameObjectsEvent::UpdateAllGameObjectsEvent() 
		: Event(UpdateAllGameObjectsEventTypeEventId){}
	
	string UpdateAllGameObjectsEvent::ToString()
	{
		return "LogicUpdateEvent";
	}
}
