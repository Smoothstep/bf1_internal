#include "stdafx.h"

#include <iostream>

#include <Windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <TlHelp32.h>

#include <Psapi.h>

#include <cstdio>
#include <dsound.h>

#pragma comment (lib, "Shlwapi.lib")

#define INVALID_HANDLE(x) (x == NULL || x == INVALID_HANDLE_VALUE)
#define WAIT_SHUTDOWN while(1) { printf("Execution halted. Waiting for shutdown...\n"); return std::getchar(); }

struct DLLParameters
{
	void * pLoadLibraryAddr;
	wchar_t wszPathName[MAX_PATH];
	wchar_t wszPathNameDep[MAX_PATH];
};

int _tmain(int argc, const wchar_t* argv[])
{
	if (argc < 3)
	{
		printf("Missing parameters: Injector <ProcessName> <DllPathRelative>\n");
		WAIT_SHUTDOWN;
	}

	const wchar_t* c_wszLibraryName = argv[2];
	const wchar_t* c_wszProcessName = argv[1];

	if (!PathFileExistsW(c_wszLibraryName))
	{
		printf("Dll not found.\n");
		WAIT_SHUTDOWN;
	}

	DLLParameters LoadingParameters;

	DWORD dwLength = GetCurrentDirectory(0, NULL);
	DWORD dwPid = 0;

	if (dwLength + wcslen(c_wszLibraryName) >= MAX_PATH - 1)
	{
		printf("Path too long.\n");
		WAIT_SHUTDOWN;
	}

	GetCurrentDirectory(dwLength, LoadingParameters.wszPathNameDep);
	PathAppend(LoadingParameters.wszPathName, LoadingParameters.wszPathNameDep);
	PathAppend(LoadingParameters.wszPathName, c_wszLibraryName);

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcess = NULL;

	while (dwPid == 0)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (INVALID_HANDLE(hSnapshot))
		{
			printf("CreateToolhelp32Snapshot failed.\n");
			WAIT_SHUTDOWN;
		}

		if (!Process32First(hSnapshot, &entry))
		{
			printf("No process entry specified.\n");
			WAIT_SHUTDOWN;
		}

		do
		{
			if (!wcscmp(entry.szExeFile, c_wszProcessName))
			{
				dwPid = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &entry));


		if (dwPid)
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);

			if (INVALID_HANDLE(hProcess))
			{
				dwPid = 0;
			}
		}

		CloseHandle(hSnapshot);

		if (!dwPid)
		{
			printf("Process not found\n");
			WAIT_SHUTDOWN;
		}

		Sleep(100);
	}

	if (!dwPid)
	{
		printf("Process not found.\n");
		WAIT_SHUTDOWN;
	}

	size_t iAllocationSize = wcslen(LoadingParameters.wszPathName) * 2;
	LPVOID pAllocated = VirtualAllocEx(hProcess, 0, iAllocationSize, MEM_COMMIT, PAGE_READWRITE);

	if (pAllocated == NULL)
	{
		printf("Unable to allocate %llu bytes.\n", wcslen(LoadingParameters.wszPathName));
		WAIT_SHUTDOWN;
	}

	HMODULE K32 = LoadLibrary(L"Kernel32");

	if (!K32)
	{
		printf("Unable to allocate %llu bytes.\n", wcslen(LoadingParameters.wszPathName));
		WAIT_SHUTDOWN;
	}

	// Set directory
	{
		if (!WriteProcessMemory(
			hProcess,
			pAllocated,
			LoadingParameters.wszPathNameDep,
			iAllocationSize + 1,
			NULL))
		{
			printf("Failed to write memory to 0x%p. Size: %llu\n",
				pAllocated,
				wcslen(LoadingParameters.wszPathName));

			WAIT_SHUTDOWN;
		}

		LPVOID pSetDllDirectoryAddr = GetProcAddress(K32, "SetDllDirectoryW");

		if (!pSetDllDirectoryAddr)
		{
			printf("ProcAddress not found: SetDllDirectoryW\n");
			WAIT_SHUTDOWN;
		}

		HANDLE hThread = CreateRemoteThread(
			hProcess,
			0, 0,
			(LPTHREAD_START_ROUTINE)pSetDllDirectoryAddr,
			pAllocated,
			0, 0);

		if (INVALID_HANDLE(hThread))
		{
			printf("Create Remote Thread failed.\n");
			WAIT_SHUTDOWN;
		}

		WaitForSingleObject(hThread, INFINITE);

		DWORD dwExitCode;

		if (GetExitCodeThread(hThread, &dwExitCode))
		{
			if (!dwExitCode)
			{
				printf("DLL Injection failed.");
			}
			else if (dwExitCode != STILL_ACTIVE)
			{
				printf("DLL Injection succeeded.\n");
			}
		}
	}

	// Load Lib
	{
		if (!WriteProcessMemory(
			hProcess,
			pAllocated,
			LoadingParameters.wszPathName,
			iAllocationSize + 1,
			NULL))
		{
			printf("Failed to write memory to 0x%p. Size: %llu\n",
				pAllocated,
				wcslen(LoadingParameters.wszPathName));

			WAIT_SHUTDOWN;
		}

		LPVOID pLoadLibraryAddr = GetProcAddress(K32, "LoadLibraryW");

		if (!pLoadLibraryAddr)
		{
			printf("ProcAddress not found: LoadLibraryW\n");
			WAIT_SHUTDOWN;
		}

		HANDLE hThread = CreateRemoteThread(
			hProcess,
			0, 0,
			(LPTHREAD_START_ROUTINE)pLoadLibraryAddr,
			pAllocated,
			0, 0);

		if (INVALID_HANDLE(hThread))
		{
			printf("Create Remote Thread failed.\n");
			WAIT_SHUTDOWN;
		}

		WaitForSingleObject(hThread, INFINITE);

		DWORD dwExitCode;

		if (GetExitCodeThread(hThread, &dwExitCode))
		{
			if (!dwExitCode)
			{
				printf("DLL Injection failed.");
				WAIT_SHUTDOWN;
			}
			else if (dwExitCode != STILL_ACTIVE)
			{
				printf("DLL Injection succeeded.\n");
			}
		}
	}

	CloseHandle(hProcess);

	return 0;
}