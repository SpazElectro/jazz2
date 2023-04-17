#include <windows.h>
#include <stdio.h>
#include <cstdio>

typedef void (*FunctionPtr)();

void start() {
    // FunctionPtr func = (FunctionPtr)0x0043E301;

    // func();

    AllocConsole();
    FILE* f = new FILE();
    freopen_s(&f, "CONOUT$", "w", stdout);
    
    printf("[*] Running under Jazz2!\n");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            start();
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        default:
            break;
    }

    return TRUE;
}
