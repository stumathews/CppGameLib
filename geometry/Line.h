#pragma once

namespace gamelib
{
	class Line
	{
	public:
		Line()
		{
			this->X1 = 0;
			this->Y1 = 0;
			this->X2 = 0;
			this->Y2 = 0;
		}
		Line(const int x1, const int y1, const int x2, const int y2)
		{
			this->X1 = x1;
			this->Y1 = y1;
			this->X2 = x2;
			this->Y2 = y2;
		}

		int X1;
		int Y1; 
		int X2;
		int Y2;
	};
}

