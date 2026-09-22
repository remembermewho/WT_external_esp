#include "GameDataSource.h"
#include "../Offsets/Offsets.h"
#include <iostream>
#include <cmath>

GameDataSource::GameDataSource(MemoryManager& memory) : memory(memory)
{

}

// ============================================================
// GAME CONTEXT
// ============================================================

std::uintptr_t GameDataSource::GetGameContext() // // Получение указателя на GameContext плюсуя его к baseAddress
{
    if (!memory.IsValid())
        return 0;

    const std::uintptr_t baseAddress = memory.GetBaseAddress();

    const std::uintptr_t pointerAddress = baseAddress + offsets::Global::g_GameContext;

    std::uintptr_t gameContext = 0;

    if (!memory.Read(pointerAddress, gameContext))
    {
        std::cout << "[GameDataSource] Failed to read GameContext\n";

        return 0;
    }

    /*std::cout
        << "[DEBUG] BaseAddress: 0x"
        << std::hex << baseAddress
        << '\n';

    std::cout
        << "[DEBUG] GameContext pointer address: 0x"
        << std::hex << pointerAddress
        << '\n';

    std::cout
        << "[DEBUG] GameContext: 0x"
        << std::hex << gameContext
        << std::dec
        << '\n';*/

    if (gameContext == 0)
    {
        std::cout << "[GameDataSource] GameContext pointer is null\n";
        return 0;
    }

    return gameContext;
}

// ============================================================
// UNIT LIST
// ============================================================

bool GameDataSource::GetUnitList(std::uintptr_t gameContext, std::uintptr_t& unitList, std::uint16_t& unitCount)
{
    unitList = 0;
    unitCount = 0;

    if (gameContext == 0)
        return false;

    if (offsets::Game::m_UnitList1 == 0 || offsets::Game::m_UnitCount1 == 0)
    {
        std::cout << "[GameDataSource] UnitList/UnitCount offsets not configured\n";

        return false;
    }

    /*
        Тут уже baseAddress НЕ используется.

        Теперь начало отсчёта — объект GameContext.

        GameContext
             +
        member offset
             =
        address of field
    */

    const std::uintptr_t unitListFieldAddress = gameContext + offsets::Game::m_UnitList1;

    const std::uintptr_t unitCountFieldAddress = gameContext + offsets::Game::m_UnitCount1;

    // UnitList field предполагается pointer'ом
    if (!memory.Read(unitListFieldAddress, unitList))
    {
        std::cout << "[GameDataSource] Failed to read UnitList pointer\n";

        return false;
    }

    // UnitCount читается как uint16_t
    if (!memory.Read(unitCountFieldAddress, unitCount))
    {
        std::cout << "[GameDataSource] Failed to read UnitCount\n";

        return false;
    }

    /*std::cout
        << "[DEBUG] UnitList field address: 0x"
        << std::hex << unitListFieldAddress
        << '\n';

    std::cout
        << "[DEBUG] UnitCount field address: 0x"
        << std::hex << unitCountFieldAddress
        << '\n';

    std::cout
        << "[DEBUG] UnitList pointer: 0x"
        << std::hex << unitList
        << '\n';

    std::cout
        << "[DEBUG] UnitCount: "
        << std::dec << unitCount
        << '\n';*/

    if (unitList == 0)
    {
        std::cout << "[GameDataSource] UnitList pointer is null\n";

        return false;
    }

    // sanity check
    if (unitCount == 0 || unitCount > 1024)
    {
        std::cout << "[GameDataSource] Invalid UnitCount: " << unitCount << '\n';

        return false;
    }

    return true;
}


// ============================================================
// READ SINGLE UNIT
// ============================================================

