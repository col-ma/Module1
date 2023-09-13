#include <Windows.h>
#include <Shlwapi.h>
#include <winsafer.h>

#include "FileHandling.h"
#include "PathParser.h"
#include "PathTypes.h"
#include "ExecutableFileHandling.h"


/********************
 *	Path Validation	*
 ********************/
// Gets a Path and determines if the path is evil.
BOOL IsPathEvil(LPCWSTR filePath);

// Validates inputed path.
PATHS_T PathIsValid(LPCWSTR filePath);