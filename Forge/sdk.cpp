#include "sdk.hpp"

namespace sdk
{
	JavaVM* virtual_machine{};
	JNIEnv* env{};
	jvmtiEnv* env_ti{};
	HMODULE virtual_machine_module{};

	extern "C" {
		JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved)
		{
			return JNI_OK;
		}

		JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options, void* reserved)
		{
			return JNI_OK;
		}

		JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm)
		{
		}
	}
}