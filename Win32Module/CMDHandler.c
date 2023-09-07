#include "CMDHandler.h"

static LPCWSTR RandString(UINT uMaxLength) 
{
    UINT uRandSeed = rand();
    UINT uLength;
    WCHAR charset[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    WCHAR * lpcwRandomString = NULL;

    srand(time(NULL) + uRandSeed);

    uLength = ((UINT)rand() % uMaxLength) + 1;

    if (uLength) {
        lpcwRandomString = (LPCWSTR)malloc(sizeof(WCHAR) * (uLength + 1));

        for (UINT n = 0; n < uLength; n++) {
            UINT key = rand() % (UINT)((sizeof(charset) / sizeof(WCHAR)) - 1);
            lpcwRandomString[n] = charset[key];
        }
        lpcwRandomString[uLength] = '\0';
    }

    return lpcwRandomString;
}

BOOL CMDCommandAssembler(LPCWSTR lpcwFilePath, LPCWSTR lpcwToEcho, LPCWSTR lpcwFullCommand)
{
    LPCWSTR lpcwCommandBase = L"C:\\Windows\\System32\\cmd.exe /c echo ";

    wcscat_s(lpcwFullCommand, wcslen(lpcwFullCommand) + wcslen(lpcwCommandBase) + 1, lpcwCommandBase);
    wcscat_s(lpcwFullCommand, wcslen(lpcwFullCommand) + wcslen(lpcwToEcho) + 1, lpcwToEcho);
    wcscat_s(lpcwFullCommand, wcslen(lpcwFullCommand) + wcslen(L" > ") + 1, L" > ");
    wcscat_s(lpcwFullCommand, wcslen(lpcwFullCommand) + wcslen(lpcwFilePath) + 1, lpcwFilePath);
}

DWORD CMDExecutioner(LPCWSTR lpcwBatchCommand)
{
    BOOL ok = TRUE;
    HANDLE hStdInPipeRead = NULL;
    HANDLE hStdInPipeWrite = NULL;
    HANDLE hStdOutPipeRead = NULL;
    HANDLE hStdOutPipeWrite = NULL;

    // Create two pipes.
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    ok = CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0);
    if (ok == FALSE) return -1;
    ok = CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0);
    if (ok == FALSE) return -1;

    // Create the process.
    STARTUPINFO si = { 0 };
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdError = hStdOutPipeWrite;
    si.hStdOutput = hStdOutPipeWrite;
    si.hStdInput = hStdInPipeRead;
    PROCESS_INFORMATION pi = { 0 };
    LPCWSTR lpApplicationName = L"C:\\Windows\\System32\\cmd.exe";
    LPWSTR lpCommandLine = (LPWSTR)lpcwBatchCommand;
    LPSECURITY_ATTRIBUTES lpProcessAttributes = NULL;
    LPSECURITY_ATTRIBUTES lpThreadAttribute = NULL;
    BOOL bInheritHandles = TRUE;
    DWORD dwCreationFlags = 0;
    LPVOID lpEnvironment = NULL;
    LPCWSTR lpCurrentDirectory = NULL;

    ok = CreateProcess(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttribute,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        &si,
        &pi
    );

    if (ok == FALSE)
    {
        return -1;
    }

    // Close pipes we do not need.
    CloseHandle(hStdOutPipeWrite);
    CloseHandle(hStdInPipeRead);

    // The main loop for reading output from the DIR command.
    WCHAR buf[1024 + 1] = { 0 };
    DWORD dwRead = 0;
    DWORD dwAvail = 0;
    ok = ReadFile(hStdOutPipeRead, buf, 1024, &dwRead, NULL);
    while (ok == TRUE)
    {
        buf[dwRead] = '\0';
        OutputDebugString(buf);
        puts(buf);
        ok = ReadFile(hStdOutPipeRead, buf, 1024, &dwRead, NULL);
    }

    // Clean up and exit.
    CloseHandle(hStdOutPipeRead);
    CloseHandle(hStdInPipeWrite);
    DWORD dwExitCode = 0;
    GetExitCodeProcess(pi.hProcess, &dwExitCode);

    return dwExitCode;
}

