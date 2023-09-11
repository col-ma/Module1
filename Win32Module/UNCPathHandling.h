#include <windows.h>
#include <lmshare.h>

#include "PathParser.h"
#include "PathTypes.h"

BOOL RemoveBackslashAtEnd(WCHAR* lpcwPath);
DWORD GetLocalPathFromUncPath(LPCWSTR lpcwPath);
PATHS_T GetLocalPathByUncServerArndSharePath(LPCWSTR lpcwServerName, LPCWSTR lpcwShare, LPCWSTR lpcwLocalPath);