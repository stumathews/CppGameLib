#include "asset.h"

using namespace std;

namespace gamelib
{
	asset::asset(const int uid, string &name, const string &path, const string &type, const int scene) : uid(uid), name(name), path(path), type(type), scene(scene), is_loaded(false)
	{
		// constructor initializes member only
	}
	
}



