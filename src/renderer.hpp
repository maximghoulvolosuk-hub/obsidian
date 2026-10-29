#pragma once
#include "sdk.hpp"
#include "gui.hpp"
#include <cmath>

class ObsidianRenderer {
public:
    static bool WorldToScreen(const Vector3& pos, const Matrix4x4& viewMatrix, float screenWidth, float screenHeight, Vector2& out) {
        float w = viewMatrix.m[3] * pos.x + viewMatrix.m[7] * pos.y + viewMatrix.m[11] * pos.z + viewMatrix.m[15];
        if (w < 0.01f) return false;

        float x = viewMatrix.m[0] * pos.x + viewMatrix.m[4] * pos.y + viewMatrix.m[8] * pos.z + viewMatrix.m[12];
        float y = viewMatrix.m[1] * pos.x + viewMatrix.m[5] * pos.y + viewMatrix.m[9] * pos.z + viewMatrix.m[13];

        out.x = (screenWidth / 2.0f) + (screenWidth / 2.0f) * x / w;
        out.y = (screenHeight / 2.0f) - (screenHeight / 2.0f) * y / w;
        return true;
    }

    static void DrawGlassESPBox(ImVec2 top_left, ImVec2 bottom_right, int health) {
        if (!ObsidianGUI::cfg.esp_enable) return;

        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        float rounding = 6.0f;

        if (ObsidianGUI::cfg.esp_box_glass) {
            dl->AddRectFilled(top_left, bottom_right, IM_COL32(20, 22, 35, 45), rounding);
            dl->AddRect(ImVec2(top_left.x - 1, top_left.y - 1), ImVec2(bottom_right.x + 1, bottom_right.y + 1), IM_COL32(0, 0, 0, 140), rounding, 0, 1.5f);
            dl->AddRect(top_left, bottom_right, IM_COL32(230, 225, 255, 190), rounding, 0, 1.2f);
        }

        if (ObsidianGUI::cfg.esp_health) {
            float box_h = bottom_right.y - top_left.y;
            float hp_clamped = health > 100 ? 100.0f : (health < 0 ? 0.0f : static_cast<float>(health));
            float fill_h = box_h * (hp_clamped / 100.0f);

            ImVec2 bar_min(top_left.x - 7.0f, top_left.y);
            ImVec2 bar_max(top_left.x - 3.0f, bottom_right.y);

            dl->AddRectFilled(bar_min, bar_max, IM_COL32(10, 10, 15, 180), 2.0f);
            dl->AddRectFilled(ImVec2(bar_min.x, bar_max.y - fill_h), bar_max, IM_COL32(135, 105, 255, 240), 2.0f);
        }
    }
};
