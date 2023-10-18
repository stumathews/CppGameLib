#include "asset.h"

using namespace std;

namespace gamelib
{
	Asset::Asset(const int uid, string name, string path, string type, const int scene)
		: Uid(uid), Name(std::move(name)), FilePath(std::move(path)), type(std::move(type)),
		  AssetType(AssetType::Undefined), SceneId(scene), IsLoadedInMemory(false)
	{
		// constructor initializes member only
	}
}
