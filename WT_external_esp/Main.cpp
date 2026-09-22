#include "Game/GameDataSource.h"

#include "Memory/MemoryManager.h"

#include "Utils/ExternMath.h"
#include "Utils/WindowUtils.h"

#include "Overlay/Overlay.h"

#include "ESP/UnitESP.h"

#include <Windows.h>
#include <iostream>
#include <cstdint>


int main()
{
    // ========================================================
    // 1. MEMORY
    // ========================================================

    MemoryManager memory;

    if (!memory.Initialize(
        L"aces.exe",
        L"aces.exe"))
    {
        std::cout
            << "[MAIN] Memory initialization failed\n";

        return 1;
    }


    std::cout
        << "[MAIN] PID: "
        << memory.GetProcessId()
        << '\n';


    std::cout
        << "[MAIN] BaseAddress: 0x"
        << std::hex
        << memory.GetBaseAddress()
        << std::dec
        << '\n';


    // ========================================================
    // 2. FIND GAME WINDOW
    // ========================================================

    HWND hwndGame =
        WindowUtils::FindMainWindow(
            memory.GetProcessId()
        );


    if (!hwndGame)
    {
        std::cout
            << "[MAIN] Game window not found\n";

        return 1;
    }


    std::cout
        << "[MAIN] HWND: 0x"
        << std::hex
        << reinterpret_cast<std::uintptr_t>(
            hwndGame
            )
        << std::dec
        << '\n';


    // ========================================================
    // 3. GET GAME CLIENT RECT
    // ========================================================

    RECT overlayRect{};


    if (!WindowUtils::GetClientRectOnScreen(
        hwndGame,
        overlayRect))
    {
        std::cout
            << "[MAIN] Failed to get client rect\n";

        return 1;
    }


    const int overlayWidth =
        overlayRect.right -
        overlayRect.left;


    const int overlayHeight =
        overlayRect.bottom -
        overlayRect.top;


    std::cout
        << "[MAIN] Client size: "
        << overlayWidth
        << " x "
        << overlayHeight
        << '\n';


    // ========================================================
    // 4. GAME DATA
    // ========================================================

    GameDataSource gameData(memory);


    // ========================================================
    // 5. ESP
    // ========================================================

    UnitESP unitESP;


    // ========================================================
    // 6. OVERLAY
    // ========================================================

    Overlay overlay;


    if (!overlay.Initialize(
        hwndGame,
        overlayRect,
        SW_SHOWNOACTIVATE))
    {
        std::cout
            << "[MAIN] Overlay initialization failed\n";

        return 1;
    }


    // ========================================================
    // 7. RENDER LOOP
    // ========================================================

    bool running = true;


    while (running)
    {
        // ----------------------------------------------------
        // WINDOWS MESSAGES
        // ----------------------------------------------------

        MSG msg{};


        while (PeekMessage(
            &msg,
            nullptr,
            0,
            0,
            PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }


            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }


        if (!running)
            break;


        // ----------------------------------------------------
        // OPTIONAL:
        // если окно игры закрыли
        // ----------------------------------------------------

        if (!IsWindow(hwndGame))
        {
            std::cout
                << "[MAIN] Game window closed\n";

            break;
        }


        // ----------------------------------------------------
        // BEGIN FRAME
        // ----------------------------------------------------

        overlay.BeginRender();


        // ====================================================
        // READ GAME DATA
        // ====================================================

        Vector3 localPosition{};
        Matrix4x4 viewMatrix{};


        const bool positionOk =
            gameData.GetLocalPosition(
                localPosition
            );


        const bool matrixOk =
            gameData.GetViewMatrix(
                viewMatrix
            );


        const std::uintptr_t localUnitAddress =
            gameData.GetLocalUnitAddress();


        // —ейчас используем именно List2,
        // потому что в нЄм нашли наземную технику
        // и локальный Unit.
        const auto units =
            gameData.GetUnitsList3();


        // ====================================================
        // ESP RENDER
        // ====================================================

        if (positionOk &&
            matrixOk &&
            localUnitAddress != 0)
        {
            const ImVec2 displaySize =
                ImGui::GetIO().DisplaySize;


            unitESP.Render(
                units,
                localUnitAddress,
                localPosition,
                viewMatrix,
                displaySize.x,
                displaySize.y
            );
        }


        // ====================================================
        // DEBUG CONSOLE
        // ====================================================

        static ULONGLONG lastDebugTime = 0;

        const ULONGLONG now =
            GetTickCount64();


        if (now - lastDebugTime >= 1000)
        {
            lastDebugTime = now;


            std::cout
                << "[DEBUG]"
                << " PositionOK="
                << std::boolalpha
                << positionOk

                << " | MatrixOK="
                << matrixOk

                << " | LocalUnit=0x"
                << std::hex
                << localUnitAddress
                << std::dec

                << " | Units="
                << units.size();


            if (positionOk)
            {
                std::cout
                    << " | LocalPos=("
                    << localPosition.x
                    << ", "
                    << localPosition.y
                    << ", "
                    << localPosition.z
                    << ")";
            }


            std::cout << '\n';
        }


        // ----------------------------------------------------
        // END FRAME
        // ----------------------------------------------------

        overlay.EndRender();
    }


    // ========================================================
    // 8. CLEANUP
    // ========================================================

    overlay.Shutdown();


    return 0;
}