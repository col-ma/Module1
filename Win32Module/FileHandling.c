#include "FileHandling.h"

BOOL FileExists(WCHAR * filePath) 
{
    DWORD dwFileAttrib;
    BOOL bWasWithBackslash = filePath[wcslen(filePath) - 1] == L'\\';
    
    dwFileAttrib = GetFileAttributes(filePath);

    // To determine if drive the file has to have the back slash.
    if (dwFileAttrib == INVALID_FILE_ATTRIBUTES && bWasWithBackslash) 
    {
        filePath[wcslen(filePath) - 1] = L'\0';
        dwFileAttrib = GetFileAttributes(filePath);
        filePath[wcslen(filePath)] = L'\\';
    }

    return (dwFileAttrib != INVALID_FILE_ATTRIBUTES);
}

BOOL FileIsDirectory(WCHAR * filePath) 
{
    DWORD dwFileAttrib;
    BOOL bWasWithBackslash = (filePath[wcslen(filePath) - 1] == L'\\');

    if (bWasWithBackslash)
    {
        filePath[wcslen(filePath) - 1] = L'\0';
    }

    dwFileAttrib = GetFileAttributes(filePath);

    if (bWasWithBackslash)
    {
        filePath[wcslen(filePath)] = L'\\';
    }

    return (FileExists(filePath) && (dwFileAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int DeleteFileWithValidPath(LPCWSTR lpcwValidFilePathToDelete)
{
    if (FileIsDirectory(lpcwValidFilePathToDelete))
    {
        int iSHFileOpReturnVal;

        MovePointerAfterPrePend(&lpcwValidFilePathToDelete);

        // The file path to SHFileOperation should be double NULL terminated string.
        // Specifically because this is directory deletion with the backslash.
        PrepareToSHFileOp(lpcwValidFilePathToDelete);

        SHFILEOPSTRUCT shFileOp = {
            NULL,
            FO_DELETE,
            lpcwValidFilePathToDelete,
            NULL,
            FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
            FALSE,
            0,
            L""
        };

        // SHFileOperation doesn't use GetLastError, the return value determines the error.
        iSHFileOpReturnVal = SHFileOperation(&shFileOp);

        if (iSHFileOpReturnVal != 0 && iSHFileOpReturnVal != 2) {
            printf_s("[DeleteTheFile | SHFileOperation]:\n\tError code : %x\n", SHFileOperation(&shFileOp));
            return 1;
        }
    }
    else
    {
        if (DeleteFile(lpcwValidFilePathToDelete) == 0)
        {
            DWORD dwFileHandleError = GetLastError();

            wprintf_s(L"Error Code is %x\n", dwFileHandleError);
        }
    }
}

int DeleteTheFile(LPCWSTR filePath) 
{
    char answer[DEL_ANSWER_MAX_SIZE] = {0};
    BOOL bAnswerIsYes;

    printf_s("The file you wrote exist, do you want to delete it [y/N]\n");
    scanf_s("%s", answer, (unsigned int)sizeof(answer));

    bAnswerIsYes = !(strlen(answer) != 1 || (answer[0] != 'y' && answer[0] != 'Y'));

    if(bAnswerIsYes) {
        //WCHAR * filePathToDelete= (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
        WCHAR filePathToDelete[MAX_PATH] = { 0 };
        *filePathToDelete = L'\0';

        ValidPathInNonExistingPath(filePath, filePathToDelete);


        // If the given path is of a file which should be a directory, it needs to remove the backslash to recognize the file.
        if (filePathToDelete[wcslen(filePathToDelete) - 1] == L'\\') {
            filePathToDelete[wcslen(filePathToDelete) - 1] = L'\0';
        }
        
        DeleteFileWithValidPath(filePathToDelete);
    } 
    else 
    {
        printf_s("Will not delete.\n");
    }

    return (!bAnswerIsYes);
}

BOOL WriteToTheFile(HANDLE hFile, LPCWSTR bufferToWrite)
{
    DWORD dwBufferSizeToWrite   = (DWORD)wcslen(bufferToWrite) * sizeof(WCHAR);
    DWORD dwWrittenBufferLength = 0;
    BOOL bWriteSucceed = TRUE;

    // Checks if the file is in the evil directory.
    
    bWriteSucceed = WriteFile(
        hFile,
        bufferToWrite,
        dwBufferSizeToWrite,
        &dwWrittenBufferLength,
        NULL
    );
    
    return bWriteSucceed;
}

BOOL ReadTheFile(LPCWSTR filePath, WCHAR* bufferToRead, DWORD dwMaxBufferSizeToRead)
{
    OVERLAPPED overlap = {0};
    //DWORD dwBufferLength = 0;
    DWORD dwReadBufferLength = 0;
    BOOL bReadSucceed = FALSE;
    //LPVOID buffer = (LPVOID)malloc(dwMaxBufferSizeToRead);

    HANDLE hFile = CreateFileHandle(filePath, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY);

    bReadSucceed = ReadFile(
        hFile,
        bufferToRead, //buffer,
        dwMaxBufferSizeToRead - 1, // To be able to add a null character in the end.
        &dwReadBufferLength, //&dwBufferLength,
        &overlap
    );

    // Adding a null character in the end of the read line.
    dwReadBufferLength /= sizeof(WCHAR);
    bufferToRead[dwReadBufferLength] = L'\0';

    // Closing The File
    CloseHandle(hFile);

    return bReadSucceed;
}

HANDLE CreateFileHandle(LPCWSTR filePath, DWORD dwCreationDispositions, DWORD dwFlagsAndAttributes) 
{
    HANDLE hFile = CreateFile(
                        filePath, 
                        (GENERIC_WRITE | GENERIC_READ),
                        0, 
                        NULL, 
                        dwCreationDispositions,
                        dwFlagsAndAttributes,
                        NULL
                    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD fileHandleError = GetLastError();

        if(fileHandleError != ERROR_PRIVILEGE_NOT_HELD && fileHandleError != ERROR_ACCESS_DENIED)
        wprintf_s(L"[CreateFileHandle]:\n\tfor path %s, last Error %x\n", filePath, fileHandleError);
    }

    return hFile;
}

HANDLE CreateFileHierarchy(LPCWSTR filePath)
{
    WCHAR parsedFilePath[MAX_PATH] = { 0 };
    WCHAR filePathCopy[MAX_PATH] = { 0 };
    HANDLE hFile = INVALID_HANDLE_VALUE;
    
    MovePointerAfterPrePend(&filePath);

    wcscpy_s(filePathCopy, wcslen(filePath) + 1, filePath);
    
    while (wcscmp(filePathCopy, parsedFilePath))
    {
        GetAndConcatenateSteps(parsedFilePath, &filePath, MAX_PATH);

        if (!FileExists(parsedFilePath))
        {
            if (!wcscmp(filePathCopy, parsedFilePath))
            {
                // The loop got to the end of the file Path, i.e. thats a file.
                hFile = CreateFileHandle(parsedFilePath, CREATE_NEW, FILE_ATTRIBUTE_NORMAL);
            } 
            else
            {
                // If it did not got to the end of the path, the current path is of a directory which does not exist.
                if (!CreateDirectory(parsedFilePath, NULL))
                {
                    DWORD dwCreateDirectoryError = GetLastError();

                    if (dwCreateDirectoryError == ERROR_PATH_NOT_FOUND) {
                        wprintf_s(L"[CreateDirectoryHandle]\n\tFor Path : %s Error code : %x\n", parsedFilePath, dwCreateDirectoryError);
                    }

                    return INVALID_HANDLE_VALUE;
                }
            }
        }
    }

    return hFile;
}