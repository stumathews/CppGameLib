#pragma once
#ifndef NETWORKING_ACTIVITY_MONITOR_H
#define NETWORKING_ACTIVITY_MONITOR_H

#include <GameDataManager.h>
#include <processes\Action.h>
#include <processes\Process.h>
#include <utils\Utils.h>
#include <file/TextFile.h>
#include "NetworkingStatistics.h"

namespace gamelib
{
	class NetworkingActivityMonitor : public gamelib::EventSubscriber
	{
	public:
	
		NetworkingActivityMonitor(ProcessManager& processManager, EventManager& eventManager);
		void ScheduleSaveStatistics();
		void ScheduleProcess(std::shared_ptr<Process> process) const;
		void InitialiseStatisticsFile() const;
		void AppendStatsToFile(int tSeconds) const;
		void SaveAndReset(unsigned long deltaMs);
		void Initialise();
		ListOfEvents HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long inDeltaMs) override;
		std::string GetSubscriberName() override;
		void SetSendRateMs(int ms);
		void SetSendRatePs(int sec);
				
	private:

		NetworkingStatistics stats;
		std::shared_ptr<gamelib::TextFile> statisticsFile;
		gamelib::PeriodicTimer statisticsSampleInterval;
		ProcessManager& processManager;
		EventManager& eventManager;

		void OnReliableUdpAckPacketEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpPacketRttCalculatedEvent(const std::shared_ptr<gamelib::Event>& evt);
		void OnNetworkTrafficReceivedEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpPacketReceivedEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpCheckSumFailedEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpPacketLossDetectedEvent(const std::shared_ptr<Event>& evt);
		static void OnNetworkPlayerJoinedEvent(const std::shared_ptr<Event>& evt);
	};
}

#endif