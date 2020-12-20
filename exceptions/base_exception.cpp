#include "base_exception.h"
namespace gamelib
{
	base_exception::base_exception(const std::string message, const std::string subsystem) : exception(message.c_str()), subsystem(subsystem)
	{


	}
}
