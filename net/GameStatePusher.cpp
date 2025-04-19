#include "GameStatePusher.h"

namespace gamelib
{
	GameStatePusher::GameStatePusher(const std::function<void()>& sendGameStateFunc, const int sendRateMs, const bool isGameServer,
		ProcessManager& processManager): isGameServer(isGameServer), sendRateMs(sendRateMs), processManager(processManager),
		                                          sendGameStateFunc(sendGameStateFunc)
	{
	}

	void GameStatePusher::Initialise()
	{
		if (isGameServer)
		{
			// We won't send out periodic state to the game server if we are the game server
			return;
		}

		// Send game state every sendRateMs
		gameStateSendInterval.SetFrequency(sendRateMs);

		// create task to send the game state as a background task
		sendTask = std::dynamic_pointer_cast<Process>(std::make_shared<Action>([&](const unsigned long deltaMs)
		{
			// Update timers
			gameStateSendInterval.Update(deltaMs);

			// Send game state of sending interval is elapsed
			gameStateSendInterval.DoIfReady([&]()
			{
					if (sendGameStateFunc) 
					{
						sendGameStateFunc();
					}
			});
		}, false));
	}

	void GameStatePusher::Run() const
	{
		// Add as a background task
		processManager.AttachProcess(sendTask);
	}
}
