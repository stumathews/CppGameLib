#pragma once

namespace gamelib
{
	class ColourKey
	{
	public:
		ColourKey();
		ColourKey(int red, int green, int blue);
		int Red, Blue, Green;
		[[nodiscard]] bool IsSet() const;
		friend bool operator==(const ColourKey& lhs, const ColourKey& rhs);
	private:
		bool isSet;
		
	};	
}
