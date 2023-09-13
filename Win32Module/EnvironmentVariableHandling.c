#include "EnvironmentVariableHandling.h"

/*******************************************************************
 *	IsNumber:
 *		Checks if the given string is of a number.
 *
 *	@param	lpNum	A pointer to the assumed number.
 *
 *	@rerurn	If the string is a number then True, False otherwise.	
 *******************************************************************/
BOOL IsNumber(LPCWSTR lpNum)
{
	BOOL bIsNumber = TRUE;

	for (UINT i = 0; i < wcslen(lpNum); i++)
	{
		// If character is a digit, or if its the first index a minus is also acceptable.
		bIsNumber &= (iswdigit(lpNum[i]) != 0) || (i == 0 && lpNum[0] == L'-');
	}

	return bIsNumber;
}

/*******************************************************************************
 *	EnvVariableSubstringOP:
 *		Checks if the given environment variableand the pressumed operation on 
 *		it are syntatically valid, and if they are it outputs the substring.
 *
 *	@param	lpcwEnvVarOperation	A pointer to the string which pressumed to be 
 *									the environment variable.
 *	@param	lpcwOperation		A pointer to the string which pressumed to be 
 *									the operation on the environment variable.
 *	@param	lpcwEnvValue		An output of the string with the value of the 
 *									operation.
 *
 *	@rerurn	If the opration was conducted successfully with no syntax issues	
 *				then True, False otherwise.
 *******************************************************************************/
BOOL EnvVariableSubstringOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwOperation, WCHAR * lpcwEnvValue)
{
	int iFirstIndex = 0;
	int iSecondIndex = 0;

	WCHAR lpcwEnvValueCopy[MAX_PATH] = { 0 };

	WCHAR* lpcwFirstNum;
	WCHAR* lpcwSecondNum = NULL;
	WCHAR* lpcwTemp;

	if (GetEnvironmentVariable(lpcwEnvVarOperation, lpcwEnvValue, MAX_PATH) == 0) {
		return FALSE;
	}

	wcscpy_s(lpcwEnvValueCopy, wcslen(lpcwEnvValue) + 1, lpcwEnvValue);

	// If the next character is a '~' than its a substring operation.
	// ~ handling
	// if no number after ~ it a try it as aregular envvar.
	if (lpcwOperation == NULL || lpcwOperation[0] != L'~')
	{
		return FALSE;
	}

	lpcwTemp = wcstok_s(lpcwOperation, L"~", &lpcwFirstNum);
	lpcwFirstNum = wcstok_s(lpcwTemp, L",", &lpcwSecondNum);

	// check if numbers if they are get the sub string out of it.
	// This is a valid substring 
	if (lpcwFirstNum[0] == L'\0')
	{
		iFirstIndex = 0;
	}
	else if (!IsNumber(lpcwFirstNum))
	{
		return FALSE;
	}

	iFirstIndex = _wtoi(lpcwFirstNum);

	if (lpcwSecondNum[0] == L'\0')
	{
		iSecondIndex = wcslen(lpcwEnvValue);
	}
	else if (IsNumber(lpcwSecondNum))
	{
		iSecondIndex = _wtoi(lpcwSecondNum);
	}
	else
	{
		return FALSE;
	}
	
	iFirstIndex += (iFirstIndex < 0) ? wcslen(lpcwEnvValue) : 0;
	iSecondIndex += (iSecondIndex < 0) ? wcslen(lpcwEnvValue) : 0;

	if ((iFirstIndex > iSecondIndex) || iFirstIndex > wcslen(lpcwEnvValue) || iSecondIndex > wcslen(lpcwEnvValue))
	{
		return FALSE;
	}
	
	memcpy_s(lpcwEnvValue, MAX_PATH, lpcwEnvValueCopy + iFirstIndex, iSecondIndex + 1);
	lpcwEnvValue[iSecondIndex] = L'\0';

	return TRUE;
}

/*******************************************************************************
 *	ChangeSubstring:
 *		Replaces a substring in a given path with a new substring.
 *
 *	@param	uSubStrStart	The starting index of the substring to be replaced.
 *	@param	uSubStrEnd		The end index of the substring to be replaced.
 *	@param	lpcwPath		A pointer to the path.
 *	@param	lpcwNewSubStr	A pointer to the new substring to be inserted to 
 *								the path, instead the old one.
 *
 *	@rerurn	If the opration was conducted successfully then True, 
 *				False otherwise.
 *******************************************************************************/
