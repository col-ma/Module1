#include <stdio.h>   // scanf_s, printf_s, wscanf_s, wprintf_s
#include <string.h>  // strlen
#include <Windows.h> // CretaeFile, DeleteFile, CreateDirectory, GetLastError

#include "PathParser.h"

#define DEL_ANSWER_MAX_SIZE 64

/********************************
 *	Determine File Attributes	*
 ********************************/
// Determine if the File exists
BOOL FileExists(WCHAR * filePath);

// Determine if the File is a Directory
BOOL FileIsDirectory(WCHAR* filePath);

// Validates if there is a valid path inside an invalid path.
BOOL ValidPathInNonExistingPath(LPCWSTR filePath, LPCWSTR parsedFilePath);

/********************
 *	File Deletion	*
 ********************/
// File Deletion of a valid path
int DeleteFileWithValidPath(LPCWSTR filePath);

// Using the users input it deletes chooses wether to delete a file.
int DeleteTheFile(LPCWSTR filePath);

/********************************
 *	Handle Creation Functions	*
 ********************************/
// File Hierarchy Creation
HANDLE CreateFileHandle(LPCWSTR filePath, DWORD dwCreationDispositions, DWORD dwFlagsAndAttributes);

// Creates the File handle.
HANDLE CreateFileHierarchy(LPCWSTR filePath);