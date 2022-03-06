#include "asset.h"

using namespace std;

namespace gamelib
{
	Asset::Asset(const int uid, string &name, const string &path, const string &type, const int scene) : uid(uid), name(name), path(path), type(type), scene(scene), isLoadedInMemory(false)
	{
		// constructor initializes member only
	}
	
}



