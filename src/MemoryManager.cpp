#include "MemoryManager.hpp"

MemoryManager::~MemoryManager()
{
	if (m_process)
		CloseHandle(m_process);
}

bool MemoryManager::is_attached_to_process() const
{
	if (!m_process)
		return false;

	DWORD exitCode = 0;
	if (!GetExitCodeProcess(m_process, &exitCode))
		return false;

	return exitCode == STILL_ACTIVE;
}

bool MemoryManager::attach_to_process(const char* window_name)
{
	auto newSize = strlen(window_name) + 1;
	wchar_t* wstr = new wchar_t[newSize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wstr, newSize, window_name, _TRUNCATE);
	bool success = attach_to_process(wstr);
	delete[] wstr;

	return success;
}

bool MemoryManager::attach_to_process(const wchar_t* window_name)
{
	HWND handle = FindWindowW(nullptr, window_name);
	if (!handle)
		return false;
	if (m_process)
		CloseHandle(m_process);

	DWORD process_ID = 0;
	GetWindowThreadProcessId(handle, &process_ID);
	m_process = OpenProcess(PROCESS_ALL_ACCESS, false, process_ID);
	if (!m_process)
		return false;

	return true;
}

uintptr_t MemoryManager::get_module_address(const std::regex& regex) const
{
	HMODULE modules[1024] = {};
	DWORD bytes_needed = 0;

	if (!EnumProcessModules(m_process, modules, sizeof(modules), &bytes_needed))
		return 0;

	const int module_count = bytes_needed / sizeof(HMODULE);
	for (size_t i = 0; i < module_count; i++)
	{
		CHAR name[MAX_PATH] = "";
		GetModuleBaseNameA(m_process, modules[i], name, sizeof(name));
		for (char& c : name)
			c = tolower(c);

		std::smatch match;
		if (!std::regex_search(name, regex))
			continue;

		if (debug_print)
		{
			std::cout << name << " found address: ";
			print_4_byte_hex(reinterpret_cast<uintptr_t>(modules[i]));
			std::cout << std::endl;
		}
		return reinterpret_cast<uintptr_t>(modules[i]);
	}

	return 0;
}

void MemoryManager::print_4_byte_hex(uintptr_t number) const
{
	if constexpr (sizeof(uintptr_t) == 8)
	{
		printf_s("0x%16llx", number);
	}
	else
	{
		printf_s("0x%08llx", number);
	}
}
