
#include "NetworkingActivityMonitor.h"
#include <processes/ProcessManager.h>
#include <events/event.h>
#include <events/NetworkPlayerJoinedEvent.h>

#include "IElapsedTimeProvider.h"
#include "Message.h"
#include "events/NetworkTrafficReceivedEvent.h"
#include "events/ReliableUdpAckPacketEvent.h"
#include "events/ReliableUdpCheckSumFailedEvent.h"
#include "events/ReliableUdpPacketLossDetectedEvent.h"
#include "events/ReliableUdpPacketReceivedEvent.h"
#include "events/ReliableUdpPacketRttCalculatedEvent.h"
#include "file/Logger.h"

using namespace std;

namespace gamelib
{
	NetworkingActivityMonitor::NetworkingActivityMonitor(ProcessManager& processManager, EventManager& eventManager,
	                                                     std::shared_ptr<IElapsedTimeProvider> elapsedTimeProvider, const bool verbose = false):
		processManager(processManager), eventManager(eventManager), verbose(verbose),
		elapsedTimeProvider(std::move(elapsedTimeProvider))
	{
		// Prepare a file that will receive the statistics every saveIntervalMs
		statisticsFile = std::make_shared<TextFile>("statistics.txt");
	}

	void NetworkingActivityMonitor::Initialise()
	{
		constexpr auto saveIntervalMs = 1000; // every second

		// Subscribe to Network Events
		eventManager.SubscribeToEvent(NetworkTrafficReceivedEventId, this);
		eventManager.SubscribeToEvent(ReliableUdpPacketReceivedEventId, this);
		eventManager.SubscribeToEvent(ReliableUdpCheckSumFailedEventId, this);
		eventManager.SubscribeToEvent(ReliableUdpPacketLossDetectedEventId, this);
		eventManager.SubscribeToEvent(ReliableUdpAckPacketEventId, this);
		eventManager.SubscribeToEvent(ReliableUdpPacketRttCalculatedEventId, this);

		// Schedule save operation to run periodically
		statisticsSampleInterval.SetFrequency(saveIntervalMs);

		InitialiseStatisticsFile();

		// Schedule sampler to run as a background process
		ScheduleSaveStatistics();
	}

