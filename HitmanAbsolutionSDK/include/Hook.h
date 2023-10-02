#pragma once

#include <cstdint>

#include <MinHook.h>

#include "Global.h"
#include "Utility/MinHookUtility.h"
#include "Logger.h"

template <typename ReturnType, typename... Args>
class CdeclHook
{
public:
	using Function = ReturnType(__cdecl*)(Args...);
	using HookFunction = ReturnType(__cdecl*)(Args...);

	CdeclHook()
	{
		pFunction = nullptr;
		pOriginalFunction = nullptr;
	}

	void CreateHook(const std::string& functionName, const uintptr_t offset, HookFunction hookFunction)
	{
		this->functionName = functionName;

		Function pFunction = reinterpret_cast<Function>(BaseAddress + offset);
		MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(pFunction), reinterpret_cast<LPVOID>(hookFunction), reinterpret_cast<LPVOID*>(&pOriginalFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully created hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to create hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void EnableHook()
	{
		MH_STATUS status = MH_EnableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully enabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to enable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void DisableHook()
	{
		MH_STATUS status = MH_DisableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully disabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to disable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void RemoveHook()
	{
		MH_STATUS status = MH_RemoveHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully removed hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to remove hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	ReturnType CallOriginalFunction(Args... args)
	{
		if (!std::is_void_v<ReturnType>)
		{
			return reinterpret_cast<Function>(pOriginalFunction)(args...);
		}

		reinterpret_cast<Function>(pOriginalFunction)(args...);
	}

private:
	Function pFunction;
	Function pOriginalFunction;
	std::string functionName;
	std::string moduleName;
};

template <typename ReturnType, typename... Args>
class StdCallHook
{
public:
	using Function = ReturnType(__stdcall*)(Args...);
	using HookFunction = ReturnType(__stdcall*)(Args...);

	StdCallHook()
	{
		pFunction = nullptr;
		pOriginalFunction = nullptr;
	}

	void CreateHook(const std::string& functionName, const uintptr_t offset, HookFunction hookFunction)
	{
		this->functionName = functionName;

		Function pFunction = reinterpret_cast<Function>(BaseAddress + offset);
		MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(pFunction), reinterpret_cast<LPVOID>(hookFunction), reinterpret_cast<LPVOID*>(&pOriginalFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully created hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to create hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void CreateWinApiHook(const std::string& functionName, const std::string& moduleName, HookFunction hookFunction)
	{
		this->functionName = functionName;

		std::wstring moduleName2 = std::wstring(moduleName.begin(), moduleName.end());
		MH_STATUS status = MH_CreateHookApi(moduleName2.c_str(), functionName.c_str(), reinterpret_cast<LPVOID>(hookFunction), reinterpret_cast<LPVOID*>(&pOriginalFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully created hook for {} function in {}.", functionName, moduleName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to create hook for {} function in {}! {}", functionName, moduleName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void EnableHook()
	{
		MH_STATUS status = MH_EnableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully enabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to enable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void DisableHook()
	{
		MH_STATUS status = MH_DisableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully disabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to disable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void RemoveHook()
	{
		MH_STATUS status = MH_RemoveHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully removed hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to remove hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	ReturnType CallOriginalFunction(Args... args)
	{
		if (!std::is_void_v<ReturnType>)
		{
			return reinterpret_cast<Function>(pOriginalFunction)(args...);
		}

		reinterpret_cast<Function>(pOriginalFunction)(args...);
	}

private:
	Function pFunction;
	Function pOriginalFunction;
	std::string functionName;
	std::string moduleName;
};

template <typename ReturnType, typename... Args>
class FastCallHook
{
public:
	using Function = ReturnType(__fastcall*)(Args...);
	using HookFunction = ReturnType(__fastcall*)(Args...);

	FastCallHook()
	{
		pFunction = nullptr;
		pOriginalFunction = nullptr;
	}

	void CreateHook(const std::string& functionName, const uintptr_t offset, HookFunction hookFunction)
	{
		this->functionName = functionName;

		Function pFunction = reinterpret_cast<Function>(BaseAddress + offset);
		MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(pFunction), reinterpret_cast<LPVOID>(hookFunction), reinterpret_cast<LPVOID*>(&pOriginalFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully created hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to create hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void EnableHook()
	{
		MH_STATUS status = MH_EnableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully enabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to enable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void DisableHook()
	{
		MH_STATUS status = MH_DisableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully disabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to disable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void RemoveHook()
	{
		MH_STATUS status = MH_RemoveHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully removed hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to remove hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	ReturnType CallOriginalFunction(Args... args)
	{
		if (!std::is_void_v<ReturnType>)
		{
			return reinterpret_cast<Function>(pOriginalFunction)(args...);
		}

		reinterpret_cast<Function>(pOriginalFunction)(args...);
	}

private:
	Function pFunction;
	Function pOriginalFunction;
	std::string functionName;
	std::string moduleName;
};

template <typename ReturnType, typename Class, typename... Args>
class ThisCallHook
{
public:
	using Function = ReturnType(__thiscall*)(Class* pThis, Args...);
	using HookFunction = ReturnType(__fastcall*)(Class* pThis, int edx, Args...);

	ThisCallHook()
	{
		pFunction = nullptr;
		pOriginalFunction = nullptr;
	}

	void CreateHook(const std::string& functionName, const uintptr_t offset, HookFunction hookFunction)
	{
		this->functionName = functionName;

		Function pFunction = reinterpret_cast<Function>(BaseAddress + offset);
		MH_STATUS status = MH_CreateHook(reinterpret_cast<LPVOID>(pFunction), reinterpret_cast<LPVOID>(hookFunction), reinterpret_cast<LPVOID*>(&pOriginalFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully created hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to create hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void EnableHook()
	{
		MH_STATUS status = MH_EnableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully enabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to enable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void DisableHook()
	{
		MH_STATUS status = MH_DisableHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully disabled hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to disable hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	void RemoveHook()
	{
		MH_STATUS status = MH_RemoveHook(reinterpret_cast<LPVOID>(pFunction));

		if (status == MH_OK)
		{
			Logger::GetInstance().Log(Logger::Level::Info, "Successfully removed hook for {} function.", functionName);
		}
		else
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to remove hook for {} function! {}", functionName, MinHookUtility::GetMessageFromStatus(status));
		}
	}

	ReturnType CallOriginalFunction(Class* pThis, Args... args)
	{
		if (!std::is_void_v<ReturnType>)
		{
			return pOriginalFunction(pThis, args...);
		}

		pOriginalFunction(pThis, args...);
	}

private:
	Function pFunction;
	Function pOriginalFunction;
	std::string functionName;
};
