#include "pch.h"
#include <MinHook.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "util.h"

const std::string baseDir = GetCurrentDir();

typedef int (WINAPI* MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
MessageBoxW_t originalMessageBoxW = nullptr;

typedef HANDLE(WINAPI* CreateFileW_t)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
CreateFileW_t originalCreateFileW = nullptr;

int WINAPI HookedMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    return originalMessageBoxW(hWnd, L"cccc", lpCaption, uType);
}

bool fileExists(const std::wstring& path) {
	std::ifstream file(path);
    return file.is_open();
}

HANDLE WINAPI HookedCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{   
    std::wstring lpFileNameW(lpFileName);
    std::wstring baseDirW(baseDir.begin(), baseDir.end());
    
    std::wcout << "HookedCreateFileW called with: " << lpFileNameW << std::endl;
    if (dwDesiredAccess & GENERIC_READ) std::wcout << L"  - GENERIC_READ" << std::endl;
    if (dwDesiredAccess & GENERIC_WRITE) std::wcout << L"  - GENERIC_WRITE" << std::endl;
    if (dwDesiredAccess & GENERIC_EXECUTE) std::wcout << L"  - GENERIC_EXECUTE" << std::endl;
    if (dwDesiredAccess & GENERIC_ALL) std::wcout << L"  - GENERIC_ALL" << std::endl << std::endl;



    return originalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

void InstallHooks() {
    FILE* pCout = nullptr;

    AllocConsole();
    if (freopen_s(&pCout, "CONOUT$", "w", stdout) != 0) {
        MessageBoxW(NULL, L"Error allocating console", L"Error", NULL);
    }

    std::cout << baseDir << std::endl;

    MH_Initialize();

    //MH_CreateHook(&MessageBoxW, &HookedMessageBoxW, reinterpret_cast<LPVOID*>(&originalMessageBoxW));

    //MH_EnableHook(&MessageBoxW);

    MH_CreateHook(&CreateFileW, &HookedCreateFileW, reinterpret_cast<LPVOID*>(&originalCreateFileW));
    MH_EnableHook(&CreateFileW);
}

void UninstallHooks() {
    MH_DisableHook(&MessageBoxW);
    MH_Uninitialize();
}
