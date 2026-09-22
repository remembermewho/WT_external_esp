// GameDataSource - реальные данные
#include "Game/GameDataSource.h"

#include "Memory/MemoryManager.h"

#include "Utils/ExternMath.h"
#include "Utils/WindowUtils.h"

#include "Overlay/Overlay.h"

#include <iostream>
#include <vector>


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
            << "Memory initialization failed\n";

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
            << "Game window not found\n";

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
    // 3. GET WINDOW RECT
    // ========================================================

    RECT overlayRect{};


    if (!WindowUtils::GetClientRectOnScreen(
        hwndGame,
        overlayRect))
    {
        std::cout
            << "Failed to get client rect\n";

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
    // 5. OVERLAY
    // ========================================================

    Overlay overlay;


    if (!overlay.Initialize(
        hwndGame,
        overlayRect,
        SW_SHOWNOACTIVATE))
    {
        std::cout
            << "Overlay initialization failed\n";

        return 1;
    }


    // ========================================================
    // 6. RENDER LOOP
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
        // BEGIN FRAME
        // ----------------------------------------------------

        overlay.BeginRender();


        // ----------------------------------------------------
        // READ LOCAL DATA
        // ----------------------------------------------------

        Vector3 worldPosition{};
        Matrix4x4 viewMatrix{};
        Vector2 screenPosition{};


        const bool positionOk =
            gameData.GetLocalPosition(
                worldPosition
            );


        const bool matrixOk =
            gameData.GetViewMatrix(
                viewMatrix
            );


        // ----------------------------------------------------
        // READ ALL 3 UNIT LISTS
        // ----------------------------------------------------

        const auto units1 =
            gameData.GetUnitsList1();


        const auto units2 =
            gameData.GetUnitsList2();


        const auto units3 =
            gameData.GetUnitsList3();

        const std::uintptr_t localUnitAddress =
            gameData.GetLocalUnitAddress();

        // ----------------------------------------------------
        // LOCAL PLAYER WORLD -> SCREEN
        // ----------------------------------------------------

        bool visible = false;


        if (positionOk && matrixOk)
        {
            const ImVec2 displaySize =
                ImGui::GetIO().DisplaySize;


            visible =
                Math::WorldToScreen(
                    worldPosition,
                    screenPosition,
                    viewMatrix,
                    displaySize.x,
                    displaySize.y
                );


            if (visible)
            {
                ImGui::GetBackgroundDrawList()
                    ->AddCircleFilled(
                        ImVec2(
                            screenPosition.x,
                            screenPosition.y
                        ),
                        8.0f,
                        IM_COL32(
                            255,
                            0,
                            0,
                            255
                        )
                    );
            }
        }
        
        int drawnUnits = 0;

        if (matrixOk && localUnitAddress != 0)
        {
            const ImVec2 displaySize =
                ImGui::GetIO().DisplaySize;

            for (const UnitInfo& unit : units2)
            {
                // Не рисуем собственную технику
                if (unit.id == localUnitAddress)
                    continue;

                Vector2 unitScreen{};

                const bool unitVisible =
                    Math::WorldToScreen(
                        unit.position,
                        unitScreen,
                        viewMatrix,
                        displaySize.x,
                        displaySize.y
                    );

                if (!unitVisible)
                    continue;

                ImGui::GetBackgroundDrawList()
                    ->AddCircleFilled(
                        ImVec2(
                            unitScreen.x,
                            unitScreen.y
                        ),
                        6.0f,
                        IM_COL32(
                            255,
                            255,
                            0,
                            255
                        )
                    );
                ++drawnUnits;
            }
        }


        // ----------------------------------------------------
        // DEBUG CONSOLE
        // ----------------------------------------------------

        static ULONGLONG lastDebugTime = 0;


        const ULONGLONG now =
            GetTickCount64();


        if (now - lastDebugTime >= 500)
        {
            lastDebugTime = now;


            // =================================================
            // MAIN INFO
            // =================================================

            std::cout
                << "\nWORLD: "
                << "X=" << worldPosition.x
                << " Y=" << worldPosition.y
                << " Z=" << worldPosition.z

                << " | PositionOK="
                << std::boolalpha
                << positionOk

                << " | MatrixOK="
                << matrixOk

                << " | W2S="
                << visible

                << " | List1="
                << units1.size()

                << " | List2="
                << units2.size()

                << " | List3="
                << units3.size()

                << " | DrawnUnits="
                << drawnUnits;


            if (visible)
            {
                std::cout
                    << " | SCREEN: X="
                    << screenPosition.x

                    << " Y="
                    << screenPosition.y;
            }


            std::cout << '\n';


            // =================================================
            // HELPER FOR PRINTING UNIT LIST
            // =================================================

            auto printUnits =
                [](
                    const char* listName,
                    const std::vector<UnitInfo>& units)
                {
                    std::cout
                        << "\n========== "
                        << listName
                        << " ==========\n";


                    for (std::size_t i = 0;
                        i < units.size();
                        ++i)
                    {
                        const UnitInfo& unit =
                            units[i];


                        std::cout
                            << "[" << i << "]"

                            << " Address=0x"
                            << std::hex
                            << unit.id
                            << std::dec

                            << " | Team="
                            << unit.team

                            << " | Type="
                            << unit.type

                            << " | State="
                            << unit.state

                            << " | Pos=("
                            << unit.position.x
                            << ", "
                            << unit.position.y
                            << ", "
                            << unit.position.z
                            << ")"

                            << '\n';
                    }


                    std::cout
                        << "============================\n";
                };


            // =================================================
            // PRINT ALL LISTS
            // =================================================

            printUnits(
                "LIST 1",
                units1
            );


            printUnits(
                "LIST 2",
                units2
            );


            printUnits(
                "LIST 3",
                units3
            );
        }


        // ----------------------------------------------------
        // END FRAME
        // ----------------------------------------------------

        overlay.EndRender();
    }


    // ========================================================
    // 7. CLEANUP
    // ========================================================

    overlay.Shutdown();


    return 0;
}