	// Handle events
	ListOfEvents NetworkingActivityMonitor::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long inDeltaMs)
	{
		if (evt->Id.PrimaryId == NetworkPlayerJoinedEventId.PrimaryId) { OnNetworkPlayerJoinedEvent(evt); }
		if (evt->Id.PrimaryId == ReliableUdpPacketReceivedEventId.PrimaryId) { OnReliableUdpPacketReceivedEvent(evt); }
		if (evt->Id.PrimaryId == ReliableUdpCheckSumFailedEventId.PrimaryId) { OnReliableUdpCheckSumFailedEvent(evt); }
		if (evt->Id.PrimaryId == ReliableUdpPacketLossDetectedEventId.PrimaryId) { OnReliableUdpPacketLossDetectedEvent(evt); }
		if (evt->Id.PrimaryId == ReliableUdpAckPacketEventId.PrimaryId) { OnReliableUdpAckPacketEvent(evt); }
		if (evt->Id.PrimaryId == ReliableUdpPacketRttCalculatedEventId.PrimaryId) { OnReliableUdpPacketRttCalculatedEvent(evt); }
		if (evt->Id.PrimaryId == NetworkTrafficReceivedEventId.PrimaryId) { OnNetworkTrafficReceivedEvent(evt); }
		return {};
	}

	void NetworkingActivityMonitor::InitialiseStatisticsFile() const
	{
		// Setup statistics file header (first line)
		std::stringstream header;
		header << "TimeSecs" << "\t"
			<< "BytesReceived" << "\t"
			<< "CountPacketsLost" << "\t"
			<< "CountPacketsReceived" << "\t"
			<< "AverageLatencyMsSma3" << "\t"
			<< "CountAcks" << "\t"
			<< "VerificationFailedCount" << "\t"
			<< "CountAggregateMessagesReceived" << "\t"
			<< "SendingRateMs" << "\t"
			<< "SendingRatePs" << "\t"
			<< "RttMs"
			<< "\n";

		statisticsFile->Append(header.str(), false);
	}

	void NetworkingActivityMonitor::AppendStatsToFile(const int tSeconds) const
	{
		std::stringstream message;

		// Fetch statistics and add them to the log file (see header)
		message
			<< tSeconds << "\t"
			<< stats.BytesReceived << "\t"
			<< stats.CountPacketsLost << "\t"
			<< stats.CountPacketsReceived << "\t"
			<< stats.AverageLatencySMA3 << "\t"
			<< stats.CountAcks << "\t"
			<< stats.VerificationFailedCount << "\t"
			<< stats.CountAggregateMessagesReceived << "\t"
			<< stats.SendRateMs << "\t"
			<< stats.SendRatePs << "\t"
			<< stats.RttMs
			<< "\n";

		// Write to file
		statisticsFile->Append(message.str(), false);
	}

	void NetworkingActivityMonitor::SaveAndReset(const unsigned long deltaMs)
	{
		// Progress the interval
		statisticsSampleInterval.Update(deltaMs);

		// Will only write statistics to file if the statistics sampling interval has elapsed (ready)
		statisticsSampleInterval.DoIfReady([&]()
		{
			// Record the current second we are in since game was started, this will be the independent variable
			const auto tNowSecs = elapsedTimeProvider->GetElapsedTime();

			AppendStatsToFile(tNowSecs);

			// Reset statistics to zero.
			stats.Reset();
		});
	}

	void NetworkingActivityMonitor::ScheduleSaveStatistics()
	{
		// Schedule process to write statistics to file

		const auto process = std::make_shared<Action>([this](auto&& deltaMs) { SaveAndReset(deltaMs); }, false);

		ScheduleProcess(process);
	}

	// ReSharper disable once CppPassValueParameterByConstReference
	void NetworkingActivityMonitor::ScheduleProcess(shared_ptr<Process> process) const
	{
		// Add this process to the process manager - schedules process to receive updates
		processManager.AttachProcess(std::static_pointer_cast<Process>(process));
	}

	std::string NetworkingActivityMonitor::GetSubscriberName()
	{
		return "NetworkStatistics";
	}
	
	void NetworkingActivityMonitor::OnNetworkPlayerJoinedEvent(const std::shared_ptr<Event>& evt) const
	{
		if (verbose)
		{
			const auto joinEvent = To<NetworkPlayerJoinedEvent>(evt);
			stringstream message;
			message << joinEvent->Player.GetNickName() << " joined.";
			Logger::Get()->LogThis(message.str());
		}
	}

	void NetworkingActivityMonitor::OnNetworkTrafficReceivedEvent(const std::shared_ptr<Event>& evt)
	{
		const auto networkPlayerTrafficReceivedEvent = To<NetworkTrafficReceivedEvent>(evt);
		
		// how much data did we receive in this second?
		stats.BytesReceived += networkPlayerTrafficReceivedEvent->BytesReceived;

		if (verbose)
		{
			std::stringstream message;
			message
				<< networkPlayerTrafficReceivedEvent->BytesReceived << " bytes of data received from " << networkPlayerTrafficReceivedEvent->Identifier
				<< ". Message: \"" << networkPlayerTrafficReceivedEvent->Message << "\"";

			Logger::Get()->LogThis(message.str());
		}
	}

	void NetworkingActivityMonitor::OnReliableUdpPacketReceivedEvent(const std::shared_ptr<Event>& evt)
	{
		const auto rudpEvent = To<ReliableUdpPacketReceivedEvent>(evt);
		const auto rudpMessage = rudpEvent->ReceivedMessage;
		
		// write stats
		stats.CountPacketsReceived++;
		stats.CountAggregateMessagesReceived += rudpMessage->DataCount();

		if (verbose)
		{
			stringstream bundledSeqs;

			bundledSeqs << "(";
			for (int i = 0; i < rudpMessage->DataCount(); i++)
			{
				bundledSeqs << rudpMessage->Data()[i].Sequence;
				if (i < rudpMessage->DataCount() - 1)
				{
					bundledSeqs << ",";
				}
			}
			bundledSeqs << ")";

			std::stringstream message;
			message
				<< "Received " << rudpMessage->Header.Sequence << ". Playload: " << bundledSeqs.str()
				<< " Sender acks: "
				<< BitFiddler<uint32_t>::ToString(rudpMessage->Header.LastAckedSequence);

			Logger::Get()->LogThis(message.str());
		}
	}

	void NetworkingActivityMonitor::OnReliableUdpCheckSumFailedEvent(const std::shared_ptr<Event>& evt)
	{		
		// write stats
		stats.VerificationFailedCount++;

		if (verbose) 
		{
			const auto failedChecksumEvent = To<ReliableUdpCheckSumFailedEvent>(evt);
			const auto failedMessage = failedChecksumEvent->failedMessage;

			std::stringstream message;
			message << "Checksum failed for sequence " << failedMessage->Header.Sequence << ". Dropping packet.";
			Logger::Get()->LogThis(message.str());
		}
	}

	void NetworkingActivityMonitor::OnReliableUdpPacketLossDetectedEvent(const std::shared_ptr<Event>& evt)
	{
		// Track statistics
		stats.CountPacketsLost++;

		if (verbose)
		{
			const auto reliableUdpPacketLossDetectedEvent = To<ReliableUdpPacketLossDetectedEvent>(evt);
			const auto resendingMessage = reliableUdpPacketLossDetectedEvent->messageBundle;

			stringstream bundledSequences;
			std::stringstream message;

			for (int i = static_cast<int>(resendingMessage->Data().size()) - 1; i >= 0; i--)
			{
				bundledSequences << resendingMessage->Data()[i].Sequence;
				if (i < resendingMessage->DataCount() - 1)
				{
					bundledSequences << ",";
				}
			}

			message << "Packet loss detected. Sequences " << bundledSequences.str() << " were not acknowledged by receiver and will be resent with sending sequence "
				<< resendingMessage->Header.Sequence;

			Logger::Get()->LogThis(message.str());
		}
	}

	void NetworkingActivityMonitor::OnReliableUdpAckPacketEvent(const std::shared_ptr<Event>& evt)
	{
		// how many acknowledgements did we receive?
		stats.CountAcks++;

		if (verbose)
		{
			const auto reliableUdpPacketLossDetectedEvent = To<ReliableUdpAckPacketEvent>(evt);
			const auto ackMessage = reliableUdpPacketLossDetectedEvent->ReceivedMessage;

			std::stringstream message;

			message << "Acknowledgement " << (reliableUdpPacketLossDetectedEvent->Sent ? "sent: " : " received: ") << ackMessage->Header.Sequence;

			Logger::Get()->LogThis(message.str());
		}
	}

	void NetworkingActivityMonitor::OnReliableUdpPacketRttCalculatedEvent(const std::shared_ptr<gamelib::Event>& evt)
	{
		const auto rttEvent = To<ReliableUdpPacketRttCalculatedEvent>(evt);

		// The last latency recorded is a smooth moving average considering last 3 packets
		stats.AverageLatencySMA3 = rttEvent->Rtt.Sma3;
		stats.RttMs = static_cast<int>(rttEvent->Rtt.rtt);
	}

	void NetworkingActivityMonitor::SetSendRateMs(const int ms)
	{
		stats.SendRateMs = ms;
	}
	void NetworkingActivityMonitor::SetSendRatePs(const int sec)
	{
		stats.SendRatePs = sec;
	}
}