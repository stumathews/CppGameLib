//
// Created by stuart on 15/01/2026.
//

#ifndef CPPGAMELIB_CONDITION_H
#define CPPGAMELIB_CONDITION_H
#include "Behavior.h"

namespace gamelib
{
    class Condition : public Behavior
    {
    public:
        Condition(bool simulateNotCondition, Behavior* condition) : simulateNotCondition(simulateNotCondition),
                                                                    condition(condition)
        {
        }

        Status Update(unsigned long deltaMs) override
        {
            // Tick the child
            const auto result = condition->Update(deltaMs);

            // If simulating not, invert the result
            if (simulateNotCondition)
            {
                if (result == Status::Success)
                {
                    return Status::Failure;
                }
                if (result == Status::Failure)
                {
                    return Status::Success;
                }
            }
            return result;
        }
    private:
        bool simulateNotCondition;
        Behavior *condition;
    };
}

#endif //CPPGAMELIB_CONDITION_H