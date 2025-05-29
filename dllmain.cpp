#include "pch.h"
#include <Windows.h>
#include "hook.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
    InstallHooks();
    return 0;
}

// DllMain: punto de entrada del DLL
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        // Limpieza opcional
        UninstallHooks();
        break;
    }
    return TRUE;
}
