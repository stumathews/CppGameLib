#pragma once
#ifndef GAMESTATE_PUSHER_H
#define GAMESTATE_PUSHER_H

#include <functional>
#include "processes/Action.h"
#include "processes/Process.h"
#include "processes/ProcessManager.h"
#include "time/PeriodicTimer.h"

namespace gamelib
{
	class GameStatePusher
	{
	public:
		void Initialise();
		GameStatePusher(const std::function<void()>& sendGameStateFunc, int sendRateMs, bool isGameServer, ProcessManager& processManager);
		void Run() const;

	private:
		bool isGameServer;

		// Periodically send game state to the game server
		int sendRateMs;

		std::shared_ptr<Process> sendTask;

		PeriodicTimer gameStateSendInterval;
		ProcessManager& processManager;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
		std::function<void()> sendGameStateFunc;
	};
}

#endif

