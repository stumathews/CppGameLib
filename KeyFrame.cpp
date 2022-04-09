#include "KeyFrame.h"
using namespace std;

namespace gamelib
{
	KeyFrame::KeyFrame(int x, int y, int w, int h, string group) : x(x), y(y), w(w), h(h), group(group) {}
	bool KeyFrame::HasGroup()
	{
		return group.length();
	}

}
