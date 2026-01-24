
#include "BlackboardKeyRegistry.h"

namespace gamelib {
    BlackboardKeyRegistry& BlackboardKeyRegistry::Instance() {
        static BlackboardKeyRegistry inst; return inst;
    }
}
