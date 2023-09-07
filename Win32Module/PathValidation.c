#include "PathValidation.h"

LPCWSTR GetExtention(LPCWSTR lpcwFilePath, LPCWSTR lpcwFileExtention, UINT uFileExtentionBufferSize)
{
    // Assuming the path went throuh all the validations and asserts.
    LPCWSTR lpcwStartOfExtention = wcsrchr(lpcwFilePath, L'\\') + 1;

    if (lpcwStartOfExtention == NULL)
    {
        lpcwStartOfExtention = lpcwFilePath;
    }

    lpcwStartOfExtention = wcsrchr(lpcwStartOfExtention, L'.');

    wprintf_s(L"lpcwStartOfExtention %s\n", lpcwStartOfExtention);

    if (lpcwStartOfExtention != NULL)
    {
        wcscpy_s(lpcwFileExtention, wcslen(lpcwStartOfExtention) + 1, lpcwStartOfExtention);
    }

    return lpcwStartOfExtention;
}

BOOL IsPathExecutableByExtention(LPCWSTR lpcwFilePath)
{
    WCHAR lpcwExecutableSuffixes[MAX_BUF] = { 0 };
    WCHAR lpcwExtention[MAX_BUF] = { 0 };
    
    BOOL bExtentionIsExecutable = FALSE;

    LPCWSTR lpcwCurrentExecutableSuffix = NULL;
    LPCWSTR lpcwTemp = NULL;
  
    // If there is no extention there is nothing to check.
    if (GetExtention(lpcwFilePath, lpcwExtention, MAX_BUF) == NULL)
    {
        return FALSE;
    }

    _wcslwr_s(lpcwExtention, wcslen(lpcwExtention) + 1);
    
    // If there is no PATHEXT there is no default executable files for windows, which probably is bad.
    if (GetEnvironmentVariable(L"PATHEXT", lpcwExecutableSuffixes, MAX_PATH) == 0) {
        return FALSE;
    }

    lpcwCurrentExecutableSuffix = wcstok_s(lpcwExecutableSuffixes, L";", &lpcwTemp);

    while (lpcwCurrentExecutableSuffix != NULL && !bExtentionIsExecutable)
    {
        _wcslwr_s(lpcwCurrentExecutableSuffix, wcslen(lpcwCurrentExecutableSuffix) + 1);

        bExtentionIsExecutable = (wcscmp(lpcwExtention, lpcwCurrentExecutableSuffix) == 0);

        if (!bExtentionIsExecutable)
        {
            wcscpy_s(lpcwExecutableSuffixes, wcslen(lpcwTemp) + 1, lpcwTemp);
            
            lpcwCurrentExecutableSuffix = wcstok_s(lpcwExecutableSuffixes, L";", &lpcwTemp);
        }
    }

    return bExtentionIsExecutable;
}

BOOL IsPathExecutableByEncodement(LPCWSTR lpcwFilePath)
{
    DWORD dwFileType;
    SHFILEINFO shfi = { 0 };
    DWORD * dwpSHGFI;

    if (!FileExists(lpcwFilePath))
    {
        return FALSE;
    }

    dwpSHGFI = SHGetFileInfo(
        lpcwFilePath,
        -1,
        &shfi,
        sizeof(shfi),
        SHGFI_EXETYPE
    );

    if (dwpSHGFI == 0)
    {
        DWORD dwSHGetFileInfoErrorCode = GetLastError();

        if (dwSHGetFileInfoErrorCode != ERROR_SUCCESS)
        {
            wprintf_s(L"[IsPathExecutableByEncodement] : For path %s Error %x.", lpcwFilePath, dwSHGetFileInfoErrorCode);
        }
            
        return FALSE;
    }

    return TRUE;
}

BOOL IsPathExecutable(LPCWSTR lpcwFilePath)
{       
    BOOL bIsExecutableByExtention = IsPathExecutableByExtention(lpcwFilePath);
    BOOL bIsExecutableByEncodement = IsPathExecutableByEncodement(lpcwFilePath);
    BOOL bIsExecutableBySecpol = FileExists(lpcwFilePath) && SaferiIsExecutableFileType(lpcwFilePath, FALSE);

    return (bIsExecutableByExtention || bIsExecutableByEncodement || bIsExecutableBySecpol);
}

