#include "UpdateAllGameObjectsEvent.h"

using namespace std;
namespace gamelib
{

	/// <summary>
	/// Logic Update Event initializer
	/// </summary>
	UpdateAllGameObjectsEvent::UpdateAllGameObjectsEvent(unsigned long deltaMs) 
		: Event(EventType::UpdateAllGameObjectsEventType), deltaMs(deltaMs) {}

	/// <summary>
	/// Provide a string representation of event
	/// </summary>
	/// <returns></returns>
	string UpdateAllGameObjectsEvent::ToString()
	{
		return "LogicUpdateEvent";
	}
}
