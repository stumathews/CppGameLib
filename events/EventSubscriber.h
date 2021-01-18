#pragma once
#include "IEventSubscriber.h"
namespace gamelib
{
	class EventSubscriber : public IEventSubscriber
	{
		virtual int get_subscriber_id() override;
	};
}

