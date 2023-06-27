#include <windows.h>
#include <stdio.h>
#include <cstdio>

//                                    second arg is probably the filename
typedef BOOL (*JJ2Log)(char *text, ...);
typedef char __cdecl (*PlaySample)(int xPos, int yPos, int sample, int idk, int alsoIdk);
typedef int __cdecl (*shrug)(int xPos, int yPos);

void sendLog(char* text) {
    // log
    ((JJ2Log)0x48EDC0)(text, "jazz2.log");
    ((shrug)0x401010)(500, 500);
}

void start() {
    AllocConsole();
    FILE* f = new FILE();
    freopen_s(&f, "CONOUT$", "w", stdout);
    
    printf("[*] Running under Jazz2!\n");

    while(true) {
        if((GetAsyncKeyState(VK_F10) & 0x8000)) {
            sendLog("Test message from external DLL");
        }
        
        if((GetAsyncKeyState(VK_F1) & 0x8000)) {
            sendLog("calling function yaay");

            ((PlaySample)0x477D10)(0, 0, 137, 0, 0);
        }

        if((GetAsyncKeyState(VK_F12) & 0x8000)) {
            break;
        }
        
        Sleep(100);
    }
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
