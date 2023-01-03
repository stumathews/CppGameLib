#pragma once
#include <string>
#include <list>
#include <memory>
#include <util/Tuple.h>

namespace gamelib
{
	class GameObject;

	/// <summary>
	/// Layer of game objects
	/// </summary>
	class Layer
	{
	public:
		Layer();
		bool Visible;
		unsigned int Zorder;

		Coordinate<int> Position;
		[[nodiscard]] bool IsVisible() const { return Visible; }
		[[nodiscard]] bool ZOrder() const { return Zorder; }
		void SetName(const std::string& inName) { name = inName; }
		std::string Name() { return name; }
		std::list<std::weak_ptr<GameObject>> Objects;
	private:
		std::string name;
		
	};
}


