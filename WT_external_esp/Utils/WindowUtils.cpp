#include "WindowUtils.h"

namespace
{
    struct EnumWindowData
    {
        DWORD processId{};
        HWND hwnd{};
    };

    BOOL CALLBACK EnumWindowsCallback(
        HWND hwnd,
        LPARAM lParam)
    {
        auto* data =
            reinterpret_cast<EnumWindowData*>(lParam);

        DWORD windowProcessId = 0;

        GetWindowThreadProcessId(
            hwnd,
            &windowProcessId
        );

        if (windowProcessId != data->processId)
            return TRUE;

        // Нас интересует только видимое окно.
        if (!IsWindowVisible(hwnd))
            return TRUE;

        // Пропускаем owned windows.
        if (GetWindow(hwnd, GW_OWNER) != nullptr)
            return TRUE;

        RECT clientRect{};

        if (!GetClientRect(hwnd, &clientRect))
            return TRUE;

        const int width =
            clientRect.right - clientRect.left;

        const int height =
            clientRect.bottom - clientRect.top;

        // Пропускаем технические окна размером 0x0.
        if (width <= 0 || height <= 0)
            return TRUE;

        data->hwnd = hwnd;

        return FALSE;
    }
}


HWND WindowUtils::FindMainWindow(
    DWORD processId)
{
    EnumWindowData data{};

    data.processId = processId;

    EnumWindows(
        EnumWindowsCallback,
        reinterpret_cast<LPARAM>(&data)
    );

    return data.hwnd;
}


bool WindowUtils::GetClientRectOnScreen(
    HWND hwnd,
    RECT& rect)
{
    if (!hwnd)
        return false;

    RECT clientRect{};

    if (!GetClientRect(
        hwnd,
        &clientRect))
    {
        return false;
    }

    POINT topLeft
    {
        clientRect.left,
        clientRect.top
    };

    POINT bottomRight
    {
        clientRect.right,
        clientRect.bottom
    };

    if (!ClientToScreen(
        hwnd,
        &topLeft))
    {
        return false;
    }

    if (!ClientToScreen(
        hwnd,
        &bottomRight))
    {
        return false;
    }

    rect.left = topLeft.x;
    rect.top = topLeft.y;

    rect.right = bottomRight.x;
    rect.bottom = bottomRight.y;

    return true;
}