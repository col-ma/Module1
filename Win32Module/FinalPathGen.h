#include <Windows.h>

#include "PathParser.h"
#include "PathTypes.h"
#include "UNCPathHandling.h"
#include "EnvironmentVariableHandling.h"

#define MAX_PATH_LENGTH 2048

// Creates the true and final path by opening the file.
DWORD GetFinalPathByExistingFile(LPCWSTR lpcwFilePath, LPCWSTR lpcwFinalPath);

// Tries opening every directory in a path to determine which part exists, and outputs the seperated parts of the path.
BOOL GetFinalPathAndItParts(LPCWSTR lpcwFilePath, LPCWSTR lpcwFullPath, LPCWSTR lpcwParsedPath, LPCWSTR lpcwPrevParsedPath, LPCWSTR lpcwPrevCurrentSteps);

// tries to expand the path to its long form.
BOOL GetTrueLongPath(LPCWSTR lpcwPath);

// expands the existing path and concatenates the expanded path with the nono existing path.
BOOL AssembleFinalFullLongPath(LPCWSTR lpcwValidPath, LPCWSTR lpcwNonExistingPath, LPCWSTR lpcwFullPath, BOOL bCurrentParsedPathIsValid);

// Generates the true full path of a file, removing the shortcuts and etc...
BOOL GetTrueFullPath(WCHAR* lpcwFilePath, LPCWSTR lpcwFullPath);

// Asserts Regular prepend
BOOL AssertRegularPrepend(WCHAR* lpPath);

// Strips all the UNC layers of the path.
PATHS_T StripAllUNCPathLayers(LPCWSTR lpcwPath);

// Assembles the most expended and a local version of the path win api can correctly validate and use.
PATHS_T AssembleFullPath(LPCWSTR lpcwFilePath);
