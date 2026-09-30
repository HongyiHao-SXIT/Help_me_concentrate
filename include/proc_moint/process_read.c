#include <windows.h>
#include <tlhelp32.h>
#include <pdh.h>

HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
PROCESSENTRY32 pe32;
pe32.dwSize = sizeof(PROCESSENTRY32);
Process32First(hSnap, &pe32);
do
{
    printf("%s (PID: %d)\n", pe32.szExeFile, pe32.th32ProcessID);
} while (Process32Next(hSnap, &pe32));