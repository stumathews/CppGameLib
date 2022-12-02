#include "UpdateAllGameObjectsEvent.h"

using namespace std;
namespace gamelib
{

	/// <summary>
	/// Logic Update Event initializer
	/// </summary>
	UpdateAllGameObjectsEvent::UpdateAllGameObjectsEvent() 
		: Event(EventType::UpdateAllGameObjectsEventType){}

	/// <summary>
	/// Provide a string representation of event
	/// </summary>
	/// <returns></returns>
	string UpdateAllGameObjectsEvent::ToString()
	{
		return "LogicUpdateEvent";
	}
}
