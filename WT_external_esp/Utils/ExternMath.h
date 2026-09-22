#pragma once

#include "../Game/GameTypes.h"

namespace Math
{
    bool WorldToScreen(
        const Vector3& world,
        Vector2& screen,
        const Matrix4x4& matrix,
        float screenWidth,
        float screenHeight
    );
}