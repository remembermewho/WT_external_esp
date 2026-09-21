#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <string>

//ЧАТ ГПТ до основы
class MemoryManager
{
private:
    DWORD processId = 0;
    HANDLE processHandle = nullptr;

    std::uintptr_t baseAddress = 0;

    bool initialized = false;

private:
    DWORD GetProcessIdByName(const wchar_t* processName) const;

    std::uintptr_t GetModuleBaseAddress(
        DWORD processId,
        const wchar_t* moduleName
    ) const;

public:
    MemoryManager() = default;

    ~MemoryManager();

    bool Initialize(
        const wchar_t* processName,
        const wchar_t* moduleName
    );

    void Cleanup();

    bool IsValid() const;

    DWORD GetProcessId() const;

    HANDLE GetProcessHandle() const;

    std::uintptr_t GetBaseAddress() const;

    template<typename T>
    bool Read(std::uintptr_t address, T& value) const
    {
        if (!IsValid() || address == 0)
            return false;

        SIZE_T bytesRead = 0;

        BOOL result = ReadProcessMemory(
            processHandle,
            reinterpret_cast<LPCVOID>(address),
            &value,
            sizeof(T),
            &bytesRead
        );

        return result != FALSE && bytesRead == sizeof(T);
    }

    bool ReadBuffer(
        std::uintptr_t address,
        void* buffer,
        std::size_t size
    ) const;
};

/*
* -Основа
class MemoryManager
{
private:
	bool initialized = false;

	std::uintptr_t baseAddress = 0;

public:
	MemoryManager() = default;
	~MemoryManager();

	bool Initialize();
	
	void Cleanup();

	bool IsValid() const;

	std::uintptr_t GetBaseAddress() const;

	template<typename T>
	bool Read(std::uintptr_t address, T& value) const
	{
		// Тут должна быть реализация
		if (!initialized || address == 0) {
			return false;
		}


	}
};
*/