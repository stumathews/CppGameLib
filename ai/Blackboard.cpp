
#include "Blackboard.h"
#include "BlackboardKeyRegistry.h"
#include <mutex>

namespace gamelib
{
    template<typename T>
    bool Bucket<T>::set(BlackboardKeyId id, const T& v, Writer src, uint32_t tick, float ttl, uint8_t flags)
    {
        auto& e = items[id.id];
        e.value = v;
        e.source = src;
        e.lastWriteTick = tick;
        e.ttlSeconds = ttl;
        e.flags = static_cast<uint8_t>(flags | Flags::Dirty);
        e.lastWriteTime = Clock::now();
        return true;
    }

    template<typename T>
    const T* Bucket<T>::get(BlackboardKeyId id) const {
        auto it = items.find(id.id);
        if (it==items.end()) return nullptr;
        return &it->second.value;
    }

    template<typename T>
    Entry<T>* Bucket<T>::raw(BlackboardKeyId id) {
        auto it = items.find(id.id);
        if (it==items.end()) return nullptr;
        return &it->second;
    }

    template<typename T>
    void Bucket<T>::decay(float dt)
    {
        // Prepare a list of indexes to erase once the items have decayed past their ttl times
        std::vector<uint16_t> toErase;
        toErase.reserve(items.size());

        for (auto& [k,e] : items)
        {
            // Still got time left?
            if (e.ttlSeconds > 0.f)
            {
                // Yes, reduce by tick time
                e.ttlSeconds -= dt;

                // Still go time?
                if (e.ttlSeconds <= 0.f)
                {
                    // No, schedule for erasure
                    toErase.push_back(k);
                }
            }
        }

        // Erase all expired items
        for (auto k : toErase)
        {
            items.erase(k);
        }
    }

    Blackboard::Blackboard() = default;
    Blackboard::~Blackboard() = default;

    template<typename T>
    bool Blackboard::Set(BlackboardKey<T>* keyTag, const T& value, Writer src, float ttl, bool replicate)
    {
        auto kid = keyTag->Id();

        // Stage write for thread safety (will be written when blackboard is updated/ticked)
        StagedWrite stagedWrite;
        stagedWrite.Type = std::type_index(typeid(T));
        stagedWrite.TheKeyId = kid;
        stagedWrite.Source = src;
        stagedWrite.TTL = ttl;
        stagedWrite.ExtraFlags = replicate ? Flags::Replicate : 0;

        // Setup the functions that will write the values
        if constexpr(std::is_same_v<T,bool>)
        {
            stagedWrite.apply = [=,this](BucketBase& bucket)
            {
                dynamic_cast<Bucket<bool>&>(bucket).set(kid, value, src, currentTick, ttl, stagedWrite.ExtraFlags);
            };
        }
        else if constexpr(std::is_same_v<T,int>)
        {
            stagedWrite.apply = [=,this](BucketBase& bucket)
            {
                dynamic_cast<Bucket<int>&>(bucket).set(kid, value, src, currentTick, ttl, stagedWrite.ExtraFlags);
            };
        }
        else if constexpr(std::is_same_v<T,float>)
        {
            stagedWrite.apply = [=,this](BucketBase& bucket)
            {
                dynamic_cast<Bucket<float>&>(bucket).set(kid, value, src, currentTick, ttl, stagedWrite.ExtraFlags);
            };
        }
        else if constexpr(std::is_same_v<T,uint64_t>)
        {
            stagedWrite.apply = [=,this](BucketBase& bucket)
            {
                dynamic_cast<Bucket<uint64_t>&>(bucket).set(kid, value, src, currentTick, ttl, stagedWrite.ExtraFlags);
            };
        }
        else if constexpr(std::is_same_v<T,double>)
        {
            stagedWrite.apply = [=,this](BucketBase& bucket)
            {
                dynamic_cast<Bucket<double>&>(bucket).set(kid, value, src, currentTick, ttl, stagedWrite.ExtraFlags);
            };
        }
        else
        {
            static_assert(!sizeof(T*), "Unsupported type in this minimal example");
        }

        // Obtain lock and push stage a write
        {
            std::lock_guard lock(q_m_);
            stagedWriteQueue.push(std::move(stagedWrite));
        }

        return true;
    }

