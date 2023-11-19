#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace gamelib
{		
	class Utils
	{
	public:
		template <typename T>
		static bool IsUninitialized(std::weak_ptr<T> const& weak)
		{
			using wt = std::weak_ptr<T>;
			return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
		}

		template <typename T>
		bool VectorContains(std::vector<T> v, std::function<bool(T)> predicate)
		{
			return std::any_of(begin(v), end(v), predicate);
		}
			
		
	};

	// Shortens call to dynamic_pointer_cast<T>
	template <typename T, typename F>
	std::shared_ptr<T> To(std::shared_ptr<F> from)
	{
		return std::dynamic_pointer_cast<T>(from);
	}
}
