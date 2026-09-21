#include "MemoryManager.h"
//ЧАТ ГПТ ДО основы
#include <iostream>


MemoryManager::~MemoryManager()
{
    Cleanup();
}


DWORD MemoryManager::GetProcessIdByName(const wchar_t* processName) const
{
    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0
    );

    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;


    PROCESSENTRY32W processEntry{};
    processEntry.dwSize = sizeof(PROCESSENTRY32W);


    if (!Process32FirstW(snapshot, &processEntry))
    {
        CloseHandle(snapshot);
        return 0;
    }


    do
    {
        if (_wcsicmp(
            processEntry.szExeFile,
            processName
        ) == 0)
        {
            DWORD pid = processEntry.th32ProcessID;

            CloseHandle(snapshot);

            return pid;
        }

    } while (Process32NextW(snapshot, &processEntry));


    CloseHandle(snapshot);

    return 0;
}


std::uintptr_t MemoryManager::GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName) const
{
    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
        pid
    );

    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;


    MODULEENTRY32W moduleEntry{};
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);


    if (!Module32FirstW(snapshot, &moduleEntry))
    {
        CloseHandle(snapshot);
        return 0;
    }


    do
    {
        if (_wcsicmp(
            moduleEntry.szModule,
            moduleName
        ) == 0)
        {
            std::uintptr_t result =
                reinterpret_cast<std::uintptr_t>(
                    moduleEntry.modBaseAddr
                    );

            CloseHandle(snapshot);

            return result;
        }

    } while (Module32NextW(snapshot, &moduleEntry));


    CloseHandle(snapshot);

    return 0;
}


bool MemoryManager::Initialize(const wchar_t* processName, const wchar_t* moduleName)
{
    Cleanup();


    // 1. Найти PID
    processId = GetProcessIdByName(processName);

    if (processId == 0)
    {
        std::cerr
            << "[MemoryManager] Process not found\n";

        return false;
    }


    // 2. Открыть процесс только для чтения
    processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);

    if (
        processHandle == nullptr ||
        processHandle == INVALID_HANDLE_VALUE
        )
    {
        std::cerr
            << "[MemoryManager] OpenProcess failed\n";

        processHandle = nullptr;

        return false;
    }


    // 3. Получить base address модуля
    baseAddress = GetModuleBaseAddress(
        processId,
        moduleName
    );

    if (baseAddress == 0)
    {
        std::cerr
            << "[MemoryManager] Module not found\n";

        Cleanup();

        return false;
    }


    initialized = true;


    std::cout
        << "[MemoryManager] Initialized\n";

    std::cout
        << "[MemoryManager] PID: "
        << processId
        << '\n';

    std::cout
        << "[MemoryManager] Base: 0x"
        << std::hex
        << baseAddress
        << std::dec
        << '\n';


    return true;
}


void MemoryManager::Cleanup()
{
    if (
        processHandle != nullptr &&
        processHandle != INVALID_HANDLE_VALUE
        )
    {
        CloseHandle(processHandle);
    }


    processHandle = nullptr;

    processId = 0;

    baseAddress = 0;

    initialized = false;
}


bool MemoryManager::IsValid() const
{
    return
        initialized &&
        processHandle != nullptr &&
        processHandle != INVALID_HANDLE_VALUE &&
        baseAddress != 0;
}


DWORD MemoryManager::GetProcessId() const
{
    return processId;
}


HANDLE MemoryManager::GetProcessHandle() const
{
    return processHandle;
}


std::uintptr_t MemoryManager::GetBaseAddress() const
{
    return baseAddress;
}


bool MemoryManager::ReadBuffer(std::uintptr_t address, void* buffer, std::size_t size) const
{
    if (
        !IsValid() ||
        address == 0 ||
        buffer == nullptr ||
        size == 0
        )
    {
        return false;
    }


    SIZE_T bytesRead = 0;

    BOOL result = ReadProcessMemory(
        processHandle,
        reinterpret_cast<LPCVOID>(address),
        buffer,
        size,
        &bytesRead
    );


    return
        result != FALSE &&
        bytesRead == size;
}

/* Основа
MemoryManager::~MemoryManager()
{
	Cleanup();
}

bool MemoryManager::Initialize()
{
	return initialized;
}

void MemoryManager::Cleanup()
{	
	
		//Если Initialize() создал какие-либо ресурсы,
		//здесь они освобождаются.
	

	initialized = false;
	baseAddress = 0;
}

bool MemoryManager::IsValid() const
{
	return initialized && baseAddress != 0;
}

std::uintptr_t MemoryManager::GetBaseAddress() const
{
	return baseAddress;
}
*/
