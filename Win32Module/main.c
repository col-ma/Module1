#include "main.h"
#include <string.h>
#include <shlwapi.h>

int main(int argc, char* argv[]) {
    //C:\Users\Michael\lol.txt
    //C:\Users\Michael\tempYo\tempYo\lol.txt
    //C:\Users\Michael\tempYo\tempYo 
    //C:\Users\Michael\tempYo\juncLinkYo\junction
    //C:\Users\Michael\tempYo\simLinkYo\symbolic
    // //DESKTOP-MIOHITT/evileviLEVIL
    // C:\evilevilevil\hello\hellowforjgjd
    // C:\evilev~1\hello\hellowforjgjd
    // C:\rabira~1\hello\hellowforjgjd
    // \\?\Volume{d7af83fd-c227-4d24-b44e-77be3b0cde85}\Users\Michael\tempYo\simLinkYo\lol.txt

    //TestIsFileEvil();
    //TestFileExists();
    //TestEscapeCharactersHandling();

    //TestAssembleFullPath();

    //if (IsPathExecutable(L"C:\\Users\\Michael\\source\\repos\\Win32Module\\x64\\Debug\\Win32Module.bat"))
    //{
    //    wprintf_s(L"YAY");
    //}

    //IsPathExecutableByRegistery(L"htmlfile");

    //STARTUPINFO StartupInfo;
    //PROCESS_INFORMATION ProcessInfo;

    //memset(&StartupInfo, 0, sizeof(StartupInfo));
    //memset(&ProcessInfo, 0, sizeof(ProcessInfo));

    //CreateProcess(L"C:\\Windows\\System32\\calc.exe",
    //    NULL,
    //    NULL,
    //    NULL,
    //    FALSE,
    //    CREATE_DEFAULT_ERROR_MODE,
    //    NULL,
    //    NULL,
    //    &StartupInfo,
    //    &ProcessInfo);

    //WCHAR pszOut[1000] = { 0 };
    //DWORD dwOut = 0;

    ///*queryAssoc->GetString(
    //    ASSOCF_NONE,
    //    ASSOCSTR_EXECUTABLE,
    //    L".bat",
    //    pszOut,
    //    dwOut
    //);*/

    //wprintf_s(L"%s\n", pszOut);

    /*while(TRUE)
    {
        SecureInputToFileToOutput();
    }*/

    system("start calc.exe");
}

int TestEscapeCharactersHandling()
{
    LPCWSTR lpcwFilePathA = L"C:\\Users\\Michael\\temp^Yo\\tempYo\\lol.txt";
    LPCWSTR lpcwFilePathB = L"C:\\Users\\Michael\\tempYo\\temp^^Yo";
    LPCWSTR lpcwFilePathC = L"C:\\Users\\Michael\\tempYo\\juncLinkYo\\junctio&n";
    LPCWSTR lpcwFilePathD = L"C:\\Users\\Michael\\tempYo\\simLinkYo\\symboli^&c";
    
    LPCWSTR lpcwFilePathCopyA = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);

    wcscpy_s(lpcwFilePathCopyA, MAX_PATH, lpcwFilePathA);
    
    LPCWSTR lpcwFilePathCopyB = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);

    wcscpy_s(lpcwFilePathCopyB, MAX_PATH, lpcwFilePathB);
    
    LPCWSTR lpcwFilePathCopyC = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);

    wcscpy_s(lpcwFilePathCopyC, MAX_PATH, lpcwFilePathC);
    
    LPCWSTR lpcwFilePathCopyD = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);

    wcscpy_s(lpcwFilePathCopyD, MAX_PATH, lpcwFilePathD);
    
    if (EscapeCharactersHandling(lpcwFilePathCopyA))
    {
        wprintf_s(L"Path is valid, and file is: %s\n", lpcwFilePathCopyA);
    }

    if (EscapeCharactersHandling(lpcwFilePathCopyB))
    {
        wprintf_s(L"Path is valid, and file is: %s\n", lpcwFilePathCopyB);
    }

    if (EscapeCharactersHandling(lpcwFilePathCopyC))
    {
        wprintf_s(L"Path is valid, and file is: %s\n", lpcwFilePathCopyC);
    }

    if (EscapeCharactersHandling(lpcwFilePathCopyD))
    {
        wprintf_s(L"Path is valid, and file is: %s\n", lpcwFilePathCopyD);
    }
}

