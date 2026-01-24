
#pragma once
#include <cstdint>
#include <typeindex>
#include <string_view>

namespace gamelib
{
    // Id
    struct BlackboardKeyId
    {
        uint16_t id = 0;
    };

    // Base key metadata registered at startup for validation
    struct KeyMeta
    {
        BlackboardKeyId id {};
        std::type_index type {typeid(void)};
        std::string_view name {};
    };

    // A typed key acts as a tag for lookups
    template <typename T>
    struct BlackboardKey
    {
        virtual ~BlackboardKey() = default;

        virtual BlackboardKeyId Id() = 0;
        virtual std::string_view Name() = 0;
    };
}
