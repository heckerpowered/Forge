#include <Windows.h>
#include <cstdint>
#include <exception>
#include <thread>
#include "sdk.hpp"
#include "report.hpp"
#include "monitor.hpp"

bool DllMain(HMODULE const module, std::uint32_t const  call_reason, void const* reserved [[maybe_unused]] ) noexcept
{
	DisableThreadLibraryCalls(module);

	if (call_reason != DLL_PROCESS_ATTACH)
	{
		return false;
	}

	try
	{
		if (!sdk::initialize())
		{
			sdk::report_error("Failed to initialize sdk");
			return false;
		}

		if (!sdk::initialize_monitor())
		{
			sdk::report_error("Failed to initialize monitor");
			return false;
		}
	}
	catch (std::exception const& exception)
	{
		sdk::report_error(exception.what());
		return false;
	}

	return true;
}