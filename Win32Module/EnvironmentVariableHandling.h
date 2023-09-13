#ifndef __ENVIRONMENT_VARIABLE_HANDLING
#define __ENVIRONMENT_VARIABLE_HANDLING

#include <Windows.h>

/********************************************
 *	Environment Variable String Operations	*
 ********************************************/
// Possible states of the existance of an environment variable in a given path.
typedef enum
{
	NOT_AN_ENVIRONMENT_VARIABLE,
	REGULAR_ENVIRONMENT_VARIABLE,
	CUT_A_SUBSTRING,
	CHANGE_EACH_SUBSTRING_WITH_STRING
} ENVIRONMENT_VARIABLE_OPERATRION_T;

// Checks if a given string is a number.
BOOL IsNumber(LPCWSTR lpNum);

// Checks and executes if a given environment variable operation expression is a substring.
BOOL EnvVariableSubstringOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwOperation, WCHAR* lpcwEnvValue);

// Changes a given substring to another substring.
BOOL ChangeSubstring(UINT uSubStrStart, UINT uSubStrEnd, WCHAR* lpcwPath, LPCWSTR lpcwNewSubStr);

// Checks and executes if a given environment variable operation expression is string edit.
BOOL EnvVariableEditOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwOperation, WCHAR* lpcwEnvValue);

// Determines the environment variable operation and executes it.
ENVIRONMENT_VARIABLE_OPERATRION_T DetermineEnvVarOP(LPCWSTR lpcwEnvVar, LPCWSTR lpcwEnvValue);

// Searches for an environment variable expression in a given path from the Initial index.
BOOL FindEnvironmentVariable(WCHAR* lpcwPath, UINT uInitialIndex);

// Expends all the environment variables in the given path.
BOOL ExpendAllEnvironmentVariables(WCHAR* lpcwPath);

#endif //__ENVIRONMENT_VARIABLE_HANDLING