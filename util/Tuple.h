#pragma once
namespace gamelib
{
	template <class T>
	class coordinate
	{
	public:
		coordinate(T x, T y);
		T GetX() const;
		T GetY() const;
	private: 
		T x, y;
	};

	template <class T>
	coordinate<T>::coordinate(T x, T y): x(x), y(y)
	{
	}

	template <class T>
	T coordinate<T>::GetX() const
	{
		return this->x;
	}

	template <class T>
	T coordinate<T>::GetY() const
	{
		return this->y;
	}
	
}