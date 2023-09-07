#include <Windows.h>

/********************************************
 *	Environment Variable String Operations	*
 ********************************************/
typedef enum
{
	NOT_AN_ENVIRONMENT_VARIABLE,
	REGULAR_ENVIRONMENT_VARIABLE,
	CUT_A_SUBSTRING,
	CHANGE_EACH_SUBSTRING_WITH_STRING
} ENVIRONMENT_VARIABLE_OPERATRION_T;

BOOL IsNumber(LPCWSTR lpNum);

BOOL EnvVariableSubstringOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwOperation, WCHAR* lpcwEnvValue);

BOOL ChangeSubstring(UINT uSubStrStart, UINT uSubStrEnd, WCHAR* lpcwPath, LPCWSTR lpcwNewSubStr);

BOOL EnvVariableEditOP(LPCWSTR lpcwEnvVarOperation, LPCWSTR lpcwOperation, WCHAR* lpcwEnvValue);

ENVIRONMENT_VARIABLE_OPERATRION_T DetermineEnvVarOP(LPCWSTR lpcwEnvVar, LPCWSTR lpcwEnvValue);

BOOL FindEnvironmentVariable(WCHAR* lpcwPath, UINT uInitialIndex);

BOOL ExpendAllEnvironmentVariables(WCHAR* lpcwPath);
