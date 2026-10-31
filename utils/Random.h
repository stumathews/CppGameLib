//
// Created by stuart on 10/31/26.
//

#ifndef CPPGAMELIB_RANDOM_H
#define CPPGAMELIB_RANDOM_H

#include <random>
#include <cstdint>

namespace gamelib
{
    class Random
    {
    public:
        explicit Random(uint32_t seed)
            : generator(seed)
        {
        }

        int Int(int min, int max)
        {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(generator);
        }

        float Float(float min, float max)
        {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(generator);
        }

        bool Chance(float probability)
        {
            return Float(0.0f, 1.0f) < probability;
        }

    private:
        std::mt19937 generator;
    };
}

#endif //CPPGAMELIB_RANDOM_H
