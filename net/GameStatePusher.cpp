#include "GameStatePusher.h"

namespace gamelib
{
	GameStatePusher::GameStatePusher(const int sendRateMs, const bool isGameServer,
		gamelib::ProcessManager& processManager): isGameServer(isGameServer), sendRateMs(sendRateMs), processManager(processManager)
	{
	}

	void GameStatePusher::Initialise(const std::function<void()>& sendGameStateFunc)
	{
		if (isGameServer)
		{
			// We won't send out periodic state to the game server if we are the game server
			return;
		}

		// Send game state every sendRateMs
		gameStateSendInterval.SetFrequency(sendRateMs);


		// Send the game state as a background task
		sendTask = std::dynamic_pointer_cast<gamelib::Process>(std::make_shared<gamelib::Action>([&](const unsigned long deltaMs)
		{
			// Update timers
			gameStateSendInterval.Update(deltaMs);

			// Send game state of sending interval is elapsed
			gameStateSendInterval.DoIfReady([=]()
			{
				// We represent our game state as rudimentary pings
				sendGameStateFunc();
			});
		}, false));
	}

	void GameStatePusher::Run() const
	{
		// Add as a background task
		processManager.AttachProcess(sendTask);
	}
}
