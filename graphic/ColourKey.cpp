#include "ColourKey.h"

namespace gamelib
{
	ColourKey::ColourKey() : Red(0), Blue(0), Green(0)
	{
		isSet = false;
	}

	bool ColourKey::IsSet() const
	{
		return isSet;
	}

	ColourKey::ColourKey(const int red, const int green, const int blue) : Red(red), Blue(blue), Green(green)
	{
		isSet = true;
	}

	bool operator==(const ColourKey& lhs, const ColourKey& rhs)
	{
		return lhs.Red == rhs.Red && lhs.Green == rhs.Green && lhs.Blue == rhs.Blue;
	}

}
