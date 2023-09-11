#include "SecureUserInterface.h"

void GetPathToFile(LPCWSTR filePath) 
{
    printf_s("Please enter a name of a file:\n");

    // Clear the stdin buffer before the input operation
    SecureStdinFlush();

    // To get the whole line, to avoid problems in the input.
    _getws_s(filePath, MAX_PATH);
}

PATHS_T ValidatePath(LPCWSTR filePath)
{
    PATHS_T ptPathValidity;
    PATHS_T pAssemblePathErrors = AssembleFullPath(filePath);

    if (pAssemblePathErrors != VALID_PATH)
    {
        return pAssemblePathErrors;
    }

    ptPathValidity = PathIsValid(filePath);

    if (ptPathValidity == INVALID_PATH_TO_DIR)
    {
        printf_s("Path is to a directory please input a path to a file, i.e. it should not end with \'\\\'.\n");
    }
    else if (ptPathValidity == INVALID_PATH_FILE_EXIST_BUT_ITS_A_DIR_PATH)
    {
        printf_s("Path is contains a name which exist as a file.\n");
    }

    return (ptPathValidity);
}

int GetFilePathAndDelete(HANDLE* hFile) 
{
    WCHAR* filePath = (WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);
    PATHS_T ptPathValidity;

    GetPathToFile(filePath);
    ptPathValidity = ValidatePath(filePath);

    if(ptPathValidity == PATH_IS_EVIL)
    {
        printf_s("This file is in an evil directory, will not create it.\n");
        return PATH_IS_EVIL;
    }
    else if (ptPathValidity == INVALID_PATH_TO_DIR) 
    {
        return FILE_PATH_INPUT_ERROR;
    }
    else if (ptPathValidity == INVALID_PATH_NAME || ptPathValidity == INVALID_CHARACTERS_IN_PATH)
    {
        printf_s("The path name inputed was invalid, or does not match to the naming standart of windows.\n");
        return FILE_PATH_INPUT_ERROR;
    }
    else if (ptPathValidity == INCOMPLETE_UNC_PATH || ptPathValidity == INVALID_UNC_PATH)
    {
        printf_s("The path name inputed is an invalid UNC path, make sure you have inputed the server name, share and the file inside of it - you want to make.\n");
        return FILE_PATH_INPUT_ERROR;
    }
    else if (ptPathValidity == PATH_IS_AN_EXECUTABLE)
    {
        printf_s("Path is to an executable file, unfortunatly this is not allowed in this program. You would have to change the path.\n");
        return FILE_PATH_INPUT_ERROR;
    }
    // Any other return type needs the user to decide to delete the files.
    else if (ptPathValidity != VALID_PATH && DeleteTheFile(filePath))
    {
        return FILE_EXIST_BUT_WAS_NOT_DELETED;
    }

    *hFile = CreateFileHierarchy(filePath);

    if (*hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dwFileHandleError = GetLastError();

        if(dwFileHandleError == ERROR_PATH_NOT_FOUND || dwFileHandleError == ERROR_BAD_PATHNAME || dwFileHandleError == ERROR_FILE_NOT_FOUND || dwFileHandleError == ERROR_INVALID_NAME || dwFileHandleError == ERROR_DIRECTORY)
        {
            printf_s("The path name inputed was invalid, or does not match to the naming standart of windows.\n");
            return INVALID_PATH_NAME;
        } 
        else if (dwFileHandleError == ERROR_PRIVILEGE_NOT_HELD || dwFileHandleError == ERROR_ACCESS_DENIED)
        {
            printf_s("Currently the User you ar using to preform this operation do not have the requiered privilages to do this,\n\tplease preform a differend operation or switch to an user who holds the requiered privileges.\n");
            return INVALID_PATH_NAME;
        }
        else 
        {
            wprintf_s(L"[GetFilePathAndDelete]\n\tError Code : %x.\n", dwFileHandleError);
        }

        return INVALID_HANDLE_WAS_CREATED;
    }

    free(filePath);

    return VALID_FUNCTION;
}

int SecureInputToFileToOutput()
{
    HANDLE* hPtrFile = (HANDLE*)malloc(sizeof(HANDLE));
    LPCWSTR stdinLineBuffer = (LPCWSTR)malloc(sizeof(WCHAR) * STDIN_MAX_LINE_SIZE);
    LPCWSTR filePath = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);

    int functionStatus = FUNCTION_DID_NOT_TRIGGER;

    // Getting the file handle to be used in the program.
    while (functionStatus != VALID_FUNCTION)
    {
        functionStatus = GetFilePathAndDelete(hPtrFile);
    }

    // Reading stdin
    printf_s("Please write now a line you want to write into the file:\n");

    // Clear the stdin buffer before the input operation
    SecureStdinFlush();

    _getws_s(stdinLineBuffer, STDIN_MAX_LINE_SIZE);
    
    printf_s("\n");

    if (*hPtrFile != INVALID_HANDLE_VALUE)
    {
        // Writing to the file
        WriteToTheFile(*hPtrFile, stdinLineBuffer);

        free(stdinLineBuffer);

        GetFinalPathNameByHandle(*hPtrFile, filePath, MAX_PATH, FILE_NAME_NORMALIZED);

        // Closing The File
        CloseHandle(*hPtrFile);

        // Reading the file
        stdinLineBuffer = (LPCWSTR)malloc(sizeof(WCHAR) * STDIN_MAX_LINE_SIZE);
        ReadTheFile(filePath, stdinLineBuffer, sizeof(WCHAR) * STDIN_MAX_LINE_SIZE);

        wprintf_s(L"file contants are: \n\t%s\n", stdinLineBuffer);
    }

    free(filePath);
    free(stdinLineBuffer);
    free(hPtrFile);

    return 0;
}