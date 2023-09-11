#include <Windows.h>
#include "PathParser.h"

// Creates the true and final path by opening the file.
DWORD GetFinalPathByExistingFile(LPCWSTR lpcwFilePath, LPCWSTR lpcwFinalPath);

BOOL GetFinalPathAndItParts(LPCWSTR lpcwFilePath, LPCWSTR lpcwFullPath, LPCWSTR lpcwParsedPath, LPCWSTR lpcwPrevParsedPath, LPCWSTR lpcwPrevCurrentSteps);

BOOL GetTrueLongPath(LPCWSTR lpcwPath);

BOOL AssembleFinalFullLongPath(LPCWSTR lpcwValidPath, LPCWSTR lpcwNonExistingPath, LPCWSTR lpcwFullPath, BOOL bCurrentParsedPathIsValid);

// Generates the true full path of a file, removing the shortcuts and etc...
BOOL GetTrueFullPath(WCHAR* lpcwFilePath, LPCWSTR lpcwFullPath);