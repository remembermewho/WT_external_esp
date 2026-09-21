// MockGameDataSource - это тестовые данные
#include "Game/GameDataSource.h" // GameDataSource - реальные данные
#include "ESP/ConsoleESP.h"
#include "Memory/MemoryManager.h"
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
        Vector3 cameraPosition{};

        std::cout
            << "\n========================================\n"
            << "            NEW UPDATE\n"
            << "========================================\n";

        //esp.Update(gameData);

        if (gameData.GetLocalPosition(localPosition))
        {
            std::cout
                << "LOCAL POSITION:\n"
                << "X = " << localPosition.x << '\n'
                << "Y = " << localPosition.y << '\n'
                << "Z = " << localPosition.z << '\n';
        }

        std::cout << '\n';

        if (gameData.GetCameraPosition(cameraPosition))
        {
            std::cout
                << "CAMERA POSITION:\n"
                << "X = " << cameraPosition.x << '\n'
                << "Y = " << cameraPosition.y << '\n'
                << "Z = " << cameraPosition.z << '\n';
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(2)
        );
    }

    return 0;
}