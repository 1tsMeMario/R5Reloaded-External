#include "Memory.h"
VOID startup(LPCTSTR lpApplicationName)
{
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcess(lpApplicationName,   // the path
        NULL,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

bool Memory::Init()
{
    TargetHwnd = FindWindowA(NULL, TargetName);

    if (!TargetHwnd)
    {
        printf("[-] Waiting Apex...\n");
        Sleep(2000);
        startup("r5apex.exe");
        while (!TargetHwnd)
        {
            TargetHwnd = FindWindowA(NULL, TargetName);

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }

    GetWindowThreadProcessId(TargetHwnd, &PID);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (!pHandle)
    {
        printf("[-] Can't get pHandle!\n");

        return false;
    }

    BaseAddress = GetModuleBase("r5apex.exe");

    if (!BaseAddress)
    {
        printf("[-] Can't ModuleBaseAddress!\n");

        return false;
    }

    printf("[+] Successful initialization!\n");
    return true;
}

uintptr_t Memory::GetModuleBase(const std::string moduleName)
{
    MODULEENTRY32 entry = { };
    entry.dwSize = sizeof(MODULEENTRY32);

    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

    std::uintptr_t result = 0;

    while (Module32Next(snapShot, &entry))
    {
        if (!moduleName.compare(entry.szModule))
        {
            result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
            break;
        }
    }

    if (snapShot)
        CloseHandle(snapShot);

    return result;
}