BOOL ChangeSubstring(UINT uSubStrStart, UINT uSubStrEnd, WCHAR* lpcwPath, LPCWSTR lpcwNewSubStr)
{
	LPCWSTR lpcwTempRestOfPath[MAX_PATH] = { 0 };

	// Now only assigned it between the indexes the value of the environment variable:
	// 1. Copy all the rest of the path to temp variable.
	// 2. Copy the new env value to the path.
	// 3. copy the temp variable to the path.
	wcscpy_s(lpcwTempRestOfPath, wcslen(lpcwPath + uSubStrEnd + 1) + 1, lpcwPath + uSubStrEnd + 1);
	lpcwPath[uSubStrStart] = L'\0';

	wcscat_s(lpcwPath, wcslen(lpcwPath) + wcslen(lpcwNewSubStr) + 1, lpcwNewSubStr);
	wcscat_s(lpcwPath, wcslen(lpcwPath) + wcslen(lpcwTempRestOfPath) + 1, lpcwTempRestOfPath);

	return TRUE;
}

/*******************************************************************************
 *	EnvVariableEditOP:
 *		Checks if the given environment variableand the pressumed operation on
 *		it are syntatically valid, and if they are it outputs 
 *		the edited string.
 *
 *	@param	lpcwEnvVarOperation	A pointer to the string which pressumed to be
 *									the environment variable.
 *	@param	lpcwOperation		A pointer to the string which pressumed to be
 *									the operation on the environment variable.
 *	@param	lpcwEnvValue		An output of the string with the value of the
 *									operation.
 *
 *	@rerurn	If the opration was conducted successfully with no syntax issues
 *				then True, False otherwise.
 *******************************************************************************/
BOOL EnvVariableEditOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwOperation, WCHAR * lpcwEnvValue)
{
	WCHAR lpcwEnvValueCopy[MAX_PATH] = { 0 };

	WCHAR* lpcwSubStrToRemove = NULL;
	WCHAR* lpcwSubStrToReplaceWith = NULL;

	if (GetEnvironmentVariable(lpcwEnvVarOperation, lpcwEnvValue, MAX_PATH) == 0) {
		return FALSE;
	}

	wcscpy_s(lpcwEnvValueCopy, wcslen(lpcwEnvValue) + 1, lpcwEnvValue);

	if (lpcwOperation == NULL || wcsstr(lpcwOperation, L"=") == 0 || lpcwOperation[0] == L'=')
	{
		return FALSE;
	}

	lpcwSubStrToRemove = wcstok_s(lpcwOperation, L"=", &lpcwSubStrToReplaceWith);


	if (lpcwSubStrToRemove[0] == L'\0')
	{
		return FALSE;
	}

	UINT uCurrentIndex = 0;
	LPCWSTR lpcwSubstring = wcswcs(lpcwEnvValue, lpcwSubStrToRemove);

	while(lpcwSubstring != NULL)
	{
		uCurrentIndex = lpcwSubstring - lpcwEnvValue;

		ChangeSubstring(uCurrentIndex, uCurrentIndex+ wcslen(lpcwSubStrToRemove) - 1, lpcwEnvValue, lpcwSubStrToReplaceWith);

		uCurrentIndex += wcslen(lpcwSubStrToReplaceWith) + 1;

		lpcwSubstring = wcswcs(lpcwEnvValue, lpcwSubStrToRemove);
	}

	return TRUE;
}

/*******************************************************************************
 *	DetermineEnvVarOP:
 *		Checks if the given environment variableis an operation on
 *		it, and if they are it outputs the environment variable 
 *		expression value.
 *
 *	@param	lpcwEnvVarOperation	A pointer to the string which pressumed to be
 *									the environment variable.
 *	@param	lpcwEnvValue		An output of the string with the value of the
 *									operation.
 *
 *	@rerurn	Output which operation was conducted if anything, 
 *				according to the ENVIRONMENT_VARIABLE_OPERATRION_T enum
 *******************************************************************************/
