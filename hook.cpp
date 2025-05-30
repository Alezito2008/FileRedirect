#include "pch.h"
#include <MinHook.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ShlObj.h>
#include "util.h"
#include <PathCch.h>
#pragma comment(lib, "Pathcch.lib")

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

std::wstring createDirFromPath(std::wstring originalPath, std::wstring baseDir) {
    wchar_t buffer[MAX_PATH];
    wcscpy_s(buffer, originalPath.c_str());
    PathCchRemoveFileSpec(buffer, MAX_PATH);
    std::wstring dirsToCreate = baseDir + splitLast(std::wstring(buffer), L"\\Balatro");
    SHCreateDirectory(NULL, dirsToCreate.c_str());
    return dirsToCreate;
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
    std::wstring redirectPath = baseDirW + L"\\redirect" + splitLast(lpFileNameW, L"\\Balatro");
    
    std::wcout << "HookedCreateFileW called with: " << lpFileNameW << std::endl;
    if (dwDesiredAccess & GENERIC_READ) std::wcout << L"  - GENERIC_READ" << std::endl;
    if (dwDesiredAccess & GENERIC_WRITE) std::wcout << L"  - GENERIC_WRITE" << std::endl;
    if (dwDesiredAccess & GENERIC_EXECUTE) std::wcout << L"  - GENERIC_EXECUTE" << std::endl;
    if (dwDesiredAccess & GENERIC_ALL) std::wcout << L"  - GENERIC_ALL" << std::endl;

    std::wstring dirToCreate = createDirFromPath(lpFileNameW, baseDirW + L"\\redirect");
    size_t lastSlash = lpFileNameW.find_last_of(L"\\/");
    std::wstring fileNameOnly = lpFileNameW.substr(lastSlash + 1);
    std::wstring newFilePath = dirToCreate + L"\\" + fileNameOnly;

    if (dwDesiredAccess & GENERIC_WRITE) {
        lpFileNameW = newFilePath;
    }
    else if (dwDesiredAccess & GENERIC_READ) {
        if (fileExists(newFilePath)) {
			lpFileNameW = newFilePath;
        }
        else {
            CopyFileW(lpFileNameW.c_str(), newFilePath.c_str(), FALSE);
        }
    }

    return originalCreateFileW(lpFileNameW.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
};

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
