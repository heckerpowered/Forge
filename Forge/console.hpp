#pragma once

#include <string>
#include <Windows.h>

namespace sdk
{
	inline bool initialize_console(std::string const& title)
	{
		AllocConsole();

		freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(0)), "conin$", "r", __acrt_iob_func(0));
		freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(1)), "conout$", "w", __acrt_iob_func(1));
		freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(2)), "conout$", "w", __acrt_iob_func(2));

		SetConsoleTitleA(title.data());

		return true;
	}

	inline bool release_console()
	{
		fclose(__acrt_iob_func(0));
		fclose(__acrt_iob_func(1));
		fclose(__acrt_iob_func(2));

		FreeConsole();

		return true;
	}
}