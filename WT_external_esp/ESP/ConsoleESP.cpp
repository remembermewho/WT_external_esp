#include "ConsoleESP.h"

#include <iostream>

void ConsoleESP::Update(IGameDataSource& dataSource)
{
    const auto units = dataSource.GetUnits();

    std::cout << "Units: " << units.size() << "\n\n";

    for (const auto& unit : units)
    {
        std::cout
            << "ID: " << unit.id
            << " | Name: " << unit.name
            << " | Team: " << unit.team
            << " | State: " << unit.state
            << " | Position: "
            << "X=" << unit.position.x
            << " Y=" << unit.position.y
            << " Z=" << unit.position.z
            << '\n';
    }
}