bool GameDataSource::ReadUnit(
    std::uintptr_t unitAddress,
    UnitInfo& outUnit)
{
    if (unitAddress == 0)
        return false;


    if (offsets::Unit::m_Position == 0)
    {
        std::cout
            << "[GameDataSource] Position offset not configured\n";

        return false;
    }


    outUnit.id =
        unitAddress;


    // ========================================================
    // POSITION
    // ========================================================

    if (!memory.Read(
        unitAddress + offsets::Unit::m_Position,
        outUnit.position))
    {
        return false;
    }


    // ========================================================
    // BOUNDING BOX
    // ========================================================

    if (!memory.Read(
        unitAddress + offsets::Unit::m_BBMin,
        outUnit.bbMin))
    {
        return false;
    }


    if (!memory.Read(
        unitAddress + offsets::Unit::m_BBMax,
        outUnit.bbMax))
    {
        return false;
    }


    // ========================================================
    // ROTATION
    // ========================================================

    if (!memory.Read(
        unitAddress + offsets::Unit::m_RotationMatrix,
        outUnit.rotation))
    {
        return false;
    }


    // ========================================================
    // TEAM
    // ========================================================

    if (offsets::Unit::m_TeamNum != 0)
    {
        std::int8_t team = 0;


        if (!memory.Read(
            unitAddress + offsets::Unit::m_TeamNum,
            team))
        {
            return false;
        }


        outUnit.team =
            static_cast<int>(team);
    }


    // ========================================================
    // STATE
    // ========================================================

    if (offsets::Unit::m_UnitState != 0)
    {
        std::int8_t state = 0;


        if (!memory.Read(
            unitAddress + offsets::Unit::m_UnitState,
            state))
        {
            return false;
        }


        outUnit.state =
            static_cast<int>(state);
    }


    // ========================================================
    // TYPE
    // ========================================================

    if (offsets::Unit::m_UnitType != 0)
    {
        std::int8_t type = 0;


        if (!memory.Read(
            unitAddress + offsets::Unit::m_UnitType,
            type))
        {
            return false;
        }


        outUnit.type =
            static_cast<int>(type);
    }


    // ========================================================
    // FLAGS
    // ========================================================
    //
    // Пока диагностические поля.
    // Если чтение одного из них не удалось,
    // Unit всё равно оставляем валидным.
    // ========================================================

    memory.Read(
        unitAddress + offsets::Unit::m_UnitFlags1,
        outUnit.flags1
    );


    memory.Read(
        unitAddress + offsets::Unit::m_UnitFlags2,
        outUnit.flags2
    );


    memory.Read(
        unitAddress + offsets::Unit::m_UnitFlags3,
        outUnit.flags3
    );


    memory.Read(
        unitAddress + offsets::Unit::m_UnitFlags4,
        outUnit.flags4
    );


    return true;
}

// ============================================================
// VALIDATE UNIT
// ============================================================

bool GameDataSource::IsUnitValid(const UnitInfo& unit) const
{
    if (unit.id == 0)
        return false;

    // NaN / Infinity
    if (!std::isfinite(unit.position.x) || !std::isfinite(unit.position.y) || !std::isfinite(unit.position.z))
    {
        return false;
    }

    /*
        Просто защита от явно мусорных float.

        Это НЕ реальный размер карты.
    */

    constexpr float maxCoordinate = 1'000'000.0f;

    if (std::abs(unit.position.x) > maxCoordinate || std::abs(unit.position.y) > maxCoordinate || std::abs(unit.position.z) > maxCoordinate)
    {
        return false;
    }

    return true;
}


// ============================================================
// GET ALL UNITS
// ============================================================

std::vector<UnitInfo> GameDataSource::GetUnits()
{
    std::vector<UnitInfo> units;

    // ========================================================
    // 0. MemoryManager
    // ========================================================

    if (!memory.IsValid())
    {
        std::cout << "[GameDataSource] MemoryManager invalid\n";

        return units;
    }

    // ========================================================
    // 1. GameContext
    // ========================================================

    const std::uintptr_t gameContext = GetGameContext();

    if (gameContext == 0)
        return units;

    //std::cout << "[GameDataSource] GameContext: 0x" << std::hex << gameContext << std::dec << '\n';

    // ========================================================
    // 2. UnitList + UnitCount
    // ========================================================

    std::uintptr_t unitList = 0;
    std::uint16_t unitCount = 0;

    if (!GetUnitList(gameContext, unitList, unitCount))
    {
        return units;
    }

    //std::cout << "[GameDataSource] UnitList: 0x" << std::hex << unitList << std::dec << '\n';

    //std::cout << "[GameDataSource] UnitCount: " << unitCount << '\n';

    units.reserve(unitCount);

    // ========================================================
    // 3. UnitList iteration
    // ========================================================

    for (std::uint16_t i = 0; i < unitCount; ++i)   
    {
        /*
            Предполагаемый layout:

            unitList + 0x00 -> Unit*
            unitList + 0x08 -> Unit*
            unitList + 0x10 -> Unit*
            ...

            На x64 pointer обычно 8 bytes.
        */

        const std::uintptr_t elementAddress = unitList + static_cast<std::uintptr_t>(i) * sizeof(std::uintptr_t);

        // Сам pointer на Unit
        std::uintptr_t unitAddress = 0;

        if (!memory.Read(elementAddress, unitAddress))
        {
            continue;
        }

        /*std::cout
            << "[DEBUG] Unit[" << i << "]"
            << " elementAddress=0x"
            << std::hex << elementAddress
            << " -> unitAddress=0x"
            << unitAddress
            << std::dec
            << '\n';*/

        if (unitAddress == 0)
            continue;

        // ====================================================
        // 4. Unit -> UnitInfo
        // ====================================================

        UnitInfo unit{};

        if (!ReadUnit(unitAddress, unit))
        {
            continue;
        }

        if (!IsUnitValid(unit))
            continue;

        units.push_back(unit);
    }

    return units;
}

