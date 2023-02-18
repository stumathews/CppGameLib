#include "UpdateAllGameObjectsEvent.h"

#include "Events.h"

using namespace std;
namespace gamelib
{

	/// <summary>
	/// Logic Update Event initializer
	/// </summary>
	UpdateAllGameObjectsEvent::UpdateAllGameObjectsEvent() 
		: Event(UpdateAllGameObjectsEventTypeEventId){}

	/// <summary>
	/// Provide a string representation of event
	/// </summary>
	/// <returns></returns>
	string UpdateAllGameObjectsEvent::ToString()
	{
		return "LogicUpdateEvent";
	}
}