PATHS_T TestSpecificSrtringAssembleFullPath(LPCWSTR lpcwFilePath)
{
    LPCWSTR lpcwFilePathCopy = (LPCWSTR)malloc(sizeof(WCHAR) * MAX_PATH);
    PATHS_T pValidatePathVal = VALID_PATH;
    wcscpy_s(lpcwFilePathCopy, MAX_PATH, lpcwFilePath);

    wprintf_s(L"For Path : %s\n", lpcwFilePath);
    pValidatePathVal = ValidatePath(lpcwFilePathCopy);
    wprintf_s(L"\tThe Full Path is : % s, vlidation val is %d\n", lpcwFilePathCopy, pValidatePathVal);

    return pValidatePathVal;
}

int TestAssembleFullPath() 
{
    LPCWSTR lpcwTestFilePaths[] = {
        L"\\", // \ 
        L"\\\\", // \\ 

        L"\\\\?\\", // \\?\ 
        L"\\\\.\\", // \\.\ 
        L"\\\\.\\localhost", // \\.\localhost 
        L"\\\\?\\localhost", // \\?\localhost

        L"C:\\evilevilevil\\hello\\hellowforjgjd", //C:\evilevilevil\hello\hellowforjgjd
        L"C:\\evilev~1\\hello\\hellowforjgjd",
        L"C:\\rabira~1\\hello\\hellowforjgjd",
        L"\\\\?\\Volume{d7af83fd-c227-4d24-b44e-77be3b0cde85}\\Users\\Michael\\tempYo\\simLinkYo\\lol.txt",
        L"C:\\Users\\Michael\\lol.txt",
        L"C:\\Users\\Michael\\tempYo\\tempYo\\lol.txt",
        L"C:\\Users\\Michael\\tempYo\\tempYo",
        L"C:\\Users\\Michael\\tempYo\\juncLinkYo\\junction",
        L"C:\\Users\\Michael\\tempYo\\simLinkYo\\symbolic",
        L"C:\\EvILevilevil\\hello\\hellowforjgjd",
        L"C:\\evilEvILEVil\\hello\\hellowforjgjd",

        L"C:\\Users\\Michael\\temp^Yo\\tempYo\\lol.txt",
        L"C:\\Users\\Michael\\tempYo\\temp^^Yo",
        L"C:\\Users\\Michael\\tempYo\\juncLinkYo\\junctio&n",
        L"C:\\Users\\Michael\\tempYo\\simLinkYo\\symboli^&c",

        L"//DESKTOP-MIOHITT/evileviLEVIL",
        L"//localhost/eflkjdnbdjkf",
        L"//deflkvfelkn/eflkjdnbdjkf/llojkfd",
        L"//127.0.0.1/eflkjdnbdjkf",
        L"//127.0.0.1/c$/evilevilevil",

        L"C:\\Good\\%trolololo%\\evilevilevil",
        L"\\\\.\\C:\\Good\\%trolololo%\\evilevilevil",

        L"\\\\?\\UNC\\localhost\\c$\\Good\\%trolololo%\\evilevilevil",

        L"\\\\?\\c:\\%basa%",
        L"\\\\?\\c:\\%basa:~0,2%",
        L"\\\\?\\UNC\\localhost\\c$\\%basa:~0,6%~1",
        L"\\\\?\\c:\\%basa:~0,6%~1%basa:ev=%%basa%basa:~1,2%",

        L"\\\\?\\c:\\%basa:ev=,2%",
        L"\\\\?\\c:\\%basa:~,2%",
        L"\\\\?\\c:\\%basa:~1%",
        L"\\\\?\\c:\\%basa:~1,1%",

        L"%windir%\\..\\%basa:1,2%\\lol.txt",

        L"z:\\temp.txt",

        L"C:\\Users\\Michael\\source\\repos\\Win32Module\\x64\\Debug\\Win32Module.scr",
        L"C:\\Users\\Michael\\source\\repos\\Win32Module\\x64\\Debug\\Win32Module.exe"
    };

    PATHS_T pTestFilePathsReturns[] = {
        INVALID_PATH_NAME,
        INVALID_PATH_NAME,

        INVALID_PATH_NAME,
        INVALID_PATH_NAME,
        INVALID_PATH_NAME,
        INVALID_PATH_NAME,

        PATH_IS_EVIL,
        VALID_PATH,
        INVALID_PATH_TO_EXISTING_FILE,
        PATH_IS_EVIL,
        INVALID_PATH_TO_EXISTING_FILE,
        INVALID_PATH_TO_EXISTING_FILE,
        INVALID_PATH_TO_EXISTING_DIR,
        PATH_IS_EVIL,
        PATH_IS_EVIL,
        PATH_IS_EVIL,
        PATH_IS_EVIL,
        VALID_PATH,
        VALID_PATH,
        INVALID_CHARACTERS_IN_PATH,
        PATH_IS_EVIL,

        INCOMPLETE_UNC_PATH,
        INCOMPLETE_UNC_PATH,
        INVALID_UNC_PATH,
        INCOMPLETE_UNC_PATH,
        PATH_IS_EVIL,

        VALID_PATH,
        VALID_PATH,
        VALID_PATH,
        
        PATH_IS_EVIL,
        VALID_PATH,
        PATH_IS_EVIL,
        VALID_PATH,

        VALID_PATH,
        VALID_PATH,
        VALID_PATH,
        VALID_PATH,

        VALID_PATH,
        PATH_IS_EVIL,

        PATH_IS_AN_EXECUTABLE,
        PATH_IS_AN_EXECUTABLE
    };

    for (UINT i = 0; i < (sizeof(lpcwTestFilePaths) / sizeof(LPCWSTR)); i++)
    {
        if(TestSpecificSrtringAssembleFullPath(lpcwTestFilePaths[i]) == pTestFilePathsReturns[i])
        {
            wprintf_s(L"\nFile Path %d is valid\n\n", i);
        }
    }
}

