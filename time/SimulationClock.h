//
// Created by stuart on 11/1/26.
//

#ifndef CPPGAMELIB_SIMULATIONCLOCK_H
#define CPPGAMELIB_SIMULATIONCLOCK_H
#include <cstdint>

namespace gamelib
{
    class SimulationClock
    {
    public:
        explicit SimulationClock(uint32_t tickRate = 60)
            : tickRate(tickRate)
        {
        }

        void Tick()
        {
            ++currentTick;
        }

        uint64_t GetTick() const { return currentTick; }

        double GetSeconds() const {
            return TickToSeconds(currentTick, tickRate);
        }

        double GetMinutes() const {
            return TickToMinutes(currentTick, tickRate);
        }

        static uint64_t MinutesToTicks(double minutes, uint32_t tickRate = 60)
        {
            return static_cast<uint64_t>(minutes * tickRate * 60.0);
        }

        static double TickToSeconds(uint64_t tick, uint32_t tickRate = 60)
        {
            return static_cast<double>(tick) / tickRate;
        }

        static double TickToMinutes(uint64_t tick, uint32_t tickRate = 60)
        {
            return static_cast<double>(tick) / (tickRate * 60.0);
        }

        static uint64_t SecondsToTicks(double seconds, uint32_t tickRate = 60)
        {
            return static_cast<uint64_t>(seconds * tickRate);
        }

        static uint64_t TickToMilliseconds(uint64_t tick, uint32_t tickRate = 60)
        {
            return (tick * 1000) / tickRate;
        }

    private:
        uint64_t currentTick = 0;
        uint32_t tickRate;
    };
}

#endif //CPPGAMELIB_SIMULATIONCLOCK_H
