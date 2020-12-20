#pragma once
namespace gamelib
{
	template <class T>
	class coordinate
	{
	public:
		coordinate(T x, T y);
		T get_x();
		T get_y();
	private: 
		T x, y;
	};

	template <class T>
	coordinate<T>::coordinate(T x, T y): x(x), y(y)
	{
	}

	template <class T>
	T coordinate<T>::get_x()
	{
		return this->x;
	}

	template <class T>
	T coordinate<T>::get_y()
	{
		return this->y;
	}
}