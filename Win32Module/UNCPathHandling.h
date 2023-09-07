#include <windows.h>
#include <lmshare.h>

#include "PathParser.h"

DWORD GetLocalPathFromUncPath(LPCWSTR lpcwPath);
DWORD GetLocalPathByUncServerArndSharePath(LPCWSTR lpcwServerName, LPCWSTR lpcwShare, LPCWSTR lpcwLocalPath);