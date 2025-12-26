#pragma once
#ifndef NETWORKING_ACTIVITY_MONITOR_H
#define NETWORKING_ACTIVITY_MONITOR_H

#include <processes/Action.h>
#include <processes/Process.h>
#include <utils/Utils.h>
#include <file/TextFile.h>

#include "NetworkingStatistics.h"
#include <events/EventManager.h>
#include <events/EventSubscriber.h>
#include "objects/GameObject.h"
#include "time/PeriodicTimer.h"

namespace gamelib
{
	class IElapsedTimeProvider;
	class ProcessManager;
	class EventManagert;
	class NetworkingActivityMonitor : public gamelib::EventSubscriber
	{
	public:
	
		NetworkingActivityMonitor(ProcessManager& processManager, EventManager& eventManager, std::shared_ptr<IElapsedTimeProvider> elapsedTimeProvider, bool verbose);
		void ScheduleSaveStatistics();
		void ScheduleProcess(std::shared_ptr<Process> process) const;
		void InitialiseStatisticsFile() const;
		void AppendStatsToFile(int tSeconds) const;
		void SaveAndReset(unsigned long deltaMs);
		void Initialise();
		ListOfEvents HandleEvent(const std::shared_ptr<Event>& evt, unsigned long inDeltaMs) override;
		std::string GetSubscriberName() override;
		void SetSendRateMs(int ms);
		void SetSendRatePs(int sec);
				
	private:

		NetworkingStatistics stats;
		std::shared_ptr<gamelib::TextFile> statisticsFile;
		gamelib::PeriodicTimer statisticsSampleInterval;
		ProcessManager& processManager;
		EventManager& eventManager;
		bool verbose;
		std::shared_ptr<IElapsedTimeProvider> elapsedTimeProvider;

		void OnReliableUdpAckPacketEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpPacketRttCalculatedEvent(const std::shared_ptr<gamelib::Event>& evt);
		void OnNetworkTrafficReceivedEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpPacketReceivedEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpCheckSumFailedEvent(const std::shared_ptr<Event>& evt);
		void OnReliableUdpPacketLossDetectedEvent(const std::shared_ptr<Event>& evt);
		void OnNetworkPlayerJoinedEvent(const std::shared_ptr<Event>& evt) const;
	};
}

#endif