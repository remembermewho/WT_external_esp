#include "UnitESP.h"

#include "../Utils/ExternMath.h"
#include "../imgui/imgui.h"

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <limits>


// ============================================================
// DISTANCE
// ============================================================

float UnitESP::Distance(
    const Vector3& a,
    const Vector3& b) const
{
    const float dx =
        a.x - b.x;

    const float dy =
        a.y - b.y;

    const float dz =
        a.z - b.z;


    return std::sqrt(
        dx * dx +
        dy * dy +
        dz * dz
    );
}


// ============================================================
// LOCAL POINT -> WORLD POINT
// ============================================================

Vector3 UnitESP::TransformLocalPoint(
    const Vector3& localPoint,
    const Vector3& unitPosition,
    const Matrix3x3& rotation) const
{
    Vector3 world{};


    world.x =
        unitPosition.x +
        localPoint.x * rotation.m[0] +
        localPoint.y * rotation.m[3] +
        localPoint.z * rotation.m[6];


    world.y =
        unitPosition.y +
        localPoint.x * rotation.m[1] +
        localPoint.y * rotation.m[4] +
        localPoint.z * rotation.m[7];


    world.z =
        unitPosition.z +
        localPoint.x * rotation.m[2] +
        localPoint.y * rotation.m[5] +
        localPoint.z * rotation.m[8];


    return world;
}


// ============================================================
// 3D BOUNDING BOX -> 2D BOX
// ============================================================

bool UnitESP::GetBoundingBox2D(
    const UnitInfo& unit,
    const Matrix4x4& viewMatrix,
    float screenWidth,
    float screenHeight,
    Vector2& topLeft,
    Vector2& bottomRight) const
{
    // --------------------------------------------------------
    // 8 LOCAL CORNERS
    // --------------------------------------------------------

    const Vector3 localCorners[8] =
    {
        {
            unit.bbMin.x,
            unit.bbMin.y,
            unit.bbMin.z
        },

        {
            unit.bbMax.x,
            unit.bbMin.y,
            unit.bbMin.z
        },

        {
            unit.bbMin.x,
            unit.bbMax.y,
            unit.bbMin.z
        },

        {
            unit.bbMax.x,
            unit.bbMax.y,
            unit.bbMin.z
        },

        {
            unit.bbMin.x,
            unit.bbMin.y,
            unit.bbMax.z
        },

        {
            unit.bbMax.x,
            unit.bbMin.y,
            unit.bbMax.z
        },

        {
            unit.bbMin.x,
            unit.bbMax.y,
            unit.bbMax.z
        },

        {
            unit.bbMax.x,
            unit.bbMax.y,
            unit.bbMax.z
        }
    };


    float minX =
        std::numeric_limits<float>::max();

    float minY =
        std::numeric_limits<float>::max();

    float maxX =
        std::numeric_limits<float>::lowest();

    float maxY =
        std::numeric_limits<float>::lowest();


    int projectedCorners = 0;


    // --------------------------------------------------------
    // PROJECT ALL CORNERS
    // --------------------------------------------------------

    for (const Vector3& localCorner : localCorners)
    {
        const Vector3 worldCorner =
            TransformLocalPoint(
                localCorner,
                unit.position,
                unit.rotation
            );


        Vector2 screenCorner{};


        if (!Math::WorldToScreen(
            worldCorner,
            screenCorner,
            viewMatrix,
            screenWidth,
            screenHeight))
        {
            continue;
        }


        minX =
            std::min(
                minX,
                screenCorner.x
            );


        minY =
            std::min(
                minY,
                screenCorner.y
            );


        maxX =
            std::max(
                maxX,
                screenCorner.x
            );


        maxY =
            std::max(
                maxY,
                screenCorner.y
            );


        ++projectedCorners;
    }


    // --------------------------------------------------------
    // VALIDATE
    // --------------------------------------------------------

    if (projectedCorners < 4)
    {
        return false;
    }


    if (maxX <= minX ||
        maxY <= minY)
    {
        return false;
    }


    const float width =
        maxX - minX;

    const float height =
        maxY - minY;


    // Защита от явно сломанного box
    if (width < 1.0f ||
        height < 1.0f)
    {
        return false;
    }


    if (width > screenWidth ||
        height > screenHeight)
    {
        return false;
    }


    topLeft.x =
        minX;

    topLeft.y =
        minY;


    bottomRight.x =
        maxX;

    bottomRight.y =
        maxY;


    return true;
}


