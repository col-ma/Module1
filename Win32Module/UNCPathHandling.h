#include <windows.h>
#include <lmshare.h>

#include "PathParser.h"

BOOL RemoveBackslashAtEnd(WCHAR* lpcwPath);
DWORD GetLocalPathFromUncPath(LPCWSTR lpcwPath);
DWORD GetLocalPathByUncServerArndSharePath(LPCWSTR lpcwServerName, LPCWSTR lpcwShare, LPCWSTR lpcwLocalPath);