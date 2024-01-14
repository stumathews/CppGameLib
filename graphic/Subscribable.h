#pragma once
#include "events/EventSubscriber.h"
namespace gamelib
{
	/**
	 * \brief Allows any object to be subscribe and be notified about events
	 * \tparam T Underlying item
	 */
	template <typename T>
	class Subscribable: public EventSubscriber
	{
	public:
		Subscribable(const std::shared_ptr<T>& item, std::string subscriberName)
			: item(item), subscriberName(std::move(subscriberName)) {}
		std::shared_ptr<T> GetItem() { return item;}
		std::string GetSubscriberName() override { return subscriberName; }

	private:
		std::shared_ptr<T> item;
		std::string subscriberName;
	};
}
