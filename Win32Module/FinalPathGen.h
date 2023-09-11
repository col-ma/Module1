#include <Windows.h>

#include "PathParser.h"
#include "PathTypes.h"
#include "UNCPathHandling.h"
#include "EnvironmentVariableHandling.h"

// Creates the true and final path by opening the file.
DWORD GetFinalPathByExistingFile(LPCWSTR lpcwFilePath, LPCWSTR lpcwFinalPath);

BOOL GetFinalPathAndItParts(LPCWSTR lpcwFilePath, LPCWSTR lpcwFullPath, LPCWSTR lpcwParsedPath, LPCWSTR lpcwPrevParsedPath, LPCWSTR lpcwPrevCurrentSteps);

BOOL GetTrueLongPath(LPCWSTR lpcwPath);

BOOL AssembleFinalFullLongPath(LPCWSTR lpcwValidPath, LPCWSTR lpcwNonExistingPath, LPCWSTR lpcwFullPath, BOOL bCurrentParsedPathIsValid);

// Generates the true full path of a file, removing the shortcuts and etc...
BOOL GetTrueFullPath(WCHAR* lpcwFilePath, LPCWSTR lpcwFullPath);

PATHS_T StripAllUNCPathLayers(LPCWSTR lpcwPath);

PATHS_T AssembleFullPath(LPCWSTR lpcwFilePath);
