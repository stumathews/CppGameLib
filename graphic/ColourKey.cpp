#pragma once
#include "ColourKey.h"

namespace gamelib
{
	ColourKey::ColourKey() : Red(0), Green(0), Blue(0) 
	{
		isSet = false;
	}

	bool ColourKey::IsSet()
	{
		return isSet;
	}

	ColourKey::ColourKey(int red, int green, int blue) : Red(red), Green(green), Blue(blue) 
	{
		isSet = true;
	}
	bool operator==(const ColourKey& lhs, const ColourKey& rhs)
	{
		return lhs.Red == rhs.Red && lhs.Green == rhs.Green && lhs.Blue == rhs.Blue;
	}

}
