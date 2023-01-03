#pragma once

#include <events/PlayerMovedEvent.h>
#include <events/IEventSerializationManager.h>
#include <memory>
#include <string>
#include <events/NetworkTrafficReceivedEvent.h>
#include <events/SceneChangedEvent.h>
#include <net/NetworkPlayer.h>

namespace gamelib
{
	class NetworkPlayerJoinedEvent;
	class AddGameObjectToCurrentSceneEvent;
	class GameObject;

	class EventFactory
	{
	protected:
			static EventFactory* Instance;
	private:
			std::shared_ptr<IEventSerializationManager> eventSerializationManager;
	public:
		static EventFactory* Get();
		EventFactory();
			static bool Initialize();

		// Cannot copy an EventFactory
		EventFactory(EventFactory const&) = delete;
	
		~EventFactory();
		
		// Cannot assign to an EventFactory
		void operator=(EventFactory const&) = delete;

			[[nodiscard]] std::shared_ptr<PlayerMovedEvent> CreatePlayerMovedEvent(Direction direction, const std::string& target = "") const;
			[[nodiscard]] std::shared_ptr<PlayerMovedEvent> CreatePlayerMovedEvent(const std::string& serializedMessage) const;
			[[nodiscard]] std::shared_ptr<NetworkTrafficReceivedEvent> CreateNetworkTrafficReceivedEvent(const std::string& message,
			const std::string& identifier, int bytesReceived) const;
			[[nodiscard]] std::shared_ptr<SceneChangedEvent> CreateLevelEvent(int level) const;
			[[nodiscard]] std::shared_ptr<StartNetworkLevelEvent> CreateStartNetworkLevelEvent(int level) const;
			[[nodiscard]] std::shared_ptr<Event> CreateNetworkPlayerJoinedEvent(const NetworkPlayer& player) const;
			[[nodiscard]] static std::shared_ptr<AddGameObjectToCurrentSceneEvent> CreateAddToSceneEvent(const std::shared_ptr<GameObject>& obj);
			[[nodiscard]] std::shared_ptr<Event> CreateGenericEvent(EventType type) const;
	};
}

