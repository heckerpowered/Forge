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
	inline void report_error(std::string const& message, std::optional<std::string> title, std::source_location const& source = std::source_location::current()) noexcept
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

	inline void initialize_reporter() noexcept
	{
		if (AddVectoredExceptionHandler(true, [](struct _EXCEPTION_POINTERS* info) noexcept [[msvc::forceinline]] -> long
		{
			static_cast<void>(std::async([&]
			{
				sdk::report_error(mixins::concat(
					mixins::last_error(info->ExceptionRecord->ExceptionCode), "\n",
					"\nDr0: 0x", info->ContextRecord->Dr0,
					"\nDr1: 0x", info->ContextRecord->Dr1,
					"\nDr2: 0x", info->ContextRecord->Dr2,
					"\nDr3: 0x", info->ContextRecord->Dr3,
					"\nDr6: 0x", info->ContextRecord->Dr6,
					"\nDr7: 0x", info->ContextRecord->Dr7,
					"\nEFlags: 0x", info->ContextRecord->EFlags,
					"\nRax: 0x", info->ContextRecord->Rax,
					"\nRcx: 0x", info->ContextRecord->Rcx,
					"\nRdx: 0x", info->ContextRecord->Rdx,
					"\nRbx: 0x", info->ContextRecord->Rbx,
					"\nRsp: 0x", info->ContextRecord->Rsp,
					"\nRbp: 0x", info->ContextRecord->Rbp,
					"\nRsi: 0x", info->ContextRecord->Rsi,
					"\nRdi: 0x", info->ContextRecord->Rdi,
					"\nR8: 0x", info->ContextRecord->R8,
					"\nR9: 0x", info->ContextRecord->R9,
					"\nR10: 0x", info->ContextRecord->R10,
					"\nR11: 0x", info->ContextRecord->R11,
					"\nR12: 0x", info->ContextRecord->R12,
					"\nR13: 0x", info->ContextRecord->R13,
					"\nR14: 0x", info->ContextRecord->R14,
					"\nR15: 0x", info->ContextRecord->R15,
					"\nRip: 0x", info->ContextRecord->Rip,
					"\nException Address: ", reinterpret_cast<std::uint64_t>(info->ExceptionRecord->ExceptionAddress),
					"\nException Code: ", info->ExceptionRecord->ExceptionCode,
					"\nException Flags: ", info->ExceptionRecord->ExceptionFlags,
					"\nNumber Parameters: ", info->ExceptionRecord->NumberParameters,
					"\nException Information: ", info->ExceptionRecord->ExceptionInformation[0],
					", ", info->ExceptionRecord->ExceptionInformation[1],
					", ", info->ExceptionRecord->ExceptionInformation[2],
					", ", info->ExceptionRecord->ExceptionInformation[3],
					", ", info->ExceptionRecord->ExceptionInformation[4],
					", ", info->ExceptionRecord->ExceptionInformation[5],
					", ", info->ExceptionRecord->ExceptionInformation[6],
					", ", info->ExceptionRecord->ExceptionInformation[7],
					", ", info->ExceptionRecord->ExceptionInformation[8],
					", ", info->ExceptionRecord->ExceptionInformation[9],
					", ", info->ExceptionRecord->ExceptionInformation[10],
					", ", info->ExceptionRecord->ExceptionInformation[11],
					", ", info->ExceptionRecord->ExceptionInformation[12],
					", ", info->ExceptionRecord->ExceptionInformation[13],
					", ", info->ExceptionRecord->ExceptionInformation[14]), std::nullopt);
			}));

			return EXCEPTION_CONTINUE_EXECUTION;
		}) == nullptr)
		{
			sdk::report_error(mixins::last_error(), "Failed to initialize reporter.");
		}
	}
}