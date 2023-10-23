#include "SettingDetail.h"
#include <exception>
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
		throw exception("Setting is not an integer");
	}

	bool SettingDetail::ToBool() const
	{
		if (Type == "bool")
			return Value == "true" ? true : false;
		throw exception("Setting is not a bool");
	}

	std::string SettingDetail::ToString() const
	{
		if (Type == "string")
			return Value;
		throw exception("Setting is not a string");
	}

	long SettingDetail::ToLong() const
	{
		if (Type == "long")
			return std::strtol(Value.c_str(), nullptr, 10);
		throw exception("Setting is not a long");
	}
}