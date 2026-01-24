
#pragma once
#include <unordered_map>
#include <mutex>
#include <typeindex>
#include <string>
#include <vector>
#include "BlackboardKey.h"

namespace gamelib {
    class BlackboardKeyRegistry
    {
    public:
        static BlackboardKeyRegistry& Instance();

        template<typename T>
        BlackboardKeyId RegisterKey(std::string name)
        {
            std::lock_guard lock(theMutex);

            // Do we have a KeyId for this name?

            // ReSharper disable once CppTooWideScopeInitStatement
            const auto it = KeyNamesToKeyId.find(name);

            if (it != KeyNamesToKeyId.end())
            {
                // Yes
                return it->second;
            }

            // No, create new KeyId
            const BlackboardKeyId keyId {static_cast<uint16_t>(KeyMetaData.size()+1)}; // 0 reserved

            // Associate name with keyId
            KeyNamesToKeyId[name] = keyId;

            // Create meta-data for this Key and store the name in it
            KeyMetaData.push_back({
                keyId,
                std::type_index(typeid(T)),
                // Store the name in the meta-data storage and add reference to inserted name in metaData
                MetaStorage.emplace_back(std::move(name))
            });

            return keyId;
        }

        const KeyMeta* GetMetaData(BlackboardKeyId keyId) const
        {
            const auto index = keyId.id;
            if (index==0 || index > KeyMetaData.size())
            {
                return nullptr;
            }

            // Found the key's meta-data
            return &KeyMetaData[index-1];
        }

        BlackboardKeyId FindByName(std::string_view name) const
        {
            const auto it = KeyNamesToKeyId.find(std::string(name));

            if (it == KeyNamesToKeyId.end())
            {
                // Could not find Key by that name
                return {};
            }

            // Found key
            return it->second;
        }

    private:
        BlackboardKeyRegistry() = default;

        mutable std::mutex theMutex;
        std::unordered_map<std::string, BlackboardKeyId> KeyNamesToKeyId;
        std::vector<KeyMeta> KeyMetaData;

        // keep storage of names stable
        mutable std::vector<std::string> MetaStorage;
    };

    // Helper macro for key declaration/definition
    #define BB_DECLARE_KEY(type, name) \
    struct name : public gamelib::BlackboardKey<type> \
    { \
        gamelib::BlackboardKeyId Id() \
        { \
            static gamelib::BlackboardKeyId k = gamelib::BlackboardKeyRegistry::Instance().RegisterKey<type>(#name); \
            return k; \
        } \
        std::string_view Name() { return #name; } \
    };
}
