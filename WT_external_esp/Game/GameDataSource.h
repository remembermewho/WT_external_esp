#pragma once

#include <vector>
#include <cstdint>

#include "IGameDataSource.h"
#include "GameTypes.h"
#include "../Memory/MemoryManager.h"

class GameDataSource : public IGameDataSource
{
public:
    explicit GameDataSource(MemoryManager& memory);

    std::vector<UnitInfo> GetUnits() override;

    bool GetLocalPosition(Vector3& position);
    bool GetCameraPosition(Vector3& position);
    bool GetViewMatrix(Matrix4x4& matrix);
    bool GetLocalPositionViaPlayer(Vector3& position);

    std::uintptr_t GetLocalUnitAddress();

    std::vector<UnitInfo> GetUnitsList1();
    std::vector<UnitInfo> GetUnitsList2();
    std::vector<UnitInfo> GetUnitsList3();
private:
    MemoryManager& memory;

    // Получить корневой объект/контекст игры.
    std::uintptr_t GetGameContext();

    // Получить список сущностей и их количество.
    bool GetUnitList(std::uintptr_t gameContext, std::uintptr_t& unitList, std::uint16_t& unitCount);

    // Прочитать одну сущность и преобразовать ее в UnitInfo.
    bool ReadUnit(std::uintptr_t unitAddress, UnitInfo& outUnit);

    // Проверить, что полученные данные выглядят адекватно.
    bool IsUnitValid(const UnitInfo& unit) const;

    std::vector<UnitInfo> GetUnitsFromList(
        std::ptrdiff_t listOffset,
        std::ptrdiff_t countOffset
    );
};