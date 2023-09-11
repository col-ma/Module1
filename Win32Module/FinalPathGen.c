#include "FinalPathGen.h"

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

    BOOL bFunctionValid = TRUE;
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

BOOL GetTrueFullPath(WCHAR* lpcwFilePath, LPCWSTR lpcwFullPath)
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

PATHS_T StripAllUNCPathLayers(LPCWSTR lpcwPath)
{
    WCHAR lpwcFullFilePath[MAX_PATH] = { 0 };
    PATHS_T pathUNCError;

    AssertRegularPrepend(lpwcFullFilePath);

    do
    {
        pathUNCError = GetLocalPathFromUncPath(lpcwPath);

        if (pathUNCError != VALID_PATH)
        {
            return pathUNCError;
        }

        if (!GetTrueFullPath(lpcwPath, lpwcFullFilePath))
        {
            return INVALID_PATH_NAME;
        }

        wcscpy_s(lpcwPath, (wcslen(lpwcFullFilePath) + 1), lpwcFullFilePath);
    } while (PathIsUNC(lpcwPath));

    return VALID_PATH;
}

PATHS_T AssembleFullPath(LPCWSTR lpcwFilePath)
{
    WCHAR lpwcFullFilePath[MAX_PATH] = { 0 };

    PATHS_T pathUNCError;

    ExpendAllEnvironmentVariables(lpcwFilePath);

    if (!HandleSpecialCharacters(lpcwFilePath))
    {
        return INVALID_CHARACTERS_IN_PATH;
    }

    GetFullPathName(lpcwFilePath, MAX_PATH, lpwcFullFilePath, NULL);

    pathUNCError = StripAllUNCPathLayers(lpwcFullFilePath);

    if (pathUNCError != VALID_PATH)
    {
        return pathUNCError;
    }

    wcscpy_s(lpcwFilePath, (wcslen(lpwcFullFilePath) + 1), lpwcFullFilePath);

    return VALID_PATH;
}