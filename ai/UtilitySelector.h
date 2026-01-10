//
// Created by stuart on 10/01/2026.
//

#ifndef CPPGAMELIB_UTILITYSELECTOR_H
#define CPPGAMELIB_UTILITYSELECTOR_H

#pragma once
#include <map>

#include "Composite.h"
#include <algorithm>

namespace gamelib
{
    // A selector that selects the child highest utility first
    class UtilitySelector : public Composite
    {
    public:

        virtual ~UtilitySelector() override = default;

        virtual void OnInitialize() override
        {
            // Start at the first child
            child = children.begin();
        }

        Status Update(const unsigned long deltaMs) override
        {
            // Sort children by greatest utility first
            std::sort(children.begin(), children.end(), [](Behavior* a, Behavior* b)
            {
                return a->CalculateUtility() > b->CalculateUtility();
            });

            // Persistently keep running the last successful child (first child will have the highest utility)
            while(child != children.end())
            {
                const auto childStatus = (*child)->Tick();

                // If any child succeeds or is running, we do too
                if(childStatus != Status::Failure)
                {
                    return childStatus;
                }

                ++child;
            }

            // Unexpected loop exit
            return Status::Failure;
        }
    std::vector<Behavior*> GetChildren() { return children;};
    protected:
        Behaviors::iterator child; // the child selector keeps running its current child until it fails....
        std::map<float, Behavior*> Utility;
    };
}


#endif //CPPGAMELIB_UTILITYSELECTOR_H