#include "ExternMath.h"

bool Math::WorldToScreen(
    const Vector3& world,
    Vector2& screen,
    const Matrix4x4& matrix,
    float screenWidth,
    float screenHeight)
{
    const float clipX =
        world.x * matrix.m[0] +
        world.y * matrix.m[4] +
        world.z * matrix.m[8] +
        matrix.m[12];

    const float clipY =
        world.x * matrix.m[1] +
        world.y * matrix.m[5] +
        world.z * matrix.m[9] +
        matrix.m[13];

    const float clipW =
        world.x * matrix.m[3] +
        world.y * matrix.m[7] +
        world.z * matrix.m[11] +
        matrix.m[15];

    // ≈сли W <= 0, точка обычно находитс€ позади камеры.
    if (clipW <= 0.001f)
        return false;

    // Normalized Device Coordinates (-1 ... +1)
    const float ndcX = clipX / clipW;
    const float ndcY = clipY / clipW;

    if (ndcX < -1.0f || ndcX > 1.0f ||
        ndcY < -1.0f || ndcY > 1.0f)
    {
        return false;
    }

    // NDC -> pixels
    screen.x =
        (screenWidth * 0.5f) *
        (1.0f + ndcX);

    screen.y =
        (screenHeight * 0.5f) *
        (1.0f - ndcY);

    return true;
}