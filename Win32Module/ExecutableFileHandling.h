#include <Windows.h>
#include <winsafer.h>
#include <shlwapi.h>

#include "FileHandling.h"

#define MAX_BUF 1024

// Fetches and copies the extension from
LPCWSTR GetExtention(LPCWSTR lpcwFilePath, LPCWSTR lpcwFileExtention, UINT uFileExtentionBufferSize);

BOOL IsPathExecutableByRegistery(LPCWSTR lpcwExtention);

// Checks if a path is an executable by extention.
BOOL IsPathExecutableByExtention(LPCWSTR lpcwFilePath);

// Checks if a path is an executable by how it is encoded.
BOOL IsPathExecutableByEncodement(LPCWSTR lpcwFilePath);

// Checks if a path is an executable in ant possible way.
BOOL IsPathExecutable(LPCWSTR lpcwFilePath);
