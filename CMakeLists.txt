cmake_minimum_required(VERSION 3.31)
project(FileRedirect)

set(CMAKE_CXX_STANDARD 20)


add_library(FileRedirect SHARED
        dllmain.cpp
        library.cpp

        # Archivos fuente de MinHook
        minhook/src/buffer.c
        minhook/src/hook.c
        minhook/src/trampoline.c
        minhook/src/hde/hde32.c
        minhook/src/hde/hde64.c
)

target_include_directories(FileRedirect PRIVATE
        minhook/include      # para MinHook.h
        minhook/src          # para buffer.h, hook.c incluye src headers
        minhook/src/hde      # para hde headers
)

target_link_libraries(FileRedirect PRIVATE
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
)