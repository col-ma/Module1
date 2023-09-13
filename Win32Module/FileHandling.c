#include "FileHandling.h"

/***********************************************************
 *	FileExists:
 *		Checks if the file in the given path exists.
 *
 *	@param	filePath    A pointer to the file path.
 * 
 *	@rerurn	If the file exists then True, False otherwise.  
 ***********************************************************/
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

/*******************************************************************
 *	FileIsDirectory:
 *		Checks if the file in the given path is a directory.    
 *
 *	@param	filePath    A pointer to the file path.
 *
 *	@rerurn If the file is a directory then True, False otherwise.  
 *******************************************************************/
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

/***************************************************************************
 *	ValidPathInNonExistingPath:
 *		Fetches to the parsed file the existing part of the path, 
 *      and determines if the valid path is a directory, 
 *      or the full path is not valid.
 *
 *	@param	filePath        A pointer to the file path.
 *  @param	parsedFilePath  The fetched valid part of the path - 
 *                          should be allocated before usign this function. 
 *
 *	@rerurn	    If the file is a directory or the full path does not exist 
 *              then False, True otherwise.
 ***************************************************************************/
BOOL ValidPathInNonExistingPath(LPCWSTR filePath, LPCWSTR parsedFilePath)
{
    BOOL bValidPathInNonExistingPath = FALSE;
    WCHAR* filePathCopy = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);
    WCHAR wcBackSlash = L'\\';
    WCHAR wcSlash = L'/';

    wcscpy_s(filePathCopy, wcslen(filePath) + 1, filePath);

    while (wcscmp(filePathCopy, parsedFilePath) && !bValidPathInNonExistingPath)
    {
        GetAndConcatenateSteps(parsedFilePath, &filePath, MAX_PATH);

        if (wcscmp(L"\\\\", parsedFilePath) != 0)
        {
            bValidPathInNonExistingPath |= (
                FileExists(parsedFilePath) &&
                (wcscmp(filePathCopy, parsedFilePath) != 0) &&
                (
                    parsedFilePath[wcslen(parsedFilePath) - 1] == wcBackSlash ||
                    parsedFilePath[wcslen(parsedFilePath) - 1] == wcSlash
                    ) &&
                !FileIsDirectory(parsedFilePath)
                );
        }
    }

    return (!bValidPathInNonExistingPath);
}

/*******************************************************************************
 *	DeleteFileWithValidPath:
 *		Assuming the given file path exists, this function handles the 
 *      deletion of it, it deletes regular files and directories differently.   
 *
 *	@param	lpcwValidFilePathToDelete   The pointer to the valid file path.
 *
 *	@rerurn	    1 if the deletion failed, 0 otherwise.
 *******************************************************************************/
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
            return 1;
        }
    }

    return 0;
}

/*******************************************************************************
 *	DeleteTheFile:
 *		Asks the users premission to delete the file, if the users agrees then  
 *      it dletes the file.
 *
 *	@param	filePath    The pointer to the valid file path.
 *
 *	@rerurn	    Returns the users answer True if he agreed to delete, 
 *              false if he did not.
 *******************************************************************************/
int DeleteTheFile(LPCWSTR filePath) 
{
    char answer[DEL_ANSWER_MAX_SIZE] = {0};
    BOOL bAnswerIsYes;

    printf_s("The file you wrote exist, do you want to delete it [y/N]\n");
    scanf_s("%s", answer, (unsigned int)sizeof(answer));

    bAnswerIsYes = !(strlen(answer) != 1 || (answer[0] != 'y' && answer[0] != 'Y'));

    if(bAnswerIsYes) {
        WCHAR filePathToDelete[MAX_PATH] = { 0 };

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

/***********************************************************************************
 *	CreateFileHandle:
 *		Creates a file handle according to the creation disposition and the 
 *      flags and attrributes. Then i handle sthe relevant error codes if needed.   
 *
 *	@param	filePath                The pointer to the file path.
 *  @param	dwCreationDispositions  The creation disposition the CreateFile 
 *                                      function will get.
 *  @param	dwFlagsAndAttributes    The Flags and attributes to the 
 *                                      CreateFile function .
 *
 *	@rerurn	    Returns the Handle to the created file, if there was an error will 
 *                  return the Value INVALID_HANDLE_VALUE
 ***********************************************************************************/
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
        {
            wprintf_s(L"[CreateFileHandle]:\n\tfor path %s, last Error %x\n", filePath, fileHandleError);
        }
    }

    return hFile;
}

/***********************************************************************************
 *	CreateFileHierarchy:
 *		Creates a full hierarchy of directories and the file. 
 *      Returns the handle to the file.
 *
 *	@param	filePath    The pointer to the file path.
 *
 *	@rerurn	    Returns the Handle to the created file, if there was an error will
 *                  return the Value INVALID_HANDLE_VALUE
 ***********************************************************************************/
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