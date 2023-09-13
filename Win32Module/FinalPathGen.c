#include "FinalPathGen.h"

/***************************************************************
 *	GetFinalPathByExistingFile:
 *      Gets the final path by opening the file, 
 *      fetching the final path and closing the handle.
 *
 *	@param	lpcwFilePath    The original file path.
 *	@param	lpcwFinalPath   The final path to the file.
 *
 *	@rerurn The error code from CreateFile function.
 ***************************************************************/
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

/***************************************************************************************
 *	GetFinalPathAndItParts:
 *      Tries get the final path by the parsed path. If the operation succseed the
 *      function updates the previos parsed path to be the final path, and copies the
 *      file path to the previous current steps.
 *
 *	@param	lpcwFilePath            The current file path.
 *	@param	lpcwFullPath            The final path to the file.
 *  @param	lpcwParsedPath          The the part of the file path to be checked.
 *  @param	lpcwPrevParsedPath      The previous and checked parts of the path.
 *  @param	lpcwPrevCurrentSteps    The previous steps that were left until completion. 
 *
 *	@rerurn If the current parsed path exist then True, False otherwise.
 ***************************************************************************************/
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

/***********************************************************
 *	GetTrueLongPath:
 *      Expands a path to its long path.
 *
 *	@param	lpcwPath    The file path.
 *
 *	@rerurn If the function succseed True, False otherwise. 
 ***********************************************************/
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

/*******************************************************************************
 *	AssembleFinalFullLongPath:
 *      Gets the valid part of a path, expands it to long form and then 
 *      to the final path form and assemples the true path of the file.
 *
 *	@param	lpcwValidPath               The existing part of the file path.
 *  @param	lpcwNonExistingPath         The non existing part of the file path. 
 *  @param	lpcwFullPath                The true and full file path to be.
 *  @param	bCurrentParsedPathIsValid   If the Valid path is the full path of 
 *                                          the file.
 *
 *	@rerurn If the function succseed True, False otherwise.
 *******************************************************************************/
BOOL AssembleFinalFullLongPath(LPCWSTR lpcwValidPath, LPCWSTR lpcwNonExistingPath, LPCWSTR lpcwFullPath, BOOL bCurrentParsedPathIsValid)
{

    // Try to get longpathname.
    if (!GetTrueLongPath(lpcwValidPath))
    {
        return FALSE;
    }

    GetFinalPathByExistingFile(lpcwValidPath, lpcwFullPath);

    // If the current path is not valid the function concatenates the non existing part of the path to the path.
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

/***************************************************************************
 *	GetTrueFullPath:
 *      Goes over the path to find the existing part of the path expands    
 *      it and outputs the full path.
 *
 *	@param	lpcwFilePath    The file path.
 *  @param	lpcwFullPath    The true and full file path to be.
 *
 *	@rerurn If the function succseed True, False otherwise.
 ***************************************************************************/
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

/***********************************************************
 *	AssertRegularPrepend:
 *      Asserts there is a regular prepend, if there is no  
 *      regular prepend it fixes it.
 *
 *	@param	lpPath  The file path.
 *
 *	@rerurn If the function found an irregular prepend  
 *              then True, otherwise False.
 ***********************************************************/
BOOL AssertRegularPrepend(WCHAR* lpPath)
{
    LPCWSTR lpIrregularPrepend = L"\\\\.\\";

    if (CheckIfContainedInStart(lpPath, lpIrregularPrepend))
    {
        lpPath[2] = L'?';

        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
 *	StripAllUNCPathLayers:
 *      UNC path can be striped into another UNC path, this means there is the  
 *      possability to have lots of layers of UNC. 
 *      This function strippes all of them until it gets to a non-UNC path.
 *
 *	@param	lpcwPath  The file path.
 *
 *	@rerurn If the path is some way is invalid, it return the invalidity, 
 *              otherwise VALID_PATH
 *******************************************************************************/
PATHS_T StripAllUNCPathLayers(LPCWSTR lpcwPath)
{
    WCHAR lpwcFullFilePath[MAX_PATH] = { 0 };
    PATHS_T pathUNCError;

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

/*******************************************************************************
 *	AssembleFullPath:
 *      Assembles the most expended and a local version of the path win32 api 
 *      can correctly validate and use.
 *
 *	@param	lpcwPath  The file path.
 *
 *	@rerurn If the path is some way is invalid, it return the invalidity,
 *              otherwise VALID_PATH
 *******************************************************************************/
PATHS_T AssembleFullPath(LPCWSTR lpcwFilePath)
{
    WCHAR lpwcFullFilePath[MAX_PATH] = { 0 };

    PATHS_T pathUNCError;

    AssertRegularPrepend(lpcwFilePath);

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