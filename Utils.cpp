#include "Utils.h"

bool gamelib::Utils::StrToBool(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}
