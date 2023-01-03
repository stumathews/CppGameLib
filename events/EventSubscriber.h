#pragma once
#include "IEventSubscriber.h"
namespace gamelib
{
	class EventSubscriber : public IEventSubscriber
	{
		int GetSubscriberId() override;
	};
}

