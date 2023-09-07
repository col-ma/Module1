#include <Windows.h>
#include <Shlwapi.h>
#include <winsafer.h>

#include "FileHandling.h"
#include "PathParser.h"

#define MAX_BUF 1024

/********************
 *	Path Validation	*
 ********************/
 // All the relevant and possible options for a path to be during validation.
typedef enum
{
	VALID_PATH,
	INVALID_PATH_TO_EXISTING_FILE,
	INVALID_PATH_TO_EXISTING_DIR,
	INVALID_PATH_TO_DIR,
	INVALID_PATH_FILE_EXIST_BUT_ITS_A_DIR_PATH,
	INVALID_PATH_NAME,
	PATH_IS_EVIL,
	INVALID_CHARACTERS_IN_PATH,
	INCOMPLETE_UNC_PATH,
	INVALID_UNC_PATH,
	PATH_IS_AN_EXECUTABLE
} PATHS_T;


LPCWSTR GetExtention(LPCWSTR lpcwFilePath, LPCWSTR lpcwFileExtention, UINT uFileExtentionBufferSize);

BOOL IsPathExecutableByExtention(LPCWSTR lpcwFilePath);
BOOL IsPathExecutableByEncodement(LPCWSTR lpcwFilePath);
BOOL IsPathExecutable(LPCWSTR lpcwFilePath);

// Gets a Path and determines if the path is evil.
BOOL IsPathEvil(LPCWSTR filePath);

// Creates the true and final path by opening the file.
DWORD GetFinalPathByExistingFile(LPCWSTR lpcwFilePath, LPCWSTR lpcwFinalPath);

// Generates a final Path to a file and determines if the path is evil.
BOOL IsFullPathEvil(LPCWSTR filePath);

// Validates if there is a valid path inside an invalid path.
PATHS_T ValidPathInNonExistingPath(LPCWSTR filePath, LPCWSTR parsedFilePath);

// Validates inputed path.
PATHS_T PathIsValid(LPCWSTR filePath);

BOOL GetFinalPathAndItParts(LPCWSTR lpcwFilePath, LPCWSTR lpcwFullPath, LPCWSTR lpcwParsedPath, LPCWSTR lpcwPrevParsedPath, LPCWSTR lpcwPrevCurrentSteps);

BOOL GetTrueLongPath(LPCWSTR lpcwPath);

BOOL AssembleFinalFullLongPath(LPCWSTR lpcwValidPath, LPCWSTR lpcwNonExistingPath, LPCWSTR lpcwFullPath, BOOL bCurrentParsedPathIsValid);

// Generates the true full path of a file, removing the shortcuts and etc...
BOOL GetTrueFullPath(WCHAR * lpcwFilePath, LPCWSTR lpcwFullPath);