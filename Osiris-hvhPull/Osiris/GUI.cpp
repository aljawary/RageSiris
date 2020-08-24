#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
#include "Hacks/SkinChanger.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "SDK/InputSystem.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
constexpr auto wposwindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

GUI::GUI() noexcept
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        static constexpr ImWchar ranges[]{ 0x0020, 0xFFFF, 0 };
        ImFontConfig cfg;
        cfg.OversampleV = 3;

        fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / "tahoma.ttf").string().c_str(), 15.0f, &cfg, ranges);
        fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / "segoeui.ttf").string().c_str(), 15.0f, &cfg, ranges);
    }
}

void GUI::render() noexcept
{
    if (!config->style.menuStyle) {
        renderMenuBar();
        renderAimbotWindow();
        renderAntiAimWindow();
        renderTriggerbotWindow();
        renderBacktrackWindow();
        renderGlowWindow();
        renderChamsWindow();
        renderEspWindow();
        renderVisualsWindow();
        renderSkinChangerWindow();
        renderSoundWindow();
        renderStyleWindow();
        renderMiscWindow();
        renderReportbotWindow();
        renderConfigWindow();
    } else {
        renderGuiStyle2();
    }
}

void GUI::updateColors() const noexcept
{
    switch (config->style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void GUI::hotkey(int& key) noexcept
{
    key ? ImGui::Text("[ %s ]", interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ key ]");

    if (!ImGui::IsItemHovered())
        return;

    ImGui::SetTooltip("Press any key to change keybind");
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(i) && i != config->misc.menuKey)
            key = i != VK_ESCAPE ? i : 0;

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config->misc.menuKey)
            key = i + (i > 1 ? 2 : 1);
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Aimbot", nullptr, &window.aimbot);
        ImGui::MenuItem("Anti aim", nullptr, &window.antiAim);
        ImGui::MenuItem("Triggerbot", nullptr, &window.triggerbot);
        ImGui::MenuItem("Backtrack", nullptr, &window.backtrack);
        ImGui::MenuItem("Glow", nullptr, &window.glow);
        ImGui::MenuItem("Chams", nullptr, &window.chams);
        ImGui::MenuItem("Esp", nullptr, &window.esp);
        ImGui::MenuItem("Visuals", nullptr, &window.visuals);
		ImGui::MenuItem("Misc", nullptr, &window.misc);
		ImGui::MenuItem("Reportbot", nullptr, &window.reportbot);
        ImGui::MenuItem("Skin changer", nullptr, &window.skinChanger);
        ImGui::MenuItem("Sound", nullptr, &window.sound);
        ImGui::MenuItem("Style", nullptr, &window.style);    
        ImGui::MenuItem("Config", nullptr, &window.config);
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[0] && config->style.menuStyle == 0) {
            ImGui::Begin("Aimbot", &window.aimbot, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Aimbot", &window.aimbot, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[0]) {
            if (config->misc.wposAimbotX != ImGui::GetWindowPos().x) { config->misc.wposAimbotX = ImGui::GetWindowPos().x; }
            if (config->misc.wposAimbotY != ImGui::GetWindowPos().y) { config->misc.wposAimbotY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposAimbotX || ImGui::GetWindowPos().y != config->misc.wposAimbotY) {
                ImGui::SetWindowPos({ config->misc.wposAimbotX, config->misc.wposAimbotY });
            }
        }
    }

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0");
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::PushID(1);

    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &config->aimbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox("On key", &config->aimbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->aimbot[currentWeapon].key);
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::PushItemWidth(70.0f);
    ImGui::Combo("", &config->aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Checkbox("Aimlock", &config->aimbot[currentWeapon].aimlock);
    ImGui::Checkbox("Silent", &config->aimbot[currentWeapon].silent);
    ImGui::Checkbox("Friendly fire", &config->aimbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("Visible only", &config->aimbot[currentWeapon].visibleOnly);
    ImGui::Checkbox("Scoped only", &config->aimbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("Ignore flash", &config->aimbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("Ignore smoke", &config->aimbot[currentWeapon].ignoreSmoke);
    ImGui::Checkbox("Auto shot", &config->aimbot[currentWeapon].autoShot);
    ImGui::Checkbox("Auto scope", &config->aimbot[currentWeapon].autoScope);
    ImGui::Combo("Bone", &config->aimbot[currentWeapon].bone, "Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
    ImGui::NextColumn();
    ImGui::PushItemWidth(240.0f);
    ImGui::SliderFloat("Fov", &config->aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", 2.5f);
    ImGui::SliderFloat("Smooth", &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("Recoil control x", &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Recoil control y", &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
    ImGui::SliderInt("Hit chance", &config->aimbot[currentWeapon].hitchance, 0, 100, "%d");
    ImGui::InputInt("Min damage", &config->aimbot[currentWeapon].minDamage);
    ImGui::SliderInt("First shot delay", &config->aimbot[currentWeapon].firstShotDelay, 0, 250, "%d ms");
    ImGui::SliderInt("Kill delay", &config->aimbot[currentWeapon].killDelay, 0, 500, "%d ms");
    config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("Killshot", &config->aimbot[currentWeapon].killshot);
    ImGui::Checkbox("Between shots", &config->aimbot[currentWeapon].betweenShots);
    ImGui::Checkbox("Standalone RCS", &config->aimbot[currentWeapon].standaloneRCS);
    ImGui::InputInt("Ignore Shots", &config->aimbot[currentWeapon].shotsFired);
    config->aimbot[currentWeapon].shotsFired = std::clamp(config->aimbot[currentWeapon].shotsFired, 0, 10);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderAntiAimWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.antiAim)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[1] && config->style.menuStyle == 0) {
            ImGui::Begin("Anti aim", &window.antiAim, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Anti aim", &window.antiAim, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[1]) {
            if (config->misc.wposAntiAimX != ImGui::GetWindowPos().x) { config->misc.wposAntiAimX = ImGui::GetWindowPos().x; }
            if (config->misc.wposAntiAimY != ImGui::GetWindowPos().y) { config->misc.wposAntiAimY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposAntiAimX || ImGui::GetWindowPos().y != config->misc.wposAntiAimY) {
                ImGui::SetWindowPos({ config->misc.wposAntiAimX, config->misc.wposAntiAimY });
            }
        }
    }

    ImGui::Checkbox("Enabled", &config->antiAim.enabled);
    if (config->antiAim.enabled)
    {
        ImGui::NewLine();
        ImGui::TextColored(ImColor(255, 255, 255), "Resolver");
        ImGui::Separator();
        ImGui::Checkbox("Resolver", &config->antiAim.resolveall);
        ImGui::Checkbox("Pitch Resolver", &config->antiAim.pitchResolver);
        ImGui::Separator(); 
        ImGui::Checkbox("Pitch", &config->antiAim.pitch);
        if (config->antiAim.pitch) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Pitch Angle", &config->antiAim.pitchAngle, -89.0f, 89.0f, "%.2f°", 1);
        }
        ImGui::Checkbox("Yaw", &config->antiAim.yaw);
        if (config->antiAim.yaw) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Yaw Angle", &config->antiAim.yawAngle, -179.0f, 179.0f, "%.2f°", 1);
            ImGui::Text("Invert Key");
            ImGui::SameLine();
            hotkey(config->antiAim.yawInverseAngleKey);
            ImGui::SameLine();
            ImGui::PushID(1);
            ImGui::SetNextItemWidth(75.0f);
            ImGui::Combo("", &config->antiAim.yawInverseKeyMode, "Hold\0Toggle\0");
            ImGui::PopID();
        }
        else
        {
            ImGui::Text("Invert Key");
            ImGui::SameLine();
            hotkey(config->antiAim.yawInverseAngleKey);
        }
        ImGui::Checkbox("Yaw Desync", &config->antiAim.yawReal);
        if (config->antiAim.yawReal == true)
        {
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Body Lean", &config->antiAim.bodyLean, -100.0f, 100.0f, "%.2f", 1);
            ImGui::Checkbox("LBY Breaker", &config->antiAim.LBYBreaker);
            if (config->antiAim.LBYBreaker)
            {
                ImGui::SliderFloat("LBY Angle", &config->antiAim.LBYAngle, -180.0f, 180.0f, "%.2f°", 1);
                ImGui::Checkbox("Fakewalk", &config->antiAim.fakeWalk.enabled);
                if (config->antiAim.fakeWalk.enabled) {
                    ImGui::SameLine();
                    hotkey(config->antiAim.fakeWalk.key);
                    ImGui::SameLine();
                    ImGui::PushID(2);
                    ImGui::SetNextItemWidth(75.0f);
                    ImGui::Combo("", &config->antiAim.fakeWalk.keyMode, "Hold\0Toggle\0");
                    ImGui::PopID();
                    ImGui::SetNextItemWidth(240.0f);
                    ImGui::SliderInt("Speed", &config->antiAim.fakeWalk.maxChoke, 3, 15);
                }
            }
        }
        ImGui::SetNextItemWidth(85.0f);
        ImGui::Combo("Anti-Aim Mode", &config->antiAim.mode, "Static\0Jitter\0");
        if (config->antiAim.mode == 1)
        {
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Jitter Max", &config->antiAim.jitterMax, -179.0f, 179.0f, "%.2f°", 1);
            ImGui::SetNextItemWidth(240.0f);
            ImGui::SliderFloat("Jitter Min", &config->antiAim.jitterMin, -179.0f, 179.0f, "%.2f°", 1);
        }
    }
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.triggerbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[2] && config->style.menuStyle == 0) {
            ImGui::Begin("Triggerbot", &window.triggerbot, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Triggerbot", &window.triggerbot, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[2]) {
            if (config->misc.wposTriggerBotX != ImGui::GetWindowPos().x) { config->misc.wposTriggerBotX = ImGui::GetWindowPos().x; }
            if (config->misc.wposTriggerBotY != ImGui::GetWindowPos().y) { config->misc.wposTriggerBotY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposTriggerBotX || ImGui::GetWindowPos().y != config->misc.wposTriggerBotY) {
                ImGui::SetWindowPos({ config->misc.wposTriggerBotX, config->misc.wposTriggerBotY });
            }
        }
    }

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0");
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::PushID(1);
    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 5:
        currentWeapon = 39;
        ImGui::NewLine();
        break;

    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &config->triggerbot[currentWeapon].enabled);
    ImGui::Separator();
    ImGui::Checkbox("On key", &config->triggerbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->triggerbot[currentWeapon].key);
    ImGui::Checkbox("Hitchance", &config->triggerbot[currentWeapon].hitChance);
    ImGui::SliderInt("Ammount", &config->triggerbot[currentWeapon].hitChanceAmmount, 0, 100, "%d %");
    ImGui::Checkbox("Friendly fire", &config->triggerbot[currentWeapon].friendlyFire);
    ImGui::Checkbox("Scoped only", &config->triggerbot[currentWeapon].scopedOnly);
    ImGui::Checkbox("Ignore flash", &config->triggerbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox("Ignore smoke", &config->triggerbot[currentWeapon].ignoreSmoke);
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo("Hitgroup", &config->triggerbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("Shot delay", &config->triggerbot[currentWeapon].shotDelay, 0, 250, "%d ms");
    ImGui::InputInt("Min damage", &config->triggerbot[currentWeapon].minDamage);
    config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
    ImGui::Checkbox("Killshot", &config->triggerbot[currentWeapon].killshot);
    ImGui::SliderFloat("Burst Time", &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, "%.3f s");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderBacktrackWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.backtrack)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[3] && config->style.menuStyle == 0) {
            ImGui::Begin("Backtrack", &window.backtrack, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Backtrack", &window.backtrack, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[3]) {
            if (config->misc.wposBacktrackX != ImGui::GetWindowPos().x) { config->misc.wposBacktrackX = ImGui::GetWindowPos().x; }
            if (config->misc.wposBacktrackY != ImGui::GetWindowPos().y) { config->misc.wposBacktrackY = ImGui::GetWindowPos().y; }
        } else {
            if (ImGui::GetWindowPos().x != config->misc.wposBacktrackX || ImGui::GetWindowPos().y != config->misc.wposBacktrackY) {
                ImGui::SetWindowPos({ config->misc.wposBacktrackX, config->misc.wposBacktrackY });
            }
        }
    }

    ImGui::Checkbox("Enabled", &config->backtrack.enabled);
    ImGui::Checkbox("Ignore smoke", &config->backtrack.ignoreSmoke);
    ImGui::Checkbox("Recoil based fov", &config->backtrack.recoilBasedFov);
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("Time limit", &config->backtrack.timeLimit, 1, 200, "%d ms");
    ImGui::PopItemWidth();
    ImGui::Checkbox("Enabled Fake Latency", &config->backtrack.fakeLatency);
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderInt("Latency Ammount", &config->backtrack.fakeLatencyAmmount, 1, 200, "%d ms");
    ImGui::PopItemWidth();
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.glow)
            return;
        ImGui::SetNextWindowSize({ 450.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[4] && config->style.menuStyle == 0) {
            ImGui::Begin("Glow", &window.glow, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Glow", &window.glow, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[4]) {
            if (config->misc.wposGlowX != ImGui::GetWindowPos().x) { config->misc.wposGlowX = ImGui::GetWindowPos().x; }
            if (config->misc.wposGlowY != ImGui::GetWindowPos().y) { config->misc.wposGlowY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposGlowX || ImGui::GetWindowPos().y != config->misc.wposGlowY) {
                ImGui::SetWindowPos({ config->misc.wposGlowX, config->misc.wposGlowY });
            }
        }
    }

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0");
    ImGui::PopID();
    static int currentItem{ 0 };
    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0");
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();
    ImGui::Checkbox("Enabled", &config->glow[currentItem].enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 150.0f);
    ImGui::Checkbox("Health based", &config->glow[currentItem].healthBased);

    ImGuiCustom::colorPicker("Color", config->glow[currentItem].color.color, nullptr, &config->glow[currentItem].color.rainbow, &config->glow[currentItem].color.rainbowSpeed);

    ImGui::NextColumn();
    ImGui::PushItemWidth(220.0f);
    ImGui::SliderFloat("Thickness", &config->glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Alpha", &config->glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
    ImGui::SliderInt("Style", &config->glow[currentItem].style, 0, 3);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[5] && config->style.menuStyle == 0) {
            ImGui::Begin("Chams", &window.chams, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Chams", &window.chams, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[5]) {
            if (config->misc.wposChamsX != ImGui::GetWindowPos().x) { config->misc.wposChamsX = ImGui::GetWindowPos().x; }
            if (config->misc.wposChamsY != ImGui::GetWindowPos().y) { config->misc.wposChamsY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposChamsX || ImGui::GetWindowPos().y != config->misc.wposChamsY) {
                ImGui::SetWindowPos({ config->misc.wposChamsX, config->misc.wposChamsY });
            }
        }
    }

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    static int material = 1;

    if (ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0"))
        material = 1;

    ImGui::PopID();
    static int currentItem{ 0 };

    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        if (ImGui::Combo("", &currentType, "All\0Visible\0Occluded\0"))
            material = 1;
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    ImGui::SameLine();

    if (material <= 1)
        ImGuiCustom::arrowButtonDisabled("##left", ImGuiDir_Left);
    else if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        --material;

    ImGui::SameLine();
    ImGui::Text("%d", material);
    ImGui::SameLine();

    if (material >= int(config->chams[0].materials.size()))
        ImGuiCustom::arrowButtonDisabled("##right", ImGuiDir_Right);
    else if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        ++material;

    ImGui::SameLine();
    auto& chams{ config->chams[currentItem].materials[material - 1] };

    ImGui::Checkbox("Enabled", &chams.enabled);
    ImGui::Separator();
    ImGui::Checkbox("Health based", &chams.healthBased);
    ImGui::Checkbox("Blinking", &chams.blinking);
    ImGui::Combo("Material", &chams.material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0Pearlescent\0Metallic\0");
    ImGui::Checkbox("Wireframe", &chams.wireframe);
    ImGuiCustom::colorPopup("Color", chams.color, &chams.rainbow, &chams.rainbowSpeed);

    if (!contentOnly) {
        ImGui::End();
    }
}

void GUI::renderEspWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.esp)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[6] && config->style.menuStyle == 0) {
            ImGui::Begin("Esp", &window.esp, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Esp", &window.esp, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[6]) {
            if (config->misc.wposEspX != ImGui::GetWindowPos().x) { config->misc.wposEspX = ImGui::GetWindowPos().x; }
            if (config->misc.wposEspY != ImGui::GetWindowPos().y) { config->misc.wposEspY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposEspX || ImGui::GetWindowPos().y != config->misc.wposEspY) {
                ImGui::SetWindowPos({ config->misc.wposEspX, config->misc.wposEspY });
            }
        }
    }
    static int currentCategory = 0;
    static int currentItem = 0;

    if (ImGui::ListBoxHeader("##", { 125.0f, 300.0f })) {
        static constexpr const char* players[]{ "All", "Visible", "Occluded" };

        ImGui::Text("Allies");
        ImGui::Indent();
        ImGui::PushID("Allies");
        ImGui::PushFont(fonts.segoeui);

        for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
            bool isSelected = currentCategory == 0 && currentItem == i;

            if ((i == 0 || !config->esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                currentItem = i;
                currentCategory = 0;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();
        ImGui::Text("Enemies");
        ImGui::Indent();
        ImGui::PushID("Enemies");
        ImGui::PushFont(fonts.segoeui);

        for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
            bool isSelected = currentCategory == 1 && currentItem == i;

            if ((i == 0 || !config->esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                currentItem = i;
                currentCategory = 1;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();
        if (bool isSelected = currentCategory == 2; ImGui::Selectable("Weapons", isSelected))
            currentCategory = 2;

        ImGui::Text("Projectiles");
        ImGui::Indent();
        ImGui::PushID("Projectiles");
        ImGui::PushFont(fonts.segoeui);
        static constexpr const char* projectiles[]{ "Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball" };

        for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++) {
            bool isSelected = currentCategory == 3 && currentItem == i;

            if (ImGui::Selectable(projectiles[i], isSelected)) {
                currentItem = i;
                currentCategory = 3;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::Unindent();

        ImGui::Text("Danger Zone");
        ImGui::Indent();
        ImGui::PushID("Danger Zone");
        ImGui::PushFont(fonts.segoeui);
        static constexpr const char* dangerZone[]{ "Sentries", "Drones", "Cash", "Cash Dufflebag", "Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Full Armor", "Armor", "Helmet", "Parachute", "Briefcase", "Tablet Upgrade", "ExoJump", "Ammobox", "Radar Jammer" };

        for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
            bool isSelected = currentCategory == 4 && currentItem == i;

            if (ImGui::Selectable(dangerZone[i], isSelected)) {
                currentItem = i;
                currentCategory = 4;
            }
        }

        ImGui::PopFont();
        ImGui::PopID();
        ImGui::ListBoxFooter();
    }
    ImGui::SameLine();
    if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
        switch (currentCategory) {
        case 0:
        case 1: {
            int selected = currentCategory * 3 + currentItem;
            ImGui::Checkbox("Enabled", &config->esp.players[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config->esp.players[selected].font, 1, 294);
            config->esp.players[selected].font = std::clamp(config->esp.players[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 185.0f };
            ImGuiCustom::colorPicker("Snaplines", config->esp.players[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config->esp.players[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.players[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
            ImGuiCustom::colorPicker("Eye traces", config->esp.players[selected].eyeTraces);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Health", config->esp.players[selected].health);
            ImGuiCustom::colorPicker("Head dot", config->esp.players[selected].headDot);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Health bar", config->esp.players[selected].healthBar);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(70.f);
            ImGui::Combo("##HP side", &config->esp.players[selected].hpside, "Left\0Bottom\0Right\0");
            ImGui::PushID("hotfix");
            ImGuiCustom::colorPicker("Name", config->esp.players[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Armor", config->esp.players[selected].armor);
            ImGuiCustom::colorPicker("Money", config->esp.players[selected].money);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Armor bar", config->esp.players[selected].armorBar);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(70.f);
            ImGui::PopID();
            ImGui::Combo("##AR side", &config->esp.players[selected].armorside, "Left\0Bottom\0Right\0");
            ImGuiCustom::colorPicker("Outline", config->esp.players[selected].outline);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Distance", config->esp.players[selected].distance);
            ImGuiCustom::colorPicker("Active Weapon", config->esp.players[selected].activeWeapon);
            ImGui::SameLine(spacing);
            ImGui::Checkbox("Dead ESP", &config->esp.players[selected].deadesp);
            ImGui::SliderFloat("Max distance", &config->esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 2: {
            ImGui::Checkbox("Enabled", &config->esp.weapon.enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config->esp.weapon.font, 1, 294);
            config->esp.weapon.font = std::clamp(config->esp.weapon.font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config->esp.weapon.snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config->esp.weapon.box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.weapon.boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
            ImGuiCustom::colorPicker("Name", config->esp.weapon.name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Ammo", config->esp.weapon.ammo);
            ImGuiCustom::colorPicker("Outline", config->esp.weapon.outline);
            ImGuiCustom::colorPicker("Distance", config->esp.weapon.distance);
            ImGui::SliderFloat("Max distance", &config->esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 3: {
            ImGui::Checkbox("Enabled", &config->esp.projectiles[currentItem].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config->esp.projectiles[currentItem].font, 1, 294);
            config->esp.projectiles[currentItem].font = std::clamp(config->esp.projectiles[currentItem].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config->esp.projectiles[currentItem].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config->esp.projectiles[currentItem].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.projectiles[currentItem].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
            ImGuiCustom::colorPicker("Name", config->esp.projectiles[currentItem].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Outline", config->esp.projectiles[currentItem].outline);
            ImGuiCustom::colorPicker("Distance", config->esp.projectiles[currentItem].distance);
            ImGui::SliderFloat("Max distance", &config->esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        case 4: {
            int selected = currentItem;
            ImGui::Checkbox("Enabled", &config->esp.dangerZone[selected].enabled);
            ImGui::SameLine(0.0f, 50.0f);
            ImGui::SetNextItemWidth(85.0f);
            ImGui::InputInt("Font", &config->esp.dangerZone[selected].font, 1, 294);
            config->esp.dangerZone[selected].font = std::clamp(config->esp.dangerZone[selected].font, 1, 294);

            ImGui::Separator();

            constexpr auto spacing{ 200.0f };
            ImGuiCustom::colorPicker("Snaplines", config->esp.dangerZone[selected].snaplines);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Box", config->esp.dangerZone[selected].box);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("", &config->esp.dangerZone[selected].boxType, "2D\0""2D corners\0""3D\0""3D corners\0");
            ImGuiCustom::colorPicker("Name", config->esp.dangerZone[selected].name);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Outline", config->esp.dangerZone[selected].outline);
            ImGuiCustom::colorPicker("Distance", config->esp.dangerZone[selected].distance);
            ImGui::SliderFloat("Max distance", &config->esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
            break;
        }
        }

        ImGui::EndChild();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderVisualsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.visuals)
            return;
        ImGui::SetNextWindowSize({ 680.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[7] && config->style.menuStyle == 0) {
            ImGui::Begin("Visuals", &window.visuals, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Visuals", &window.visuals, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[7]) {
            if (config->misc.wposVisualsX != ImGui::GetWindowPos().x) { config->misc.wposVisualsX = ImGui::GetWindowPos().x; }
            if (config->misc.wposVisualsY != ImGui::GetWindowPos().y) { config->misc.wposVisualsY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposVisualsX || ImGui::GetWindowPos().y != config->misc.wposVisualsY) {
                ImGui::SetWindowPos({ config->misc.wposVisualsX, config->misc.wposVisualsY });
            }
        }
    }
    
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 280.0f);
    constexpr auto playerModels = "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0Pirate\0Pirate Variant A\0Pirate Variant B\0Pirate Variant C\0Pirate Variant D\0Anarchist\0Anarchist Variant A\0Anarchist Variant B\0Anarchist Variant C\0Anarchist Variant D\0Balkan Variant A\0Balkan Variant B\0Balkan Variant C\0Balkan Variant D\0Balkan Variant E\0Jumpsuit Variant A\0Jumpsuit Variant B\0Jumpsuit Variant C\0";
    ImGui::Combo("T Player Model", &config->visuals.playerModelT, playerModels);
    ImGui::Combo("CT Player Model", &config->visuals.playerModelCT, playerModels);
    ImGui::Checkbox("Disable post-processing", &config->visuals.disablePostProcessing);
    ImGui::Checkbox("Inverse ragdoll gravity", &config->visuals.inverseRagdollGravity);
    ImGui::Checkbox("No fog", &config->visuals.noFog);
    ImGui::Checkbox("No 3d sky", &config->visuals.no3dSky);
    ImGui::Checkbox("No aim punch", &config->visuals.noAimPunch);
    ImGui::Checkbox("No view punch", &config->visuals.noViewPunch);
    ImGui::Checkbox("No hands", &config->visuals.noHands);
    ImGui::Checkbox("No sleeves", &config->visuals.noSleeves);
    ImGui::Checkbox("No weapons", &config->visuals.noWeapons);
    ImGui::Checkbox("No smoke", &config->visuals.noSmoke);
    ImGui::Checkbox("No blur", &config->visuals.noBlur);
    ImGui::Checkbox("No bloom", &config->visuals.noBloom);
    ImGui::Checkbox("No scope overlay", &config->visuals.noScopeOverlay);
    ImGui::Checkbox("No grass", &config->visuals.noGrass);
    ImGui::Checkbox("No shadows", &config->visuals.noShadows);
    ImGui::Checkbox("Wireframe smoke", &config->visuals.wireframeSmoke);
    ImGui::NextColumn();
    ImGui::Checkbox("Zoom", &config->visuals.zoom);
    ImGui::SameLine();
    hotkey(config->visuals.zoomKey);
    ImGui::Checkbox("Thirdperson", &config->visuals.thirdperson);
    ImGui::SameLine();
    hotkey(config->visuals.thirdpersonKey);
    ImGui::SetNextItemWidth(140.0f);
    ImGui::Combo("Thirdperson Angles", &config->antiAim.thirdpersonMode, "Fake\0Real\0Current Tick\0");
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(0);
    ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "Thirdperson distance: %d");
    ImGui::PopID();
    ImGui::PushID(1);
    ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
    ImGui::PopID();
    ImGui::PushID(2);
    ImGui::SliderInt("", &config->visuals.fov, -60, 60, "FOV: %d");
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "Far Z: %d");
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "Flash reduction: %d%%");
    ImGui::PopID();
    ImGui::PushID(5);
    ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::Combo("Skybox", &config->visuals.skybox, "Default\0Custom\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
    if (config->visuals.skybox == 1) {
        ImGui::InputText("Skybox filename", &config->visuals.customSkybox);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("skybox file must be put in csgo/materials/skybox directory");
    }    ImGuiCustom::colorPicker("World color", config->visuals.world);
    ImGuiCustom::colorPicker("Sky color", config->visuals.sky);
    ImGui::Checkbox("Deagle spinner", &config->visuals.deagleSpinner);
    ImGui::Combo("Screen effect", &config->visuals.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::Combo("Hit effect", &config->visuals.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::SliderFloat("Hit effect time", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Combo("Hit marker", &config->visuals.hitMarker, "None\0Default (Cross)\0");
    ImGui::SliderFloat("Hit marker time", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
    ImGuiCustom::colorPicker("Hit marker Damage Indicator", config->visuals.hitMarkerDamageIndicator);
    if (config->visuals.hitMarkerDamageIndicator.enabled    ) {
        ImGui::SameLine();
        ImGui::Checkbox("Customize Hitmarker", &config->visuals.hitMarkerDamageIndicatorCustomize);
        if (config->visuals.hitMarkerDamageIndicatorCustomize) {
            ImGui::InputInt("Font", &config->visuals.hitMarkerDamageIndicatorFont, 1, 294);
            ImGui::InputInt("Alpha", &config->visuals.hitMarkerDamageIndicatorAlpha, 1, 1000);
            ImGui::InputInt("Dist", &config->visuals.hitMarkerDamageIndicatorDist, -100, 100);
            ImGui::InputInt("Text X", &config->visuals.hitMarkerDamageIndicatorTextX, -100, 100);
            ImGui::InputInt("Text Y", &config->visuals.hitMarkerDamageIndicatorTextY, -100, 100);
            ImGui::PushID(15);
            ImGui::SliderFloat(" ", &config->visuals.hitMarkerDamageIndicatorRatio, -1.0f, 1.0f, "Ratio: %.2f");
            ImGui::PopID();
        };
    };
    ImGui::Checkbox("Indicators", &config->visuals.indicatorsEnabled);
    ImGui::SameLine();
    ImGui::PushID(6);
    ImGuiCustom::MultiCombo("", config->visuals.indicators, config->visuals.selectedIndicators, 4);
    ImGui::PopID();
    ImGuiCustom::colorPicker("Bullet Tracers", config->visuals.bulletTracers);
    
    ImGui::Checkbox("Color correction", &config->visuals.colorCorrection.enabled);
    ImGui::SameLine();
    bool ccPopup = ImGui::Button("Edit");

    if (ccPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }
    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSkinChangerWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.skinChanger)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[8] && config->style.menuStyle == 0) {
            ImGui::Begin("nSkinz", &window.skinChanger, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("nSkinz", &window.skinChanger, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[8]) {
            if (config->misc.wposSkinchangerX != ImGui::GetWindowPos().x) { config->misc.wposSkinchangerX = ImGui::GetWindowPos().x; }
            if (config->misc.wposSkinchangerY != ImGui::GetWindowPos().y) { config->misc.wposSkinchangerY = ImGui::GetWindowPos().y; }
        }
        else {
            if (ImGui::GetWindowPos().x != config->misc.wposSkinchangerX || ImGui::GetWindowPos().y != config->misc.wposSkinchangerY) {
                ImGui::SetWindowPos({ config->misc.wposSkinchangerX, config->misc.wposSkinchangerY });
            }
        }
    }

    static auto itemIndex = 0;

    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
        *out_text = game_data::weapon_names[idx].name;
        return true;
        }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
    ImGui::PopItemWidth();

    auto& selected_entry = config->skinChanger[itemIndex];
    selected_entry.itemIdIndex = itemIndex;

    {
        ImGui::SameLine();
        ImGui::Checkbox("Enabled", &selected_entry.enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);
        ImGui::InputInt("Seed", &selected_entry.seed);
        ImGui::InputInt("StatTrak\u2122", &selected_entry.stat_trak);
        selected_entry.stat_trak = (std::max)(selected_entry.stat_trak, -1);
        ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

        ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
            return true;
            }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

        ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = game_data::quality_names[idx].name;
            return true;
            }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

        if (itemIndex == 0) {
            ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::knife_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
        } else if (itemIndex == 1) {
            ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::glove_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
        } else {
            static auto unused_value = 0;
            selected_entry.definition_override_vector_index = 0;
            ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
        }

        ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
    }

    ImGui::NextColumn();

    {
        ImGui::PushID("sticker");

        static auto selectedStickerSlot = 0;

        ImGui::PushItemWidth(-1);

        if (ImGui::ListBoxHeader("", 5)) {
            for (int i = 0; i < 5; ++i) {
                ImGui::PushID(i);

                const auto kit_vector_index = config->skinChanger[itemIndex].stickers[i].kit_vector_index;
                const std::string text = '#' + std::to_string(i + 1) + "  " + SkinChanger::stickerKits[kit_vector_index].name;

                if (ImGui::Selectable(text.c_str(), i == selectedStickerSlot))
                    selectedStickerSlot = i;

                ImGui::PopID();
            }
            ImGui::ListBoxFooter();
        }

        ImGui::PopItemWidth();

        auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

        ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = SkinChanger::stickerKits[idx].name.c_str();
            return true;
            }, nullptr, SkinChanger::stickerKits.size(), 10);

        ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
        ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
        ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

        ImGui::PopID();
    }
    selected_entry.update();

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button("Update", { 130.0f, 30.0f }))
        SkinChanger::scheduleHudUpdate();

    ImGui::TextUnformatted("nSkinz by namazso");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSoundWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.sound)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[9] && config->style.menuStyle == 0) {
            ImGui::Begin("Sound", &window.sound, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Sound", &window.sound, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[9]) {
            if (config->misc.wposSoundX != ImGui::GetWindowPos().x) { config->misc.wposSoundX = ImGui::GetWindowPos().x; }
            if (config->misc.wposSoundY != ImGui::GetWindowPos().y) { config->misc.wposSoundY = ImGui::GetWindowPos().y; }
        }
        else {
            if (ImGui::GetWindowPos().x != config->misc.wposSoundX || ImGui::GetWindowPos().y != config->misc.wposSoundY) {
                ImGui::SetWindowPos({ config->misc.wposSoundX, config->misc.wposSoundY });
            }
        }
    }

    ImGui::SliderInt("Chicken volume", &config->sound.chickenVolume, 0, 200, "%d%%");

    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("", &currentCategory, "Local player\0Allies\0Enemies\0");
    ImGui::PopItemWidth();
    ImGui::SliderInt("Master volume", &config->sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Headshot volume", &config->sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Weapon volume", &config->sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
    ImGui::SliderInt("Footstep volume", &config->sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderStyleWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.style)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[10] && config->style.menuStyle == 0) {
            ImGui::Begin("Style", &window.style, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Style", &window.style, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[10])
        {
            if (config->misc.wposStyleX != ImGui::GetWindowPos().x) { config->misc.wposStyleX = ImGui::GetWindowPos().x; }
            if (config->misc.wposStyleY != ImGui::GetWindowPos().y) { config->misc.wposStyleY = ImGui::GetWindowPos().y; }
        
        }else {
            if (ImGui::GetWindowPos().x != config->misc.wposStyleX || ImGui::GetWindowPos().y != config->misc.wposStyleY) {
                ImGui::SetWindowPos({ config->misc.wposStyleX, config->misc.wposStyleY });
            }
        }
    }

    ImGui::PushItemWidth(150.0f);
    if (ImGui::Combo("Menu style", &config->style.menuStyle, "Classic\0One window\0"))
        window = { };
    if (ImGui::Combo("Menu colors", &config->style.menuColors, "Dark\0Light\0Classic\0Custom\0"))
        updateColors();
    ImGui::PopItemWidth();

    if (config->style.menuColors == 3) {
        ImGuiStyle& style = ImGui::GetStyle();
        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            if (i && i & 3) ImGui::SameLine(220.0f * (i & 3));

            ImGuiCustom::colorPicker(ImGui::GetStyleColorName(i), (float*)&style.Colors[i]);
        }
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderMiscWindow(bool contentOnly) noexcept
{
    static const char* Spammers[]{ "None", "Breaker Chat", "alsware.cc" };

    if (!contentOnly) {
        if (!window.misc)
            return;
        ImGui::SetNextWindowSize({ 580.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[11] && config->style.menuStyle == 0) {
            ImGui::Begin("Misc", &window.misc, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Misc", &window.misc, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[11]) {
            if (config->misc.wposMiscX != ImGui::GetWindowPos().x) { config->misc.wposMiscX = ImGui::GetWindowPos().x; }
            if (config->misc.wposMiscY != ImGui::GetWindowPos().y) { config->misc.wposMiscY = ImGui::GetWindowPos().y; }
        }else{
      
            if (ImGui::GetWindowPos().x != config->misc.wposMiscX || ImGui::GetWindowPos().y != config->misc.wposMiscY) {
                ImGui::SetWindowPos({ config->misc.wposMiscX, config->misc.wposMiscY });
            }
        }
    }

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 230.0f);
    ImGui::TextUnformatted("Menu key");
    ImGui::SameLine();
    hotkey(config->misc.menuKey);

    ImGui::Checkbox("Anti AFK kick", &config->misc.antiAfkKick);
    //ImGui::Checkbox("Resolver", &config->misc.resolver);
    ImGui::Checkbox("Auto strafe", &config->misc.autoStrafe);
    ImGui::SameLine();
    hotkey(config->misc.autoStrafeKey);
    ImGui::Combo("Strafe Style", &config->misc.autoStrafeStyle, "Legit\0Normal\0");
    ImGui::Checkbox("Bunny hop", &config->misc.bunnyHop);
    ImGui::Checkbox("Fast duck", &config->misc.fastDuck);
    ImGui::Checkbox("Fast Stop", &config->misc.fastStop);
    ImGui::Checkbox("Door Spam", &config->misc.usespam);
    ImGui::Checkbox("BlockBot", &config->misc.playerBlocker);
    ImGui::SameLine();
    hotkey(config->misc.playerBlockerKey); 

    ImGui::Checkbox("Custom View", &config->misc.CustomViewModel);
    if (config->misc.CustomViewModel) {
        ImGui::PushID(0);
        ImGui::SliderFloat(" ", &config->misc.viewmodel_x, -60.0f, 60.0f, "View x: %.1f");
        ImGui::PopID();
        ImGui::PushID(1);
        ImGui::SliderFloat(" ", &config->misc.viewmodel_y, -60.0f, 60.0f, "View y: %.1f");
        ImGui::PopID();
        ImGui::PushID(2);
        ImGui::SliderFloat(" ", &config->misc.viewmodel_z, -60.0f, 60.0f, "View z: %.1f");
        ImGui::PopID();
    };

    ImGui::Checkbox("Moonwalk", &config->misc.moonwalk);
    ImGui::Checkbox("Jump Bug", &config->misc.jumpbug);
    ImGui::SameLine();
    hotkey(config->misc.jumpbugkey);
    ImGui::Checkbox("Edge Jump", &config->misc.edgejump);
    ImGui::SameLine();
    hotkey(config->misc.edgejumpkey);
    ImGui::Checkbox("Slowwalk", &config->misc.slowwalk);
    ImGui::SameLine();
    hotkey(config->misc.slowwalkKey);
    ImGui::Checkbox("Sniper crosshair", &config->misc.sniperCrosshair);
    ImGui::Checkbox("Recoil crosshair", &config->misc.recoilCrosshair);
    ImGui::Checkbox("Auto pistol", &config->misc.autoPistol);
    ImGui::Checkbox("Auto reload", &config->misc.autoReload);
    ImGui::Checkbox("Auto accept", &config->misc.autoAccept);
    ImGui::Checkbox("Radar hack", &config->misc.radarHack);
    ImGui::Checkbox("Reveal ranks", &config->misc.revealRanks);
    ImGui::Checkbox("Reveal money", &config->misc.revealMoney);
    ImGui::Checkbox("Reveal suspect", &config->misc.revealSuspect);

    ImGui::Checkbox("Spectator list", &config->misc.spectatorList);
    if (config->misc.spectatorList) 
    {
        ImGui::SameLine();
        ImGui::Checkbox("Lock and save Window Position", &config->misc.wposLockSelectedFlags[16]);
    }

    ImGuiCustom::colorPicker("Watermark", config->misc.watermark);
    ImGui::Checkbox("Fix animation LOD", &config->misc.fixAnimationLOD);
    ImGui::Checkbox("Fix bone matrix", &config->misc.fixBoneMatrix);
    ImGui::Checkbox("Fix movement", &config->misc.fixMovement);
    ImGui::Checkbox("Disable model occlusion", &config->misc.disableModelOcclusion);
    ImGui::SliderFloat("Aspect Ratio", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
    ImGui::NextColumn();
    ImGui::Checkbox("Disable HUD blur", &config->misc.disablePanoramablur);
    ImGui::Combo("", &config->misc.tagAnimType, "SortedAnimation\0ClockTag\0OsirisWare\0gamesense\0");
    ImGui::Checkbox("Custom clantag", &config->misc.customClanTag);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);

    if (ImGui::InputText("", config->misc.clanTag, sizeof(config->misc.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();
    ImGui::Checkbox("Kill message", &config->misc.killMessage);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(1);
    ImGui::InputText("", &config->misc.killMessageString);
    ImGui::PopID();
    ImGui::Checkbox("Name stealer", &config->misc.nameStealer);
    ImGui::PushID(3);
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo("", &config->misc.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(4);
    ImGui::InputText("", &config->misc.banText);
    ImGui::PopID();
    ImGui::SameLine();
    if (ImGui::Button("Setup fake ban"))
        Misc::fakeBan(true);
    ImGui::Checkbox("Fast plant", &config->misc.fastPlant);
    ImGuiCustom::colorPicker("Bomb timer", config->misc.bombTimer);
    ImGui::Checkbox("Bomb Damage", &config->misc.bombDmg);
    ImGui::Checkbox("Quick reload", &config->misc.quickReload);
    ImGui::Checkbox("Prepare revolver", &config->misc.prepareRevolver);
    ImGui::SameLine();
    hotkey(config->misc.prepareRevolverKey);
 

    ImGui::Combo("Hit Sound", &config->misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (config->misc.hitSound == 5) {
        ImGui::InputText("Hit Sound filename", &config->misc.customHitSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PushID(5);
    ImGui::Combo("Kill Sound", &config->misc.killSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (config->misc.killSound == 5) {
        ImGui::InputText("Kill Sound filename", &config->misc.customKillSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PopID();
    ImGui::Combo("Fake Lag", &config->misc.fakeLagMode, "Off\0Normal\0Adaptive\0Random\0Switch");
    ImGui::SameLine();
    hotkey(config->misc.fakeLagKey);
    if (!(config->misc.fakeLagMode == 0))
    {
        ImGuiCustom::MultiCombo("Flags", config->misc.fakeLagFlags, config->misc.fakeLagSelectedFlags, 4);
        if (config->misc.fakeLagMode == 3)
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Min Fakelag Amount", &config->misc.fakeLagTicks, 1, 16);
        }
        else if (!(config->misc.fakeLagMode == 4))
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Fakelag Amount", &config->misc.fakeLagTicks, 1, 16);
        }
    }
    ImGui::Text("Quick healthshot");
    ImGui::SameLine();
    hotkey(config->misc.quickHealthshotKey);
    ImGui::Checkbox("Grenade Prediction", &config->misc.nadePredict);
    ImGui::Checkbox("Fix tablet signal", &config->misc.fixTabletSignal);
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Max angle delta", &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
    ImGui::Checkbox("Fake prime", &config->misc.fakePrime);
    ImGui::Checkbox("Sv pure bypass", &config->misc.svpurebypass);
    ImGui::Checkbox("Draw aimbot FOV", &config->misc.drawAimbotFov);
    ImGui::Checkbox("Zeusbot", &config->misc.autoZeus);
    if (config->misc.autoZeus)
    {
        ImGui::SetNextItemWidth(120.0f);
        ImGui::SliderInt("Zeus Max Wall Penetration Distance", &config->misc.autoZeusMaxPenDist, 0, 50);
        config->misc.autoZeusMaxPenDist = std::clamp(config->misc.autoZeusMaxPenDist, 0, 50);
        ImGui::Checkbox("Zeusbot BAIM Only", &config->misc.autoZeusBaimOnly);
    }
    ImGui::Checkbox("Fakeduck", &config->misc.fakeDuck);
    ImGui::SameLine();
    hotkey(config->misc.fakeDuckKey);
    ImGui::Checkbox("Purchase List", &config->misc.purchaseList.enabled);
    ImGui::SameLine();

    ImGui::PushID("Purchase List");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo("Mode", &config->misc.purchaseList.mode, "Details\0Summary\0");
        ImGui::Checkbox("Only During Freeze Time", &config->misc.purchaseList.onlyDuringFreezeTime);
        ImGui::Checkbox("Show Prices", &config->misc.purchaseList.showPrices);
        ImGui::Checkbox("No Title Bar", &config->misc.purchaseList.noTitleBar);
        ImGui::Checkbox("Lock/Save Scale/Position", &config->misc.wposLockSelectedFlags[15]);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (config->style.menuStyle != 1) {
        ImGuiCustom::MultiCombo("Save and lock window position", config->misc.wposLockFlags, config->misc.wposLockSelectedFlags, 14);
    }
    else { ImGui::Checkbox("Save and lock window position", &config->misc.wposLockSelectedFlags[14]); }

    if (ImGui::Button("Unhook"))
        hooks->uninstall();

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();

    ImGui::SetCursorPos(ImVec2{ 370 , 530 }); //left and right, up and down
    ImGui::BeginChild("##4", ImVec2{ 200, 150 }, true);
    {
        ImGui::Combo("Spammer", &config->misc.chat_spammer, Spammers, IM_ARRAYSIZE(Spammers));
    }
    ImGui::EndChild();

}

void GUI::renderReportbotWindow(bool contentOnly) noexcept
{   
    if (!contentOnly) {
        if (!window.reportbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        if (config->misc.wposLockSelectedFlags[12] && config->style.menuStyle == 0) {
            ImGui::Begin("Reportbot", &window.reportbot, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Reportbot", &window.reportbot, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[12]) {
            if (config->misc.wposReportbotX != ImGui::GetWindowPos().x) { config->misc.wposReportbotX = ImGui::GetWindowPos().x; }
            if (config->misc.wposReportbotY != ImGui::GetWindowPos().y) { config->misc.wposReportbotY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposReportbotX || ImGui::GetWindowPos().y != config->misc.wposReportbotY) {
                ImGui::SetWindowPos({ config->misc.wposReportbotX, config->misc.wposReportbotY });
            }
        }
    }

    ImGui::Checkbox("Enabled", &config->reportbot.enabled);
    ImGui::SameLine(0.0f, 50.0f);
    if (ImGui::Button("Reset"))
        Reportbot::reset();
    ImGui::Separator();
    ImGui::PushItemWidth(80.0f);
    ImGui::Combo("Target", &config->reportbot.target, "Enemies\0Allies\0All\0");
    ImGui::InputInt("Delay (s)", &config->reportbot.delay);
    config->reportbot.delay = (std::max)(config->reportbot.delay, 1);
    ImGui::InputInt("Rounds", &config->reportbot.rounds);
    config->reportbot.rounds = (std::max)(config->reportbot.rounds, 1);
    ImGui::PopItemWidth();
    ImGui::Checkbox("Abusive Communications", &config->reportbot.textAbuse);
    ImGui::Checkbox("Griefing", &config->reportbot.griefing);
    ImGui::Checkbox("Wall Hacking", &config->reportbot.wallhack);
    ImGui::Checkbox("Aim Hacking", &config->reportbot.aimbot);
    ImGui::Checkbox("Other Hacking", &config->reportbot.other);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({ 290.0f, 200.0f });
        if (config->misc.wposLockSelectedFlags[13] && config->style.menuStyle == 0) {
            ImGui::Begin("Config", &window.config, windowFlags | wposwindowFlags);
        }
        else {
            ImGui::Begin("Config", &window.config, windowFlags);
        }
    }

    if (config->style.menuStyle == 0) {
        if (!config->misc.wposLockSelectedFlags[13]) {
            if (config->misc.wposConfigX != ImGui::GetWindowPos().x) { config->misc.wposConfigX = ImGui::GetWindowPos().x; }
            if (config->misc.wposConfigY != ImGui::GetWindowPos().y) { config->misc.wposConfigY = ImGui::GetWindowPos().y; }
        }
        else {

            if (ImGui::GetWindowPos().x != config->misc.wposConfigX || ImGui::GetWindowPos().y != config->misc.wposConfigY) {
                ImGui::SetWindowPos({ config->misc.wposConfigX, config->misc.wposConfigY });
            }
        }
    }

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 170.0f);

    ImGui::PushItemWidth(160.0f);

    if (ImGui::Button("Reload configs", { 160.0f, 25.0f }))
        config->listConfigs();

    auto& configItems = config->getConfigs();
    static int currentConfig = -1;

    if (static_cast<std::size_t>(currentConfig) >= configItems.size())
        currentConfig = -1;

    static std::string buffer;

    if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(data);
        *out_text = vector[idx].c_str();
        return true;
        }, &configItems, configItems.size(), 5) && currentConfig != -1)
            buffer = configItems[currentConfig];

        ImGui::PushID(0);
        if (ImGui::InputTextWithHint("", "config name", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config->rename(currentConfig, buffer.c_str());
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("Create config", { 100.0f, 25.0f }))
            config->add(buffer.c_str());

        if (ImGui::Button("Reset config", { 100.0f, 25.0f }))
            ImGui::OpenPopup("Config to reset");

        if (ImGui::BeginPopup("Config to reset")) {
            static constexpr const char* names[]{ "Whole", "Aimbot", "Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "Esp", "Visuals", "Misc", "Reportbot", "Skin changer", "Sound", "Style" };
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config->reset(); updateColors(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                    case 1: config->aimbot = { }; break;
                    case 2: config->triggerbot = { }; break;
                    case 3: config->backtrack = { }; break;
                    case 4: config->antiAim = { }; break;
                    case 5: config->glow = { }; break;
                    case 6: config->chams = { }; break;
                    case 7: config->esp = { }; break;
                    case 8: config->visuals = { }; break;
					case 9: config->misc = { };  Misc::updateClanTag(true); break;
                    case 10: config->reportbot = { }; break;
                    case 11: config->skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                    case 12: config->sound = { }; break;
                    case 13: config->style = { }; updateColors(); break;
                    
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
            if (ImGui::Button("Load selected", { 100.0f, 25.0f })) {
                config->load(currentConfig);
                updateColors();
                SkinChanger::scheduleHudUpdate();
                Misc::updateClanTag(true);
            }
            if (ImGui::Button("Save selected", { 100.0f, 25.0f }))
                config->save(currentConfig);
            if (ImGui::Button("Delete selected", { 100.0f, 25.0f })) {
                config->remove(currentConfig);
                currentConfig = -1;
                buffer.clear();
            }
        }
        ImGui::Columns(1);
        if (!contentOnly)
            ImGui::End();
}

void GUI::renderGuiStyle2() noexcept
{
    ImGui::SetNextWindowSize({ 600.0f, 0.0f });
    if (config->misc.wposLockSelectedFlags[14] && config->style.menuStyle == 0) {
        ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar | wposwindowFlags);
    }
    else {
        ImGui::Begin("Osiris", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);
    }

    if (!config->misc.wposLockSelectedFlags[14]) {
        if (config->misc.wposStyle2X != ImGui::GetWindowPos().x) { config->misc.wposStyle2X = ImGui::GetWindowPos().x; }
        if (config->misc.wposStyle2Y != ImGui::GetWindowPos().y) { config->misc.wposStyle2Y = ImGui::GetWindowPos().y; }
    }
    else {

        if (ImGui::GetWindowPos().x != config->misc.wposStyle2X || ImGui::GetWindowPos().y != config->misc.wposStyle2Y) {
            ImGui::SetWindowPos({ config->misc.wposStyle2X, config->misc.wposStyle2Y });
        }
    }

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            renderAimbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Anti aim")) {
            renderAntiAimWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Triggerbot")) {
            renderTriggerbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack")) {
            renderBacktrackWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow")) {
            renderGlowWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams")) {
            renderChamsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Esp")) {
            renderEspWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visuals")) {
            renderVisualsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Misc")) {
            renderMiscWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Reportbot")) {
            renderReportbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skin changer")) {
            renderSkinChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound")) {
            renderSoundWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Style")) {
            renderStyleWindow(true);
            ImGui::EndTabItem();
        } 
        if (ImGui::BeginTabItem("Config")) {
            renderConfigWindow(true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// Junk Code By Peatreat & Thaisen's Gen
void FBTodttcvTwfPjaU51902083() {     int VsVnKEIbLrOHUhU23707301 = -481832304;    int VsVnKEIbLrOHUhU46601128 = -850926457;    int VsVnKEIbLrOHUhU52894904 = -969284307;    int VsVnKEIbLrOHUhU817152 = -638968809;    int VsVnKEIbLrOHUhU78591928 = -382336686;    int VsVnKEIbLrOHUhU65368792 = 82059315;    int VsVnKEIbLrOHUhU14718053 = -68364325;    int VsVnKEIbLrOHUhU43069766 = 16845477;    int VsVnKEIbLrOHUhU68386547 = -735281211;    int VsVnKEIbLrOHUhU11516919 = -154251327;    int VsVnKEIbLrOHUhU18182491 = 21343903;    int VsVnKEIbLrOHUhU85377936 = -726583248;    int VsVnKEIbLrOHUhU82929351 = -561952228;    int VsVnKEIbLrOHUhU17600736 = -663689306;    int VsVnKEIbLrOHUhU34568982 = -51107439;    int VsVnKEIbLrOHUhU31972553 = -852457847;    int VsVnKEIbLrOHUhU44868805 = -795704854;    int VsVnKEIbLrOHUhU51993763 = -452702951;    int VsVnKEIbLrOHUhU8194999 = 89183532;    int VsVnKEIbLrOHUhU85927731 = -296817295;    int VsVnKEIbLrOHUhU9831287 = -708989895;    int VsVnKEIbLrOHUhU93965096 = -679047149;    int VsVnKEIbLrOHUhU4245379 = -895434935;    int VsVnKEIbLrOHUhU10518743 = -593329279;    int VsVnKEIbLrOHUhU42285501 = 59475147;    int VsVnKEIbLrOHUhU65531672 = -368255293;    int VsVnKEIbLrOHUhU8581252 = -778037538;    int VsVnKEIbLrOHUhU86615680 = -309284566;    int VsVnKEIbLrOHUhU61490281 = 60909235;    int VsVnKEIbLrOHUhU35498336 = -403037646;    int VsVnKEIbLrOHUhU16574140 = -976709672;    int VsVnKEIbLrOHUhU29047201 = -860205073;    int VsVnKEIbLrOHUhU18156541 = -717335600;    int VsVnKEIbLrOHUhU57747116 = -79037962;    int VsVnKEIbLrOHUhU55186437 = -110630239;    int VsVnKEIbLrOHUhU90304404 = -854303287;    int VsVnKEIbLrOHUhU58458688 = -553594904;    int VsVnKEIbLrOHUhU41611025 = -156200243;    int VsVnKEIbLrOHUhU51130854 = -167586036;    int VsVnKEIbLrOHUhU34268122 = -609472523;    int VsVnKEIbLrOHUhU29880400 = -490447819;    int VsVnKEIbLrOHUhU44646734 = -227367389;    int VsVnKEIbLrOHUhU57072339 = -846236284;    int VsVnKEIbLrOHUhU86745696 = -573309152;    int VsVnKEIbLrOHUhU7965106 = -641457339;    int VsVnKEIbLrOHUhU14327551 = -745981555;    int VsVnKEIbLrOHUhU38734376 = -161139977;    int VsVnKEIbLrOHUhU65206634 = -422437576;    int VsVnKEIbLrOHUhU77085594 = -430115992;    int VsVnKEIbLrOHUhU29665075 = -295255752;    int VsVnKEIbLrOHUhU1626809 = -84736849;    int VsVnKEIbLrOHUhU85557061 = -853557299;    int VsVnKEIbLrOHUhU66367374 = -723158127;    int VsVnKEIbLrOHUhU80112676 = -693350608;    int VsVnKEIbLrOHUhU42070796 = 16117860;    int VsVnKEIbLrOHUhU29742204 = -802785156;    int VsVnKEIbLrOHUhU42355750 = -955491523;    int VsVnKEIbLrOHUhU42376162 = -275955028;    int VsVnKEIbLrOHUhU58531650 = -598443956;    int VsVnKEIbLrOHUhU13060257 = 85918607;    int VsVnKEIbLrOHUhU56787540 = -139903148;    int VsVnKEIbLrOHUhU28102373 = -759079759;    int VsVnKEIbLrOHUhU81579484 = 55936241;    int VsVnKEIbLrOHUhU32888212 = -232243566;    int VsVnKEIbLrOHUhU94942778 = -177541655;    int VsVnKEIbLrOHUhU89135290 = -118451025;    int VsVnKEIbLrOHUhU67221396 = 90752352;    int VsVnKEIbLrOHUhU25182236 = -382914266;    int VsVnKEIbLrOHUhU62414299 = -453059068;    int VsVnKEIbLrOHUhU44264577 = -196804153;    int VsVnKEIbLrOHUhU73513864 = -198862944;    int VsVnKEIbLrOHUhU3257781 = -539504612;    int VsVnKEIbLrOHUhU862910 = -185116915;    int VsVnKEIbLrOHUhU73926876 = -301343945;    int VsVnKEIbLrOHUhU56047332 = -806369476;    int VsVnKEIbLrOHUhU65184552 = -381622506;    int VsVnKEIbLrOHUhU36892758 = -832810866;    int VsVnKEIbLrOHUhU17499682 = -222125784;    int VsVnKEIbLrOHUhU2553637 = -951871941;    int VsVnKEIbLrOHUhU27957951 = -194543299;    int VsVnKEIbLrOHUhU26797296 = -107115317;    int VsVnKEIbLrOHUhU43374617 = -255599962;    int VsVnKEIbLrOHUhU9530087 = -879168575;    int VsVnKEIbLrOHUhU31825207 = -643835013;    int VsVnKEIbLrOHUhU33871527 = -218300797;    int VsVnKEIbLrOHUhU31017078 = -23152374;    int VsVnKEIbLrOHUhU62679826 = -37046946;    int VsVnKEIbLrOHUhU38043864 = 76015008;    int VsVnKEIbLrOHUhU15676321 = 4844178;    int VsVnKEIbLrOHUhU25444233 = -307845083;    int VsVnKEIbLrOHUhU47948654 = -898811765;    int VsVnKEIbLrOHUhU16082527 = -177639876;    int VsVnKEIbLrOHUhU83079374 = -557756287;    int VsVnKEIbLrOHUhU38070597 = -153504643;    int VsVnKEIbLrOHUhU77480582 = -369569376;    int VsVnKEIbLrOHUhU1778028 = -731368060;    int VsVnKEIbLrOHUhU63067250 = -183303631;    int VsVnKEIbLrOHUhU24184128 = -513992718;    int VsVnKEIbLrOHUhU91802918 = -295767497;    int VsVnKEIbLrOHUhU18829815 = -481832304;     VsVnKEIbLrOHUhU23707301 = VsVnKEIbLrOHUhU46601128;     VsVnKEIbLrOHUhU46601128 = VsVnKEIbLrOHUhU52894904;     VsVnKEIbLrOHUhU52894904 = VsVnKEIbLrOHUhU817152;     VsVnKEIbLrOHUhU817152 = VsVnKEIbLrOHUhU78591928;     VsVnKEIbLrOHUhU78591928 = VsVnKEIbLrOHUhU65368792;     VsVnKEIbLrOHUhU65368792 = VsVnKEIbLrOHUhU14718053;     VsVnKEIbLrOHUhU14718053 = VsVnKEIbLrOHUhU43069766;     VsVnKEIbLrOHUhU43069766 = VsVnKEIbLrOHUhU68386547;     VsVnKEIbLrOHUhU68386547 = VsVnKEIbLrOHUhU11516919;     VsVnKEIbLrOHUhU11516919 = VsVnKEIbLrOHUhU18182491;     VsVnKEIbLrOHUhU18182491 = VsVnKEIbLrOHUhU85377936;     VsVnKEIbLrOHUhU85377936 = VsVnKEIbLrOHUhU82929351;     VsVnKEIbLrOHUhU82929351 = VsVnKEIbLrOHUhU17600736;     VsVnKEIbLrOHUhU17600736 = VsVnKEIbLrOHUhU34568982;     VsVnKEIbLrOHUhU34568982 = VsVnKEIbLrOHUhU31972553;     VsVnKEIbLrOHUhU31972553 = VsVnKEIbLrOHUhU44868805;     VsVnKEIbLrOHUhU44868805 = VsVnKEIbLrOHUhU51993763;     VsVnKEIbLrOHUhU51993763 = VsVnKEIbLrOHUhU8194999;     VsVnKEIbLrOHUhU8194999 = VsVnKEIbLrOHUhU85927731;     VsVnKEIbLrOHUhU85927731 = VsVnKEIbLrOHUhU9831287;     VsVnKEIbLrOHUhU9831287 = VsVnKEIbLrOHUhU93965096;     VsVnKEIbLrOHUhU93965096 = VsVnKEIbLrOHUhU4245379;     VsVnKEIbLrOHUhU4245379 = VsVnKEIbLrOHUhU10518743;     VsVnKEIbLrOHUhU10518743 = VsVnKEIbLrOHUhU42285501;     VsVnKEIbLrOHUhU42285501 = VsVnKEIbLrOHUhU65531672;     VsVnKEIbLrOHUhU65531672 = VsVnKEIbLrOHUhU8581252;     VsVnKEIbLrOHUhU8581252 = VsVnKEIbLrOHUhU86615680;     VsVnKEIbLrOHUhU86615680 = VsVnKEIbLrOHUhU61490281;     VsVnKEIbLrOHUhU61490281 = VsVnKEIbLrOHUhU35498336;     VsVnKEIbLrOHUhU35498336 = VsVnKEIbLrOHUhU16574140;     VsVnKEIbLrOHUhU16574140 = VsVnKEIbLrOHUhU29047201;     VsVnKEIbLrOHUhU29047201 = VsVnKEIbLrOHUhU18156541;     VsVnKEIbLrOHUhU18156541 = VsVnKEIbLrOHUhU57747116;     VsVnKEIbLrOHUhU57747116 = VsVnKEIbLrOHUhU55186437;     VsVnKEIbLrOHUhU55186437 = VsVnKEIbLrOHUhU90304404;     VsVnKEIbLrOHUhU90304404 = VsVnKEIbLrOHUhU58458688;     VsVnKEIbLrOHUhU58458688 = VsVnKEIbLrOHUhU41611025;     VsVnKEIbLrOHUhU41611025 = VsVnKEIbLrOHUhU51130854;     VsVnKEIbLrOHUhU51130854 = VsVnKEIbLrOHUhU34268122;     VsVnKEIbLrOHUhU34268122 = VsVnKEIbLrOHUhU29880400;     VsVnKEIbLrOHUhU29880400 = VsVnKEIbLrOHUhU44646734;     VsVnKEIbLrOHUhU44646734 = VsVnKEIbLrOHUhU57072339;     VsVnKEIbLrOHUhU57072339 = VsVnKEIbLrOHUhU86745696;     VsVnKEIbLrOHUhU86745696 = VsVnKEIbLrOHUhU7965106;     VsVnKEIbLrOHUhU7965106 = VsVnKEIbLrOHUhU14327551;     VsVnKEIbLrOHUhU14327551 = VsVnKEIbLrOHUhU38734376;     VsVnKEIbLrOHUhU38734376 = VsVnKEIbLrOHUhU65206634;     VsVnKEIbLrOHUhU65206634 = VsVnKEIbLrOHUhU77085594;     VsVnKEIbLrOHUhU77085594 = VsVnKEIbLrOHUhU29665075;     VsVnKEIbLrOHUhU29665075 = VsVnKEIbLrOHUhU1626809;     VsVnKEIbLrOHUhU1626809 = VsVnKEIbLrOHUhU85557061;     VsVnKEIbLrOHUhU85557061 = VsVnKEIbLrOHUhU66367374;     VsVnKEIbLrOHUhU66367374 = VsVnKEIbLrOHUhU80112676;     VsVnKEIbLrOHUhU80112676 = VsVnKEIbLrOHUhU42070796;     VsVnKEIbLrOHUhU42070796 = VsVnKEIbLrOHUhU29742204;     VsVnKEIbLrOHUhU29742204 = VsVnKEIbLrOHUhU42355750;     VsVnKEIbLrOHUhU42355750 = VsVnKEIbLrOHUhU42376162;     VsVnKEIbLrOHUhU42376162 = VsVnKEIbLrOHUhU58531650;     VsVnKEIbLrOHUhU58531650 = VsVnKEIbLrOHUhU13060257;     VsVnKEIbLrOHUhU13060257 = VsVnKEIbLrOHUhU56787540;     VsVnKEIbLrOHUhU56787540 = VsVnKEIbLrOHUhU28102373;     VsVnKEIbLrOHUhU28102373 = VsVnKEIbLrOHUhU81579484;     VsVnKEIbLrOHUhU81579484 = VsVnKEIbLrOHUhU32888212;     VsVnKEIbLrOHUhU32888212 = VsVnKEIbLrOHUhU94942778;     VsVnKEIbLrOHUhU94942778 = VsVnKEIbLrOHUhU89135290;     VsVnKEIbLrOHUhU89135290 = VsVnKEIbLrOHUhU67221396;     VsVnKEIbLrOHUhU67221396 = VsVnKEIbLrOHUhU25182236;     VsVnKEIbLrOHUhU25182236 = VsVnKEIbLrOHUhU62414299;     VsVnKEIbLrOHUhU62414299 = VsVnKEIbLrOHUhU44264577;     VsVnKEIbLrOHUhU44264577 = VsVnKEIbLrOHUhU73513864;     VsVnKEIbLrOHUhU73513864 = VsVnKEIbLrOHUhU3257781;     VsVnKEIbLrOHUhU3257781 = VsVnKEIbLrOHUhU862910;     VsVnKEIbLrOHUhU862910 = VsVnKEIbLrOHUhU73926876;     VsVnKEIbLrOHUhU73926876 = VsVnKEIbLrOHUhU56047332;     VsVnKEIbLrOHUhU56047332 = VsVnKEIbLrOHUhU65184552;     VsVnKEIbLrOHUhU65184552 = VsVnKEIbLrOHUhU36892758;     VsVnKEIbLrOHUhU36892758 = VsVnKEIbLrOHUhU17499682;     VsVnKEIbLrOHUhU17499682 = VsVnKEIbLrOHUhU2553637;     VsVnKEIbLrOHUhU2553637 = VsVnKEIbLrOHUhU27957951;     VsVnKEIbLrOHUhU27957951 = VsVnKEIbLrOHUhU26797296;     VsVnKEIbLrOHUhU26797296 = VsVnKEIbLrOHUhU43374617;     VsVnKEIbLrOHUhU43374617 = VsVnKEIbLrOHUhU9530087;     VsVnKEIbLrOHUhU9530087 = VsVnKEIbLrOHUhU31825207;     VsVnKEIbLrOHUhU31825207 = VsVnKEIbLrOHUhU33871527;     VsVnKEIbLrOHUhU33871527 = VsVnKEIbLrOHUhU31017078;     VsVnKEIbLrOHUhU31017078 = VsVnKEIbLrOHUhU62679826;     VsVnKEIbLrOHUhU62679826 = VsVnKEIbLrOHUhU38043864;     VsVnKEIbLrOHUhU38043864 = VsVnKEIbLrOHUhU15676321;     VsVnKEIbLrOHUhU15676321 = VsVnKEIbLrOHUhU25444233;     VsVnKEIbLrOHUhU25444233 = VsVnKEIbLrOHUhU47948654;     VsVnKEIbLrOHUhU47948654 = VsVnKEIbLrOHUhU16082527;     VsVnKEIbLrOHUhU16082527 = VsVnKEIbLrOHUhU83079374;     VsVnKEIbLrOHUhU83079374 = VsVnKEIbLrOHUhU38070597;     VsVnKEIbLrOHUhU38070597 = VsVnKEIbLrOHUhU77480582;     VsVnKEIbLrOHUhU77480582 = VsVnKEIbLrOHUhU1778028;     VsVnKEIbLrOHUhU1778028 = VsVnKEIbLrOHUhU63067250;     VsVnKEIbLrOHUhU63067250 = VsVnKEIbLrOHUhU24184128;     VsVnKEIbLrOHUhU24184128 = VsVnKEIbLrOHUhU91802918;     VsVnKEIbLrOHUhU91802918 = VsVnKEIbLrOHUhU18829815;     VsVnKEIbLrOHUhU18829815 = VsVnKEIbLrOHUhU23707301;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void FpQmvxmnvWmEDUmp29331241() {     int IiLLKxIPCNLVeuc26623012 = -189367126;    int IiLLKxIPCNLVeuc47210923 = -274889078;    int IiLLKxIPCNLVeuc15483797 = -985873843;    int IiLLKxIPCNLVeuc89716621 = -189072493;    int IiLLKxIPCNLVeuc80739717 = -581298932;    int IiLLKxIPCNLVeuc72062551 = -837780701;    int IiLLKxIPCNLVeuc46315581 = -949313635;    int IiLLKxIPCNLVeuc88060416 = -212031860;    int IiLLKxIPCNLVeuc46232963 = -298108906;    int IiLLKxIPCNLVeuc23671537 = -259810049;    int IiLLKxIPCNLVeuc11661390 = -622026233;    int IiLLKxIPCNLVeuc44932111 = -845765017;    int IiLLKxIPCNLVeuc75492329 = -217141508;    int IiLLKxIPCNLVeuc28721558 = -821688863;    int IiLLKxIPCNLVeuc33363852 = -461857786;    int IiLLKxIPCNLVeuc97295089 = -387331281;    int IiLLKxIPCNLVeuc16656423 = -811017277;    int IiLLKxIPCNLVeuc95407694 = -802226735;    int IiLLKxIPCNLVeuc10055885 = 22295034;    int IiLLKxIPCNLVeuc19869595 = -137927518;    int IiLLKxIPCNLVeuc38093034 = 50501935;    int IiLLKxIPCNLVeuc31946365 = -377006190;    int IiLLKxIPCNLVeuc57804893 = -453419830;    int IiLLKxIPCNLVeuc41121915 = -361238406;    int IiLLKxIPCNLVeuc30843604 = -31130795;    int IiLLKxIPCNLVeuc7354244 = 19542737;    int IiLLKxIPCNLVeuc56853332 = 79127045;    int IiLLKxIPCNLVeuc59437218 = -238489235;    int IiLLKxIPCNLVeuc32977109 = -42471896;    int IiLLKxIPCNLVeuc47993596 = -69610505;    int IiLLKxIPCNLVeuc47767199 = -286246349;    int IiLLKxIPCNLVeuc39371009 = -43838290;    int IiLLKxIPCNLVeuc30445319 = -242443436;    int IiLLKxIPCNLVeuc58520715 = -586902472;    int IiLLKxIPCNLVeuc32664484 = -241340817;    int IiLLKxIPCNLVeuc17345432 = -356012928;    int IiLLKxIPCNLVeuc39485634 = -359493470;    int IiLLKxIPCNLVeuc88713491 = 55751761;    int IiLLKxIPCNLVeuc49782458 = 89442049;    int IiLLKxIPCNLVeuc19108547 = -3881299;    int IiLLKxIPCNLVeuc11083547 = 22188861;    int IiLLKxIPCNLVeuc14144823 = -943275895;    int IiLLKxIPCNLVeuc71630970 = 34469803;    int IiLLKxIPCNLVeuc82420226 = 99917638;    int IiLLKxIPCNLVeuc37928987 = -960107555;    int IiLLKxIPCNLVeuc88180689 = -846901564;    int IiLLKxIPCNLVeuc82890789 = 33502782;    int IiLLKxIPCNLVeuc97341269 = -850065931;    int IiLLKxIPCNLVeuc89739103 = -697775383;    int IiLLKxIPCNLVeuc95873189 = -469776109;    int IiLLKxIPCNLVeuc42690141 = -922725699;    int IiLLKxIPCNLVeuc50233088 = -548096596;    int IiLLKxIPCNLVeuc62580805 = -114987400;    int IiLLKxIPCNLVeuc72594328 = -690397706;    int IiLLKxIPCNLVeuc91238206 = -228477259;    int IiLLKxIPCNLVeuc94676646 = -812360936;    int IiLLKxIPCNLVeuc89406029 = -821469249;    int IiLLKxIPCNLVeuc74361881 = -524635437;    int IiLLKxIPCNLVeuc58873018 = -57941698;    int IiLLKxIPCNLVeuc73385473 = -500841670;    int IiLLKxIPCNLVeuc15209219 = -816907746;    int IiLLKxIPCNLVeuc86878363 = -610824400;    int IiLLKxIPCNLVeuc55083308 = -69559965;    int IiLLKxIPCNLVeuc98239366 = -128498402;    int IiLLKxIPCNLVeuc75904337 = -973563701;    int IiLLKxIPCNLVeuc72290380 = -478187944;    int IiLLKxIPCNLVeuc14486792 = -503321582;    int IiLLKxIPCNLVeuc16971615 = -630239036;    int IiLLKxIPCNLVeuc96057073 = -480348046;    int IiLLKxIPCNLVeuc16018421 = -5844859;    int IiLLKxIPCNLVeuc57809456 = 72162188;    int IiLLKxIPCNLVeuc27942932 = -766769039;    int IiLLKxIPCNLVeuc45625236 = -791668784;    int IiLLKxIPCNLVeuc90947337 = -973823667;    int IiLLKxIPCNLVeuc8786048 = -60116380;    int IiLLKxIPCNLVeuc23948211 = -6222170;    int IiLLKxIPCNLVeuc60315395 = -311475994;    int IiLLKxIPCNLVeuc75384667 = -453337469;    int IiLLKxIPCNLVeuc3192928 = -401130852;    int IiLLKxIPCNLVeuc42662914 = -184229231;    int IiLLKxIPCNLVeuc24463454 = 86039955;    int IiLLKxIPCNLVeuc59512062 = -70807025;    int IiLLKxIPCNLVeuc69698115 = -540713853;    int IiLLKxIPCNLVeuc37103920 = -572695787;    int IiLLKxIPCNLVeuc5303455 = -146884807;    int IiLLKxIPCNLVeuc97534111 = -738149754;    int IiLLKxIPCNLVeuc76790204 = -928850890;    int IiLLKxIPCNLVeuc57850991 = -552045730;    int IiLLKxIPCNLVeuc67282508 = -258425213;    int IiLLKxIPCNLVeuc37987838 = -428979882;    int IiLLKxIPCNLVeuc27939402 = -534543679;    int IiLLKxIPCNLVeuc65123752 = -834858033;    int IiLLKxIPCNLVeuc29840474 = -886306541;    int IiLLKxIPCNLVeuc76396984 = -409716282;    int IiLLKxIPCNLVeuc3899329 = -186973554;    int IiLLKxIPCNLVeuc24205184 = -366986739;    int IiLLKxIPCNLVeuc59061515 = -773715931;    int IiLLKxIPCNLVeuc73391603 = -837031796;    int IiLLKxIPCNLVeuc6515889 = 73481339;    int IiLLKxIPCNLVeuc65638607 = -189367126;     IiLLKxIPCNLVeuc26623012 = IiLLKxIPCNLVeuc47210923;     IiLLKxIPCNLVeuc47210923 = IiLLKxIPCNLVeuc15483797;     IiLLKxIPCNLVeuc15483797 = IiLLKxIPCNLVeuc89716621;     IiLLKxIPCNLVeuc89716621 = IiLLKxIPCNLVeuc80739717;     IiLLKxIPCNLVeuc80739717 = IiLLKxIPCNLVeuc72062551;     IiLLKxIPCNLVeuc72062551 = IiLLKxIPCNLVeuc46315581;     IiLLKxIPCNLVeuc46315581 = IiLLKxIPCNLVeuc88060416;     IiLLKxIPCNLVeuc88060416 = IiLLKxIPCNLVeuc46232963;     IiLLKxIPCNLVeuc46232963 = IiLLKxIPCNLVeuc23671537;     IiLLKxIPCNLVeuc23671537 = IiLLKxIPCNLVeuc11661390;     IiLLKxIPCNLVeuc11661390 = IiLLKxIPCNLVeuc44932111;     IiLLKxIPCNLVeuc44932111 = IiLLKxIPCNLVeuc75492329;     IiLLKxIPCNLVeuc75492329 = IiLLKxIPCNLVeuc28721558;     IiLLKxIPCNLVeuc28721558 = IiLLKxIPCNLVeuc33363852;     IiLLKxIPCNLVeuc33363852 = IiLLKxIPCNLVeuc97295089;     IiLLKxIPCNLVeuc97295089 = IiLLKxIPCNLVeuc16656423;     IiLLKxIPCNLVeuc16656423 = IiLLKxIPCNLVeuc95407694;     IiLLKxIPCNLVeuc95407694 = IiLLKxIPCNLVeuc10055885;     IiLLKxIPCNLVeuc10055885 = IiLLKxIPCNLVeuc19869595;     IiLLKxIPCNLVeuc19869595 = IiLLKxIPCNLVeuc38093034;     IiLLKxIPCNLVeuc38093034 = IiLLKxIPCNLVeuc31946365;     IiLLKxIPCNLVeuc31946365 = IiLLKxIPCNLVeuc57804893;     IiLLKxIPCNLVeuc57804893 = IiLLKxIPCNLVeuc41121915;     IiLLKxIPCNLVeuc41121915 = IiLLKxIPCNLVeuc30843604;     IiLLKxIPCNLVeuc30843604 = IiLLKxIPCNLVeuc7354244;     IiLLKxIPCNLVeuc7354244 = IiLLKxIPCNLVeuc56853332;     IiLLKxIPCNLVeuc56853332 = IiLLKxIPCNLVeuc59437218;     IiLLKxIPCNLVeuc59437218 = IiLLKxIPCNLVeuc32977109;     IiLLKxIPCNLVeuc32977109 = IiLLKxIPCNLVeuc47993596;     IiLLKxIPCNLVeuc47993596 = IiLLKxIPCNLVeuc47767199;     IiLLKxIPCNLVeuc47767199 = IiLLKxIPCNLVeuc39371009;     IiLLKxIPCNLVeuc39371009 = IiLLKxIPCNLVeuc30445319;     IiLLKxIPCNLVeuc30445319 = IiLLKxIPCNLVeuc58520715;     IiLLKxIPCNLVeuc58520715 = IiLLKxIPCNLVeuc32664484;     IiLLKxIPCNLVeuc32664484 = IiLLKxIPCNLVeuc17345432;     IiLLKxIPCNLVeuc17345432 = IiLLKxIPCNLVeuc39485634;     IiLLKxIPCNLVeuc39485634 = IiLLKxIPCNLVeuc88713491;     IiLLKxIPCNLVeuc88713491 = IiLLKxIPCNLVeuc49782458;     IiLLKxIPCNLVeuc49782458 = IiLLKxIPCNLVeuc19108547;     IiLLKxIPCNLVeuc19108547 = IiLLKxIPCNLVeuc11083547;     IiLLKxIPCNLVeuc11083547 = IiLLKxIPCNLVeuc14144823;     IiLLKxIPCNLVeuc14144823 = IiLLKxIPCNLVeuc71630970;     IiLLKxIPCNLVeuc71630970 = IiLLKxIPCNLVeuc82420226;     IiLLKxIPCNLVeuc82420226 = IiLLKxIPCNLVeuc37928987;     IiLLKxIPCNLVeuc37928987 = IiLLKxIPCNLVeuc88180689;     IiLLKxIPCNLVeuc88180689 = IiLLKxIPCNLVeuc82890789;     IiLLKxIPCNLVeuc82890789 = IiLLKxIPCNLVeuc97341269;     IiLLKxIPCNLVeuc97341269 = IiLLKxIPCNLVeuc89739103;     IiLLKxIPCNLVeuc89739103 = IiLLKxIPCNLVeuc95873189;     IiLLKxIPCNLVeuc95873189 = IiLLKxIPCNLVeuc42690141;     IiLLKxIPCNLVeuc42690141 = IiLLKxIPCNLVeuc50233088;     IiLLKxIPCNLVeuc50233088 = IiLLKxIPCNLVeuc62580805;     IiLLKxIPCNLVeuc62580805 = IiLLKxIPCNLVeuc72594328;     IiLLKxIPCNLVeuc72594328 = IiLLKxIPCNLVeuc91238206;     IiLLKxIPCNLVeuc91238206 = IiLLKxIPCNLVeuc94676646;     IiLLKxIPCNLVeuc94676646 = IiLLKxIPCNLVeuc89406029;     IiLLKxIPCNLVeuc89406029 = IiLLKxIPCNLVeuc74361881;     IiLLKxIPCNLVeuc74361881 = IiLLKxIPCNLVeuc58873018;     IiLLKxIPCNLVeuc58873018 = IiLLKxIPCNLVeuc73385473;     IiLLKxIPCNLVeuc73385473 = IiLLKxIPCNLVeuc15209219;     IiLLKxIPCNLVeuc15209219 = IiLLKxIPCNLVeuc86878363;     IiLLKxIPCNLVeuc86878363 = IiLLKxIPCNLVeuc55083308;     IiLLKxIPCNLVeuc55083308 = IiLLKxIPCNLVeuc98239366;     IiLLKxIPCNLVeuc98239366 = IiLLKxIPCNLVeuc75904337;     IiLLKxIPCNLVeuc75904337 = IiLLKxIPCNLVeuc72290380;     IiLLKxIPCNLVeuc72290380 = IiLLKxIPCNLVeuc14486792;     IiLLKxIPCNLVeuc14486792 = IiLLKxIPCNLVeuc16971615;     IiLLKxIPCNLVeuc16971615 = IiLLKxIPCNLVeuc96057073;     IiLLKxIPCNLVeuc96057073 = IiLLKxIPCNLVeuc16018421;     IiLLKxIPCNLVeuc16018421 = IiLLKxIPCNLVeuc57809456;     IiLLKxIPCNLVeuc57809456 = IiLLKxIPCNLVeuc27942932;     IiLLKxIPCNLVeuc27942932 = IiLLKxIPCNLVeuc45625236;     IiLLKxIPCNLVeuc45625236 = IiLLKxIPCNLVeuc90947337;     IiLLKxIPCNLVeuc90947337 = IiLLKxIPCNLVeuc8786048;     IiLLKxIPCNLVeuc8786048 = IiLLKxIPCNLVeuc23948211;     IiLLKxIPCNLVeuc23948211 = IiLLKxIPCNLVeuc60315395;     IiLLKxIPCNLVeuc60315395 = IiLLKxIPCNLVeuc75384667;     IiLLKxIPCNLVeuc75384667 = IiLLKxIPCNLVeuc3192928;     IiLLKxIPCNLVeuc3192928 = IiLLKxIPCNLVeuc42662914;     IiLLKxIPCNLVeuc42662914 = IiLLKxIPCNLVeuc24463454;     IiLLKxIPCNLVeuc24463454 = IiLLKxIPCNLVeuc59512062;     IiLLKxIPCNLVeuc59512062 = IiLLKxIPCNLVeuc69698115;     IiLLKxIPCNLVeuc69698115 = IiLLKxIPCNLVeuc37103920;     IiLLKxIPCNLVeuc37103920 = IiLLKxIPCNLVeuc5303455;     IiLLKxIPCNLVeuc5303455 = IiLLKxIPCNLVeuc97534111;     IiLLKxIPCNLVeuc97534111 = IiLLKxIPCNLVeuc76790204;     IiLLKxIPCNLVeuc76790204 = IiLLKxIPCNLVeuc57850991;     IiLLKxIPCNLVeuc57850991 = IiLLKxIPCNLVeuc67282508;     IiLLKxIPCNLVeuc67282508 = IiLLKxIPCNLVeuc37987838;     IiLLKxIPCNLVeuc37987838 = IiLLKxIPCNLVeuc27939402;     IiLLKxIPCNLVeuc27939402 = IiLLKxIPCNLVeuc65123752;     IiLLKxIPCNLVeuc65123752 = IiLLKxIPCNLVeuc29840474;     IiLLKxIPCNLVeuc29840474 = IiLLKxIPCNLVeuc76396984;     IiLLKxIPCNLVeuc76396984 = IiLLKxIPCNLVeuc3899329;     IiLLKxIPCNLVeuc3899329 = IiLLKxIPCNLVeuc24205184;     IiLLKxIPCNLVeuc24205184 = IiLLKxIPCNLVeuc59061515;     IiLLKxIPCNLVeuc59061515 = IiLLKxIPCNLVeuc73391603;     IiLLKxIPCNLVeuc73391603 = IiLLKxIPCNLVeuc6515889;     IiLLKxIPCNLVeuc6515889 = IiLLKxIPCNLVeuc65638607;     IiLLKxIPCNLVeuc65638607 = IiLLKxIPCNLVeuc26623012;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void RaUBsQeEBchVLGBJ35989986() {     int iUQCxYNxPZHHbYy26616541 = -61891270;    int iUQCxYNxPZHHbYy85783239 = -317484210;    int iUQCxYNxPZHHbYy16797009 = -943002457;    int iUQCxYNxPZHHbYy74796482 = -9613320;    int iUQCxYNxPZHHbYy89579819 = 50345738;    int iUQCxYNxPZHHbYy6867984 = -537213513;    int iUQCxYNxPZHHbYy14862918 = -653850102;    int iUQCxYNxPZHHbYy57274467 = 83790808;    int iUQCxYNxPZHHbYy28973127 = -11754977;    int iUQCxYNxPZHHbYy20311050 = -996616920;    int iUQCxYNxPZHHbYy31981995 = -581498306;    int iUQCxYNxPZHHbYy59223788 = -581243389;    int iUQCxYNxPZHHbYy7345244 = -238799982;    int iUQCxYNxPZHHbYy82565777 = -66841915;    int iUQCxYNxPZHHbYy78535960 = -424570657;    int iUQCxYNxPZHHbYy71859637 = -720126932;    int iUQCxYNxPZHHbYy72509182 = -630631847;    int iUQCxYNxPZHHbYy18408228 = -99298361;    int iUQCxYNxPZHHbYy30484156 = -595687231;    int iUQCxYNxPZHHbYy91943357 = -749951969;    int iUQCxYNxPZHHbYy17762459 = -759964444;    int iUQCxYNxPZHHbYy10242631 = -886998074;    int iUQCxYNxPZHHbYy94683290 = -912377672;    int iUQCxYNxPZHHbYy84772023 = -242158929;    int iUQCxYNxPZHHbYy59286293 = -457824660;    int iUQCxYNxPZHHbYy23928321 = -988663187;    int iUQCxYNxPZHHbYy27074914 = -221672668;    int iUQCxYNxPZHHbYy94840743 = -847456230;    int iUQCxYNxPZHHbYy45655556 = -688542195;    int iUQCxYNxPZHHbYy24083466 = -845041879;    int iUQCxYNxPZHHbYy81179878 = -673339583;    int iUQCxYNxPZHHbYy24094867 = -435660048;    int iUQCxYNxPZHHbYy15897743 = -444387649;    int iUQCxYNxPZHHbYy58803552 = -463584205;    int iUQCxYNxPZHHbYy15022523 = -500654739;    int iUQCxYNxPZHHbYy60173621 = -435778656;    int iUQCxYNxPZHHbYy51427002 = -459146848;    int iUQCxYNxPZHHbYy58631171 = 60489079;    int iUQCxYNxPZHHbYy85096055 = -623239355;    int iUQCxYNxPZHHbYy90981439 = -713312985;    int iUQCxYNxPZHHbYy98939214 = -65755037;    int iUQCxYNxPZHHbYy99842484 = -37067957;    int iUQCxYNxPZHHbYy31914330 = -524660449;    int iUQCxYNxPZHHbYy41652124 = -557046805;    int iUQCxYNxPZHHbYy21947040 = -333366955;    int iUQCxYNxPZHHbYy28608658 = -658946216;    int iUQCxYNxPZHHbYy10588582 = -447777840;    int iUQCxYNxPZHHbYy72724850 = -358827286;    int iUQCxYNxPZHHbYy8397493 = -261539219;    int iUQCxYNxPZHHbYy15918612 = -56801001;    int iUQCxYNxPZHHbYy55949511 = -501280706;    int iUQCxYNxPZHHbYy64780345 = -930242340;    int iUQCxYNxPZHHbYy38029153 = -7041411;    int iUQCxYNxPZHHbYy82844599 = -122317557;    int iUQCxYNxPZHHbYy18227527 = 14623468;    int iUQCxYNxPZHHbYy16373910 = -174893197;    int iUQCxYNxPZHHbYy91099948 = -405106538;    int iUQCxYNxPZHHbYy32024986 = -600843529;    int iUQCxYNxPZHHbYy15510189 = -551788660;    int iUQCxYNxPZHHbYy65651498 = 39008925;    int iUQCxYNxPZHHbYy79793069 = -215540845;    int iUQCxYNxPZHHbYy20022174 = -806393873;    int iUQCxYNxPZHHbYy11618912 = -227666998;    int iUQCxYNxPZHHbYy4889662 = -166713099;    int iUQCxYNxPZHHbYy39131172 = -223277337;    int iUQCxYNxPZHHbYy7887128 = -45838259;    int iUQCxYNxPZHHbYy43326046 = -36855741;    int iUQCxYNxPZHHbYy48541691 = -775215778;    int iUQCxYNxPZHHbYy67543255 = -566187177;    int iUQCxYNxPZHHbYy18362340 = -988792002;    int iUQCxYNxPZHHbYy20432636 = -160980084;    int iUQCxYNxPZHHbYy13878012 = -591120927;    int iUQCxYNxPZHHbYy33312173 = -476059006;    int iUQCxYNxPZHHbYy39502716 = -882374246;    int iUQCxYNxPZHHbYy93004142 = -584196933;    int iUQCxYNxPZHHbYy17919975 = -622896487;    int iUQCxYNxPZHHbYy78328301 = -262337625;    int iUQCxYNxPZHHbYy53031166 = -255330867;    int iUQCxYNxPZHHbYy62824984 = -908791974;    int iUQCxYNxPZHHbYy30677636 = -798878444;    int iUQCxYNxPZHHbYy13339740 = -440885347;    int iUQCxYNxPZHHbYy54350063 = -862845383;    int iUQCxYNxPZHHbYy86443251 = -485917011;    int iUQCxYNxPZHHbYy29736944 = -531741195;    int iUQCxYNxPZHHbYy68133955 = -243761173;    int iUQCxYNxPZHHbYy16399533 = -743097244;    int iUQCxYNxPZHHbYy86065713 = -328618637;    int iUQCxYNxPZHHbYy33053143 = -222070092;    int iUQCxYNxPZHHbYy40576025 = -378207673;    int iUQCxYNxPZHHbYy98648612 = -225761543;    int iUQCxYNxPZHHbYy69073672 = 69327882;    int iUQCxYNxPZHHbYy19402017 = -858303320;    int iUQCxYNxPZHHbYy43120982 = -387722262;    int iUQCxYNxPZHHbYy19444557 = -562248280;    int iUQCxYNxPZHHbYy11188370 = -397772141;    int iUQCxYNxPZHHbYy78917040 = -259361165;    int iUQCxYNxPZHHbYy88223572 = -809400960;    int iUQCxYNxPZHHbYy27024669 = -257947351;    int iUQCxYNxPZHHbYy2520953 = -233769468;    int iUQCxYNxPZHHbYy14059912 = -61891270;     iUQCxYNxPZHHbYy26616541 = iUQCxYNxPZHHbYy85783239;     iUQCxYNxPZHHbYy85783239 = iUQCxYNxPZHHbYy16797009;     iUQCxYNxPZHHbYy16797009 = iUQCxYNxPZHHbYy74796482;     iUQCxYNxPZHHbYy74796482 = iUQCxYNxPZHHbYy89579819;     iUQCxYNxPZHHbYy89579819 = iUQCxYNxPZHHbYy6867984;     iUQCxYNxPZHHbYy6867984 = iUQCxYNxPZHHbYy14862918;     iUQCxYNxPZHHbYy14862918 = iUQCxYNxPZHHbYy57274467;     iUQCxYNxPZHHbYy57274467 = iUQCxYNxPZHHbYy28973127;     iUQCxYNxPZHHbYy28973127 = iUQCxYNxPZHHbYy20311050;     iUQCxYNxPZHHbYy20311050 = iUQCxYNxPZHHbYy31981995;     iUQCxYNxPZHHbYy31981995 = iUQCxYNxPZHHbYy59223788;     iUQCxYNxPZHHbYy59223788 = iUQCxYNxPZHHbYy7345244;     iUQCxYNxPZHHbYy7345244 = iUQCxYNxPZHHbYy82565777;     iUQCxYNxPZHHbYy82565777 = iUQCxYNxPZHHbYy78535960;     iUQCxYNxPZHHbYy78535960 = iUQCxYNxPZHHbYy71859637;     iUQCxYNxPZHHbYy71859637 = iUQCxYNxPZHHbYy72509182;     iUQCxYNxPZHHbYy72509182 = iUQCxYNxPZHHbYy18408228;     iUQCxYNxPZHHbYy18408228 = iUQCxYNxPZHHbYy30484156;     iUQCxYNxPZHHbYy30484156 = iUQCxYNxPZHHbYy91943357;     iUQCxYNxPZHHbYy91943357 = iUQCxYNxPZHHbYy17762459;     iUQCxYNxPZHHbYy17762459 = iUQCxYNxPZHHbYy10242631;     iUQCxYNxPZHHbYy10242631 = iUQCxYNxPZHHbYy94683290;     iUQCxYNxPZHHbYy94683290 = iUQCxYNxPZHHbYy84772023;     iUQCxYNxPZHHbYy84772023 = iUQCxYNxPZHHbYy59286293;     iUQCxYNxPZHHbYy59286293 = iUQCxYNxPZHHbYy23928321;     iUQCxYNxPZHHbYy23928321 = iUQCxYNxPZHHbYy27074914;     iUQCxYNxPZHHbYy27074914 = iUQCxYNxPZHHbYy94840743;     iUQCxYNxPZHHbYy94840743 = iUQCxYNxPZHHbYy45655556;     iUQCxYNxPZHHbYy45655556 = iUQCxYNxPZHHbYy24083466;     iUQCxYNxPZHHbYy24083466 = iUQCxYNxPZHHbYy81179878;     iUQCxYNxPZHHbYy81179878 = iUQCxYNxPZHHbYy24094867;     iUQCxYNxPZHHbYy24094867 = iUQCxYNxPZHHbYy15897743;     iUQCxYNxPZHHbYy15897743 = iUQCxYNxPZHHbYy58803552;     iUQCxYNxPZHHbYy58803552 = iUQCxYNxPZHHbYy15022523;     iUQCxYNxPZHHbYy15022523 = iUQCxYNxPZHHbYy60173621;     iUQCxYNxPZHHbYy60173621 = iUQCxYNxPZHHbYy51427002;     iUQCxYNxPZHHbYy51427002 = iUQCxYNxPZHHbYy58631171;     iUQCxYNxPZHHbYy58631171 = iUQCxYNxPZHHbYy85096055;     iUQCxYNxPZHHbYy85096055 = iUQCxYNxPZHHbYy90981439;     iUQCxYNxPZHHbYy90981439 = iUQCxYNxPZHHbYy98939214;     iUQCxYNxPZHHbYy98939214 = iUQCxYNxPZHHbYy99842484;     iUQCxYNxPZHHbYy99842484 = iUQCxYNxPZHHbYy31914330;     iUQCxYNxPZHHbYy31914330 = iUQCxYNxPZHHbYy41652124;     iUQCxYNxPZHHbYy41652124 = iUQCxYNxPZHHbYy21947040;     iUQCxYNxPZHHbYy21947040 = iUQCxYNxPZHHbYy28608658;     iUQCxYNxPZHHbYy28608658 = iUQCxYNxPZHHbYy10588582;     iUQCxYNxPZHHbYy10588582 = iUQCxYNxPZHHbYy72724850;     iUQCxYNxPZHHbYy72724850 = iUQCxYNxPZHHbYy8397493;     iUQCxYNxPZHHbYy8397493 = iUQCxYNxPZHHbYy15918612;     iUQCxYNxPZHHbYy15918612 = iUQCxYNxPZHHbYy55949511;     iUQCxYNxPZHHbYy55949511 = iUQCxYNxPZHHbYy64780345;     iUQCxYNxPZHHbYy64780345 = iUQCxYNxPZHHbYy38029153;     iUQCxYNxPZHHbYy38029153 = iUQCxYNxPZHHbYy82844599;     iUQCxYNxPZHHbYy82844599 = iUQCxYNxPZHHbYy18227527;     iUQCxYNxPZHHbYy18227527 = iUQCxYNxPZHHbYy16373910;     iUQCxYNxPZHHbYy16373910 = iUQCxYNxPZHHbYy91099948;     iUQCxYNxPZHHbYy91099948 = iUQCxYNxPZHHbYy32024986;     iUQCxYNxPZHHbYy32024986 = iUQCxYNxPZHHbYy15510189;     iUQCxYNxPZHHbYy15510189 = iUQCxYNxPZHHbYy65651498;     iUQCxYNxPZHHbYy65651498 = iUQCxYNxPZHHbYy79793069;     iUQCxYNxPZHHbYy79793069 = iUQCxYNxPZHHbYy20022174;     iUQCxYNxPZHHbYy20022174 = iUQCxYNxPZHHbYy11618912;     iUQCxYNxPZHHbYy11618912 = iUQCxYNxPZHHbYy4889662;     iUQCxYNxPZHHbYy4889662 = iUQCxYNxPZHHbYy39131172;     iUQCxYNxPZHHbYy39131172 = iUQCxYNxPZHHbYy7887128;     iUQCxYNxPZHHbYy7887128 = iUQCxYNxPZHHbYy43326046;     iUQCxYNxPZHHbYy43326046 = iUQCxYNxPZHHbYy48541691;     iUQCxYNxPZHHbYy48541691 = iUQCxYNxPZHHbYy67543255;     iUQCxYNxPZHHbYy67543255 = iUQCxYNxPZHHbYy18362340;     iUQCxYNxPZHHbYy18362340 = iUQCxYNxPZHHbYy20432636;     iUQCxYNxPZHHbYy20432636 = iUQCxYNxPZHHbYy13878012;     iUQCxYNxPZHHbYy13878012 = iUQCxYNxPZHHbYy33312173;     iUQCxYNxPZHHbYy33312173 = iUQCxYNxPZHHbYy39502716;     iUQCxYNxPZHHbYy39502716 = iUQCxYNxPZHHbYy93004142;     iUQCxYNxPZHHbYy93004142 = iUQCxYNxPZHHbYy17919975;     iUQCxYNxPZHHbYy17919975 = iUQCxYNxPZHHbYy78328301;     iUQCxYNxPZHHbYy78328301 = iUQCxYNxPZHHbYy53031166;     iUQCxYNxPZHHbYy53031166 = iUQCxYNxPZHHbYy62824984;     iUQCxYNxPZHHbYy62824984 = iUQCxYNxPZHHbYy30677636;     iUQCxYNxPZHHbYy30677636 = iUQCxYNxPZHHbYy13339740;     iUQCxYNxPZHHbYy13339740 = iUQCxYNxPZHHbYy54350063;     iUQCxYNxPZHHbYy54350063 = iUQCxYNxPZHHbYy86443251;     iUQCxYNxPZHHbYy86443251 = iUQCxYNxPZHHbYy29736944;     iUQCxYNxPZHHbYy29736944 = iUQCxYNxPZHHbYy68133955;     iUQCxYNxPZHHbYy68133955 = iUQCxYNxPZHHbYy16399533;     iUQCxYNxPZHHbYy16399533 = iUQCxYNxPZHHbYy86065713;     iUQCxYNxPZHHbYy86065713 = iUQCxYNxPZHHbYy33053143;     iUQCxYNxPZHHbYy33053143 = iUQCxYNxPZHHbYy40576025;     iUQCxYNxPZHHbYy40576025 = iUQCxYNxPZHHbYy98648612;     iUQCxYNxPZHHbYy98648612 = iUQCxYNxPZHHbYy69073672;     iUQCxYNxPZHHbYy69073672 = iUQCxYNxPZHHbYy19402017;     iUQCxYNxPZHHbYy19402017 = iUQCxYNxPZHHbYy43120982;     iUQCxYNxPZHHbYy43120982 = iUQCxYNxPZHHbYy19444557;     iUQCxYNxPZHHbYy19444557 = iUQCxYNxPZHHbYy11188370;     iUQCxYNxPZHHbYy11188370 = iUQCxYNxPZHHbYy78917040;     iUQCxYNxPZHHbYy78917040 = iUQCxYNxPZHHbYy88223572;     iUQCxYNxPZHHbYy88223572 = iUQCxYNxPZHHbYy27024669;     iUQCxYNxPZHHbYy27024669 = iUQCxYNxPZHHbYy2520953;     iUQCxYNxPZHHbYy2520953 = iUQCxYNxPZHHbYy14059912;     iUQCxYNxPZHHbYy14059912 = iUQCxYNxPZHHbYy26616541;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void POflJpxSSXcTuieG90406198() {     int NVBKkXKiVLpeEPP97268207 = -680040398;    int NVBKkXKiVLpeEPP54456757 = -189127503;    int NVBKkXKiVLpeEPP96765927 = -285296916;    int NVBKkXKiVLpeEPP14727769 = -199751712;    int NVBKkXKiVLpeEPP11030141 = -833060601;    int NVBKkXKiVLpeEPP96867746 = -238794664;    int NVBKkXKiVLpeEPP33241813 = -652625812;    int NVBKkXKiVLpeEPP14116848 = -263026267;    int NVBKkXKiVLpeEPP74838181 = -105102305;    int NVBKkXKiVLpeEPP43277511 = -197712271;    int NVBKkXKiVLpeEPP74231155 = -616400235;    int NVBKkXKiVLpeEPP4770360 = -897007173;    int NVBKkXKiVLpeEPP87172630 = -148337041;    int NVBKkXKiVLpeEPP66412811 = -967870124;    int NVBKkXKiVLpeEPP61255980 = -574320641;    int NVBKkXKiVLpeEPP4388278 = -290110518;    int NVBKkXKiVLpeEPP66362009 = -539816977;    int NVBKkXKiVLpeEPP38701722 = -88464552;    int NVBKkXKiVLpeEPP12191937 = -202875174;    int NVBKkXKiVLpeEPP50730067 = -192318274;    int NVBKkXKiVLpeEPP72898213 = -668643579;    int NVBKkXKiVLpeEPP71638396 = -118283379;    int NVBKkXKiVLpeEPP96627098 = -380220816;    int NVBKkXKiVLpeEPP18621560 = -934559835;    int NVBKkXKiVLpeEPP23410682 = -133729336;    int NVBKkXKiVLpeEPP54525901 = 13052364;    int NVBKkXKiVLpeEPP35335700 = -616850443;    int NVBKkXKiVLpeEPP2622969 = -252579991;    int NVBKkXKiVLpeEPP77798043 = -982900427;    int NVBKkXKiVLpeEPP63255561 = -303436949;    int NVBKkXKiVLpeEPP88251874 = -421773269;    int NVBKkXKiVLpeEPP68606327 = -763957214;    int NVBKkXKiVLpeEPP98843103 = -391349261;    int NVBKkXKiVLpeEPP24168527 = -27113889;    int NVBKkXKiVLpeEPP93046758 = -347601522;    int NVBKkXKiVLpeEPP20166880 = -515005222;    int NVBKkXKiVLpeEPP95218487 = -647740055;    int NVBKkXKiVLpeEPP11423660 = -823624025;    int NVBKkXKiVLpeEPP63629344 = -578968004;    int NVBKkXKiVLpeEPP86084738 = -230921348;    int NVBKkXKiVLpeEPP59012498 = -449773915;    int NVBKkXKiVLpeEPP28325475 = -906457047;    int NVBKkXKiVLpeEPP80881290 = -511414503;    int NVBKkXKiVLpeEPP48160425 = -629021777;    int NVBKkXKiVLpeEPP17901098 = -642013351;    int NVBKkXKiVLpeEPP35014493 = -417283390;    int NVBKkXKiVLpeEPP36719785 = -563847640;    int NVBKkXKiVLpeEPP38339061 = -810957582;    int NVBKkXKiVLpeEPP13838945 = -247303213;    int NVBKkXKiVLpeEPP24346099 = -339809874;    int NVBKkXKiVLpeEPP24138836 = -47852827;    int NVBKkXKiVLpeEPP6651850 = -899462066;    int NVBKkXKiVLpeEPP93104810 = 65160550;    int NVBKkXKiVLpeEPP73071904 = -783199054;    int NVBKkXKiVLpeEPP17507986 = -96317452;    int NVBKkXKiVLpeEPP25629812 = -461757019;    int NVBKkXKiVLpeEPP57829658 = -808906687;    int NVBKkXKiVLpeEPP78144367 = -350737082;    int NVBKkXKiVLpeEPP91317087 = 33977623;    int NVBKkXKiVLpeEPP56504240 = -746112965;    int NVBKkXKiVLpeEPP61532046 = -621944221;    int NVBKkXKiVLpeEPP30618844 = -300045821;    int NVBKkXKiVLpeEPP36318804 = -280125840;    int NVBKkXKiVLpeEPP11582620 = -801665356;    int NVBKkXKiVLpeEPP55025636 = -775939002;    int NVBKkXKiVLpeEPP5624829 = -852443021;    int NVBKkXKiVLpeEPP5927256 = -405657913;    int NVBKkXKiVLpeEPP63004103 = -21223152;    int NVBKkXKiVLpeEPP73366053 = -520268603;    int NVBKkXKiVLpeEPP41089100 = 40684580;    int NVBKkXKiVLpeEPP9169790 = -642370463;    int NVBKkXKiVLpeEPP54938350 = -716192952;    int NVBKkXKiVLpeEPP75072378 = -509496549;    int NVBKkXKiVLpeEPP26107199 = -971953827;    int NVBKkXKiVLpeEPP91717568 = -742544359;    int NVBKkXKiVLpeEPP44572738 = -762186532;    int NVBKkXKiVLpeEPP90757105 = -606868877;    int NVBKkXKiVLpeEPP48466674 = -751199040;    int NVBKkXKiVLpeEPP720463 = -192546484;    int NVBKkXKiVLpeEPP88396189 = -716445946;    int NVBKkXKiVLpeEPP17806116 = -423099997;    int NVBKkXKiVLpeEPP96996639 = -805892861;    int NVBKkXKiVLpeEPP88784024 = 94723222;    int NVBKkXKiVLpeEPP53451945 = -543090554;    int NVBKkXKiVLpeEPP39116726 = -155584123;    int NVBKkXKiVLpeEPP81600025 = -522311204;    int NVBKkXKiVLpeEPP75501517 = -729117764;    int NVBKkXKiVLpeEPP25771200 = -608150207;    int NVBKkXKiVLpeEPP6660542 = -930796438;    int NVBKkXKiVLpeEPP67416947 = -885844503;    int NVBKkXKiVLpeEPP62337222 = -706098536;    int NVBKkXKiVLpeEPP17074120 = -197002974;    int NVBKkXKiVLpeEPP20106573 = -757601649;    int NVBKkXKiVLpeEPP7125105 = -832855039;    int NVBKkXKiVLpeEPP24552692 = -608977127;    int NVBKkXKiVLpeEPP28393654 = -49728095;    int NVBKkXKiVLpeEPP92006671 = -526331208;    int NVBKkXKiVLpeEPP69298670 = -709749148;    int NVBKkXKiVLpeEPP93134788 = -853082775;    int NVBKkXKiVLpeEPP12276270 = -680040398;     NVBKkXKiVLpeEPP97268207 = NVBKkXKiVLpeEPP54456757;     NVBKkXKiVLpeEPP54456757 = NVBKkXKiVLpeEPP96765927;     NVBKkXKiVLpeEPP96765927 = NVBKkXKiVLpeEPP14727769;     NVBKkXKiVLpeEPP14727769 = NVBKkXKiVLpeEPP11030141;     NVBKkXKiVLpeEPP11030141 = NVBKkXKiVLpeEPP96867746;     NVBKkXKiVLpeEPP96867746 = NVBKkXKiVLpeEPP33241813;     NVBKkXKiVLpeEPP33241813 = NVBKkXKiVLpeEPP14116848;     NVBKkXKiVLpeEPP14116848 = NVBKkXKiVLpeEPP74838181;     NVBKkXKiVLpeEPP74838181 = NVBKkXKiVLpeEPP43277511;     NVBKkXKiVLpeEPP43277511 = NVBKkXKiVLpeEPP74231155;     NVBKkXKiVLpeEPP74231155 = NVBKkXKiVLpeEPP4770360;     NVBKkXKiVLpeEPP4770360 = NVBKkXKiVLpeEPP87172630;     NVBKkXKiVLpeEPP87172630 = NVBKkXKiVLpeEPP66412811;     NVBKkXKiVLpeEPP66412811 = NVBKkXKiVLpeEPP61255980;     NVBKkXKiVLpeEPP61255980 = NVBKkXKiVLpeEPP4388278;     NVBKkXKiVLpeEPP4388278 = NVBKkXKiVLpeEPP66362009;     NVBKkXKiVLpeEPP66362009 = NVBKkXKiVLpeEPP38701722;     NVBKkXKiVLpeEPP38701722 = NVBKkXKiVLpeEPP12191937;     NVBKkXKiVLpeEPP12191937 = NVBKkXKiVLpeEPP50730067;     NVBKkXKiVLpeEPP50730067 = NVBKkXKiVLpeEPP72898213;     NVBKkXKiVLpeEPP72898213 = NVBKkXKiVLpeEPP71638396;     NVBKkXKiVLpeEPP71638396 = NVBKkXKiVLpeEPP96627098;     NVBKkXKiVLpeEPP96627098 = NVBKkXKiVLpeEPP18621560;     NVBKkXKiVLpeEPP18621560 = NVBKkXKiVLpeEPP23410682;     NVBKkXKiVLpeEPP23410682 = NVBKkXKiVLpeEPP54525901;     NVBKkXKiVLpeEPP54525901 = NVBKkXKiVLpeEPP35335700;     NVBKkXKiVLpeEPP35335700 = NVBKkXKiVLpeEPP2622969;     NVBKkXKiVLpeEPP2622969 = NVBKkXKiVLpeEPP77798043;     NVBKkXKiVLpeEPP77798043 = NVBKkXKiVLpeEPP63255561;     NVBKkXKiVLpeEPP63255561 = NVBKkXKiVLpeEPP88251874;     NVBKkXKiVLpeEPP88251874 = NVBKkXKiVLpeEPP68606327;     NVBKkXKiVLpeEPP68606327 = NVBKkXKiVLpeEPP98843103;     NVBKkXKiVLpeEPP98843103 = NVBKkXKiVLpeEPP24168527;     NVBKkXKiVLpeEPP24168527 = NVBKkXKiVLpeEPP93046758;     NVBKkXKiVLpeEPP93046758 = NVBKkXKiVLpeEPP20166880;     NVBKkXKiVLpeEPP20166880 = NVBKkXKiVLpeEPP95218487;     NVBKkXKiVLpeEPP95218487 = NVBKkXKiVLpeEPP11423660;     NVBKkXKiVLpeEPP11423660 = NVBKkXKiVLpeEPP63629344;     NVBKkXKiVLpeEPP63629344 = NVBKkXKiVLpeEPP86084738;     NVBKkXKiVLpeEPP86084738 = NVBKkXKiVLpeEPP59012498;     NVBKkXKiVLpeEPP59012498 = NVBKkXKiVLpeEPP28325475;     NVBKkXKiVLpeEPP28325475 = NVBKkXKiVLpeEPP80881290;     NVBKkXKiVLpeEPP80881290 = NVBKkXKiVLpeEPP48160425;     NVBKkXKiVLpeEPP48160425 = NVBKkXKiVLpeEPP17901098;     NVBKkXKiVLpeEPP17901098 = NVBKkXKiVLpeEPP35014493;     NVBKkXKiVLpeEPP35014493 = NVBKkXKiVLpeEPP36719785;     NVBKkXKiVLpeEPP36719785 = NVBKkXKiVLpeEPP38339061;     NVBKkXKiVLpeEPP38339061 = NVBKkXKiVLpeEPP13838945;     NVBKkXKiVLpeEPP13838945 = NVBKkXKiVLpeEPP24346099;     NVBKkXKiVLpeEPP24346099 = NVBKkXKiVLpeEPP24138836;     NVBKkXKiVLpeEPP24138836 = NVBKkXKiVLpeEPP6651850;     NVBKkXKiVLpeEPP6651850 = NVBKkXKiVLpeEPP93104810;     NVBKkXKiVLpeEPP93104810 = NVBKkXKiVLpeEPP73071904;     NVBKkXKiVLpeEPP73071904 = NVBKkXKiVLpeEPP17507986;     NVBKkXKiVLpeEPP17507986 = NVBKkXKiVLpeEPP25629812;     NVBKkXKiVLpeEPP25629812 = NVBKkXKiVLpeEPP57829658;     NVBKkXKiVLpeEPP57829658 = NVBKkXKiVLpeEPP78144367;     NVBKkXKiVLpeEPP78144367 = NVBKkXKiVLpeEPP91317087;     NVBKkXKiVLpeEPP91317087 = NVBKkXKiVLpeEPP56504240;     NVBKkXKiVLpeEPP56504240 = NVBKkXKiVLpeEPP61532046;     NVBKkXKiVLpeEPP61532046 = NVBKkXKiVLpeEPP30618844;     NVBKkXKiVLpeEPP30618844 = NVBKkXKiVLpeEPP36318804;     NVBKkXKiVLpeEPP36318804 = NVBKkXKiVLpeEPP11582620;     NVBKkXKiVLpeEPP11582620 = NVBKkXKiVLpeEPP55025636;     NVBKkXKiVLpeEPP55025636 = NVBKkXKiVLpeEPP5624829;     NVBKkXKiVLpeEPP5624829 = NVBKkXKiVLpeEPP5927256;     NVBKkXKiVLpeEPP5927256 = NVBKkXKiVLpeEPP63004103;     NVBKkXKiVLpeEPP63004103 = NVBKkXKiVLpeEPP73366053;     NVBKkXKiVLpeEPP73366053 = NVBKkXKiVLpeEPP41089100;     NVBKkXKiVLpeEPP41089100 = NVBKkXKiVLpeEPP9169790;     NVBKkXKiVLpeEPP9169790 = NVBKkXKiVLpeEPP54938350;     NVBKkXKiVLpeEPP54938350 = NVBKkXKiVLpeEPP75072378;     NVBKkXKiVLpeEPP75072378 = NVBKkXKiVLpeEPP26107199;     NVBKkXKiVLpeEPP26107199 = NVBKkXKiVLpeEPP91717568;     NVBKkXKiVLpeEPP91717568 = NVBKkXKiVLpeEPP44572738;     NVBKkXKiVLpeEPP44572738 = NVBKkXKiVLpeEPP90757105;     NVBKkXKiVLpeEPP90757105 = NVBKkXKiVLpeEPP48466674;     NVBKkXKiVLpeEPP48466674 = NVBKkXKiVLpeEPP720463;     NVBKkXKiVLpeEPP720463 = NVBKkXKiVLpeEPP88396189;     NVBKkXKiVLpeEPP88396189 = NVBKkXKiVLpeEPP17806116;     NVBKkXKiVLpeEPP17806116 = NVBKkXKiVLpeEPP96996639;     NVBKkXKiVLpeEPP96996639 = NVBKkXKiVLpeEPP88784024;     NVBKkXKiVLpeEPP88784024 = NVBKkXKiVLpeEPP53451945;     NVBKkXKiVLpeEPP53451945 = NVBKkXKiVLpeEPP39116726;     NVBKkXKiVLpeEPP39116726 = NVBKkXKiVLpeEPP81600025;     NVBKkXKiVLpeEPP81600025 = NVBKkXKiVLpeEPP75501517;     NVBKkXKiVLpeEPP75501517 = NVBKkXKiVLpeEPP25771200;     NVBKkXKiVLpeEPP25771200 = NVBKkXKiVLpeEPP6660542;     NVBKkXKiVLpeEPP6660542 = NVBKkXKiVLpeEPP67416947;     NVBKkXKiVLpeEPP67416947 = NVBKkXKiVLpeEPP62337222;     NVBKkXKiVLpeEPP62337222 = NVBKkXKiVLpeEPP17074120;     NVBKkXKiVLpeEPP17074120 = NVBKkXKiVLpeEPP20106573;     NVBKkXKiVLpeEPP20106573 = NVBKkXKiVLpeEPP7125105;     NVBKkXKiVLpeEPP7125105 = NVBKkXKiVLpeEPP24552692;     NVBKkXKiVLpeEPP24552692 = NVBKkXKiVLpeEPP28393654;     NVBKkXKiVLpeEPP28393654 = NVBKkXKiVLpeEPP92006671;     NVBKkXKiVLpeEPP92006671 = NVBKkXKiVLpeEPP69298670;     NVBKkXKiVLpeEPP69298670 = NVBKkXKiVLpeEPP93134788;     NVBKkXKiVLpeEPP93134788 = NVBKkXKiVLpeEPP12276270;     NVBKkXKiVLpeEPP12276270 = NVBKkXKiVLpeEPP97268207;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void zFoSKsJBubFgPWIS97064943() {     int wYjUDDJYntNbiDa97261736 = -552564542;    int wYjUDDJYntNbiDa93029073 = -231722634;    int wYjUDDJYntNbiDa98079139 = -242425530;    int wYjUDDJYntNbiDa99807629 = -20292539;    int wYjUDDJYntNbiDa19870243 = -201415931;    int wYjUDDJYntNbiDa31673179 = 61772524;    int wYjUDDJYntNbiDa1789149 = -357162279;    int wYjUDDJYntNbiDa83330898 = 32796401;    int wYjUDDJYntNbiDa57578345 = -918748376;    int wYjUDDJYntNbiDa39917025 = -934519142;    int wYjUDDJYntNbiDa94551760 = -575872308;    int wYjUDDJYntNbiDa19062037 = -632485545;    int wYjUDDJYntNbiDa19025545 = -169995515;    int wYjUDDJYntNbiDa20257032 = -213023177;    int wYjUDDJYntNbiDa6428090 = -537033512;    int wYjUDDJYntNbiDa78952824 = -622906169;    int wYjUDDJYntNbiDa22214769 = -359431547;    int wYjUDDJYntNbiDa61702254 = -485536178;    int wYjUDDJYntNbiDa32620207 = -820857440;    int wYjUDDJYntNbiDa22803830 = -804342725;    int wYjUDDJYntNbiDa52567637 = -379109958;    int wYjUDDJYntNbiDa49934662 = -628275262;    int wYjUDDJYntNbiDa33505496 = -839178659;    int wYjUDDJYntNbiDa62271668 = -815480357;    int wYjUDDJYntNbiDa51853371 = -560423201;    int wYjUDDJYntNbiDa71099978 = -995153560;    int wYjUDDJYntNbiDa5557282 = -917650156;    int wYjUDDJYntNbiDa38026494 = -861546985;    int wYjUDDJYntNbiDa90476490 = -528970727;    int wYjUDDJYntNbiDa39345432 = 21131677;    int wYjUDDJYntNbiDa21664553 = -808866504;    int wYjUDDJYntNbiDa53330185 = -55778973;    int wYjUDDJYntNbiDa84295526 = -593293474;    int wYjUDDJYntNbiDa24451365 = 96204378;    int wYjUDDJYntNbiDa75404796 = -606915443;    int wYjUDDJYntNbiDa62995070 = -594770950;    int wYjUDDJYntNbiDa7159856 = -747393434;    int wYjUDDJYntNbiDa81341338 = -818886707;    int wYjUDDJYntNbiDa98942940 = -191649408;    int wYjUDDJYntNbiDa57957630 = -940353034;    int wYjUDDJYntNbiDa46868166 = -537717814;    int wYjUDDJYntNbiDa14023136 = -249109;    int wYjUDDJYntNbiDa41164650 = 29455245;    int wYjUDDJYntNbiDa7392324 = -185986220;    int wYjUDDJYntNbiDa1919151 = -15272751;    int wYjUDDJYntNbiDa75442460 = -229328042;    int wYjUDDJYntNbiDa64417577 = 54871738;    int wYjUDDJYntNbiDa13722642 = -319718937;    int wYjUDDJYntNbiDa32497334 = -911067050;    int wYjUDDJYntNbiDa44391521 = 73165234;    int wYjUDDJYntNbiDa37398205 = -726407834;    int wYjUDDJYntNbiDa21199107 = -181607810;    int wYjUDDJYntNbiDa68553158 = -926893462;    int wYjUDDJYntNbiDa83322174 = -215118905;    int wYjUDDJYntNbiDa44497306 = -953216724;    int wYjUDDJYntNbiDa47327075 = -924289280;    int wYjUDDJYntNbiDa59523577 = -392543976;    int wYjUDDJYntNbiDa35807471 = -426945173;    int wYjUDDJYntNbiDa47954258 = -459869338;    int wYjUDDJYntNbiDa48770264 = -206262371;    int wYjUDDJYntNbiDa26115897 = -20577320;    int wYjUDDJYntNbiDa63762655 = -495615294;    int wYjUDDJYntNbiDa92854407 = -438232873;    int wYjUDDJYntNbiDa18232914 = -839880053;    int wYjUDDJYntNbiDa18252472 = -25652639;    int wYjUDDJYntNbiDa41221576 = -420093336;    int wYjUDDJYntNbiDa34766510 = 60807928;    int wYjUDDJYntNbiDa94574180 = -166199894;    int wYjUDDJYntNbiDa44852235 = -606107734;    int wYjUDDJYntNbiDa43433019 = -942262563;    int wYjUDDJYntNbiDa71792969 = -875512736;    int wYjUDDJYntNbiDa40873430 = -540544840;    int wYjUDDJYntNbiDa62759314 = -193886771;    int wYjUDDJYntNbiDa74662576 = -880504406;    int wYjUDDJYntNbiDa75935663 = -166624912;    int wYjUDDJYntNbiDa38544501 = -278860849;    int wYjUDDJYntNbiDa8770012 = -557730508;    int wYjUDDJYntNbiDa26113173 = -553192439;    int wYjUDDJYntNbiDa60352518 = -700207606;    int wYjUDDJYntNbiDa76410910 = -231095160;    int wYjUDDJYntNbiDa6682402 = -950025299;    int wYjUDDJYntNbiDa91834639 = -497931219;    int wYjUDDJYntNbiDa5529160 = -950479936;    int wYjUDDJYntNbiDa46084969 = -502135962;    int wYjUDDJYntNbiDa1947227 = -252460489;    int wYjUDDJYntNbiDa465447 = -527258694;    int wYjUDDJYntNbiDa84777026 = -128885511;    int wYjUDDJYntNbiDa973352 = -278174569;    int wYjUDDJYntNbiDa79954058 = 49421102;    int wYjUDDJYntNbiDa28077722 = -682626164;    int wYjUDDJYntNbiDa3471493 = -102226974;    int wYjUDDJYntNbiDa71352384 = -220448261;    int wYjUDDJYntNbiDa33387081 = -259017370;    int wYjUDDJYntNbiDa50172676 = -985387037;    int wYjUDDJYntNbiDa31841734 = -819775715;    int wYjUDDJYntNbiDa83105511 = 57897479;    int wYjUDDJYntNbiDa21168729 = -562016237;    int wYjUDDJYntNbiDa22931737 = -130664703;    int wYjUDDJYntNbiDa89139851 = -60333582;    int wYjUDDJYntNbiDa60697574 = -552564542;     wYjUDDJYntNbiDa97261736 = wYjUDDJYntNbiDa93029073;     wYjUDDJYntNbiDa93029073 = wYjUDDJYntNbiDa98079139;     wYjUDDJYntNbiDa98079139 = wYjUDDJYntNbiDa99807629;     wYjUDDJYntNbiDa99807629 = wYjUDDJYntNbiDa19870243;     wYjUDDJYntNbiDa19870243 = wYjUDDJYntNbiDa31673179;     wYjUDDJYntNbiDa31673179 = wYjUDDJYntNbiDa1789149;     wYjUDDJYntNbiDa1789149 = wYjUDDJYntNbiDa83330898;     wYjUDDJYntNbiDa83330898 = wYjUDDJYntNbiDa57578345;     wYjUDDJYntNbiDa57578345 = wYjUDDJYntNbiDa39917025;     wYjUDDJYntNbiDa39917025 = wYjUDDJYntNbiDa94551760;     wYjUDDJYntNbiDa94551760 = wYjUDDJYntNbiDa19062037;     wYjUDDJYntNbiDa19062037 = wYjUDDJYntNbiDa19025545;     wYjUDDJYntNbiDa19025545 = wYjUDDJYntNbiDa20257032;     wYjUDDJYntNbiDa20257032 = wYjUDDJYntNbiDa6428090;     wYjUDDJYntNbiDa6428090 = wYjUDDJYntNbiDa78952824;     wYjUDDJYntNbiDa78952824 = wYjUDDJYntNbiDa22214769;     wYjUDDJYntNbiDa22214769 = wYjUDDJYntNbiDa61702254;     wYjUDDJYntNbiDa61702254 = wYjUDDJYntNbiDa32620207;     wYjUDDJYntNbiDa32620207 = wYjUDDJYntNbiDa22803830;     wYjUDDJYntNbiDa22803830 = wYjUDDJYntNbiDa52567637;     wYjUDDJYntNbiDa52567637 = wYjUDDJYntNbiDa49934662;     wYjUDDJYntNbiDa49934662 = wYjUDDJYntNbiDa33505496;     wYjUDDJYntNbiDa33505496 = wYjUDDJYntNbiDa62271668;     wYjUDDJYntNbiDa62271668 = wYjUDDJYntNbiDa51853371;     wYjUDDJYntNbiDa51853371 = wYjUDDJYntNbiDa71099978;     wYjUDDJYntNbiDa71099978 = wYjUDDJYntNbiDa5557282;     wYjUDDJYntNbiDa5557282 = wYjUDDJYntNbiDa38026494;     wYjUDDJYntNbiDa38026494 = wYjUDDJYntNbiDa90476490;     wYjUDDJYntNbiDa90476490 = wYjUDDJYntNbiDa39345432;     wYjUDDJYntNbiDa39345432 = wYjUDDJYntNbiDa21664553;     wYjUDDJYntNbiDa21664553 = wYjUDDJYntNbiDa53330185;     wYjUDDJYntNbiDa53330185 = wYjUDDJYntNbiDa84295526;     wYjUDDJYntNbiDa84295526 = wYjUDDJYntNbiDa24451365;     wYjUDDJYntNbiDa24451365 = wYjUDDJYntNbiDa75404796;     wYjUDDJYntNbiDa75404796 = wYjUDDJYntNbiDa62995070;     wYjUDDJYntNbiDa62995070 = wYjUDDJYntNbiDa7159856;     wYjUDDJYntNbiDa7159856 = wYjUDDJYntNbiDa81341338;     wYjUDDJYntNbiDa81341338 = wYjUDDJYntNbiDa98942940;     wYjUDDJYntNbiDa98942940 = wYjUDDJYntNbiDa57957630;     wYjUDDJYntNbiDa57957630 = wYjUDDJYntNbiDa46868166;     wYjUDDJYntNbiDa46868166 = wYjUDDJYntNbiDa14023136;     wYjUDDJYntNbiDa14023136 = wYjUDDJYntNbiDa41164650;     wYjUDDJYntNbiDa41164650 = wYjUDDJYntNbiDa7392324;     wYjUDDJYntNbiDa7392324 = wYjUDDJYntNbiDa1919151;     wYjUDDJYntNbiDa1919151 = wYjUDDJYntNbiDa75442460;     wYjUDDJYntNbiDa75442460 = wYjUDDJYntNbiDa64417577;     wYjUDDJYntNbiDa64417577 = wYjUDDJYntNbiDa13722642;     wYjUDDJYntNbiDa13722642 = wYjUDDJYntNbiDa32497334;     wYjUDDJYntNbiDa32497334 = wYjUDDJYntNbiDa44391521;     wYjUDDJYntNbiDa44391521 = wYjUDDJYntNbiDa37398205;     wYjUDDJYntNbiDa37398205 = wYjUDDJYntNbiDa21199107;     wYjUDDJYntNbiDa21199107 = wYjUDDJYntNbiDa68553158;     wYjUDDJYntNbiDa68553158 = wYjUDDJYntNbiDa83322174;     wYjUDDJYntNbiDa83322174 = wYjUDDJYntNbiDa44497306;     wYjUDDJYntNbiDa44497306 = wYjUDDJYntNbiDa47327075;     wYjUDDJYntNbiDa47327075 = wYjUDDJYntNbiDa59523577;     wYjUDDJYntNbiDa59523577 = wYjUDDJYntNbiDa35807471;     wYjUDDJYntNbiDa35807471 = wYjUDDJYntNbiDa47954258;     wYjUDDJYntNbiDa47954258 = wYjUDDJYntNbiDa48770264;     wYjUDDJYntNbiDa48770264 = wYjUDDJYntNbiDa26115897;     wYjUDDJYntNbiDa26115897 = wYjUDDJYntNbiDa63762655;     wYjUDDJYntNbiDa63762655 = wYjUDDJYntNbiDa92854407;     wYjUDDJYntNbiDa92854407 = wYjUDDJYntNbiDa18232914;     wYjUDDJYntNbiDa18232914 = wYjUDDJYntNbiDa18252472;     wYjUDDJYntNbiDa18252472 = wYjUDDJYntNbiDa41221576;     wYjUDDJYntNbiDa41221576 = wYjUDDJYntNbiDa34766510;     wYjUDDJYntNbiDa34766510 = wYjUDDJYntNbiDa94574180;     wYjUDDJYntNbiDa94574180 = wYjUDDJYntNbiDa44852235;     wYjUDDJYntNbiDa44852235 = wYjUDDJYntNbiDa43433019;     wYjUDDJYntNbiDa43433019 = wYjUDDJYntNbiDa71792969;     wYjUDDJYntNbiDa71792969 = wYjUDDJYntNbiDa40873430;     wYjUDDJYntNbiDa40873430 = wYjUDDJYntNbiDa62759314;     wYjUDDJYntNbiDa62759314 = wYjUDDJYntNbiDa74662576;     wYjUDDJYntNbiDa74662576 = wYjUDDJYntNbiDa75935663;     wYjUDDJYntNbiDa75935663 = wYjUDDJYntNbiDa38544501;     wYjUDDJYntNbiDa38544501 = wYjUDDJYntNbiDa8770012;     wYjUDDJYntNbiDa8770012 = wYjUDDJYntNbiDa26113173;     wYjUDDJYntNbiDa26113173 = wYjUDDJYntNbiDa60352518;     wYjUDDJYntNbiDa60352518 = wYjUDDJYntNbiDa76410910;     wYjUDDJYntNbiDa76410910 = wYjUDDJYntNbiDa6682402;     wYjUDDJYntNbiDa6682402 = wYjUDDJYntNbiDa91834639;     wYjUDDJYntNbiDa91834639 = wYjUDDJYntNbiDa5529160;     wYjUDDJYntNbiDa5529160 = wYjUDDJYntNbiDa46084969;     wYjUDDJYntNbiDa46084969 = wYjUDDJYntNbiDa1947227;     wYjUDDJYntNbiDa1947227 = wYjUDDJYntNbiDa465447;     wYjUDDJYntNbiDa465447 = wYjUDDJYntNbiDa84777026;     wYjUDDJYntNbiDa84777026 = wYjUDDJYntNbiDa973352;     wYjUDDJYntNbiDa973352 = wYjUDDJYntNbiDa79954058;     wYjUDDJYntNbiDa79954058 = wYjUDDJYntNbiDa28077722;     wYjUDDJYntNbiDa28077722 = wYjUDDJYntNbiDa3471493;     wYjUDDJYntNbiDa3471493 = wYjUDDJYntNbiDa71352384;     wYjUDDJYntNbiDa71352384 = wYjUDDJYntNbiDa33387081;     wYjUDDJYntNbiDa33387081 = wYjUDDJYntNbiDa50172676;     wYjUDDJYntNbiDa50172676 = wYjUDDJYntNbiDa31841734;     wYjUDDJYntNbiDa31841734 = wYjUDDJYntNbiDa83105511;     wYjUDDJYntNbiDa83105511 = wYjUDDJYntNbiDa21168729;     wYjUDDJYntNbiDa21168729 = wYjUDDJYntNbiDa22931737;     wYjUDDJYntNbiDa22931737 = wYjUDDJYntNbiDa89139851;     wYjUDDJYntNbiDa89139851 = wYjUDDJYntNbiDa60697574;     wYjUDDJYntNbiDa60697574 = wYjUDDJYntNbiDa97261736;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void oXHyFgswxQPYfGEc51481156() {     int SGhyctEXbZkDgud67913403 = -70713670;    int SGhyctEXbZkDgud61702591 = -103365927;    int SGhyctEXbZkDgud78048058 = -684719990;    int SGhyctEXbZkDgud39738916 = -210430931;    int SGhyctEXbZkDgud41320564 = 15177730;    int SGhyctEXbZkDgud21672941 = -739808627;    int SGhyctEXbZkDgud20168045 = -355937988;    int SGhyctEXbZkDgud40173279 = -314020674;    int SGhyctEXbZkDgud3443400 = 87904297;    int SGhyctEXbZkDgud62883485 = -135614492;    int SGhyctEXbZkDgud36800922 = -610774236;    int SGhyctEXbZkDgud64608607 = -948249329;    int SGhyctEXbZkDgud98852931 = -79532574;    int SGhyctEXbZkDgud4104066 = -14051386;    int SGhyctEXbZkDgud89148108 = -686783497;    int SGhyctEXbZkDgud11481465 = -192889756;    int SGhyctEXbZkDgud16067596 = -268616677;    int SGhyctEXbZkDgud81995748 = -474702370;    int SGhyctEXbZkDgud14327988 = -428045383;    int SGhyctEXbZkDgud81590538 = -246709030;    int SGhyctEXbZkDgud7703392 = -287789093;    int SGhyctEXbZkDgud11330427 = -959560568;    int SGhyctEXbZkDgud35449304 = -307021803;    int SGhyctEXbZkDgud96121205 = -407881263;    int SGhyctEXbZkDgud15977760 = -236327877;    int SGhyctEXbZkDgud1697559 = 6561991;    int SGhyctEXbZkDgud13818067 = -212827931;    int SGhyctEXbZkDgud45808719 = -266670747;    int SGhyctEXbZkDgud22618979 = -823328959;    int SGhyctEXbZkDgud78517527 = -537263394;    int SGhyctEXbZkDgud28736550 = -557300189;    int SGhyctEXbZkDgud97841645 = -384076139;    int SGhyctEXbZkDgud67240887 = -540255086;    int SGhyctEXbZkDgud89816339 = -567325306;    int SGhyctEXbZkDgud53429033 = -453862226;    int SGhyctEXbZkDgud22988329 = -673997516;    int SGhyctEXbZkDgud50951341 = -935986641;    int SGhyctEXbZkDgud34133828 = -602999812;    int SGhyctEXbZkDgud77476229 = -147378057;    int SGhyctEXbZkDgud53060929 = -457961397;    int SGhyctEXbZkDgud6941450 = -921736692;    int SGhyctEXbZkDgud42506127 = -869638199;    int SGhyctEXbZkDgud90131610 = 42701191;    int SGhyctEXbZkDgud13900624 = -257961191;    int SGhyctEXbZkDgud97873209 = -323919147;    int SGhyctEXbZkDgud81848295 = 12334785;    int SGhyctEXbZkDgud90548780 = -61198061;    int SGhyctEXbZkDgud79336851 = -771849234;    int SGhyctEXbZkDgud37938786 = -896831044;    int SGhyctEXbZkDgud52819008 = -209843639;    int SGhyctEXbZkDgud5587530 = -272979955;    int SGhyctEXbZkDgud63070611 = -150827536;    int SGhyctEXbZkDgud23628816 = -854691501;    int SGhyctEXbZkDgud73549479 = -876000402;    int SGhyctEXbZkDgud43777765 = 35842356;    int SGhyctEXbZkDgud56582977 = -111153102;    int SGhyctEXbZkDgud26253287 = -796344124;    int SGhyctEXbZkDgud81926853 = -176838727;    int SGhyctEXbZkDgud23761157 = -974103055;    int SGhyctEXbZkDgud39623006 = -991384261;    int SGhyctEXbZkDgud7854874 = -426980696;    int SGhyctEXbZkDgud74359325 = 10732758;    int SGhyctEXbZkDgud17554301 = -490691715;    int SGhyctEXbZkDgud24925872 = -374832310;    int SGhyctEXbZkDgud34146936 = -578314304;    int SGhyctEXbZkDgud38959276 = -126698098;    int SGhyctEXbZkDgud97367720 = -307994243;    int SGhyctEXbZkDgud9036593 = -512207268;    int SGhyctEXbZkDgud50675032 = -560189161;    int SGhyctEXbZkDgud66159780 = 87214019;    int SGhyctEXbZkDgud60530123 = -256903115;    int SGhyctEXbZkDgud81933768 = -665616865;    int SGhyctEXbZkDgud4519520 = -227324314;    int SGhyctEXbZkDgud61267059 = -970083987;    int SGhyctEXbZkDgud74649088 = -324972338;    int SGhyctEXbZkDgud65197264 = -418150895;    int SGhyctEXbZkDgud21198817 = -902261759;    int SGhyctEXbZkDgud21548681 = 50939388;    int SGhyctEXbZkDgud98247996 = 16037883;    int SGhyctEXbZkDgud34129464 = -148662662;    int SGhyctEXbZkDgud11148778 = -932239949;    int SGhyctEXbZkDgud34481216 = -440978698;    int SGhyctEXbZkDgud7869933 = -369839704;    int SGhyctEXbZkDgud69799970 = -513485321;    int SGhyctEXbZkDgud72929998 = -164283439;    int SGhyctEXbZkDgud65665938 = -306472653;    int SGhyctEXbZkDgud74212830 = -529384638;    int SGhyctEXbZkDgud93691408 = -664254684;    int SGhyctEXbZkDgud46038574 = -503167663;    int SGhyctEXbZkDgud96846056 = -242709124;    int SGhyctEXbZkDgud96735041 = -877653392;    int SGhyctEXbZkDgud69024488 = -659147915;    int SGhyctEXbZkDgud10372671 = -628896757;    int SGhyctEXbZkDgud37853224 = -155993796;    int SGhyctEXbZkDgud45206055 = 69019299;    int SGhyctEXbZkDgud32582125 = -832469451;    int SGhyctEXbZkDgud24951827 = -278946484;    int SGhyctEXbZkDgud65205738 = -582466500;    int SGhyctEXbZkDgud79753688 = -679646888;    int SGhyctEXbZkDgud58913933 = -70713670;     SGhyctEXbZkDgud67913403 = SGhyctEXbZkDgud61702591;     SGhyctEXbZkDgud61702591 = SGhyctEXbZkDgud78048058;     SGhyctEXbZkDgud78048058 = SGhyctEXbZkDgud39738916;     SGhyctEXbZkDgud39738916 = SGhyctEXbZkDgud41320564;     SGhyctEXbZkDgud41320564 = SGhyctEXbZkDgud21672941;     SGhyctEXbZkDgud21672941 = SGhyctEXbZkDgud20168045;     SGhyctEXbZkDgud20168045 = SGhyctEXbZkDgud40173279;     SGhyctEXbZkDgud40173279 = SGhyctEXbZkDgud3443400;     SGhyctEXbZkDgud3443400 = SGhyctEXbZkDgud62883485;     SGhyctEXbZkDgud62883485 = SGhyctEXbZkDgud36800922;     SGhyctEXbZkDgud36800922 = SGhyctEXbZkDgud64608607;     SGhyctEXbZkDgud64608607 = SGhyctEXbZkDgud98852931;     SGhyctEXbZkDgud98852931 = SGhyctEXbZkDgud4104066;     SGhyctEXbZkDgud4104066 = SGhyctEXbZkDgud89148108;     SGhyctEXbZkDgud89148108 = SGhyctEXbZkDgud11481465;     SGhyctEXbZkDgud11481465 = SGhyctEXbZkDgud16067596;     SGhyctEXbZkDgud16067596 = SGhyctEXbZkDgud81995748;     SGhyctEXbZkDgud81995748 = SGhyctEXbZkDgud14327988;     SGhyctEXbZkDgud14327988 = SGhyctEXbZkDgud81590538;     SGhyctEXbZkDgud81590538 = SGhyctEXbZkDgud7703392;     SGhyctEXbZkDgud7703392 = SGhyctEXbZkDgud11330427;     SGhyctEXbZkDgud11330427 = SGhyctEXbZkDgud35449304;     SGhyctEXbZkDgud35449304 = SGhyctEXbZkDgud96121205;     SGhyctEXbZkDgud96121205 = SGhyctEXbZkDgud15977760;     SGhyctEXbZkDgud15977760 = SGhyctEXbZkDgud1697559;     SGhyctEXbZkDgud1697559 = SGhyctEXbZkDgud13818067;     SGhyctEXbZkDgud13818067 = SGhyctEXbZkDgud45808719;     SGhyctEXbZkDgud45808719 = SGhyctEXbZkDgud22618979;     SGhyctEXbZkDgud22618979 = SGhyctEXbZkDgud78517527;     SGhyctEXbZkDgud78517527 = SGhyctEXbZkDgud28736550;     SGhyctEXbZkDgud28736550 = SGhyctEXbZkDgud97841645;     SGhyctEXbZkDgud97841645 = SGhyctEXbZkDgud67240887;     SGhyctEXbZkDgud67240887 = SGhyctEXbZkDgud89816339;     SGhyctEXbZkDgud89816339 = SGhyctEXbZkDgud53429033;     SGhyctEXbZkDgud53429033 = SGhyctEXbZkDgud22988329;     SGhyctEXbZkDgud22988329 = SGhyctEXbZkDgud50951341;     SGhyctEXbZkDgud50951341 = SGhyctEXbZkDgud34133828;     SGhyctEXbZkDgud34133828 = SGhyctEXbZkDgud77476229;     SGhyctEXbZkDgud77476229 = SGhyctEXbZkDgud53060929;     SGhyctEXbZkDgud53060929 = SGhyctEXbZkDgud6941450;     SGhyctEXbZkDgud6941450 = SGhyctEXbZkDgud42506127;     SGhyctEXbZkDgud42506127 = SGhyctEXbZkDgud90131610;     SGhyctEXbZkDgud90131610 = SGhyctEXbZkDgud13900624;     SGhyctEXbZkDgud13900624 = SGhyctEXbZkDgud97873209;     SGhyctEXbZkDgud97873209 = SGhyctEXbZkDgud81848295;     SGhyctEXbZkDgud81848295 = SGhyctEXbZkDgud90548780;     SGhyctEXbZkDgud90548780 = SGhyctEXbZkDgud79336851;     SGhyctEXbZkDgud79336851 = SGhyctEXbZkDgud37938786;     SGhyctEXbZkDgud37938786 = SGhyctEXbZkDgud52819008;     SGhyctEXbZkDgud52819008 = SGhyctEXbZkDgud5587530;     SGhyctEXbZkDgud5587530 = SGhyctEXbZkDgud63070611;     SGhyctEXbZkDgud63070611 = SGhyctEXbZkDgud23628816;     SGhyctEXbZkDgud23628816 = SGhyctEXbZkDgud73549479;     SGhyctEXbZkDgud73549479 = SGhyctEXbZkDgud43777765;     SGhyctEXbZkDgud43777765 = SGhyctEXbZkDgud56582977;     SGhyctEXbZkDgud56582977 = SGhyctEXbZkDgud26253287;     SGhyctEXbZkDgud26253287 = SGhyctEXbZkDgud81926853;     SGhyctEXbZkDgud81926853 = SGhyctEXbZkDgud23761157;     SGhyctEXbZkDgud23761157 = SGhyctEXbZkDgud39623006;     SGhyctEXbZkDgud39623006 = SGhyctEXbZkDgud7854874;     SGhyctEXbZkDgud7854874 = SGhyctEXbZkDgud74359325;     SGhyctEXbZkDgud74359325 = SGhyctEXbZkDgud17554301;     SGhyctEXbZkDgud17554301 = SGhyctEXbZkDgud24925872;     SGhyctEXbZkDgud24925872 = SGhyctEXbZkDgud34146936;     SGhyctEXbZkDgud34146936 = SGhyctEXbZkDgud38959276;     SGhyctEXbZkDgud38959276 = SGhyctEXbZkDgud97367720;     SGhyctEXbZkDgud97367720 = SGhyctEXbZkDgud9036593;     SGhyctEXbZkDgud9036593 = SGhyctEXbZkDgud50675032;     SGhyctEXbZkDgud50675032 = SGhyctEXbZkDgud66159780;     SGhyctEXbZkDgud66159780 = SGhyctEXbZkDgud60530123;     SGhyctEXbZkDgud60530123 = SGhyctEXbZkDgud81933768;     SGhyctEXbZkDgud81933768 = SGhyctEXbZkDgud4519520;     SGhyctEXbZkDgud4519520 = SGhyctEXbZkDgud61267059;     SGhyctEXbZkDgud61267059 = SGhyctEXbZkDgud74649088;     SGhyctEXbZkDgud74649088 = SGhyctEXbZkDgud65197264;     SGhyctEXbZkDgud65197264 = SGhyctEXbZkDgud21198817;     SGhyctEXbZkDgud21198817 = SGhyctEXbZkDgud21548681;     SGhyctEXbZkDgud21548681 = SGhyctEXbZkDgud98247996;     SGhyctEXbZkDgud98247996 = SGhyctEXbZkDgud34129464;     SGhyctEXbZkDgud34129464 = SGhyctEXbZkDgud11148778;     SGhyctEXbZkDgud11148778 = SGhyctEXbZkDgud34481216;     SGhyctEXbZkDgud34481216 = SGhyctEXbZkDgud7869933;     SGhyctEXbZkDgud7869933 = SGhyctEXbZkDgud69799970;     SGhyctEXbZkDgud69799970 = SGhyctEXbZkDgud72929998;     SGhyctEXbZkDgud72929998 = SGhyctEXbZkDgud65665938;     SGhyctEXbZkDgud65665938 = SGhyctEXbZkDgud74212830;     SGhyctEXbZkDgud74212830 = SGhyctEXbZkDgud93691408;     SGhyctEXbZkDgud93691408 = SGhyctEXbZkDgud46038574;     SGhyctEXbZkDgud46038574 = SGhyctEXbZkDgud96846056;     SGhyctEXbZkDgud96846056 = SGhyctEXbZkDgud96735041;     SGhyctEXbZkDgud96735041 = SGhyctEXbZkDgud69024488;     SGhyctEXbZkDgud69024488 = SGhyctEXbZkDgud10372671;     SGhyctEXbZkDgud10372671 = SGhyctEXbZkDgud37853224;     SGhyctEXbZkDgud37853224 = SGhyctEXbZkDgud45206055;     SGhyctEXbZkDgud45206055 = SGhyctEXbZkDgud32582125;     SGhyctEXbZkDgud32582125 = SGhyctEXbZkDgud24951827;     SGhyctEXbZkDgud24951827 = SGhyctEXbZkDgud65205738;     SGhyctEXbZkDgud65205738 = SGhyctEXbZkDgud79753688;     SGhyctEXbZkDgud79753688 = SGhyctEXbZkDgud58913933;     SGhyctEXbZkDgud58913933 = SGhyctEXbZkDgud67913403;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void wwyRcVwosjYTxbaD58139901() {     int oOFJdinDQyVABBW67906932 = 56762187;    int oOFJdinDQyVABBW274908 = -145961059;    int oOFJdinDQyVABBW79361271 = -641848604;    int oOFJdinDQyVABBW24818777 = -30971758;    int oOFJdinDQyVABBW50160666 = -453177599;    int oOFJdinDQyVABBW56478373 = -439241439;    int oOFJdinDQyVABBW88715380 = -60474455;    int oOFJdinDQyVABBW9387330 = -18198006;    int oOFJdinDQyVABBW86183563 = -725741774;    int oOFJdinDQyVABBW59522999 = -872421363;    int oOFJdinDQyVABBW57121527 = -570246309;    int oOFJdinDQyVABBW78900284 = -683727700;    int oOFJdinDQyVABBW30705846 = -101191048;    int oOFJdinDQyVABBW57948285 = -359204439;    int oOFJdinDQyVABBW34320218 = -649496367;    int oOFJdinDQyVABBW86046012 = -525685406;    int oOFJdinDQyVABBW71920355 = -88231247;    int oOFJdinDQyVABBW4996282 = -871773996;    int oOFJdinDQyVABBW34756259 = 53972351;    int oOFJdinDQyVABBW53664302 = -858733481;    int oOFJdinDQyVABBW87372815 = 1744528;    int oOFJdinDQyVABBW89626692 = -369552451;    int oOFJdinDQyVABBW72327701 = -765979645;    int oOFJdinDQyVABBW39771314 = -288801786;    int oOFJdinDQyVABBW44420449 = -663021742;    int oOFJdinDQyVABBW18271637 = 98356067;    int oOFJdinDQyVABBW84039648 = -513627644;    int oOFJdinDQyVABBW81212243 = -875637741;    int oOFJdinDQyVABBW35297425 = -369399259;    int oOFJdinDQyVABBW54607397 = -212694768;    int oOFJdinDQyVABBW62149228 = -944393424;    int oOFJdinDQyVABBW82565503 = -775897897;    int oOFJdinDQyVABBW52693311 = -742199299;    int oOFJdinDQyVABBW90099176 = -444007038;    int oOFJdinDQyVABBW35787071 = -713176147;    int oOFJdinDQyVABBW65816518 = -753763244;    int oOFJdinDQyVABBW62892709 = 64359981;    int oOFJdinDQyVABBW4051507 = -598262494;    int oOFJdinDQyVABBW12789826 = -860059461;    int oOFJdinDQyVABBW24933822 = -67393083;    int oOFJdinDQyVABBW94797117 = 90319409;    int oOFJdinDQyVABBW28203788 = 36569739;    int oOFJdinDQyVABBW50414970 = -516429061;    int oOFJdinDQyVABBW73132522 = -914925635;    int oOFJdinDQyVABBW81891261 = -797178547;    int oOFJdinDQyVABBW22276264 = -899709868;    int oOFJdinDQyVABBW18246573 = -542478683;    int oOFJdinDQyVABBW54720432 = -280610589;    int oOFJdinDQyVABBW56597176 = -460594880;    int oOFJdinDQyVABBW72864430 = -896868531;    int oOFJdinDQyVABBW18846900 = -951534963;    int oOFJdinDQyVABBW77617869 = -532973280;    int oOFJdinDQyVABBW99077163 = -746745512;    int oOFJdinDQyVABBW83799750 = -307920253;    int oOFJdinDQyVABBW70767085 = -821056916;    int oOFJdinDQyVABBW78280239 = -573685363;    int oOFJdinDQyVABBW27947206 = -379981414;    int oOFJdinDQyVABBW39589957 = -253046818;    int oOFJdinDQyVABBW80398327 = -367950017;    int oOFJdinDQyVABBW31889030 = -451533666;    int oOFJdinDQyVABBW72438725 = -925613795;    int oOFJdinDQyVABBW7503137 = -184836715;    int oOFJdinDQyVABBW74089904 = -648798748;    int oOFJdinDQyVABBW31576167 = -413047007;    int oOFJdinDQyVABBW97373770 = -928027940;    int oOFJdinDQyVABBW74556024 = -794348413;    int oOFJdinDQyVABBW26206974 = -941528402;    int oOFJdinDQyVABBW40606669 = -657184010;    int oOFJdinDQyVABBW22161215 = -646028292;    int oOFJdinDQyVABBW68503699 = -895733124;    int oOFJdinDQyVABBW23153304 = -490045387;    int oOFJdinDQyVABBW67868848 = -489968753;    int oOFJdinDQyVABBW92206455 = 88285464;    int oOFJdinDQyVABBW9822437 = -878634566;    int oOFJdinDQyVABBW58867184 = -849052891;    int oOFJdinDQyVABBW59169027 = 65174788;    int oOFJdinDQyVABBW39211722 = -853123390;    int oOFJdinDQyVABBW99195178 = -851054011;    int oOFJdinDQyVABBW57880052 = -491623239;    int oOFJdinDQyVABBW22144186 = -763311875;    int oOFJdinDQyVABBW25064 = -359165251;    int oOFJdinDQyVABBW29319216 = -133017056;    int oOFJdinDQyVABBW24615068 = -315042861;    int oOFJdinDQyVABBW62432995 = -472530729;    int oOFJdinDQyVABBW35760498 = -261159805;    int oOFJdinDQyVABBW84531359 = -311420144;    int oOFJdinDQyVABBW83488339 = 70847614;    int oOFJdinDQyVABBW68893560 = -334279046;    int oOFJdinDQyVABBW19332091 = -622950123;    int oOFJdinDQyVABBW57506831 = -39490785;    int oOFJdinDQyVABBW37869312 = -273781831;    int oOFJdinDQyVABBW23302752 = -682593202;    int oOFJdinDQyVABBW23653180 = -130312478;    int oOFJdinDQyVABBW80900795 = -308525795;    int oOFJdinDQyVABBW52495097 = -141779288;    int oOFJdinDQyVABBW87293981 = -724843877;    int oOFJdinDQyVABBW54113884 = -314631513;    int oOFJdinDQyVABBW18838804 = -3382055;    int oOFJdinDQyVABBW75758751 = -986897695;    int oOFJdinDQyVABBW7335238 = 56762187;     oOFJdinDQyVABBW67906932 = oOFJdinDQyVABBW274908;     oOFJdinDQyVABBW274908 = oOFJdinDQyVABBW79361271;     oOFJdinDQyVABBW79361271 = oOFJdinDQyVABBW24818777;     oOFJdinDQyVABBW24818777 = oOFJdinDQyVABBW50160666;     oOFJdinDQyVABBW50160666 = oOFJdinDQyVABBW56478373;     oOFJdinDQyVABBW56478373 = oOFJdinDQyVABBW88715380;     oOFJdinDQyVABBW88715380 = oOFJdinDQyVABBW9387330;     oOFJdinDQyVABBW9387330 = oOFJdinDQyVABBW86183563;     oOFJdinDQyVABBW86183563 = oOFJdinDQyVABBW59522999;     oOFJdinDQyVABBW59522999 = oOFJdinDQyVABBW57121527;     oOFJdinDQyVABBW57121527 = oOFJdinDQyVABBW78900284;     oOFJdinDQyVABBW78900284 = oOFJdinDQyVABBW30705846;     oOFJdinDQyVABBW30705846 = oOFJdinDQyVABBW57948285;     oOFJdinDQyVABBW57948285 = oOFJdinDQyVABBW34320218;     oOFJdinDQyVABBW34320218 = oOFJdinDQyVABBW86046012;     oOFJdinDQyVABBW86046012 = oOFJdinDQyVABBW71920355;     oOFJdinDQyVABBW71920355 = oOFJdinDQyVABBW4996282;     oOFJdinDQyVABBW4996282 = oOFJdinDQyVABBW34756259;     oOFJdinDQyVABBW34756259 = oOFJdinDQyVABBW53664302;     oOFJdinDQyVABBW53664302 = oOFJdinDQyVABBW87372815;     oOFJdinDQyVABBW87372815 = oOFJdinDQyVABBW89626692;     oOFJdinDQyVABBW89626692 = oOFJdinDQyVABBW72327701;     oOFJdinDQyVABBW72327701 = oOFJdinDQyVABBW39771314;     oOFJdinDQyVABBW39771314 = oOFJdinDQyVABBW44420449;     oOFJdinDQyVABBW44420449 = oOFJdinDQyVABBW18271637;     oOFJdinDQyVABBW18271637 = oOFJdinDQyVABBW84039648;     oOFJdinDQyVABBW84039648 = oOFJdinDQyVABBW81212243;     oOFJdinDQyVABBW81212243 = oOFJdinDQyVABBW35297425;     oOFJdinDQyVABBW35297425 = oOFJdinDQyVABBW54607397;     oOFJdinDQyVABBW54607397 = oOFJdinDQyVABBW62149228;     oOFJdinDQyVABBW62149228 = oOFJdinDQyVABBW82565503;     oOFJdinDQyVABBW82565503 = oOFJdinDQyVABBW52693311;     oOFJdinDQyVABBW52693311 = oOFJdinDQyVABBW90099176;     oOFJdinDQyVABBW90099176 = oOFJdinDQyVABBW35787071;     oOFJdinDQyVABBW35787071 = oOFJdinDQyVABBW65816518;     oOFJdinDQyVABBW65816518 = oOFJdinDQyVABBW62892709;     oOFJdinDQyVABBW62892709 = oOFJdinDQyVABBW4051507;     oOFJdinDQyVABBW4051507 = oOFJdinDQyVABBW12789826;     oOFJdinDQyVABBW12789826 = oOFJdinDQyVABBW24933822;     oOFJdinDQyVABBW24933822 = oOFJdinDQyVABBW94797117;     oOFJdinDQyVABBW94797117 = oOFJdinDQyVABBW28203788;     oOFJdinDQyVABBW28203788 = oOFJdinDQyVABBW50414970;     oOFJdinDQyVABBW50414970 = oOFJdinDQyVABBW73132522;     oOFJdinDQyVABBW73132522 = oOFJdinDQyVABBW81891261;     oOFJdinDQyVABBW81891261 = oOFJdinDQyVABBW22276264;     oOFJdinDQyVABBW22276264 = oOFJdinDQyVABBW18246573;     oOFJdinDQyVABBW18246573 = oOFJdinDQyVABBW54720432;     oOFJdinDQyVABBW54720432 = oOFJdinDQyVABBW56597176;     oOFJdinDQyVABBW56597176 = oOFJdinDQyVABBW72864430;     oOFJdinDQyVABBW72864430 = oOFJdinDQyVABBW18846900;     oOFJdinDQyVABBW18846900 = oOFJdinDQyVABBW77617869;     oOFJdinDQyVABBW77617869 = oOFJdinDQyVABBW99077163;     oOFJdinDQyVABBW99077163 = oOFJdinDQyVABBW83799750;     oOFJdinDQyVABBW83799750 = oOFJdinDQyVABBW70767085;     oOFJdinDQyVABBW70767085 = oOFJdinDQyVABBW78280239;     oOFJdinDQyVABBW78280239 = oOFJdinDQyVABBW27947206;     oOFJdinDQyVABBW27947206 = oOFJdinDQyVABBW39589957;     oOFJdinDQyVABBW39589957 = oOFJdinDQyVABBW80398327;     oOFJdinDQyVABBW80398327 = oOFJdinDQyVABBW31889030;     oOFJdinDQyVABBW31889030 = oOFJdinDQyVABBW72438725;     oOFJdinDQyVABBW72438725 = oOFJdinDQyVABBW7503137;     oOFJdinDQyVABBW7503137 = oOFJdinDQyVABBW74089904;     oOFJdinDQyVABBW74089904 = oOFJdinDQyVABBW31576167;     oOFJdinDQyVABBW31576167 = oOFJdinDQyVABBW97373770;     oOFJdinDQyVABBW97373770 = oOFJdinDQyVABBW74556024;     oOFJdinDQyVABBW74556024 = oOFJdinDQyVABBW26206974;     oOFJdinDQyVABBW26206974 = oOFJdinDQyVABBW40606669;     oOFJdinDQyVABBW40606669 = oOFJdinDQyVABBW22161215;     oOFJdinDQyVABBW22161215 = oOFJdinDQyVABBW68503699;     oOFJdinDQyVABBW68503699 = oOFJdinDQyVABBW23153304;     oOFJdinDQyVABBW23153304 = oOFJdinDQyVABBW67868848;     oOFJdinDQyVABBW67868848 = oOFJdinDQyVABBW92206455;     oOFJdinDQyVABBW92206455 = oOFJdinDQyVABBW9822437;     oOFJdinDQyVABBW9822437 = oOFJdinDQyVABBW58867184;     oOFJdinDQyVABBW58867184 = oOFJdinDQyVABBW59169027;     oOFJdinDQyVABBW59169027 = oOFJdinDQyVABBW39211722;     oOFJdinDQyVABBW39211722 = oOFJdinDQyVABBW99195178;     oOFJdinDQyVABBW99195178 = oOFJdinDQyVABBW57880052;     oOFJdinDQyVABBW57880052 = oOFJdinDQyVABBW22144186;     oOFJdinDQyVABBW22144186 = oOFJdinDQyVABBW25064;     oOFJdinDQyVABBW25064 = oOFJdinDQyVABBW29319216;     oOFJdinDQyVABBW29319216 = oOFJdinDQyVABBW24615068;     oOFJdinDQyVABBW24615068 = oOFJdinDQyVABBW62432995;     oOFJdinDQyVABBW62432995 = oOFJdinDQyVABBW35760498;     oOFJdinDQyVABBW35760498 = oOFJdinDQyVABBW84531359;     oOFJdinDQyVABBW84531359 = oOFJdinDQyVABBW83488339;     oOFJdinDQyVABBW83488339 = oOFJdinDQyVABBW68893560;     oOFJdinDQyVABBW68893560 = oOFJdinDQyVABBW19332091;     oOFJdinDQyVABBW19332091 = oOFJdinDQyVABBW57506831;     oOFJdinDQyVABBW57506831 = oOFJdinDQyVABBW37869312;     oOFJdinDQyVABBW37869312 = oOFJdinDQyVABBW23302752;     oOFJdinDQyVABBW23302752 = oOFJdinDQyVABBW23653180;     oOFJdinDQyVABBW23653180 = oOFJdinDQyVABBW80900795;     oOFJdinDQyVABBW80900795 = oOFJdinDQyVABBW52495097;     oOFJdinDQyVABBW52495097 = oOFJdinDQyVABBW87293981;     oOFJdinDQyVABBW87293981 = oOFJdinDQyVABBW54113884;     oOFJdinDQyVABBW54113884 = oOFJdinDQyVABBW18838804;     oOFJdinDQyVABBW18838804 = oOFJdinDQyVABBW75758751;     oOFJdinDQyVABBW75758751 = oOFJdinDQyVABBW7335238;     oOFJdinDQyVABBW7335238 = oOFJdinDQyVABBW67906932;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void YsERvDXrUEbbmCxK12556114() {     int SqhHNTPyPPfBzaK38558599 = -561386942;    int SqhHNTPyPPfBzaK68948425 = -17604352;    int SqhHNTPyPPfBzaK59330190 = 15856937;    int SqhHNTPyPPfBzaK64750063 = -221110151;    int SqhHNTPyPPfBzaK71610988 = -236583939;    int SqhHNTPyPPfBzaK46478136 = -140822590;    int SqhHNTPyPPfBzaK7094276 = -59250165;    int SqhHNTPyPPfBzaK66229710 = -365015081;    int SqhHNTPyPPfBzaK32048618 = -819089102;    int SqhHNTPyPPfBzaK82489459 = -73516714;    int SqhHNTPyPPfBzaK99370687 = -605148238;    int SqhHNTPyPPfBzaK24446856 = -999491484;    int SqhHNTPyPPfBzaK10533233 = -10728106;    int SqhHNTPyPPfBzaK41795319 = -160232648;    int SqhHNTPyPPfBzaK17040237 = -799246352;    int SqhHNTPyPPfBzaK18574652 = -95668993;    int SqhHNTPyPPfBzaK65773182 = 2583623;    int SqhHNTPyPPfBzaK25289776 = -860940188;    int SqhHNTPyPPfBzaK16464040 = -653215592;    int SqhHNTPyPPfBzaK12451011 = -301099786;    int SqhHNTPyPPfBzaK42508570 = 93065393;    int SqhHNTPyPPfBzaK51022457 = -700837757;    int SqhHNTPyPPfBzaK74271509 = -233822790;    int SqhHNTPyPPfBzaK73620850 = -981202692;    int SqhHNTPyPPfBzaK8544838 = -338926418;    int SqhHNTPyPPfBzaK48869216 = 71618;    int SqhHNTPyPPfBzaK92300434 = -908805419;    int SqhHNTPyPPfBzaK88994469 = -280761503;    int SqhHNTPyPPfBzaK67439913 = -663757491;    int SqhHNTPyPPfBzaK93779493 = -771089838;    int SqhHNTPyPPfBzaK69221225 = -692827109;    int SqhHNTPyPPfBzaK27076964 = -4195063;    int SqhHNTPyPPfBzaK35638672 = -689160910;    int SqhHNTPyPPfBzaK55464151 = -7536723;    int SqhHNTPyPPfBzaK13811308 = -560122930;    int SqhHNTPyPPfBzaK25809777 = -832989810;    int SqhHNTPyPPfBzaK6684195 = -124233227;    int SqhHNTPyPPfBzaK56843995 = -382375598;    int SqhHNTPyPPfBzaK91323114 = -815788110;    int SqhHNTPyPPfBzaK20037120 = -685001446;    int SqhHNTPyPPfBzaK54870401 = -293699469;    int SqhHNTPyPPfBzaK56686779 = -832819351;    int SqhHNTPyPPfBzaK99381930 = -503183116;    int SqhHNTPyPPfBzaK79640822 = -986900606;    int SqhHNTPyPPfBzaK77845320 = -5824943;    int SqhHNTPyPPfBzaK28682099 = -658047041;    int SqhHNTPyPPfBzaK44377776 = -658548483;    int SqhHNTPyPPfBzaK20334643 = -732740885;    int SqhHNTPyPPfBzaK62038628 = -446358874;    int SqhHNTPyPPfBzaK81291917 = -79877404;    int SqhHNTPyPPfBzaK87036223 = -498107084;    int SqhHNTPyPPfBzaK19489374 = -502193006;    int SqhHNTPyPPfBzaK54152821 = -674543551;    int SqhHNTPyPPfBzaK74027054 = -968801750;    int SqhHNTPyPPfBzaK70047544 = -931997836;    int SqhHNTPyPPfBzaK87536141 = -860549185;    int SqhHNTPyPPfBzaK94676915 = -783781562;    int SqhHNTPyPPfBzaK85709339 = -2940372;    int SqhHNTPyPPfBzaK56205226 = -882183734;    int SqhHNTPyPPfBzaK22741772 = -136655557;    int SqhHNTPyPPfBzaK54177702 = -232017171;    int SqhHNTPyPPfBzaK18099807 = -778488663;    int SqhHNTPyPPfBzaK98789796 = -701257590;    int SqhHNTPyPPfBzaK38269125 = 52000736;    int SqhHNTPyPPfBzaK13268235 = -380689605;    int SqhHNTPyPPfBzaK72293724 = -500953175;    int SqhHNTPyPPfBzaK88808184 = -210330574;    int SqhHNTPyPPfBzaK55069081 = 96808616;    int SqhHNTPyPPfBzaK27984012 = -600109718;    int SqhHNTPyPPfBzaK91230459 = -966256542;    int SqhHNTPyPPfBzaK11890458 = -971435767;    int SqhHNTPyPPfBzaK8929187 = -615040779;    int SqhHNTPyPPfBzaK33966661 = 54847921;    int SqhHNTPyPPfBzaK96426919 = -968214147;    int SqhHNTPyPPfBzaK57580609 = 92599683;    int SqhHNTPyPPfBzaK85821790 = -74115257;    int SqhHNTPyPPfBzaK51640527 = -97654642;    int SqhHNTPyPPfBzaK94630686 = -246922184;    int SqhHNTPyPPfBzaK95775530 = -875377750;    int SqhHNTPyPPfBzaK79862739 = -680879377;    int SqhHNTPyPPfBzaK4491440 = -341379900;    int SqhHNTPyPPfBzaK71965792 = -76064535;    int SqhHNTPyPPfBzaK26955841 = -834402629;    int SqhHNTPyPPfBzaK86147995 = -483880088;    int SqhHNTPyPPfBzaK6743270 = -172982755;    int SqhHNTPyPPfBzaK49731852 = -90634103;    int SqhHNTPyPPfBzaK72924142 = -329651513;    int SqhHNTPyPPfBzaK61611617 = -720359161;    int SqhHNTPyPPfBzaK85416607 = -75538887;    int SqhHNTPyPPfBzaK26275166 = -699573745;    int SqhHNTPyPPfBzaK31132862 = 50791751;    int SqhHNTPyPPfBzaK20974856 = -21292856;    int SqhHNTPyPPfBzaK638770 = -500191865;    int SqhHNTPyPPfBzaK68581343 = -579132553;    int SqhHNTPyPPfBzaK65859418 = -352984275;    int SqhHNTPyPPfBzaK36770595 = -515210806;    int SqhHNTPyPPfBzaK57896982 = -31561761;    int SqhHNTPyPPfBzaK61112805 = -455183852;    int SqhHNTPyPPfBzaK66372588 = -506211001;    int SqhHNTPyPPfBzaK5551597 = -561386942;     SqhHNTPyPPfBzaK38558599 = SqhHNTPyPPfBzaK68948425;     SqhHNTPyPPfBzaK68948425 = SqhHNTPyPPfBzaK59330190;     SqhHNTPyPPfBzaK59330190 = SqhHNTPyPPfBzaK64750063;     SqhHNTPyPPfBzaK64750063 = SqhHNTPyPPfBzaK71610988;     SqhHNTPyPPfBzaK71610988 = SqhHNTPyPPfBzaK46478136;     SqhHNTPyPPfBzaK46478136 = SqhHNTPyPPfBzaK7094276;     SqhHNTPyPPfBzaK7094276 = SqhHNTPyPPfBzaK66229710;     SqhHNTPyPPfBzaK66229710 = SqhHNTPyPPfBzaK32048618;     SqhHNTPyPPfBzaK32048618 = SqhHNTPyPPfBzaK82489459;     SqhHNTPyPPfBzaK82489459 = SqhHNTPyPPfBzaK99370687;     SqhHNTPyPPfBzaK99370687 = SqhHNTPyPPfBzaK24446856;     SqhHNTPyPPfBzaK24446856 = SqhHNTPyPPfBzaK10533233;     SqhHNTPyPPfBzaK10533233 = SqhHNTPyPPfBzaK41795319;     SqhHNTPyPPfBzaK41795319 = SqhHNTPyPPfBzaK17040237;     SqhHNTPyPPfBzaK17040237 = SqhHNTPyPPfBzaK18574652;     SqhHNTPyPPfBzaK18574652 = SqhHNTPyPPfBzaK65773182;     SqhHNTPyPPfBzaK65773182 = SqhHNTPyPPfBzaK25289776;     SqhHNTPyPPfBzaK25289776 = SqhHNTPyPPfBzaK16464040;     SqhHNTPyPPfBzaK16464040 = SqhHNTPyPPfBzaK12451011;     SqhHNTPyPPfBzaK12451011 = SqhHNTPyPPfBzaK42508570;     SqhHNTPyPPfBzaK42508570 = SqhHNTPyPPfBzaK51022457;     SqhHNTPyPPfBzaK51022457 = SqhHNTPyPPfBzaK74271509;     SqhHNTPyPPfBzaK74271509 = SqhHNTPyPPfBzaK73620850;     SqhHNTPyPPfBzaK73620850 = SqhHNTPyPPfBzaK8544838;     SqhHNTPyPPfBzaK8544838 = SqhHNTPyPPfBzaK48869216;     SqhHNTPyPPfBzaK48869216 = SqhHNTPyPPfBzaK92300434;     SqhHNTPyPPfBzaK92300434 = SqhHNTPyPPfBzaK88994469;     SqhHNTPyPPfBzaK88994469 = SqhHNTPyPPfBzaK67439913;     SqhHNTPyPPfBzaK67439913 = SqhHNTPyPPfBzaK93779493;     SqhHNTPyPPfBzaK93779493 = SqhHNTPyPPfBzaK69221225;     SqhHNTPyPPfBzaK69221225 = SqhHNTPyPPfBzaK27076964;     SqhHNTPyPPfBzaK27076964 = SqhHNTPyPPfBzaK35638672;     SqhHNTPyPPfBzaK35638672 = SqhHNTPyPPfBzaK55464151;     SqhHNTPyPPfBzaK55464151 = SqhHNTPyPPfBzaK13811308;     SqhHNTPyPPfBzaK13811308 = SqhHNTPyPPfBzaK25809777;     SqhHNTPyPPfBzaK25809777 = SqhHNTPyPPfBzaK6684195;     SqhHNTPyPPfBzaK6684195 = SqhHNTPyPPfBzaK56843995;     SqhHNTPyPPfBzaK56843995 = SqhHNTPyPPfBzaK91323114;     SqhHNTPyPPfBzaK91323114 = SqhHNTPyPPfBzaK20037120;     SqhHNTPyPPfBzaK20037120 = SqhHNTPyPPfBzaK54870401;     SqhHNTPyPPfBzaK54870401 = SqhHNTPyPPfBzaK56686779;     SqhHNTPyPPfBzaK56686779 = SqhHNTPyPPfBzaK99381930;     SqhHNTPyPPfBzaK99381930 = SqhHNTPyPPfBzaK79640822;     SqhHNTPyPPfBzaK79640822 = SqhHNTPyPPfBzaK77845320;     SqhHNTPyPPfBzaK77845320 = SqhHNTPyPPfBzaK28682099;     SqhHNTPyPPfBzaK28682099 = SqhHNTPyPPfBzaK44377776;     SqhHNTPyPPfBzaK44377776 = SqhHNTPyPPfBzaK20334643;     SqhHNTPyPPfBzaK20334643 = SqhHNTPyPPfBzaK62038628;     SqhHNTPyPPfBzaK62038628 = SqhHNTPyPPfBzaK81291917;     SqhHNTPyPPfBzaK81291917 = SqhHNTPyPPfBzaK87036223;     SqhHNTPyPPfBzaK87036223 = SqhHNTPyPPfBzaK19489374;     SqhHNTPyPPfBzaK19489374 = SqhHNTPyPPfBzaK54152821;     SqhHNTPyPPfBzaK54152821 = SqhHNTPyPPfBzaK74027054;     SqhHNTPyPPfBzaK74027054 = SqhHNTPyPPfBzaK70047544;     SqhHNTPyPPfBzaK70047544 = SqhHNTPyPPfBzaK87536141;     SqhHNTPyPPfBzaK87536141 = SqhHNTPyPPfBzaK94676915;     SqhHNTPyPPfBzaK94676915 = SqhHNTPyPPfBzaK85709339;     SqhHNTPyPPfBzaK85709339 = SqhHNTPyPPfBzaK56205226;     SqhHNTPyPPfBzaK56205226 = SqhHNTPyPPfBzaK22741772;     SqhHNTPyPPfBzaK22741772 = SqhHNTPyPPfBzaK54177702;     SqhHNTPyPPfBzaK54177702 = SqhHNTPyPPfBzaK18099807;     SqhHNTPyPPfBzaK18099807 = SqhHNTPyPPfBzaK98789796;     SqhHNTPyPPfBzaK98789796 = SqhHNTPyPPfBzaK38269125;     SqhHNTPyPPfBzaK38269125 = SqhHNTPyPPfBzaK13268235;     SqhHNTPyPPfBzaK13268235 = SqhHNTPyPPfBzaK72293724;     SqhHNTPyPPfBzaK72293724 = SqhHNTPyPPfBzaK88808184;     SqhHNTPyPPfBzaK88808184 = SqhHNTPyPPfBzaK55069081;     SqhHNTPyPPfBzaK55069081 = SqhHNTPyPPfBzaK27984012;     SqhHNTPyPPfBzaK27984012 = SqhHNTPyPPfBzaK91230459;     SqhHNTPyPPfBzaK91230459 = SqhHNTPyPPfBzaK11890458;     SqhHNTPyPPfBzaK11890458 = SqhHNTPyPPfBzaK8929187;     SqhHNTPyPPfBzaK8929187 = SqhHNTPyPPfBzaK33966661;     SqhHNTPyPPfBzaK33966661 = SqhHNTPyPPfBzaK96426919;     SqhHNTPyPPfBzaK96426919 = SqhHNTPyPPfBzaK57580609;     SqhHNTPyPPfBzaK57580609 = SqhHNTPyPPfBzaK85821790;     SqhHNTPyPPfBzaK85821790 = SqhHNTPyPPfBzaK51640527;     SqhHNTPyPPfBzaK51640527 = SqhHNTPyPPfBzaK94630686;     SqhHNTPyPPfBzaK94630686 = SqhHNTPyPPfBzaK95775530;     SqhHNTPyPPfBzaK95775530 = SqhHNTPyPPfBzaK79862739;     SqhHNTPyPPfBzaK79862739 = SqhHNTPyPPfBzaK4491440;     SqhHNTPyPPfBzaK4491440 = SqhHNTPyPPfBzaK71965792;     SqhHNTPyPPfBzaK71965792 = SqhHNTPyPPfBzaK26955841;     SqhHNTPyPPfBzaK26955841 = SqhHNTPyPPfBzaK86147995;     SqhHNTPyPPfBzaK86147995 = SqhHNTPyPPfBzaK6743270;     SqhHNTPyPPfBzaK6743270 = SqhHNTPyPPfBzaK49731852;     SqhHNTPyPPfBzaK49731852 = SqhHNTPyPPfBzaK72924142;     SqhHNTPyPPfBzaK72924142 = SqhHNTPyPPfBzaK61611617;     SqhHNTPyPPfBzaK61611617 = SqhHNTPyPPfBzaK85416607;     SqhHNTPyPPfBzaK85416607 = SqhHNTPyPPfBzaK26275166;     SqhHNTPyPPfBzaK26275166 = SqhHNTPyPPfBzaK31132862;     SqhHNTPyPPfBzaK31132862 = SqhHNTPyPPfBzaK20974856;     SqhHNTPyPPfBzaK20974856 = SqhHNTPyPPfBzaK638770;     SqhHNTPyPPfBzaK638770 = SqhHNTPyPPfBzaK68581343;     SqhHNTPyPPfBzaK68581343 = SqhHNTPyPPfBzaK65859418;     SqhHNTPyPPfBzaK65859418 = SqhHNTPyPPfBzaK36770595;     SqhHNTPyPPfBzaK36770595 = SqhHNTPyPPfBzaK57896982;     SqhHNTPyPPfBzaK57896982 = SqhHNTPyPPfBzaK61112805;     SqhHNTPyPPfBzaK61112805 = SqhHNTPyPPfBzaK66372588;     SqhHNTPyPPfBzaK66372588 = SqhHNTPyPPfBzaK5551597;     SqhHNTPyPPfBzaK5551597 = SqhHNTPyPPfBzaK38558599;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WvuhxJMAJsZoOEua19214859() {     int ygGqUTxlRBAttJN38552128 = -433911085;    int ygGqUTxlRBAttJN7520742 = -60199483;    int ygGqUTxlRBAttJN60643402 = 58728323;    int ygGqUTxlRBAttJN49829924 = -41650978;    int ygGqUTxlRBAttJN80451090 = -704939268;    int ygGqUTxlRBAttJN81283568 = -940255402;    int ygGqUTxlRBAttJN75641612 = -863786632;    int ygGqUTxlRBAttJN35443761 = -69192413;    int ygGqUTxlRBAttJN14788783 = -532735173;    int ygGqUTxlRBAttJN79128973 = -810323585;    int ygGqUTxlRBAttJN19691293 = -564620311;    int ygGqUTxlRBAttJN38738533 = -734969856;    int ygGqUTxlRBAttJN42386146 = -32386581;    int ygGqUTxlRBAttJN95639539 = -505385700;    int ygGqUTxlRBAttJN62212346 = -761959222;    int ygGqUTxlRBAttJN93139199 = -428464644;    int ygGqUTxlRBAttJN21625942 = -917030946;    int ygGqUTxlRBAttJN48290309 = -158011814;    int ygGqUTxlRBAttJN36892310 = -171197857;    int ygGqUTxlRBAttJN84524774 = -913124237;    int ygGqUTxlRBAttJN22177994 = -717400986;    int ygGqUTxlRBAttJN29318723 = -110829640;    int ygGqUTxlRBAttJN11149907 = -692780632;    int ygGqUTxlRBAttJN17270959 = -862123215;    int ygGqUTxlRBAttJN36987527 = -765620283;    int ygGqUTxlRBAttJN65443294 = 91865693;    int ygGqUTxlRBAttJN62522016 = -109605132;    int ygGqUTxlRBAttJN24397994 = -889728497;    int ygGqUTxlRBAttJN80118360 = -209827791;    int ygGqUTxlRBAttJN69869363 = -446521212;    int ygGqUTxlRBAttJN2633904 = 20079656;    int ygGqUTxlRBAttJN11800822 = -396016822;    int ygGqUTxlRBAttJN21091095 = -891105123;    int ygGqUTxlRBAttJN55746989 = -984218455;    int ygGqUTxlRBAttJN96169345 = -819436852;    int ygGqUTxlRBAttJN68637967 = -912755538;    int ygGqUTxlRBAttJN18625563 = -223886605;    int ygGqUTxlRBAttJN26761675 = -377638281;    int ygGqUTxlRBAttJN26636712 = -428469513;    int ygGqUTxlRBAttJN91910012 = -294433132;    int ygGqUTxlRBAttJN42726069 = -381643367;    int ygGqUTxlRBAttJN42384440 = 73388588;    int ygGqUTxlRBAttJN59665290 = 37686632;    int ygGqUTxlRBAttJN38872721 = -543865050;    int ygGqUTxlRBAttJN61863372 = -479084344;    int ygGqUTxlRBAttJN69110067 = -470091693;    int ygGqUTxlRBAttJN72075568 = -39829105;    int ygGqUTxlRBAttJN95718223 = -241502240;    int ygGqUTxlRBAttJN80697017 = -10122711;    int ygGqUTxlRBAttJN1337341 = -766902295;    int ygGqUTxlRBAttJN295594 = -76662091;    int ygGqUTxlRBAttJN34036631 = -884338750;    int ygGqUTxlRBAttJN29601169 = -566597562;    int ygGqUTxlRBAttJN84277325 = -400721601;    int ygGqUTxlRBAttJN97036864 = -688897108;    int ygGqUTxlRBAttJN9233405 = -223081446;    int ygGqUTxlRBAttJN96370834 = -367418851;    int ygGqUTxlRBAttJN43372443 = -79148463;    int ygGqUTxlRBAttJN12842397 = -276030695;    int ygGqUTxlRBAttJN15007797 = -696804962;    int ygGqUTxlRBAttJN18761553 = -730650270;    int ygGqUTxlRBAttJN51243618 = -974058135;    int ygGqUTxlRBAttJN55325400 = -859364623;    int ygGqUTxlRBAttJN44919419 = 13786039;    int ygGqUTxlRBAttJN76495069 = -730403242;    int ygGqUTxlRBAttJN7890472 = -68603490;    int ygGqUTxlRBAttJN17647439 = -843864733;    int ygGqUTxlRBAttJN86639157 = -48168126;    int ygGqUTxlRBAttJN99470193 = -685948849;    int ygGqUTxlRBAttJN93574379 = -849203685;    int ygGqUTxlRBAttJN74513637 = -104578039;    int ygGqUTxlRBAttJN94864266 = -439392666;    int ygGqUTxlRBAttJN21653598 = -729542301;    int ygGqUTxlRBAttJN44982297 = -876764726;    int ygGqUTxlRBAttJN41798705 = -431480870;    int ygGqUTxlRBAttJN79793554 = -690789574;    int ygGqUTxlRBAttJN69653433 = -48516273;    int ygGqUTxlRBAttJN72277185 = -48915583;    int ygGqUTxlRBAttJN55407587 = -283038872;    int ygGqUTxlRBAttJN67877460 = -195528590;    int ygGqUTxlRBAttJN93367725 = -868305202;    int ygGqUTxlRBAttJN66803792 = -868102892;    int ygGqUTxlRBAttJN43700977 = -779605787;    int ygGqUTxlRBAttJN78781020 = -442925496;    int ygGqUTxlRBAttJN69573770 = -269859121;    int ygGqUTxlRBAttJN68597273 = -95581594;    int ygGqUTxlRBAttJN82199652 = -829419260;    int ygGqUTxlRBAttJN36813769 = -390383523;    int ygGqUTxlRBAttJN58710124 = -195321347;    int ygGqUTxlRBAttJN86935940 = -496355406;    int ygGqUTxlRBAttJN72267132 = -445336687;    int ygGqUTxlRBAttJN75253119 = -44738143;    int ygGqUTxlRBAttJN13919279 = -1607586;    int ygGqUTxlRBAttJN11628916 = -731664552;    int ygGqUTxlRBAttJN73148460 = -563782862;    int ygGqUTxlRBAttJN91482451 = -407585232;    int ygGqUTxlRBAttJN87059039 = -67246790;    int ygGqUTxlRBAttJN14745871 = -976099407;    int ygGqUTxlRBAttJN62377651 = -813461808;    int ygGqUTxlRBAttJN53972901 = -433911085;     ygGqUTxlRBAttJN38552128 = ygGqUTxlRBAttJN7520742;     ygGqUTxlRBAttJN7520742 = ygGqUTxlRBAttJN60643402;     ygGqUTxlRBAttJN60643402 = ygGqUTxlRBAttJN49829924;     ygGqUTxlRBAttJN49829924 = ygGqUTxlRBAttJN80451090;     ygGqUTxlRBAttJN80451090 = ygGqUTxlRBAttJN81283568;     ygGqUTxlRBAttJN81283568 = ygGqUTxlRBAttJN75641612;     ygGqUTxlRBAttJN75641612 = ygGqUTxlRBAttJN35443761;     ygGqUTxlRBAttJN35443761 = ygGqUTxlRBAttJN14788783;     ygGqUTxlRBAttJN14788783 = ygGqUTxlRBAttJN79128973;     ygGqUTxlRBAttJN79128973 = ygGqUTxlRBAttJN19691293;     ygGqUTxlRBAttJN19691293 = ygGqUTxlRBAttJN38738533;     ygGqUTxlRBAttJN38738533 = ygGqUTxlRBAttJN42386146;     ygGqUTxlRBAttJN42386146 = ygGqUTxlRBAttJN95639539;     ygGqUTxlRBAttJN95639539 = ygGqUTxlRBAttJN62212346;     ygGqUTxlRBAttJN62212346 = ygGqUTxlRBAttJN93139199;     ygGqUTxlRBAttJN93139199 = ygGqUTxlRBAttJN21625942;     ygGqUTxlRBAttJN21625942 = ygGqUTxlRBAttJN48290309;     ygGqUTxlRBAttJN48290309 = ygGqUTxlRBAttJN36892310;     ygGqUTxlRBAttJN36892310 = ygGqUTxlRBAttJN84524774;     ygGqUTxlRBAttJN84524774 = ygGqUTxlRBAttJN22177994;     ygGqUTxlRBAttJN22177994 = ygGqUTxlRBAttJN29318723;     ygGqUTxlRBAttJN29318723 = ygGqUTxlRBAttJN11149907;     ygGqUTxlRBAttJN11149907 = ygGqUTxlRBAttJN17270959;     ygGqUTxlRBAttJN17270959 = ygGqUTxlRBAttJN36987527;     ygGqUTxlRBAttJN36987527 = ygGqUTxlRBAttJN65443294;     ygGqUTxlRBAttJN65443294 = ygGqUTxlRBAttJN62522016;     ygGqUTxlRBAttJN62522016 = ygGqUTxlRBAttJN24397994;     ygGqUTxlRBAttJN24397994 = ygGqUTxlRBAttJN80118360;     ygGqUTxlRBAttJN80118360 = ygGqUTxlRBAttJN69869363;     ygGqUTxlRBAttJN69869363 = ygGqUTxlRBAttJN2633904;     ygGqUTxlRBAttJN2633904 = ygGqUTxlRBAttJN11800822;     ygGqUTxlRBAttJN11800822 = ygGqUTxlRBAttJN21091095;     ygGqUTxlRBAttJN21091095 = ygGqUTxlRBAttJN55746989;     ygGqUTxlRBAttJN55746989 = ygGqUTxlRBAttJN96169345;     ygGqUTxlRBAttJN96169345 = ygGqUTxlRBAttJN68637967;     ygGqUTxlRBAttJN68637967 = ygGqUTxlRBAttJN18625563;     ygGqUTxlRBAttJN18625563 = ygGqUTxlRBAttJN26761675;     ygGqUTxlRBAttJN26761675 = ygGqUTxlRBAttJN26636712;     ygGqUTxlRBAttJN26636712 = ygGqUTxlRBAttJN91910012;     ygGqUTxlRBAttJN91910012 = ygGqUTxlRBAttJN42726069;     ygGqUTxlRBAttJN42726069 = ygGqUTxlRBAttJN42384440;     ygGqUTxlRBAttJN42384440 = ygGqUTxlRBAttJN59665290;     ygGqUTxlRBAttJN59665290 = ygGqUTxlRBAttJN38872721;     ygGqUTxlRBAttJN38872721 = ygGqUTxlRBAttJN61863372;     ygGqUTxlRBAttJN61863372 = ygGqUTxlRBAttJN69110067;     ygGqUTxlRBAttJN69110067 = ygGqUTxlRBAttJN72075568;     ygGqUTxlRBAttJN72075568 = ygGqUTxlRBAttJN95718223;     ygGqUTxlRBAttJN95718223 = ygGqUTxlRBAttJN80697017;     ygGqUTxlRBAttJN80697017 = ygGqUTxlRBAttJN1337341;     ygGqUTxlRBAttJN1337341 = ygGqUTxlRBAttJN295594;     ygGqUTxlRBAttJN295594 = ygGqUTxlRBAttJN34036631;     ygGqUTxlRBAttJN34036631 = ygGqUTxlRBAttJN29601169;     ygGqUTxlRBAttJN29601169 = ygGqUTxlRBAttJN84277325;     ygGqUTxlRBAttJN84277325 = ygGqUTxlRBAttJN97036864;     ygGqUTxlRBAttJN97036864 = ygGqUTxlRBAttJN9233405;     ygGqUTxlRBAttJN9233405 = ygGqUTxlRBAttJN96370834;     ygGqUTxlRBAttJN96370834 = ygGqUTxlRBAttJN43372443;     ygGqUTxlRBAttJN43372443 = ygGqUTxlRBAttJN12842397;     ygGqUTxlRBAttJN12842397 = ygGqUTxlRBAttJN15007797;     ygGqUTxlRBAttJN15007797 = ygGqUTxlRBAttJN18761553;     ygGqUTxlRBAttJN18761553 = ygGqUTxlRBAttJN51243618;     ygGqUTxlRBAttJN51243618 = ygGqUTxlRBAttJN55325400;     ygGqUTxlRBAttJN55325400 = ygGqUTxlRBAttJN44919419;     ygGqUTxlRBAttJN44919419 = ygGqUTxlRBAttJN76495069;     ygGqUTxlRBAttJN76495069 = ygGqUTxlRBAttJN7890472;     ygGqUTxlRBAttJN7890472 = ygGqUTxlRBAttJN17647439;     ygGqUTxlRBAttJN17647439 = ygGqUTxlRBAttJN86639157;     ygGqUTxlRBAttJN86639157 = ygGqUTxlRBAttJN99470193;     ygGqUTxlRBAttJN99470193 = ygGqUTxlRBAttJN93574379;     ygGqUTxlRBAttJN93574379 = ygGqUTxlRBAttJN74513637;     ygGqUTxlRBAttJN74513637 = ygGqUTxlRBAttJN94864266;     ygGqUTxlRBAttJN94864266 = ygGqUTxlRBAttJN21653598;     ygGqUTxlRBAttJN21653598 = ygGqUTxlRBAttJN44982297;     ygGqUTxlRBAttJN44982297 = ygGqUTxlRBAttJN41798705;     ygGqUTxlRBAttJN41798705 = ygGqUTxlRBAttJN79793554;     ygGqUTxlRBAttJN79793554 = ygGqUTxlRBAttJN69653433;     ygGqUTxlRBAttJN69653433 = ygGqUTxlRBAttJN72277185;     ygGqUTxlRBAttJN72277185 = ygGqUTxlRBAttJN55407587;     ygGqUTxlRBAttJN55407587 = ygGqUTxlRBAttJN67877460;     ygGqUTxlRBAttJN67877460 = ygGqUTxlRBAttJN93367725;     ygGqUTxlRBAttJN93367725 = ygGqUTxlRBAttJN66803792;     ygGqUTxlRBAttJN66803792 = ygGqUTxlRBAttJN43700977;     ygGqUTxlRBAttJN43700977 = ygGqUTxlRBAttJN78781020;     ygGqUTxlRBAttJN78781020 = ygGqUTxlRBAttJN69573770;     ygGqUTxlRBAttJN69573770 = ygGqUTxlRBAttJN68597273;     ygGqUTxlRBAttJN68597273 = ygGqUTxlRBAttJN82199652;     ygGqUTxlRBAttJN82199652 = ygGqUTxlRBAttJN36813769;     ygGqUTxlRBAttJN36813769 = ygGqUTxlRBAttJN58710124;     ygGqUTxlRBAttJN58710124 = ygGqUTxlRBAttJN86935940;     ygGqUTxlRBAttJN86935940 = ygGqUTxlRBAttJN72267132;     ygGqUTxlRBAttJN72267132 = ygGqUTxlRBAttJN75253119;     ygGqUTxlRBAttJN75253119 = ygGqUTxlRBAttJN13919279;     ygGqUTxlRBAttJN13919279 = ygGqUTxlRBAttJN11628916;     ygGqUTxlRBAttJN11628916 = ygGqUTxlRBAttJN73148460;     ygGqUTxlRBAttJN73148460 = ygGqUTxlRBAttJN91482451;     ygGqUTxlRBAttJN91482451 = ygGqUTxlRBAttJN87059039;     ygGqUTxlRBAttJN87059039 = ygGqUTxlRBAttJN14745871;     ygGqUTxlRBAttJN14745871 = ygGqUTxlRBAttJN62377651;     ygGqUTxlRBAttJN62377651 = ygGqUTxlRBAttJN53972901;     ygGqUTxlRBAttJN53972901 = ygGqUTxlRBAttJN38552128;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void cCsHkJPhwBctvcLY3947500() {     int xynHDCHgSTLqUsY76936514 = -248936595;    int xynHDCHgSTLqUsY13544140 = -400821023;    int xynHDCHgSTLqUsY8648954 = -237835368;    int xynHDCHgSTLqUsY33332958 = -232094488;    int xynHDCHgSTLqUsY25623995 = -306967378;    int xynHDCHgSTLqUsY71992050 = -373294095;    int xynHDCHgSTLqUsY65075543 = -382656976;    int xynHDCHgSTLqUsY38744896 = -386037915;    int xynHDCHgSTLqUsY70042557 = -463425170;    int xynHDCHgSTLqUsY11227034 = -575358999;    int xynHDCHgSTLqUsY20871019 = 29209931;    int xynHDCHgSTLqUsY77423339 = -15054853;    int xynHDCHgSTLqUsY25404399 = -757100654;    int xynHDCHgSTLqUsY29134894 = -122019089;    int xynHDCHgSTLqUsY51443569 = -632065307;    int xynHDCHgSTLqUsY80156216 = -749956209;    int xynHDCHgSTLqUsY65470356 = -629896068;    int xynHDCHgSTLqUsY6963633 = -535356233;    int xynHDCHgSTLqUsY58661121 = -727676377;    int xynHDCHgSTLqUsY4193211 = -262758849;    int xynHDCHgSTLqUsY44022468 = -49484284;    int xynHDCHgSTLqUsY3277118 = -780437151;    int xynHDCHgSTLqUsY79917206 = -849960948;    int xynHDCHgSTLqUsY76191914 = -219476162;    int xynHDCHgSTLqUsY40899547 = -790170643;    int xynHDCHgSTLqUsY43102922 = -446604191;    int xynHDCHgSTLqUsY15882298 = -807525121;    int xynHDCHgSTLqUsY84842669 = -75254858;    int xynHDCHgSTLqUsY79255731 = -468198251;    int xynHDCHgSTLqUsY23763230 = -634454181;    int xynHDCHgSTLqUsY2291177 = -360797655;    int xynHDCHgSTLqUsY82861862 = -964888811;    int xynHDCHgSTLqUsY17419250 = -810892613;    int xynHDCHgSTLqUsY85844758 = -91754180;    int xynHDCHgSTLqUsY15918790 = -512276794;    int xynHDCHgSTLqUsY82997553 = 40618134;    int xynHDCHgSTLqUsY81152273 = -295001144;    int xynHDCHgSTLqUsY20203026 = -155447839;    int xynHDCHgSTLqUsY2708483 = -403295580;    int xynHDCHgSTLqUsY83212630 = -289956924;    int xynHDCHgSTLqUsY75597323 = -56289754;    int xynHDCHgSTLqUsY22701165 = 53622914;    int xynHDCHgSTLqUsY23182260 = 3907314;    int xynHDCHgSTLqUsY35830741 = -39524005;    int xynHDCHgSTLqUsY65816634 = -464356618;    int xynHDCHgSTLqUsY28282582 = -341868329;    int xynHDCHgSTLqUsY94030457 = -707251773;    int xynHDCHgSTLqUsY82503798 = -472515154;    int xynHDCHgSTLqUsY38255608 = -45873190;    int xynHDCHgSTLqUsY62006910 = -951912139;    int xynHDCHgSTLqUsY2240595 = -321094987;    int xynHDCHgSTLqUsY60377243 = -769311771;    int xynHDCHgSTLqUsY31263226 = -426391377;    int xynHDCHgSTLqUsY77375418 = -341397422;    int xynHDCHgSTLqUsY59925031 = -544633457;    int xynHDCHgSTLqUsY73659397 = -468499444;    int xynHDCHgSTLqUsY33626933 = -550860076;    int xynHDCHgSTLqUsY32457039 = 81640794;    int xynHDCHgSTLqUsY92433411 = -441923845;    int xynHDCHgSTLqUsY82521073 = -860363187;    int xynHDCHgSTLqUsY56109753 = -565768974;    int xynHDCHgSTLqUsY80232873 = -207402118;    int xynHDCHgSTLqUsY59489164 = -917839664;    int xynHDCHgSTLqUsY46279328 = -828970990;    int xynHDCHgSTLqUsY8935857 = -114561344;    int xynHDCHgSTLqUsY38009156 = -5901258;    int xynHDCHgSTLqUsY60004090 = -204162241;    int xynHDCHgSTLqUsY39559641 = -565346475;    int xynHDCHgSTLqUsY13216105 = -609742296;    int xynHDCHgSTLqUsY68446016 = -572683442;    int xynHDCHgSTLqUsY99003943 = -354955065;    int xynHDCHgSTLqUsY45267331 = -374448229;    int xynHDCHgSTLqUsY4255150 = -32060654;    int xynHDCHgSTLqUsY75448491 = -337719453;    int xynHDCHgSTLqUsY28595888 = -106469096;    int xynHDCHgSTLqUsY21321304 = -3107198;    int xynHDCHgSTLqUsY80094858 = -684344466;    int xynHDCHgSTLqUsY44086465 = -710436944;    int xynHDCHgSTLqUsY10375281 = -755119544;    int xynHDCHgSTLqUsY12616965 = -348302315;    int xynHDCHgSTLqUsY49072465 = -739352418;    int xynHDCHgSTLqUsY33378499 = -235009968;    int xynHDCHgSTLqUsY46587061 = 70618331;    int xynHDCHgSTLqUsY17248822 = -516286113;    int xynHDCHgSTLqUsY21522635 = -213359194;    int xynHDCHgSTLqUsY41913934 = -591485885;    int xynHDCHgSTLqUsY51598636 = -438497434;    int xynHDCHgSTLqUsY40043832 = -369495191;    int xynHDCHgSTLqUsY25919727 = -547120724;    int xynHDCHgSTLqUsY42259392 = 56222650;    int xynHDCHgSTLqUsY49370620 = -408521791;    int xynHDCHgSTLqUsY48695234 = -276641938;    int xynHDCHgSTLqUsY27769615 = -713523995;    int xynHDCHgSTLqUsY20187410 = -542932393;    int xynHDCHgSTLqUsY27102878 = -724187950;    int xynHDCHgSTLqUsY95364449 = -848887636;    int xynHDCHgSTLqUsY63212000 = -28537423;    int xynHDCHgSTLqUsY76902931 = -167121697;    int xynHDCHgSTLqUsY26894885 = -924962661;    int xynHDCHgSTLqUsY27807478 = -248936595;     xynHDCHgSTLqUsY76936514 = xynHDCHgSTLqUsY13544140;     xynHDCHgSTLqUsY13544140 = xynHDCHgSTLqUsY8648954;     xynHDCHgSTLqUsY8648954 = xynHDCHgSTLqUsY33332958;     xynHDCHgSTLqUsY33332958 = xynHDCHgSTLqUsY25623995;     xynHDCHgSTLqUsY25623995 = xynHDCHgSTLqUsY71992050;     xynHDCHgSTLqUsY71992050 = xynHDCHgSTLqUsY65075543;     xynHDCHgSTLqUsY65075543 = xynHDCHgSTLqUsY38744896;     xynHDCHgSTLqUsY38744896 = xynHDCHgSTLqUsY70042557;     xynHDCHgSTLqUsY70042557 = xynHDCHgSTLqUsY11227034;     xynHDCHgSTLqUsY11227034 = xynHDCHgSTLqUsY20871019;     xynHDCHgSTLqUsY20871019 = xynHDCHgSTLqUsY77423339;     xynHDCHgSTLqUsY77423339 = xynHDCHgSTLqUsY25404399;     xynHDCHgSTLqUsY25404399 = xynHDCHgSTLqUsY29134894;     xynHDCHgSTLqUsY29134894 = xynHDCHgSTLqUsY51443569;     xynHDCHgSTLqUsY51443569 = xynHDCHgSTLqUsY80156216;     xynHDCHgSTLqUsY80156216 = xynHDCHgSTLqUsY65470356;     xynHDCHgSTLqUsY65470356 = xynHDCHgSTLqUsY6963633;     xynHDCHgSTLqUsY6963633 = xynHDCHgSTLqUsY58661121;     xynHDCHgSTLqUsY58661121 = xynHDCHgSTLqUsY4193211;     xynHDCHgSTLqUsY4193211 = xynHDCHgSTLqUsY44022468;     xynHDCHgSTLqUsY44022468 = xynHDCHgSTLqUsY3277118;     xynHDCHgSTLqUsY3277118 = xynHDCHgSTLqUsY79917206;     xynHDCHgSTLqUsY79917206 = xynHDCHgSTLqUsY76191914;     xynHDCHgSTLqUsY76191914 = xynHDCHgSTLqUsY40899547;     xynHDCHgSTLqUsY40899547 = xynHDCHgSTLqUsY43102922;     xynHDCHgSTLqUsY43102922 = xynHDCHgSTLqUsY15882298;     xynHDCHgSTLqUsY15882298 = xynHDCHgSTLqUsY84842669;     xynHDCHgSTLqUsY84842669 = xynHDCHgSTLqUsY79255731;     xynHDCHgSTLqUsY79255731 = xynHDCHgSTLqUsY23763230;     xynHDCHgSTLqUsY23763230 = xynHDCHgSTLqUsY2291177;     xynHDCHgSTLqUsY2291177 = xynHDCHgSTLqUsY82861862;     xynHDCHgSTLqUsY82861862 = xynHDCHgSTLqUsY17419250;     xynHDCHgSTLqUsY17419250 = xynHDCHgSTLqUsY85844758;     xynHDCHgSTLqUsY85844758 = xynHDCHgSTLqUsY15918790;     xynHDCHgSTLqUsY15918790 = xynHDCHgSTLqUsY82997553;     xynHDCHgSTLqUsY82997553 = xynHDCHgSTLqUsY81152273;     xynHDCHgSTLqUsY81152273 = xynHDCHgSTLqUsY20203026;     xynHDCHgSTLqUsY20203026 = xynHDCHgSTLqUsY2708483;     xynHDCHgSTLqUsY2708483 = xynHDCHgSTLqUsY83212630;     xynHDCHgSTLqUsY83212630 = xynHDCHgSTLqUsY75597323;     xynHDCHgSTLqUsY75597323 = xynHDCHgSTLqUsY22701165;     xynHDCHgSTLqUsY22701165 = xynHDCHgSTLqUsY23182260;     xynHDCHgSTLqUsY23182260 = xynHDCHgSTLqUsY35830741;     xynHDCHgSTLqUsY35830741 = xynHDCHgSTLqUsY65816634;     xynHDCHgSTLqUsY65816634 = xynHDCHgSTLqUsY28282582;     xynHDCHgSTLqUsY28282582 = xynHDCHgSTLqUsY94030457;     xynHDCHgSTLqUsY94030457 = xynHDCHgSTLqUsY82503798;     xynHDCHgSTLqUsY82503798 = xynHDCHgSTLqUsY38255608;     xynHDCHgSTLqUsY38255608 = xynHDCHgSTLqUsY62006910;     xynHDCHgSTLqUsY62006910 = xynHDCHgSTLqUsY2240595;     xynHDCHgSTLqUsY2240595 = xynHDCHgSTLqUsY60377243;     xynHDCHgSTLqUsY60377243 = xynHDCHgSTLqUsY31263226;     xynHDCHgSTLqUsY31263226 = xynHDCHgSTLqUsY77375418;     xynHDCHgSTLqUsY77375418 = xynHDCHgSTLqUsY59925031;     xynHDCHgSTLqUsY59925031 = xynHDCHgSTLqUsY73659397;     xynHDCHgSTLqUsY73659397 = xynHDCHgSTLqUsY33626933;     xynHDCHgSTLqUsY33626933 = xynHDCHgSTLqUsY32457039;     xynHDCHgSTLqUsY32457039 = xynHDCHgSTLqUsY92433411;     xynHDCHgSTLqUsY92433411 = xynHDCHgSTLqUsY82521073;     xynHDCHgSTLqUsY82521073 = xynHDCHgSTLqUsY56109753;     xynHDCHgSTLqUsY56109753 = xynHDCHgSTLqUsY80232873;     xynHDCHgSTLqUsY80232873 = xynHDCHgSTLqUsY59489164;     xynHDCHgSTLqUsY59489164 = xynHDCHgSTLqUsY46279328;     xynHDCHgSTLqUsY46279328 = xynHDCHgSTLqUsY8935857;     xynHDCHgSTLqUsY8935857 = xynHDCHgSTLqUsY38009156;     xynHDCHgSTLqUsY38009156 = xynHDCHgSTLqUsY60004090;     xynHDCHgSTLqUsY60004090 = xynHDCHgSTLqUsY39559641;     xynHDCHgSTLqUsY39559641 = xynHDCHgSTLqUsY13216105;     xynHDCHgSTLqUsY13216105 = xynHDCHgSTLqUsY68446016;     xynHDCHgSTLqUsY68446016 = xynHDCHgSTLqUsY99003943;     xynHDCHgSTLqUsY99003943 = xynHDCHgSTLqUsY45267331;     xynHDCHgSTLqUsY45267331 = xynHDCHgSTLqUsY4255150;     xynHDCHgSTLqUsY4255150 = xynHDCHgSTLqUsY75448491;     xynHDCHgSTLqUsY75448491 = xynHDCHgSTLqUsY28595888;     xynHDCHgSTLqUsY28595888 = xynHDCHgSTLqUsY21321304;     xynHDCHgSTLqUsY21321304 = xynHDCHgSTLqUsY80094858;     xynHDCHgSTLqUsY80094858 = xynHDCHgSTLqUsY44086465;     xynHDCHgSTLqUsY44086465 = xynHDCHgSTLqUsY10375281;     xynHDCHgSTLqUsY10375281 = xynHDCHgSTLqUsY12616965;     xynHDCHgSTLqUsY12616965 = xynHDCHgSTLqUsY49072465;     xynHDCHgSTLqUsY49072465 = xynHDCHgSTLqUsY33378499;     xynHDCHgSTLqUsY33378499 = xynHDCHgSTLqUsY46587061;     xynHDCHgSTLqUsY46587061 = xynHDCHgSTLqUsY17248822;     xynHDCHgSTLqUsY17248822 = xynHDCHgSTLqUsY21522635;     xynHDCHgSTLqUsY21522635 = xynHDCHgSTLqUsY41913934;     xynHDCHgSTLqUsY41913934 = xynHDCHgSTLqUsY51598636;     xynHDCHgSTLqUsY51598636 = xynHDCHgSTLqUsY40043832;     xynHDCHgSTLqUsY40043832 = xynHDCHgSTLqUsY25919727;     xynHDCHgSTLqUsY25919727 = xynHDCHgSTLqUsY42259392;     xynHDCHgSTLqUsY42259392 = xynHDCHgSTLqUsY49370620;     xynHDCHgSTLqUsY49370620 = xynHDCHgSTLqUsY48695234;     xynHDCHgSTLqUsY48695234 = xynHDCHgSTLqUsY27769615;     xynHDCHgSTLqUsY27769615 = xynHDCHgSTLqUsY20187410;     xynHDCHgSTLqUsY20187410 = xynHDCHgSTLqUsY27102878;     xynHDCHgSTLqUsY27102878 = xynHDCHgSTLqUsY95364449;     xynHDCHgSTLqUsY95364449 = xynHDCHgSTLqUsY63212000;     xynHDCHgSTLqUsY63212000 = xynHDCHgSTLqUsY76902931;     xynHDCHgSTLqUsY76902931 = xynHDCHgSTLqUsY26894885;     xynHDCHgSTLqUsY26894885 = xynHDCHgSTLqUsY27807478;     xynHDCHgSTLqUsY27807478 = xynHDCHgSTLqUsY76936514;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void qvbvClyLFjJOokty10606244() {     float uZSapDWTyZgKTYh76930043 = -121460736;    float uZSapDWTyZgKTYh52116456 = -443416148;    float uZSapDWTyZgKTYh9962166 = -194963981;    float uZSapDWTyZgKTYh18412819 = -52635318;    float uZSapDWTyZgKTYh34464097 = -775322699;    float uZSapDWTyZgKTYh6797483 = -72726907;    float uZSapDWTyZgKTYh33622879 = -87193443;    float uZSapDWTyZgKTYh7958947 = -90215232;    float uZSapDWTyZgKTYh52782721 = -177071240;    float uZSapDWTyZgKTYh7866547 = -212165870;    float uZSapDWTyZgKTYh41191624 = 69737859;    float uZSapDWTyZgKTYh91715016 = -850533216;    float uZSapDWTyZgKTYh57257313 = -778759129;    float uZSapDWTyZgKTYh82979114 = -467172141;    float uZSapDWTyZgKTYh96615678 = -594778159;    float uZSapDWTyZgKTYh54720764 = 17248141;    float uZSapDWTyZgKTYh21323116 = -449510638;    float uZSapDWTyZgKTYh29964166 = -932427855;    float uZSapDWTyZgKTYh79089391 = -245658644;    float uZSapDWTyZgKTYh76266974 = -874783300;    float uZSapDWTyZgKTYh23691892 = -859950657;    float uZSapDWTyZgKTYh81573383 = -190429034;    float uZSapDWTyZgKTYh16795603 = -208918790;    float uZSapDWTyZgKTYh19842023 = -100396684;    float uZSapDWTyZgKTYh69342236 = -116864496;    float uZSapDWTyZgKTYh59676999 = -354810119;    float uZSapDWTyZgKTYh86103879 = -8324834;    float uZSapDWTyZgKTYh20246195 = -684221846;    float uZSapDWTyZgKTYh91934178 = -14268566;    float uZSapDWTyZgKTYh99853099 = -309885555;    float uZSapDWTyZgKTYh35703856 = -747890890;    float uZSapDWTyZgKTYh67585720 = -256710572;    float uZSapDWTyZgKTYh2871673 = 87163171;    float uZSapDWTyZgKTYh86127595 = 31564088;    float uZSapDWTyZgKTYh98276827 = -771590719;    float uZSapDWTyZgKTYh25825743 = -39147612;    float uZSapDWTyZgKTYh93093640 = -394654522;    float uZSapDWTyZgKTYh90120705 = -150710518;    float uZSapDWTyZgKTYh38022080 = -15976996;    float uZSapDWTyZgKTYh55085523 = -999388610;    float uZSapDWTyZgKTYh63452990 = -144233652;    float uZSapDWTyZgKTYh8398826 = -140169168;    float uZSapDWTyZgKTYh83465619 = -555222940;    float uZSapDWTyZgKTYh95062639 = -696488448;    float uZSapDWTyZgKTYh49834687 = -937616020;    float uZSapDWTyZgKTYh68710550 = -153913000;    float uZSapDWTyZgKTYh21728250 = -88532395;    float uZSapDWTyZgKTYh57887380 = 18723490;    float uZSapDWTyZgKTYh56913997 = -709637051;    float uZSapDWTyZgKTYh82052332 = -538937025;    float uZSapDWTyZgKTYh15499965 = -999649994;    float uZSapDWTyZgKTYh74924500 = -51457519;    float uZSapDWTyZgKTYh6711575 = -318445385;    float uZSapDWTyZgKTYh87625688 = -873317273;    float uZSapDWTyZgKTYh86914351 = -301532734;    float uZSapDWTyZgKTYh95356660 = -931031702;    float uZSapDWTyZgKTYh35320853 = -134497359;    float uZSapDWTyZgKTYh90120142 = 5432702;    float uZSapDWTyZgKTYh49070582 = -935770822;    float uZSapDWTyZgKTYh74787098 = -320512581;    float uZSapDWTyZgKTYh20693604 = 35597927;    float uZSapDWTyZgKTYh13376685 = -402971597;    float uZSapDWTyZgKTYh16024769 = 24053334;    float uZSapDWTyZgKTYh52929622 = -867185686;    float uZSapDWTyZgKTYh72162691 = -464274981;    float uZSapDWTyZgKTYh73605904 = -673551569;    float uZSapDWTyZgKTYh88843344 = -837696388;    float uZSapDWTyZgKTYh71129717 = -710323217;    float uZSapDWTyZgKTYh84702287 = -695581422;    float uZSapDWTyZgKTYh70789935 = -455630547;    float uZSapDWTyZgKTYh61627123 = -588097338;    float uZSapDWTyZgKTYh31202411 = -198800120;    float uZSapDWTyZgKTYh91942086 = -816450859;    float uZSapDWTyZgKTYh24003868 = -246270034;    float uZSapDWTyZgKTYh12813984 = -630549649;    float uZSapDWTyZgKTYh15293067 = -619781489;    float uZSapDWTyZgKTYh98107763 = -635206095;    float uZSapDWTyZgKTYh21732964 = -512430342;    float uZSapDWTyZgKTYh70007336 = -162780665;    float uZSapDWTyZgKTYh631687 = -962951497;    float uZSapDWTyZgKTYh37948749 = -166277724;    float uZSapDWTyZgKTYh28216500 = 72951676;    float uZSapDWTyZgKTYh63332197 = -974584796;    float uZSapDWTyZgKTYh9881846 = -475331542;    float uZSapDWTyZgKTYh84353135 = -310235561;    float uZSapDWTyZgKTYh60779355 = -596433371;    float uZSapDWTyZgKTYh60874145 = -938265187;    float uZSapDWTyZgKTYh15245984 = -39519556;    float uZSapDWTyZgKTYh99213243 = -666903179;    float uZSapDWTyZgKTYh2920168 = -840559017;    float uZSapDWTyZgKTYh90504889 = -904650254;    float uZSapDWTyZgKTYh2973498 = -300087225;    float uZSapDWTyZgKTYh41050123 = -214939697;    float uZSapDWTyZgKTYh63234981 = -695464416;    float uZSapDWTyZgKTYh34391920 = -934986538;    float uZSapDWTyZgKTYh50076306 = -741262056;    float uZSapDWTyZgKTYh92374057 = -64222503;    float uZSapDWTyZgKTYh30535998 = -688037255;    float uZSapDWTyZgKTYh22899948 = -132213468;    float uZSapDWTyZgKTYh76228782 = -121460736;     uZSapDWTyZgKTYh76930043 = uZSapDWTyZgKTYh52116456;     uZSapDWTyZgKTYh52116456 = uZSapDWTyZgKTYh9962166;     uZSapDWTyZgKTYh9962166 = uZSapDWTyZgKTYh18412819;     uZSapDWTyZgKTYh18412819 = uZSapDWTyZgKTYh34464097;     uZSapDWTyZgKTYh34464097 = uZSapDWTyZgKTYh6797483;     uZSapDWTyZgKTYh6797483 = uZSapDWTyZgKTYh33622879;     uZSapDWTyZgKTYh33622879 = uZSapDWTyZgKTYh7958947;     uZSapDWTyZgKTYh7958947 = uZSapDWTyZgKTYh52782721;     uZSapDWTyZgKTYh52782721 = uZSapDWTyZgKTYh7866547;     uZSapDWTyZgKTYh7866547 = uZSapDWTyZgKTYh41191624;     uZSapDWTyZgKTYh41191624 = uZSapDWTyZgKTYh91715016;     uZSapDWTyZgKTYh91715016 = uZSapDWTyZgKTYh57257313;     uZSapDWTyZgKTYh57257313 = uZSapDWTyZgKTYh82979114;     uZSapDWTyZgKTYh82979114 = uZSapDWTyZgKTYh96615678;     uZSapDWTyZgKTYh96615678 = uZSapDWTyZgKTYh54720764;     uZSapDWTyZgKTYh54720764 = uZSapDWTyZgKTYh21323116;     uZSapDWTyZgKTYh21323116 = uZSapDWTyZgKTYh29964166;     uZSapDWTyZgKTYh29964166 = uZSapDWTyZgKTYh79089391;     uZSapDWTyZgKTYh79089391 = uZSapDWTyZgKTYh76266974;     uZSapDWTyZgKTYh76266974 = uZSapDWTyZgKTYh23691892;     uZSapDWTyZgKTYh23691892 = uZSapDWTyZgKTYh81573383;     uZSapDWTyZgKTYh81573383 = uZSapDWTyZgKTYh16795603;     uZSapDWTyZgKTYh16795603 = uZSapDWTyZgKTYh19842023;     uZSapDWTyZgKTYh19842023 = uZSapDWTyZgKTYh69342236;     uZSapDWTyZgKTYh69342236 = uZSapDWTyZgKTYh59676999;     uZSapDWTyZgKTYh59676999 = uZSapDWTyZgKTYh86103879;     uZSapDWTyZgKTYh86103879 = uZSapDWTyZgKTYh20246195;     uZSapDWTyZgKTYh20246195 = uZSapDWTyZgKTYh91934178;     uZSapDWTyZgKTYh91934178 = uZSapDWTyZgKTYh99853099;     uZSapDWTyZgKTYh99853099 = uZSapDWTyZgKTYh35703856;     uZSapDWTyZgKTYh35703856 = uZSapDWTyZgKTYh67585720;     uZSapDWTyZgKTYh67585720 = uZSapDWTyZgKTYh2871673;     uZSapDWTyZgKTYh2871673 = uZSapDWTyZgKTYh86127595;     uZSapDWTyZgKTYh86127595 = uZSapDWTyZgKTYh98276827;     uZSapDWTyZgKTYh98276827 = uZSapDWTyZgKTYh25825743;     uZSapDWTyZgKTYh25825743 = uZSapDWTyZgKTYh93093640;     uZSapDWTyZgKTYh93093640 = uZSapDWTyZgKTYh90120705;     uZSapDWTyZgKTYh90120705 = uZSapDWTyZgKTYh38022080;     uZSapDWTyZgKTYh38022080 = uZSapDWTyZgKTYh55085523;     uZSapDWTyZgKTYh55085523 = uZSapDWTyZgKTYh63452990;     uZSapDWTyZgKTYh63452990 = uZSapDWTyZgKTYh8398826;     uZSapDWTyZgKTYh8398826 = uZSapDWTyZgKTYh83465619;     uZSapDWTyZgKTYh83465619 = uZSapDWTyZgKTYh95062639;     uZSapDWTyZgKTYh95062639 = uZSapDWTyZgKTYh49834687;     uZSapDWTyZgKTYh49834687 = uZSapDWTyZgKTYh68710550;     uZSapDWTyZgKTYh68710550 = uZSapDWTyZgKTYh21728250;     uZSapDWTyZgKTYh21728250 = uZSapDWTyZgKTYh57887380;     uZSapDWTyZgKTYh57887380 = uZSapDWTyZgKTYh56913997;     uZSapDWTyZgKTYh56913997 = uZSapDWTyZgKTYh82052332;     uZSapDWTyZgKTYh82052332 = uZSapDWTyZgKTYh15499965;     uZSapDWTyZgKTYh15499965 = uZSapDWTyZgKTYh74924500;     uZSapDWTyZgKTYh74924500 = uZSapDWTyZgKTYh6711575;     uZSapDWTyZgKTYh6711575 = uZSapDWTyZgKTYh87625688;     uZSapDWTyZgKTYh87625688 = uZSapDWTyZgKTYh86914351;     uZSapDWTyZgKTYh86914351 = uZSapDWTyZgKTYh95356660;     uZSapDWTyZgKTYh95356660 = uZSapDWTyZgKTYh35320853;     uZSapDWTyZgKTYh35320853 = uZSapDWTyZgKTYh90120142;     uZSapDWTyZgKTYh90120142 = uZSapDWTyZgKTYh49070582;     uZSapDWTyZgKTYh49070582 = uZSapDWTyZgKTYh74787098;     uZSapDWTyZgKTYh74787098 = uZSapDWTyZgKTYh20693604;     uZSapDWTyZgKTYh20693604 = uZSapDWTyZgKTYh13376685;     uZSapDWTyZgKTYh13376685 = uZSapDWTyZgKTYh16024769;     uZSapDWTyZgKTYh16024769 = uZSapDWTyZgKTYh52929622;     uZSapDWTyZgKTYh52929622 = uZSapDWTyZgKTYh72162691;     uZSapDWTyZgKTYh72162691 = uZSapDWTyZgKTYh73605904;     uZSapDWTyZgKTYh73605904 = uZSapDWTyZgKTYh88843344;     uZSapDWTyZgKTYh88843344 = uZSapDWTyZgKTYh71129717;     uZSapDWTyZgKTYh71129717 = uZSapDWTyZgKTYh84702287;     uZSapDWTyZgKTYh84702287 = uZSapDWTyZgKTYh70789935;     uZSapDWTyZgKTYh70789935 = uZSapDWTyZgKTYh61627123;     uZSapDWTyZgKTYh61627123 = uZSapDWTyZgKTYh31202411;     uZSapDWTyZgKTYh31202411 = uZSapDWTyZgKTYh91942086;     uZSapDWTyZgKTYh91942086 = uZSapDWTyZgKTYh24003868;     uZSapDWTyZgKTYh24003868 = uZSapDWTyZgKTYh12813984;     uZSapDWTyZgKTYh12813984 = uZSapDWTyZgKTYh15293067;     uZSapDWTyZgKTYh15293067 = uZSapDWTyZgKTYh98107763;     uZSapDWTyZgKTYh98107763 = uZSapDWTyZgKTYh21732964;     uZSapDWTyZgKTYh21732964 = uZSapDWTyZgKTYh70007336;     uZSapDWTyZgKTYh70007336 = uZSapDWTyZgKTYh631687;     uZSapDWTyZgKTYh631687 = uZSapDWTyZgKTYh37948749;     uZSapDWTyZgKTYh37948749 = uZSapDWTyZgKTYh28216500;     uZSapDWTyZgKTYh28216500 = uZSapDWTyZgKTYh63332197;     uZSapDWTyZgKTYh63332197 = uZSapDWTyZgKTYh9881846;     uZSapDWTyZgKTYh9881846 = uZSapDWTyZgKTYh84353135;     uZSapDWTyZgKTYh84353135 = uZSapDWTyZgKTYh60779355;     uZSapDWTyZgKTYh60779355 = uZSapDWTyZgKTYh60874145;     uZSapDWTyZgKTYh60874145 = uZSapDWTyZgKTYh15245984;     uZSapDWTyZgKTYh15245984 = uZSapDWTyZgKTYh99213243;     uZSapDWTyZgKTYh99213243 = uZSapDWTyZgKTYh2920168;     uZSapDWTyZgKTYh2920168 = uZSapDWTyZgKTYh90504889;     uZSapDWTyZgKTYh90504889 = uZSapDWTyZgKTYh2973498;     uZSapDWTyZgKTYh2973498 = uZSapDWTyZgKTYh41050123;     uZSapDWTyZgKTYh41050123 = uZSapDWTyZgKTYh63234981;     uZSapDWTyZgKTYh63234981 = uZSapDWTyZgKTYh34391920;     uZSapDWTyZgKTYh34391920 = uZSapDWTyZgKTYh50076306;     uZSapDWTyZgKTYh50076306 = uZSapDWTyZgKTYh92374057;     uZSapDWTyZgKTYh92374057 = uZSapDWTyZgKTYh30535998;     uZSapDWTyZgKTYh30535998 = uZSapDWTyZgKTYh22899948;     uZSapDWTyZgKTYh22899948 = uZSapDWTyZgKTYh76228782;     uZSapDWTyZgKTYh76228782 = uZSapDWTyZgKTYh76930043;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void VxxrwVAhPOdJjRWx65022457() {     float NJSDBcIhvGtgAgW47581710 = -739609867;    float NJSDBcIhvGtgAgW20789974 = -315059447;    float NJSDBcIhvGtgAgW89931084 = -637258441;    float NJSDBcIhvGtgAgW58344105 = -242773707;    float NJSDBcIhvGtgAgW55914419 = -558729046;    float NJSDBcIhvGtgAgW96797245 = -874308058;    float NJSDBcIhvGtgAgW52001774 = -85969153;    float NJSDBcIhvGtgAgW64801327 = -437032322;    float NJSDBcIhvGtgAgW98647775 = -270418568;    float NJSDBcIhvGtgAgW30833008 = -513261221;    float NJSDBcIhvGtgAgW83440784 = 34835930;    float NJSDBcIhvGtgAgW37261588 = -66297009;    float NJSDBcIhvGtgAgW37084700 = -688296187;    float NJSDBcIhvGtgAgW66826148 = -268200351;    float NJSDBcIhvGtgAgW79335697 = -744528162;    float NJSDBcIhvGtgAgW87249404 = -652735446;    float NJSDBcIhvGtgAgW15175943 = -358695768;    float NJSDBcIhvGtgAgW50257660 = -921594051;    float NJSDBcIhvGtgAgW60797173 = -952846586;    float NJSDBcIhvGtgAgW35053683 = -317149605;    float NJSDBcIhvGtgAgW78827646 = -768629798;    float NJSDBcIhvGtgAgW42969148 = -521714340;    float NJSDBcIhvGtgAgW18739412 = -776761935;    float NJSDBcIhvGtgAgW53691560 = -792797590;    float NJSDBcIhvGtgAgW33466625 = -892769184;    float NJSDBcIhvGtgAgW90274579 = -453094564;    float NJSDBcIhvGtgAgW94364665 = -403502609;    float NJSDBcIhvGtgAgW28028420 = -89345614;    float NJSDBcIhvGtgAgW24076667 = -308626783;    float NJSDBcIhvGtgAgW39025195 = -868280625;    float NJSDBcIhvGtgAgW42775852 = -496324575;    float NJSDBcIhvGtgAgW12097181 = -585007735;    float NJSDBcIhvGtgAgW85817033 = -959798438;    float NJSDBcIhvGtgAgW51492570 = -631965597;    float NJSDBcIhvGtgAgW76301063 = -618537498;    float NJSDBcIhvGtgAgW85819001 = -118374160;    float NJSDBcIhvGtgAgW36885127 = -583247730;    float NJSDBcIhvGtgAgW42913194 = 65176374;    float NJSDBcIhvGtgAgW16555369 = 28294367;    float NJSDBcIhvGtgAgW50188821 = -516996973;    float NJSDBcIhvGtgAgW23526275 = -528252530;    float NJSDBcIhvGtgAgW36881816 = 90441762;    float NJSDBcIhvGtgAgW32432579 = -541976992;    float NJSDBcIhvGtgAgW1570940 = -768463420;    float NJSDBcIhvGtgAgW45788745 = -146262414;    float NJSDBcIhvGtgAgW75116385 = 87749845;    float NJSDBcIhvGtgAgW47859453 = -204602195;    float NJSDBcIhvGtgAgW23501590 = -433406805;    float NJSDBcIhvGtgAgW62355450 = -695401020;    float NJSDBcIhvGtgAgW90479819 = -821945904;    float NJSDBcIhvGtgAgW83689289 = -546222116;    float NJSDBcIhvGtgAgW16796005 = -20677241;    float NJSDBcIhvGtgAgW61787231 = -246243427;    float NJSDBcIhvGtgAgW77852993 = -434198770;    float NJSDBcIhvGtgAgW86194810 = -412473649;    float NJSDBcIhvGtgAgW4612563 = -117895527;    float NJSDBcIhvGtgAgW2050562 = -538297513;    float NJSDBcIhvGtgAgW36239525 = -844460851;    float NJSDBcIhvGtgAgW24877481 = -350004523;    float NJSDBcIhvGtgAgW65639839 = -5634483;    float NJSDBcIhvGtgAgW2432581 = -370805450;    float NJSDBcIhvGtgAgW23973355 = -996623539;    float NJSDBcIhvGtgAgW40724661 = -28405539;    float NJSDBcIhvGtgAgW59622580 = -402137944;    float NJSDBcIhvGtgAgW88057155 = 83063354;    float NJSDBcIhvGtgAgW71343604 = -380156335;    float NJSDBcIhvGtgAgW51444555 = -106498572;    float NJSDBcIhvGtgAgW85592129 = 43669409;    float NJSDBcIhvGtgAgW90525084 = -649662853;    float NJSDBcIhvGtgAgW93516695 = -526154003;    float NJSDBcIhvGtgAgW50364278 = 30512283;    float NJSDBcIhvGtgAgW72262749 = -323872142;    float NJSDBcIhvGtgAgW33702292 = -849888418;    float NJSDBcIhvGtgAgW10608352 = -335849613;    float NJSDBcIhvGtgAgW11527409 = -788897075;    float NJSDBcIhvGtgAgW41945830 = -759071560;    float NJSDBcIhvGtgAgW10536569 = -979737348;    float NJSDBcIhvGtgAgW17168472 = 91701485;    float NJSDBcIhvGtgAgW7902815 = -546535177;    float NJSDBcIhvGtgAgW58350240 = -880519030;    float NJSDBcIhvGtgAgW42415127 = -148492370;    float NJSDBcIhvGtgAgW70863075 = -970095804;    float NJSDBcIhvGtgAgW65672970 = -393944594;    float NJSDBcIhvGtgAgW33596848 = -486680880;    float NJSDBcIhvGtgAgW55335906 = -222058510;    float NJSDBcIhvGtgAgW25979847 = -375647335;    float NJSDBcIhvGtgAgW50309949 = -238764308;    float NJSDBcIhvGtgAgW7964040 = -425599668;    float NJSDBcIhvGtgAgW65297760 = -119491949;    float NJSDBcIhvGtgAgW71688501 = -400641971;    float NJSDBcIhvGtgAgW83768439 = -580076647;    float NJSDBcIhvGtgAgW645602 = -738786879;    float NJSDBcIhvGtgAgW18035713 = -584819103;    float NJSDBcIhvGtgAgW50915529 = -966071151;    float NJSDBcIhvGtgAgW47756241 = -46191524;    float NJSDBcIhvGtgAgW99552919 = -531628992;    float NJSDBcIhvGtgAgW96157155 = -881152699;    float NJSDBcIhvGtgAgW72809999 = -39839049;    float NJSDBcIhvGtgAgW13513785 = -751526774;    float NJSDBcIhvGtgAgW74445141 = -739609867;     NJSDBcIhvGtgAgW47581710 = NJSDBcIhvGtgAgW20789974;     NJSDBcIhvGtgAgW20789974 = NJSDBcIhvGtgAgW89931084;     NJSDBcIhvGtgAgW89931084 = NJSDBcIhvGtgAgW58344105;     NJSDBcIhvGtgAgW58344105 = NJSDBcIhvGtgAgW55914419;     NJSDBcIhvGtgAgW55914419 = NJSDBcIhvGtgAgW96797245;     NJSDBcIhvGtgAgW96797245 = NJSDBcIhvGtgAgW52001774;     NJSDBcIhvGtgAgW52001774 = NJSDBcIhvGtgAgW64801327;     NJSDBcIhvGtgAgW64801327 = NJSDBcIhvGtgAgW98647775;     NJSDBcIhvGtgAgW98647775 = NJSDBcIhvGtgAgW30833008;     NJSDBcIhvGtgAgW30833008 = NJSDBcIhvGtgAgW83440784;     NJSDBcIhvGtgAgW83440784 = NJSDBcIhvGtgAgW37261588;     NJSDBcIhvGtgAgW37261588 = NJSDBcIhvGtgAgW37084700;     NJSDBcIhvGtgAgW37084700 = NJSDBcIhvGtgAgW66826148;     NJSDBcIhvGtgAgW66826148 = NJSDBcIhvGtgAgW79335697;     NJSDBcIhvGtgAgW79335697 = NJSDBcIhvGtgAgW87249404;     NJSDBcIhvGtgAgW87249404 = NJSDBcIhvGtgAgW15175943;     NJSDBcIhvGtgAgW15175943 = NJSDBcIhvGtgAgW50257660;     NJSDBcIhvGtgAgW50257660 = NJSDBcIhvGtgAgW60797173;     NJSDBcIhvGtgAgW60797173 = NJSDBcIhvGtgAgW35053683;     NJSDBcIhvGtgAgW35053683 = NJSDBcIhvGtgAgW78827646;     NJSDBcIhvGtgAgW78827646 = NJSDBcIhvGtgAgW42969148;     NJSDBcIhvGtgAgW42969148 = NJSDBcIhvGtgAgW18739412;     NJSDBcIhvGtgAgW18739412 = NJSDBcIhvGtgAgW53691560;     NJSDBcIhvGtgAgW53691560 = NJSDBcIhvGtgAgW33466625;     NJSDBcIhvGtgAgW33466625 = NJSDBcIhvGtgAgW90274579;     NJSDBcIhvGtgAgW90274579 = NJSDBcIhvGtgAgW94364665;     NJSDBcIhvGtgAgW94364665 = NJSDBcIhvGtgAgW28028420;     NJSDBcIhvGtgAgW28028420 = NJSDBcIhvGtgAgW24076667;     NJSDBcIhvGtgAgW24076667 = NJSDBcIhvGtgAgW39025195;     NJSDBcIhvGtgAgW39025195 = NJSDBcIhvGtgAgW42775852;     NJSDBcIhvGtgAgW42775852 = NJSDBcIhvGtgAgW12097181;     NJSDBcIhvGtgAgW12097181 = NJSDBcIhvGtgAgW85817033;     NJSDBcIhvGtgAgW85817033 = NJSDBcIhvGtgAgW51492570;     NJSDBcIhvGtgAgW51492570 = NJSDBcIhvGtgAgW76301063;     NJSDBcIhvGtgAgW76301063 = NJSDBcIhvGtgAgW85819001;     NJSDBcIhvGtgAgW85819001 = NJSDBcIhvGtgAgW36885127;     NJSDBcIhvGtgAgW36885127 = NJSDBcIhvGtgAgW42913194;     NJSDBcIhvGtgAgW42913194 = NJSDBcIhvGtgAgW16555369;     NJSDBcIhvGtgAgW16555369 = NJSDBcIhvGtgAgW50188821;     NJSDBcIhvGtgAgW50188821 = NJSDBcIhvGtgAgW23526275;     NJSDBcIhvGtgAgW23526275 = NJSDBcIhvGtgAgW36881816;     NJSDBcIhvGtgAgW36881816 = NJSDBcIhvGtgAgW32432579;     NJSDBcIhvGtgAgW32432579 = NJSDBcIhvGtgAgW1570940;     NJSDBcIhvGtgAgW1570940 = NJSDBcIhvGtgAgW45788745;     NJSDBcIhvGtgAgW45788745 = NJSDBcIhvGtgAgW75116385;     NJSDBcIhvGtgAgW75116385 = NJSDBcIhvGtgAgW47859453;     NJSDBcIhvGtgAgW47859453 = NJSDBcIhvGtgAgW23501590;     NJSDBcIhvGtgAgW23501590 = NJSDBcIhvGtgAgW62355450;     NJSDBcIhvGtgAgW62355450 = NJSDBcIhvGtgAgW90479819;     NJSDBcIhvGtgAgW90479819 = NJSDBcIhvGtgAgW83689289;     NJSDBcIhvGtgAgW83689289 = NJSDBcIhvGtgAgW16796005;     NJSDBcIhvGtgAgW16796005 = NJSDBcIhvGtgAgW61787231;     NJSDBcIhvGtgAgW61787231 = NJSDBcIhvGtgAgW77852993;     NJSDBcIhvGtgAgW77852993 = NJSDBcIhvGtgAgW86194810;     NJSDBcIhvGtgAgW86194810 = NJSDBcIhvGtgAgW4612563;     NJSDBcIhvGtgAgW4612563 = NJSDBcIhvGtgAgW2050562;     NJSDBcIhvGtgAgW2050562 = NJSDBcIhvGtgAgW36239525;     NJSDBcIhvGtgAgW36239525 = NJSDBcIhvGtgAgW24877481;     NJSDBcIhvGtgAgW24877481 = NJSDBcIhvGtgAgW65639839;     NJSDBcIhvGtgAgW65639839 = NJSDBcIhvGtgAgW2432581;     NJSDBcIhvGtgAgW2432581 = NJSDBcIhvGtgAgW23973355;     NJSDBcIhvGtgAgW23973355 = NJSDBcIhvGtgAgW40724661;     NJSDBcIhvGtgAgW40724661 = NJSDBcIhvGtgAgW59622580;     NJSDBcIhvGtgAgW59622580 = NJSDBcIhvGtgAgW88057155;     NJSDBcIhvGtgAgW88057155 = NJSDBcIhvGtgAgW71343604;     NJSDBcIhvGtgAgW71343604 = NJSDBcIhvGtgAgW51444555;     NJSDBcIhvGtgAgW51444555 = NJSDBcIhvGtgAgW85592129;     NJSDBcIhvGtgAgW85592129 = NJSDBcIhvGtgAgW90525084;     NJSDBcIhvGtgAgW90525084 = NJSDBcIhvGtgAgW93516695;     NJSDBcIhvGtgAgW93516695 = NJSDBcIhvGtgAgW50364278;     NJSDBcIhvGtgAgW50364278 = NJSDBcIhvGtgAgW72262749;     NJSDBcIhvGtgAgW72262749 = NJSDBcIhvGtgAgW33702292;     NJSDBcIhvGtgAgW33702292 = NJSDBcIhvGtgAgW10608352;     NJSDBcIhvGtgAgW10608352 = NJSDBcIhvGtgAgW11527409;     NJSDBcIhvGtgAgW11527409 = NJSDBcIhvGtgAgW41945830;     NJSDBcIhvGtgAgW41945830 = NJSDBcIhvGtgAgW10536569;     NJSDBcIhvGtgAgW10536569 = NJSDBcIhvGtgAgW17168472;     NJSDBcIhvGtgAgW17168472 = NJSDBcIhvGtgAgW7902815;     NJSDBcIhvGtgAgW7902815 = NJSDBcIhvGtgAgW58350240;     NJSDBcIhvGtgAgW58350240 = NJSDBcIhvGtgAgW42415127;     NJSDBcIhvGtgAgW42415127 = NJSDBcIhvGtgAgW70863075;     NJSDBcIhvGtgAgW70863075 = NJSDBcIhvGtgAgW65672970;     NJSDBcIhvGtgAgW65672970 = NJSDBcIhvGtgAgW33596848;     NJSDBcIhvGtgAgW33596848 = NJSDBcIhvGtgAgW55335906;     NJSDBcIhvGtgAgW55335906 = NJSDBcIhvGtgAgW25979847;     NJSDBcIhvGtgAgW25979847 = NJSDBcIhvGtgAgW50309949;     NJSDBcIhvGtgAgW50309949 = NJSDBcIhvGtgAgW7964040;     NJSDBcIhvGtgAgW7964040 = NJSDBcIhvGtgAgW65297760;     NJSDBcIhvGtgAgW65297760 = NJSDBcIhvGtgAgW71688501;     NJSDBcIhvGtgAgW71688501 = NJSDBcIhvGtgAgW83768439;     NJSDBcIhvGtgAgW83768439 = NJSDBcIhvGtgAgW645602;     NJSDBcIhvGtgAgW645602 = NJSDBcIhvGtgAgW18035713;     NJSDBcIhvGtgAgW18035713 = NJSDBcIhvGtgAgW50915529;     NJSDBcIhvGtgAgW50915529 = NJSDBcIhvGtgAgW47756241;     NJSDBcIhvGtgAgW47756241 = NJSDBcIhvGtgAgW99552919;     NJSDBcIhvGtgAgW99552919 = NJSDBcIhvGtgAgW96157155;     NJSDBcIhvGtgAgW96157155 = NJSDBcIhvGtgAgW72809999;     NJSDBcIhvGtgAgW72809999 = NJSDBcIhvGtgAgW13513785;     NJSDBcIhvGtgAgW13513785 = NJSDBcIhvGtgAgW74445141;     NJSDBcIhvGtgAgW74445141 = NJSDBcIhvGtgAgW47581710;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void CzkyAsfprIRzsmDK41034577() {     float FTsvjefGpAfQapf34503388 = -880261071;    float FTsvjefGpAfQapf30437712 = -425646021;    float FTsvjefGpAfQapf82627098 = -68373824;    float FTsvjefGpAfQapf86207299 = -248570998;    float FTsvjefGpAfQapf6643506 = -412542524;    float FTsvjefGpAfQapf10262923 = -172001353;    float FTsvjefGpAfQapf2047443 = -867767191;    float FTsvjefGpAfQapf47517676 = -967572143;    float FTsvjefGpAfQapf77033465 = -479929270;    float FTsvjefGpAfQapf4333394 = -228122427;    float FTsvjefGpAfQapf3121515 = -119252814;    float FTsvjefGpAfQapf6888066 = -188399893;    float FTsvjefGpAfQapf97711149 = -776659476;    float FTsvjefGpAfQapf10144258 = -64698750;    float FTsvjefGpAfQapf3048568 = -931293712;    float FTsvjefGpAfQapf22528563 = -631387032;    float FTsvjefGpAfQapf65016118 = 71384395;    float FTsvjefGpAfQapf79474417 = -596980295;    float FTsvjefGpAfQapf21956744 = -289367556;    float FTsvjefGpAfQapf91806510 = -755247444;    float FTsvjefGpAfQapf46293315 = -813308791;    float FTsvjefGpAfQapf81659107 = -349836242;    float FTsvjefGpAfQapf88385751 = -674168185;    float FTsvjefGpAfQapf30048510 = -726886366;    float FTsvjefGpAfQapf89431610 = -917036964;    float FTsvjefGpAfQapf84453479 = -16617909;    float FTsvjefGpAfQapf51255093 = -655604674;    float FTsvjefGpAfQapf28614970 = -316994881;    float FTsvjefGpAfQapf96979459 = -724859415;    float FTsvjefGpAfQapf18738834 = -429500695;    float FTsvjefGpAfQapf1896105 = -412753475;    float FTsvjefGpAfQapf16539211 = -755929437;    float FTsvjefGpAfQapf40090116 = -443490171;    float FTsvjefGpAfQapf81415668 = -768080366;    float FTsvjefGpAfQapf69080012 = -990507595;    float FTsvjefGpAfQapf18779217 = -298969977;    float FTsvjefGpAfQapf92854389 = -551153019;    float FTsvjefGpAfQapf15241571 = -915056196;    float FTsvjefGpAfQapf69786535 = -334556805;    float FTsvjefGpAfQapf77975896 = -797390142;    float FTsvjefGpAfQapf6687706 = -250175181;    float FTsvjefGpAfQapf21722742 = -266713720;    float FTsvjefGpAfQapf8882753 = -335457044;    float FTsvjefGpAfQapf20115620 = -818459102;    float FTsvjefGpAfQapf97773605 = -602154132;    float FTsvjefGpAfQapf77683306 = -967600289;    float FTsvjefGpAfQapf68509479 = -780306709;    float FTsvjefGpAfQapf25757534 = -632176559;    float FTsvjefGpAfQapf52581078 = -545144699;    float FTsvjefGpAfQapf83079398 = -59964233;    float FTsvjefGpAfQapf25047152 = -605576842;    float FTsvjefGpAfQapf21709047 = -619989925;    float FTsvjefGpAfQapf46928834 = -54163112;    float FTsvjefGpAfQapf32397963 = 49709070;    float FTsvjefGpAfQapf94741261 = 36413104;    float FTsvjefGpAfQapf52844280 = -430424829;    float FTsvjefGpAfQapf42051960 = -751477837;    float FTsvjefGpAfQapf52578588 = -341487458;    float FTsvjefGpAfQapf96775689 = -331534034;    float FTsvjefGpAfQapf22190027 = -295924615;    float FTsvjefGpAfQapf59007830 = -516396679;    float FTsvjefGpAfQapf73432473 = -450772311;    float FTsvjefGpAfQapf50538216 = -142712728;    float FTsvjefGpAfQapf58294632 = 49571424;    float FTsvjefGpAfQapf2437290 = -815368952;    float FTsvjefGpAfQapf86582304 = -363323377;    float FTsvjefGpAfQapf66797949 = -744909723;    float FTsvjefGpAfQapf16295481 = 91420889;    float FTsvjefGpAfQapf41064245 = -74191156;    float FTsvjefGpAfQapf84269350 = -532323736;    float FTsvjefGpAfQapf29674174 = 19765986;    float FTsvjefGpAfQapf49774547 = -13559410;    float FTsvjefGpAfQapf9687883 = -162423491;    float FTsvjefGpAfQapf43980847 = -491977414;    float FTsvjefGpAfQapf85118805 = -405072264;    float FTsvjefGpAfQapf24570574 = -446595071;    float FTsvjefGpAfQapf72776354 = 85620801;    float FTsvjefGpAfQapf68270132 = -855709083;    float FTsvjefGpAfQapf32274905 = -24732234;    float FTsvjefGpAfQapf11748304 = -949436675;    float FTsvjefGpAfQapf15944001 = -236311201;    float FTsvjefGpAfQapf25497560 = 76571884;    float FTsvjefGpAfQapf76033891 = -771850182;    float FTsvjefGpAfQapf13900062 = -564895182;    float FTsvjefGpAfQapf93691682 = -823923853;    float FTsvjefGpAfQapf80187057 = -792763550;    float FTsvjefGpAfQapf69610376 = -601766326;    float FTsvjefGpAfQapf7692154 = -393199241;    float FTsvjefGpAfQapf86674406 = -704493471;    float FTsvjefGpAfQapf16235733 = -460082766;    float FTsvjefGpAfQapf76727256 = -547492140;    float FTsvjefGpAfQapf40275802 = -109665561;    float FTsvjefGpAfQapf18465881 = -483522162;    float FTsvjefGpAfQapf47596508 = 61367810;    float FTsvjefGpAfQapf18968067 = -180993464;    float FTsvjefGpAfQapf33255232 = -799402870;    float FTsvjefGpAfQapf71184525 = -24000992;    float FTsvjefGpAfQapf50588121 = -285028468;    float FTsvjefGpAfQapf17678330 = 96909850;    float FTsvjefGpAfQapf11191302 = -880261071;     FTsvjefGpAfQapf34503388 = FTsvjefGpAfQapf30437712;     FTsvjefGpAfQapf30437712 = FTsvjefGpAfQapf82627098;     FTsvjefGpAfQapf82627098 = FTsvjefGpAfQapf86207299;     FTsvjefGpAfQapf86207299 = FTsvjefGpAfQapf6643506;     FTsvjefGpAfQapf6643506 = FTsvjefGpAfQapf10262923;     FTsvjefGpAfQapf10262923 = FTsvjefGpAfQapf2047443;     FTsvjefGpAfQapf2047443 = FTsvjefGpAfQapf47517676;     FTsvjefGpAfQapf47517676 = FTsvjefGpAfQapf77033465;     FTsvjefGpAfQapf77033465 = FTsvjefGpAfQapf4333394;     FTsvjefGpAfQapf4333394 = FTsvjefGpAfQapf3121515;     FTsvjefGpAfQapf3121515 = FTsvjefGpAfQapf6888066;     FTsvjefGpAfQapf6888066 = FTsvjefGpAfQapf97711149;     FTsvjefGpAfQapf97711149 = FTsvjefGpAfQapf10144258;     FTsvjefGpAfQapf10144258 = FTsvjefGpAfQapf3048568;     FTsvjefGpAfQapf3048568 = FTsvjefGpAfQapf22528563;     FTsvjefGpAfQapf22528563 = FTsvjefGpAfQapf65016118;     FTsvjefGpAfQapf65016118 = FTsvjefGpAfQapf79474417;     FTsvjefGpAfQapf79474417 = FTsvjefGpAfQapf21956744;     FTsvjefGpAfQapf21956744 = FTsvjefGpAfQapf91806510;     FTsvjefGpAfQapf91806510 = FTsvjefGpAfQapf46293315;     FTsvjefGpAfQapf46293315 = FTsvjefGpAfQapf81659107;     FTsvjefGpAfQapf81659107 = FTsvjefGpAfQapf88385751;     FTsvjefGpAfQapf88385751 = FTsvjefGpAfQapf30048510;     FTsvjefGpAfQapf30048510 = FTsvjefGpAfQapf89431610;     FTsvjefGpAfQapf89431610 = FTsvjefGpAfQapf84453479;     FTsvjefGpAfQapf84453479 = FTsvjefGpAfQapf51255093;     FTsvjefGpAfQapf51255093 = FTsvjefGpAfQapf28614970;     FTsvjefGpAfQapf28614970 = FTsvjefGpAfQapf96979459;     FTsvjefGpAfQapf96979459 = FTsvjefGpAfQapf18738834;     FTsvjefGpAfQapf18738834 = FTsvjefGpAfQapf1896105;     FTsvjefGpAfQapf1896105 = FTsvjefGpAfQapf16539211;     FTsvjefGpAfQapf16539211 = FTsvjefGpAfQapf40090116;     FTsvjefGpAfQapf40090116 = FTsvjefGpAfQapf81415668;     FTsvjefGpAfQapf81415668 = FTsvjefGpAfQapf69080012;     FTsvjefGpAfQapf69080012 = FTsvjefGpAfQapf18779217;     FTsvjefGpAfQapf18779217 = FTsvjefGpAfQapf92854389;     FTsvjefGpAfQapf92854389 = FTsvjefGpAfQapf15241571;     FTsvjefGpAfQapf15241571 = FTsvjefGpAfQapf69786535;     FTsvjefGpAfQapf69786535 = FTsvjefGpAfQapf77975896;     FTsvjefGpAfQapf77975896 = FTsvjefGpAfQapf6687706;     FTsvjefGpAfQapf6687706 = FTsvjefGpAfQapf21722742;     FTsvjefGpAfQapf21722742 = FTsvjefGpAfQapf8882753;     FTsvjefGpAfQapf8882753 = FTsvjefGpAfQapf20115620;     FTsvjefGpAfQapf20115620 = FTsvjefGpAfQapf97773605;     FTsvjefGpAfQapf97773605 = FTsvjefGpAfQapf77683306;     FTsvjefGpAfQapf77683306 = FTsvjefGpAfQapf68509479;     FTsvjefGpAfQapf68509479 = FTsvjefGpAfQapf25757534;     FTsvjefGpAfQapf25757534 = FTsvjefGpAfQapf52581078;     FTsvjefGpAfQapf52581078 = FTsvjefGpAfQapf83079398;     FTsvjefGpAfQapf83079398 = FTsvjefGpAfQapf25047152;     FTsvjefGpAfQapf25047152 = FTsvjefGpAfQapf21709047;     FTsvjefGpAfQapf21709047 = FTsvjefGpAfQapf46928834;     FTsvjefGpAfQapf46928834 = FTsvjefGpAfQapf32397963;     FTsvjefGpAfQapf32397963 = FTsvjefGpAfQapf94741261;     FTsvjefGpAfQapf94741261 = FTsvjefGpAfQapf52844280;     FTsvjefGpAfQapf52844280 = FTsvjefGpAfQapf42051960;     FTsvjefGpAfQapf42051960 = FTsvjefGpAfQapf52578588;     FTsvjefGpAfQapf52578588 = FTsvjefGpAfQapf96775689;     FTsvjefGpAfQapf96775689 = FTsvjefGpAfQapf22190027;     FTsvjefGpAfQapf22190027 = FTsvjefGpAfQapf59007830;     FTsvjefGpAfQapf59007830 = FTsvjefGpAfQapf73432473;     FTsvjefGpAfQapf73432473 = FTsvjefGpAfQapf50538216;     FTsvjefGpAfQapf50538216 = FTsvjefGpAfQapf58294632;     FTsvjefGpAfQapf58294632 = FTsvjefGpAfQapf2437290;     FTsvjefGpAfQapf2437290 = FTsvjefGpAfQapf86582304;     FTsvjefGpAfQapf86582304 = FTsvjefGpAfQapf66797949;     FTsvjefGpAfQapf66797949 = FTsvjefGpAfQapf16295481;     FTsvjefGpAfQapf16295481 = FTsvjefGpAfQapf41064245;     FTsvjefGpAfQapf41064245 = FTsvjefGpAfQapf84269350;     FTsvjefGpAfQapf84269350 = FTsvjefGpAfQapf29674174;     FTsvjefGpAfQapf29674174 = FTsvjefGpAfQapf49774547;     FTsvjefGpAfQapf49774547 = FTsvjefGpAfQapf9687883;     FTsvjefGpAfQapf9687883 = FTsvjefGpAfQapf43980847;     FTsvjefGpAfQapf43980847 = FTsvjefGpAfQapf85118805;     FTsvjefGpAfQapf85118805 = FTsvjefGpAfQapf24570574;     FTsvjefGpAfQapf24570574 = FTsvjefGpAfQapf72776354;     FTsvjefGpAfQapf72776354 = FTsvjefGpAfQapf68270132;     FTsvjefGpAfQapf68270132 = FTsvjefGpAfQapf32274905;     FTsvjefGpAfQapf32274905 = FTsvjefGpAfQapf11748304;     FTsvjefGpAfQapf11748304 = FTsvjefGpAfQapf15944001;     FTsvjefGpAfQapf15944001 = FTsvjefGpAfQapf25497560;     FTsvjefGpAfQapf25497560 = FTsvjefGpAfQapf76033891;     FTsvjefGpAfQapf76033891 = FTsvjefGpAfQapf13900062;     FTsvjefGpAfQapf13900062 = FTsvjefGpAfQapf93691682;     FTsvjefGpAfQapf93691682 = FTsvjefGpAfQapf80187057;     FTsvjefGpAfQapf80187057 = FTsvjefGpAfQapf69610376;     FTsvjefGpAfQapf69610376 = FTsvjefGpAfQapf7692154;     FTsvjefGpAfQapf7692154 = FTsvjefGpAfQapf86674406;     FTsvjefGpAfQapf86674406 = FTsvjefGpAfQapf16235733;     FTsvjefGpAfQapf16235733 = FTsvjefGpAfQapf76727256;     FTsvjefGpAfQapf76727256 = FTsvjefGpAfQapf40275802;     FTsvjefGpAfQapf40275802 = FTsvjefGpAfQapf18465881;     FTsvjefGpAfQapf18465881 = FTsvjefGpAfQapf47596508;     FTsvjefGpAfQapf47596508 = FTsvjefGpAfQapf18968067;     FTsvjefGpAfQapf18968067 = FTsvjefGpAfQapf33255232;     FTsvjefGpAfQapf33255232 = FTsvjefGpAfQapf71184525;     FTsvjefGpAfQapf71184525 = FTsvjefGpAfQapf50588121;     FTsvjefGpAfQapf50588121 = FTsvjefGpAfQapf17678330;     FTsvjefGpAfQapf17678330 = FTsvjefGpAfQapf11191302;     FTsvjefGpAfQapf11191302 = FTsvjefGpAfQapf34503388;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void fcUIDHntcsFheIlJ78009749() {     float TgeSBhgISxmXvbh2229637 = 50338408;    float TgeSBhgISxmXvbh6359910 = -937219387;    float TgeSBhgISxmXvbh51976943 = -979771668;    float TgeSBhgISxmXvbh14858908 = -69416948;    float TgeSBhgISxmXvbh39206192 = -699519607;    float TgeSBhgISxmXvbh45777075 = -702891706;    float TgeSBhgISxmXvbh41649814 = -92398292;    float TgeSBhgISxmXvbh63190481 = -641777872;    float TgeSBhgISxmXvbh69162350 = -30918009;    float TgeSBhgISxmXvbh10104507 = -428869361;    float TgeSBhgISxmXvbh82372685 = -549992714;    float TgeSBhgISxmXvbh14317979 = 11800540;    float TgeSBhgISxmXvbh32754929 = -513494966;    float TgeSBhgISxmXvbh13636799 = -225456980;    float TgeSBhgISxmXvbh54731880 = -614362645;    float TgeSBhgISxmXvbh51581487 = -615690661;    float TgeSBhgISxmXvbh70860465 = -651910166;    float TgeSBhgISxmXvbh40854780 = -282230140;    float TgeSBhgISxmXvbh82446044 = -756640400;    float TgeSBhgISxmXvbh24762002 = -174540203;    float TgeSBhgISxmXvbh92671458 = 52820678;    float TgeSBhgISxmXvbh72518002 = -98150331;    float TgeSBhgISxmXvbh92087639 = -722463198;    float TgeSBhgISxmXvbh98770036 = -372758929;    float TgeSBhgISxmXvbh57661930 = -592376489;    float TgeSBhgISxmXvbh48089604 = -365009277;    float TgeSBhgISxmXvbh66576171 = -159146601;    float TgeSBhgISxmXvbh16680945 = -706364463;    float TgeSBhgISxmXvbh76652790 = -234941974;    float TgeSBhgISxmXvbh9550475 = -834469967;    float TgeSBhgISxmXvbh27894060 = -332290336;    float TgeSBhgISxmXvbh27812649 = -288326025;    float TgeSBhgISxmXvbh38925333 = -618260268;    float TgeSBhgISxmXvbh46431300 = -188768139;    float TgeSBhgISxmXvbh93163258 = 4285318;    float TgeSBhgISxmXvbh15973734 = -446135503;    float TgeSBhgISxmXvbh23530982 = -533327728;    float TgeSBhgISxmXvbh25808112 = -904015326;    float TgeSBhgISxmXvbh2638614 = 33664349;    float TgeSBhgISxmXvbh46048109 = -884737259;    float TgeSBhgISxmXvbh67341343 = -728746587;    float TgeSBhgISxmXvbh59254135 = -710882407;    float TgeSBhgISxmXvbh83716122 = -941612564;    float TgeSBhgISxmXvbh69797237 = -899107529;    float TgeSBhgISxmXvbh89790861 = -752039414;    float TgeSBhgISxmXvbh70877955 = -893084440;    float TgeSBhgISxmXvbh92030957 = -712940201;    float TgeSBhgISxmXvbh22312480 = 80179466;    float TgeSBhgISxmXvbh23356606 = -158895070;    float TgeSBhgISxmXvbh55366905 = -648990084;    float TgeSBhgISxmXvbh72062198 = -881992625;    float TgeSBhgISxmXvbh20725412 = -917888972;    float TgeSBhgISxmXvbh68963583 = -978212893;    float TgeSBhgISxmXvbh45519021 = -862005106;    float TgeSBhgISxmXvbh85338290 = -565281608;    float TgeSBhgISxmXvbh29711634 = -851511261;    float TgeSBhgISxmXvbh14272270 = -114756189;    float TgeSBhgISxmXvbh53206906 = -507012739;    float TgeSBhgISxmXvbh57196977 = -477040459;    float TgeSBhgISxmXvbh91116587 = -234510331;    float TgeSBhgISxmXvbh79200904 = -443745106;    float TgeSBhgISxmXvbh24968870 = -386033830;    float TgeSBhgISxmXvbh86537691 = -306835899;    float TgeSBhgISxmXvbh59611876 = -196448042;    float TgeSBhgISxmXvbh82210447 = 3420974;    float TgeSBhgISxmXvbh54560036 = -161666690;    float TgeSBhgISxmXvbh75392645 = -369939193;    float TgeSBhgISxmXvbh86323628 = -224726828;    float TgeSBhgISxmXvbh20473541 = -129742298;    float TgeSBhgISxmXvbh38758147 = -68227143;    float TgeSBhgISxmXvbh28050506 = 17637067;    float TgeSBhgISxmXvbh45052353 = -747894841;    float TgeSBhgISxmXvbh38216166 = -215894490;    float TgeSBhgISxmXvbh36397935 = -871903142;    float TgeSBhgISxmXvbh57420658 = -445793616;    float TgeSBhgISxmXvbh33417323 = -236296916;    float TgeSBhgISxmXvbh88801880 = -156537767;    float TgeSBhgISxmXvbh22290403 = -823355669;    float TgeSBhgISxmXvbh8979176 = -620719516;    float TgeSBhgISxmXvbh86783975 = -699292050;    float TgeSBhgISxmXvbh56058646 = -652069077;    float TgeSBhgISxmXvbh44263691 = -139326068;    float TgeSBhgISxmXvbh93324338 = -447469393;    float TgeSBhgISxmXvbh21285886 = -585951890;    float TgeSBhgISxmXvbh37488276 = -952477343;    float TgeSBhgISxmXvbh7168648 = -414401364;    float TgeSBhgISxmXvbh58849066 = -310113133;    float TgeSBhgISxmXvbh93406311 = -756255163;    float TgeSBhgISxmXvbh61093010 = -623486532;    float TgeSBhgISxmXvbh63451625 = -144203421;    float TgeSBhgISxmXvbh1701464 = -231379314;    float TgeSBhgISxmXvbh70324076 = 73685011;    float TgeSBhgISxmXvbh68611135 = -326974867;    float TgeSBhgISxmXvbh11522026 = -731825321;    float TgeSBhgISxmXvbh66847205 = -340992153;    float TgeSBhgISxmXvbh42372474 = -242712758;    float TgeSBhgISxmXvbh72716444 = -304046509;    float TgeSBhgISxmXvbh24104246 = -645164522;    float TgeSBhgISxmXvbh87586790 = -802528503;    float TgeSBhgISxmXvbh35230825 = 50338408;     TgeSBhgISxmXvbh2229637 = TgeSBhgISxmXvbh6359910;     TgeSBhgISxmXvbh6359910 = TgeSBhgISxmXvbh51976943;     TgeSBhgISxmXvbh51976943 = TgeSBhgISxmXvbh14858908;     TgeSBhgISxmXvbh14858908 = TgeSBhgISxmXvbh39206192;     TgeSBhgISxmXvbh39206192 = TgeSBhgISxmXvbh45777075;     TgeSBhgISxmXvbh45777075 = TgeSBhgISxmXvbh41649814;     TgeSBhgISxmXvbh41649814 = TgeSBhgISxmXvbh63190481;     TgeSBhgISxmXvbh63190481 = TgeSBhgISxmXvbh69162350;     TgeSBhgISxmXvbh69162350 = TgeSBhgISxmXvbh10104507;     TgeSBhgISxmXvbh10104507 = TgeSBhgISxmXvbh82372685;     TgeSBhgISxmXvbh82372685 = TgeSBhgISxmXvbh14317979;     TgeSBhgISxmXvbh14317979 = TgeSBhgISxmXvbh32754929;     TgeSBhgISxmXvbh32754929 = TgeSBhgISxmXvbh13636799;     TgeSBhgISxmXvbh13636799 = TgeSBhgISxmXvbh54731880;     TgeSBhgISxmXvbh54731880 = TgeSBhgISxmXvbh51581487;     TgeSBhgISxmXvbh51581487 = TgeSBhgISxmXvbh70860465;     TgeSBhgISxmXvbh70860465 = TgeSBhgISxmXvbh40854780;     TgeSBhgISxmXvbh40854780 = TgeSBhgISxmXvbh82446044;     TgeSBhgISxmXvbh82446044 = TgeSBhgISxmXvbh24762002;     TgeSBhgISxmXvbh24762002 = TgeSBhgISxmXvbh92671458;     TgeSBhgISxmXvbh92671458 = TgeSBhgISxmXvbh72518002;     TgeSBhgISxmXvbh72518002 = TgeSBhgISxmXvbh92087639;     TgeSBhgISxmXvbh92087639 = TgeSBhgISxmXvbh98770036;     TgeSBhgISxmXvbh98770036 = TgeSBhgISxmXvbh57661930;     TgeSBhgISxmXvbh57661930 = TgeSBhgISxmXvbh48089604;     TgeSBhgISxmXvbh48089604 = TgeSBhgISxmXvbh66576171;     TgeSBhgISxmXvbh66576171 = TgeSBhgISxmXvbh16680945;     TgeSBhgISxmXvbh16680945 = TgeSBhgISxmXvbh76652790;     TgeSBhgISxmXvbh76652790 = TgeSBhgISxmXvbh9550475;     TgeSBhgISxmXvbh9550475 = TgeSBhgISxmXvbh27894060;     TgeSBhgISxmXvbh27894060 = TgeSBhgISxmXvbh27812649;     TgeSBhgISxmXvbh27812649 = TgeSBhgISxmXvbh38925333;     TgeSBhgISxmXvbh38925333 = TgeSBhgISxmXvbh46431300;     TgeSBhgISxmXvbh46431300 = TgeSBhgISxmXvbh93163258;     TgeSBhgISxmXvbh93163258 = TgeSBhgISxmXvbh15973734;     TgeSBhgISxmXvbh15973734 = TgeSBhgISxmXvbh23530982;     TgeSBhgISxmXvbh23530982 = TgeSBhgISxmXvbh25808112;     TgeSBhgISxmXvbh25808112 = TgeSBhgISxmXvbh2638614;     TgeSBhgISxmXvbh2638614 = TgeSBhgISxmXvbh46048109;     TgeSBhgISxmXvbh46048109 = TgeSBhgISxmXvbh67341343;     TgeSBhgISxmXvbh67341343 = TgeSBhgISxmXvbh59254135;     TgeSBhgISxmXvbh59254135 = TgeSBhgISxmXvbh83716122;     TgeSBhgISxmXvbh83716122 = TgeSBhgISxmXvbh69797237;     TgeSBhgISxmXvbh69797237 = TgeSBhgISxmXvbh89790861;     TgeSBhgISxmXvbh89790861 = TgeSBhgISxmXvbh70877955;     TgeSBhgISxmXvbh70877955 = TgeSBhgISxmXvbh92030957;     TgeSBhgISxmXvbh92030957 = TgeSBhgISxmXvbh22312480;     TgeSBhgISxmXvbh22312480 = TgeSBhgISxmXvbh23356606;     TgeSBhgISxmXvbh23356606 = TgeSBhgISxmXvbh55366905;     TgeSBhgISxmXvbh55366905 = TgeSBhgISxmXvbh72062198;     TgeSBhgISxmXvbh72062198 = TgeSBhgISxmXvbh20725412;     TgeSBhgISxmXvbh20725412 = TgeSBhgISxmXvbh68963583;     TgeSBhgISxmXvbh68963583 = TgeSBhgISxmXvbh45519021;     TgeSBhgISxmXvbh45519021 = TgeSBhgISxmXvbh85338290;     TgeSBhgISxmXvbh85338290 = TgeSBhgISxmXvbh29711634;     TgeSBhgISxmXvbh29711634 = TgeSBhgISxmXvbh14272270;     TgeSBhgISxmXvbh14272270 = TgeSBhgISxmXvbh53206906;     TgeSBhgISxmXvbh53206906 = TgeSBhgISxmXvbh57196977;     TgeSBhgISxmXvbh57196977 = TgeSBhgISxmXvbh91116587;     TgeSBhgISxmXvbh91116587 = TgeSBhgISxmXvbh79200904;     TgeSBhgISxmXvbh79200904 = TgeSBhgISxmXvbh24968870;     TgeSBhgISxmXvbh24968870 = TgeSBhgISxmXvbh86537691;     TgeSBhgISxmXvbh86537691 = TgeSBhgISxmXvbh59611876;     TgeSBhgISxmXvbh59611876 = TgeSBhgISxmXvbh82210447;     TgeSBhgISxmXvbh82210447 = TgeSBhgISxmXvbh54560036;     TgeSBhgISxmXvbh54560036 = TgeSBhgISxmXvbh75392645;     TgeSBhgISxmXvbh75392645 = TgeSBhgISxmXvbh86323628;     TgeSBhgISxmXvbh86323628 = TgeSBhgISxmXvbh20473541;     TgeSBhgISxmXvbh20473541 = TgeSBhgISxmXvbh38758147;     TgeSBhgISxmXvbh38758147 = TgeSBhgISxmXvbh28050506;     TgeSBhgISxmXvbh28050506 = TgeSBhgISxmXvbh45052353;     TgeSBhgISxmXvbh45052353 = TgeSBhgISxmXvbh38216166;     TgeSBhgISxmXvbh38216166 = TgeSBhgISxmXvbh36397935;     TgeSBhgISxmXvbh36397935 = TgeSBhgISxmXvbh57420658;     TgeSBhgISxmXvbh57420658 = TgeSBhgISxmXvbh33417323;     TgeSBhgISxmXvbh33417323 = TgeSBhgISxmXvbh88801880;     TgeSBhgISxmXvbh88801880 = TgeSBhgISxmXvbh22290403;     TgeSBhgISxmXvbh22290403 = TgeSBhgISxmXvbh8979176;     TgeSBhgISxmXvbh8979176 = TgeSBhgISxmXvbh86783975;     TgeSBhgISxmXvbh86783975 = TgeSBhgISxmXvbh56058646;     TgeSBhgISxmXvbh56058646 = TgeSBhgISxmXvbh44263691;     TgeSBhgISxmXvbh44263691 = TgeSBhgISxmXvbh93324338;     TgeSBhgISxmXvbh93324338 = TgeSBhgISxmXvbh21285886;     TgeSBhgISxmXvbh21285886 = TgeSBhgISxmXvbh37488276;     TgeSBhgISxmXvbh37488276 = TgeSBhgISxmXvbh7168648;     TgeSBhgISxmXvbh7168648 = TgeSBhgISxmXvbh58849066;     TgeSBhgISxmXvbh58849066 = TgeSBhgISxmXvbh93406311;     TgeSBhgISxmXvbh93406311 = TgeSBhgISxmXvbh61093010;     TgeSBhgISxmXvbh61093010 = TgeSBhgISxmXvbh63451625;     TgeSBhgISxmXvbh63451625 = TgeSBhgISxmXvbh1701464;     TgeSBhgISxmXvbh1701464 = TgeSBhgISxmXvbh70324076;     TgeSBhgISxmXvbh70324076 = TgeSBhgISxmXvbh68611135;     TgeSBhgISxmXvbh68611135 = TgeSBhgISxmXvbh11522026;     TgeSBhgISxmXvbh11522026 = TgeSBhgISxmXvbh66847205;     TgeSBhgISxmXvbh66847205 = TgeSBhgISxmXvbh42372474;     TgeSBhgISxmXvbh42372474 = TgeSBhgISxmXvbh72716444;     TgeSBhgISxmXvbh72716444 = TgeSBhgISxmXvbh24104246;     TgeSBhgISxmXvbh24104246 = TgeSBhgISxmXvbh87586790;     TgeSBhgISxmXvbh87586790 = TgeSBhgISxmXvbh35230825;     TgeSBhgISxmXvbh35230825 = TgeSBhgISxmXvbh2229637;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void CjuVmiGUteVHzwTB32425962() {     float BJYpdtQpQZoFrIB72881303 = -567810722;    float BJYpdtQpQZoFrIB75033427 = -808862686;    float BJYpdtQpQZoFrIB31945862 = -322066128;    float BJYpdtQpQZoFrIB54790194 = -259555337;    float BJYpdtQpQZoFrIB60656513 = -482925954;    float BJYpdtQpQZoFrIB35776838 = -404472858;    float BJYpdtQpQZoFrIB60028709 = -91174002;    float BJYpdtQpQZoFrIB20032862 = -988594962;    float BJYpdtQpQZoFrIB15027405 = -124265337;    float BJYpdtQpQZoFrIB33070968 = -729964712;    float BJYpdtQpQZoFrIB24621846 = -584894644;    float BJYpdtQpQZoFrIB59864549 = -303963253;    float BJYpdtQpQZoFrIB12582316 = -423032024;    float BJYpdtQpQZoFrIB97483832 = -26485190;    float BJYpdtQpQZoFrIB37451899 = -764112649;    float BJYpdtQpQZoFrIB84110127 = -185674248;    float BJYpdtQpQZoFrIB64713292 = -561095296;    float BJYpdtQpQZoFrIB61148274 = -271396336;    float BJYpdtQpQZoFrIB64153825 = -363828342;    float BJYpdtQpQZoFrIB83548710 = -716906508;    float BJYpdtQpQZoFrIB47807213 = -955858463;    float BJYpdtQpQZoFrIB33913768 = -429435636;    float BJYpdtQpQZoFrIB94031448 = -190306342;    float BJYpdtQpQZoFrIB32619574 = 34840165;    float BJYpdtQpQZoFrIB21786319 = -268281177;    float BJYpdtQpQZoFrIB78687184 = -463293722;    float BJYpdtQpQZoFrIB74836956 = -554324376;    float BJYpdtQpQZoFrIB24463170 = -111488230;    float BJYpdtQpQZoFrIB8795279 = -529300191;    float BJYpdtQpQZoFrIB48722570 = -292865038;    float BJYpdtQpQZoFrIB34966056 = -80724021;    float BJYpdtQpQZoFrIB72324109 = -616623188;    float BJYpdtQpQZoFrIB21870694 = -565221877;    float BJYpdtQpQZoFrIB11796276 = -852297823;    float BJYpdtQpQZoFrIB71187495 = -942661462;    float BJYpdtQpQZoFrIB75966992 = -525362050;    float BJYpdtQpQZoFrIB67322468 = -721920936;    float BJYpdtQpQZoFrIB78600601 = -688128433;    float BJYpdtQpQZoFrIB81171902 = 77935712;    float BJYpdtQpQZoFrIB41151407 = -402345621;    float BJYpdtQpQZoFrIB27414627 = -12765465;    float BJYpdtQpQZoFrIB87737126 = -480271476;    float BJYpdtQpQZoFrIB32683082 = -928366616;    float BJYpdtQpQZoFrIB76305538 = -971082500;    float BJYpdtQpQZoFrIB85744919 = 39314192;    float BJYpdtQpQZoFrIB77283790 = -651421595;    float BJYpdtQpQZoFrIB18162160 = -829010000;    float BJYpdtQpQZoFrIB87926689 = -371950829;    float BJYpdtQpQZoFrIB28798058 = -144659039;    float BJYpdtQpQZoFrIB63794391 = -931998963;    float BJYpdtQpQZoFrIB40251523 = -428564746;    float BJYpdtQpQZoFrIB62596916 = -887108694;    float BJYpdtQpQZoFrIB24039240 = -906010935;    float BJYpdtQpQZoFrIB35746326 = -422886602;    float BJYpdtQpQZoFrIB84618749 = -676222522;    float BJYpdtQpQZoFrIB38967536 = -38375086;    float BJYpdtQpQZoFrIB81001978 = -518556344;    float BJYpdtQpQZoFrIB99326288 = -256906293;    float BJYpdtQpQZoFrIB33003875 = -991274161;    float BJYpdtQpQZoFrIB81969328 = 80367767;    float BJYpdtQpQZoFrIB60939881 = -850148482;    float BJYpdtQpQZoFrIB35565540 = -979685772;    float BJYpdtQpQZoFrIB11237584 = -359294771;    float BJYpdtQpQZoFrIB66304834 = -831400300;    float BJYpdtQpQZoFrIB98104911 = -549240691;    float BJYpdtQpQZoFrIB52297736 = -968271456;    float BJYpdtQpQZoFrIB37993856 = -738741377;    float BJYpdtQpQZoFrIB786041 = -570734202;    float BJYpdtQpQZoFrIB26296338 = -83823729;    float BJYpdtQpQZoFrIB61484907 = -138750599;    float BJYpdtQpQZoFrIB16787660 = -463753313;    float BJYpdtQpQZoFrIB86112691 = -872966863;    float BJYpdtQpQZoFrIB79976371 = -249332049;    float BJYpdtQpQZoFrIB23002418 = -961482721;    float BJYpdtQpQZoFrIB56134084 = -604141043;    float BJYpdtQpQZoFrIB60070086 = -375586987;    float BJYpdtQpQZoFrIB1230686 = -501069021;    float BJYpdtQpQZoFrIB17725911 = -219223843;    float BJYpdtQpQZoFrIB46874654 = 95525972;    float BJYpdtQpQZoFrIB44502529 = -616859582;    float BJYpdtQpQZoFrIB60525024 = -634283722;    float BJYpdtQpQZoFrIB86910267 = -82373548;    float BJYpdtQpQZoFrIB95665111 = -966829191;    float BJYpdtQpQZoFrIB45000887 = -597301228;    float BJYpdtQpQZoFrIB8471048 = -864300292;    float BJYpdtQpQZoFrIB72369139 = -193615327;    float BJYpdtQpQZoFrIB48284869 = -710612253;    float BJYpdtQpQZoFrIB86124368 = -42335275;    float BJYpdtQpQZoFrIB27177526 = -76075302;    float BJYpdtQpQZoFrIB32219959 = -804286376;    float BJYpdtQpQZoFrIB94965013 = 93194293;    float BJYpdtQpQZoFrIB67996180 = -365014643;    float BJYpdtQpQZoFrIB45596726 = -696854273;    float BJYpdtQpQZoFrIB99202573 = 97567945;    float BJYpdtQpQZoFrIB80211526 = -552197140;    float BJYpdtQpQZoFrIB91849087 = -33079694;    float BJYpdtQpQZoFrIB76499543 = -20976705;    float BJYpdtQpQZoFrIB66378247 = 3033684;    float BJYpdtQpQZoFrIB78200626 = -321841810;    float BJYpdtQpQZoFrIB33447183 = -567810722;     BJYpdtQpQZoFrIB72881303 = BJYpdtQpQZoFrIB75033427;     BJYpdtQpQZoFrIB75033427 = BJYpdtQpQZoFrIB31945862;     BJYpdtQpQZoFrIB31945862 = BJYpdtQpQZoFrIB54790194;     BJYpdtQpQZoFrIB54790194 = BJYpdtQpQZoFrIB60656513;     BJYpdtQpQZoFrIB60656513 = BJYpdtQpQZoFrIB35776838;     BJYpdtQpQZoFrIB35776838 = BJYpdtQpQZoFrIB60028709;     BJYpdtQpQZoFrIB60028709 = BJYpdtQpQZoFrIB20032862;     BJYpdtQpQZoFrIB20032862 = BJYpdtQpQZoFrIB15027405;     BJYpdtQpQZoFrIB15027405 = BJYpdtQpQZoFrIB33070968;     BJYpdtQpQZoFrIB33070968 = BJYpdtQpQZoFrIB24621846;     BJYpdtQpQZoFrIB24621846 = BJYpdtQpQZoFrIB59864549;     BJYpdtQpQZoFrIB59864549 = BJYpdtQpQZoFrIB12582316;     BJYpdtQpQZoFrIB12582316 = BJYpdtQpQZoFrIB97483832;     BJYpdtQpQZoFrIB97483832 = BJYpdtQpQZoFrIB37451899;     BJYpdtQpQZoFrIB37451899 = BJYpdtQpQZoFrIB84110127;     BJYpdtQpQZoFrIB84110127 = BJYpdtQpQZoFrIB64713292;     BJYpdtQpQZoFrIB64713292 = BJYpdtQpQZoFrIB61148274;     BJYpdtQpQZoFrIB61148274 = BJYpdtQpQZoFrIB64153825;     BJYpdtQpQZoFrIB64153825 = BJYpdtQpQZoFrIB83548710;     BJYpdtQpQZoFrIB83548710 = BJYpdtQpQZoFrIB47807213;     BJYpdtQpQZoFrIB47807213 = BJYpdtQpQZoFrIB33913768;     BJYpdtQpQZoFrIB33913768 = BJYpdtQpQZoFrIB94031448;     BJYpdtQpQZoFrIB94031448 = BJYpdtQpQZoFrIB32619574;     BJYpdtQpQZoFrIB32619574 = BJYpdtQpQZoFrIB21786319;     BJYpdtQpQZoFrIB21786319 = BJYpdtQpQZoFrIB78687184;     BJYpdtQpQZoFrIB78687184 = BJYpdtQpQZoFrIB74836956;     BJYpdtQpQZoFrIB74836956 = BJYpdtQpQZoFrIB24463170;     BJYpdtQpQZoFrIB24463170 = BJYpdtQpQZoFrIB8795279;     BJYpdtQpQZoFrIB8795279 = BJYpdtQpQZoFrIB48722570;     BJYpdtQpQZoFrIB48722570 = BJYpdtQpQZoFrIB34966056;     BJYpdtQpQZoFrIB34966056 = BJYpdtQpQZoFrIB72324109;     BJYpdtQpQZoFrIB72324109 = BJYpdtQpQZoFrIB21870694;     BJYpdtQpQZoFrIB21870694 = BJYpdtQpQZoFrIB11796276;     BJYpdtQpQZoFrIB11796276 = BJYpdtQpQZoFrIB71187495;     BJYpdtQpQZoFrIB71187495 = BJYpdtQpQZoFrIB75966992;     BJYpdtQpQZoFrIB75966992 = BJYpdtQpQZoFrIB67322468;     BJYpdtQpQZoFrIB67322468 = BJYpdtQpQZoFrIB78600601;     BJYpdtQpQZoFrIB78600601 = BJYpdtQpQZoFrIB81171902;     BJYpdtQpQZoFrIB81171902 = BJYpdtQpQZoFrIB41151407;     BJYpdtQpQZoFrIB41151407 = BJYpdtQpQZoFrIB27414627;     BJYpdtQpQZoFrIB27414627 = BJYpdtQpQZoFrIB87737126;     BJYpdtQpQZoFrIB87737126 = BJYpdtQpQZoFrIB32683082;     BJYpdtQpQZoFrIB32683082 = BJYpdtQpQZoFrIB76305538;     BJYpdtQpQZoFrIB76305538 = BJYpdtQpQZoFrIB85744919;     BJYpdtQpQZoFrIB85744919 = BJYpdtQpQZoFrIB77283790;     BJYpdtQpQZoFrIB77283790 = BJYpdtQpQZoFrIB18162160;     BJYpdtQpQZoFrIB18162160 = BJYpdtQpQZoFrIB87926689;     BJYpdtQpQZoFrIB87926689 = BJYpdtQpQZoFrIB28798058;     BJYpdtQpQZoFrIB28798058 = BJYpdtQpQZoFrIB63794391;     BJYpdtQpQZoFrIB63794391 = BJYpdtQpQZoFrIB40251523;     BJYpdtQpQZoFrIB40251523 = BJYpdtQpQZoFrIB62596916;     BJYpdtQpQZoFrIB62596916 = BJYpdtQpQZoFrIB24039240;     BJYpdtQpQZoFrIB24039240 = BJYpdtQpQZoFrIB35746326;     BJYpdtQpQZoFrIB35746326 = BJYpdtQpQZoFrIB84618749;     BJYpdtQpQZoFrIB84618749 = BJYpdtQpQZoFrIB38967536;     BJYpdtQpQZoFrIB38967536 = BJYpdtQpQZoFrIB81001978;     BJYpdtQpQZoFrIB81001978 = BJYpdtQpQZoFrIB99326288;     BJYpdtQpQZoFrIB99326288 = BJYpdtQpQZoFrIB33003875;     BJYpdtQpQZoFrIB33003875 = BJYpdtQpQZoFrIB81969328;     BJYpdtQpQZoFrIB81969328 = BJYpdtQpQZoFrIB60939881;     BJYpdtQpQZoFrIB60939881 = BJYpdtQpQZoFrIB35565540;     BJYpdtQpQZoFrIB35565540 = BJYpdtQpQZoFrIB11237584;     BJYpdtQpQZoFrIB11237584 = BJYpdtQpQZoFrIB66304834;     BJYpdtQpQZoFrIB66304834 = BJYpdtQpQZoFrIB98104911;     BJYpdtQpQZoFrIB98104911 = BJYpdtQpQZoFrIB52297736;     BJYpdtQpQZoFrIB52297736 = BJYpdtQpQZoFrIB37993856;     BJYpdtQpQZoFrIB37993856 = BJYpdtQpQZoFrIB786041;     BJYpdtQpQZoFrIB786041 = BJYpdtQpQZoFrIB26296338;     BJYpdtQpQZoFrIB26296338 = BJYpdtQpQZoFrIB61484907;     BJYpdtQpQZoFrIB61484907 = BJYpdtQpQZoFrIB16787660;     BJYpdtQpQZoFrIB16787660 = BJYpdtQpQZoFrIB86112691;     BJYpdtQpQZoFrIB86112691 = BJYpdtQpQZoFrIB79976371;     BJYpdtQpQZoFrIB79976371 = BJYpdtQpQZoFrIB23002418;     BJYpdtQpQZoFrIB23002418 = BJYpdtQpQZoFrIB56134084;     BJYpdtQpQZoFrIB56134084 = BJYpdtQpQZoFrIB60070086;     BJYpdtQpQZoFrIB60070086 = BJYpdtQpQZoFrIB1230686;     BJYpdtQpQZoFrIB1230686 = BJYpdtQpQZoFrIB17725911;     BJYpdtQpQZoFrIB17725911 = BJYpdtQpQZoFrIB46874654;     BJYpdtQpQZoFrIB46874654 = BJYpdtQpQZoFrIB44502529;     BJYpdtQpQZoFrIB44502529 = BJYpdtQpQZoFrIB60525024;     BJYpdtQpQZoFrIB60525024 = BJYpdtQpQZoFrIB86910267;     BJYpdtQpQZoFrIB86910267 = BJYpdtQpQZoFrIB95665111;     BJYpdtQpQZoFrIB95665111 = BJYpdtQpQZoFrIB45000887;     BJYpdtQpQZoFrIB45000887 = BJYpdtQpQZoFrIB8471048;     BJYpdtQpQZoFrIB8471048 = BJYpdtQpQZoFrIB72369139;     BJYpdtQpQZoFrIB72369139 = BJYpdtQpQZoFrIB48284869;     BJYpdtQpQZoFrIB48284869 = BJYpdtQpQZoFrIB86124368;     BJYpdtQpQZoFrIB86124368 = BJYpdtQpQZoFrIB27177526;     BJYpdtQpQZoFrIB27177526 = BJYpdtQpQZoFrIB32219959;     BJYpdtQpQZoFrIB32219959 = BJYpdtQpQZoFrIB94965013;     BJYpdtQpQZoFrIB94965013 = BJYpdtQpQZoFrIB67996180;     BJYpdtQpQZoFrIB67996180 = BJYpdtQpQZoFrIB45596726;     BJYpdtQpQZoFrIB45596726 = BJYpdtQpQZoFrIB99202573;     BJYpdtQpQZoFrIB99202573 = BJYpdtQpQZoFrIB80211526;     BJYpdtQpQZoFrIB80211526 = BJYpdtQpQZoFrIB91849087;     BJYpdtQpQZoFrIB91849087 = BJYpdtQpQZoFrIB76499543;     BJYpdtQpQZoFrIB76499543 = BJYpdtQpQZoFrIB66378247;     BJYpdtQpQZoFrIB66378247 = BJYpdtQpQZoFrIB78200626;     BJYpdtQpQZoFrIB78200626 = BJYpdtQpQZoFrIB33447183;     BJYpdtQpQZoFrIB33447183 = BJYpdtQpQZoFrIB72881303;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void PeMgSPHdfJvbhfmR39084707() {     float LwNSGBnmOTSnkct72874832 = -440334862;    float LwNSGBnmOTSnkct13605744 = -851457805;    float LwNSGBnmOTSnkct33259074 = -279194742;    float LwNSGBnmOTSnkct39870055 = -80096171;    float LwNSGBnmOTSnkct69496615 = -951281268;    float LwNSGBnmOTSnkct70582270 = -103905669;    float LwNSGBnmOTSnkct28576046 = -895710469;    float LwNSGBnmOTSnkct89246912 = -692772263;    float LwNSGBnmOTSnkct97767568 = -937911406;    float LwNSGBnmOTSnkct29710481 = -366771583;    float LwNSGBnmOTSnkct44942451 = -544366715;    float LwNSGBnmOTSnkct74156226 = -39441607;    float LwNSGBnmOTSnkct44435230 = -444690499;    float LwNSGBnmOTSnkct51328053 = -371638241;    float LwNSGBnmOTSnkct82624008 = -726825482;    float LwNSGBnmOTSnkct58674674 = -518469898;    float LwNSGBnmOTSnkct20566052 = -380709866;    float LwNSGBnmOTSnkct84148806 = -668467954;    float LwNSGBnmOTSnkct84582095 = -981810610;    float LwNSGBnmOTSnkct55622474 = -228930959;    float LwNSGBnmOTSnkct27476637 = -666324831;    float LwNSGBnmOTSnkct12210034 = -939427520;    float LwNSGBnmOTSnkct30909845 = -649264184;    float LwNSGBnmOTSnkct76269682 = -946080358;    float LwNSGBnmOTSnkct50229008 = -694975018;    float LwNSGBnmOTSnkct95261260 = -371499654;    float LwNSGBnmOTSnkct45058538 = -855124089;    float LwNSGBnmOTSnkct59866695 = -720455212;    float LwNSGBnmOTSnkct21473725 = -75370521;    float LwNSGBnmOTSnkct24812440 = 31703589;    float LwNSGBnmOTSnkct68378735 = -467817256;    float LwNSGBnmOTSnkct57047967 = 91555047;    float LwNSGBnmOTSnkct7323118 = -767166096;    float LwNSGBnmOTSnkct12079113 = -728979556;    float LwNSGBnmOTSnkct53545533 = -101975390;    float LwNSGBnmOTSnkct18795182 = -605127815;    float LwNSGBnmOTSnkct79263835 = -821574314;    float LwNSGBnmOTSnkct48518280 = -683391109;    float LwNSGBnmOTSnkct16485500 = -634745716;    float LwNSGBnmOTSnkct13024301 = -11777307;    float LwNSGBnmOTSnkct15270295 = -100709364;    float LwNSGBnmOTSnkct73434787 = -674063579;    float LwNSGBnmOTSnkct92966442 = -387496872;    float LwNSGBnmOTSnkct35537436 = -528046944;    float LwNSGBnmOTSnkct69762972 = -433945211;    float LwNSGBnmOTSnkct17711759 = -463466284;    float LwNSGBnmOTSnkct45859953 = -210290622;    float LwNSGBnmOTSnkct63310271 = -980712186;    float LwNSGBnmOTSnkct47456448 = -808422925;    float LwNSGBnmOTSnkct83839814 = -519023844;    float LwNSGBnmOTSnkct53510892 = -7119753;    float LwNSGBnmOTSnkct77144174 = -169254447;    float LwNSGBnmOTSnkct99487588 = -798064940;    float LwNSGBnmOTSnkct45996597 = -954806454;    float LwNSGBnmOTSnkct11608070 = -433121805;    float LwNSGBnmOTSnkct60664799 = -500907342;    float LwNSGBnmOTSnkct82695898 = -102193621;    float LwNSGBnmOTSnkct56989392 = -333114384;    float LwNSGBnmOTSnkct89641046 = -385121153;    float LwNSGBnmOTSnkct74235354 = -479781614;    float LwNSGBnmOTSnkct25523732 = -248781581;    float LwNSGBnmOTSnkct68709350 = -75255257;    float LwNSGBnmOTSnkct67773187 = -517401743;    float LwNSGBnmOTSnkct72955128 = -869614995;    float LwNSGBnmOTSnkct61331746 = -898954328;    float LwNSGBnmOTSnkct87894484 = -535921763;    float LwNSGBnmOTSnkct66833109 = -272275512;    float LwNSGBnmOTSnkct32356117 = -715710944;    float LwNSGBnmOTSnkct97782519 = -169662852;    float LwNSGBnmOTSnkct63828826 = -21697667;    float LwNSGBnmOTSnkct79410838 = -696895585;    float LwNSGBnmOTSnkct72047771 = -697318757;    float LwNSGBnmOTSnkct67663307 = 66277762;    float LwNSGBnmOTSnkct71557794 = -870033303;    float LwNSGBnmOTSnkct40352179 = -28221596;    float LwNSGBnmOTSnkct54041850 = -992261253;    float LwNSGBnmOTSnkct19243591 = -451930648;    float LwNSGBnmOTSnkct95372408 = -21217241;    float LwNSGBnmOTSnkct6506710 = -412135148;    float LwNSGBnmOTSnkct32517250 = -131508734;    float LwNSGBnmOTSnkct49401307 = -61209033;    float LwNSGBnmOTSnkct81748267 = -874411903;    float LwNSGBnmOTSnkct12410248 = -912032288;    float LwNSGBnmOTSnkct37633911 = -556346677;    float LwNSGBnmOTSnkct71301547 = -961176659;    float LwNSGBnmOTSnkct91234560 = -198562810;    float LwNSGBnmOTSnkct57560378 = -110380013;    float LwNSGBnmOTSnkct61326520 = -812359642;    float LwNSGBnmOTSnkct471044 = -195857751;    float LwNSGBnmOTSnkct92880734 = -601068048;    float LwNSGBnmOTSnkct36099283 = -402934195;    float LwNSGBnmOTSnkct22274444 = -388459930;    float LwNSGBnmOTSnkct58877234 = -198269956;    float LwNSGBnmOTSnkct42250145 = -54964102;    float LwNSGBnmOTSnkct87500569 = -762995727;    float LwNSGBnmOTSnkct46560944 = 74545893;    float LwNSGBnmOTSnkct5661600 = -56661837;    float LwNSGBnmOTSnkct20011314 = -517881877;    float LwNSGBnmOTSnkct74205690 = -629092617;    float LwNSGBnmOTSnkct81868487 = -440334862;     LwNSGBnmOTSnkct72874832 = LwNSGBnmOTSnkct13605744;     LwNSGBnmOTSnkct13605744 = LwNSGBnmOTSnkct33259074;     LwNSGBnmOTSnkct33259074 = LwNSGBnmOTSnkct39870055;     LwNSGBnmOTSnkct39870055 = LwNSGBnmOTSnkct69496615;     LwNSGBnmOTSnkct69496615 = LwNSGBnmOTSnkct70582270;     LwNSGBnmOTSnkct70582270 = LwNSGBnmOTSnkct28576046;     LwNSGBnmOTSnkct28576046 = LwNSGBnmOTSnkct89246912;     LwNSGBnmOTSnkct89246912 = LwNSGBnmOTSnkct97767568;     LwNSGBnmOTSnkct97767568 = LwNSGBnmOTSnkct29710481;     LwNSGBnmOTSnkct29710481 = LwNSGBnmOTSnkct44942451;     LwNSGBnmOTSnkct44942451 = LwNSGBnmOTSnkct74156226;     LwNSGBnmOTSnkct74156226 = LwNSGBnmOTSnkct44435230;     LwNSGBnmOTSnkct44435230 = LwNSGBnmOTSnkct51328053;     LwNSGBnmOTSnkct51328053 = LwNSGBnmOTSnkct82624008;     LwNSGBnmOTSnkct82624008 = LwNSGBnmOTSnkct58674674;     LwNSGBnmOTSnkct58674674 = LwNSGBnmOTSnkct20566052;     LwNSGBnmOTSnkct20566052 = LwNSGBnmOTSnkct84148806;     LwNSGBnmOTSnkct84148806 = LwNSGBnmOTSnkct84582095;     LwNSGBnmOTSnkct84582095 = LwNSGBnmOTSnkct55622474;     LwNSGBnmOTSnkct55622474 = LwNSGBnmOTSnkct27476637;     LwNSGBnmOTSnkct27476637 = LwNSGBnmOTSnkct12210034;     LwNSGBnmOTSnkct12210034 = LwNSGBnmOTSnkct30909845;     LwNSGBnmOTSnkct30909845 = LwNSGBnmOTSnkct76269682;     LwNSGBnmOTSnkct76269682 = LwNSGBnmOTSnkct50229008;     LwNSGBnmOTSnkct50229008 = LwNSGBnmOTSnkct95261260;     LwNSGBnmOTSnkct95261260 = LwNSGBnmOTSnkct45058538;     LwNSGBnmOTSnkct45058538 = LwNSGBnmOTSnkct59866695;     LwNSGBnmOTSnkct59866695 = LwNSGBnmOTSnkct21473725;     LwNSGBnmOTSnkct21473725 = LwNSGBnmOTSnkct24812440;     LwNSGBnmOTSnkct24812440 = LwNSGBnmOTSnkct68378735;     LwNSGBnmOTSnkct68378735 = LwNSGBnmOTSnkct57047967;     LwNSGBnmOTSnkct57047967 = LwNSGBnmOTSnkct7323118;     LwNSGBnmOTSnkct7323118 = LwNSGBnmOTSnkct12079113;     LwNSGBnmOTSnkct12079113 = LwNSGBnmOTSnkct53545533;     LwNSGBnmOTSnkct53545533 = LwNSGBnmOTSnkct18795182;     LwNSGBnmOTSnkct18795182 = LwNSGBnmOTSnkct79263835;     LwNSGBnmOTSnkct79263835 = LwNSGBnmOTSnkct48518280;     LwNSGBnmOTSnkct48518280 = LwNSGBnmOTSnkct16485500;     LwNSGBnmOTSnkct16485500 = LwNSGBnmOTSnkct13024301;     LwNSGBnmOTSnkct13024301 = LwNSGBnmOTSnkct15270295;     LwNSGBnmOTSnkct15270295 = LwNSGBnmOTSnkct73434787;     LwNSGBnmOTSnkct73434787 = LwNSGBnmOTSnkct92966442;     LwNSGBnmOTSnkct92966442 = LwNSGBnmOTSnkct35537436;     LwNSGBnmOTSnkct35537436 = LwNSGBnmOTSnkct69762972;     LwNSGBnmOTSnkct69762972 = LwNSGBnmOTSnkct17711759;     LwNSGBnmOTSnkct17711759 = LwNSGBnmOTSnkct45859953;     LwNSGBnmOTSnkct45859953 = LwNSGBnmOTSnkct63310271;     LwNSGBnmOTSnkct63310271 = LwNSGBnmOTSnkct47456448;     LwNSGBnmOTSnkct47456448 = LwNSGBnmOTSnkct83839814;     LwNSGBnmOTSnkct83839814 = LwNSGBnmOTSnkct53510892;     LwNSGBnmOTSnkct53510892 = LwNSGBnmOTSnkct77144174;     LwNSGBnmOTSnkct77144174 = LwNSGBnmOTSnkct99487588;     LwNSGBnmOTSnkct99487588 = LwNSGBnmOTSnkct45996597;     LwNSGBnmOTSnkct45996597 = LwNSGBnmOTSnkct11608070;     LwNSGBnmOTSnkct11608070 = LwNSGBnmOTSnkct60664799;     LwNSGBnmOTSnkct60664799 = LwNSGBnmOTSnkct82695898;     LwNSGBnmOTSnkct82695898 = LwNSGBnmOTSnkct56989392;     LwNSGBnmOTSnkct56989392 = LwNSGBnmOTSnkct89641046;     LwNSGBnmOTSnkct89641046 = LwNSGBnmOTSnkct74235354;     LwNSGBnmOTSnkct74235354 = LwNSGBnmOTSnkct25523732;     LwNSGBnmOTSnkct25523732 = LwNSGBnmOTSnkct68709350;     LwNSGBnmOTSnkct68709350 = LwNSGBnmOTSnkct67773187;     LwNSGBnmOTSnkct67773187 = LwNSGBnmOTSnkct72955128;     LwNSGBnmOTSnkct72955128 = LwNSGBnmOTSnkct61331746;     LwNSGBnmOTSnkct61331746 = LwNSGBnmOTSnkct87894484;     LwNSGBnmOTSnkct87894484 = LwNSGBnmOTSnkct66833109;     LwNSGBnmOTSnkct66833109 = LwNSGBnmOTSnkct32356117;     LwNSGBnmOTSnkct32356117 = LwNSGBnmOTSnkct97782519;     LwNSGBnmOTSnkct97782519 = LwNSGBnmOTSnkct63828826;     LwNSGBnmOTSnkct63828826 = LwNSGBnmOTSnkct79410838;     LwNSGBnmOTSnkct79410838 = LwNSGBnmOTSnkct72047771;     LwNSGBnmOTSnkct72047771 = LwNSGBnmOTSnkct67663307;     LwNSGBnmOTSnkct67663307 = LwNSGBnmOTSnkct71557794;     LwNSGBnmOTSnkct71557794 = LwNSGBnmOTSnkct40352179;     LwNSGBnmOTSnkct40352179 = LwNSGBnmOTSnkct54041850;     LwNSGBnmOTSnkct54041850 = LwNSGBnmOTSnkct19243591;     LwNSGBnmOTSnkct19243591 = LwNSGBnmOTSnkct95372408;     LwNSGBnmOTSnkct95372408 = LwNSGBnmOTSnkct6506710;     LwNSGBnmOTSnkct6506710 = LwNSGBnmOTSnkct32517250;     LwNSGBnmOTSnkct32517250 = LwNSGBnmOTSnkct49401307;     LwNSGBnmOTSnkct49401307 = LwNSGBnmOTSnkct81748267;     LwNSGBnmOTSnkct81748267 = LwNSGBnmOTSnkct12410248;     LwNSGBnmOTSnkct12410248 = LwNSGBnmOTSnkct37633911;     LwNSGBnmOTSnkct37633911 = LwNSGBnmOTSnkct71301547;     LwNSGBnmOTSnkct71301547 = LwNSGBnmOTSnkct91234560;     LwNSGBnmOTSnkct91234560 = LwNSGBnmOTSnkct57560378;     LwNSGBnmOTSnkct57560378 = LwNSGBnmOTSnkct61326520;     LwNSGBnmOTSnkct61326520 = LwNSGBnmOTSnkct471044;     LwNSGBnmOTSnkct471044 = LwNSGBnmOTSnkct92880734;     LwNSGBnmOTSnkct92880734 = LwNSGBnmOTSnkct36099283;     LwNSGBnmOTSnkct36099283 = LwNSGBnmOTSnkct22274444;     LwNSGBnmOTSnkct22274444 = LwNSGBnmOTSnkct58877234;     LwNSGBnmOTSnkct58877234 = LwNSGBnmOTSnkct42250145;     LwNSGBnmOTSnkct42250145 = LwNSGBnmOTSnkct87500569;     LwNSGBnmOTSnkct87500569 = LwNSGBnmOTSnkct46560944;     LwNSGBnmOTSnkct46560944 = LwNSGBnmOTSnkct5661600;     LwNSGBnmOTSnkct5661600 = LwNSGBnmOTSnkct20011314;     LwNSGBnmOTSnkct20011314 = LwNSGBnmOTSnkct74205690;     LwNSGBnmOTSnkct74205690 = LwNSGBnmOTSnkct81868487;     LwNSGBnmOTSnkct81868487 = LwNSGBnmOTSnkct72874832;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void JPdItWvuDxaUOjqD93500919() {     float FMNIbQKTbNBESxn43526499 = 41516006;    float FMNIbQKTbNBESxn82279260 = -723101110;    float FMNIbQKTbNBESxn13227993 = -721489201;    float FMNIbQKTbNBESxn79801341 = -270234557;    float FMNIbQKTbNBESxn90946936 = -734687623;    float FMNIbQKTbNBESxn60582032 = -905486821;    float FMNIbQKTbNBESxn46954941 = -894486179;    float FMNIbQKTbNBESxn46089293 = 60410631;    float FMNIbQKTbNBESxn43632623 = 68741264;    float FMNIbQKTbNBESxn52676942 = -667866933;    float FMNIbQKTbNBESxn87191611 = -579268645;    float FMNIbQKTbNBESxn19702798 = -355205409;    float FMNIbQKTbNBESxn24262617 = -354227557;    float FMNIbQKTbNBESxn35175086 = -172666452;    float FMNIbQKTbNBESxn65344027 = -876575504;    float FMNIbQKTbNBESxn91203315 = -88453485;    float FMNIbQKTbNBESxn14418879 = -289894996;    float FMNIbQKTbNBESxn4442302 = -657634154;    float FMNIbQKTbNBESxn66289877 = -588998551;    float FMNIbQKTbNBESxn14409183 = -771297264;    float FMNIbQKTbNBESxn82612391 = -575003977;    float FMNIbQKTbNBESxn73605798 = -170712825;    float FMNIbQKTbNBESxn32853654 = -117107329;    float FMNIbQKTbNBESxn10119219 = -538481264;    float FMNIbQKTbNBESxn14353397 = -370879718;    float FMNIbQKTbNBESxn25858842 = -469784095;    float FMNIbQKTbNBESxn53319324 = -150301864;    float FMNIbQKTbNBESxn67648920 = -125578986;    float FMNIbQKTbNBESxn53616213 = -369728722;    float FMNIbQKTbNBESxn63984536 = -526691482;    float FMNIbQKTbNBESxn75450731 = -216250941;    float FMNIbQKTbNBESxn1559428 = -236742112;    float FMNIbQKTbNBESxn90268478 = -714127701;    float FMNIbQKTbNBESxn77444087 = -292509240;    float FMNIbQKTbNBESxn31569769 = 51077834;    float FMNIbQKTbNBESxn78788441 = -684354345;    float FMNIbQKTbNBESxn23055322 = 89832478;    float FMNIbQKTbNBESxn1310769 = -467504220;    float FMNIbQKTbNBESxn95018788 = -590474340;    float FMNIbQKTbNBESxn8127599 = -629385670;    float FMNIbQKTbNBESxn75343578 = -484728242;    float FMNIbQKTbNBESxn1917779 = -443452628;    float FMNIbQKTbNBESxn41933402 = -374250922;    float FMNIbQKTbNBESxn42045737 = -600021915;    float FMNIbQKTbNBESxn65717031 = -742591604;    float FMNIbQKTbNBESxn24117593 = -221803421;    float FMNIbQKTbNBESxn71991156 = -326360421;    float FMNIbQKTbNBESxn28924481 = -332842480;    float FMNIbQKTbNBESxn52897900 = -794186870;    float FMNIbQKTbNBESxn92267300 = -802032728;    float FMNIbQKTbNBESxn21700217 = -653691874;    float FMNIbQKTbNBESxn19015679 = -138474164;    float FMNIbQKTbNBESxn54563245 = -725862985;    float FMNIbQKTbNBESxn36223901 = -515687950;    float FMNIbQKTbNBESxn10888528 = -544062714;    float FMNIbQKTbNBESxn69920701 = -787771169;    float FMNIbQKTbNBESxn49425607 = -505993782;    float FMNIbQKTbNBESxn3108775 = -83007938;    float FMNIbQKTbNBESxn65447944 = -899354839;    float FMNIbQKTbNBESxn65088095 = -164903529;    float FMNIbQKTbNBESxn7262709 = -655184957;    float FMNIbQKTbNBESxn79306021 = -668907193;    float FMNIbQKTbNBESxn92473080 = -569860647;    float FMNIbQKTbNBESxn79648087 = -404567255;    float FMNIbQKTbNBESxn77226210 = -351615993;    float FMNIbQKTbNBESxn85632184 = -242526533;    float FMNIbQKTbNBESxn29434320 = -641077708;    float FMNIbQKTbNBESxn46818529 = 38281682;    float FMNIbQKTbNBESxn3605318 = -123744286;    float FMNIbQKTbNBESxn86555586 = -92221160;    float FMNIbQKTbNBESxn68147993 = -78285964;    float FMNIbQKTbNBESxn13108110 = -822390776;    float FMNIbQKTbNBESxn9423513 = 32840186;    float FMNIbQKTbNBESxn58162279 = -959612881;    float FMNIbQKTbNBESxn39065605 = -186569022;    float FMNIbQKTbNBESxn80694613 = -31551349;    float FMNIbQKTbNBESxn31672396 = -796461903;    float FMNIbQKTbNBESxn90807917 = -517085414;    float FMNIbQKTbNBESxn44402188 = -795889660;    float FMNIbQKTbNBESxn90235803 = -49076298;    float FMNIbQKTbNBESxn53867686 = -43423674;    float FMNIbQKTbNBESxn24394844 = -817459384;    float FMNIbQKTbNBESxn14751021 = -331392117;    float FMNIbQKTbNBESxn61348912 = -567695995;    float FMNIbQKTbNBESxn42284319 = -872999608;    float FMNIbQKTbNBESxn56435053 = 22223223;    float FMNIbQKTbNBESxn46996182 = -510879128;    float FMNIbQKTbNBESxn54044577 = -98439752;    float FMNIbQKTbNBESxn66555559 = -748446527;    float FMNIbQKTbNBESxn61649068 = -161150997;    float FMNIbQKTbNBESxn29362834 = -78360563;    float FMNIbQKTbNBESxn19946548 = -827159584;    float FMNIbQKTbNBESxn35862825 = -568149381;    float FMNIbQKTbNBESxn29930694 = -325570812;    float FMNIbQKTbNBESxn864890 = -974200713;    float FMNIbQKTbNBESxn96037557 = -815821049;    float FMNIbQKTbNBESxn9444699 = -873591982;    float FMNIbQKTbNBESxn62285315 = -969683668;    float FMNIbQKTbNBESxn64819526 = -148405923;    float FMNIbQKTbNBESxn80084846 = 41516006;     FMNIbQKTbNBESxn43526499 = FMNIbQKTbNBESxn82279260;     FMNIbQKTbNBESxn82279260 = FMNIbQKTbNBESxn13227993;     FMNIbQKTbNBESxn13227993 = FMNIbQKTbNBESxn79801341;     FMNIbQKTbNBESxn79801341 = FMNIbQKTbNBESxn90946936;     FMNIbQKTbNBESxn90946936 = FMNIbQKTbNBESxn60582032;     FMNIbQKTbNBESxn60582032 = FMNIbQKTbNBESxn46954941;     FMNIbQKTbNBESxn46954941 = FMNIbQKTbNBESxn46089293;     FMNIbQKTbNBESxn46089293 = FMNIbQKTbNBESxn43632623;     FMNIbQKTbNBESxn43632623 = FMNIbQKTbNBESxn52676942;     FMNIbQKTbNBESxn52676942 = FMNIbQKTbNBESxn87191611;     FMNIbQKTbNBESxn87191611 = FMNIbQKTbNBESxn19702798;     FMNIbQKTbNBESxn19702798 = FMNIbQKTbNBESxn24262617;     FMNIbQKTbNBESxn24262617 = FMNIbQKTbNBESxn35175086;     FMNIbQKTbNBESxn35175086 = FMNIbQKTbNBESxn65344027;     FMNIbQKTbNBESxn65344027 = FMNIbQKTbNBESxn91203315;     FMNIbQKTbNBESxn91203315 = FMNIbQKTbNBESxn14418879;     FMNIbQKTbNBESxn14418879 = FMNIbQKTbNBESxn4442302;     FMNIbQKTbNBESxn4442302 = FMNIbQKTbNBESxn66289877;     FMNIbQKTbNBESxn66289877 = FMNIbQKTbNBESxn14409183;     FMNIbQKTbNBESxn14409183 = FMNIbQKTbNBESxn82612391;     FMNIbQKTbNBESxn82612391 = FMNIbQKTbNBESxn73605798;     FMNIbQKTbNBESxn73605798 = FMNIbQKTbNBESxn32853654;     FMNIbQKTbNBESxn32853654 = FMNIbQKTbNBESxn10119219;     FMNIbQKTbNBESxn10119219 = FMNIbQKTbNBESxn14353397;     FMNIbQKTbNBESxn14353397 = FMNIbQKTbNBESxn25858842;     FMNIbQKTbNBESxn25858842 = FMNIbQKTbNBESxn53319324;     FMNIbQKTbNBESxn53319324 = FMNIbQKTbNBESxn67648920;     FMNIbQKTbNBESxn67648920 = FMNIbQKTbNBESxn53616213;     FMNIbQKTbNBESxn53616213 = FMNIbQKTbNBESxn63984536;     FMNIbQKTbNBESxn63984536 = FMNIbQKTbNBESxn75450731;     FMNIbQKTbNBESxn75450731 = FMNIbQKTbNBESxn1559428;     FMNIbQKTbNBESxn1559428 = FMNIbQKTbNBESxn90268478;     FMNIbQKTbNBESxn90268478 = FMNIbQKTbNBESxn77444087;     FMNIbQKTbNBESxn77444087 = FMNIbQKTbNBESxn31569769;     FMNIbQKTbNBESxn31569769 = FMNIbQKTbNBESxn78788441;     FMNIbQKTbNBESxn78788441 = FMNIbQKTbNBESxn23055322;     FMNIbQKTbNBESxn23055322 = FMNIbQKTbNBESxn1310769;     FMNIbQKTbNBESxn1310769 = FMNIbQKTbNBESxn95018788;     FMNIbQKTbNBESxn95018788 = FMNIbQKTbNBESxn8127599;     FMNIbQKTbNBESxn8127599 = FMNIbQKTbNBESxn75343578;     FMNIbQKTbNBESxn75343578 = FMNIbQKTbNBESxn1917779;     FMNIbQKTbNBESxn1917779 = FMNIbQKTbNBESxn41933402;     FMNIbQKTbNBESxn41933402 = FMNIbQKTbNBESxn42045737;     FMNIbQKTbNBESxn42045737 = FMNIbQKTbNBESxn65717031;     FMNIbQKTbNBESxn65717031 = FMNIbQKTbNBESxn24117593;     FMNIbQKTbNBESxn24117593 = FMNIbQKTbNBESxn71991156;     FMNIbQKTbNBESxn71991156 = FMNIbQKTbNBESxn28924481;     FMNIbQKTbNBESxn28924481 = FMNIbQKTbNBESxn52897900;     FMNIbQKTbNBESxn52897900 = FMNIbQKTbNBESxn92267300;     FMNIbQKTbNBESxn92267300 = FMNIbQKTbNBESxn21700217;     FMNIbQKTbNBESxn21700217 = FMNIbQKTbNBESxn19015679;     FMNIbQKTbNBESxn19015679 = FMNIbQKTbNBESxn54563245;     FMNIbQKTbNBESxn54563245 = FMNIbQKTbNBESxn36223901;     FMNIbQKTbNBESxn36223901 = FMNIbQKTbNBESxn10888528;     FMNIbQKTbNBESxn10888528 = FMNIbQKTbNBESxn69920701;     FMNIbQKTbNBESxn69920701 = FMNIbQKTbNBESxn49425607;     FMNIbQKTbNBESxn49425607 = FMNIbQKTbNBESxn3108775;     FMNIbQKTbNBESxn3108775 = FMNIbQKTbNBESxn65447944;     FMNIbQKTbNBESxn65447944 = FMNIbQKTbNBESxn65088095;     FMNIbQKTbNBESxn65088095 = FMNIbQKTbNBESxn7262709;     FMNIbQKTbNBESxn7262709 = FMNIbQKTbNBESxn79306021;     FMNIbQKTbNBESxn79306021 = FMNIbQKTbNBESxn92473080;     FMNIbQKTbNBESxn92473080 = FMNIbQKTbNBESxn79648087;     FMNIbQKTbNBESxn79648087 = FMNIbQKTbNBESxn77226210;     FMNIbQKTbNBESxn77226210 = FMNIbQKTbNBESxn85632184;     FMNIbQKTbNBESxn85632184 = FMNIbQKTbNBESxn29434320;     FMNIbQKTbNBESxn29434320 = FMNIbQKTbNBESxn46818529;     FMNIbQKTbNBESxn46818529 = FMNIbQKTbNBESxn3605318;     FMNIbQKTbNBESxn3605318 = FMNIbQKTbNBESxn86555586;     FMNIbQKTbNBESxn86555586 = FMNIbQKTbNBESxn68147993;     FMNIbQKTbNBESxn68147993 = FMNIbQKTbNBESxn13108110;     FMNIbQKTbNBESxn13108110 = FMNIbQKTbNBESxn9423513;     FMNIbQKTbNBESxn9423513 = FMNIbQKTbNBESxn58162279;     FMNIbQKTbNBESxn58162279 = FMNIbQKTbNBESxn39065605;     FMNIbQKTbNBESxn39065605 = FMNIbQKTbNBESxn80694613;     FMNIbQKTbNBESxn80694613 = FMNIbQKTbNBESxn31672396;     FMNIbQKTbNBESxn31672396 = FMNIbQKTbNBESxn90807917;     FMNIbQKTbNBESxn90807917 = FMNIbQKTbNBESxn44402188;     FMNIbQKTbNBESxn44402188 = FMNIbQKTbNBESxn90235803;     FMNIbQKTbNBESxn90235803 = FMNIbQKTbNBESxn53867686;     FMNIbQKTbNBESxn53867686 = FMNIbQKTbNBESxn24394844;     FMNIbQKTbNBESxn24394844 = FMNIbQKTbNBESxn14751021;     FMNIbQKTbNBESxn14751021 = FMNIbQKTbNBESxn61348912;     FMNIbQKTbNBESxn61348912 = FMNIbQKTbNBESxn42284319;     FMNIbQKTbNBESxn42284319 = FMNIbQKTbNBESxn56435053;     FMNIbQKTbNBESxn56435053 = FMNIbQKTbNBESxn46996182;     FMNIbQKTbNBESxn46996182 = FMNIbQKTbNBESxn54044577;     FMNIbQKTbNBESxn54044577 = FMNIbQKTbNBESxn66555559;     FMNIbQKTbNBESxn66555559 = FMNIbQKTbNBESxn61649068;     FMNIbQKTbNBESxn61649068 = FMNIbQKTbNBESxn29362834;     FMNIbQKTbNBESxn29362834 = FMNIbQKTbNBESxn19946548;     FMNIbQKTbNBESxn19946548 = FMNIbQKTbNBESxn35862825;     FMNIbQKTbNBESxn35862825 = FMNIbQKTbNBESxn29930694;     FMNIbQKTbNBESxn29930694 = FMNIbQKTbNBESxn864890;     FMNIbQKTbNBESxn864890 = FMNIbQKTbNBESxn96037557;     FMNIbQKTbNBESxn96037557 = FMNIbQKTbNBESxn9444699;     FMNIbQKTbNBESxn9444699 = FMNIbQKTbNBESxn62285315;     FMNIbQKTbNBESxn62285315 = FMNIbQKTbNBESxn64819526;     FMNIbQKTbNBESxn64819526 = FMNIbQKTbNBESxn80084846;     FMNIbQKTbNBESxn80084846 = FMNIbQKTbNBESxn43526499;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void lOUjEHvMaZvpNEaN99829467() {     float IYbTPxNfBDDjODb98180896 = -396011576;    float IYbTPxNfBDDjODb29276881 = -202665918;    float IYbTPxNfBDDjODb73960639 = -6873815;    float IYbTPxNfBDDjODb51236283 = -276336971;    float IYbTPxNfBDDjODb65398607 = -407122854;    float IYbTPxNfBDDjODb74756429 = 65362343;    float IYbTPxNfBDDjODb68055645 = -96378851;    float IYbTPxNfBDDjODb75264397 = -440157586;    float IYbTPxNfBDDjODb31407034 = 21887895;    float IYbTPxNfBDDjODb35308927 = -946668203;    float IYbTPxNfBDDjODb65802907 = -104625216;    float IYbTPxNfBDDjODb82467511 = -541629489;    float IYbTPxNfBDDjODb88079931 = -157767862;    float IYbTPxNfBDDjODb28141517 = -884770029;    float IYbTPxNfBDDjODb95568100 = -783697117;    float IYbTPxNfBDDjODb80970850 = -818613049;    float IYbTPxNfBDDjODb14250642 = -763494824;    float IYbTPxNfBDDjODb72038888 = -721198617;    float IYbTPxNfBDDjODb67510477 = -874810100;    float IYbTPxNfBDDjODb32043738 = -16663410;    float IYbTPxNfBDDjODb16786780 = -43087123;    float IYbTPxNfBDDjODb24858387 = -337156933;    float IYbTPxNfBDDjODb69323485 = -703850750;    float IYbTPxNfBDDjODb11547588 = -237522081;    float IYbTPxNfBDDjODb10106013 = -743793158;    float IYbTPxNfBDDjODb67099788 = -473492883;    float IYbTPxNfBDDjODb55309248 = -705146143;    float IYbTPxNfBDDjODb20897921 = -133630841;    float IYbTPxNfBDDjODb93513890 = -749973613;    float IYbTPxNfBDDjODb58419945 = -817449450;    float IYbTPxNfBDDjODb27156260 = -765123467;    float IYbTPxNfBDDjODb32551038 = -648238644;    float IYbTPxNfBDDjODb57924354 = -170645319;    float IYbTPxNfBDDjODb72099980 = 27369951;    float IYbTPxNfBDDjODb66073926 = -166785429;    float IYbTPxNfBDDjODb66114982 = -932349960;    float IYbTPxNfBDDjODb97759809 = -860594142;    float IYbTPxNfBDDjODb14288008 = -341433238;    float IYbTPxNfBDDjODb45788437 = -972422954;    float IYbTPxNfBDDjODb32113994 = -287694269;    float IYbTPxNfBDDjODb31302979 = -597278400;    float IYbTPxNfBDDjODb38592437 = 49015265;    float IYbTPxNfBDDjODb32933585 = -214756242;    float IYbTPxNfBDDjODb51040136 = -73701581;    float IYbTPxNfBDDjODb25701094 = -875109203;    float IYbTPxNfBDDjODb79451195 = -290593054;    float IYbTPxNfBDDjODb88464868 = -353417805;    float IYbTPxNfBDDjODb52351790 = -310494854;    float IYbTPxNfBDDjODb95240666 = -693917083;    float IYbTPxNfBDDjODb37108964 = 57947983;    float IYbTPxNfBDDjODb96813756 = -310907376;    float IYbTPxNfBDDjODb8397829 = -653540151;    float IYbTPxNfBDDjODb86291248 = -465778439;    float IYbTPxNfBDDjODb93639658 = -411574435;    float IYbTPxNfBDDjODb83042687 = -939971400;    float IYbTPxNfBDDjODb73322509 = 41145357;    float IYbTPxNfBDDjODb59953395 = -498815169;    float IYbTPxNfBDDjODb62413052 = -769351735;    float IYbTPxNfBDDjODb41130270 = -532543814;    float IYbTPxNfBDDjODb98298818 = -933629971;    float IYbTPxNfBDDjODb19447182 = -229491514;    float IYbTPxNfBDDjODb47157724 = -962748011;    float IYbTPxNfBDDjODb81750507 = -690183973;    float IYbTPxNfBDDjODb72987088 = -160662656;    float IYbTPxNfBDDjODb8152668 = -81544737;    float IYbTPxNfBDDjODb33251869 = -456386573;    float IYbTPxNfBDDjODb24543157 = -270984170;    float IYbTPxNfBDDjODb15979952 = -85137813;    float IYbTPxNfBDDjODb62067591 = -617984601;    float IYbTPxNfBDDjODb29453119 = -851347158;    float IYbTPxNfBDDjODb83211041 = -958018908;    float IYbTPxNfBDDjODb99962634 = -322061587;    float IYbTPxNfBDDjODb26250451 = -748775663;    float IYbTPxNfBDDjODb35396484 = -487115831;    float IYbTPxNfBDDjODb740760 = -419385010;    float IYbTPxNfBDDjODb78194342 = 7897612;    float IYbTPxNfBDDjODb91924801 = -22400691;    float IYbTPxNfBDDjODb18283349 = -530149170;    float IYbTPxNfBDDjODb85846493 = -362412878;    float IYbTPxNfBDDjODb30654818 = -353200104;    float IYbTPxNfBDDjODb78634920 = -20075079;    float IYbTPxNfBDDjODb2957459 = -294651290;    float IYbTPxNfBDDjODb25657254 = -439713758;    float IYbTPxNfBDDjODb56404926 = -707921597;    float IYbTPxNfBDDjODb61606188 = -406542075;    float IYbTPxNfBDDjODb18758432 = -11583316;    float IYbTPxNfBDDjODb46259789 = -82460205;    float IYbTPxNfBDDjODb64284696 = -759070884;    float IYbTPxNfBDDjODb89057292 = -32658650;    float IYbTPxNfBDDjODb92751416 = -107930786;    float IYbTPxNfBDDjODb6161588 = -333534792;    float IYbTPxNfBDDjODb35346758 = 8757592;    float IYbTPxNfBDDjODb73157738 = -808889424;    float IYbTPxNfBDDjODb47489618 = 61207017;    float IYbTPxNfBDDjODb12666813 = 41797244;    float IYbTPxNfBDDjODb84145254 = -634530389;    float IYbTPxNfBDDjODb56841930 = -260800763;    float IYbTPxNfBDDjODb59946496 = 45906413;    float IYbTPxNfBDDjODb42887469 = -992156845;    float IYbTPxNfBDDjODb92449225 = -396011576;     IYbTPxNfBDDjODb98180896 = IYbTPxNfBDDjODb29276881;     IYbTPxNfBDDjODb29276881 = IYbTPxNfBDDjODb73960639;     IYbTPxNfBDDjODb73960639 = IYbTPxNfBDDjODb51236283;     IYbTPxNfBDDjODb51236283 = IYbTPxNfBDDjODb65398607;     IYbTPxNfBDDjODb65398607 = IYbTPxNfBDDjODb74756429;     IYbTPxNfBDDjODb74756429 = IYbTPxNfBDDjODb68055645;     IYbTPxNfBDDjODb68055645 = IYbTPxNfBDDjODb75264397;     IYbTPxNfBDDjODb75264397 = IYbTPxNfBDDjODb31407034;     IYbTPxNfBDDjODb31407034 = IYbTPxNfBDDjODb35308927;     IYbTPxNfBDDjODb35308927 = IYbTPxNfBDDjODb65802907;     IYbTPxNfBDDjODb65802907 = IYbTPxNfBDDjODb82467511;     IYbTPxNfBDDjODb82467511 = IYbTPxNfBDDjODb88079931;     IYbTPxNfBDDjODb88079931 = IYbTPxNfBDDjODb28141517;     IYbTPxNfBDDjODb28141517 = IYbTPxNfBDDjODb95568100;     IYbTPxNfBDDjODb95568100 = IYbTPxNfBDDjODb80970850;     IYbTPxNfBDDjODb80970850 = IYbTPxNfBDDjODb14250642;     IYbTPxNfBDDjODb14250642 = IYbTPxNfBDDjODb72038888;     IYbTPxNfBDDjODb72038888 = IYbTPxNfBDDjODb67510477;     IYbTPxNfBDDjODb67510477 = IYbTPxNfBDDjODb32043738;     IYbTPxNfBDDjODb32043738 = IYbTPxNfBDDjODb16786780;     IYbTPxNfBDDjODb16786780 = IYbTPxNfBDDjODb24858387;     IYbTPxNfBDDjODb24858387 = IYbTPxNfBDDjODb69323485;     IYbTPxNfBDDjODb69323485 = IYbTPxNfBDDjODb11547588;     IYbTPxNfBDDjODb11547588 = IYbTPxNfBDDjODb10106013;     IYbTPxNfBDDjODb10106013 = IYbTPxNfBDDjODb67099788;     IYbTPxNfBDDjODb67099788 = IYbTPxNfBDDjODb55309248;     IYbTPxNfBDDjODb55309248 = IYbTPxNfBDDjODb20897921;     IYbTPxNfBDDjODb20897921 = IYbTPxNfBDDjODb93513890;     IYbTPxNfBDDjODb93513890 = IYbTPxNfBDDjODb58419945;     IYbTPxNfBDDjODb58419945 = IYbTPxNfBDDjODb27156260;     IYbTPxNfBDDjODb27156260 = IYbTPxNfBDDjODb32551038;     IYbTPxNfBDDjODb32551038 = IYbTPxNfBDDjODb57924354;     IYbTPxNfBDDjODb57924354 = IYbTPxNfBDDjODb72099980;     IYbTPxNfBDDjODb72099980 = IYbTPxNfBDDjODb66073926;     IYbTPxNfBDDjODb66073926 = IYbTPxNfBDDjODb66114982;     IYbTPxNfBDDjODb66114982 = IYbTPxNfBDDjODb97759809;     IYbTPxNfBDDjODb97759809 = IYbTPxNfBDDjODb14288008;     IYbTPxNfBDDjODb14288008 = IYbTPxNfBDDjODb45788437;     IYbTPxNfBDDjODb45788437 = IYbTPxNfBDDjODb32113994;     IYbTPxNfBDDjODb32113994 = IYbTPxNfBDDjODb31302979;     IYbTPxNfBDDjODb31302979 = IYbTPxNfBDDjODb38592437;     IYbTPxNfBDDjODb38592437 = IYbTPxNfBDDjODb32933585;     IYbTPxNfBDDjODb32933585 = IYbTPxNfBDDjODb51040136;     IYbTPxNfBDDjODb51040136 = IYbTPxNfBDDjODb25701094;     IYbTPxNfBDDjODb25701094 = IYbTPxNfBDDjODb79451195;     IYbTPxNfBDDjODb79451195 = IYbTPxNfBDDjODb88464868;     IYbTPxNfBDDjODb88464868 = IYbTPxNfBDDjODb52351790;     IYbTPxNfBDDjODb52351790 = IYbTPxNfBDDjODb95240666;     IYbTPxNfBDDjODb95240666 = IYbTPxNfBDDjODb37108964;     IYbTPxNfBDDjODb37108964 = IYbTPxNfBDDjODb96813756;     IYbTPxNfBDDjODb96813756 = IYbTPxNfBDDjODb8397829;     IYbTPxNfBDDjODb8397829 = IYbTPxNfBDDjODb86291248;     IYbTPxNfBDDjODb86291248 = IYbTPxNfBDDjODb93639658;     IYbTPxNfBDDjODb93639658 = IYbTPxNfBDDjODb83042687;     IYbTPxNfBDDjODb83042687 = IYbTPxNfBDDjODb73322509;     IYbTPxNfBDDjODb73322509 = IYbTPxNfBDDjODb59953395;     IYbTPxNfBDDjODb59953395 = IYbTPxNfBDDjODb62413052;     IYbTPxNfBDDjODb62413052 = IYbTPxNfBDDjODb41130270;     IYbTPxNfBDDjODb41130270 = IYbTPxNfBDDjODb98298818;     IYbTPxNfBDDjODb98298818 = IYbTPxNfBDDjODb19447182;     IYbTPxNfBDDjODb19447182 = IYbTPxNfBDDjODb47157724;     IYbTPxNfBDDjODb47157724 = IYbTPxNfBDDjODb81750507;     IYbTPxNfBDDjODb81750507 = IYbTPxNfBDDjODb72987088;     IYbTPxNfBDDjODb72987088 = IYbTPxNfBDDjODb8152668;     IYbTPxNfBDDjODb8152668 = IYbTPxNfBDDjODb33251869;     IYbTPxNfBDDjODb33251869 = IYbTPxNfBDDjODb24543157;     IYbTPxNfBDDjODb24543157 = IYbTPxNfBDDjODb15979952;     IYbTPxNfBDDjODb15979952 = IYbTPxNfBDDjODb62067591;     IYbTPxNfBDDjODb62067591 = IYbTPxNfBDDjODb29453119;     IYbTPxNfBDDjODb29453119 = IYbTPxNfBDDjODb83211041;     IYbTPxNfBDDjODb83211041 = IYbTPxNfBDDjODb99962634;     IYbTPxNfBDDjODb99962634 = IYbTPxNfBDDjODb26250451;     IYbTPxNfBDDjODb26250451 = IYbTPxNfBDDjODb35396484;     IYbTPxNfBDDjODb35396484 = IYbTPxNfBDDjODb740760;     IYbTPxNfBDDjODb740760 = IYbTPxNfBDDjODb78194342;     IYbTPxNfBDDjODb78194342 = IYbTPxNfBDDjODb91924801;     IYbTPxNfBDDjODb91924801 = IYbTPxNfBDDjODb18283349;     IYbTPxNfBDDjODb18283349 = IYbTPxNfBDDjODb85846493;     IYbTPxNfBDDjODb85846493 = IYbTPxNfBDDjODb30654818;     IYbTPxNfBDDjODb30654818 = IYbTPxNfBDDjODb78634920;     IYbTPxNfBDDjODb78634920 = IYbTPxNfBDDjODb2957459;     IYbTPxNfBDDjODb2957459 = IYbTPxNfBDDjODb25657254;     IYbTPxNfBDDjODb25657254 = IYbTPxNfBDDjODb56404926;     IYbTPxNfBDDjODb56404926 = IYbTPxNfBDDjODb61606188;     IYbTPxNfBDDjODb61606188 = IYbTPxNfBDDjODb18758432;     IYbTPxNfBDDjODb18758432 = IYbTPxNfBDDjODb46259789;     IYbTPxNfBDDjODb46259789 = IYbTPxNfBDDjODb64284696;     IYbTPxNfBDDjODb64284696 = IYbTPxNfBDDjODb89057292;     IYbTPxNfBDDjODb89057292 = IYbTPxNfBDDjODb92751416;     IYbTPxNfBDDjODb92751416 = IYbTPxNfBDDjODb6161588;     IYbTPxNfBDDjODb6161588 = IYbTPxNfBDDjODb35346758;     IYbTPxNfBDDjODb35346758 = IYbTPxNfBDDjODb73157738;     IYbTPxNfBDDjODb73157738 = IYbTPxNfBDDjODb47489618;     IYbTPxNfBDDjODb47489618 = IYbTPxNfBDDjODb12666813;     IYbTPxNfBDDjODb12666813 = IYbTPxNfBDDjODb84145254;     IYbTPxNfBDDjODb84145254 = IYbTPxNfBDDjODb56841930;     IYbTPxNfBDDjODb56841930 = IYbTPxNfBDDjODb59946496;     IYbTPxNfBDDjODb59946496 = IYbTPxNfBDDjODb42887469;     IYbTPxNfBDDjODb42887469 = IYbTPxNfBDDjODb92449225;     IYbTPxNfBDDjODb92449225 = IYbTPxNfBDDjODb98180896;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void zJgTydCPJxbsQsCs45525160() {     float tzrqbjfocgzPwni17369854 = -239786401;    float tzrqbjfocgzPwni1574738 = -944274251;    float tzrqbjfocgzPwni98620021 = -683719967;    float tzrqbjfocgzPwni35527730 = -281829141;    float tzrqbjfocgzPwni92405110 = -442314570;    float tzrqbjfocgzPwni87513387 = -600873409;    float tzrqbjfocgzPwni47046278 = -258082256;    float tzrqbjfocgzPwni11521990 = 99331005;    float tzrqbjfocgzPwni404004 = -350280139;    float tzrqbjfocgzPwni99677714 = -97589345;    float tzrqbjfocgzPwni26553072 = -887446131;    float tzrqbjfocgzPwni58955753 = -599411169;    float tzrqbjfocgzPwni45515515 = -530954136;    float tzrqbjfocgzPwni21811305 = -865663249;    float tzrqbjfocgzPwni12769767 = -150106585;    float tzrqbjfocgzPwni61761632 = -45756657;    float tzrqbjfocgzPwni14099230 = -529734670;    float tzrqbjfocgzPwni62875816 = -8406638;    float tzrqbjfocgzPwni88609018 = -362040493;    float tzrqbjfocgzPwni27914838 = -547492942;    float tzrqbjfocgzPwni17543729 = -664361958;    float tzrqbjfocgzPwni50985717 = -926956630;    float tzrqbjfocgzPwni72146333 = 88080171;    float tzrqbjfocgzPwni62833120 = -406658815;    float tzrqbjfocgzPwni26283368 = -419415264;    float tzrqbjfocgzPwni14216641 = -696830790;    float tzrqbjfocgzPwni67100180 = -654505994;    float tzrqbjfocgzPwni68822020 = -580877515;    float tzrqbjfocgzPwni99421799 = -102194001;    float tzrqbjfocgzPwni23411813 = -749131621;    float tzrqbjfocgzPwni93691235 = -49108740;    float tzrqbjfocgzPwni10443488 = -578585519;    float tzrqbjfocgzPwni98814643 = -781511171;    float tzrqbjfocgzPwni37290284 = -564738778;    float tzrqbjfocgzPwni17127667 = -692862362;    float tzrqbjfocgzPwni44708870 = 54454003;    float tzrqbjfocgzPwni34993849 = -945978100;    float tzrqbjfocgzPwni45967523 = -227969356;    float tzrqbjfocgzPwni1481122 = -216176696;    float tzrqbjfocgzPwni63701749 = -90172009;    float tzrqbjfocgzPwni41666440 = 71426457;    float tzrqbjfocgzPwni71599629 = -57763613;    float tzrqbjfocgzPwni94833749 = 38788972;    float tzrqbjfocgzPwni79135095 = -700013280;    float tzrqbjfocgzPwni69686751 = -554375041;    float tzrqbjfocgzPwni29251437 = -132503707;    float tzrqbjfocgzPwni13291209 = -377769450;    float tzrqbjfocgzPwni33436368 = -730381989;    float tzrqbjfocgzPwni33349157 = -493674253;    float tzrqbjfocgzPwni77466459 = -378069382;    float tzrqbjfocgzPwni4415942 = -772401328;    float tzrqbjfocgzPwni28841763 = -237099536;    float tzrqbjfocgzPwni24846452 = -341702351;    float tzrqbjfocgzPwni45313840 = -647872271;    float tzrqbjfocgzPwni27981431 = -746289213;    float tzrqbjfocgzPwni66384137 = -312829772;    float tzrqbjfocgzPwni29428405 = -932354422;    float tzrqbjfocgzPwni35786902 = -177061152;    float tzrqbjfocgzPwni9244363 = -862413877;    float tzrqbjfocgzPwni78188470 = -745483781;    float tzrqbjfocgzPwni20413207 = -946367416;    float tzrqbjfocgzPwni78224257 = -677204742;    float tzrqbjfocgzPwni12100191 = -798474995;    float tzrqbjfocgzPwni76992190 = -601148518;    float tzrqbjfocgzPwni5986479 = 51519394;    float tzrqbjfocgzPwni16109585 = -208860613;    float tzrqbjfocgzPwni60141109 = -817899998;    float tzrqbjfocgzPwni8225232 = -966215358;    float tzrqbjfocgzPwni4683638 = -72800887;    float tzrqbjfocgzPwni68060896 = -104560589;    float tzrqbjfocgzPwni26767784 = -99778557;    float tzrqbjfocgzPwni68131706 = -201765314;    float tzrqbjfocgzPwni61394695 = -792229942;    float tzrqbjfocgzPwni24907269 = -171868484;    float tzrqbjfocgzPwni86248398 = -518919400;    float tzrqbjfocgzPwni45944099 = -506598346;    float tzrqbjfocgzPwni56151967 = -865745602;    float tzrqbjfocgzPwni93011238 = -211906549;    float tzrqbjfocgzPwni93146368 = -852283775;    float tzrqbjfocgzPwni97031930 = -186911558;    float tzrqbjfocgzPwni925433 = -219061340;    float tzrqbjfocgzPwni33663812 = -924124006;    float tzrqbjfocgzPwni35472864 = 12796738;    float tzrqbjfocgzPwni21955340 = -724124620;    float tzrqbjfocgzPwni18995871 = -976730294;    float tzrqbjfocgzPwni64849473 = -812009205;    float tzrqbjfocgzPwni85597035 = -136883169;    float tzrqbjfocgzPwni53500803 = -33638901;    float tzrqbjfocgzPwni9308853 = -818449565;    float tzrqbjfocgzPwni50743530 = -280032591;    float tzrqbjfocgzPwni15280466 = -13191575;    float tzrqbjfocgzPwni99206946 = -668916949;    float tzrqbjfocgzPwni36723160 = -365555480;    float tzrqbjfocgzPwni23292651 = -470692916;    float tzrqbjfocgzPwni43288542 = -143804593;    float tzrqbjfocgzPwni63442182 = -251368801;    float tzrqbjfocgzPwni59499439 = -259288619;    float tzrqbjfocgzPwni17841560 = -360062511;    float tzrqbjfocgzPwni73148617 = -651532675;    float tzrqbjfocgzPwni53577166 = -239786401;     tzrqbjfocgzPwni17369854 = tzrqbjfocgzPwni1574738;     tzrqbjfocgzPwni1574738 = tzrqbjfocgzPwni98620021;     tzrqbjfocgzPwni98620021 = tzrqbjfocgzPwni35527730;     tzrqbjfocgzPwni35527730 = tzrqbjfocgzPwni92405110;     tzrqbjfocgzPwni92405110 = tzrqbjfocgzPwni87513387;     tzrqbjfocgzPwni87513387 = tzrqbjfocgzPwni47046278;     tzrqbjfocgzPwni47046278 = tzrqbjfocgzPwni11521990;     tzrqbjfocgzPwni11521990 = tzrqbjfocgzPwni404004;     tzrqbjfocgzPwni404004 = tzrqbjfocgzPwni99677714;     tzrqbjfocgzPwni99677714 = tzrqbjfocgzPwni26553072;     tzrqbjfocgzPwni26553072 = tzrqbjfocgzPwni58955753;     tzrqbjfocgzPwni58955753 = tzrqbjfocgzPwni45515515;     tzrqbjfocgzPwni45515515 = tzrqbjfocgzPwni21811305;     tzrqbjfocgzPwni21811305 = tzrqbjfocgzPwni12769767;     tzrqbjfocgzPwni12769767 = tzrqbjfocgzPwni61761632;     tzrqbjfocgzPwni61761632 = tzrqbjfocgzPwni14099230;     tzrqbjfocgzPwni14099230 = tzrqbjfocgzPwni62875816;     tzrqbjfocgzPwni62875816 = tzrqbjfocgzPwni88609018;     tzrqbjfocgzPwni88609018 = tzrqbjfocgzPwni27914838;     tzrqbjfocgzPwni27914838 = tzrqbjfocgzPwni17543729;     tzrqbjfocgzPwni17543729 = tzrqbjfocgzPwni50985717;     tzrqbjfocgzPwni50985717 = tzrqbjfocgzPwni72146333;     tzrqbjfocgzPwni72146333 = tzrqbjfocgzPwni62833120;     tzrqbjfocgzPwni62833120 = tzrqbjfocgzPwni26283368;     tzrqbjfocgzPwni26283368 = tzrqbjfocgzPwni14216641;     tzrqbjfocgzPwni14216641 = tzrqbjfocgzPwni67100180;     tzrqbjfocgzPwni67100180 = tzrqbjfocgzPwni68822020;     tzrqbjfocgzPwni68822020 = tzrqbjfocgzPwni99421799;     tzrqbjfocgzPwni99421799 = tzrqbjfocgzPwni23411813;     tzrqbjfocgzPwni23411813 = tzrqbjfocgzPwni93691235;     tzrqbjfocgzPwni93691235 = tzrqbjfocgzPwni10443488;     tzrqbjfocgzPwni10443488 = tzrqbjfocgzPwni98814643;     tzrqbjfocgzPwni98814643 = tzrqbjfocgzPwni37290284;     tzrqbjfocgzPwni37290284 = tzrqbjfocgzPwni17127667;     tzrqbjfocgzPwni17127667 = tzrqbjfocgzPwni44708870;     tzrqbjfocgzPwni44708870 = tzrqbjfocgzPwni34993849;     tzrqbjfocgzPwni34993849 = tzrqbjfocgzPwni45967523;     tzrqbjfocgzPwni45967523 = tzrqbjfocgzPwni1481122;     tzrqbjfocgzPwni1481122 = tzrqbjfocgzPwni63701749;     tzrqbjfocgzPwni63701749 = tzrqbjfocgzPwni41666440;     tzrqbjfocgzPwni41666440 = tzrqbjfocgzPwni71599629;     tzrqbjfocgzPwni71599629 = tzrqbjfocgzPwni94833749;     tzrqbjfocgzPwni94833749 = tzrqbjfocgzPwni79135095;     tzrqbjfocgzPwni79135095 = tzrqbjfocgzPwni69686751;     tzrqbjfocgzPwni69686751 = tzrqbjfocgzPwni29251437;     tzrqbjfocgzPwni29251437 = tzrqbjfocgzPwni13291209;     tzrqbjfocgzPwni13291209 = tzrqbjfocgzPwni33436368;     tzrqbjfocgzPwni33436368 = tzrqbjfocgzPwni33349157;     tzrqbjfocgzPwni33349157 = tzrqbjfocgzPwni77466459;     tzrqbjfocgzPwni77466459 = tzrqbjfocgzPwni4415942;     tzrqbjfocgzPwni4415942 = tzrqbjfocgzPwni28841763;     tzrqbjfocgzPwni28841763 = tzrqbjfocgzPwni24846452;     tzrqbjfocgzPwni24846452 = tzrqbjfocgzPwni45313840;     tzrqbjfocgzPwni45313840 = tzrqbjfocgzPwni27981431;     tzrqbjfocgzPwni27981431 = tzrqbjfocgzPwni66384137;     tzrqbjfocgzPwni66384137 = tzrqbjfocgzPwni29428405;     tzrqbjfocgzPwni29428405 = tzrqbjfocgzPwni35786902;     tzrqbjfocgzPwni35786902 = tzrqbjfocgzPwni9244363;     tzrqbjfocgzPwni9244363 = tzrqbjfocgzPwni78188470;     tzrqbjfocgzPwni78188470 = tzrqbjfocgzPwni20413207;     tzrqbjfocgzPwni20413207 = tzrqbjfocgzPwni78224257;     tzrqbjfocgzPwni78224257 = tzrqbjfocgzPwni12100191;     tzrqbjfocgzPwni12100191 = tzrqbjfocgzPwni76992190;     tzrqbjfocgzPwni76992190 = tzrqbjfocgzPwni5986479;     tzrqbjfocgzPwni5986479 = tzrqbjfocgzPwni16109585;     tzrqbjfocgzPwni16109585 = tzrqbjfocgzPwni60141109;     tzrqbjfocgzPwni60141109 = tzrqbjfocgzPwni8225232;     tzrqbjfocgzPwni8225232 = tzrqbjfocgzPwni4683638;     tzrqbjfocgzPwni4683638 = tzrqbjfocgzPwni68060896;     tzrqbjfocgzPwni68060896 = tzrqbjfocgzPwni26767784;     tzrqbjfocgzPwni26767784 = tzrqbjfocgzPwni68131706;     tzrqbjfocgzPwni68131706 = tzrqbjfocgzPwni61394695;     tzrqbjfocgzPwni61394695 = tzrqbjfocgzPwni24907269;     tzrqbjfocgzPwni24907269 = tzrqbjfocgzPwni86248398;     tzrqbjfocgzPwni86248398 = tzrqbjfocgzPwni45944099;     tzrqbjfocgzPwni45944099 = tzrqbjfocgzPwni56151967;     tzrqbjfocgzPwni56151967 = tzrqbjfocgzPwni93011238;     tzrqbjfocgzPwni93011238 = tzrqbjfocgzPwni93146368;     tzrqbjfocgzPwni93146368 = tzrqbjfocgzPwni97031930;     tzrqbjfocgzPwni97031930 = tzrqbjfocgzPwni925433;     tzrqbjfocgzPwni925433 = tzrqbjfocgzPwni33663812;     tzrqbjfocgzPwni33663812 = tzrqbjfocgzPwni35472864;     tzrqbjfocgzPwni35472864 = tzrqbjfocgzPwni21955340;     tzrqbjfocgzPwni21955340 = tzrqbjfocgzPwni18995871;     tzrqbjfocgzPwni18995871 = tzrqbjfocgzPwni64849473;     tzrqbjfocgzPwni64849473 = tzrqbjfocgzPwni85597035;     tzrqbjfocgzPwni85597035 = tzrqbjfocgzPwni53500803;     tzrqbjfocgzPwni53500803 = tzrqbjfocgzPwni9308853;     tzrqbjfocgzPwni9308853 = tzrqbjfocgzPwni50743530;     tzrqbjfocgzPwni50743530 = tzrqbjfocgzPwni15280466;     tzrqbjfocgzPwni15280466 = tzrqbjfocgzPwni99206946;     tzrqbjfocgzPwni99206946 = tzrqbjfocgzPwni36723160;     tzrqbjfocgzPwni36723160 = tzrqbjfocgzPwni23292651;     tzrqbjfocgzPwni23292651 = tzrqbjfocgzPwni43288542;     tzrqbjfocgzPwni43288542 = tzrqbjfocgzPwni63442182;     tzrqbjfocgzPwni63442182 = tzrqbjfocgzPwni59499439;     tzrqbjfocgzPwni59499439 = tzrqbjfocgzPwni17841560;     tzrqbjfocgzPwni17841560 = tzrqbjfocgzPwni73148617;     tzrqbjfocgzPwni73148617 = tzrqbjfocgzPwni53577166;     tzrqbjfocgzPwni53577166 = tzrqbjfocgzPwni17369854;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void gfKeWtzuSNpkmFfP52183904() {     float zUssvwwCAacRonc17363383 = -112310545;    float zUssvwwCAacRonc40147054 = -986869382;    float zUssvwwCAacRonc99933233 = -640848580;    float zUssvwwCAacRonc20607591 = -102369968;    float zUssvwwCAacRonc1245213 = -910669899;    float zUssvwwCAacRonc22318820 = -300306221;    float zUssvwwCAacRonc15593614 = 37381277;    float zUssvwwCAacRonc80736040 = -704846328;    float zUssvwwCAacRonc83144167 = -63926210;    float zUssvwwCAacRonc96317227 = -834396217;    float zUssvwwCAacRonc46873678 = -846918204;    float zUssvwwCAacRonc73247430 = -334889540;    float zUssvwwCAacRonc77368429 = -552612610;    float zUssvwwCAacRonc75655524 = -110816302;    float zUssvwwCAacRonc57941876 = -112819456;    float zUssvwwCAacRonc36326180 = -378552308;    float zUssvwwCAacRonc69951989 = -349349239;    float zUssvwwCAacRonc85876349 = -405478264;    float zUssvwwCAacRonc9037289 = -980022758;    float zUssvwwCAacRonc99988601 = -59517393;    float zUssvwwCAacRonc97213152 = -374828337;    float zUssvwwCAacRonc29281983 = -336948513;    float zUssvwwCAacRonc9024731 = -370877671;    float zUssvwwCAacRonc6483228 = -287579338;    float zUssvwwCAacRonc54726057 = -846109130;    float zUssvwwCAacRonc30790719 = -605036714;    float zUssvwwCAacRonc37321762 = -955305707;    float zUssvwwCAacRonc4225546 = -89844509;    float zUssvwwCAacRonc12100247 = -748264301;    float zUssvwwCAacRonc99501683 = -424562995;    float zUssvwwCAacRonc27103915 = -436201975;    float zUssvwwCAacRonc95167344 = -970407278;    float zUssvwwCAacRonc84267066 = -983455384;    float zUssvwwCAacRonc37573121 = -441420511;    float zUssvwwCAacRonc99485704 = -952176284;    float zUssvwwCAacRonc87537060 = -25311724;    float zUssvwwCAacRonc46935216 = 54368521;    float zUssvwwCAacRonc15885203 = -223232038;    float zUssvwwCAacRonc36794718 = -928858100;    float zUssvwwCAacRonc35574641 = -799603695;    float zUssvwwCAacRonc29522108 = -16517441;    float zUssvwwCAacRonc57297290 = -251555675;    float zUssvwwCAacRonc55117110 = -520341280;    float zUssvwwCAacRonc38366994 = -256977724;    float zUssvwwCAacRonc53704804 = 72365558;    float zUssvwwCAacRonc69679405 = 55451641;    float zUssvwwCAacRonc40989001 = -859050072;    float zUssvwwCAacRonc8819949 = -239143344;    float zUssvwwCAacRonc52007546 = -57438090;    float zUssvwwCAacRonc97511882 = 34905727;    float zUssvwwCAacRonc17675312 = -350956335;    float zUssvwwCAacRonc43389020 = -619245280;    float zUssvwwCAacRonc294800 = -233756362;    float zUssvwwCAacRonc55564111 = -79792122;    float zUssvwwCAacRonc54970751 = -503188486;    float zUssvwwCAacRonc88081400 = -775362032;    float zUssvwwCAacRonc31122324 = -515991711;    float zUssvwwCAacRonc93450005 = -253269243;    float zUssvwwCAacRonc65881534 = -256260839;    float zUssvwwCAacRonc70454494 = -205633186;    float zUssvwwCAacRonc84997057 = -345000515;    float zUssvwwCAacRonc11368069 = -872774214;    float zUssvwwCAacRonc68635794 = -956582028;    float zUssvwwCAacRonc83642484 = -639363215;    float zUssvwwCAacRonc69213313 = -298194242;    float zUssvwwCAacRonc51706333 = -876510927;    float zUssvwwCAacRonc88980363 = -351434157;    float zUssvwwCAacRonc39795308 = -11192100;    float zUssvwwCAacRonc76169820 = -158640018;    float zUssvwwCAacRonc70404815 = 12492268;    float zUssvwwCAacRonc89390964 = -332920830;    float zUssvwwCAacRonc54066786 = -26117201;    float zUssvwwCAacRonc49081632 = -476620165;    float zUssvwwCAacRonc73462647 = -80419064;    float zUssvwwCAacRonc70466494 = 57000048;    float zUssvwwCAacRonc39915862 = -23272663;    float zUssvwwCAacRonc74164873 = -816607234;    float zUssvwwCAacRonc70657736 = -13899948;    float zUssvwwCAacRonc52778424 = -259944897;    float zUssvwwCAacRonc85046652 = -801560771;    float zUssvwwCAacRonc89801717 = -745986642;    float zUssvwwCAacRonc28501813 = -616162364;    float zUssvwwCAacRonc52217999 = 67593580;    float zUssvwwCAacRonc14588364 = -683170028;    float zUssvwwCAacRonc81826371 = 26393339;    float zUssvwwCAacRonc83714894 = -816956695;    float zUssvwwCAacRonc94872545 = -636650916;    float zUssvwwCAacRonc28702956 = -803663263;    float zUssvwwCAacRonc82602369 = -938232025;    float zUssvwwCAacRonc11404305 = -76814252;    float zUssvwwCAacRonc56414736 = -509320013;    float zUssvwwCAacRonc53485211 = -692362236;    float zUssvwwCAacRonc50003669 = -966971200;    float zUssvwwCAacRonc66340223 = -623224914;    float zUssvwwCAacRonc50577584 = -354603181;    float zUssvwwCAacRonc18154039 = -143743227;    float zUssvwwCAacRonc88661496 = -294973648;    float zUssvwwCAacRonc71474625 = -880978066;    float zUssvwwCAacRonc69153680 = -958783482;    float zUssvwwCAacRonc1998471 = -112310545;     zUssvwwCAacRonc17363383 = zUssvwwCAacRonc40147054;     zUssvwwCAacRonc40147054 = zUssvwwCAacRonc99933233;     zUssvwwCAacRonc99933233 = zUssvwwCAacRonc20607591;     zUssvwwCAacRonc20607591 = zUssvwwCAacRonc1245213;     zUssvwwCAacRonc1245213 = zUssvwwCAacRonc22318820;     zUssvwwCAacRonc22318820 = zUssvwwCAacRonc15593614;     zUssvwwCAacRonc15593614 = zUssvwwCAacRonc80736040;     zUssvwwCAacRonc80736040 = zUssvwwCAacRonc83144167;     zUssvwwCAacRonc83144167 = zUssvwwCAacRonc96317227;     zUssvwwCAacRonc96317227 = zUssvwwCAacRonc46873678;     zUssvwwCAacRonc46873678 = zUssvwwCAacRonc73247430;     zUssvwwCAacRonc73247430 = zUssvwwCAacRonc77368429;     zUssvwwCAacRonc77368429 = zUssvwwCAacRonc75655524;     zUssvwwCAacRonc75655524 = zUssvwwCAacRonc57941876;     zUssvwwCAacRonc57941876 = zUssvwwCAacRonc36326180;     zUssvwwCAacRonc36326180 = zUssvwwCAacRonc69951989;     zUssvwwCAacRonc69951989 = zUssvwwCAacRonc85876349;     zUssvwwCAacRonc85876349 = zUssvwwCAacRonc9037289;     zUssvwwCAacRonc9037289 = zUssvwwCAacRonc99988601;     zUssvwwCAacRonc99988601 = zUssvwwCAacRonc97213152;     zUssvwwCAacRonc97213152 = zUssvwwCAacRonc29281983;     zUssvwwCAacRonc29281983 = zUssvwwCAacRonc9024731;     zUssvwwCAacRonc9024731 = zUssvwwCAacRonc6483228;     zUssvwwCAacRonc6483228 = zUssvwwCAacRonc54726057;     zUssvwwCAacRonc54726057 = zUssvwwCAacRonc30790719;     zUssvwwCAacRonc30790719 = zUssvwwCAacRonc37321762;     zUssvwwCAacRonc37321762 = zUssvwwCAacRonc4225546;     zUssvwwCAacRonc4225546 = zUssvwwCAacRonc12100247;     zUssvwwCAacRonc12100247 = zUssvwwCAacRonc99501683;     zUssvwwCAacRonc99501683 = zUssvwwCAacRonc27103915;     zUssvwwCAacRonc27103915 = zUssvwwCAacRonc95167344;     zUssvwwCAacRonc95167344 = zUssvwwCAacRonc84267066;     zUssvwwCAacRonc84267066 = zUssvwwCAacRonc37573121;     zUssvwwCAacRonc37573121 = zUssvwwCAacRonc99485704;     zUssvwwCAacRonc99485704 = zUssvwwCAacRonc87537060;     zUssvwwCAacRonc87537060 = zUssvwwCAacRonc46935216;     zUssvwwCAacRonc46935216 = zUssvwwCAacRonc15885203;     zUssvwwCAacRonc15885203 = zUssvwwCAacRonc36794718;     zUssvwwCAacRonc36794718 = zUssvwwCAacRonc35574641;     zUssvwwCAacRonc35574641 = zUssvwwCAacRonc29522108;     zUssvwwCAacRonc29522108 = zUssvwwCAacRonc57297290;     zUssvwwCAacRonc57297290 = zUssvwwCAacRonc55117110;     zUssvwwCAacRonc55117110 = zUssvwwCAacRonc38366994;     zUssvwwCAacRonc38366994 = zUssvwwCAacRonc53704804;     zUssvwwCAacRonc53704804 = zUssvwwCAacRonc69679405;     zUssvwwCAacRonc69679405 = zUssvwwCAacRonc40989001;     zUssvwwCAacRonc40989001 = zUssvwwCAacRonc8819949;     zUssvwwCAacRonc8819949 = zUssvwwCAacRonc52007546;     zUssvwwCAacRonc52007546 = zUssvwwCAacRonc97511882;     zUssvwwCAacRonc97511882 = zUssvwwCAacRonc17675312;     zUssvwwCAacRonc17675312 = zUssvwwCAacRonc43389020;     zUssvwwCAacRonc43389020 = zUssvwwCAacRonc294800;     zUssvwwCAacRonc294800 = zUssvwwCAacRonc55564111;     zUssvwwCAacRonc55564111 = zUssvwwCAacRonc54970751;     zUssvwwCAacRonc54970751 = zUssvwwCAacRonc88081400;     zUssvwwCAacRonc88081400 = zUssvwwCAacRonc31122324;     zUssvwwCAacRonc31122324 = zUssvwwCAacRonc93450005;     zUssvwwCAacRonc93450005 = zUssvwwCAacRonc65881534;     zUssvwwCAacRonc65881534 = zUssvwwCAacRonc70454494;     zUssvwwCAacRonc70454494 = zUssvwwCAacRonc84997057;     zUssvwwCAacRonc84997057 = zUssvwwCAacRonc11368069;     zUssvwwCAacRonc11368069 = zUssvwwCAacRonc68635794;     zUssvwwCAacRonc68635794 = zUssvwwCAacRonc83642484;     zUssvwwCAacRonc83642484 = zUssvwwCAacRonc69213313;     zUssvwwCAacRonc69213313 = zUssvwwCAacRonc51706333;     zUssvwwCAacRonc51706333 = zUssvwwCAacRonc88980363;     zUssvwwCAacRonc88980363 = zUssvwwCAacRonc39795308;     zUssvwwCAacRonc39795308 = zUssvwwCAacRonc76169820;     zUssvwwCAacRonc76169820 = zUssvwwCAacRonc70404815;     zUssvwwCAacRonc70404815 = zUssvwwCAacRonc89390964;     zUssvwwCAacRonc89390964 = zUssvwwCAacRonc54066786;     zUssvwwCAacRonc54066786 = zUssvwwCAacRonc49081632;     zUssvwwCAacRonc49081632 = zUssvwwCAacRonc73462647;     zUssvwwCAacRonc73462647 = zUssvwwCAacRonc70466494;     zUssvwwCAacRonc70466494 = zUssvwwCAacRonc39915862;     zUssvwwCAacRonc39915862 = zUssvwwCAacRonc74164873;     zUssvwwCAacRonc74164873 = zUssvwwCAacRonc70657736;     zUssvwwCAacRonc70657736 = zUssvwwCAacRonc52778424;     zUssvwwCAacRonc52778424 = zUssvwwCAacRonc85046652;     zUssvwwCAacRonc85046652 = zUssvwwCAacRonc89801717;     zUssvwwCAacRonc89801717 = zUssvwwCAacRonc28501813;     zUssvwwCAacRonc28501813 = zUssvwwCAacRonc52217999;     zUssvwwCAacRonc52217999 = zUssvwwCAacRonc14588364;     zUssvwwCAacRonc14588364 = zUssvwwCAacRonc81826371;     zUssvwwCAacRonc81826371 = zUssvwwCAacRonc83714894;     zUssvwwCAacRonc83714894 = zUssvwwCAacRonc94872545;     zUssvwwCAacRonc94872545 = zUssvwwCAacRonc28702956;     zUssvwwCAacRonc28702956 = zUssvwwCAacRonc82602369;     zUssvwwCAacRonc82602369 = zUssvwwCAacRonc11404305;     zUssvwwCAacRonc11404305 = zUssvwwCAacRonc56414736;     zUssvwwCAacRonc56414736 = zUssvwwCAacRonc53485211;     zUssvwwCAacRonc53485211 = zUssvwwCAacRonc50003669;     zUssvwwCAacRonc50003669 = zUssvwwCAacRonc66340223;     zUssvwwCAacRonc66340223 = zUssvwwCAacRonc50577584;     zUssvwwCAacRonc50577584 = zUssvwwCAacRonc18154039;     zUssvwwCAacRonc18154039 = zUssvwwCAacRonc88661496;     zUssvwwCAacRonc88661496 = zUssvwwCAacRonc71474625;     zUssvwwCAacRonc71474625 = zUssvwwCAacRonc69153680;     zUssvwwCAacRonc69153680 = zUssvwwCAacRonc1998471;     zUssvwwCAacRonc1998471 = zUssvwwCAacRonc17363383;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ZXAOiXkeRnUGBHBl40411016() {     float YWyPSrnUlRxijOU78414099 = -131096404;    float YWyPSrnUlRxijOU11243960 = 19696360;    float YWyPSrnUlRxijOU68885673 = -151848578;    float YWyPSrnUlRxijOU53473014 = -110303102;    float YWyPSrnUlRxijOU18032385 = -594835711;    float YWyPSrnUlRxijOU40745536 = 81797692;    float YWyPSrnUlRxijOU63024529 = -685079198;    float YWyPSrnUlRxijOU88663675 = 74414970;    float YWyPSrnUlRxijOU27250901 = -234835592;    float YWyPSrnUlRxijOU33738809 = -96837867;    float YWyPSrnUlRxijOU79068361 = -999881748;    float YWyPSrnUlRxijOU94841557 = -907240856;    float YWyPSrnUlRxijOU60330938 = -847215006;    float YWyPSrnUlRxijOU66511885 = -816550953;    float YWyPSrnUlRxijOU27233172 = -542077576;    float YWyPSrnUlRxijOU53023977 = -117759741;    float YWyPSrnUlRxijOU69733281 = -745029016;    float YWyPSrnUlRxijOU83751912 = -598112071;    float YWyPSrnUlRxijOU50624070 = -361577771;    float YWyPSrnUlRxijOU82913523 = -948493383;    float YWyPSrnUlRxijOU31639857 = -783336433;    float YWyPSrnUlRxijOU55910348 = -333325854;    float YWyPSrnUlRxijOU46435511 = -693644118;    float YWyPSrnUlRxijOU58340107 = -776332399;    float YWyPSrnUlRxijOU89204457 = -10896617;    float YWyPSrnUlRxijOU54403950 = 50141866;    float YWyPSrnUlRxijOU9908664 = -26603269;    float YWyPSrnUlRxijOU73449245 = -980311928;    float YWyPSrnUlRxijOU53967226 = -912582639;    float YWyPSrnUlRxijOU82267714 = -692548354;    float YWyPSrnUlRxijOU34321102 = -379736258;    float YWyPSrnUlRxijOU85456438 = -625352764;    float YWyPSrnUlRxijOU32219706 = -276928283;    float YWyPSrnUlRxijOU20625781 = -685577563;    float YWyPSrnUlRxijOU84341108 = -245398521;    float YWyPSrnUlRxijOU1061565 = -677706000;    float YWyPSrnUlRxijOU34051050 = -191186085;    float YWyPSrnUlRxijOU72755613 = -59339766;    float YWyPSrnUlRxijOU72795261 = -325391282;    float YWyPSrnUlRxijOU36756955 = -25404874;    float YWyPSrnUlRxijOU22269329 = -272832647;    float YWyPSrnUlRxijOU4974347 = -161347388;    float YWyPSrnUlRxijOU33417348 = -642998194;    float YWyPSrnUlRxijOU90059712 = -672761289;    float YWyPSrnUlRxijOU61684086 = -319907319;    float YWyPSrnUlRxijOU41613087 = -693974859;    float YWyPSrnUlRxijOU32404826 = -894224671;    float YWyPSrnUlRxijOU59275451 = 9908572;    float YWyPSrnUlRxijOU7053143 = -257087336;    float YWyPSrnUlRxijOU55806044 = -717119356;    float YWyPSrnUlRxijOU95322913 = -895336488;    float YWyPSrnUlRxijOU39585815 = -628831058;    float YWyPSrnUlRxijOU11541204 = -665646457;    float YWyPSrnUlRxijOU30204596 = -54444552;    float YWyPSrnUlRxijOU8771159 = -467869771;    float YWyPSrnUlRxijOU22503751 = -797770551;    float YWyPSrnUlRxijOU64808448 = -286659522;    float YWyPSrnUlRxijOU10545567 = -375516179;    float YWyPSrnUlRxijOU64268557 = 593514;    float YWyPSrnUlRxijOU63628435 = -544977577;    float YWyPSrnUlRxijOU30836873 = -891599039;    float YWyPSrnUlRxijOU89575283 = -704767270;    float YWyPSrnUlRxijOU34696449 = -13002392;    float YWyPSrnUlRxijOU44983186 = -542287238;    float YWyPSrnUlRxijOU99417706 = -717101609;    float YWyPSrnUlRxijOU93611922 = -274528984;    float YWyPSrnUlRxijOU62621851 = -530312574;    float YWyPSrnUlRxijOU39705157 = -61637443;    float YWyPSrnUlRxijOU82170776 = -471152432;    float YWyPSrnUlRxijOU26171607 = -864371577;    float YWyPSrnUlRxijOU18972927 = -926573657;    float YWyPSrnUlRxijOU96977668 = -585689251;    float YWyPSrnUlRxijOU10956651 = -172720790;    float YWyPSrnUlRxijOU13867115 = -236172897;    float YWyPSrnUlRxijOU60644195 = -575660737;    float YWyPSrnUlRxijOU26665511 = -521989046;    float YWyPSrnUlRxijOU22493001 = -690327661;    float YWyPSrnUlRxijOU56375798 = 79117170;    float YWyPSrnUlRxijOU96656021 = -356425081;    float YWyPSrnUlRxijOU47591371 = -316921759;    float YWyPSrnUlRxijOU21999124 = -55633463;    float YWyPSrnUlRxijOU50633213 = 63488158;    float YWyPSrnUlRxijOU66396103 = -623224593;    float YWyPSrnUlRxijOU98161182 = -95463283;    float YWyPSrnUlRxijOU86944801 = -797211867;    float YWyPSrnUlRxijOU94735287 = -750905201;    float YWyPSrnUlRxijOU73915235 = -959706308;    float YWyPSrnUlRxijOU2015111 = -122483731;    float YWyPSrnUlRxijOU11854623 = -117707792;    float YWyPSrnUlRxijOU61837358 = -447627971;    float YWyPSrnUlRxijOU36253117 = -291046478;    float YWyPSrnUlRxijOU23505484 = -815669907;    float YWyPSrnUlRxijOU8487057 = 40066720;    float YWyPSrnUlRxijOU9166827 = -780413705;    float YWyPSrnUlRxijOU5920083 = -133805835;    float YWyPSrnUlRxijOU32694045 = -568065377;    float YWyPSrnUlRxijOU70277897 = -48344996;    float YWyPSrnUlRxijOU88434161 = -710956;    float YWyPSrnUlRxijOU90642006 = -955659680;    float YWyPSrnUlRxijOU68072163 = -131096404;     YWyPSrnUlRxijOU78414099 = YWyPSrnUlRxijOU11243960;     YWyPSrnUlRxijOU11243960 = YWyPSrnUlRxijOU68885673;     YWyPSrnUlRxijOU68885673 = YWyPSrnUlRxijOU53473014;     YWyPSrnUlRxijOU53473014 = YWyPSrnUlRxijOU18032385;     YWyPSrnUlRxijOU18032385 = YWyPSrnUlRxijOU40745536;     YWyPSrnUlRxijOU40745536 = YWyPSrnUlRxijOU63024529;     YWyPSrnUlRxijOU63024529 = YWyPSrnUlRxijOU88663675;     YWyPSrnUlRxijOU88663675 = YWyPSrnUlRxijOU27250901;     YWyPSrnUlRxijOU27250901 = YWyPSrnUlRxijOU33738809;     YWyPSrnUlRxijOU33738809 = YWyPSrnUlRxijOU79068361;     YWyPSrnUlRxijOU79068361 = YWyPSrnUlRxijOU94841557;     YWyPSrnUlRxijOU94841557 = YWyPSrnUlRxijOU60330938;     YWyPSrnUlRxijOU60330938 = YWyPSrnUlRxijOU66511885;     YWyPSrnUlRxijOU66511885 = YWyPSrnUlRxijOU27233172;     YWyPSrnUlRxijOU27233172 = YWyPSrnUlRxijOU53023977;     YWyPSrnUlRxijOU53023977 = YWyPSrnUlRxijOU69733281;     YWyPSrnUlRxijOU69733281 = YWyPSrnUlRxijOU83751912;     YWyPSrnUlRxijOU83751912 = YWyPSrnUlRxijOU50624070;     YWyPSrnUlRxijOU50624070 = YWyPSrnUlRxijOU82913523;     YWyPSrnUlRxijOU82913523 = YWyPSrnUlRxijOU31639857;     YWyPSrnUlRxijOU31639857 = YWyPSrnUlRxijOU55910348;     YWyPSrnUlRxijOU55910348 = YWyPSrnUlRxijOU46435511;     YWyPSrnUlRxijOU46435511 = YWyPSrnUlRxijOU58340107;     YWyPSrnUlRxijOU58340107 = YWyPSrnUlRxijOU89204457;     YWyPSrnUlRxijOU89204457 = YWyPSrnUlRxijOU54403950;     YWyPSrnUlRxijOU54403950 = YWyPSrnUlRxijOU9908664;     YWyPSrnUlRxijOU9908664 = YWyPSrnUlRxijOU73449245;     YWyPSrnUlRxijOU73449245 = YWyPSrnUlRxijOU53967226;     YWyPSrnUlRxijOU53967226 = YWyPSrnUlRxijOU82267714;     YWyPSrnUlRxijOU82267714 = YWyPSrnUlRxijOU34321102;     YWyPSrnUlRxijOU34321102 = YWyPSrnUlRxijOU85456438;     YWyPSrnUlRxijOU85456438 = YWyPSrnUlRxijOU32219706;     YWyPSrnUlRxijOU32219706 = YWyPSrnUlRxijOU20625781;     YWyPSrnUlRxijOU20625781 = YWyPSrnUlRxijOU84341108;     YWyPSrnUlRxijOU84341108 = YWyPSrnUlRxijOU1061565;     YWyPSrnUlRxijOU1061565 = YWyPSrnUlRxijOU34051050;     YWyPSrnUlRxijOU34051050 = YWyPSrnUlRxijOU72755613;     YWyPSrnUlRxijOU72755613 = YWyPSrnUlRxijOU72795261;     YWyPSrnUlRxijOU72795261 = YWyPSrnUlRxijOU36756955;     YWyPSrnUlRxijOU36756955 = YWyPSrnUlRxijOU22269329;     YWyPSrnUlRxijOU22269329 = YWyPSrnUlRxijOU4974347;     YWyPSrnUlRxijOU4974347 = YWyPSrnUlRxijOU33417348;     YWyPSrnUlRxijOU33417348 = YWyPSrnUlRxijOU90059712;     YWyPSrnUlRxijOU90059712 = YWyPSrnUlRxijOU61684086;     YWyPSrnUlRxijOU61684086 = YWyPSrnUlRxijOU41613087;     YWyPSrnUlRxijOU41613087 = YWyPSrnUlRxijOU32404826;     YWyPSrnUlRxijOU32404826 = YWyPSrnUlRxijOU59275451;     YWyPSrnUlRxijOU59275451 = YWyPSrnUlRxijOU7053143;     YWyPSrnUlRxijOU7053143 = YWyPSrnUlRxijOU55806044;     YWyPSrnUlRxijOU55806044 = YWyPSrnUlRxijOU95322913;     YWyPSrnUlRxijOU95322913 = YWyPSrnUlRxijOU39585815;     YWyPSrnUlRxijOU39585815 = YWyPSrnUlRxijOU11541204;     YWyPSrnUlRxijOU11541204 = YWyPSrnUlRxijOU30204596;     YWyPSrnUlRxijOU30204596 = YWyPSrnUlRxijOU8771159;     YWyPSrnUlRxijOU8771159 = YWyPSrnUlRxijOU22503751;     YWyPSrnUlRxijOU22503751 = YWyPSrnUlRxijOU64808448;     YWyPSrnUlRxijOU64808448 = YWyPSrnUlRxijOU10545567;     YWyPSrnUlRxijOU10545567 = YWyPSrnUlRxijOU64268557;     YWyPSrnUlRxijOU64268557 = YWyPSrnUlRxijOU63628435;     YWyPSrnUlRxijOU63628435 = YWyPSrnUlRxijOU30836873;     YWyPSrnUlRxijOU30836873 = YWyPSrnUlRxijOU89575283;     YWyPSrnUlRxijOU89575283 = YWyPSrnUlRxijOU34696449;     YWyPSrnUlRxijOU34696449 = YWyPSrnUlRxijOU44983186;     YWyPSrnUlRxijOU44983186 = YWyPSrnUlRxijOU99417706;     YWyPSrnUlRxijOU99417706 = YWyPSrnUlRxijOU93611922;     YWyPSrnUlRxijOU93611922 = YWyPSrnUlRxijOU62621851;     YWyPSrnUlRxijOU62621851 = YWyPSrnUlRxijOU39705157;     YWyPSrnUlRxijOU39705157 = YWyPSrnUlRxijOU82170776;     YWyPSrnUlRxijOU82170776 = YWyPSrnUlRxijOU26171607;     YWyPSrnUlRxijOU26171607 = YWyPSrnUlRxijOU18972927;     YWyPSrnUlRxijOU18972927 = YWyPSrnUlRxijOU96977668;     YWyPSrnUlRxijOU96977668 = YWyPSrnUlRxijOU10956651;     YWyPSrnUlRxijOU10956651 = YWyPSrnUlRxijOU13867115;     YWyPSrnUlRxijOU13867115 = YWyPSrnUlRxijOU60644195;     YWyPSrnUlRxijOU60644195 = YWyPSrnUlRxijOU26665511;     YWyPSrnUlRxijOU26665511 = YWyPSrnUlRxijOU22493001;     YWyPSrnUlRxijOU22493001 = YWyPSrnUlRxijOU56375798;     YWyPSrnUlRxijOU56375798 = YWyPSrnUlRxijOU96656021;     YWyPSrnUlRxijOU96656021 = YWyPSrnUlRxijOU47591371;     YWyPSrnUlRxijOU47591371 = YWyPSrnUlRxijOU21999124;     YWyPSrnUlRxijOU21999124 = YWyPSrnUlRxijOU50633213;     YWyPSrnUlRxijOU50633213 = YWyPSrnUlRxijOU66396103;     YWyPSrnUlRxijOU66396103 = YWyPSrnUlRxijOU98161182;     YWyPSrnUlRxijOU98161182 = YWyPSrnUlRxijOU86944801;     YWyPSrnUlRxijOU86944801 = YWyPSrnUlRxijOU94735287;     YWyPSrnUlRxijOU94735287 = YWyPSrnUlRxijOU73915235;     YWyPSrnUlRxijOU73915235 = YWyPSrnUlRxijOU2015111;     YWyPSrnUlRxijOU2015111 = YWyPSrnUlRxijOU11854623;     YWyPSrnUlRxijOU11854623 = YWyPSrnUlRxijOU61837358;     YWyPSrnUlRxijOU61837358 = YWyPSrnUlRxijOU36253117;     YWyPSrnUlRxijOU36253117 = YWyPSrnUlRxijOU23505484;     YWyPSrnUlRxijOU23505484 = YWyPSrnUlRxijOU8487057;     YWyPSrnUlRxijOU8487057 = YWyPSrnUlRxijOU9166827;     YWyPSrnUlRxijOU9166827 = YWyPSrnUlRxijOU5920083;     YWyPSrnUlRxijOU5920083 = YWyPSrnUlRxijOU32694045;     YWyPSrnUlRxijOU32694045 = YWyPSrnUlRxijOU70277897;     YWyPSrnUlRxijOU70277897 = YWyPSrnUlRxijOU88434161;     YWyPSrnUlRxijOU88434161 = YWyPSrnUlRxijOU90642006;     YWyPSrnUlRxijOU90642006 = YWyPSrnUlRxijOU68072163;     YWyPSrnUlRxijOU68072163 = YWyPSrnUlRxijOU78414099;}
// Junk Finished
