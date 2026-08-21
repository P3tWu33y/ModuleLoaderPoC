#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <psapi.h>
#include "suspender.h"
#include "peb-unlink.h"


#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "advapi32.lib")

ThreadSuspender g_threadSuspender;
HMODULE g_hModule = nullptr;

// --- Exports ---
#pragma comment(linker, "/export:GetFileVersionInfoA=C:\\Windows\\System32\\version.GetFileVersionInfoA")
#pragma comment(linker, "/export:GetFileVersionInfoByHandle=C:\\Windows\\System32\\version.GetFileVersionInfoByHandle")
#pragma comment(linker, "/export:GetFileVersionInfoExW=C:\\Windows\\System32\\version.GetFileVersionInfoExW")
#pragma comment(linker, "/export:GetFileVersionInfoSizeA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeA")
#pragma comment(linker, "/export:GetFileVersionInfoSizeExW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExW")
#pragma comment(linker, "/export:GetFileVersionInfoSizeW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeW")
#pragma comment(linker, "/export:GetFileVersionInfoW=C:\\Windows\\System32\\version.GetFileVersionInfoW")
#pragma comment(linker, "/export:VerQueryValueA=C:\\Windows\\System32\\version.VerQueryValueA")
#pragma comment(linker, "/export:VerQueryValueW=C:\\Windows\\System32\\version.VerQueryValueW")


DWORD WINAPI InitThread(LPVOID)
{

    //
	// Check if loaded inside the intended process (play.exe in this case)
    //

    wchar_t path[MAX_PATH]{};

    GetModuleFileNameW(
        NULL,
        path,
        MAX_PATH
    );

    if (!wcsstr(path, L"play.exe"))
    {
        FreeLibraryAndExitThread(g_hModule, 0);
        return 0;
    }

    //
    // Check if loaded inside the intended process (play.exe in this case)
    //


    MessageBoxA(
        NULL,
        "Loaded into the selected process!",
        "Success",
        MB_OK
    );


    //
    // Your normal DLL code here
    //

    Sleep(5000); // Simulate some work);
    Beep(500, 500);

    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);


   
    while (true) 
    {

		if (GetAsyncKeyState(VK_F9) & 0x8000)
        {

            Beep(250, 250);

        }

        if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000)
        {
            Beep(500, 500);
        }



        
        Sleep(100);
    }

    return 0;
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;

        DisableThreadLibraryCalls(hModule);

        HANDLE hThread = CreateThread(
            nullptr,
            0,
            InitThread,
            nullptr,
            0,
            nullptr
        );

        if (hThread)
            CloseHandle(hThread);
    }

    return TRUE;
}

   
