#include <stdio.h> // scanf, printf
#include <Windows.h>

#include "FileHandling.h"
#include "PathValidation.h"
#include "FinalPathGen.h"

#define STDIN_MAX_LINE_SIZE 65536 //Which is 2^16

// Flushes stdin and ensures there are no new lines in the buffer.
void SecureStdinFlush();

/************************************
 *	Get File Path and File Deletion	*
 ************************************/
// All the possible and relevant error codes for the GetFilePathAndDelete function
enum GET_FILE_PATH_AND_DELETE_ERROR_CODES {
	FUNCTION_DID_NOT_TRIGGER,
	VALID_FUNCTION,
	FILE_PATH_INPUT_ERROR,
	FILE_EXIST_BUT_WAS_NOT_DELETED,
	INVALID_HANDLE_WAS_CREATED
};

// Gets the file path and from the stdin, and validates the path.
void GetPathToFile(LPCWSTR filePath);

// Chages the path to a format the rest of the program is familiar with and validates it.
PATHS_T ValidatePath(LPCWSTR filePath);

// Write To a file
BOOL WriteToTheFile(HANDLE hFile, LPCWSTR bufferToWrite);

// Read a File
BOOL ReadTheFile(LPCWSTR filePath, WCHAR* bufferToWrite, DWORD dwMaxBufferSizeToRead);

// Gets The file path and lets the user delete if it's needed. 
int GetFilePathAndDelete(HANDLE* hFile);

// Securely writes to a file, closes it and prints its contents
int SecureInputToFileToOutput();