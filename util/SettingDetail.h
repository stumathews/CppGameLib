#pragma once

#include <string>
namespace gamelib
{
	class SettingDetail
	{
	public:
		std::string Name, Value, Type;
		SettingDetail(std::string name, std::string value, std::string type);
		SettingDetail();
		[[nodiscard]] int ToInt() const;
		[[nodiscard]] bool ToBool() const;
		[[nodiscard]] std::string ToString() const;
		[[nodiscard]] long ToLong() const;
	};
}
