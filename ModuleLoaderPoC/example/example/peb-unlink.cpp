#include "peb-unlink.h"
#include <iostream>
#include <TlHelp32.h>


// Get current process PEB (works for x64 and x86)
PPEB get_peb(void) {
#ifdef _WIN64
    return (PPEB)__readgsqword(0x60);  // GS:[0x60] points to PEB on x64
#else
    return (PPEB)__readfsdword(0x30);  // FS:[0x30] points to PEB on x86
#endif
}


void unlink_peb(void) {
    // Get the base address of the current DLL
    HMODULE hCurrentDll = (HMODULE)&__ImageBase;

    PPEB peb = get_peb();
    if (!peb || !peb->Ldr) return;

    PLIST_ENTRY current = &peb->Ldr->InMemoryOrderModuleList;
    PLIST_ENTRY first = current;

    do {
        PMY_LDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(
            current, MY_LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks
        );

        // Compare the DLL's base address instead of its name
        if (entry->DllBase == hCurrentDll) {
            // Unlink from InLoadOrderModuleList
            if (entry->InLoadOrderLinks.Blink && entry->InLoadOrderLinks.Flink) {
                entry->InLoadOrderLinks.Blink->Flink = entry->InLoadOrderLinks.Flink;
                entry->InLoadOrderLinks.Flink->Blink = entry->InLoadOrderLinks.Blink;
            }

            // Unlink from InMemoryOrderModuleList
            if (entry->InMemoryOrderLinks.Blink && entry->InMemoryOrderLinks.Flink) {
                entry->InMemoryOrderLinks.Blink->Flink = entry->InMemoryOrderLinks.Flink;
                entry->InMemoryOrderLinks.Flink->Blink = entry->InMemoryOrderLinks.Blink;
            }

            // Unlink from InInitializationOrderModuleList
            if (entry->InInitializationOrderLinks.Blink && entry->InInitializationOrderLinks.Flink) {
                entry->InInitializationOrderLinks.Blink->Flink = entry->InInitializationOrderLinks.Flink;
                entry->InInitializationOrderLinks.Flink->Blink = entry->InInitializationOrderLinks.Blink;
            }

            // Clear base address and other fields for extra stealth
            entry->DllBase = NULL;
            entry->SizeOfImage = 0;
            entry->EntryPoint = NULL;

            return;  // Found and hidden
        }

        current = current->Flink;
    } while (current != first);
}


static bool CheckPEB(const char* dllName)
{
    PPEB peb = get_peb();
    if (!peb || !peb->Ldr)
        return false;

    LIST_ENTRY* head = &peb->Ldr->InMemoryOrderModuleList;
    LIST_ENTRY* current = head->Flink;

    while (current != head)
    {
        PMY_LDR_DATA_TABLE_ENTRY entry =
            CONTAINING_RECORD(current, MY_LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (entry->BaseDllName.Buffer)
        {
            char name[MAX_PATH] = { 0 };

            int len = WideCharToMultiByte(
                CP_ACP,
                0,
                entry->BaseDllName.Buffer,
                entry->BaseDllName.Length / sizeof(WCHAR),
                name,
                sizeof(name) - 1,
                NULL,
                NULL);

            if (len > 0)
            {
                name[len] = 0;

                if (_stricmp(name, dllName) == 0)
                    return true;
            }
        }

        current = current->Flink;
    }

    return false;
}

static bool CheckEnumProcessModules(const char* dllName)
{
    HMODULE mods[1024];
    DWORD needed = 0;

    if (!EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &needed))
        return false;

    DWORD count = needed / sizeof(HMODULE);

    for (DWORD i = 0; i < count; ++i)
    {
        char name[MAX_PATH] = { 0 };

        if (GetModuleBaseNameA(GetCurrentProcess(), mods[i], name, sizeof(name)))
        {
            if (_stricmp(name, dllName) == 0)
                return true;
        }
    }

    return false;
}

static bool CheckToolhelp(const char* dllName)
{
    HANDLE snap = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
        GetCurrentProcessId());

    if (snap == INVALID_HANDLE_VALUE)
        return false;

    MODULEENTRY32 me = {};
    me.dwSize = sizeof(me);

    bool found = false;

    if (Module32First(snap, &me))
    {
        do
        {
            if (_stricmp(me.szModule, dllName) == 0)
            {
                found = true;
                break;
            }
        } while (Module32Next(snap, &me));
    }

    CloseHandle(snap);
    return found;
}

void CheckModule(const char* dllName)
{
    if (GetModuleHandleA(dllName))
        std::cout << "GetModuleHandleA - Found the DLL!" << std::endl;

    if (GetModuleHandleExA(0, dllName, nullptr))
        std::cout << "GetModuleHandleExA - Found the DLL!" << std::endl;

    if (CheckEnumProcessModules(dllName))
        std::cout << "EnumProcessModules - Found the DLL!" << std::endl;

    if (CheckToolhelp(dllName))
        std::cout << "Toolhelp32 - Found the DLL!" << std::endl;

    if (CheckPEB(dllName))
        std::cout << "PEB loader list - Found the DLL!" << std::endl;
}