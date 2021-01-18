#include "EventSubscriber.h"
#include <common/Common.h>

int gamelib::EventSubscriber::get_subscriber_id()
{
	return get_random_int(0, INT_MAX);
}
