#pragma once
namespace gamelib
{
	template <class T>
	class coordinate
	{
	public:
		coordinate(T x, T y);
		coordinate();
		T GetX() const;
		T GetY() const;

		void SetX(T value);
		void SetY(T value);

	private: 
		T x, y;
	};

	template <typename T> 
	coordinate<T>::coordinate(T x, T y)  : x(x), y(y) { }

	template <typename T>
	coordinate<T>::coordinate() { x = y = 0; }

	template <typename T> 
	T coordinate<T>::GetY() const
	{
		return this->y;
	}

	template <typename T> 
	T coordinate<T>::GetX() const
	{
		return this->x;
	}

	template <typename T> 
	void coordinate<T>::SetX(T value)
	{
		this->x = value;
	}

	template <typename T> 
	void coordinate<T>::SetY(T value)
	{
		this->y = value;
	}
	
}