#include "KeyFrame.h"
using namespace std;

namespace gamelib
{
	KeyFrame::KeyFrame(const int x, const int y, const int w, const int h, string group) : X(x), Y(y), W(w), H(h), Group(
		std::move(group)) {}
	bool KeyFrame::HasGroup() const
	{
		return Group.length();
	}

}
