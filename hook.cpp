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

std::wstring RedirectPath(LPCWSTR originalPath, const std::wstring& writeTo, const std::wstring& limiter) {
    std::wstring pathW(originalPath);
    size_t pos = pathW.find(limiter);
    if (pos != std::wstring::npos) {
        std::wstring relPath = pathW.substr(pos + limiter.length());
        pathW = writeTo + relPath;
    }
    return pathW;
}

bool fileExists(const std::wstring& path) {
	std::ifstream file(path);
    return file.is_open();
}

HANDLE WINAPI HookedCreateFileW(
    LPCWSTR lpFileName, DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{   
    std::wstring lpFileNameW(lpFileName);
    std::wstring baseDirW(baseDir.begin(), baseDir.end());
    std::wstring newPath = RedirectPath(lpFileName, baseDirW + L"\\Redirect", L"AppData\\Roaming\\Balatro");

    // if file doesnt exist in /redirect, copy from original location to redirect
    if (!fileExists(newPath)) {
		// copy file from original path (lpFileName) to /redirect
        // lpFileName is entire path, so we need to extract the filename
        std::wstring fileName = lpFileNameW.substr(lpFileNameW.find_last_of(L"\\") + 1);
        std::wstring newFile = baseDirW + L"\\Redirect\\aaa\\" + std::wstring(fileName);
        CopyFileW(lpFileName, newFile.c_str(), FALSE);
        std::wcout << L"Copied file from " << lpFileNameW << L" to " << newFile << std::endl;
	}

    return originalCreateFileW(newPath.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
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