BOOL IsPathEvil(LPCWSTR filePath)
{
    LPCWSTR lpwcEvilDir = L"c:\\evilevilevil";
    LPCWSTR lpwcPathPrefix = L"\\\\?\\";
    WCHAR* lpwcFullFilePath = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);

    // To be safe this function ensures the path checked is full
    GetFullPathName(filePath, MAX_PATH, lpwcFullFilePath, NULL);

    MovePointerAfterPrePend(&lpwcFullFilePath);

    _wcslwr_s(lpwcFullFilePath, MAX_PATH);

    return (CheckIfContainedInStart(lpwcFullFilePath, lpwcEvilDir));
}

DWORD GetFinalPathByExistingFile(LPCWSTR lpcwFilePath, LPCWSTR lpcwFinalPath)
{
    // To handle the directory handle creation.
    HANDLE hFile = CreateFile(
        lpcwFilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD fileHandleError = GetLastError();

        //wprintf_s(L"[GetFinalPathByExistingFile]:\n\tlast Error %x for %s\n", fileHandleError, lpcwFilePath);

        return fileHandleError;
    }

    GetFinalPathNameByHandle(hFile, lpcwFinalPath, MAX_PATH, FILE_NAME_OPENED);


    CloseHandle(hFile);

    return ERROR_SUCCESS;
}

BOOL IsFullPathEvil(LPCWSTR filePath)
{
    LPCWSTR lpcwFinalPath = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);

    GetFinalPathByExistingFile(filePath, lpcwFinalPath);

    return (IsPathEvil(lpcwFinalPath));
}

PATHS_T ValidPathInNonExistingPath(LPCWSTR filePath, LPCWSTR parsedFilePath)
{
    BOOL bValidPathInNonExistingPath = FALSE;
    WCHAR* filePathCopy = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);
    WCHAR wcBackSlash = L'\\';
    WCHAR wcSlash = L'/';

    wcscpy_s(filePathCopy, wcslen(filePath) + 1, filePath);

    while (wcscmp(filePathCopy, parsedFilePath) && !bValidPathInNonExistingPath)
    {
        GetAndConcatenateSteps(parsedFilePath, &filePath, MAX_PATH);

        if (wcscmp(L"\\\\", parsedFilePath) != 0) {
            bValidPathInNonExistingPath |= (
                FileExists(parsedFilePath) &&
                (wcscmp(filePathCopy, parsedFilePath) != 0) &&
                (
                    parsedFilePath[wcslen(parsedFilePath) - 1] == wcBackSlash ||
                    parsedFilePath[wcslen(parsedFilePath) - 1] == wcSlash
                    ) &&
                !FileIsDirectory(parsedFilePath)
                );

            if (FileExists(parsedFilePath))
            {
                if (IsFullPathEvil(parsedFilePath))
                {
                    return PATH_IS_EVIL;
                }
            }
        }
    }

    return (bValidPathInNonExistingPath) ? INVALID_PATH_FILE_EXIST_BUT_ITS_A_DIR_PATH : VALID_PATH;
}

PATHS_T PathIsValid(LPCWSTR filePath)
{
    WCHAR parsedFilePath[MAX_PATH] = { 0 };
    WCHAR wcSlashSeperator = L'/';
    WCHAR wcBackSlashSeperator = L'\\';

    if (filePath[wcslen(filePath) - 1] == wcBackSlashSeperator || filePath[wcslen(filePath) - 1] == wcSlashSeperator)
    {
        return INVALID_PATH_TO_DIR;
    }

    if (IsPathEvil(filePath) || (ValidPathInNonExistingPath(filePath, parsedFilePath) == PATH_IS_EVIL))
    {
        return PATH_IS_EVIL;
    }

    if (FileExists(parsedFilePath))
    {
        if (FileIsDirectory(parsedFilePath))
        {
            return INVALID_PATH_TO_EXISTING_DIR;
        }
        else
        {
            return INVALID_PATH_TO_EXISTING_FILE;
        }
    }

    if (IsPathExecutable(parsedFilePath))
    {
        return PATH_IS_AN_EXECUTABLE;
    }

    return VALID_PATH;
}

