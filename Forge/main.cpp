#include <Windows.h>
#include <cstdint>
#include <exception>
#include <thread>
#include "sdk.hpp"
#include "report.hpp"
#include "monitor.hpp"

bool DllMain(const HMODULE module, const std::uint32_t call_reason, void* reserved [[maybe_unused]] ) noexcept
{
	DisableThreadLibraryCalls(module);

	if (call_reason != DLL_PROCESS_ATTACH)
	{
		return false;
	}

	std::thread([&module]
	{
		try
		{
			sdk::initialize_reporter();

			if (!sdk::initialize())
			{
				sdk::report_error("Failed to initialize sdk");
			}

			if (!sdk::initialize_monitor())
			{
				sdk::report_error("Failed to initialize monitor");
			}
		}
		catch (std::exception const& exception)
		{
			sdk::report_error(exception.what());
		}
	}).detach();

	return true;
}