// ============================================================
// ESP RENDER
// ============================================================

void UnitESP::Render(
    const std::vector<UnitInfo>& units,
    std::uintptr_t localUnitAddress,
    const Vector3& localPosition,
    const Matrix4x4& viewMatrix,
    float screenWidth,
    float screenHeight)
{
    ImDrawList* drawList =
        ImGui::GetBackgroundDrawList();


    if (!drawList)
        return;


    for (const UnitInfo& unit : units)
    {
        // ----------------------------------------------------
        // SKIP LOCAL UNIT
        // ----------------------------------------------------

        if (unit.id == localUnitAddress)
        {
            continue;
        }

        // ----------------------------------------------------
// ACTIVE / VISIBLE UNIT FILTER
// ----------------------------------------------------
//
// По текущим тестам:
// flags1 0x08 -> реальная активная техника
// flags1 0x04 -> невидимый/служебный объект
//
// Проверяем именно бит 0x08, а не точное равенство,
// чтобы не сломаться, если появятся дополнительные флаги.
// ----------------------------------------------------

        if ((unit.flags1 & 0x08) == 0)
        {
            continue;
        }
        // ----------------------------------------------------
        // DISTANCE
        // ----------------------------------------------------

        const float distance =
            Distance(
                localPosition,
                unit.position
            );


        // ----------------------------------------------------
        // GET 2D BOX
        // ----------------------------------------------------

        Vector2 topLeft{};
        Vector2 bottomRight{};


        if (!GetBoundingBox2D(
            unit,
            viewMatrix,
            screenWidth,
            screenHeight,
            topLeft,
            bottomRight))
        {
            continue;
        }


        // ----------------------------------------------------
        // TYPE NAME
        // --------------------------------------------------------
        //
        // Пока НЕ фильтруем объекты.
        // Только даём известным типам подписи.
        // ----------------------------------------------------

        const char* typeName = "Unknown";

        if (unit.type == 3)
        {
            typeName = "Tank";
        }
        else if (unit.type == 5)
        {
            typeName = "Ship";
		}
        else if (unit.type == 0)
        {
            typeName = "Aircraft";
        }
        else if (unit.type == 8)
        {
            typeName = "Type8";
        }


        // ----------------------------------------------------
        // BOX COLOR
        // ----------------------------------------------------

        ImU32 boxColor;


        if (unit.team == 2)
        {
            boxColor =
                IM_COL32(
                    255,
                    80,
                    80,
                    255
                );
        }
        else
        {
            boxColor =
                IM_COL32(
                    80,
                    180,
                    255,
                    255
                );
        }


        // ----------------------------------------------------
        // DRAW BOX
        // ----------------------------------------------------

        drawList->AddRect(
            ImVec2(
                topLeft.x,
                topLeft.y
            ),

            ImVec2(
                bottomRight.x,
                bottomRight.y
            ),

            boxColor,

            0.0f,

            0,

            2.0f
        );


        // ----------------------------------------------------
        // TEXT
        // ----------------------------------------------------

        char text[128]{};


        std::snprintf(
            text,
            sizeof(text),
            "%s T:%d S:%d F:%02X/%02X/%02X/%02X",
            typeName,
            unit.type,
            unit.state,
            static_cast<unsigned int>(unit.flags1),
            static_cast<unsigned int>(unit.flags2),
            static_cast<unsigned int>(unit.flags3),
            static_cast<unsigned int>(unit.flags4)
        );


        const ImVec2 textSize =
            ImGui::CalcTextSize(
                text
            );


        const float boxCenterX =
            (
                topLeft.x +
                bottomRight.x
                ) * 0.5f;


        const float textX =
            boxCenterX -
            textSize.x * 0.5f;


        const float textY =
            topLeft.y -
            textSize.y -
            3.0f;


        // ----------------------------------------------------
        // TEXT BACKGROUND
        // ----------------------------------------------------

        drawList->AddRectFilled(
            ImVec2(
                textX - 2.0f,
                textY - 1.0f
            ),

            ImVec2(
                textX +
                textSize.x +
                2.0f,

                textY +
                textSize.y +
                1.0f
            ),

            IM_COL32(
                0,
                0,
                0,
                160
            )
        );


        // ----------------------------------------------------
        // DRAW TEXT
        // ----------------------------------------------------

        drawList->AddText(
            ImVec2(
                textX,
                textY
            ),

            IM_COL32(
                255,
                255,
                255,
                255
            ),

            text
        );
    }
}