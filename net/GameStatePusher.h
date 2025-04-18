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
		GameStatePusher(const int sendRateMs, const bool isGameServer, gamelib::ProcessManager& processManager);

		void Initialise(const std::function<void()>& sendGameStateFunc);
		void Run() const;

	private:
		bool isGameServer;

		// Periodically send game state to the game server
		int sendRateMs;

		std::shared_ptr<gamelib::Process> sendTask;

		gamelib::PeriodicTimer gameStateSendInterval;
		gamelib::ProcessManager& processManager;
	};
}

#endif

