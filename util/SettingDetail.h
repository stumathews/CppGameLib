#pragma once

#include <string>
namespace gamelib
{
	class SettingDetail
	{
	public:
		std::string name, value, type;
		SettingDetail(std::string name, std::string value, std::string type);
		SettingDetail();
		int ToInt() const;
		bool ToBool() const;
		std::string ToString() const;
		long ToLong() const;
	};
}
