#pragma once
namespace gamelib
{
	enum class Status
	{
		Invalid, // default status
		Success,
		Failure,
		Running,
		Aborted
	};
}