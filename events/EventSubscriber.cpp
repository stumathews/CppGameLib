#include "EventSubscriber.h"
#include <common/Common.h>

int gamelib::EventSubscriber::GetSubscriberId()
{
	return get_random_int(0, INT_MAX);
}
