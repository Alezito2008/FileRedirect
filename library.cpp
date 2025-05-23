#include "library.h"

#include <iostream>
#include "MinHook/include/MinHook.h"

typedef int (WINAPI* MessageBoxWType)(HWND, LPCWSTR, LPCWSTR, UINT);

MessageBoxWType originalMessageBoxW = nullptr;

int WINAPI HookedMessageBoxW(HWND hwnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    return originalMessageBoxW(hwnd, L"Hooked!", lpCaption, uType);
}

void setupHook() {
    MessageBoxW(NULL, L"DLL INYECTADO", L"DLL INYECTADO", MB_OK);

    MH_STATUS status;

    status = MH_Initialize();

    if (status != MH_OK) {
        MessageBox(NULL, "Inicialización fallida", "Error", MB_OK | MB_ICONERROR);
    }

    status = MH_CreateHook(reinterpret_cast<LPVOID>(MessageBoxW), reinterpret_cast<LPVOID>(HookedMessageBoxW),
        reinterpret_cast<LPVOID*>(&originalMessageBoxW));

    if (status != MH_OK) {
        MessageBox(NULL, "Error al hookear", "Error", MB_OK | MB_ICONERROR);
    }

    status = MH_EnableHook(reinterpret_cast<LPVOID>(MessageBoxW));

    if (status != MH_OK) {
        MessageBox(NULL, "Error al iniciar hook", "Error", MB_OK | MB_ICONERROR);
    }
}
