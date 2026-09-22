#pragma once

#include <cstdint>
#include <string>


struct Vector2
{
    float x{};
    float y{};
};


struct Vector3
{
    float x{};
    float y{};
    float z{};
};


struct Matrix3x3
{
    float m[9]{};
};


struct Matrix4x4
{
    float m[16]{};
};


struct UnitInfo
{
    std::uintptr_t id{};

    Vector3 position{};

    Vector3 bbMin{};
    Vector3 bbMax{};

    Matrix3x3 rotation{};

    int team{};
    int type{};
    int state{};

    std::uint8_t flags1{};
    std::uint8_t flags2{};
    std::uint8_t flags3{};
    std::uint8_t flags4{};


    std::string name;
};