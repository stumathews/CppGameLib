#include "DoLogicUpdateEvent.h"

using namespace std;
namespace gamelib
{

	do_logic_update_event::do_logic_update_event(): event(event_type::DoLogicUpdateEventType)
	{
		// base class init only
	}

	string do_logic_update_event::to_str()
	{
		return "do_logic_update_event";
	}
}