int TestFileExists()
{
    LPCWSTR filePathA = L"\\\\?\\Volume{d7af83fd-c227-4d24-b44e-77be3b0cde85}\\Users\\Michael\\tempYo\\simLinkYo\\symbolic";
    LPCWSTR filePathB = L"\\\\?\\Volume{d7af83fd-c227-4d24-b44e-77be3b0cde85}\\";
    LPCWSTR filePathC = L"\\\\?\\C:\\";
    
    if (FileExists(filePathA))
    {
        printf_s("A Exists\n");
    }

    if (FileExists(filePathB))
    {
        printf_s("B Exists\n");
    }

    if (FileExists(filePathC))
    {
        printf_s("C Exists\n");
    }
}

int TestIsFileEvil()
{
    LPCWSTR filePathA = L"C:\\evilevilevil\\hello\\hellowforjgjd";
    LPCWSTR filePathB = L"C:\\evilEvilEVIl\\hello\\hellowforjgjd";
    LPCWSTR filePathC = L"\\\\?\\C:\\evilEvilEVIl\\hello\\hellowforjgjd";

    if (IsPathEvil(filePathA))
    {
        printf_s("A is Evil\n");
    }

    if(IsPathEvil(filePathB))
    {
        printf_s("B is Evil\n");
    }

    if (IsPathEvil(filePathC))
    {
        printf_s("C is Evil\n");
    }
}

int TestDeleteTheFile()
{
    LPCWSTR filePath = L"C:\\Users\\Michael\\tempYo\\tempYo";

    DeleteTheFile(filePath);
}

int TestCreateFileHierarchy()
{
    LPCWSTR filePath = L"C:\\Users\\Michael\\tempYo\\tempYo\\lol.txt";

    CreateFileHierarchy(filePath);
}

int TestGetAndConcatenateSteps() 
{
    LPCWSTR directoryName = L"C:\\Users\\Michael\\tempYo\\tempYo\\";

    WCHAR directoryParsed[MAX_PATH] = { 0 };

    WCHAR* directoryNameCopy = (WCHAR *)malloc(sizeof(WCHAR) * MAX_PATH);

    wcscpy_s(directoryNameCopy, MAX_PATH, directoryName);

    while (wcscmp(directoryNameCopy, directoryParsed) || strlen(directoryName) > 0)
    {        
        GetAndConcatenateSteps(directoryParsed, &directoryName, L'\\', MAX_PATH);
    }
}

