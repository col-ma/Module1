#include "ExecutableFileHandling.h"

/***************************************************************************************
 *	GetExtention:
 *      Fetches the extention of the path and outputs it.
 *
 *	@param	lpcwFilePath	            The path the extention would be fetched from.   
 *	@param	lpcwFileExtention	        The fetched extention.
 *	@param	uFileExtentionBufferSize    The allocated buffer size for the extention.
 *
 *	@rerurn A pointer in the path where the extention starts.
 ***************************************************************************************/
LPCWSTR GetExtention(LPCWSTR lpcwFilePath, LPCWSTR lpcwFileExtention, UINT uFileExtentionBufferSize)
{
    // Assuming the path went throuh all the validations and asserts.
    LPCWSTR lpcwStartOfExtention = wcsrchr(lpcwFilePath, L'\\') + 1;

    if (lpcwStartOfExtention == NULL)
    {
        lpcwStartOfExtention = lpcwFilePath;
    }

    lpcwStartOfExtention = wcsrchr(lpcwStartOfExtention, L'.');

    if (lpcwStartOfExtention != NULL)
    {
        wcscpy_s(lpcwFileExtention, wcslen(lpcwStartOfExtention) + 1, lpcwStartOfExtention);
    }

    return lpcwStartOfExtention;
}


BOOL IsPathExecutableByRegistery(LPCWSTR lpcwExtention)
{
    DWORD dwLength = 0;
    for(int i = 1; i < ASSOCSTR_MAX;  i++)
    {
        BOOL bValidQueryAssoc = AssocQueryString(ASSOCF_NONE, i, lpcwExtention, NULL, NULL, &dwLength);

        if (bValidQueryAssoc == TRUE)
        {

            WCHAR* lpcwStringQuery = (LPCWSTR)malloc(sizeof(WCHAR) * dwLength);
            bValidQueryAssoc = AssocQueryString(ASSOCF_NONE, i, lpcwExtention, NULL, lpcwStringQuery, &dwLength);

            wprintf_s(L"Default app is : %i , %s\n", i, lpcwStringQuery);
            if(bValidQueryAssoc == TRUE)
            {
            }

            free(lpcwStringQuery);
        }
    }

    wprintf_s(L"Error :(\n");
}


/***************************************************************
 *	IsPathExecutableByExtention:
 *      Determines if the current system assumes the path 
 *      is a an executable only by the extention.
 *
 *	@param	lpcwFilePath    The path to the file.
 *
 *	@rerurn If the file is executable True, False otherwise.   
 ***************************************************************/
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

    wprintf_s(L"Extention is %s\n", lpcwExtention);

    IsPathExecutableByRegistery(lpcwExtention);

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

/***************************************************************
 *	IsPathExecutableByEncodement:
 *      Determines if the current system assumes the path
 *      is a an executable only by how the file is encoded.
 *
 *	@param	lpcwFilePath    The path to the file.
 *
 *	@rerurn If the file is executable True, False otherwise.
 ***************************************************************/
BOOL IsPathExecutableByEncodement(LPCWSTR lpcwFilePath)
{
    DWORD dwFileType;
    SHFILEINFO shfi = { 0 };
    DWORD* dwpSHGFI;

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

/***************************************************************
 *	IsPathExecutable:
 *      Determines if the current system assumes the path
 *      is a an executable in every way.
 *
 *	@param	lpcwFilePath    The path to the file.
 *
 *	@rerurn If the file is executable True, False otherwise.    
 ***************************************************************/
BOOL IsPathExecutable(LPCWSTR lpcwFilePath)
{
    BOOL bIsExecutableByExtention = IsPathExecutableByExtention(lpcwFilePath);
    BOOL bIsExecutableByEncodement = IsPathExecutableByEncodement(lpcwFilePath);
    BOOL bIsExecutableBySecpol = FileExists(lpcwFilePath) && SaferiIsExecutableFileType(lpcwFilePath, FALSE);

    return (bIsExecutableByExtention || bIsExecutableByEncodement || bIsExecutableBySecpol);
}