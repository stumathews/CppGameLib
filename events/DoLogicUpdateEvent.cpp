#include "DoLogicUpdateEvent.h"

using namespace std;
namespace gamelib
{

	/// <summary>
	/// Logic Update Event initializer
	/// </summary>
	LogicUpdateEvent::LogicUpdateEvent(float deltaMs) 
		: Event(EventType::DoLogicUpdateEventType), deltaMs(deltaMs) {}

	/// <summary>
	/// Provide a string representation of event
	/// </summary>
	/// <returns></returns>
	string LogicUpdateEvent::ToString()
	{
		return "LogicUpdateEvent";
	}
}
