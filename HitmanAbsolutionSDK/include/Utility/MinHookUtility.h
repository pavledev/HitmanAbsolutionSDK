#pragma once

#include <string>

#include <MinHook.h>

class MinHookUtility
{
public:
	static std::string GetMessageFromStatus(MH_STATUS status)
	{
		std::string message;

		switch (status)
		{
		case MH_UNKNOWN:
			message = "Unknown error. Should not be returned.";

			break;
		case MH_OK:
			message = "Successful.";

			break;
		case MH_ERROR_ALREADY_INITIALIZED:
			message = "MinHook is already initialized.";

			break;
		case MH_ERROR_NOT_INITIALIZED:
			message = "MinHook is not initialized yet, or already uninitialized.";

			break;
		case MH_ERROR_ALREADY_CREATED:
			message = "The hook for the specified target function is already created.";

			break;
		case MH_ERROR_NOT_CREATED:
			message = "The hook for the specified target function is not created yet.";

			break;
		case MH_ERROR_ENABLED:
			message = "The hook for the specified target function is already enabled.";

			break;
		case MH_ERROR_DISABLED:
			message = "The hook for the specified target function is not enabled yet, or already disabled.";

			break;
		case MH_ERROR_NOT_EXECUTABLE:
			message = "The specified pointer is invalid. It points the address of non-allocated and/or non-executable region.";

			break;
		case MH_ERROR_UNSUPPORTED_FUNCTION:
			message = "The specified target function cannot be hooked.";

			break;
		case MH_ERROR_MEMORY_ALLOC:
			message = "Failed to allocate memory.";

			break;
		case MH_ERROR_MEMORY_PROTECT:
			message = "Failed to change the memory protection.";

			break;
		case MH_ERROR_MODULE_NOT_FOUND:
			message = "The specified module is not loaded.";

			break;
		case MH_ERROR_FUNCTION_NOT_FOUND:
			message = "The specified function is not found.";

			break;
		default:
			break;
		}

		return message;
	}

	template <typename T>
	inline static MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
	{
		return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
	}

	template <typename T>
	inline static MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
	{
		return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
	}
};
