#include <Windows.h>

#include "FileHandling.h"

#define MAX_BUF 1024

LPCWSTR GetExtention(LPCWSTR lpcwFilePath, LPCWSTR lpcwFileExtention, UINT uFileExtentionBufferSize);

BOOL IsPathExecutableByExtention(LPCWSTR lpcwFilePath);
BOOL IsPathExecutableByEncodement(LPCWSTR lpcwFilePath);
BOOL IsPathExecutable(LPCWSTR lpcwFilePath);