std::uintptr_t GameDataSource::GetLocalUnitAddress()
{
    if (!memory.IsValid())
        return 0;

    const std::uintptr_t baseAddress =
        memory.GetBaseAddress();

    const std::uintptr_t myUnitGlobalAddress =
        baseAddress + offsets::Global::g_MyUnit;

    std::uintptr_t myUnit = 0;

    if (!memory.Read(
        myUnitGlobalAddress,
        myUnit))
    {
        return 0;
    }

    return myUnit;
}

bool GameDataSource::GetLocalPosition(Vector3& position)
{
    if (!memory.IsValid())
        return false;

    const std::uintptr_t myUnit =
        GetLocalUnitAddress();

    if (myUnit == 0)
    {
        return false;
    }

    if (myUnit == 0)
    {
        std::cout
            << "[MYUNIT] Unit is null\n";

        return false;
    }

    const std::uintptr_t positionAddress =
        myUnit + offsets::Unit::m_Position;

    Vector3 tempPosition{};

    if (!memory.Read(
        positionAddress,
        tempPosition))
    {
        std::cout
            << "[MYUNIT] Failed to read Position\n";

        return false;
    }


    if (!std::isfinite(tempPosition.x) ||
        !std::isfinite(tempPosition.y) ||
        !std::isfinite(tempPosition.z))
    {
        std::cout
            << "[MYUNIT] Position contains NaN/Inf\n";

        return false;
    }

    constexpr float maxCoordinate =
        1'000'000.0f;

    if (std::abs(tempPosition.x) > maxCoordinate ||
        std::abs(tempPosition.y) > maxCoordinate ||
        std::abs(tempPosition.z) > maxCoordinate)
    {
        std::cout
            << "[MYUNIT] Position looks invalid\n";

        return false;
    }

    position = tempPosition;

    return true;
}

bool GameDataSource::GetLocalPositionViaPlayer(Vector3& position)
{
    if (!memory.IsValid())
        return false;

    const std::uintptr_t baseAddress =
        memory.GetBaseAddress();


    // ========================================================
    // 1. ADDRESS OF g_LocalPlayer
    // ========================================================

    const std::uintptr_t localPlayerGlobalAddress =
        baseAddress + offsets::Global::g_LocalPlayer;


    std::cout
        << "[LOCAL2] g_LocalPlayer address: 0x"
        << std::hex
        << localPlayerGlobalAddress
        << std::dec
        << '\n';


    // ========================================================
    // 2. READ g_LocalPlayer
    // ========================================================

    std::uintptr_t localPlayer = 0;

    if (!memory.Read(
        localPlayerGlobalAddress,
        localPlayer))
    {
        std::cout
            << "[LOCAL2] Failed to read g_LocalPlayer"
            << " | WinError="
            << GetLastError()
            << '\n';

        return false;
    }


    std::cout
        << "[LOCAL2] LocalPlayer value: 0x"
        << std::hex
        << localPlayer
        << std::dec
        << '\n';


    if (localPlayer == 0)
    {
        std::cout
            << "[LOCAL2] LocalPlayer is null\n";

        return false;
    }


    // ========================================================
    // 3. ADDRESS OF m_ControlledUnit
    // ========================================================

    const std::uintptr_t controlledUnitFieldAddress =
        localPlayer +
        offsets::Player::m_ControlledUnit;


    std::cout
        << "[LOCAL2] ControlledUnit field address: 0x"
        << std::hex
        << controlledUnitFieldAddress
        << std::dec
        << '\n';


    // ========================================================
    // 4. READ ControlledUnit*
    // ========================================================

    std::uintptr_t controlledUnit = 0;

    if (!memory.Read(
        controlledUnitFieldAddress,
        controlledUnit))
    {
        const DWORD error =
            GetLastError();

        std::cout
            << "[LOCAL2] Failed to read ControlledUnit"
            << " | WinError="
            << error
            << '\n';

        return false;
    }


    std::cout
        << "[LOCAL2] ControlledUnit value: 0x"
        << std::hex
        << controlledUnit
        << std::dec
        << '\n';


    if (controlledUnit == 0)
    {
        std::cout
            << "[LOCAL2] ControlledUnit is null\n";

        return false;
    }


    // ========================================================
    // 5. POSITION ADDRESS
    // ========================================================

    const std::uintptr_t positionAddress =
        controlledUnit +
        offsets::Unit::m_Position;


    std::cout
        << "[LOCAL2] Position address: 0x"
        << std::hex
        << positionAddress
        << std::dec
        << '\n';


    // ========================================================
    // 6. READ POSITION
    // ========================================================

    Vector3 tempPosition{};

    if (!memory.Read(
        positionAddress,
        tempPosition))
    {
        std::cout
            << "[LOCAL2] Failed to read Position"
            << " | WinError="
            << GetLastError()
            << '\n';

        return false;
    }


    std::cout
        << "[LOCAL2] RAW POSITION: "
        << "X=" << tempPosition.x
        << " Y=" << tempPosition.y
        << " Z=" << tempPosition.z
        << '\n';


    // ========================================================
    // 7. VALIDATE POSITION
    // ========================================================

    if (!std::isfinite(tempPosition.x) ||
        !std::isfinite(tempPosition.y) ||
        !std::isfinite(tempPosition.z))
    {
        std::cout
            << "[LOCAL2] Position contains NaN/Inf\n";

        return false;
    }


    constexpr float maxCoordinate =
        1'000'000.0f;


    if (std::abs(tempPosition.x) > maxCoordinate ||
        std::abs(tempPosition.y) > maxCoordinate ||
        std::abs(tempPosition.z) > maxCoordinate)
    {
        std::cout
            << "[LOCAL2] Position looks invalid\n";

        return false;
    }


    position = tempPosition;

    return true;
}

