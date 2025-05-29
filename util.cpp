#include "pch.h"
#include <direct.h>

const char* GetCurrentDir() {
	static char buffer[MAX_PATH];
	if (_getcwd(buffer, MAX_PATH) == NULL) return nullptr;
	return buffer;
}
