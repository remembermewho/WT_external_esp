#pragma once
#include <windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"



class Menu {
public:
    struct MenuOptions {
        // ESP
        bool DrawPlayerESP = true;
       
    };

    Menu(); // Constructor to apply style once
    MenuOptions GetOptions();
    void Draw();
    void ApplyStyle(ImGuiStyle& style);

private:
    MenuOptions options;
    bool menuIsOpen = true;

    enum class MainTabs {
        LegitBot,
        Visuals,
        Radar
    };
    MainTabs activeMainTab = MainTabs::LegitBot;

    enum class LegitSubTabs {
        AimAssist,
        TriggerBot
    };
    LegitSubTabs activeLegitSubTab = LegitSubTabs::AimAssist;

    enum class VisualsSubTabs {
        PlayerESP,
        ItemESP
    };

    VisualsSubTabs activeVisualsSubTab = VisualsSubTabs::PlayerESP;
};