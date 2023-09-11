#ifndef PATH_PARSER_H
#define PATH_PARSER_H

#include <stdio.h> // geWCHAR(), ungetc() and fflush()
#include <Windows.h> // WCHAR, LPCWSTR, DWORD, BOOL, TRUE, FALSE, wcscsat_s(), memcpy(), free(),
#include <Shlwapi.h>

/********************************
 *	Step Parsing of the Path	*
 ********************************/
// Gets a step out of the current pointer.
BOOL GetStep(WCHAR* out, WCHAR** current, size_t maxSize);

// Gets a step and concatenates to prefix.
void GetAndConcatenateSteps(WCHAR* prefix, WCHAR** current, rsize_t prefixSize);

// Ensures Double Null termination in the end of a string.
void EnsureDoubleNullTermination(WCHAR* pStr);

// Prepares a path of a directory to the SHFileOp
void PrepareToSHFileOp(WCHAR* pPathToDirectory);

/************
 *	Misc	*
 ************/
// Checks if a string is contained in another string.
BOOL CheckIfContainedInStart(LPCWSTR pBaseStr, LPCWSTR pContainedStr);

/********************************
 *	Step Parsing of the Path	*
 ********************************/
// Moves a s tring to start wothout a path prepend.
BOOL MovePointerAfterPrePend(LPCWSTR* lpcwPath);

// Adds a prepend to a path.
void AddPrependAtStart(LPCWSTR* lpcwPath); 

BOOL ContainsForbiddenChars(LPCWSTR lpcwForbidden, LPCWSTR lpcwPath);

BOOL EscapeCharactersHandling(LPCWSTR lpcwPath);

BOOL HandleSpecialCharacters(LPCWSTR lpcwPath);

BOOL AssertRegularPrepend(WCHAR* lpPath);

#endif // __PATH_PARSER_H