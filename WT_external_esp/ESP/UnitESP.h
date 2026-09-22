#pragma once

#include <vector>
#include <cstdint>

#include "../Game/GameTypes.h"


class UnitESP
{
public:
    void Render(
        const std::vector<UnitInfo>& units,
        std::uintptr_t localUnitAddress,
        const Vector3& localPosition,
        const Matrix4x4& viewMatrix,
        float screenWidth,
        float screenHeight
    );


private:
    float Distance(
        const Vector3& a,
        const Vector3& b
    ) const;


    Vector3 TransformLocalPoint(
        const Vector3& localPoint,
        const Vector3& unitPosition,
        const Matrix3x3& rotation
    ) const;


    bool GetBoundingBox2D(
        const UnitInfo& unit,
        const Matrix4x4& viewMatrix,
        float screenWidth,
        float screenHeight,
        Vector2& topLeft,
        Vector2& bottomRight
    ) const;
};