bool GameDataSource::GetCameraPosition(Vector3& position)
{
    const std::uintptr_t gameContext = GetGameContext();

    if (gameContext == 0)
        return false;

    std::uintptr_t camera = 0;

    if (!memory.Read(gameContext + offsets::Game::m_CameraCont, camera))
    {
        std::cout << "[CAMERA] Failed to read CameraCont\n";

        return false;
    }

    if (camera == 0)
    {
        std::cout << "[CAMERA] Camera = nullptr\n";

        return false;
    }

    std::cout << "[CAMERA] Camera: 0x" << std::hex << camera << std::dec
        << '\n';

    if (!memory.Read(camera + offsets::Camera::m_CameraPos, position))
    {
        std::cout << "[CAMERA] Failed to read position\n";

        return false;
    }

    return true;
}

bool GameDataSource::GetViewMatrix(Matrix4x4& matrix)
{
    if (!memory.IsValid())
        return false;

    const std::uintptr_t baseAddress =
        memory.GetBaseAddress();

    const std::uintptr_t matrixAddress =
        baseAddress + offsets::Global::g_ViewMatrix;

    /*std::cout
        << "[VIEW] MatrixAddress: 0x"
        << std::hex
        << matrixAddress
        << std::dec
        << '\n';*/

    if (!memory.Read(matrixAddress, matrix))
    {
        std::cout
            << "[VIEW] Failed to read matrix\n";

        return false;
    }

    return true;
}

std::vector<UnitInfo> GameDataSource::GetUnitsFromList(
    std::ptrdiff_t listOffset,
    std::ptrdiff_t countOffset)
{
    std::vector<UnitInfo> units;

    if (!memory.IsValid())
        return units;

    const std::uintptr_t gameContext =
        GetGameContext();

    if (gameContext == 0)
        return units;


    std::uintptr_t unitList = 0;
    std::uint16_t unitCount = 0;


    if (!memory.Read(
        gameContext + listOffset,
        unitList))
    {
        return units;
    }


    if (!memory.Read(
        gameContext + countOffset,
        unitCount))
    {
        return units;
    }


    if (unitList == 0 ||
        unitCount == 0 ||
        unitCount > 1024)
    {
        return units;
    }


    units.reserve(unitCount);


    for (std::uint16_t i = 0;
        i < unitCount;
        ++i)
    {
        const std::uintptr_t elementAddress =
            unitList +
            static_cast<std::uintptr_t>(i) *
            sizeof(std::uintptr_t);


        std::uintptr_t unitAddress = 0;


        if (!memory.Read(
            elementAddress,
            unitAddress))
        {
            continue;
        }


        if (unitAddress == 0)
            continue;


        UnitInfo unit{};


        if (!ReadUnit(
            unitAddress,
            unit))
        {
            continue;
        }


        if (!IsUnitValid(unit))
            continue;


        units.push_back(unit);
    }


    return units;
}

std::vector<UnitInfo> GameDataSource::GetUnitsList1()
{
    return GetUnitsFromList(
        offsets::Game::m_UnitList1,
        offsets::Game::m_UnitCount1
    );
}


std::vector<UnitInfo> GameDataSource::GetUnitsList2()
{
    return GetUnitsFromList(
        offsets::Game::m_UnitList2,
        offsets::Game::m_UnitCount2
    );
}


std::vector<UnitInfo> GameDataSource::GetUnitsList3()
{
    return GetUnitsFromList(
        offsets::Game::m_UnitList3,
        offsets::Game::m_UnitCount3
    );
}