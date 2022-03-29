#include "SettingDetail.h"
#include <exception>
using namespace std;

namespace gamelib
{
	SettingDetail::SettingDetail(std::string name, std::string value, std::string type) 
		: name(name), value(value), type(type)
	{
	}

	SettingDetail::SettingDetail() 
		: name("empty"), value("empty"), type("empty")
	{
	}

	int SettingDetail::ToInt() const
	{
		if (type == "int")
			return std::atoi(value.c_str());
		throw exception("Setting is not an integer");
	}

	bool SettingDetail::ToBool() const
	{
		if (type == "bool")
			return value == "true" ? true : false;
		throw exception("Setting is not a bool");
	}

	std::string SettingDetail::ToString() const
	{
		if (type == "string")
			return value;
		throw exception("Setting is not a string");
	}

	long SettingDetail::ToLong() const
	{
		if (type == "long")
			return std::strtol(value.c_str(), nullptr, 10);
		throw exception("Setting is not a long");
	}
}