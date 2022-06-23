import <Windows.h>;
import <cstdint>;
import <filesystem>;

bool DllMain(const HMODULE module, const std::uint32_t call_reason, void* reserved [[maybe_unused]] ) noexcept
{
	DisableThreadLibraryCalls(module);

	if (call_reason != DLL_PROCESS_ATTACH)
	{
		return false;
	}

	

	return true;
}

