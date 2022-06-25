#pragma once

#include <events/PlayerMovedEvent.h>
#include <events/IEventSerializationManager.h>
#include <memory>
#include <string>
#include <events/NetworkTrafficRecievedEvent.h>

namespace gamelib
{
	class EventFactory
	{
	protected:
			static EventFactory* Instance;
	private:
			std::shared_ptr<IEventSerializationManager> eventSerializationManager;
	public:
		static EventFactory* Get();
		EventFactory();
		bool Initialize();

		// Cannot copy an EventFactory
		EventFactory(EventFactory const&) = delete;
	
		~EventFactory();
		
		// Cannot assign to an EventFactory
		void operator=(EventFactory const&) = delete;

		std::shared_ptr<PlayerMovedEvent> CreatePlayerMovedEvent(gamelib::Direction direction, std::string target);
		std::shared_ptr<PlayerMovedEvent> CreatePlayerMovedEvent(std::string serializedMessage);

		std::shared_ptr<NetworkTrafficRecievedEvent> CreateNetworkTrafficReceivedEvent(std::string message, std::string identifier, int bytesReceived);
	};
}