    template<typename T>
    const T* Blackboard::Get(BlackboardKey<T>* keyTag) const
    {
        auto kid = keyTag->Id();
        if constexpr(std::is_same_v<T,bool>)
        {
            if (auto v = Bools.get(kid))
            {
                return v;
            }
        } else if constexpr(std::is_same_v<T,int>)
        {
            if (auto v = Ints.get(kid))
            {
                return v;
            }
        } else if constexpr(std::is_same_v<T,float>)
        {
            if (auto v = Floats.get(kid))
            {
                return v;
            }
        } else if constexpr(std::is_same_v<T,uint64_t>)
        {
            if (auto v = b_u64_.get(kid))
            {
                return v;
            }
        } else if constexpr(std::is_same_v<T,double>)
        {
            if (auto v = Doubles.get(kid))
            {
                return v;
            }
        } else
        {
            static_assert(!sizeof(T*), "Unsupported type in this minimal example");
        }

        // not found, check parent
        if (parent_ != nullptr)
        {
            return parent_->Get<T>(keyTag);
        }
        return nullptr;
    }

    void Blackboard::ApplyStaged()
    {
        std::queue<StagedWrite> local;
        {
            std::lock_guard lock(q_m_);
            std::swap(local, stagedWriteQueue);
        }

        while(!local.empty())
        {
            auto w = std::move(local.front());
            local.pop();

            // route into proper bucket
            if (w.Type == std::type_index(typeid(bool))) w.apply(Bools);
            else if (w.Type == std::type_index(typeid(int))) w.apply(Ints);
            else if (w.Type == std::type_index(typeid(float))) w.apply(Floats);
            else if (w.Type == std::type_index(typeid(uint64_t))) w.apply(b_u64_);
            else if (w.Type == std::type_index(typeid(double))) w.apply(Doubles);
            notify(w.TheKeyId);
        }
    }

    void Blackboard::Update(float dt)
    {
        ++currentTick;
        ApplyStaged();

        // Decay bucket values
        Bools.decay(dt);
        Ints.decay(dt);
        Floats.decay(dt);
        b_u64_.decay(dt);
        Doubles.decay(dt);
    }

    int Blackboard::Subscribe(BlackboardKeyId key, Callback cb)
    {
        // Make a new subscription to Key
        int subId = subscriptionId++;

        // Store it
        subscriptions[subId] = Subscription { key, std::move(cb) };
        key_to_subs_.emplace(key.id, subId);

        return subId;
    }

    void Blackboard::Unsubscribe(int subscription)
    {
        // Look for the subscription
        const auto it = subscriptions.find(subscription);

        // Did we find it?
        if (it==subscriptions.end())
        {
            // No
            return;
        }

        // Yes,
        const auto key = it->second.key;

        // Remove the subscription
        subscriptions.erase(it);

        auto range = key_to_subs_.equal_range(key.id);

        for (auto iter = range.first; iter != range.second; )
        {
            if (iter->second == subscription) iter = key_to_subs_.erase(iter); else ++iter;
        }
    }

    void Blackboard::notify(BlackboardKeyId k)
    {
        auto range = key_to_subs_.equal_range(k.id);
        for (auto it = range.first; it != range.second; ++it) {
            auto h = it->second;
            auto sit = subscriptions.find(h);
            if (sit != subscriptions.end() && sit->second.cb) sit->second.cb(k);
        }
    }

    // Explicit instantiations for supported types
    template bool Blackboard::Set<bool>(BlackboardKey<bool>*, const bool&, Writer, float, bool);
    template bool Blackboard::Set<int>(BlackboardKey<int>*, const int&, Writer, float, bool);
    template bool Blackboard::Set<float>(BlackboardKey<float>*, const float&, Writer, float, bool);
    template bool Blackboard::Set<uint64_t>(BlackboardKey<uint64_t>*, const uint64_t&, Writer, float, bool);
    template bool Blackboard::Set<double>(BlackboardKey<double>*, const double&, Writer, float, bool);

    template const bool* Blackboard::Get<bool>(BlackboardKey<bool>*) const;
    template const int* Blackboard::Get<int>(BlackboardKey<int>*) const;
    template const float* Blackboard::Get<float>(BlackboardKey<float>*) const;
    template const uint64_t* Blackboard::Get<uint64_t>(BlackboardKey<uint64_t>*) const;
    template const double* Blackboard::Get<double>(BlackboardKey<double>*) const;
}
