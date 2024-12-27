#pragma once
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <psapi.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <string>
#include <iomanip>
#include <regex>

class MemoryManager
{
public:
	MemoryManager() = default;
	~MemoryManager();
	bool is_attached_to_process() const;
	bool attach_to_process(const char* window_name);
	bool attach_to_process(const wchar_t* window_name);
	uintptr_t get_module_address(const std::regex& regex) const;
	void print_4_byte_hex(uintptr_t address) const;

	void read_string_from_memory(uintptr_t address, char* buffer, size_t size, bool* success = nullptr) const
	{
		if (!ReadProcessMemory(m_process, reinterpret_cast<LPVOID>(address), buffer, size, nullptr))
		{
			if (success)
				*success = false;
		}
		else if (success) 
		{
			*success = false;
		}
	}

	template <typename type>
	[[nodiscard]] type read_memory(uintptr_t address, bool* success = nullptr) const
	{
		type result{};
		if (!ReadProcessMemory(m_process, reinterpret_cast<LPVOID>(address), &result, sizeof(type), nullptr))
		{
			if (success != nullptr)
				*success = false;
		}
		else if (success) 
		{
			*success = true;
		}

		return result;
	}

	template <typename type>
	bool write_memory(uintptr_t address, const type& data)
	{
		if (!WriteProcessMemory(m_process, reinterpret_cast<LPVOID>(address), &data, sizeof(data), NULL) && debug_print)
			return false;
		return true;
	}

private:
	HANDLE m_process = nullptr;
	static constexpr bool debug_print = false;
};