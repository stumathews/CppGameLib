#pragma once
#include "IEventSubscriber.h"
namespace gamelib
{
	class EventSubscriber : public IEventSubscriber
	{
		virtual int GetSubscriberId() override;
	};
}

