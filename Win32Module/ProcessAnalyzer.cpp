#undef _UNICODE
#include <cstdio>
#include <windows.h>
#include <detours.h>
#include <string>
#include <iostream>
#pragma warning(disable:4996)
using namespace std;

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// Value Initializion 
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(STARTUPINFO);


	string targetedProcess; //Holds the path of the suspicious PE.

	string dllToInject; //Holds the path of the DLL, which will be injected.

	int timeToRun = 0;  // The allowed running time of the process before terminating it.

	//Getting the PATH of the under testing PE.
	/*cout << "[*] Enter the Path of the targeted PE: ";
	getline(cin, targetedProcess);*/

	//Getting the PATH of the DLL
	cout << "[*] Enter the Path of the DLL \" (ex: C:\\Windows\\Temp\\APICallsMonitor.dll) ";
	getline(cin, dllToInject);

	//Getting the allowed time to run the PE.
	cout << "[*] Enter the time to run the targeted process under monitoring (in Min): ";
	cin >> timeToRun;

	//Just converting the type of var to be suitable to passed to the function
	std::wstring stemp = std::wstring(targetedProcess.begin(), targetedProcess.end());
	LPCWSTR sw = stemp.c_str();
	WCHAR cl[150] = L"C:\windows\py.exe C:\\Users\\Michael\\Desktop\\bruhbruh.py";

	// Creatintg the process for the targeted PE and injecting the DLL
	if (!DetourCreateProcessWithDllEx(NULL/*sw*/,
		cl, NULL, NULL, TRUE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
		NULL, NULL, &si, &pi,
		dllToInject.c_str(), NULL)) {
		printf("[+] DLL Injection Failed!\n");
	}
	else {
		cout << "[*] Created Process with Process ID: " << pi.dwProcessId << endl;
		printf("[+] DLL Injection Successed!\n");

		// Resume the excution of the created process of suspicious PE.
		ResumeThread(pi.hThread);

		printf("[+] Leave The process running for %d min\n", timeToRun);

		// Just printing the remaining time to kill the process in min.
		for (size_t i = 0; i < timeToRun; )
		{
			printf("\t [+] %d Min is remainaing\n", (timeToRun - i));
			Sleep((++i) * 1000 * 60);
		}

		printf("\n[+] Terminating the Process.");
		char fullCMD[30];

		sprintf_s(fullCMD, "%s%d", "taskkill  /pid ", pi.dwProcessId);
		int retval = ::system(fullCMD);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
}