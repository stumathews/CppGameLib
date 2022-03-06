#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Event.h"

namespace gamelib
{

	class IEventSubscriber
	{
		public:
		//virtual ~IEventSubscriber();
		virtual std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> evt) = 0;
		virtual std::string get_subscriber_name() = 0;
		virtual int get_subscriber_id() = 0;
	};
}

