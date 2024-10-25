#pragma once
#include <memory>
#include <string>
#include <vector>
#include <file/Logger.h>
#include <events/EventSubscriber.h>

#include "file/SerializationManager.h"


namespace gamelib
{
	class SerializationManager;
	class EventManager;
	class Networking;
	class EventFactory;
	class IGameServerConnection;

	/**
	 * \brief Game server listens for game client traffic and sends game state updates to game clients
	 */
	class GameServer final : public EventSubscriber
	{
	public:
		GameServer(const std::string& address, const std::string& port,
		           std::shared_ptr<IGameServerConnection> gameServerConnection, const std::string& nickName = "Server",
			Encoding wireFormat = Encoding::Json);
		GameServer(const GameServer& other) = delete;
		GameServer(const GameServer&& other) = delete;
		GameServer& operator=(GameServer& other) = delete;
		GameServer& operator=(GameServer&& other) = delete;

		void Initialize();

		~GameServer() override;

		// Checks to see if there is any data waiting to be read or written.
		void Listen(unsigned long deltaMs = 0) const;

		// Disconnect all clients by closing game server's listening socket
		void Disconnect() const;

		// Information about the Game server
		std::string Address;
		std::string Port;
		
	private:
				
		// Game server connection manages the network protocol used between server and client
		std::shared_ptr<IGameServerConnection> gameServerConnection;
				
		// Desired format to use to send network data, eg. Xml, Json, Bit-packed
		Encoding encoding;
		
		/// Nick name of this server
		std::string nickname;

		// Check if there is any data waiting to be ready from any of the connected player sockets
		void CheckForPlayerTraffic(unsigned long deltaMs) const;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, unsigned long deltaMs) override;

		std::string GetSubscriberName() override;

		// Used to create messages to send to the client
		std::shared_ptr<SerializationManager> serializationManager{};

		// Used to listen and emit game events
		EventManager* eventManager{};

		// Low-level network functions
		Networking* networking{};

		// Creates types of events that can be sent to the event manager
		EventFactory* eventFactory{};

	};
}

