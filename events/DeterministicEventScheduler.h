//
// Created by stuart on 10/31/26.
//

#ifndef CPPGAMELIB_DETERMINISTICEVENTSCHEDULER_H
#define CPPGAMELIB_DETERMINISTICEVENTSCHEDULER_H
#include <cstdint>
#include <functional>

namespace gamelib
{
    struct ScheduledEvent
    {
        uint64_t tick;
        std::function<void()> action;
    };

    class DeterministicEventScheduler
    {
    public:
        void Schedule(const uint64_t tick, std::function<void()> action)
        {
            events.push_back({ tick, action });
        }

        void Update(uint64_t currentTick) const
        {
            for (const auto& event : events)
            {
                if (event.tick == currentTick)
                {
                    event.action();
                }
            }
        }

    private:
        std::vector<ScheduledEvent> events;
    };
}

#endif //CPPGAMELIB_DETERMINISTICEVENTSCHEDULER_H
