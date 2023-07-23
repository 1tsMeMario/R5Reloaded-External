#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string_view>
#include <thread>

class Memory
{
private:
	char TargetName[64] = "Apex Legends";
	HWND TargetHwnd;
public:
	DWORD PID;
	uint64_t BaseAddress;
	HANDLE pHandle;

	bool Init();
	uintptr_t GetModuleBase(const std::string moduleName);

	template <typename T>
	constexpr const T Read(const uintptr_t& address) const noexcept
	{
		T value = { };
		ReadProcessMemory(pHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	constexpr void Write(const uintptr_t& address, const T& value) const noexcept
	{
		WriteProcessMemory(pHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}
};