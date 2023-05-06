#include "InjectorFuncs.h"

DWORD GetProcID(const char* targetProc)
{
    std::cout << "Target process name: " << targetProc << std::endl;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    DWORD procId = NULL;

    if (Process32First(hSnapshot, &processEntry))
    {
        do
        {
            if (_stricmp(processEntry.szExeFile, targetProc) == 0)
            {
                procId = processEntry.th32ProcessID;
                CloseHandle(hSnapshot);
                return procId;
            }
        } while (Process32Next(hSnapshot, &processEntry));
    }
    procId = NULL;
}