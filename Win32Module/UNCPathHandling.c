#include "UNCPathHandling.h"

/***************************************************************
 *	RemoveBackslashAtEnd:
 *      Removes a backslash at the end of the path if it exist. 
 *
 *	@param	lpcwPath    The path to the file.
 *
 *	@rerurn If there was a backslash at the end then True,
 *              False otherwise.
 ***************************************************************/
BOOL RemoveBackslashAtEnd(WCHAR* lpcwPath)
{
    BOOL bThereIsABackslash = (lpcwPath[wcslen(lpcwPath) - 1] == L'\\');

    if (bThereIsABackslash)
    {
        lpcwPath[wcslen(lpcwPath) - 1] = L'\0';
    }

    return bThereIsABackslash;
}

/*******************************************************************
 *	GetLocalPathByUncServerArndSharePath:
 *      Gets a local path by the server name and the share name.    
 * 
 *	@param	lpcwServerName  The server name.
 *	@param	lpcwShare       The shared directory name.
 *	@param	lpcwLocalPath   The local path of the Shared directory. 
 *
 *	@rerurn The return value of the NetShareGetInfo, 
 *              i.e. the error code.
 *******************************************************************/
DWORD GetLocalPathByUncServerArndSharePath(LPCWSTR lpcwServerName, LPCWSTR lpcwShare, LPCWSTR lpcwLocalPath)
{
    PSHARE_INFO_502 BufPtr;
    NET_API_STATUS nasNetShareInfo;

    nasNetShareInfo = NetShareGetInfo(lpcwServerName, lpcwShare, 502, (LPBYTE*)&BufPtr);

    // Call the NetShareGetInfo function, specifying level 502.
    if (nasNetShareInfo == ERROR_SUCCESS)
    {
        wcscpy_s(lpcwLocalPath, wcslen(BufPtr->shi502_path) + 1, BufPtr->shi502_path);
    }

    return nasNetShareInfo;
}

/*******************************************************************
 *	GetLocalPathFromUncPath:
 *      Converts a UNC path  - ifi it a UNC path, to a local path.
 *
 *	@param	lpcwPath    The path.
 *
 *	@rerurn If the path is invalid in some way it return it UNC 
 *          invalidity, otherwise VALID_PATH.
 *******************************************************************/
PATHS_T GetLocalPathFromUncPath(LPCWSTR lpcwPath)
{
    LPCWSTR lpUNCPrepend = L"\\\\?\\UNC\\";
    LPCWSTR lpRegularUNCPrepend = L"\\\\";
    LPCWSTR lpcwServer = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
    LPCWSTR lpcwShare = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
    LPCWSTR lpcwPathCopy = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
    DWORD dwGetLocalPathByUNC = 0;

    // If the UNC starts with '\\?\UNC\...' it switches it to just regular '\\'
    if (CheckIfContainedInStart(lpcwPath, lpUNCPrepend))
    {
        wcscpy_s(lpcwPath + wcslen(lpRegularUNCPrepend), wcslen(lpcwPath + wcslen(lpUNCPrepend)) + 1, (lpcwPath + wcslen(lpUNCPrepend)));
    }

    if (PathIsUNCServer(lpcwPath) || PathIsUNCServerShare(lpcwPath))
    {
        return INCOMPLETE_UNC_PATH;
    }

    if (PathIsUNC(lpcwPath) && !PathIsUNCServer(lpcwPath) && !PathIsUNCServerShare(lpcwPath))
    {
        WCHAR lpcwUNCServerName[MAX_PATH] = { 0 };
        WCHAR lpcwUNCShareName[MAX_PATH] = { 0 };

        wcscpy_s(lpcwPathCopy, wcslen(lpcwPath) + 1, lpcwPath);

        lpcwPathCopy += wcslen(lpRegularUNCPrepend);

        GetStep(lpcwUNCServerName, &lpcwPathCopy, MAX_PATH);
        GetStep(lpcwUNCShareName, &lpcwPathCopy, MAX_PATH);
        
        RemoveBackslashAtEnd(lpcwUNCServerName);
        RemoveBackslashAtEnd(lpcwUNCShareName);

        dwGetLocalPathByUNC = GetLocalPathByUncServerArndSharePath(lpcwUNCServerName, lpcwUNCShareName, lpcwPath);

        wcscat_s(lpcwPath, wcslen(lpcwPath) + wcslen(lpcwPathCopy) + 1, lpcwPathCopy);
    }

    if(dwGetLocalPathByUNC == ERROR_BAD_NETPATH)
    {
        return INVALID_UNC_PATH;
    }

    return VALID_PATH;
}
