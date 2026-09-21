#pragma once

#include <cstdint>
#include <cstddef>

// War Thunder
// Channel: live
// Version: 2.59.0.13
// Offsets count: 66

namespace offsets
{
    namespace Ballistics
    {
        constexpr std::ptrdiff_t m_BombPredPos = 0x1CCC;
    }

    namespace Camera
    {
        static constexpr std::ptrdiff_t m_CameraPos = 0x60; // ESP: камера
        static constexpr std::ptrdiff_t m_ViewMatrix = 0x1D8; // ESP: WorldToScreen
    }

    namespace Game
    {
        static constexpr std::ptrdiff_t m_BallisticsCont = 0x3F0;
        static constexpr std::ptrdiff_t m_CameraCont = 0x660; // ESP: контейнер камеры
        static constexpr std::ptrdiff_t m_CurrentMap = 0x1F0;

        static constexpr std::ptrdiff_t m_UnitList1 = 0x310; // ESP: список Unit ===========
        static constexpr std::ptrdiff_t m_UnitCount1 = 0x320; // ESP: количество Unit =============

        static constexpr std::ptrdiff_t m_UnitList2 = 0x328; // ESP: возможно доп. список Unit
        static constexpr std::ptrdiff_t m_UnitCount2 = 0x338; // ESP: количество Unit

        static constexpr std::ptrdiff_t m_UnitList3 = 0x340; // ESP: возможно доп. список Unit
        static constexpr std::ptrdiff_t m_UnitCount3 = 0x350; // ESP: количество Unit
    }

    namespace Global
    {
        static constexpr std::uintptr_t g_AirPredictionBool = 0x7E7FEA0;
        static constexpr std::uintptr_t g_BombListIndexPtr = 0x79F1F60;
        static constexpr std::uintptr_t g_GameContext = 0x7A17268; // ESP: основной контекст ===========
        static constexpr std::uintptr_t g_GameOptics = 0x7A190E0;
        static constexpr std::uintptr_t g_HudInfo = 0x7A16890;
        static constexpr std::uintptr_t g_IsScoping = 0x7A6047C;
        static constexpr std::uintptr_t g_LocalPlayer = 0x79EE8F0; // ESP: локальный игрок
        static constexpr std::uintptr_t g_MyUnit = 0x7A19210; // ESP: твоя техника
        static constexpr std::uintptr_t g_RocketListIndexPtr = 0x79F34A0;
        static constexpr std::uintptr_t g_ScreenWidth = 0x7E5D6D0; // ESP: размеры экрана
        static constexpr std::uintptr_t g_UIPosArr = 0x7A636A8;
        static constexpr std::uintptr_t g_ViewAngles = 0x7A1BD78;
        static constexpr std::uintptr_t g_ViewMatrix = 0x7A712F8; // ESP: WorldToScreen
    }

    namespace GroundMovement
    {
        static constexpr std::ptrdiff_t m_Velocity = 0x5C;
    }

    namespace MissileNameCont
    {
        static constexpr std::ptrdiff_t m_Type = 0x870;
    }

    namespace Player
    {
        static constexpr std::ptrdiff_t m_ControlledUnit = 0x878; // ESP: Unit игрока
        static constexpr std::ptrdiff_t m_GuiState = 0x640;
        static constexpr std::ptrdiff_t m_Name = 0x60; // ESP: имя, если выводишь
        static constexpr std::ptrdiff_t m_OwnedUnit = 0x848;
        static constexpr std::ptrdiff_t m_SpectatedModelIndex = 0x670;
    }

    namespace Projectile
    {
        static constexpr std::ptrdiff_t m_BombNameCont = 0x718;
        static constexpr std::ptrdiff_t m_MissileNameCont = 0x720;
        static constexpr std::ptrdiff_t m_OwnerUnit = 0x58;
        static constexpr std::ptrdiff_t m_Position = 0x2C0; // ==========
        static constexpr std::ptrdiff_t m_Velocity = 0x2DC;
    }

    namespace UI
    {
        static constexpr std::ptrdiff_t m_CursorPos = 0xD5C;
    }

    namespace Unit
    {
        static constexpr std::ptrdiff_t m_AirContainer = 0xD50;
        static constexpr std::ptrdiff_t m_Armory = 0x10D0;

        static constexpr std::ptrdiff_t m_BBMax = 0x274; // ESP: 3D/2D box
        static constexpr std::ptrdiff_t m_BBMin = 0x268; // ESP: 3D/2D box

        static constexpr std::ptrdiff_t m_BodyBBMax = 0x218C; // ESP: bounding box корпуса
        static constexpr std::ptrdiff_t m_BodyBBMin = 0x2180; // ESP: bounding box корпуса

        static constexpr std::ptrdiff_t m_DamageModelCont = 0x10A8;

        static constexpr std::ptrdiff_t m_InvulTimer = 0xE74;
        static constexpr std::ptrdiff_t m_Invulnerable = 0xE98;

        static constexpr std::ptrdiff_t m_PlayerInfo = 0xFA0; // ESP: связь Unit -> player info

        static constexpr std::ptrdiff_t m_Position = 0xD40; // ESP: мировая позиция
        static constexpr std::ptrdiff_t m_RotationMatrix = 0xD1C; // ESP: ориентация

        static constexpr std::ptrdiff_t m_TeamNum = 0x1018; // ESP: свой / противник

        static constexpr std::ptrdiff_t m_UnitFlags1 = 0x90;
        static constexpr std::ptrdiff_t m_UnitFlags2 = 0x91;
        static constexpr std::ptrdiff_t m_UnitFlags3 = 0x92;
        static constexpr std::ptrdiff_t m_UnitFlags4 = 0x93;

        static constexpr std::ptrdiff_t m_UnitIndex = 0x8; // ESP: ID/индекс Unit
        static constexpr std::ptrdiff_t m_UnitInfo = 0x1028;
        static constexpr std::ptrdiff_t m_UnitState = 0xF98; // ESP: жив / уничтожен / состояние
        static constexpr std::ptrdiff_t m_UnitType = 0x8C; // ESP: тип техники

        static constexpr std::ptrdiff_t m_Velocity = 0x2100;
        static constexpr std::ptrdiff_t m_VisualReload = 0xAF0;

        namespace Info
        {
            static constexpr std::ptrdiff_t m_BomberView = 0x454;
            static constexpr std::ptrdiff_t m_HaveCCIPForBombs = 0x459;
            static constexpr std::ptrdiff_t m_HaveCCIPForGun = 0x457;
            static constexpr std::ptrdiff_t m_HaveCCIPForRocket = 0x456;
            static constexpr std::ptrdiff_t m_HaveCCIPForTurret = 0x458;
        }
    }
}