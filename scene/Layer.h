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
		bool visible;
		unsigned int zorder;

		coordinate<int> Position;
		bool IsVisible() { return visible; }
		bool ZOrder() { return zorder; }
		void SetName(std::string inName) { name = inName; }
		std::string Name() { return name; }
		std::list<std::weak_ptr<GameObject>> Objects;
	private:
		std::string name;
		
	};
}


