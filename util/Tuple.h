#pragma once
namespace gamelib
{
	template <class T>
	class Coordinate
	{
	public:
		Coordinate(T x, T y);
		Coordinate();
		T GetX() const;
		T GetY() const;

		void SetX(T value);
		void SetY(T value);

	private: 
		T x, y;
	};

	template <typename T> 
	Coordinate<T>::Coordinate(T x, T y)  : x(x), y(y) { }

	template <typename T>
	Coordinate<T>::Coordinate() { x = y = 0; }

	template <typename T> 
	T Coordinate<T>::GetY() const
	{
		return this->y;
	}

	template <typename T> 
	T Coordinate<T>::GetX() const
	{
		return this->x;
	}

	template <typename T> 
	void Coordinate<T>::SetX(T value)
	{
		this->x = value;
	}

	template <typename T> 
	void Coordinate<T>::SetY(T value)
	{
		this->y = value;
	}
	
}