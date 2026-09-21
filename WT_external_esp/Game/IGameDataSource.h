#pragma once //Интерфейс для получения объектов
#include <vector>
#include "GameTypes.h"

class IGameDataSource
{
public:
    virtual ~IGameDataSource() = default;

    virtual std::vector<UnitInfo> GetUnits() = 0;
};