#include "PathParser.h"

/************************************************************************************************
 *	GetStep:
 *		Cuts a string from current up to the given seperator, including the seperator.
 *		Writing the cut string to out.
 *
 *	@param	out			A pointer to the cut section of the string.
 *	@param	current		A pointer to a pointer of a string, which the string will be cut from.
 *	@param	seperator	A character which up to it the string current will be cut.
 *  @param	maxSize		An unsigned int which will be the upper limit of each step.
 *
 *	@rerurn	The step was valid and without errors.
 ************************************************************************************************/
BOOL GetStep(WCHAR *out, WCHAR** current, size_t maxSize)
{
	BOOL stepIsValid = FALSE;
	WCHAR wcBackSlash = L'\\';
	WCHAR wcSlash = L'/';

	if(**current != L'\0')
	{
		while (maxSize > 0 && **current != L'\0' && **current != wcBackSlash && **current != wcSlash)
		{
			*out++ = **current;
			++*current;
			--maxSize;
		}

		*out++ = **current;
		++*current;

		--maxSize;

		*out = L'\0';

		stepIsValid = TRUE;
	}

	return stepIsValid;
}

/***********************************************************************************************
 *	GetAndConcatenateSteps: 
 *		Cuts a string from current up to the given seperator, including the seperator.	
 *		concatenating the cut string to the prefix.
 *
 *	@param	prefix		A pointer to the cut section of the string.
 *	@param	current		A pointer to a pointer of a string, which the string will be cut from.	
 *	@param	seperator	A character which up to it the string current will be cut.
 *  @param	prefixSize	An unsigned int which is the prefixSize.
 ***********************************************************************************************/
void GetAndConcatenateSteps(WCHAR* prefix, WCHAR** current, rsize_t prefixSize)
{
	WCHAR tempParsedPath[MAX_PATH] = {0};//(WCHAR*)malloc(sizeof(WCHAR) * MAX_PATH);

	// Gets step to tempParsedPath.
	GetStep(tempParsedPath, current, MAX_PATH);

	// Concatenates the cut string.
	wcscat_s(prefix, wcslen(prefix) + wcslen(tempParsedPath) + 1, tempParsedPath);
}

/*******************************************************************************************
 *	EnsureDoubleNullTermination: 
 *		Assigns double null terminator in the end of the string, it's up to the user to 
 *		ensure enough space is allocated to the string.
 *
 *	@param	pStr	A pointer to the string, the double null terminator will be added to.	
 *******************************************************************************************/
void EnsureDoubleNullTermination(WCHAR * pStr)
{
	LPCWSTR doubleNull = L"\0\0";
	const int unsigned doubleNullSize = 2 * sizeof(WCHAR);
	memcpy(pStr + (wcslen(pStr)), doubleNull, doubleNullSize);
}

/*******************************************************************************************************
 *	PrepareToSHFileOp:
 *		Prepares a directory path string to be used by the SHFileOperation function, 
 *		i.e. ensuring double termination, and ensuring the backslash is present at the end of the path.	
 *
 *	@param	pPathToDirectory	A pointer to the directory path stirng.
 *******************************************************************************************************/
void PrepareToSHFileOp(WCHAR * pPathToDirectory)
{
	DWORD dwOriginalStrSize = wcslen(pPathToDirectory);

	if (pPathToDirectory[dwOriginalStrSize - 1] != L'\\')
	{
		pPathToDirectory[dwOriginalStrSize] = L'\\';
	}

	pPathToDirectory[dwOriginalStrSize + 1] = L'\0';

	EnsureDoubleNullTermination(pPathToDirectory);
}

/*******************************************************************************************************
 *	CheckIfContainedInStart:
 *		Check if a string is contained in another string.
 *
 *	@param	pBaseStr		The Base string to be checkd.
 *	@param	pContainedStr	The string that the function checks if is contained by the other string.	
 *
 *	@rerurn	If the stirngs is contained True, otherwise False.
 *******************************************************************************************************/
BOOL CheckIfContainedInStart(LPCWSTR pBaseStr, LPCWSTR pContainedStr)
{
	size_t iContainedSize = wcslen(pContainedStr);
	BOOL bIsContained = TRUE;

	if (wcslen(pBaseStr) >= iContainedSize) {
		for(size_t i = 0; i < iContainedSize; i++)
		{
			bIsContained &= (pBaseStr[i] == pContainedStr[i]);
		}
	}
	else
	{
		bIsContained = FALSE;
	}

	return bIsContained;
}

/***************************************************************************************
 *	MovePointerAfterPrePend:
 *		Moves a Path string to not include the prepend.
 *
 *	@param	lpcwPath		A pointer to the string.
 *
 *	@rerurn	If the stirngs contain a prepend at the start then True, otherwise False.	
 ***************************************************************************************/