ENVIRONMENT_VARIABLE_OPERATRION_T DetermineEnvVarOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwEnvValue)
{
	UINT szEnvVarOperation = wcslen(lpcwEnvVarOperation);
	WCHAR lpcwEnvVar[MAX_PATH] = { 0 };
	WCHAR* lpcwTempEnvVar[MAX_PATH] = { 0 };

	WCHAR* lpcwOpValues = NULL;
	WCHAR lpcwEnvVarOperationCopy[MAX_PATH] = { 0 };

	wcscpy_s(lpcwEnvVarOperationCopy, wcslen(lpcwEnvVarOperation) + 1, lpcwEnvVarOperation);

	// Check if the string can be splitten by ':', if no its a regular environment variable.
	// if the last symbol is ':' than there is no operation.
	if (wcsstr(lpcwEnvVarOperation, L":") != 0 && lpcwEnvVarOperation[szEnvVarOperation - 1] != L':')
	{
		wcstok_s(lpcwEnvVarOperationCopy, L":", &lpcwOpValues);
		wcscpy_s(lpcwTempEnvVar, wcslen(lpcwOpValues) + 1, lpcwOpValues);
	}

	if(EnvVariableSubstringOP(lpcwEnvVarOperationCopy, lpcwOpValues, lpcwEnvValue))
	{
		return CUT_A_SUBSTRING;
	}
	else if (EnvVariableEditOP(lpcwEnvVarOperationCopy, lpcwOpValues, lpcwEnvValue))
	{
		return CHANGE_EACH_SUBSTRING_WITH_STRING;
	}
	else
	{
		wcscpy_s(lpcwEnvVar, wcslen(lpcwEnvVarOperation) + 1, lpcwEnvVarOperation);

		if (GetEnvironmentVariable(lpcwEnvVar, lpcwEnvValue, MAX_PATH) == 0) 
		{
			return NOT_AN_ENVIRONMENT_VARIABLE;
		}
	}

	return REGULAR_ENVIRONMENT_VARIABLE;
}

/***********************************************************************
 *	FindEnvironmentVariable:
 *		Searches for an environment variable in a path, 
 *		from the specified index.
 *
 *	@param	lpcwPath		A pointer to the path.
 *	@param	uInitialIndex	The index to start the search from.
 *
 *	@rerurn	The index of where the pressumed envronment variable ends,	
 *				or the EOS if it got to the end
 ***********************************************************************/
UINT FindEnvironmentVariable(WCHAR* lpcwPath, UINT uInitialIndex)
{
	UINT uStartOfEnvVar = uInitialIndex;
	UINT uEndOfEnvVar = uInitialIndex;

	BOOL bFoundStart = FALSE;
	BOOL bFoundEnd = FALSE;
	BOOL bGotToEndOfString = FALSE;

	WCHAR lpcwEnvVar[MAX_PATH] = { 0 };
	WCHAR lpcwEnvValue[MAX_PATH] = { 0 };

	for (; uStartOfEnvVar < wcslen(lpcwPath) && !bFoundStart && !bGotToEndOfString; uStartOfEnvVar++)
	{
		bFoundStart = (lpcwPath[uStartOfEnvVar] == L'%');
		bGotToEndOfString = (lpcwPath[uStartOfEnvVar + 1] == L'\0');
		if (bFoundStart) uStartOfEnvVar--;
	}

	// Make sure the last index is the one that is returned.
	uEndOfEnvVar = uStartOfEnvVar + 1;

	if (bFoundStart && uStartOfEnvVar < wcslen(lpcwPath))
	{
		for (; uEndOfEnvVar < wcslen(lpcwPath) && !bFoundEnd && !bGotToEndOfString; uEndOfEnvVar++)
		{
			bFoundEnd = (lpcwPath[uEndOfEnvVar] == L'%');
			bGotToEndOfString = (lpcwPath[uEndOfEnvVar + 1] == L'\0');
			if (bFoundEnd) uEndOfEnvVar--;
		}

		if (bFoundEnd)
		{
			memcpy(lpcwEnvVar, lpcwPath + uStartOfEnvVar + 1, (uEndOfEnvVar - uStartOfEnvVar - 1) * sizeof(WCHAR));
			lpcwEnvVar[uEndOfEnvVar - uStartOfEnvVar] = L'\0';

			if (DetermineEnvVarOP(lpcwEnvVar, lpcwEnvValue) != NOT_AN_ENVIRONMENT_VARIABLE) {
				ChangeSubstring(uStartOfEnvVar, uEndOfEnvVar, lpcwPath, lpcwEnvValue);
				uEndOfEnvVar = uStartOfEnvVar + wcslen(lpcwEnvValue);
			}
		}
	}

	return uEndOfEnvVar;
}

/***************************************************************************
 *	ExpendAllEnvironmentVariables:
 *		Expands all the valid environment variable expressions in a path.		
 *
 *	@param	lpcwPath	A pointer to the path.
 *
 *	@rerurn	True if the expention was successful.
 ***************************************************************************/
BOOL ExpendAllEnvironmentVariables(WCHAR* lpcwPath)
{
	UINT uInitialIndex = 0;
	while (lpcwPath[uInitialIndex] != L'\0') 
	{
		uInitialIndex = FindEnvironmentVariable(lpcwPath, uInitialIndex);
	}

	return TRUE;
}