#include "SettingDetail.h"
#include <exception>
#include <exceptions/EngineException.h>
#include <string>
using namespace std;

namespace gamelib
{
	SettingDetail::SettingDetail(std::string name, std::string value, std::string type) 
		: Name(std::move(name)), Value(std::move(value)), Type(std::move(type))
	{
	}

	SettingDetail::SettingDetail() 
		: Name("empty"), Value("empty"), Type("empty")
	{
	}

	int SettingDetail::ToInt() const
	{
		if (Type == "int")
			return std::strtol(Value.c_str(), nullptr, 0);
		THROW(99, "Setting is not an integer", "Settings");
	}

	bool SettingDetail::ToBool() const
	{
		if (Type == "bool")
			return Value == "true" ? true : false;
		THROW(99, "Setting is not a bool","Settings");
	}

	std::string SettingDetail::ToString() const
	{
		if (Type == "string")
			return Value;
		THROW(99, "Setting is not a string", "Settings");
	}

	long SettingDetail::ToLong() const
	{
		if (Type == "long")
			return std::strtol(Value.c_str(), nullptr, 10);
		THROW(99, "Setting is not a long", "Settings");
	}
}
