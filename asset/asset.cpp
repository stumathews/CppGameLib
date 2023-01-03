#include "asset.h"

using namespace std;

namespace gamelib
{
	Asset::Asset(const int uid, string name, string path, string type, const int scene)
		: uid(uid), name(std::move(name)), path(std::move(path)), type(std::move(type)),
		  assetType(AssetType::Undefined), scene(scene), isLoadedInMemory(false)
	{
		// constructor initializes member only
	}
	
}



