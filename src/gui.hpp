#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>
#include <string>
#include <map>

namespace ObsidianGUI {
    struct Config {
        bool menu_open = true;
        int current_tab = 0;
        bool aim_enable = false;
        bool aim_silent = false;
        bool aim_vis_check = true;
        float aim_fov = 45.0f;
        float aim_smooth = 4.5f;
        int aim_bone = 0;
        bool esp_enable = true;
        bool esp_box_glass = true;
        bool esp_skeleton = true;
        bool esp_health = true;
        bool esp_name = true;
        bool esp_glow = false;
        float esp_render_dist = 150.0f;
        bool misc_watermark = true;
        bool misc_fov_circle = true;
        float glass_opacity = 0.72f;
        float accent_color[4] = { 0.48f, 0.36f, 0.96f, 1.0f };
    } inline cfg;

    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * (t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t));
    }

    inline void DrawLiquidGlassPanel(ImDrawList* dl, ImVec2 p_min, ImVec2 p_max, float rounding, float alpha_mult = 1.0f) {
        for (int i = 1; i <= 4; ++i) {
            float expand = static_cast<float>(i) * 2.5f;
            dl->AddRect(ImVec2(p_min.x - expand, p_min.y - expand), ImVec2(p_max.x + expand, p_max.y + expand), IM_COL32(0, 0, 0, static_cast<int>((35 / i) * alpha_mult)), rounding + expand, 0, 2.5f);
        }
        int base_alpha = static_cast<int>(cfg.glass_opacity * 255.0f * alpha_mult);
        dl->AddRectFilled(p_min, p_max, IM_COL32(14, 15, 22, base_alpha), rounding);
        ImU32 col_tl = IM_COL32(255, 255, 255, static_cast<int>(28 * alpha_mult));
        ImU32 col_tr = IM_COL32(140, 120, 255, static_cast<int>(12 * alpha_mult));
        ImU32 col_br = IM_COL32(10, 10, 16, static_cast<int>(40 * alpha_mult));
        ImU32 col_bl = IM_COL32(80, 70, 160, static_cast<int>(15 * alpha_mult));
        dl->AddRectFilledMultiColor(ImVec2(p_min.x + 2, p_min.y + 2), ImVec2(p_max.x - 2, p_max.y - 2), col_tl, col_tr, col_br, col_bl);
        ImVec2 highlight_max = ImVec2(p_max.x - 2.0f, p_min.y + (p_max.y - p_min.y) * 0.22f);
        dl->AddRectFilled(ImVec2(p_min.x + 2.0f, p_min.y + 2.0f), highlight_max, IM_COL32(255, 255, 255, static_cast<int>(14 * alpha_mult)), rounding);
        dl->AddRect(p_min, p_max, IM_COL32(255, 255, 255, static_cast<int>(48 * alpha_mult)), rounding, 0, 1.2f);
        dl->AddRect(ImVec2(p_min.x + 1.2f, p_min.y + 1.2f), ImVec2(p_max.x - 1.2f, p_max.y - 1.2f), IM_COL32(130, 110, 255, static_cast<int>(25 * alpha_mult)), rounding - 1.0f, 0, 1.0f);
    }

    inline void BeginGlassGroup(const char* title, ImVec2 size) {
        ImGui::BeginChild(title, size, false, ImGuiWindowFlags_NoBackground);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p_min = ImGui::GetWindowPos();
        ImVec2 p_max = ImVec2(p_min.x + size.x, p_min.y + size.y);
        dl->AddRectFilled(p_min, p_max, IM_COL32(22, 24, 34, 155), 14.0f);
        dl->AddRect(p_min, p_max, IM_COL32(255, 255, 255, 32), 14.0f, 0, 1.0f);
        ImU32 accent = ImGui::ColorConvertFloat4ToU32(ImVec4(cfg.accent_color[0], cfg.accent_color[1], cfg.accent_color[2], 1.0f));
        dl->AddRectFilled(ImVec2(p_min.x + 14, p_min.y + 14), ImVec2(p_min.x + 18, p_min.y + 28), accent, 2.0f);
        dl->AddText(ImVec2(p_min.x + 26, p_min.y + 13), IM_COL32(235, 235, 245, 255), title);
        dl->AddLine(ImVec2(p_min.x + 14, p_min.y + 36), ImVec2(p_max.x - 14, p_min.y + 36), IM_COL32(255, 255, 255, 18), 1.0f);
        ImGui::SetCursorPos(ImVec2(14, 46));
        ImGui::BeginGroup();
    }

    inline void EndGlassGroup() {
        ImGui::EndGroup();
        ImGui::EndChild();
    }

    inline bool LiquidToggle(const char* label, bool* v) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;
        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        float height = 22.0f;
        float width = 42.0f;
        float avail_w = ImGui::GetContentRegionAvail().x - 14.0f;
        ImVec2 pos = window->DC.CursorPos;
        ImRect total_bb(pos, ImVec2(pos.x + avail_w, pos.y + height + 4.0f));
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id)) return false;
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed) *v = !(*v);
        static std::map<ImGuiID, float> anim_state;
        float& t = anim_state[id];
        t = Lerp(t, *v ? 1.0f : 0.0f, g.IO.DeltaTime * 12.0f);
        ImDrawList* dl = window->DrawList;
        ImVec2 pill_min = ImVec2(total_bb.Max.x - width, total_bb.Min.y + 2.0f);
        ImVec2 pill_max = ImVec2(total_bb.Max.x, total_bb.Min.y + 2.0f + height);
        ImVec4 accent(cfg.accent_color[0], cfg.accent_color[1], cfg.accent_color[2], 0.85f);
        ImU32 bg_col = ImGui::GetColorU32(ImLerp(ImVec4(0.12f, 0.13f, 0.18f, 0.75f), accent, t));
        dl->AddRectFilled(pill_min, pill_max, bg_col, height * 0.5f);
        dl->AddRect(pill_min, pill_max, IM_COL32(255, 255, 255, hovered ? 75 : 35), height * 0.5f, 0, 1.0f);
        float knob_r = (height * 0.5f) - 3.0f;
        float knob_x = Lerp(pill_min.x + knob_r + 3.0f, pill_max.x - knob_r - 3.0f, t);
        float knob_y = pill_min.y + height * 0.5f;
        dl->AddCircleFilled(ImVec2(knob_x, knob_y), knob_r, IM_COL32(250, 250, 255, 255), 24);
        dl->AddCircle(ImVec2(knob_x, knob_y), knob_r, IM_COL32(255, 255, 255, 120), 24, 1.0f);
        dl->AddText(ImVec2(total_bb.Min.x, total_bb.Min.y + 3.0f), *v ? IM_COL32(240, 240, 255, 255) : IM_COL32(165, 170, 185, 255), label);
        return pressed;
    }

    inline bool LiquidSlider(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f") {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;
        const ImGuiID id = window->GetID(label);
        float avail_w = ImGui::GetContentRegionAvail().x - 14.0f;
        float height = 38.0f;
        ImVec2 pos = window->DC.CursorPos;
        ImRect total_bb(pos, ImVec2(pos.x + avail_w, pos.y + height));
        ImRect track_bb(ImVec2(pos.x, pos.y + 22.0f), ImVec2(pos.x + avail_w, pos.y + 30.0f));
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id)) return false;
        bool hovered = ImGui::ItemHoverable(total_bb, id, 0);
        bool held = hovered && ImGui::IsMouseDown(0);
        if (held) {
            float mouse_t = (ImGui::GetIO().MousePos.x - track_bb.Min.x) / track_bb.GetWidth();
            mouse_t = mouse_t < 0.0f ? 0.0f : (mouse_t > 1.0f ? 1.0f : mouse_t);
            *v = v_min + (v_max - v_min) * mouse_t;
        }
        float t = (*v - v_min) / (v_max - v_min);
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        ImDrawList* dl = window->DrawList;
        char val_buf[64];
        snprintf(val_buf, sizeof(val_buf), format, *v);
        dl->AddText(pos, IM_COL32(210, 215, 230, 255), label);
        ImVec2 val_size = ImGui::CalcTextSize(val_buf);
        dl->AddText(ImVec2(total_bb.Max.x - val_size.x, pos.y), IM_COL32(180, 165, 255, 255), val_buf);
        dl->AddRectFilled(track_bb.Min, track_bb.Max, IM_COL32(15, 16, 24, 200), 4.0f);
        dl->AddRect(track_bb.Min, track_bb.Max, IM_COL32(255, 255, 255, 30), 4.0f);
        float fill_x = track_bb.Min.x + track_bb.GetWidth() * t;
        ImU32 col_accent = ImGui::ColorConvertFloat4ToU32(ImVec4(cfg.accent_color[0], cfg.accent_color[1], cfg.accent_color[2], 1.0f));
        dl->AddRectFilled(track_bb.Min, ImVec2(fill_x, track_bb.Max.y), col_accent, 4.0f);
        dl->AddCircleFilled(ImVec2(fill_x, track_bb.GetCenter().y), 6.5f, IM_COL32(255, 255, 255, 255), 20);
        dl->AddCircle(ImVec2(fill_x, track_bb.GetCenter().y), 7.5f, col_accent, 20, 1.5f);
        return held;
    }

    inline bool LiquidTab(const char* label, int tab_index) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImGuiID id = window->GetID(label);
        bool selected = (cfg.current_tab == tab_index);
        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size(135.0f, 38.0f);
        ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, id)) return false;
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        if (pressed) cfg.current_tab = tab_index;
        ImDrawList* dl = window->DrawList;
        if (selected || hovered) {
            int alpha = selected ? 65 : 25;
            dl->AddRectFilled(bb.Min, bb.Max, IM_COL32(255, 255, 255, alpha), 10.0f);
            dl->AddRect(bb.Min, bb.Max, IM_COL32(255, 255, 255, selected ? 70 : 30), 10.0f, 0, 1.0f);
        }
        if (selected) {
            ImU32 accent = ImGui::ColorConvertFloat4ToU32(ImVec4(cfg.accent_color[0], cfg.accent_color[1], cfg.accent_color[2], 1.0f));
            dl->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y + 8.0f), ImVec2(bb.Min.x + 3.5f, bb.Max.y - 8.0f), accent, 2.0f);
        }
        ImVec2 text_size = ImGui::CalcTextSize(label);
        dl->AddText(ImVec2(bb.Min.x + 18.0f, bb.Min.y + (size.y - text_size.y) * 0.5f), selected ? IM_COL32(255, 255, 255, 255) : IM_COL32(170, 175, 190, 220), label);
        return pressed;
    }

    inline void RenderWatermarkAndTrigger() {
        if (!cfg.misc_watermark) return;
        ImDrawList* bg_dl = ImGui::GetBackgroundDrawList();
        ImVec2 wm_min(24.0f, 24.0f);
        ImVec2 wm_max(285.0f, 60.0f);
        DrawLiquidGlassPanel(bg_dl, wm_min, wm_max, 18.0f, 0.95f);
        ImU32 accent = ImGui::ColorConvertFloat4ToU32(ImVec4(cfg.accent_color[0], cfg.accent_color[1], cfg.accent_color[2], 1.0f));
        bg_dl->AddCircleFilled(ImVec2(wm_min.x + 18.0f, wm_min.y + 18.0f), 4.5f, accent, 16);
        bg_dl->AddText(ImVec2(wm_min.x + 32.0f, wm_min.y + 10.0f), IM_COL32(245, 245, 255, 255), "OBSIDIAN.CC  |  LIQUID  |  120 FPS");
        ImVec2 mouse = ImGui::GetIO().MousePos;
        if (ImGui::IsMouseClicked(0) && mouse.x >= wm_min.x && mouse.x <= wm_max.x && mouse.y >= wm_min.y && mouse.y <= wm_max.y) {
            cfg.menu_open = !cfg.menu_open;
        }
    }

    inline void RenderClickGUI() {
        RenderWatermarkAndTrigger();
        if (!cfg.menu_open) return;
        ImGui::SetNextWindowSize(ImVec2(640.0f, 410.0f), ImGuiCond_Once);
        ImGui::Begin("##ObsidianLiquidRoot", &cfg.menu_open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p_min = ImGui::GetWindowPos();
        ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowWidth(), p_min.y + ImGui::GetWindowHeight());
        DrawLiquidGlassPanel(dl, p_min, p_max, 18.0f);
        dl->AddLine(ImVec2(p_min.x + 165.0f, p_min.y + 16.0f), ImVec2(p_min.x + 165.0f, p_max.y - 16.0f), IM_COL32(255, 255, 255, 24), 1.0f);
        ImGui::BeginGroup();
        {
            ImGui::Dummy(ImVec2(0, 6));
            ImGui::SetCursorPosX(28.0f);
            ImGui::TextColored(ImVec4(0.95f, 0.95f, 1.0f, 1.0f), "O B S I D I A N");
            ImGui::SetCursorPosX(28.0f);
            ImGui::TextColored(ImVec4(cfg.accent_color[0], cfg.accent_color[1], cfg.accent_color[2], 0.9f), "liquid glass v2.6");
            ImGui::Dummy(ImVec2(0, 18));
            LiquidTab("Combat", 0);
            LiquidTab("Visuals", 1);
            LiquidTab("Interface", 2);
        }
        ImGui::EndGroup();
        ImGui::SameLine(180.0f);
        ImGui::BeginGroup();
        {
            float col_w = 212.0f;
            float col_h = 376.0f;
            if (cfg.current_tab == 0) {
                BeginGlassGroup("Aimbot Core", ImVec2(col_w, col_h));
                LiquidToggle("Enable Aimbot", &cfg.aim_enable);
                LiquidToggle("Silent Vector", &cfg.aim_silent);
                LiquidToggle("Visibility Check", &cfg.aim_vis_check);
                LiquidSlider("FOV Radius", &cfg.aim_fov, 5.0f, 180.0f, "%.0f deg");
                LiquidSlider("Smooth Factor", &cfg.aim_smooth, 1.0f, 20.0f, "%.1f x");
                EndGlassGroup();
                ImGui::SameLine();
                BeginGlassGroup("Target & Filtering", ImVec2(col_w, col_h));
                LiquidToggle("Draw FOV Circle", &cfg.misc_fov_circle);
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.75f, 0.78f, 0.85f, 1.0f), "Hitbox Priority:");
                ImGui::RadioButton("Head", &cfg.aim_bone, 0);
                ImGui::RadioButton("Neck", &cfg.aim_bone, 1);
                ImGui::RadioButton("Pelvis", &cfg.aim_bone, 2);
                EndGlassGroup();
            } else if (cfg.current_tab == 1) {
                BeginGlassGroup("Player ESP", ImVec2(col_w, col_h));
                LiquidToggle("Enable ESP", &cfg.esp_enable);
                LiquidToggle("Frosted Glass Box", &cfg.esp_box_glass);
                LiquidToggle("Bone Skeleton", &cfg.esp_skeleton);
                LiquidToggle("Health Bar", &cfg.esp_health);
                LiquidToggle("Player NameTag", &cfg.esp_name);
                EndGlassGroup();
                ImGui::SameLine();
                BeginGlassGroup("World & Render", ImVec2(col_w, col_h));
                LiquidToggle("Chams / Outline", &cfg.esp_glow);
                LiquidSlider("Max Distance", &cfg.esp_render_dist, 20.0f, 300.0f, "%.0f m");
                EndGlassGroup();
            } else if (cfg.current_tab == 2) {
                BeginGlassGroup("Liquid Glass Theme", ImVec2(col_w, col_h));
                LiquidToggle("Show Watermark", &cfg.misc_watermark);
                LiquidSlider("Glass Opacity", &cfg.glass_opacity, 0.35f, 0.95f, "%.2f");
                EndGlassGroup();
                ImGui::SameLine();
                BeginGlassGroup("Accent & Glow", ImVec2(col_w, col_h));
                ImGui::TextColored(ImVec4(0.8f, 0.82f, 0.9f, 1.0f), "Neon Specular Tint:");
                ImGui::ColorPicker4("##AccentPicker", cfg.accent_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoInputs);
                EndGlassGroup();
            }
        }
        ImGui::EndGroup();
        ImGui::End();
    }
}
