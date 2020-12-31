#pragma once
namespace gamelib
{
	template <class T>
	class coordinate
	{
	public:
		coordinate(T x, T y);
		T get_x() const;
		T get_y() const;
	private: 
		T x, y;
	};

	template <class T>
	coordinate<T>::coordinate(T x, T y): x(x), y(y)
	{
	}

	template <class T>
	T coordinate<T>::get_x() const
	{
		return this->x;
	}

	template <class T>
	T coordinate<T>::get_y() const
	{
		return this->y;
	}
}