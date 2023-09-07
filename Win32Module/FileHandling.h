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

/************************************
 *	Operations on existing Files	*
 ************************************/
// File Deletion
int DeleteFileWithValidPath(LPCWSTR filePath);
int DeleteTheFile(LPCWSTR filePath);

// Write To a file
BOOL WriteToTheFile(HANDLE hFile, LPCWSTR bufferToWrite);

// Read a File
BOOL ReadTheFile(LPCWSTR filePath, WCHAR* bufferToWrite, DWORD dwMaxBufferSizeToRead);

/********************************
 *	Handle Creation Functions	*
 ********************************/
// File Hierarchy Creation
HANDLE CreateFileHandle(LPCWSTR filePath, DWORD dwCreationDispositions, DWORD dwFlagsAndAttributes);

// Creates Directory.
BOOL CreateDirectoryHandle(LPCWSTR directoryPath);

// Creates the File handle.
HANDLE CreateFileHierarchy(LPCWSTR filePath);