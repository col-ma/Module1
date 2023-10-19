#include "SecureUserInterface.h"

/***************************************************************************************
 *	SecureStdinFlush:
 *		Securely flushes stdin stream, ensuring there are no newlines in the buffer,
 *		to ensure complete control over the next user's input.
 ***************************************************************************************/
void SecureStdinFlush()
{
    char letter;

    // Standart flush of stdin.
    fflush(stdin);

    // Just hops over all the newlines.
    while ((letter = getchar()) == '\n') {}

    // When the letter is not new line, the function ungets the character back to stdin so the users input would be accepted.
    if (letter != '\n')
    {
        ungetc(letter, stdin);
    }
}

/*******************************************
 *	GetPathToFile:
 *      Asks the user to get the file path. 
 *
 *	@param	filePath  The file path.
 *******************************************/
void GetPathToFile(LPCWSTR filePath) 
{
    printf_s("Please enter a name of a file:\n");

    // Clear the stdin buffer before the input operation
    SecureStdinFlush();

    // To get the whole line, to avoid problems in the input.
    _getws_s(filePath, MAX_PATH);
}

/*******************************************************
 *	ValidatePath:
 *      Expands the given path and validates it.
 *
 *	@param	lpcwPath  The file path.
 *
 *	@rerurn If the path is Valid then VALID_PATH, else
 *              the invalidity error code.
 *******************************************************/
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

/***************************************************************
 *	GetFilePathAndDelete:
 *      Gets a file path validates and it, if the path exist     
 *      it deletes the file and creates a new file.
 *
 *	@param	hFile   The file handle.
 *
 *	@rerurn if invliad one of the error code, if the handle 
 *              is valid then VALID_FUNCTION
 ***************************************************************/
int GetFilePathAndDelete(HANDLE* hFile) 
{
    WCHAR filePath[MAX_PATH] = { 0 };//(WCHAR*)malloc(sizeof(WCHAR) * STDIN_MAX_LINE_SIZE);
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

    //free(filePath);

    return VALID_FUNCTION;
}

/***********************************************************************
 *	WriteToTheFile:
 *      Writes to a file a string.
 *
 *	@param	hFile           The file handle, to be written.
 *	@param	bufferToWrite   The string to write.
 *
 *	@rerurn If the write operation succseed then True,otherwise False.  
 ***********************************************************************/
BOOL WriteToTheFile(HANDLE hFile, LPCWSTR bufferToWrite)
{
    DWORD dwBufferSizeToWrite = (DWORD)wcslen(bufferToWrite) * sizeof(WCHAR);
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

/***********************************************************************
 *	ReadTheFile:
 *      Opens a file by the path, reads it tp the buffer, and closes 
 *      the handle.
 *
 *	@param	filePath                The file path to be read.
 *	@param	bufferToRead            Buffer to copy the read string to.
 *	@param	dwMaxBufferSizeToRead   The allocated buffer size.
 *
 *	@rerurn If the read operation succseed then True,otherwise False.
 ***********************************************************************/
BOOL ReadTheFile(LPCWSTR filePath, WCHAR* bufferToRead, DWORD dwMaxBufferSizeToRead)
{
    OVERLAPPED overlap = { 0 };
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

/***********************************************************************************
 *	SecureInputToFileToOutput:
 *      Securely inputs users data into a file the closes it reopens and reads it.  
 ***********************************************************************************/
void SecureInputToFileToOutput()
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
}