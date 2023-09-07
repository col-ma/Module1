#include <stdio.h> // scanf, printf

#include "SecureUserInterface.h"

/****************
 *	Unit Tests	*
 ****************/
int TestEscapeCharactersHandling();

PATHS_T TestSpecificSrtringAssembleFullPath(LPCWSTR lpcwFilePath);

int TestAssembleFullPath();

int TestFileExists();

int TestIsFileEvil();

// Tests the deletion of a file.
int TestDeleteTheFile();

// Tests the creation of a file hierarchy.
int TestCreateFileHierarchy();

// Tests the parsing of the path.
int TestGetAndConcatenateSteps();

// Entry point.
int main(int argc, char* argv[]);

