#pragma once

#include <Windows.h>
#include <MinHook.h>
#include <iostream>
#include "report.hpp"
#include "definetion.hpp"
#include "util.hpp"

namespace sdk
{
	inline bool initialize_monitor() noexcept
	{
		return true;
	}
}