#pragma once

#include <Windows.h>

namespace WindowUtils
{
    HWND FindMainWindow(DWORD processId);

    bool GetClientRectOnScreen(
        HWND hwnd,
        RECT& rect
    );
}