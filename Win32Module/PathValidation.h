#include <Windows.h>
#include <Shlwapi.h>
#include <winsafer.h>

#include "FileHandling.h"
#include "PathParser.h"
#include "FinalPathGen.h"
#include "PathTypes.h"

#define MAX_BUF 1024

/********************
 *	Path Validation	*
 ********************/
LPCWSTR GetExtention(LPCWSTR lpcwFilePath, LPCWSTR lpcwFileExtention, UINT uFileExtentionBufferSize);

BOOL IsPathExecutableByExtention(LPCWSTR lpcwFilePath);
BOOL IsPathExecutableByEncodement(LPCWSTR lpcwFilePath);
BOOL IsPathExecutable(LPCWSTR lpcwFilePath);

// Gets a Path and determines if the path is evil.
BOOL IsPathEvil(LPCWSTR filePath);

// Generates a final Path to a file and determines if the path is evil.
BOOL IsFullPathEvil(LPCWSTR filePath);

// Validates if there is a valid path inside an invalid path.
PATHS_T ValidPathInNonExistingPath(LPCWSTR filePath, LPCWSTR parsedFilePath);

// Validates inputed path.
PATHS_T PathIsValid(LPCWSTR filePath);