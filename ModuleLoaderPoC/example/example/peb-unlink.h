#pragma once

#include <windows.h>
#include <winternl.h>
#include <psapi.h>
#include <stdio.h>


#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "psapi.lib")

// Only define the loader entry we need.
// LIST_ENTRY, UNICODE_STRING, PEB, PPEB, PPEB_LDR_DATA,
// and CONTAINING_RECORD already come from winternl.h.

typedef struct _MY_LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY      InLoadOrderLinks;
    LIST_ENTRY      InMemoryOrderLinks;
    LIST_ENTRY      InInitializationOrderLinks;
    PVOID           DllBase;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
    UNICODE_STRING  FullDllName;
    UNICODE_STRING  BaseDllName;
} MY_LDR_DATA_TABLE_ENTRY, * PMY_LDR_DATA_TABLE_ENTRY;

// This global symbol gives the base address of the DLL (MSVC specific)
extern "C" IMAGE_DOS_HEADER __ImageBase;

void unlink_peb(void);

void CheckModule(const char* dllName);

static bool CheckToolhelp(const char* dllName);

static bool CheckEnumProcessModules(const char* dllName);

static bool CheckPEB(const char* dllName);

