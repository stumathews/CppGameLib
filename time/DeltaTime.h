//
// Created by stuart on 11/1/26.
//

#ifndef CPPGAMELIB_DELTATIME_H
#define CPPGAMELIB_DELTATIME_H

namespace cppgamelib
{
    class DeltaTime
    {
    public:
        unsigned long milliseconds;
        float GetSeconds() const
        {
            return milliseconds / 1000.0f;
        };
    };

}

#endif //CPPGAMELIB_DELTATIME_H
