#include "PathValidation.h"

BOOL IsPathEvil(LPCWSTR filePath)
{
    LPCWSTR lpwcEvilDir = L"\\\\?\\c:\\evilevilevil";
    WCHAR* lpwcFilePathCopy[MAX_PATH] = { 0 };

    wcscpy_s(lpwcFilePathCopy, wcslen(filePath) + 1, filePath);

    _wcslwr_s(lpwcFilePathCopy, wcslen(lpwcFilePathCopy) + 1);

    return (CheckIfContainedInStart(lpwcFilePathCopy, lpwcEvilDir));
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
                if (IsPathEvil(parsedFilePath))
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
