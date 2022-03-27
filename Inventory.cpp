#include "Inventory.h"

using namespace std;

namespace gamelib
{
	/// <summary>
	/// Add component to game object, eg. health, points etc
	/// </summary>
	void Inventory::AddComponent(const shared_ptr<Component>& component)
	{
		components[component->GetName()] = component;
	}

	/// <summary>
	/// Find a component
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	shared_ptr<Component> Inventory::FindComponent(string name)
	{
		return components.at(name);
	}

	/// <summary>
	/// Check if a component exists
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool Inventory::HasComponent(string name)
	{
		return components.find(name) != components.end();
	}
}
