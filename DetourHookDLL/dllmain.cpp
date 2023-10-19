// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <chrono>
#include <fstream>
#include <iostream>
#pragma warning(disable:4996)
using namespace std;

static BOOL(WINAPI* realCreateProcess)(LPCWSTR lpszImageName, LPWSTR lpszCmdLine, LPSECURITY_ATTRIBUTES lpsaProcess, LPSECURITY_ATTRIBUTES lpsaThread, BOOL fInheritHandles, DWORD fdwCreate, LPVOID lpvEnvironment, LPCWSTR lpszCurDir, LPSTARTUPINFOW lpsiStartInfo, LPPROCESS_INFORMATION lppiProcInfo) = CreateProcess;

BOOL WINAPI HookedCreateProcess(LPCWSTR lpszImageName, LPWSTR lpszCmdLine, LPSECURITY_ATTRIBUTES lpsaProcess, LPSECURITY_ATTRIBUTES lpsaThread, BOOL fInheritHandles, DWORD fdwCreate, LPVOID lpvEnvironment, LPCWSTR lpszCurDir, LPSTARTUPINFOW lpsiStartInfo, LPPROCESS_INFORMATION lppiProcInfo)
{
	cout << L"\n[+] CreateProcess : " << endl;/*
	logCall += L"\n[+] CreateProcess : ";
	logCall += lpszImageName;

	logCall += L" : ";
	logCall += lpszCmdLine;

	logCall += L" : ";
	logCall += lpszCurDir;
	logCall += L" : ";

	logCall += L" : ";*/


	return realCreateProcess(lpszImageName, lpszCmdLine, lpsaProcess, lpsaThread, fInheritHandles, fdwCreate, lpvEnvironment, lpszCurDir, lpsiStartInfo, lppiProcInfo);
}

void InstallHook()
{

	DetourRestoreAfterWith(); // avoid repetition
	DetourTransactionBegin(); // start hook
	DetourUpdateThread(GetCurrentThread());

	// Process APIs Functions
	DetourAttach(&(PVOID&)realCreateProcess, HookedCreateProcess);

	DetourTransactionCommit(); // commit hook

}

void UninstallHook()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// Process APIs Functions
	DetourDetach(&(PVOID&)realCreateProcess, HookedCreateProcess);

	DetourTransactionCommit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	if (DetourIsHelperProcess()) { return TRUE; }

	HANDLE currProc = GetCurrentProcess();


	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		auto start = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(start);

		ofstream myfile;
		myfile.open("C:\\Windows\\Temp\\API_Calls_Monitor.txt");
		myfile << "[+] DLLMain Entry.\n" << "[+] API Calls Monitor Started at: " << std::ctime(&end_time) << "[+] process ID: " << GetCurrentProcessId() << endl;
		myfile.close();

		DisableThreadLibraryCalls(hModule);

		InstallHook();

		break;
	}
	case DLL_THREAD_ATTACH:
	{
		break;
	}
	case DLL_THREAD_DETACH:
	{
		break;
	}
	case DLL_PROCESS_DETACH:
	{

		UninstallHook();
		//writeLog(logCall, true);
		break;
	}
	}
	return TRUE;
}