#pragma once
namespace gamelib
{
	template <class T>
	class coordinate
	{
	public:
		coordinate(T x, T y) : x(x), y(y)
	{
	}
	
	T GetX() const
	{
		return this->x;
	}

	T GetY() const
	{
		return this->y;
	}
	private: 
		T x, y;
	};
	
	
}