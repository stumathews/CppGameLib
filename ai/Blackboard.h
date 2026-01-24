
#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <optional>
#include <variant>
#include <unordered_map>
#include <shared_mutex>
#include <queue>
#include <chrono>
#include "BlackboardKey.h"

namespace gamelib {
    using Clock = std::chrono::steady_clock;

    enum class Writer : uint8_t
    {
        Unknown=0,
        Perception=1,
        Planner=2,
        Behavior=3,
        Designer=4
    };

    struct Flags
    {
        enum : uint8_t
        {
            None=0,
            Dirty=1<<0,
            Ephemeral=1<<1, Replicate=1<<2
        };
    };

    struct EntryHeader
    {
        uint32_t lastWriteTick{0};
        Writer source{Writer::Unknown};
        uint8_t flags{0};
        float ttlSeconds{0.f}; // 0 = infinite
        Clock::time_point lastWriteTime{Clock::now()};
    };

    template<typename T>
    struct Entry : EntryHeader
    {
        T value{};
    };

    // Storage buckets per type; for brevity we implement a small set
    struct BucketBase
    {
        virtual ~BucketBase() = default;
    };

    template<typename T>
    class Bucket : public BucketBase
    {
    public:

        // Add an item to this bucket
        bool set(BlackboardKeyId id, const T& v, Writer src, uint32_t tick, float ttl, uint8_t flags);

        // Get item from this bucket
        const T* get(BlackboardKeyId id) const;

        // Get raw bucket entry
        Entry<T>* raw(BlackboardKeyId id);

        // Decay the bucket
        void decay(float dt);
    private:
        std::unordered_map<uint16_t, Entry<T>> items;
    };

    using Callback = std::function<void(BlackboardKeyId)>;

    struct Subscription {
        BlackboardKeyId key{};
        Callback cb{};
    };

    // Write op staged for thread-safe application on tick
    struct StagedWrite
    {
        std::type_index Type {typeid(void)};
        BlackboardKeyId TheKeyId {};
        Writer Source {Writer::Unknown};
        float TTL {0.f};
        uint8_t ExtraFlags {0};
        std::function<void(BucketBase&)> apply; // type-erased applier into bucket
    };

    class Blackboard
    {
    public:
        Blackboard();
        ~Blackboard();

        // Set a value
        template<typename T>
        bool Set(BlackboardKey<T>* keyTag, const T& value, Writer src = Writer::Unknown, float ttl=0.f, bool replicate=false);

        // Get a value
        template<typename T>
        const T* Get(BlackboardKey<T>* keyTag) const;

        // Update
        void Update(float dt);

        // Allow callers to subscribe to changes in a key
        int Subscribe(BlackboardKeyId key, Callback cb);
        void Unsubscribe(int subscription);

        // Hierarchical parent lookup
        void SetParent(Blackboard* parent)
        {
            parent_ = parent;
        }

    private:
        uint32_t currentTick{0};
        Blackboard* parent_{nullptr};

        // buckets per supported type
        Bucket<bool> Bools;
        Bucket<int> Ints;
        Bucket<float> Floats;
        Bucket<uint64_t> b_u64_;
        Bucket<double> Doubles;
        // For IDs and vectors in examples we reuse u64 & a simple struct

        // subscription table
        int subscriptionId{1};
        std::unordered_map<int, Subscription> subscriptions;
        std::unordered_multimap<uint16_t, int> key_to_subs_;

        // staged writes queue (thread-safe)
        mutable std::mutex q_m_;
        std::queue<StagedWrite> stagedWriteQueue;

        // helpers
        void notify(BlackboardKeyId k);
        void ApplyStaged();
    };
}
