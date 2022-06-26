#pragma once

#include <Windows.h>
#include <jni.h>
#include <jvmti.h>
#include <string>

namespace sdk
{
	extern JavaVM* virtual_machine;
	extern JNIEnv* env;
	extern jvmtiEnv* env_ti;
	extern HMODULE virtual_machine_module;

	inline bool initialize() noexcept
	{
		if (JNI_GetCreatedJavaVMs(&virtual_machine, 1, nullptr) != JNI_OK)
		{
			return false;
		}

		JavaVMAttachArgs args{ .version = JNI_VERSION_1_8, .name = const_cast<char*>(""), .group = nullptr };
		if (virtual_machine->AttachCurrentThread(reinterpret_cast<void**>(&env), &args) != JNI_OK)
		{
			return false;
		}

		if (auto status = virtual_machine->GetEnv(reinterpret_cast<void**>(&env_ti), JVMTI_VERSION_1_1) != JNI_OK)
		{
			return false;
		}

		virtual_machine_module = GetModuleHandleA("jvm.dll");

		return true;
	}

	inline void finalize() noexcept
	{
		if (virtual_machine)
		{
			virtual_machine->DetachCurrentThread();
		}
	}

	inline JNIEnv* get_env() noexcept
	{
		return env;
	}

	inline jvmtiEnv* get_envti() noexcept
	{
		return env_ti;
	}

	inline JavaVM* get_virtual_machine() noexcept
	{
		return virtual_machine;
	}

	inline HMODULE get_vm_module() noexcept
	{
		return virtual_machine_module;
	}

	extern "C" {
		JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved);
		JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options, void* reserved);
		JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm);
	}
}