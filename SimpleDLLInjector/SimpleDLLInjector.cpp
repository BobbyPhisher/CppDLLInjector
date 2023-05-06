#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "InjectorFuncs.h"

int main()
{
    const char* targetProc = "yourTargetProcess.exe";
    const char* dllPath = "D:\\PathToYourDllHere\\MyDLL.dll";

    // We need the target process PID to pass to OpenProcess
    DWORD targetProcId = GetProcID(targetProc);
    if (targetProcId == NULL)
    {
        std::cout << "GetProcID returned NULL" << std::endl;
        return 0;
    }

    HANDLE targetProcHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, targetProcId);
    std::cout << "proccess handle of " << targetProc << " is: " << targetProcHandle << std::endl;
    if (targetProcHandle == NULL)
    {
        std::cout << "OpenProcess returned NULL error: " << GetLastError() << std::endl;
        return 0;
    }

    // Allocate memory in targetProc's virtual address space for our dll
    void* dllAddress = VirtualAllocEx(targetProcHandle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (dllAddress == NULL)
    {
        std::cout << "VirtualAllocEx returned NULL error: " << GetLastError() << std::endl;
        return 0;
    }

    // Write our dll's location in the memory space of our target process
    if (WriteProcessMemory(targetProcHandle, dllAddress, dllPath, strlen(dllPath) + 1, 0) == 0)
    {
        std::cout << "WriteProcessMemory failed" << "\n";
        return 0;
    }

    // In order to use LoadLibrary we need to get the address of it within kernel32.dll
    HMODULE kernel32Handle = GetModuleHandle("Kernel32");
    if (kernel32Handle == NULL)
    {
        std::cout << "GetModuleHandle returned null error: " << GetLastError() << std::endl;
        return 0;
    }

    LPVOID loadLibraryAddress = GetProcAddress(kernel32Handle, "LoadLibraryA");
    if (loadLibraryAddress == NULL)
    {
        std::cout << "GetProcAddress returned NULL error: " << GetLastError() << std::endl;
        return 0;
    }

    // Create a remote thread in target process to execute our dll
    HANDLE remoteThreadHandle = CreateRemoteThread(targetProcHandle, 0, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, dllAddress, 0, 0);

    if (remoteThreadHandle == NULL)
    {
        std::cout << "CreateRemoteThread returned NULL error: " << GetLastError() << std::endl;
        CloseHandle(targetProcHandle);
        targetProcHandle = NULL;
        return 0;
    }
    else
    {
        std::cout << "Success: the remote thread was successfully created" << std::endl;
        CloseHandle(targetProcHandle);
        targetProcHandle = NULL;
    }

    return 0;
}