BOOL GetFinalPathAndItParts(LPCWSTR lpcwFilePath, LPCWSTR lpcwFullPath, LPCWSTR lpcwParsedPath, LPCWSTR lpcwPrevParsedPath, LPCWSTR lpcwPrevCurrentSteps)
{
    BOOL bCurrentParsedPathIsValid;

    bCurrentParsedPathIsValid = (GetFinalPathByExistingFile(lpcwParsedPath, lpcwFullPath) == ERROR_SUCCESS);

    if (bCurrentParsedPathIsValid) {
        wcscpy_s(lpcwPrevParsedPath, wcslen(lpcwFullPath) + 1, lpcwFullPath);
        wcscpy_s(lpcwPrevCurrentSteps, wcslen(lpcwFilePath) + 1, lpcwFilePath);
    }

    return bCurrentParsedPathIsValid;
}

BOOL GetTrueLongPath(LPCWSTR lpcwPath)
{
    DWORD dwLongPathNameSize = GetLongPathName(lpcwPath, lpcwPath, MAX_PATH);

    BOOL bFunctionValid     = TRUE;
    BOOL bFileNotExistError = TRUE;

    // i.e. GetLongPathName failed as the return value is the size of the path written to lpcwLongPathName.
    if (dwLongPathNameSize == 0)
    {
        DWORD dwGetLongNameErrors = GetLastError();

        bFileNotExistError = ((dwGetLongNameErrors == ERROR_FILE_NOT_FOUND) || (dwGetLongNameErrors == ERROR_PATH_NOT_FOUND));


        if (!bFileNotExistError)
        {
            wprintf_s(L"[GetLongNameErrors]: %x\n", dwGetLongNameErrors);
        }

        bFunctionValid = bFileNotExistError;
    }

    return bFunctionValid;
}

BOOL AssembleFinalFullLongPath(LPCWSTR lpcwValidPath, LPCWSTR lpcwNonExistingPath, LPCWSTR lpcwFullPath, BOOL bCurrentParsedPathIsValid)
{

    // Try to get longpathname.
    if (!GetTrueLongPath(lpcwValidPath))
    {
        return FALSE;
    }

    GetFinalPathByExistingFile(lpcwValidPath, lpcwFullPath);

    if (!bCurrentParsedPathIsValid)
    {
        if (lpcwFullPath[wcslen(lpcwFullPath) - 1] != L'\\')
        {
            wcscat_s(lpcwFullPath, wcslen(lpcwFullPath) + wcslen(L"\\") + 1, L"\\");
        }

        wcscat_s(lpcwFullPath, wcslen(lpcwFullPath) + wcslen(lpcwNonExistingPath) + 1, lpcwNonExistingPath);
    }

    return TRUE;
}

BOOL GetTrueFullPath(WCHAR * lpcwFilePath, LPCWSTR lpcwFullPath)
{
    BOOL bCurrentParsedPathIsValid = TRUE;
    BOOL bPathWithPrepend = TRUE;
    WCHAR lpcwParsedPath[MAX_PATH] = { 0 };
    WCHAR lpcwPrevParsedPath[MAX_PATH] = { 0 };
    WCHAR lpcwFilePathCopy[MAX_PATH] = { 0 };
    WCHAR lpcwPrevCurrentSteps[MAX_PATH] = { 0 };

    wcscpy_s(lpcwFilePathCopy, wcslen(lpcwFilePath) + 1, lpcwFilePath);

    bPathWithPrepend = MovePointerAfterPrePend(&lpcwFilePath);

    if (wcslen(lpcwFilePath) == 0)
    {
        return FALSE;
    }

    while (wcscmp(lpcwFilePathCopy, lpcwParsedPath) && bCurrentParsedPathIsValid)
    {
        GetAndConcatenateSteps(lpcwParsedPath, &lpcwFilePath, MAX_PATH);

        if (bPathWithPrepend)
        {
            AddPrependAtStart(&lpcwParsedPath);
            bPathWithPrepend = FALSE;
        }

        bCurrentParsedPathIsValid = GetFinalPathAndItParts(lpcwFilePath, lpcwFullPath, lpcwParsedPath, lpcwPrevParsedPath, lpcwPrevCurrentSteps);
    }

    if (wcslen(lpcwFullPath) == 0)
    {   
        return FALSE;
    }

    return AssembleFinalFullLongPath(lpcwPrevParsedPath, lpcwPrevCurrentSteps, lpcwFullPath, bCurrentParsedPathIsValid);
}