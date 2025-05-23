#include "library.h"

#include <iostream>
#include "MinHook/include/MinHook.h"

void setupHook() {
    MessageBox(NULL, "Hello World!", "Hello World!", MB_OK);
}
