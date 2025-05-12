#pragma once
#include <cstdint>

namespace gamelib
{
	enum class GameObjectType : uint8_t
	{
		undefined,
		game_defined, // Game Object is custom made and has no special handling in the library
		animated_sprite,
		static_sprite,
		pickup,
		drawable_frame_rate,
		hotspot
	};

	inline const char* ToString(const GameObjectType type)
	{
		switch (type)
		{
		case GameObjectType::undefined: return "undefined";
		case GameObjectType::game_defined: return "game_defined";
		case GameObjectType::animated_sprite: return "animated_sprite";
		case GameObjectType::static_sprite: return "static_sprite";
		case GameObjectType::pickup: return "pickup";
		case GameObjectType::drawable_frame_rate: return "drawable_frame_rate";
		case GameObjectType::hotspot: return "hotspot";
		default: return "unknown";
		}
	}
}
