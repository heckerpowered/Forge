#pragma once

#include <Windows.h>
#include <jni.h>
#include <jvmti.h>
#include <optional>
#include <string>
#include <source_location>
#include <mixins.hpp>
#include <future>
#include "sdk.hpp"

namespace sdk
{
	inline void report_error(std::string const& message, std::optional<std::string const> const& title, std::source_location const& source = std::source_location::current()) noexcept
	{
		if (MessageBoxA(GetActiveWindow(), mixins::concat(message, "\n\nFile: ", source.file_name(), "\nFunction: ", source.function_name(),
			"\nLine: ", source.line(),
			"\nColumn: ", source.column(), "\n\nClick yes to terminate.").data(), title ? title->data() : nullptr, MB_ICONERROR | MB_YESNO) == IDYES)
		{
			std::terminate();
		}
	}

	inline void report_error(std::string const& message, std::source_location const& source = std::source_location::current()) noexcept
	{
		report_error(message, std::nullopt, source);
	}

	inline std::string get_jni_error_string(jint const code) noexcept
	{
		switch (code)
		{
			case JNI_OK:
				return "Success";
			case JNI_ERR:
				return "Error";
			case JNI_EDETACHED:
				return "Thread detached from the VM";
			case JNI_EVERSION:
				return "JNI version error";
			case JNI_ENOMEM:
				return "Not enough memory";
			case JNI_EEXIST:
				return "VM already created";
			case JNI_EINVAL:
				return "Invalid arguments";
			default:
				return "Unknow";
		}
	}

	inline std::string get_jvmti_error_string(jvmtiError const code) noexcept
	{
		char* name{};
		sdk::get_envti()->GetErrorName(code, &name);
		return name;
	}

	inline void report_jni_error(jint const code, std::source_location const& source = std::source_location::current()) noexcept
	{
		if (code == JNI_OK)
		{
			return;
		}

		report_error(get_jni_error_string(code), source);
	}

	inline void report_jvmti_error(jvmtiError const code, std::source_location const& source = std::source_location::current()) noexcept
	{
		if (code == JVMTI_ERROR_NONE)
		{
			return;
		}

		report_error(get_jvmti_error_string(code), source);
	}
}