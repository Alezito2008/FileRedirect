#include "pch.h"
#include <direct.h>
#include <string>

const char* GetCurrentDir() {
	static char buffer[MAX_PATH];
	if (_getcwd(buffer, MAX_PATH) == NULL) return nullptr;
	return buffer;
}

std::wstring splitLast(const std::wstring& str, const std::wstring& delimiter) {
    size_t start;
    if ((start = str.find(delimiter)) != std::wstring::npos) {
        return str.substr(start + delimiter.length());
    }
    return str;
}
