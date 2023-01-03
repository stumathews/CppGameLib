#pragma once
#include <memory>
#include "objects/Component.h"
#include <string>
#include <map>

namespace gamelib
{
	class Inventory
	{
	public:
		/// <summary>
		/// Add component to the game object (inventory)
		/// </summary>
		/// <param name="component"></param>
		void AddComponent(const std::shared_ptr<Component>& component);

		/// <summary>
		/// Find Component
		/// </summary>
		/// <param name="name">name to find</param>
		std::shared_ptr<Component> FindComponent(const std::string& name);

		/// <summary>
		/// Check if has Component
		/// </summary>
		/// <param name="name">name of component</param>
		bool HasComponent(const std::string& name);

		[[nodiscard]] size_t Count() const;
	private:

		// list of components
		std::map<std::string, std::shared_ptr<Component>> components;
	};
}

