#include <windows.h>
#include <lmshare.h>

#include "PathParser.h"
#include "PathTypes.h"

// Removes backslash at the end of the path string
BOOL RemoveBackslashAtEnd(WCHAR* lpcwPath);

// Using the seperated server name and the share name it fetches the local path
PATHS_T GetLocalPathByUncServerArndSharePath(LPCWSTR lpcwServerName, LPCWSTR lpcwShare, LPCWSTR lpcwLocalPath);

// Gets a local path from a UNC path.
DWORD GetLocalPathFromUncPath(LPCWSTR lpcwPath);