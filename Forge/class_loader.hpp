#pragma once
#include <string>
#include "sdk.hpp"

namespace sdk
{
	inline void clear_exception() noexcept
	{
		if (sdk::get_env()->ExceptionCheck())
		{
			sdk::get_env()->ExceptionClear();
		}
	}

	inline bool exception_present() noexcept
	{
		return sdk::get_env()->ExceptionCheck();
	}

	inline bool class_present(std::string const& name) noexcept
	{
		return sdk::get_env()->FindClass(name.data()) != nullptr;
	}

	inline jclass get_class(std::string const& name) noexcept
	{
		return sdk::get_env()->FindClass(name.data());
	}
}