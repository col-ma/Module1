#include "PathValidation.h"

/*******************************************************
 *	IsPathEvil:
 *      Determines if the given path is evil.
 *
 *	@param	lpcwPath  The file path.
 *
 *	@rerurn If the path is evil then True, else False.  
 *******************************************************/
BOOL IsPathEvil(LPCWSTR filePath)
{
    LPCWSTR lpwcEvilDir = L"\\\\?\\c:\\evilevilevil";
    WCHAR* lpwcFilePathCopy[MAX_PATH] = { 0 };

    wcscpy_s(lpwcFilePathCopy, wcslen(filePath) + 1, filePath);

    // Lowers case the path because in windows there is no difference between high and low case.
    _wcslwr_s(lpwcFilePathCopy, wcslen(lpwcFilePathCopy) + 1);

    return (CheckIfContainedInStart(lpwcFilePathCopy, lpwcEvilDir));
}

/*******************************************************
 *	PathIsValid:
 *      Determines if the given path is Valid.
 *
 *	@param	lpcwPath  The file path.
 *
 *	@rerurn If the path is Valid then VALID_PATH, else  
 *              the invalidity error code.
 *******************************************************/
PATHS_T PathIsValid(LPCWSTR filePath)
{
    WCHAR parsedFilePath[MAX_PATH] = { 0 };
    WCHAR wcSlashSeperator = L'/';
    WCHAR wcBackSlashSeperator = L'\\';

    if (filePath[wcslen(filePath) - 1] == wcBackSlashSeperator || filePath[wcslen(filePath) - 1] == wcSlashSeperator)
    {
        return INVALID_PATH_TO_DIR;
    }

    if (IsPathEvil(filePath))
    {
        return PATH_IS_EVIL;
    }

    ValidPathInNonExistingPath(filePath, parsedFilePath);

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
