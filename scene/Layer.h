#pragma once
#include <string>
#include <list>
#include <memory>

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
		int x;
		int y;

		/// <summary>
		/// Name of layer
		/// </summary>
		std::string name;

		/// <summary>
		/// Game objects in this layer
		/// </summary>
		std::list<std::weak_ptr<GameObject>> layerObjects;
	};
}