BOOL MovePointerAfterPrePend(LPCWSTR* lpcwPath)
{
	LPCWSTR lpwcPathPrefix = L"\\\\?\\";
	LPCWSTR lpwcPathPrefixSecond = L"\\\\.\\";

	if(wcslen(lpcwPath) != 0)
	{
		BOOL bWithPrepend = CheckIfContainedInStart(*lpcwPath, lpwcPathPrefix);
		BOOL bWithPrependSecond = CheckIfContainedInStart(*lpcwPath, lpwcPathPrefixSecond);

		if (bWithPrepend || bWithPrependSecond) {
			wcscpy_s(*lpcwPath, MAX_PATH, (*lpcwPath + wcslen(lpwcPathPrefix)));
		}

		return bWithPrepend || bWithPrependSecond;
	}

	return FALSE;
	
}

/*******************************************************
 *	AddPrependAtStart:
 *		Adds to a Path string a prepend.
 *
 *	@param	lpcwPath		A pointer to the string.	
 *******************************************************/
void AddPrependAtStart(LPCWSTR* lpcwPath)
{
	LPCWSTR lpcwPathPrepend = L"\\\\?\\";
	WCHAR lpcwPrependedPath[MAX_PATH] = { 0 };

	wcscat_s(lpcwPrependedPath, wcslen(lpcwPrependedPath) + wcslen(lpcwPathPrepend) + 1, lpcwPathPrepend);
	wcscat_s(lpcwPrependedPath, wcslen(lpcwPrependedPath) + wcslen(lpcwPath) + 1, lpcwPath);
	memcpy(lpcwPath, lpcwPrependedPath, (wcslen(lpcwPrependedPath) + 1) * sizeof(WCHAR));
}

/***********************************************************************
 *	ContainsForbiddenChars:
 *		Determines if there are forbidden strings in the Path.	
 *
 *	@param	lpcwPath		A pointer to the Path.
 * 
 *	@return True if there is a forbidden character, False otherwise.	
 ***********************************************************************/
BOOL ContainsForbiddenChars(LPCWSTR lpcwPath)
{
	WCHAR lpcwForbidden[] = L"<>|";

	UINT szForbidden = wcslen(lpcwForbidden);
	UINT szPath = wcslen(lpcwPath);

	BOOL bContainsForbiddenChars = FALSE;

	if (szPath > 0)
	{
		for (UINT i = 0; i < szForbidden && !bContainsForbiddenChars; i++)
		{
			for(UINT j = 0; j < szPath && !bContainsForbiddenChars; j++)
			{
				bContainsForbiddenChars |= (lpcwForbidden[i] == lpcwPath[j]);
			}
		}
	}

	return bContainsForbiddenChars;
}

/***********************************************************************************
 *	EscapeCharactersHandling:
 *		Handles the rules of the escape character '^'. 
 *		Which means removing it unless it has another escape character before it. 
 *		Also it handles the '&' character which is not forbidden only when there is 
 *		an escape character before it, as well.
 *
 *	@param	lpcwPath		A pointer to the Path.
 *
 *	@return True if there is a forbidden character, False otherwise.
 ***********************************************************************************/
BOOL EscapeCharactersHandling(LPCWSTR lpcwPath)
{
	WCHAR wcEscapeChar = L'^';
	WCHAR wcApmpersendChar = L'&';
	UINT szPath = wcslen(lpcwPath);
	BOOL bPathIsValid = FALSE;


	if (szPath > 0)
	{
		for (UINT i = 0; i < szPath; i++)
		{
			// & => invalid.
			if (lpcwPath[i] == wcApmpersendChar)
			{
				return bPathIsValid;
			}
			// ^ => ''
			else if (lpcwPath[i] == wcEscapeChar)
			{
				wcscpy_s(lpcwPath + i, wcslen(lpcwPath + i + 1) + 1, lpcwPath + i + 1);
				
				// Should be -1 from the previous value.
				szPath = wcslen(lpcwPath); 

				// ^^ => '^'
				// ^& => '&'
				// To pass the ^ or & chars.
				if (i + 1 < szPath && (lpcwPath[i + 1] == wcEscapeChar || lpcwPath[i + 1] == wcApmpersendChar))
				{
					i++;
				}
			}
		}

		bPathIsValid = TRUE;
	}

	return bPathIsValid;
}

/***********************************************************************************
 *	HandleSpecialCharacters:
 *		Handles the special characters in a path
 *
 *	@param	lpcwPath		A pointer to the Path.
 *
 *	@return True if there is a forbidden character in the path, False otherwise.
 ***********************************************************************************/
BOOL HandleSpecialCharacters(LPCWSTR lpcwPath)
{
	if (ContainsForbiddenChars(lpcwPath))
	{
		return FALSE;
	}

	return EscapeCharactersHandling(lpcwPath);
}
