#pragma once
#include <memory>
#include <string>
#include <vector>
#include <common/Logger.h>
#include <WinSock2.h>
#include <events/EventSubscriber.h>


namespace gamelib
{
	class SerializationManager;
	class EventManager;
	class Networking;
	class EventFactory;
	class IGameServerConnection;

	class GameServer final : public EventSubscriber
	{
	public:
		GameServer(const std::string& address, const std::string& port);

		void Initialize();

		~GameServer() override;

		/// <summary>
		/// Checks to see if there is any data waiting to be read or written.
		/// </summary>
		void Listen() const;

		/// <summary>
		/// Check if there is any data waiting to be ready from any of the connected player sockets
		/// </summary>
		void CheckForPlayerTraffic() const;

		std::string address;
		std::string port;

	private:
				
		// Game server connection can either be in TCP or UDP
		std::shared_ptr<IGameServerConnection> gameServerConnection;
		
		/// <summary>
		/// Nick name of this machine
		/// </summary>
		std::string nickname;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs) override;

		std::string GetSubscriberName() override;

		SerializationManager* serializationManager{};
		EventManager* _eventManager{};
		Networking* networking{};
		EventFactory* _eventFactory{};
		bool isTcp;

	};
}

