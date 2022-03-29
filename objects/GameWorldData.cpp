#include "GameWorldData.h"

namespace gamelib
{
    std::vector<std::shared_ptr<gamelib::GameObject>>& GameWorldData::GetGameObjects()
    {
        return objects;
    }
}