BOOL GenerateTempFilePath(LPCWSTR lpcwTempFullPath, LPCWSTR lpcwTempDirPath)
{
    LPCWSTR lpcwRootDir = L"C:\\";
    WCHAR lpcwTempDir[MAX_PATH] = { 0 };
    WCHAR lpcwTempFile[MAX_PATH] = { 0 };

    BOOL bFolderExists = FALSE;
    BOOL bFileExists = FALSE;

    LPCWSTR lpcwTempString;

    lpcwTempString = RandString(32);
    wcscpy_s(lpcwTempDir, wcslen(lpcwTempString) + 1, lpcwTempString);
    free(lpcwTempString);

    lpcwTempString = RandString(32);
    wcscpy_s(lpcwTempFile, wcslen(lpcwTempString) + 1, lpcwTempString);
    free(lpcwTempString);

    wcscat_s(lpcwTempFullPath, wcslen(lpcwTempFullPath) + wcslen(lpcwRootDir) + 1, lpcwRootDir);
    wcscat_s(lpcwTempFullPath, wcslen(lpcwTempFullPath) + wcslen(lpcwTempDir) + 1, lpcwTempDir);
    wcscpy_s(lpcwTempDirPath, wcslen(lpcwTempFullPath) + 1, lpcwTempFullPath);
    bFolderExists = FileExists(lpcwTempFullPath);

    wcscat_s(lpcwTempFullPath, wcslen(lpcwTempFullPath) + wcslen(L"\\") + 1, L"\\");
    wcscat_s(lpcwTempFullPath, wcslen(lpcwTempFullPath) + wcslen(lpcwTempFile) + 1, lpcwTempFile);

    bFileExists = FileExists(lpcwTempFullPath);

    return(!bFolderExists && !bFileExists);
}

HANDLE GenerateTempFile(LPCWSTR lpcwTempFullPath, LPCWSTR lpcwTempDirPath)
{
    while (!GenerateTempFilePath(lpcwTempFullPath, lpcwTempDirPath)){}

    return CreateFileHierarchy(lpcwTempFullPath);
}

// Randomize the temp output file name, and sub directory name.
// Create file hierarachy of c:\temp\temp 
// Create the command - with the file.
// run the sub-process
// close the sub-process
// read out of the file the CMD-formated path
// Close the handle of c:\temp\temp 
// Ensure the deletion of c:\temp\temp 
BOOL FormatPathWithCMD(LPCWSTR lpcwFilePath)
{
    HANDLE hTempFile;
    WCHAR lpcwTempFilePath[MAX_PATH] = { 0 };
    WCHAR lpcwTempDirPath[MAX_PATH] = { 0 };
    WCHAR lpcwFullCommand[MAX_PATH] = { 0 };
    WCHAR lpcwReadBuffer[sizeof(WCHAR) * 500] = {0};//[65536] = { 0 };

    hTempFile = GenerateTempFile(lpcwTempFilePath, lpcwTempDirPath);
    CloseHandle(hTempFile);

    CMDCommandAssembler(lpcwTempFilePath, lpcwFilePath, lpcwFullCommand);
    CMDExecutioner(lpcwFullCommand);

    //ReadTheFile(lpcwTempFilePath, lpcwReadBuffer, sizeof(WCHAR) * 65536);

    OVERLAPPED overlap = { 0 };
    DWORD dwBufferLength = 0;
    DWORD dwReadBufferLength = 0;
    BOOL bReadSucceed = FALSE;
    LPVOID buffer = (LPVOID)malloc(sizeof(WCHAR) * 500);

    HANDLE hFile = CreateFileHandle(lpcwTempFilePath, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY);

    bReadSucceed = ReadFile(
        hFile,
        buffer, //buffer,
        500 - 1, // To be able to add a null character in the end.
        &dwBufferLength, //&dwBufferLength,
        &overlap
    );

    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, buffer, dwBufferLength - 1, NULL, 0);

    dwReadBufferLength = MultiByteToWideChar(
                            CP_UTF8,       // CodePage
                            0,             // dwFlags
                            buffer,         // lpMultiByteStr
                            dwBufferLength - 1,   // cbMultiByte
                            lpcwReadBuffer,        // lpWideCharStr
                            wchars_num//500    // cchWideChar
                        );

    // Adding a null character in the end of the read line.
    //dwReadBufferLength /= sizeof(WCHAR);
    lpcwReadBuffer[dwReadBufferLength - 1] = L'\0';

    // Closing The File
    CloseHandle(hFile);

    wcscpy_s(lpcwFilePath, wcslen(lpcwReadBuffer) + 1, lpcwReadBuffer);

    DeleteFileWithValidPath(lpcwTempFilePath);
    DeleteFileWithValidPath(lpcwTempDirPath);
}