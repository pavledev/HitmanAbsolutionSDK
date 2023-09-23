#pragma once

namespace Function
{
	template <typename... Args>
	void Call(unsigned int address, Args... args)
	{
		reinterpret_cast<void(__cdecl*)(Args...)>(address)(args...);
	}

	template <typename... Args>
	void StdCall(unsigned int address, Args... args)
	{
		reinterpret_cast<void(__stdcall*)(Args...)>(address)(args...);
	}

	template <typename C, typename... Args>
	void FastCall(unsigned int address, C _this, Args... args)
	{
		static int padding = 0;

		reinterpret_cast<void(__fastcall*)(C, int, Args...)>(address)(_this, padding, args...);
	}

	template <typename... Args>
	void VectorCall(unsigned int address, Args... args)
	{
		reinterpret_cast<void(__vectorcall*)(Args...)>(address)(args...);
	}

	template <typename C, typename... Args>
	void CallMethod(unsigned int address, C _this, Args... args)
	{
		reinterpret_cast<void(__thiscall*)(C, Args...)>(address)(_this, args...);
	}

	template <unsigned int tableIndex, typename C, typename... Args>
	void CallVirtualMethod(C _this, Args... args)
	{
		reinterpret_cast<void(__thiscall*)(C, Args...)>((*reinterpret_cast<void***>(_this))[tableIndex])(_this, args...);
	}

	template <unsigned int tableIndex, typename C, typename... Args>
	void CallConstVirtualMethod(C _this, Args... args)
	{
		reinterpret_cast<void(__thiscall*)(C, Args...)>((*(void***)(_this))[tableIndex])(_this, args...);
	}

	template <typename Ret, typename... Args>
	Ret CallAndReturn(unsigned int address, Args... args)
	{
		return reinterpret_cast<Ret(__cdecl*)(Args...)>(address)(args...);
	}

	template <typename Ret, typename... Args>
	Ret StdCallAndReturn(unsigned int address, Args... args)
	{
		return reinterpret_cast<Ret(__stdcall*)(Args...)>(address)(args...);
	}

	template <typename Ret, typename C, typename... Args>
	Ret FastCallAndReturn(unsigned int address, C _this, Args... args)
	{
		static int padding = 0;

		return reinterpret_cast<Ret(__fastcall*)(C, int, Args...)>(address)(_this, padding, args...);
	}

	template <typename Ret, typename... Args>
	Ret VectorCallAndReturn(unsigned int address, Args... args)
	{
		return reinterpret_cast<Ret(__vectorcall*)(Args...)>(address)(args...);
	}

	template <typename Ret, typename C, typename... Args>
	Ret CallMethodAndReturn(unsigned int address, C _this, Args... args)
	{
		return reinterpret_cast<Ret(__thiscall*)(C, Args...)>(address)(_this, args...);
	}

	template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
	Ret CallVirtualMethodAndReturn(C _this, Args... args)
	{
		return reinterpret_cast<Ret(__thiscall*)(C, Args...)>((*reinterpret_cast<void***>(_this))[tableIndex])(_this, args...);
	}

	template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
	Ret CallConstVirtualMethodAndReturn(C _this, Args... args)
	{
		return reinterpret_cast<Ret(__thiscall*)(C, Args...)>((*(void***)(_this))[tableIndex])(_this, args...);
	}

	template <typename Ret, typename... Args>
	Ret CallRVOAndReturn(unsigned int address, Args... args)
	{
		Ret result = {};

		reinterpret_cast<Ret* (__cdecl*)(Ret*, Args...)>(address)(&result, args...);

		return result;
	}

	template <typename Ret, typename... Args>
	Ret StdCallRVOAndReturn(unsigned int address, Args... args)
	{
		Ret result = {};

		reinterpret_cast<Ret* (__stdcall*)(Ret*, Args...)>(address)(&result, args...);

		return result;
	}

	template <typename Ret, typename C, typename... Args>
	Ret FastCallRVOAndReturn(unsigned int address, C _this, Args... args)
	{
		static int padding = 0;
		Ret result = {};

		reinterpret_cast<Ret* (__fastcall*)(C, int, Ret*, Args...)>(address)(_this, padding, &result, args...);

		return result;
	}

	template <typename Ret, typename... Args>
	Ret VectorCallRVOAndReturn(unsigned int address, Args... args)
	{
		Ret result = {};

		reinterpret_cast<Ret* (__vectorcall*)(Ret*, Args...)>(address)(&result, args...);

		return result;
	}

	template <typename Ret, typename C, typename... Args>
	Ret CallRVOMethodAndReturn(unsigned int address, C _this, Args... args)
	{
		Ret result = {};

		reinterpret_cast<Ret* (__thiscall*)(C, Ret*, Args...)>(address)(_this, &result, args...);

		return result;
	}

	template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
	Ret CallRVOVirtualMethodAndReturn(C _this, Args... args)
	{
		Ret result = {};

		reinterpret_cast<Ret* (__thiscall*)(C, Ret*, Args...)>((*reinterpret_cast<void***>(_this))[tableIndex])(_this, &result, args...);

		return result;
	}

	template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
	Ret CallRVOConstVirtualMethodAndReturn(C _this, Args... args)
	{
		Ret result = {};

		reinterpret_cast<Ret* (__thiscall*)(C, Ret*, Args...)>((*(void***)(_this))[tableIndex])(_this, &result, args...);

		return result;
	}
}
