// MockGameDataSource - это тестовые данные
#include "Game/GameDataSource.h" // GameDataSource - реальные данные
#include "ESP/ConsoleESP.h"
#include "Memory/MemoryManager.h"
#include "Utils/ExternMath.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

int main()
{
    MemoryManager memory;

    if (!memory.Initialize(L"aces.exe", L"aces.exe"))
    {
        std::cout << "Initialization failed\n";

        return 1;
    }

    std::cout
        << "[MAIN] BaseAddress: 0x"
        << std::hex
        << memory.GetBaseAddress()
        << std::dec
        << '\n';

    GameDataSource gameData(memory);
    ConsoleESP esp;

      
    while (true)
    {   
        
        system("cls");

        Vector3 localPosition{};
        Matrix4x4 matrix{};
        Vector2 screenPosition{};

        constexpr float screenWidth = 1920.0f;
        constexpr float screenHeight = 1080.0f;

        std::cout
            << "========================================\n"
            << "            NEW UPDATE\n"
            << "========================================\n";

        const bool positionOk =
            gameData.GetLocalPosition(localPosition);

        const bool matrixOk =
            gameData.GetViewMatrix(matrix);

        if (positionOk)
        {
            std::cout
                << "\nWORLD POSITION:\n"
                << "X = " << localPosition.x << '\n'
                << "Y = " << localPosition.y << '\n'
                << "Z = " << localPosition.z << '\n';
        }
        else
        {
            std::cout << "\nWORLD POSITION: READ FAILED\n";
        }

        if (positionOk && matrixOk)
        {
            const bool visible =
                Math::WorldToScreen(
                    localPosition,
                    screenPosition,
                    matrix,
                    screenWidth,
                    screenHeight
                );

            if (visible)
            {
                std::cout
                    << "\nSCREEN POSITION:\n"
                    << "X = " << screenPosition.x << '\n'
                    << "Y = " << screenPosition.y << '\n';

                const bool onScreen =
                    screenPosition.x >= 0.0f &&
                    screenPosition.x <= screenWidth &&
                    screenPosition.y >= 0.0f &&
                    screenPosition.y <= screenHeight;

                std::cout
                    << "On screen = "
                    << std::boolalpha
                    << onScreen
                    << '\n';
            }
            else
            {
                std::cout
                    << "\nWorldToScreen: point is behind camera\n";
            }
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(2)
        );
	}
        

        
    

    return 0;
}