#include <mutex>
#include <numeric>
#include <sstream>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"
#include "../SDK/ConVar.h"
#include "../SDK/Surface.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/WeaponData.h"
#include "EnginePrediction.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/GameEvent.h"
#include "../SDK/FrameStage.h"
#include "../SDK/Client.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/WeaponSystem.h"
#include "../SDK/WeaponData.h"
#include "../GUI.h"
#include "Aimbot.cpp"
#include "../Hooks.h"


std::vector<std::string> msgs =
{
"alsware.cc - Best Legit Cheat",
"Download our Cheat and will be happy!",
"Ur cheat missing, download alsware",
"discord.gg/xTsqK48",
"---------alsware.cc----------",
"one step behind the game",
};

void Misc::edgejump(UserCmd* cmd) noexcept
{
    if (!config->misc.edgejump || !GetAsyncKeyState(config->misc.edgejumpkey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto mt = localPlayer->moveType(); mt == MoveType::LADDER || mt == MoveType::NOCLIP)
        return;

    if ((EnginePrediction::getFlags() & 1) && !(localPlayer->flags() & 1))
        cmd->buttons |= UserCmd::IN_JUMP;
}

void Misc::slowwalk(UserCmd* cmd) noexcept
{
    if (!config->misc.slowwalk || !GetAsyncKeyState(config->misc.slowwalkKey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    const auto weaponData = activeWeapon->getWeaponData();
    if (!weaponData)
        return;

    const float maxSpeed = (localPlayer->isScoped() ? weaponData->maxSpeedAlt : weaponData->maxSpeed) / 3;

    if (cmd->forwardmove && cmd->sidemove) {
        const float maxSpeedRoot = maxSpeed * static_cast<float>(M_SQRT1_2);
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
    } else if (cmd->forwardmove) {
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeed : maxSpeed;
    } else if (cmd->sidemove) {
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeed : maxSpeed;
    }
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces->cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config->visuals.inverseRagdollGravity ? -600 : 600);
}

void Misc::updateClanTag(bool tagChanged) noexcept
{
    if (config->misc.customClanTag) {
        static std::string clanTag;
        static int prevtime;
        static int prevval;

        if (tagChanged) {
            clanTag = config->misc.clanTag;
            if (!isblank(clanTag.front()) && !isblank(clanTag.back()))
                clanTag.push_back(' ');
        }

        const auto time = std::time(nullptr);
        const auto localTime = std::localtime(&time);

        switch (config->misc.tagAnimType) {
        case 0: {
            if (!clanTag.empty()) {
                std::rotate(clanTag.begin(), clanTag.begin() + 1, clanTag.end());
            }
            memory->setClanTag(clanTag.c_str(), clanTag.c_str());
            break;
        }
        case 1: {
            char s[11];
            s[0] = '\0';
            sprintf_s(s, "[%02d:%02d:%02d]", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
            memory->setClanTag(s, s);
            break;
        }
        case 2: {
            if (int(memory->globalVars->currenttime * 3.1) != prevtime)
            {
                if (prevval != int(memory->globalVars->currenttime * 3.1) % 31)
                {
                    prevval = int(memory->globalVars->currenttime * 3.1) % 31;
                    switch (int(memory->globalVars->currenttime * 3.1) % 31) {
                    case 30: { memory->setClanTag("[OsirisWare]", "[OsirisWare]"); break; }
                    case 24: { memory->setClanTag("[OsirisWar]", "[OsirisWar]");; break; }
                    case 23: { memory->setClanTag("[OsirisWa]", "[OsirisWa]"); break; }
                    case 22: { memory->setClanTag("[OsirisW]", "[OsirisW]");; break; }
                    case 21: { memory->setClanTag("[Osiris]", "[Osiris]");; break; }
                    case 20: { memory->setClanTag("[Osiri]", "[Osiri]");; break; }
                    case 19: { memory->setClanTag("[Osir]", "[Osir]");; break; }
                    case 18: { memory->setClanTag("[Osi]", "[Osi]");; break; }
                    case 17: { memory->setClanTag("[Os]", "[Os]");; break; }
                    case 16: { memory->setClanTag("[O]", "[O]");; break; }
                    case 15: { memory->setClanTag("[  -S-  ]", "[  -S-  ]"); break; }
                    case 14: { memory->setClanTag("[ --S-- ]", "[ --S-- ]"); break; }
                    case 13: { memory->setClanTag("[-O-S---]", "[-O-S---]"); break; }
                    case 12: { memory->setClanTag("[-O-S---]", "[-O-S---]"); break; }
                    case 11: { memory->setClanTag("[-O-S-W-]", "[-O-S-W-]"); break; }
                    case 10: { memory->setClanTag("[-O-S-W-]", "[-O-S-W-]"); break; }
                    case 9: { memory->setClanTag("[O]", "[O]");; break; }
                    case 8: { memory->setClanTag("[Os]", "[Os]");; break; }
                    case 7: { memory->setClanTag("[Osi]", "[Osi]");; break; }
                    case 6: { memory->setClanTag("[Osir]", "[Osir]");; break; }
                    case 5: { memory->setClanTag("[Osiri]", "[Osiri]");; break; }
                    case 4: { memory->setClanTag("[Osiris]", "[Osiris]");; break; }
                    case 3: { memory->setClanTag("[OsirisW]", "[OsirisW]");; break; }
                    case 2: { memory->setClanTag("[OsirisWa]", "[OsirisWa]");; break; }
                    case 1: { memory->setClanTag("[OsirisWar]", "[OsirisWar]");; break; }
                    case 0: { memory->setClanTag("[OsirisWare]", "[OsirisWare]");; break; }
                    default:;
                    }
                }
            }
            prevtime = int(memory->globalVars->currenttime);
            break;
        }
        case 3: {
            if (int(memory->globalVars->currenttime * 2.4) != prevtime)
            {
                if (prevval != int(memory->globalVars->currenttime * 2.4) % 27)
                {
                    prevval = int(memory->globalVars->currenttime * 2.4) % 27;
                    switch (int(memory->globalVars->currenttime * 2.4) % 27) {
                    case 0: memory->setClanTag("                  ", "                  "); break;
                    case 1: memory->setClanTag("                 g", "                 g"); break;
                    case 2: memory->setClanTag("                ga", "                ga"); break;
                    case 3: memory->setClanTag("               gam", "               gam"); break;
                    case 4: memory->setClanTag("              game", "              game"); break;
                    case 5: memory->setClanTag("             games", "             games"); break;
                    case 6: memory->setClanTag("            gamese", "            gamese"); break;
                    case 7: memory->setClanTag("           gamesen", "           gamesen"); break;
                    case 8: memory->setClanTag("          gamesens", "          gamesens"); break;
                    case 9: memory->setClanTag("         gamesense", "         gamesense"); break;
                    case 10:memory->setClanTag("        gamesense ", "        gamesense "); break;
                    case 11:memory->setClanTag("       gamesense  ", "       gamesense  "); break;
                    case 12:memory->setClanTag("      gamesense   ", "      gamesense   "); break;
                    case 13:memory->setClanTag("     gamesense    ", "     gamesense    "); break;
                    case 14:memory->setClanTag("    gamesense     ", "    gamesense     "); break;
                    case 15:memory->setClanTag("   gamesense      ", "   gamesense      "); break;
                    case 16:memory->setClanTag("  gamesense       ", "  gamesense       "); break;
                    case 17:memory->setClanTag(" gamesense        ", " gamesense        "); break;
                    case 18:memory->setClanTag("gamesense         ", "gamesense         "); break;
                    case 19:memory->setClanTag("amesense          ", "amesense          "); break;
                    case 20:memory->setClanTag("mesense           ", "mesense           "); break;
                    case 22:memory->setClanTag("esense            ", "esense            "); break;
                    case 23:memory->setClanTag("sense             ", "sense             "); break;
                    case 24:memory->setClanTag("sens              ", "sens              "); break;
                    case 25:memory->setClanTag("sen               ", "sen               "); break;
                    case 26:memory->setClanTag("se                ", "se                "); break;
                    case 27:memory->setClanTag("s                 ", "s                 "); break;
                    default:;
                    }
                }
            }
            prevtime = int(memory->globalVars->currenttime);
            break;
        }       
        default:
            break;
        }

        static auto lastTime = 0.0f;
        if (memory->globalVars->realtime - lastTime < 0.6f)
            return;
        lastTime = memory->globalVars->realtime;

        if (config->misc.tagAnimType != 1 && config->misc.tagAnimType != 3) {
            memory->setClanTag(clanTag.c_str(), clanTag.c_str());
        }
    }
}

void Misc::spectatorList() noexcept
{
    if (!config->misc.spectatorList)
        return;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
    if (!gui->open) 
        windowFlags |= ImGuiWindowFlags_NoInputs;
    if (config->misc.wposLockSelectedFlags[16]) {
        windowFlags |= ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoResize;
    }

    const auto [width, height] = interfaces->surface->getScreenSize();
    float textPositionY = static_cast<float>(0.5f * height);

    ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);
    ImGui::SetNextWindowPos({ 0.0f, textPositionY }, ImGuiCond_Once);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::Begin("Spectators List", nullptr, windowFlags);
    ImGui::PopStyleVar();

    if (!config->misc.wposLockSelectedFlags[16]) {
        if (config->misc.wposSpectatorListX != ImGui::GetWindowPos().x) { config->misc.wposSpectatorListX = ImGui::GetWindowPos().x; }
        if (config->misc.wposSpectatorListY != ImGui::GetWindowPos().y) { config->misc.wposSpectatorListY = ImGui::GetWindowPos().y; }
        if (config->misc.wposSpectatorListScaleX != ImGui::GetWindowSize().x) { config->misc.wposSpectatorListScaleX = ImGui::GetWindowSize().x; }
        if (config->misc.wposSpectatorListScaleY != ImGui::GetWindowSize().y) { config->misc.wposSpectatorListScaleY = ImGui::GetWindowSize().y; }
    }
    else {
        if (ImGui::GetWindowPos().x != config->misc.wposSpectatorListX || ImGui::GetWindowPos().y != config->misc.wposSpectatorListY) {
            ImGui::SetWindowPos({ config->misc.wposSpectatorListX, config->misc.wposSpectatorListY });
        }
        if (ImGui::GetWindowSize().x != config->misc.wposSpectatorListScaleX || ImGui::GetWindowSize().y != config->misc.wposSpectatorListScaleY) {
            ImGui::SetWindowSize({ config->misc.wposSpectatorListScaleX, config->misc.wposSpectatorListScaleY });
        }

    }

    for (int i = 1; i <= interfaces->engine->getMaxClients(); ++i) {
        const auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity->isDormant() || entity->isAlive() || entity->getObserverTarget() != localPlayer.get())
            continue;
        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(i, playerInfo))
            continue;
        if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
            ImGui::TextWrapped(" %s ", playerInfo.name);
        }
    }
    ImGui::End();
}

void Misc::sniperCrosshair() noexcept
{
    static auto showSpread = interfaces->cvar->findVar("weapon_debug_spread_show");
    showSpread->setValue(config->misc.sniperCrosshair && localPlayer && !localPlayer->isScoped() ? 3 : 0);
}

void Misc::recoilCrosshair() noexcept
{
    static auto recoilCrosshair = interfaces->cvar->findVar("cl_crosshair_recoil");
    recoilCrosshair->setValue(config->misc.recoilCrosshair ? 1 : 0);
}

void Misc::watermark() noexcept
{
    if (config->misc.watermark.enabled) {
        interfaces->surface->setTextFont(Surface::font);

        if (config->misc.watermark.rainbow)
            interfaces->surface->setTextColor(rainbowColor(memory->globalVars->realtime, config->misc.watermark.rainbowSpeed));
        else
            interfaces->surface->setTextColor(config->misc.watermark.color);

        interfaces->surface->setTextPosition(5, 0);
        interfaces->surface->printText(L"alsware.cc");


        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime;
        const auto [screenWidth, screenHeight] = interfaces->surface->getScreenSize();
        std::wstring fps{ L"FPS: " + std::to_wstring(static_cast<int>(1 / frameRate)) };
        const auto [fpsWidth, fpsHeight] = interfaces->surface->getTextSize(Surface::font, fps.c_str());
        interfaces->surface->setTextPosition(screenWidth - fpsWidth - 5, 0);
        interfaces->surface->printText(fps.c_str());

        float latency = 0.0f;
        if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
            latency = networkChannel->getLatency(0);

        std::wstring ping{ L"PING: " + std::to_wstring(static_cast<int>(latency * 1000)) + L" ms" };
        const auto pingWidth = interfaces->surface->getTextSize(Surface::font, ping.c_str()).first;
        interfaces->surface->setTextPosition(screenWidth - pingWidth - 5, fpsHeight);
        interfaces->surface->printText(ping.c_str());
    }
}

void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };
    constexpr float revolverPrepareTime{ 0.234375f };

    static float readyTime;
    if (config->misc.prepareRevolver && localPlayer && (!config->misc.prepareRevolverKey || GetAsyncKeyState(config->misc.prepareRevolverKey))) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver) {
            if (!readyTime) readyTime = memory->globalVars->serverTime() + revolverPrepareTime;
            auto ticksToReady = timeToTicks(readyTime - memory->globalVars->serverTime() - interfaces->engine->getNetworkChannel()->getLatency(0));
            if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
                cmd->buttons |= UserCmd::IN_ATTACK;
            else
                readyTime = 0.0f;
        }
    }
}

void Misc::fastPlant(UserCmd* cmd) noexcept
{
    if (config->misc.fastPlant) {
        static auto plantAnywhere = interfaces->cvar->findVar("mp_plant_c4_anywhere");

        if (plantAnywhere->getInt())
            return;

        if (!localPlayer || !localPlayer->isAlive() || localPlayer->inBombZone())
            return;

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
            return;

        cmd->buttons &= ~UserCmd::IN_ATTACK;

        constexpr float doorRange{ 200.0f };
        Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * doorRange,
                           cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * doorRange,
                          -sin(degreesToRadians(cmd->viewangles.x)) * doorRange };
        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer.get(), trace);

        if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
            cmd->buttons &= ~UserCmd::IN_USE;
    }
}

void Misc::ChatSpammer() noexcept
{
    if (config->misc.chat_spammer == 1)
    {
        static DWORD g_dTimer = 0;
        if (GetTickCount() > g_dTimer + 850)
        {
            interfaces->engine->ExecuteClientCmd("say ﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽﷽ ﷽﷽");
            g_dTimer = GetTickCount();
        }
    }

    if (config->misc.chat_spammer == 2)
    {
        long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        static long timestamp = curTime;

        if ((curTime - timestamp) < 850)
            return;

        if (config->misc.chat_spammer)
        {
            if (msgs.empty())
                return;

            std::srand(time(NULL));

            std::string str;

            std::rotate(msgs.begin(), msgs.begin() + 1, msgs.end());

            str.append("say ");
            str.append(msgs[0].data());

            interfaces->engine->ExecuteClientCmd(str.c_str());
        }
        timestamp = curTime;
    }

}

void Misc::playerBlocker(UserCmd* cmd) noexcept
{
    if (config->misc.playerBlocker && GetAsyncKeyState(config->misc.playerBlockerKey)) {
        float bestdist = 250.f;
        int index = -1;
        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            auto entity = interfaces->entityList->getEntity(i);

            if (!entity)
                continue;

            if (!entity->isAlive() || entity->isDormant() || entity == localPlayer.get())
                continue;

            float dist;

            double distance;
            distance = sqrt(((int)localPlayer->origin().x - (int)entity->origin().x) * ((int)localPlayer->origin().x - (int)entity->origin().x) +
                ((int)localPlayer->origin().y - (int)entity->origin().y) * ((int)localPlayer->origin().y - (int)entity->origin().y) +
                ((int)localPlayer->origin().z - (int)entity->origin().z) * ((int)localPlayer->origin().z - (int)entity->origin().z));
            dist = (float)abs(round(distance));

            if (dist < bestdist)
            {
                bestdist = dist;
                index = i;
            }
        }

        if (index == -1)
            return;

        auto target = interfaces->entityList->getEntity(index);

        if (!target)
            return;


        Vector delta = target->origin() - localPlayer->origin();
        Vector angles{ radiansToDegrees(atan2f(-delta.z, std::hypotf(delta.x, delta.y))), radiansToDegrees(atan2f(delta.y, delta.x)) };
        angles.normalize();

        angles.y -= localPlayer->eyeAngles().y;
        angles.normalize();
        angles.y = std::clamp(angles.y, -180.f, 180.f);

        if (angles.y < -1.0f)
            cmd->sidemove = 450.f;
        else if (angles.y > 1.0f)
            cmd->sidemove = -450.f;
    }
}

void Misc::customViewmodel(UserCmd* cmd) noexcept 
{
    if (config->misc.CustomViewModel) {
        if (!localPlayer)
            return;
    }
    static ConVar* view_x = interfaces->cvar->findVar("viewmodel_offset_x");
    static ConVar* view_y = interfaces->cvar->findVar("viewmodel_offset_y");
    static ConVar* view_z = interfaces->cvar->findVar("viewmodel_offset_z");
    static ConVar* sv_minspec = interfaces->cvar->findVar("sv_competitive_minspec");

    *(int*)((DWORD)&sv_minspec->onChangeCallbacks + 0xC) = 0;

    sv_minspec->setValue(0);
    view_x->setValue(config->misc.viewmodel_x);
    view_y->setValue(config->misc.viewmodel_y);
    view_z->setValue(config->misc.viewmodel_z);
}

void Misc::UseSpam(UserCmd* cmd) noexcept
{
    static bool usespam = true;
    const auto localPlayer = interfaces->entityList->getEntity(interfaces->engine->getLocalPlayer());
    if (localPlayer->isDefusing()) {
        return;
    }
    if (!localPlayer->isAlive() || localPlayer->inBombZone()) {
        return;
    }
    if (config->misc.usespam && cmd->buttons & UserCmd::IN_USE) {
        if (usespam)
        {
            cmd->buttons |= UserCmd::IN_USE;
            usespam = false;
        }
        else
        {
            cmd->buttons &= ~UserCmd::IN_USE;
            usespam = true;
        }
    }
}

void Misc::drawBombTimer() noexcept
{
    if (config->misc.bombTimer.enabled) {
        for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->c4Ticking())
                continue;

            constexpr unsigned font{ 0xc1 };
            interfaces->surface->setTextFont(font);
            interfaces->surface->setTextColor(255, 255, 255);
            auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
            auto bombText{ (std::wstringstream{ } << L"Bomb on " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f) << L" s").str() };
            const auto bombTextX{ interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces->surface->getTextSize(font, bombText.c_str())).first / 2) };
            interfaces->surface->setTextPosition(bombTextX, drawPositionY);
            drawPositionY += interfaces->surface->getTextSize(font, bombText.c_str()).second;
            interfaces->surface->printText(bombText.c_str());

            const auto progressBarX{ interfaces->surface->getScreenSize().first / 3 };
            const auto progressBarLength{ interfaces->surface->getScreenSize().first / 3 };
            constexpr auto progressBarHeight{ 5 };

            interfaces->surface->setDrawColor(50, 50, 50);
            interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
            if (config->misc.bombTimer.rainbow)
                interfaces->surface->setDrawColor(rainbowColor(memory->globalVars->realtime, config->misc.bombTimer.rainbowSpeed));
            else
                interfaces->surface->setDrawColor(config->misc.bombTimer.color);

            static auto c4Timer = interfaces->cvar->findVar("mp_c4timer");

            interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, static_cast<int>(progressBarX + progressBarLength * std::clamp(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f, c4Timer->getFloat()) / c4Timer->getFloat()), drawPositionY + progressBarHeight + 5);

            if (entity->c4Defuser() != -1) {
                if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
                    if (wchar_t name[128];  MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const auto defusingText{ (std::wstringstream{ } << name << L" is defusing: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) << L" s").str() };

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, defusingText.c_str()).first) / 2, drawPositionY);
                        interfaces->surface->printText(defusingText.c_str());
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;

                        interfaces->surface->setDrawColor(50, 50, 50);
                        interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
                        interfaces->surface->setDrawColor(0, 255, 0);
                        interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, progressBarX + static_cast<int>(progressBarLength * (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) / (interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f)), drawPositionY + progressBarHeight + 5);

                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const wchar_t* canDefuseText;

                        if (entity->c4BlowTime() >= entity->c4DefuseCountDown()) {
                            canDefuseText = L"Can Defuse";
                            interfaces->surface->setTextColor(0, 255, 0);
                        } else {
                            canDefuseText = L"Cannot Defuse";
                            interfaces->surface->setTextColor(255, 0, 0);
                        }

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, canDefuseText).first) / 2, drawPositionY);
                        interfaces->surface->printText(canDefuseText);
                    }
                }
            }
            break;
        }
    }
}

void Misc::drawBombDamage() noexcept
{
    if (!config->misc.bombDmg) return;

    //No Alive return since it is useful if you want to call it out to a mate that he will die
    if (!localPlayer) return;

    for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++)
    {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->
            c4Ticking())
            continue;

        auto vecBombDistance = entity->origin() - localPlayer->origin();

        const auto d = (vecBombDistance.length() - 75.68f) / 789.2f;
        auto flDamage = 450.7f * exp(-d * d);

        if (localPlayer->armor() > 0)
        {
            auto flNew = flDamage * 0.5f;
            auto flArmor = (flDamage - flNew) * 0.5f;

            if (flArmor > localPlayer->armor())
            {
                flArmor = localPlayer->armor() * 2.f;
                flNew = flDamage - flArmor;
            }

            flDamage = flNew;
        }

        //Could get the specator target here as well and set the color based on the spaceted player
        //I'm too lazy for that tho, green while you are dead just looks nicer
        if (localPlayer->isAlive() && max(ceilf(flDamage), 0) >= localPlayer->health())
            interfaces->surface->setTextColor(255, 0, 0);
        else
            interfaces->surface->setTextColor(0, 255, 0);

        auto bombDmgText{ (std::wstringstream{} << L"Bomb Damage: " << max(ceilf(flDamage), 0)).str() };

        constexpr unsigned font{ 0xc1 };
        interfaces->surface->setTextFont(font);

        auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
        const auto bombDmgX{
            interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces->surface->getTextSize(font, bombDmgText.c_str())).first / 2)
        };

        drawPositionY -= interfaces->surface->getTextSize(font, bombDmgText.c_str()).second;

        interfaces->surface->setTextPosition(bombDmgX, drawPositionY);
        interfaces->surface->printText(bombDmgText.c_str());
    }
}


void Misc::stealNames() noexcept
{
    if (!config->misc.nameStealer)
        return;

    if (!localPlayer)
        return;

    static std::vector<int> stolenIds;

    for (int i = 1; i <= memory->globalVars->maxClients; ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(entity->index(), playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(stolenIds.cbegin(), stolenIds.cend(), playerInfo.userId) != stolenIds.cend())
            continue;

        if (changeName(false, (std::string{ playerInfo.name } +'\x1').c_str(), 1.0f))
            stolenIds.push_back(playerInfo.userId);

        return;
    }
    stolenIds.clear();
}

void Misc::disablePanoramablur() noexcept
{
    static auto blur = interfaces->cvar->findVar("@panorama_disable_blur");
    blur->setValue(config->misc.disablePanoramablur);
}

void Misc::quickReload(UserCmd* cmd) noexcept
{
    if (config->misc.quickReload) {
        static Entity* reloadedWeapon{ nullptr };

        if (reloadedWeapon) {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (interfaces->entityList->getEntityFromHandle(weaponHandle) == reloadedWeapon) {
                    cmd->weaponselect = reloadedWeapon->index();
                    cmd->weaponsubtype = reloadedWeapon->getWeaponSubType();
                    break;
                }
            }
            reloadedWeapon = nullptr;
        }

        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->isInReload() && activeWeapon->clip() == activeWeapon->getWeaponData()->maxClip) {
            reloadedWeapon = activeWeapon;

            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon != reloadedWeapon) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    break;
                }
            }
        }
    }
}

bool Misc::changeName(bool reconnect, const char* newName, float delay) noexcept
{
    static auto exploitInitialized{ false };

    static auto name{ interfaces->cvar->findVar("name") };

    if (reconnect) {
        exploitInitialized = false;
        return false;
    }

    if (!exploitInitialized && interfaces->engine->isInGame()) {
        if (PlayerInfo playerInfo; localPlayer && interfaces->engine->getPlayerInfo(localPlayer->index(), playerInfo) && (!strcmp(playerInfo.name, "?empty") || !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
            exploitInitialized = true;
        } else {
            name->onChangeCallbacks.size = 0;
            name->setValue("\n\xAD\xAD\xAD");
            return false;
        }
    }

    static auto nextChangeTime{ 0.0f };
    if (nextChangeTime <= memory->globalVars->realtime) {
        name->setValue(newName);
        nextChangeTime = memory->globalVars->realtime + delay;
        return true;
    }
    return false;
}

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };

    if (config->misc.bunnyHop && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
        cmd->buttons &= ~UserCmd::IN_JUMP;

    wasLastTimeOnGround = localPlayer->flags() & 1;
}

void Misc::fakeBan(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;

    if (shouldSet && interfaces->engine->isInGame() && changeName(false, std::string{ "\x1\xB" }.append(std::string{ static_cast<char>(config->misc.banColor + 1) }).append(config->misc.banText).append("\x1").c_str(), 5.0f))
        shouldSet = false;
}

void Misc::nadePredict() noexcept
{
    static auto nadeVar{ interfaces->cvar->findVar("cl_grenadepreview") };

    nadeVar->onChangeCallbacks.size = 0;
    nadeVar->setValue(config->misc.nadePredict);
}

void Misc::quickHealthshot(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static bool inProgress{ false };

    if (GetAsyncKeyState(config->misc.quickHealthshotKey))
        inProgress = true;

    if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && inProgress) {
        if (activeWeapon->getClientClass()->classId == ClassId::Healthshot && localPlayer->nextAttack() <= memory->globalVars->serverTime() && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime())
            cmd->buttons |= UserCmd::IN_ATTACK;
        else {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (const auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon->getClientClass()->classId == ClassId::Healthshot) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    return;
                }
            }
        }
        inProgress = false;
    }
}

void Misc::fixTabletSignal() noexcept
{
    if (config->misc.fixTabletSignal && localPlayer) {
        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            activeWeapon->tabletReceptionIsBlocked() = false;
    }
}

void Misc::fakePrime() noexcept
{
    static bool lastState = false;

    if (config->misc.fakePrime != lastState) {
        lastState = config->misc.fakePrime;

        if (DWORD oldProtect; VirtualProtect(memory->fakePrime, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            constexpr uint8_t patch[]{ 0x74, 0xEB };
            *memory->fakePrime = patch[config->misc.fakePrime];
            VirtualProtect(memory->fakePrime, 1, oldProtect, nullptr);
        }
    }
}

void Misc::killMessage(GameEvent& event) noexcept
{
    if (!config->misc.killMessage)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    std::string cmd = "say \"";
    cmd += config->misc.killMessageString;
    cmd += '"';
    interfaces->engine->clientCmdUnrestricted(cmd.c_str());
}

void Misc::fixMovement(UserCmd* cmd, float yaw) noexcept
{
    if (config->misc.fixMovement) {
        float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
        float newYaw = cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
        float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
        yawDelta = 360.0f - yawDelta;

        const float forwardmove = cmd->forwardmove;
        const float sidemove = cmd->sidemove;
        cmd->forwardmove = std::cos(degreesToRadians(yawDelta)) * forwardmove + std::cos(degreesToRadians(yawDelta + 90.0f)) * sidemove;
        cmd->sidemove = std::sin(degreesToRadians(yawDelta)) * forwardmove + std::sin(degreesToRadians(yawDelta + 90.0f)) * sidemove;
    }
}

void Misc::antiAfkKick(UserCmd* cmd) noexcept
{
    if (config->misc.antiAfkKick && cmd->commandNumber % 2)
        cmd->buttons |= 1 << 26;
}

void Misc::fixAnimationLOD(FrameStage stage) noexcept
{
    if (config->misc.fixAnimationLOD && stage == FrameStage::RENDER_START) {
        if (!localPlayer)
            return;

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()) continue;
            *reinterpret_cast<int*>(entity + 0xA28) = 0;
            *reinterpret_cast<int*>(entity + 0xA30) = memory->globalVars->framecount;
        }
    }
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (config->misc.autoPistol && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->isPistol() && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime()) {
            if (activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
                cmd->buttons &= ~UserCmd::IN_ATTACK2;
            else
                cmd->buttons &= ~UserCmd::IN_ATTACK;
        }
    }
}

float Misc::RandomFloat(float min, float max) noexcept
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

void Misc::chokePackets(bool& sendPacket, UserCmd* cmd) noexcept
{
    bool doFakeLag{ false };
    auto position = localPlayer->getAbsOrigin();
    auto velocity = localPlayer->velocity();
    auto extrapolatedVelocity = sqrt(sqrt(velocity.x * velocity.y * velocity.z));
    auto& records = config->globals.serverPos;
    float distanceDifToServerSide = sqrt(sqrt(records.origin.x * records.origin.y * records.origin.z));

    if (config->misc.fakeLagMode != 0)
    {
        if ((config->misc.fakeLagSelectedFlags[0] && cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))
            || (config->misc.fakeLagSelectedFlags[1] && !(cmd->buttons& (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT)))
            || (config->misc.fakeLagSelectedFlags[2] && cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT))
            || (config->misc.fakeLagSelectedFlags[3] && !(localPlayer->flags() & 1)))
            doFakeLag = true;
        else
            doFakeLag = false;
    }

    if (doFakeLag)
    {
        if (config->misc.fakeLagMode == 1)
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.fakeLagTicks;
        if (config->misc.fakeLagMode == 2)
        {
            auto requiredPacketsToBreakLagComp = 65 / extrapolatedVelocity;
            if (!(requiredPacketsToBreakLagComp > config->misc.fakeLagTicks) && requiredPacketsToBreakLagComp <= 16)
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= requiredPacketsToBreakLagComp;
            else
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= 16;
        }
        if (config->misc.fakeLagMode == 3)
        {
            float lagTicks = RandomFloat(config->misc.fakeLagTicks, 16);
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= lagTicks;
        }
        if (config->misc.fakeLagMode == 4)
        {
            if (distanceDifToServerSide < 64.f && interfaces->engine->getNetworkChannel()->chokedPackets < 16)
                sendPacket = false;
            else
                sendPacket = true;
        }
    }
}

void Misc::autoReload(UserCmd* cmd) noexcept
{
    if (config->misc.autoReload && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex2()) && !activeWeapon->clip())
            cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
    }
}

void Misc::revealRanks(UserCmd* cmd) noexcept
{
    if (config->misc.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
        interfaces->client->dispatchUserMessage(50, 0, 0, nullptr);
}

void Misc::autoStrafe(UserCmd* cmd) noexcept
{
    //if (cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT)) //strafe check
        //return;

    if (localPlayer
        && config->misc.autoStrafe
        && (!config->misc.autoStrafeKey || GetAsyncKeyState(config->misc.autoStrafeKey))
        && !(localPlayer->flags() & 1)
        && localPlayer->moveType() != MoveType::NOCLIP) {

        if (config->misc.autoStrafeStyle == 0) {
            if (cmd->mousedx < 0)
                cmd->sidemove = -450.0f;
            else if (cmd->mousedx > 0)
                cmd->sidemove = 450.0f;
        }
        else if (config->misc.autoStrafeStyle == 1) {
            float flYawBhop = (localPlayer->velocity().length() > 50.f) ? (30.f * fabsf(30.f / localPlayer->velocity().length())) : 0.f;
            if (cmd->mousedx < 0) {
                cmd->viewangles.y -= flYawBhop;
                cmd->sidemove = -450.0f;
            }
            else if (cmd->mousedx > 0) {
                cmd->viewangles.y += flYawBhop;
                cmd->sidemove = 450.0f;
            }
        }
    }
}

void Misc::jumpbug(UserCmd* cmd) noexcept {
    if (!config->misc.jumpbug || !localPlayer || !localPlayer->isAlive())
        return;

    static bool bhopWasEnabled = false;
    bool JumpDone;

    auto unduck = true;

    float max_radias = M_PI * 2;
    float step = max_radias / 128;
    float xThick = 23;

    if (GetAsyncKeyState(config->misc.jumpbugkey) && (localPlayer->flags() & 1)) {

        if (config->misc.bunnyHop) {
            config->misc.bunnyHop = false;
            bhopWasEnabled = true;
        }

        if (unduck) {
            JumpDone = false;
            cmd->buttons &= ~UserCmd::IN_DUCK;
            // cmd->buttons |= UserCmd::IN_JUMP; // If you want to hold JB key only.
            unduck = false;
        }
        Vector pos = localPlayer->origin();
        for (float a = 0.f; a < max_radias; a += step) {
            Vector pt;
            pt.x = (xThick * cos(a)) + pos.x;
            pt.y = (xThick * sin(a)) + pos.y;
            pt.z = pos.z;

            Vector pt2 = pt;
            pt2.z -= 6;

            Trace target;

            TraceFilter flt = localPlayer.get();

            interfaces->engineTrace->traceRay({ pt, pt2 }, 0x1400B, flt, target);

            if (target.fraction != 1.0f && target.fraction != 0.0f) {
                JumpDone = true;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_JUMP;
                unduck = true;
            }
        }
        for (float a = 0.f; a < max_radias; a += step) {
            Vector pt;
            pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
            pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
            pt.z = pos.z;

            Vector pt2 = pt;
            pt2.z -= 6;

            Trace target;

            TraceFilter flt = localPlayer.get();
            interfaces->engineTrace->traceRay({ pt, pt2 }, 0x1400B, flt, target);

            if (target.fraction != 1.f && target.fraction != 0.f) {
                JumpDone = true;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_JUMP;
                unduck = true;
            }
        }
        for (float a = 0.f; a < max_radias; a += step) {
            Vector pt;
            pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
            pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
            pt.z = pos.z;

            Vector pt2 = pt;
            pt2.z -= 6;

            Trace target;

            TraceFilter flt = localPlayer.get();
            interfaces->engineTrace->traceRay({ pt, pt2 }, 0x1400B, flt, target);

            if (target.fraction != 1.f && target.fraction != 0.f) {
                JumpDone = true;
                cmd->buttons |= UserCmd::IN_DUCK;
                cmd->buttons &= ~UserCmd::IN_JUMP;
                unduck = true;
            }
        }
    }
    else if (bhopWasEnabled) {
        config->misc.bunnyHop = true;
        bhopWasEnabled = false;
    }
}


void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (config->misc.fastDuck)
        cmd->buttons |= UserCmd::IN_BULLRUSH;
}

void Misc::FastStop(UserCmd* cmd) noexcept
{
    if (!config->misc.fastStop)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (localPlayer->moveType() == MoveType::NOCLIP || localPlayer->moveType() == MoveType::LADDER || !(localPlayer->flags() & 1) || cmd->buttons & UserCmd::IN_JUMP)
        return;

    if (cmd->buttons & (UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT | UserCmd::IN_FORWARD | UserCmd::IN_BACK))
        return;

    auto VectorAngles = [](const Vector& forward, Vector& angles)
    {
        if (forward.y == 0.0f && forward.x == 0.0f)
        {
            angles.x = (forward.z > 0.0f) ? 270.0f : 90.0f;
            angles.y = 0.0f;
        }
        else
        {
            angles.x = atan2(-forward.z, forward.length2D()) * -180 / M_PI;
            angles.y = atan2(forward.y, forward.x) * 180 / M_PI;

            if (angles.y > 90)
                angles.y -= 180;
            else if (angles.y < 90)
                angles.y += 180;
            else if (angles.y == 90)    
                angles.y = 0;
        }

        angles.z = 0.0f;
    };
    auto AngleVectors = [](const Vector& angles, Vector* forward)
    {
        float	sp, sy, cp, cy;

        sy = sin(degreesToRadians(angles.y));
        cy = cos(degreesToRadians(angles.y));

        sp = sin(degreesToRadians(angles.x));
        cp = cos(degreesToRadians(angles.x));

        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    };

    Vector velocity = localPlayer->velocity();
    Vector direction;
    VectorAngles(velocity, direction);
    float speed = velocity.length2D();
    if (speed < 15.f)
        return;

    direction.y = cmd->viewangles.y - direction.y;

    Vector forward;
    AngleVectors(direction, &forward);

    Vector negated_direction = forward * speed;

    cmd->forwardmove = negated_direction.x;
    cmd->sidemove = negated_direction.y;
}

void Misc::moonwalk(UserCmd* cmd) noexcept
{
    if (config->misc.moonwalk && localPlayer && localPlayer->moveType() != MoveType::LADDER)
        cmd->buttons ^= UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT;
}

void Misc::playHitSound(GameEvent& event) noexcept
{
    if (!config->misc.hitSound)
        return;

    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array hitSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.hitSound - 1) < hitSounds.size())
        interfaces->engine->clientCmdUnrestricted(hitSounds[config->misc.hitSound - 1]);
    else if (config->misc.hitSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customHitSound).c_str());
}

void Misc::drawAimbotFov() noexcept {
    if (config->misc.drawAimbotFov && interfaces->engine->isInGame()) {
        if (!localPlayer || !localPlayer->isAlive() || !localPlayer->getActiveWeapon()) return;
        int weaponId = getWeaponIndex(localPlayer->getActiveWeapon()->itemDefinitionIndex2());
        if (!config->aimbot[weaponId].enabled) weaponId = 0;
        if (!config->aimbot[weaponId].enabled) return;
        auto [width, heigth] = interfaces->surface->getScreenSize();
        if (config->aimbot[weaponId].silent)
            interfaces->surface->setDrawColor(255, 10, 10, 255);
        else interfaces->surface->setDrawColor(10, 255, 10, 255);
        float radius = std::tan(degreesToRadians(config->aimbot[weaponId].fov / 2.f)) / std::tan(degreesToRadians(config->misc.actualFov / 2.f)) * width;
        interfaces->surface->drawOutlinedCircle(width / 2, heigth / 2, radius, 100);
    }
}

void Misc::killSound(GameEvent& event) noexcept
{
    if (!config->misc.killSound)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array killSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.killSound - 1) < killSounds.size())
        interfaces->engine->clientCmdUnrestricted(killSounds[config->misc.killSound - 1]);
    else if (config->misc.killSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customKillSound).c_str());
}

void Misc::fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept
{
    auto state = localPlayer->getAnimstate();

    if (config->misc.fakeDuck)
    {
        if (config->misc.fakeDuckKey != 0) {
            if (!GetAsyncKeyState(config->misc.fakeDuckKey))
            {
                config->misc.fakeDucking = false;
                return;
            }
            else
                config->misc.fakeDucking = true;
        }

        if (config->misc.fakeDucking)
        {
            if (cmd->buttons & UserCmd::IN_ATTACK || config->misc.fakeDuckShotState != 0)
            {
                if (localPlayer->getAnimstate()->duckAmount > 0.2 && config->misc.fakeDuckShotState == 0)
                {
                    sendPacket = true; // clear up sendPacket for fakeduck going up to choke
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons |= UserCmd::IN_DUCK;
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
                    config->misc.fakeDuckShotState = 1;
                }
                else if (localPlayer->getAnimstate()->duckAmount > 0.2 && config->misc.fakeDuckShotState == 1)
                {
                    sendPacket = false;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons &= ~UserCmd::IN_DUCK;
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
                    config->misc.fakeDuckShotState = 1;
                }
                else if (localPlayer->getAnimstate()->duckAmount <= 0.2 && config->misc.fakeDuckShotState == 1)
                {
                    sendPacket = false;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons &= ~UserCmd::IN_DUCK;
                    cmd->buttons |= UserCmd::IN_ATTACK;
                    config->misc.fakeDuckShotState = 2;
                }
                else if (config->misc.fakeDuckShotState == 2)
                {
                    sendPacket = false;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons |= UserCmd::IN_DUCK;
                    config->misc.fakeDuckShotState = 3;
                }
                else if (config->misc.fakeDuckShotState == 3)
                {
                    sendPacket = true;
                    cmd->buttons |= UserCmd::IN_BULLRUSH;
                    cmd->buttons |= UserCmd::IN_DUCK;
                    config->misc.fakeDuckShotState = 0;
                }
            }
            else
            {
                cmd->buttons |= UserCmd::IN_BULLRUSH;
                cmd->buttons |= UserCmd::IN_DUCK;
                config->misc.fakeDuckShotState = 0;
            }
        }
        else
        {
            config->misc.fakeDuckShotState = 0;
        }
    }
}
void Misc::purchaseList(GameEvent* event) noexcept
{
    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    static std::unordered_map<std::string, std::pair<std::vector<std::string>, int>> purchaseDetails;
    static std::unordered_map<std::string, int> purchaseTotal;
    static int totalCost;

    static auto freezeEnd = 0.0f;

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("item_purchase"): {
            const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

            if (player && localPlayer && memory->isOtherEnemy(player, localPlayer.get())) {
                if (const auto definition = memory->itemSystem()->getItemSchema()->getItemDefinitionByName(event->getString("weapon"))) {
                    if (const auto weaponInfo = memory->weaponSystem->getWeaponInfo(definition->getWeaponId())) {
                        purchaseDetails[player->getPlayerName(true)].second += weaponInfo->price;
                        totalCost += weaponInfo->price;
                    }
                }
                std::string weapon = event->getString("weapon");

                if (weapon.starts_with("weapon_"))
                    weapon.erase(0, 7);
                else if (weapon.starts_with("item_"))
                    weapon.erase(0, 5);

                if (weapon.starts_with("smoke"))
                    weapon = "smoke";
                else if (weapon.starts_with("m4a1_s"))
                    weapon = "m4a1_s";
                else if (weapon.starts_with("usp_s"))
                    weapon = "usp_s";

                purchaseDetails[player->getPlayerName(true)].first.push_back(weapon);
                ++purchaseTotal[weapon];
            }
            break;
        }
        case fnv::hash("round_start"):
            freezeEnd = 0.0f;
            purchaseDetails.clear();
            purchaseTotal.clear();
            totalCost = 0;
            break;
        case fnv::hash("round_freeze_end"):
            freezeEnd = memory->globalVars->realtime;
            break;
        }
    } else {
        if (!config->misc.purchaseList.enabled)
            return;


        static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

        if ((!interfaces->engine->isInGame() || freezeEnd != 0.0f && memory->globalVars->realtime > freezeEnd + (!config->misc.purchaseList.onlyDuringFreezeTime ? mp_buytime->getFloat() : 0.0f) || purchaseDetails.empty() || purchaseTotal.empty()) && !gui->open)
            return;


        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (config->misc.purchaseList.noTitleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;

        if (config->misc.wposLockSelectedFlags[15]) {
            windowFlags |= ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoResize;
        }

        const auto [width, height] = interfaces->surface->getScreenSize();
        float textPositionY = static_cast<float>(0.5f * height);

        ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);
        ImGui::SetNextWindowPos({ 0.0f, textPositionY }, ImGuiCond_Once);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Purchases", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (!config->misc.wposLockSelectedFlags[15]) {
            if (config->misc.wposPurchaseListX != ImGui::GetWindowPos().x) { config->misc.wposPurchaseListX = ImGui::GetWindowPos().x; }
            if (config->misc.wposPurchaseListY != ImGui::GetWindowPos().y) { config->misc.wposPurchaseListY = ImGui::GetWindowPos().y; }
            if (config->misc.wposPurchaseListScaleX != ImGui::GetWindowSize().x) { config->misc.wposPurchaseListScaleX = ImGui::GetWindowSize().x; }
            if (config->misc.wposPurchaseListScaleY != ImGui::GetWindowSize().y) { config->misc.wposPurchaseListScaleY = ImGui::GetWindowSize().y; }
        }
        else {
            if (ImGui::GetWindowPos().x != config->misc.wposPurchaseListX || ImGui::GetWindowPos().y != config->misc.wposPurchaseListY) {
                ImGui::SetWindowPos({ config->misc.wposPurchaseListX, config->misc.wposPurchaseListY });
            }
            if (ImGui::GetWindowSize().x != config->misc.wposPurchaseListScaleX || ImGui::GetWindowSize().y != config->misc.wposPurchaseListScaleY) {
                ImGui::SetWindowSize({ config->misc.wposPurchaseListScaleX, config->misc.wposPurchaseListScaleY });
            }

        }

        if (config->misc.purchaseList.mode == PurchaseList::Details) {
            for (const auto& [playerName, purchases] : purchaseDetails) {
                std::string s = std::accumulate(purchases.first.begin(), purchases.first.end(), std::string{ }, [](std::string s, const std::string& piece) { return s += piece + ", "; });
                if (s.length() >= 2)
                    s.erase(s.length() - 2);

                if (config->misc.purchaseList.showPrices)
                    ImGui::TextWrapped("%s $%d: %s", playerName.c_str(), purchases.second, s.c_str());
                else
                    ImGui::TextWrapped("%s: %s", playerName.c_str(), s.c_str());
            }
        } else if (config->misc.purchaseList.mode == PurchaseList::Summary) {
            for (const auto& purchase : purchaseTotal)
                ImGui::TextWrapped("%d x %s", purchase.second, purchase.first.c_str());

            if (config->misc.purchaseList.showPrices && totalCost > 0) {
                ImGui::Separator();
                ImGui::TextWrapped("Total: $%d", totalCost);
            }
        }
        ImGui::End();
    }
}

// Junk Code By Peatreat & Thaisen's Gen
void AlIClPvpfVRXLyzF74584830() {     int teDFnlEcMHQGBil10632214 = -136221438;    int teDFnlEcMHQGBil86962708 = -940215468;    int teDFnlEcMHQGBil94934311 = -971835383;    int teDFnlEcMHQGBil36140415 = -184495684;    int teDFnlEcMHQGBil24900964 = -1972503;    int teDFnlEcMHQGBil61431753 = -465917574;    int teDFnlEcMHQGBil80490053 = -447894130;    int teDFnlEcMHQGBil91179089 = -661605685;    int teDFnlEcMHQGBil5402155 = -537968878;    int teDFnlEcMHQGBil86697547 = -600709097;    int teDFnlEcMHQGBil27702919 = -153008804;    int teDFnlEcMHQGBil47858576 = -980946951;    int teDFnlEcMHQGBil27629344 = -89486280;    int teDFnlEcMHQGBil83996734 = -287611179;    int teDFnlEcMHQGBil35695797 = -256516563;    int teDFnlEcMHQGBil79969437 = -114711608;    int teDFnlEcMHQGBil66782600 = -455817406;    int teDFnlEcMHQGBil19710255 = -479553384;    int teDFnlEcMHQGBil9140435 = -38346305;    int teDFnlEcMHQGBil6643678 = -428902908;    int teDFnlEcMHQGBil37462243 = -898435702;    int teDFnlEcMHQGBil43506924 = -802173110;    int teDFnlEcMHQGBil55452520 = -13362264;    int teDFnlEcMHQGBil65050638 = -586957794;    int teDFnlEcMHQGBil34029142 = -301445706;    int teDFnlEcMHQGBil1423534 = 22324326;    int teDFnlEcMHQGBil80360889 = -879739746;    int teDFnlEcMHQGBil69500468 = -232450340;    int teDFnlEcMHQGBil28053851 = -582288239;    int teDFnlEcMHQGBil27167039 = -126542029;    int teDFnlEcMHQGBil58988053 = -699591955;    int teDFnlEcMHQGBil41127302 = -835215894;    int teDFnlEcMHQGBil29703412 = -650055225;    int teDFnlEcMHQGBil87528795 = -826811865;    int teDFnlEcMHQGBil6786367 = -352943373;    int teDFnlEcMHQGBil1850525 = -445016230;    int teDFnlEcMHQGBil58457268 = 78326496;    int teDFnlEcMHQGBil78980562 = -38801473;    int teDFnlEcMHQGBil86705221 = -724096500;    int teDFnlEcMHQGBil76118751 = -535149850;    int teDFnlEcMHQGBil19113997 = -718398520;    int teDFnlEcMHQGBil36638829 = -487626830;    int teDFnlEcMHQGBil53380833 = -360151208;    int teDFnlEcMHQGBil25674427 = -844822612;    int teDFnlEcMHQGBil17940940 = -310719356;    int teDFnlEcMHQGBil96680487 = -245309353;    int teDFnlEcMHQGBil45535506 = -496204180;    int teDFnlEcMHQGBil79770787 = -866826652;    int teDFnlEcMHQGBil7982028 = 52022259;    int teDFnlEcMHQGBil12241943 = -839761638;    int teDFnlEcMHQGBil36354987 = -354814072;    int teDFnlEcMHQGBil83196475 = -711797109;    int teDFnlEcMHQGBil63784803 = -35050807;    int teDFnlEcMHQGBil29532511 = -493482843;    int teDFnlEcMHQGBil37122587 = -756545749;    int teDFnlEcMHQGBil67125290 = -334048329;    int teDFnlEcMHQGBil31510189 = -826853204;    int teDFnlEcMHQGBil29883674 = -284877590;    int teDFnlEcMHQGBil2111274 = -883049979;    int teDFnlEcMHQGBil23477431 = 75703171;    int teDFnlEcMHQGBil81070864 = -586177828;    int teDFnlEcMHQGBil10989586 = -115443791;    int teDFnlEcMHQGBil63125238 = 20682553;    int teDFnlEcMHQGBil78235115 = -311426850;    int teDFnlEcMHQGBil27709495 = -901117143;    int teDFnlEcMHQGBil86575616 = -317792911;    int teDFnlEcMHQGBil18155164 = -230891726;    int teDFnlEcMHQGBil40100548 = -262674415;    int teDFnlEcMHQGBil77210368 = -934667807;    int teDFnlEcMHQGBil33845272 = -811500333;    int teDFnlEcMHQGBil21512170 = -93038104;    int teDFnlEcMHQGBil87802038 = -317015933;    int teDFnlEcMHQGBil33005033 = -755456885;    int teDFnlEcMHQGBil33021683 = -503196456;    int teDFnlEcMHQGBil87529681 = -710504389;    int teDFnlEcMHQGBil823414 = -310808872;    int teDFnlEcMHQGBil90126090 = -342021902;    int teDFnlEcMHQGBil29778093 = -168539652;    int teDFnlEcMHQGBil47109699 = -176238438;    int teDFnlEcMHQGBil37348654 = 43863647;    int teDFnlEcMHQGBil55888027 = -481471494;    int teDFnlEcMHQGBil590102 = 87086905;    int teDFnlEcMHQGBil61518440 = -184472599;    int teDFnlEcMHQGBil15811909 = -742526601;    int teDFnlEcMHQGBil90812052 = -771727957;    int teDFnlEcMHQGBil75791577 = -987794846;    int teDFnlEcMHQGBil77342498 = -700165087;    int teDFnlEcMHQGBil170902 = -56572383;    int teDFnlEcMHQGBil50406208 = 29733883;    int teDFnlEcMHQGBil39661077 = 81104956;    int teDFnlEcMHQGBil70340336 = -618163027;    int teDFnlEcMHQGBil28573595 = -636795915;    int teDFnlEcMHQGBil76869288 = -155751495;    int teDFnlEcMHQGBil63227791 = -699799672;    int teDFnlEcMHQGBil95047886 = -948972022;    int teDFnlEcMHQGBil8124412 = -502954729;    int teDFnlEcMHQGBil73513591 = -408309384;    int teDFnlEcMHQGBil75145717 = 51275641;    int teDFnlEcMHQGBil97964931 = -943705470;    int teDFnlEcMHQGBil31365323 = -136221438;     teDFnlEcMHQGBil10632214 = teDFnlEcMHQGBil86962708;     teDFnlEcMHQGBil86962708 = teDFnlEcMHQGBil94934311;     teDFnlEcMHQGBil94934311 = teDFnlEcMHQGBil36140415;     teDFnlEcMHQGBil36140415 = teDFnlEcMHQGBil24900964;     teDFnlEcMHQGBil24900964 = teDFnlEcMHQGBil61431753;     teDFnlEcMHQGBil61431753 = teDFnlEcMHQGBil80490053;     teDFnlEcMHQGBil80490053 = teDFnlEcMHQGBil91179089;     teDFnlEcMHQGBil91179089 = teDFnlEcMHQGBil5402155;     teDFnlEcMHQGBil5402155 = teDFnlEcMHQGBil86697547;     teDFnlEcMHQGBil86697547 = teDFnlEcMHQGBil27702919;     teDFnlEcMHQGBil27702919 = teDFnlEcMHQGBil47858576;     teDFnlEcMHQGBil47858576 = teDFnlEcMHQGBil27629344;     teDFnlEcMHQGBil27629344 = teDFnlEcMHQGBil83996734;     teDFnlEcMHQGBil83996734 = teDFnlEcMHQGBil35695797;     teDFnlEcMHQGBil35695797 = teDFnlEcMHQGBil79969437;     teDFnlEcMHQGBil79969437 = teDFnlEcMHQGBil66782600;     teDFnlEcMHQGBil66782600 = teDFnlEcMHQGBil19710255;     teDFnlEcMHQGBil19710255 = teDFnlEcMHQGBil9140435;     teDFnlEcMHQGBil9140435 = teDFnlEcMHQGBil6643678;     teDFnlEcMHQGBil6643678 = teDFnlEcMHQGBil37462243;     teDFnlEcMHQGBil37462243 = teDFnlEcMHQGBil43506924;     teDFnlEcMHQGBil43506924 = teDFnlEcMHQGBil55452520;     teDFnlEcMHQGBil55452520 = teDFnlEcMHQGBil65050638;     teDFnlEcMHQGBil65050638 = teDFnlEcMHQGBil34029142;     teDFnlEcMHQGBil34029142 = teDFnlEcMHQGBil1423534;     teDFnlEcMHQGBil1423534 = teDFnlEcMHQGBil80360889;     teDFnlEcMHQGBil80360889 = teDFnlEcMHQGBil69500468;     teDFnlEcMHQGBil69500468 = teDFnlEcMHQGBil28053851;     teDFnlEcMHQGBil28053851 = teDFnlEcMHQGBil27167039;     teDFnlEcMHQGBil27167039 = teDFnlEcMHQGBil58988053;     teDFnlEcMHQGBil58988053 = teDFnlEcMHQGBil41127302;     teDFnlEcMHQGBil41127302 = teDFnlEcMHQGBil29703412;     teDFnlEcMHQGBil29703412 = teDFnlEcMHQGBil87528795;     teDFnlEcMHQGBil87528795 = teDFnlEcMHQGBil6786367;     teDFnlEcMHQGBil6786367 = teDFnlEcMHQGBil1850525;     teDFnlEcMHQGBil1850525 = teDFnlEcMHQGBil58457268;     teDFnlEcMHQGBil58457268 = teDFnlEcMHQGBil78980562;     teDFnlEcMHQGBil78980562 = teDFnlEcMHQGBil86705221;     teDFnlEcMHQGBil86705221 = teDFnlEcMHQGBil76118751;     teDFnlEcMHQGBil76118751 = teDFnlEcMHQGBil19113997;     teDFnlEcMHQGBil19113997 = teDFnlEcMHQGBil36638829;     teDFnlEcMHQGBil36638829 = teDFnlEcMHQGBil53380833;     teDFnlEcMHQGBil53380833 = teDFnlEcMHQGBil25674427;     teDFnlEcMHQGBil25674427 = teDFnlEcMHQGBil17940940;     teDFnlEcMHQGBil17940940 = teDFnlEcMHQGBil96680487;     teDFnlEcMHQGBil96680487 = teDFnlEcMHQGBil45535506;     teDFnlEcMHQGBil45535506 = teDFnlEcMHQGBil79770787;     teDFnlEcMHQGBil79770787 = teDFnlEcMHQGBil7982028;     teDFnlEcMHQGBil7982028 = teDFnlEcMHQGBil12241943;     teDFnlEcMHQGBil12241943 = teDFnlEcMHQGBil36354987;     teDFnlEcMHQGBil36354987 = teDFnlEcMHQGBil83196475;     teDFnlEcMHQGBil83196475 = teDFnlEcMHQGBil63784803;     teDFnlEcMHQGBil63784803 = teDFnlEcMHQGBil29532511;     teDFnlEcMHQGBil29532511 = teDFnlEcMHQGBil37122587;     teDFnlEcMHQGBil37122587 = teDFnlEcMHQGBil67125290;     teDFnlEcMHQGBil67125290 = teDFnlEcMHQGBil31510189;     teDFnlEcMHQGBil31510189 = teDFnlEcMHQGBil29883674;     teDFnlEcMHQGBil29883674 = teDFnlEcMHQGBil2111274;     teDFnlEcMHQGBil2111274 = teDFnlEcMHQGBil23477431;     teDFnlEcMHQGBil23477431 = teDFnlEcMHQGBil81070864;     teDFnlEcMHQGBil81070864 = teDFnlEcMHQGBil10989586;     teDFnlEcMHQGBil10989586 = teDFnlEcMHQGBil63125238;     teDFnlEcMHQGBil63125238 = teDFnlEcMHQGBil78235115;     teDFnlEcMHQGBil78235115 = teDFnlEcMHQGBil27709495;     teDFnlEcMHQGBil27709495 = teDFnlEcMHQGBil86575616;     teDFnlEcMHQGBil86575616 = teDFnlEcMHQGBil18155164;     teDFnlEcMHQGBil18155164 = teDFnlEcMHQGBil40100548;     teDFnlEcMHQGBil40100548 = teDFnlEcMHQGBil77210368;     teDFnlEcMHQGBil77210368 = teDFnlEcMHQGBil33845272;     teDFnlEcMHQGBil33845272 = teDFnlEcMHQGBil21512170;     teDFnlEcMHQGBil21512170 = teDFnlEcMHQGBil87802038;     teDFnlEcMHQGBil87802038 = teDFnlEcMHQGBil33005033;     teDFnlEcMHQGBil33005033 = teDFnlEcMHQGBil33021683;     teDFnlEcMHQGBil33021683 = teDFnlEcMHQGBil87529681;     teDFnlEcMHQGBil87529681 = teDFnlEcMHQGBil823414;     teDFnlEcMHQGBil823414 = teDFnlEcMHQGBil90126090;     teDFnlEcMHQGBil90126090 = teDFnlEcMHQGBil29778093;     teDFnlEcMHQGBil29778093 = teDFnlEcMHQGBil47109699;     teDFnlEcMHQGBil47109699 = teDFnlEcMHQGBil37348654;     teDFnlEcMHQGBil37348654 = teDFnlEcMHQGBil55888027;     teDFnlEcMHQGBil55888027 = teDFnlEcMHQGBil590102;     teDFnlEcMHQGBil590102 = teDFnlEcMHQGBil61518440;     teDFnlEcMHQGBil61518440 = teDFnlEcMHQGBil15811909;     teDFnlEcMHQGBil15811909 = teDFnlEcMHQGBil90812052;     teDFnlEcMHQGBil90812052 = teDFnlEcMHQGBil75791577;     teDFnlEcMHQGBil75791577 = teDFnlEcMHQGBil77342498;     teDFnlEcMHQGBil77342498 = teDFnlEcMHQGBil170902;     teDFnlEcMHQGBil170902 = teDFnlEcMHQGBil50406208;     teDFnlEcMHQGBil50406208 = teDFnlEcMHQGBil39661077;     teDFnlEcMHQGBil39661077 = teDFnlEcMHQGBil70340336;     teDFnlEcMHQGBil70340336 = teDFnlEcMHQGBil28573595;     teDFnlEcMHQGBil28573595 = teDFnlEcMHQGBil76869288;     teDFnlEcMHQGBil76869288 = teDFnlEcMHQGBil63227791;     teDFnlEcMHQGBil63227791 = teDFnlEcMHQGBil95047886;     teDFnlEcMHQGBil95047886 = teDFnlEcMHQGBil8124412;     teDFnlEcMHQGBil8124412 = teDFnlEcMHQGBil73513591;     teDFnlEcMHQGBil73513591 = teDFnlEcMHQGBil75145717;     teDFnlEcMHQGBil75145717 = teDFnlEcMHQGBil97964931;     teDFnlEcMHQGBil97964931 = teDFnlEcMHQGBil31365323;     teDFnlEcMHQGBil31365323 = teDFnlEcMHQGBil10632214;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void JmdlPKkYZUPHsNew41546232() {     int sGcAOJDVWZMrwHV752051 = -67501783;    int sGcAOJDVWZMrwHV8660090 = -257736769;    int sGcAOJDVWZMrwHV91740222 = 34241542;    int sGcAOJDVWZMrwHV94718850 = -191208333;    int sGcAOJDVWZMrwHV46797802 = -411651274;    int sGcAOJDVWZMrwHV77023590 = -57983493;    int sGcAOJDVWZMrwHV43700828 = -889976070;    int sGcAOJDVWZMrwHV13271703 = -2230756;    int sGcAOJDVWZMrwHV11954007 = -259507587;    int sGcAOJDVWZMrwHV87592731 = -907390494;    int sGcAOJDVWZMrwHV24175343 = -620901035;    int sGcAOJDVWZMrwHV96899760 = -196013458;    int sGcAOJDVWZMrwHV97828389 = -423380615;    int sGcAOJDVWZMrwHV76259808 = -630925116;    int sGcAOJDVWZMrwHV78942277 = -704350376;    int sGcAOJDVWZMrwHV78713727 = -147887128;    int sGcAOJDVWZMrwHV66597540 = -536777217;    int sGcAOJDVWZMrwHV64066500 = -219474302;    int sGcAOJDVWZMrwHV90483095 = -22739006;    int sGcAOJDVWZMrwHV46041689 = -588805669;    int sGcAOJDVWZMrwHV5054070 = -313327173;    int sGcAOJDVWZMrwHV19884772 = -545261628;    int sGcAOJDVWZMrwHV25569335 = -878780027;    int sGcAOJDVWZMrwHV16621844 = -915902692;    int sGcAOJDVWZMrwHV9357020 = -271650515;    int sGcAOJDVWZMrwHV36788576 = -861755333;    int sGcAOJDVWZMrwHV72549805 = -940068453;    int sGcAOJDVWZMrwHV28074369 = -901307392;    int sGcAOJDVWZMrwHV1941296 = -890557587;    int sGcAOJDVWZMrwHV51045989 = -776375794;    int sGcAOJDVWZMrwHV95864134 = -313351733;    int sGcAOJDVWZMrwHV25218073 = -627862072;    int sGcAOJDVWZMrwHV24124876 = -52224598;    int sGcAOJDVWZMrwHV11650278 = -694944756;    int sGcAOJDVWZMrwHV24740939 = -262592955;    int sGcAOJDVWZMrwHV97909721 = -827811368;    int sGcAOJDVWZMrwHV70632204 = -637142787;    int sGcAOJDVWZMrwHV73255524 = 99876601;    int sGcAOJDVWZMrwHV32551836 = -44239949;    int sGcAOJDVWZMrwHV92503784 = -49289309;    int sGcAOJDVWZMrwHV20669338 = -512203694;    int sGcAOJDVWZMrwHV76980953 = -495912105;    int sGcAOJDVWZMrwHV73481034 = -294707056;    int sGcAOJDVWZMrwHV15568266 = -265870245;    int sGcAOJDVWZMrwHV93923409 = -896488713;    int sGcAOJDVWZMrwHV57547450 = -540977911;    int sGcAOJDVWZMrwHV53656588 = -525967302;    int sGcAOJDVWZMrwHV45540828 = -402244260;    int sGcAOJDVWZMrwHV54559071 = 52319076;    int sGcAOJDVWZMrwHV61567771 = -883782867;    int sGcAOJDVWZMrwHV78979880 = -307751124;    int sGcAOJDVWZMrwHV41516840 = 41630314;    int sGcAOJDVWZMrwHV88685605 = -738957813;    int sGcAOJDVWZMrwHV92689843 = -48957976;    int sGcAOJDVWZMrwHV36492163 = -642045293;    int sGcAOJDVWZMrwHV80867279 = -522240155;    int sGcAOJDVWZMrwHV83090754 = -378956743;    int sGcAOJDVWZMrwHV75118379 = -49855766;    int sGcAOJDVWZMrwHV85361831 = -919557819;    int sGcAOJDVWZMrwHV10009227 = -549895941;    int sGcAOJDVWZMrwHV4473785 = -117915041;    int sGcAOJDVWZMrwHV15626460 = -988668678;    int sGcAOJDVWZMrwHV11330407 = -111673170;    int sGcAOJDVWZMrwHV60908017 = -483131794;    int sGcAOJDVWZMrwHV91728597 = -494038761;    int sGcAOJDVWZMrwHV98957269 = -993038964;    int sGcAOJDVWZMrwHV72774884 = -43788860;    int sGcAOJDVWZMrwHV86178112 = -728435860;    int sGcAOJDVWZMrwHV51518869 = -268332162;    int sGcAOJDVWZMrwHV81032556 = -876539008;    int sGcAOJDVWZMrwHV8081523 = -510744342;    int sGcAOJDVWZMrwHV93342015 = -536653819;    int sGcAOJDVWZMrwHV31514665 = -75234353;    int sGcAOJDVWZMrwHV97979310 = -973449698;    int sGcAOJDVWZMrwHV25372352 = 23398024;    int sGcAOJDVWZMrwHV28073117 = -817415068;    int sGcAOJDVWZMrwHV46403737 = -150554573;    int sGcAOJDVWZMrwHV10001069 = -512909783;    int sGcAOJDVWZMrwHV22698435 = 80586021;    int sGcAOJDVWZMrwHV51809569 = -730672605;    int sGcAOJDVWZMrwHV83131987 = -235788031;    int sGcAOJDVWZMrwHV27008978 = -437824193;    int sGcAOJDVWZMrwHV73515297 = -853626469;    int sGcAOJDVWZMrwHV40373525 = 93225280;    int sGcAOJDVWZMrwHV72066109 = -368624670;    int sGcAOJDVWZMrwHV54347294 = -254982048;    int sGcAOJDVWZMrwHV36532467 = -888904259;    int sGcAOJDVWZMrwHV31435033 = 96733377;    int sGcAOJDVWZMrwHV75158115 = 47100537;    int sGcAOJDVWZMrwHV43873659 = -740352801;    int sGcAOJDVWZMrwHV14818967 = -348854626;    int sGcAOJDVWZMrwHV95513825 = -487287021;    int sGcAOJDVWZMrwHV87893693 = 19434419;    int sGcAOJDVWZMrwHV22542609 = -494344009;    int sGcAOJDVWZMrwHV88030000 = -931374268;    int sGcAOJDVWZMrwHV5042879 = -523535016;    int sGcAOJDVWZMrwHV65650546 = -284238935;    int sGcAOJDVWZMrwHV12573017 = -811575263;    int sGcAOJDVWZMrwHV23839669 = -771831484;    int sGcAOJDVWZMrwHV94966139 = -67501783;     sGcAOJDVWZMrwHV752051 = sGcAOJDVWZMrwHV8660090;     sGcAOJDVWZMrwHV8660090 = sGcAOJDVWZMrwHV91740222;     sGcAOJDVWZMrwHV91740222 = sGcAOJDVWZMrwHV94718850;     sGcAOJDVWZMrwHV94718850 = sGcAOJDVWZMrwHV46797802;     sGcAOJDVWZMrwHV46797802 = sGcAOJDVWZMrwHV77023590;     sGcAOJDVWZMrwHV77023590 = sGcAOJDVWZMrwHV43700828;     sGcAOJDVWZMrwHV43700828 = sGcAOJDVWZMrwHV13271703;     sGcAOJDVWZMrwHV13271703 = sGcAOJDVWZMrwHV11954007;     sGcAOJDVWZMrwHV11954007 = sGcAOJDVWZMrwHV87592731;     sGcAOJDVWZMrwHV87592731 = sGcAOJDVWZMrwHV24175343;     sGcAOJDVWZMrwHV24175343 = sGcAOJDVWZMrwHV96899760;     sGcAOJDVWZMrwHV96899760 = sGcAOJDVWZMrwHV97828389;     sGcAOJDVWZMrwHV97828389 = sGcAOJDVWZMrwHV76259808;     sGcAOJDVWZMrwHV76259808 = sGcAOJDVWZMrwHV78942277;     sGcAOJDVWZMrwHV78942277 = sGcAOJDVWZMrwHV78713727;     sGcAOJDVWZMrwHV78713727 = sGcAOJDVWZMrwHV66597540;     sGcAOJDVWZMrwHV66597540 = sGcAOJDVWZMrwHV64066500;     sGcAOJDVWZMrwHV64066500 = sGcAOJDVWZMrwHV90483095;     sGcAOJDVWZMrwHV90483095 = sGcAOJDVWZMrwHV46041689;     sGcAOJDVWZMrwHV46041689 = sGcAOJDVWZMrwHV5054070;     sGcAOJDVWZMrwHV5054070 = sGcAOJDVWZMrwHV19884772;     sGcAOJDVWZMrwHV19884772 = sGcAOJDVWZMrwHV25569335;     sGcAOJDVWZMrwHV25569335 = sGcAOJDVWZMrwHV16621844;     sGcAOJDVWZMrwHV16621844 = sGcAOJDVWZMrwHV9357020;     sGcAOJDVWZMrwHV9357020 = sGcAOJDVWZMrwHV36788576;     sGcAOJDVWZMrwHV36788576 = sGcAOJDVWZMrwHV72549805;     sGcAOJDVWZMrwHV72549805 = sGcAOJDVWZMrwHV28074369;     sGcAOJDVWZMrwHV28074369 = sGcAOJDVWZMrwHV1941296;     sGcAOJDVWZMrwHV1941296 = sGcAOJDVWZMrwHV51045989;     sGcAOJDVWZMrwHV51045989 = sGcAOJDVWZMrwHV95864134;     sGcAOJDVWZMrwHV95864134 = sGcAOJDVWZMrwHV25218073;     sGcAOJDVWZMrwHV25218073 = sGcAOJDVWZMrwHV24124876;     sGcAOJDVWZMrwHV24124876 = sGcAOJDVWZMrwHV11650278;     sGcAOJDVWZMrwHV11650278 = sGcAOJDVWZMrwHV24740939;     sGcAOJDVWZMrwHV24740939 = sGcAOJDVWZMrwHV97909721;     sGcAOJDVWZMrwHV97909721 = sGcAOJDVWZMrwHV70632204;     sGcAOJDVWZMrwHV70632204 = sGcAOJDVWZMrwHV73255524;     sGcAOJDVWZMrwHV73255524 = sGcAOJDVWZMrwHV32551836;     sGcAOJDVWZMrwHV32551836 = sGcAOJDVWZMrwHV92503784;     sGcAOJDVWZMrwHV92503784 = sGcAOJDVWZMrwHV20669338;     sGcAOJDVWZMrwHV20669338 = sGcAOJDVWZMrwHV76980953;     sGcAOJDVWZMrwHV76980953 = sGcAOJDVWZMrwHV73481034;     sGcAOJDVWZMrwHV73481034 = sGcAOJDVWZMrwHV15568266;     sGcAOJDVWZMrwHV15568266 = sGcAOJDVWZMrwHV93923409;     sGcAOJDVWZMrwHV93923409 = sGcAOJDVWZMrwHV57547450;     sGcAOJDVWZMrwHV57547450 = sGcAOJDVWZMrwHV53656588;     sGcAOJDVWZMrwHV53656588 = sGcAOJDVWZMrwHV45540828;     sGcAOJDVWZMrwHV45540828 = sGcAOJDVWZMrwHV54559071;     sGcAOJDVWZMrwHV54559071 = sGcAOJDVWZMrwHV61567771;     sGcAOJDVWZMrwHV61567771 = sGcAOJDVWZMrwHV78979880;     sGcAOJDVWZMrwHV78979880 = sGcAOJDVWZMrwHV41516840;     sGcAOJDVWZMrwHV41516840 = sGcAOJDVWZMrwHV88685605;     sGcAOJDVWZMrwHV88685605 = sGcAOJDVWZMrwHV92689843;     sGcAOJDVWZMrwHV92689843 = sGcAOJDVWZMrwHV36492163;     sGcAOJDVWZMrwHV36492163 = sGcAOJDVWZMrwHV80867279;     sGcAOJDVWZMrwHV80867279 = sGcAOJDVWZMrwHV83090754;     sGcAOJDVWZMrwHV83090754 = sGcAOJDVWZMrwHV75118379;     sGcAOJDVWZMrwHV75118379 = sGcAOJDVWZMrwHV85361831;     sGcAOJDVWZMrwHV85361831 = sGcAOJDVWZMrwHV10009227;     sGcAOJDVWZMrwHV10009227 = sGcAOJDVWZMrwHV4473785;     sGcAOJDVWZMrwHV4473785 = sGcAOJDVWZMrwHV15626460;     sGcAOJDVWZMrwHV15626460 = sGcAOJDVWZMrwHV11330407;     sGcAOJDVWZMrwHV11330407 = sGcAOJDVWZMrwHV60908017;     sGcAOJDVWZMrwHV60908017 = sGcAOJDVWZMrwHV91728597;     sGcAOJDVWZMrwHV91728597 = sGcAOJDVWZMrwHV98957269;     sGcAOJDVWZMrwHV98957269 = sGcAOJDVWZMrwHV72774884;     sGcAOJDVWZMrwHV72774884 = sGcAOJDVWZMrwHV86178112;     sGcAOJDVWZMrwHV86178112 = sGcAOJDVWZMrwHV51518869;     sGcAOJDVWZMrwHV51518869 = sGcAOJDVWZMrwHV81032556;     sGcAOJDVWZMrwHV81032556 = sGcAOJDVWZMrwHV8081523;     sGcAOJDVWZMrwHV8081523 = sGcAOJDVWZMrwHV93342015;     sGcAOJDVWZMrwHV93342015 = sGcAOJDVWZMrwHV31514665;     sGcAOJDVWZMrwHV31514665 = sGcAOJDVWZMrwHV97979310;     sGcAOJDVWZMrwHV97979310 = sGcAOJDVWZMrwHV25372352;     sGcAOJDVWZMrwHV25372352 = sGcAOJDVWZMrwHV28073117;     sGcAOJDVWZMrwHV28073117 = sGcAOJDVWZMrwHV46403737;     sGcAOJDVWZMrwHV46403737 = sGcAOJDVWZMrwHV10001069;     sGcAOJDVWZMrwHV10001069 = sGcAOJDVWZMrwHV22698435;     sGcAOJDVWZMrwHV22698435 = sGcAOJDVWZMrwHV51809569;     sGcAOJDVWZMrwHV51809569 = sGcAOJDVWZMrwHV83131987;     sGcAOJDVWZMrwHV83131987 = sGcAOJDVWZMrwHV27008978;     sGcAOJDVWZMrwHV27008978 = sGcAOJDVWZMrwHV73515297;     sGcAOJDVWZMrwHV73515297 = sGcAOJDVWZMrwHV40373525;     sGcAOJDVWZMrwHV40373525 = sGcAOJDVWZMrwHV72066109;     sGcAOJDVWZMrwHV72066109 = sGcAOJDVWZMrwHV54347294;     sGcAOJDVWZMrwHV54347294 = sGcAOJDVWZMrwHV36532467;     sGcAOJDVWZMrwHV36532467 = sGcAOJDVWZMrwHV31435033;     sGcAOJDVWZMrwHV31435033 = sGcAOJDVWZMrwHV75158115;     sGcAOJDVWZMrwHV75158115 = sGcAOJDVWZMrwHV43873659;     sGcAOJDVWZMrwHV43873659 = sGcAOJDVWZMrwHV14818967;     sGcAOJDVWZMrwHV14818967 = sGcAOJDVWZMrwHV95513825;     sGcAOJDVWZMrwHV95513825 = sGcAOJDVWZMrwHV87893693;     sGcAOJDVWZMrwHV87893693 = sGcAOJDVWZMrwHV22542609;     sGcAOJDVWZMrwHV22542609 = sGcAOJDVWZMrwHV88030000;     sGcAOJDVWZMrwHV88030000 = sGcAOJDVWZMrwHV5042879;     sGcAOJDVWZMrwHV5042879 = sGcAOJDVWZMrwHV65650546;     sGcAOJDVWZMrwHV65650546 = sGcAOJDVWZMrwHV12573017;     sGcAOJDVWZMrwHV12573017 = sGcAOJDVWZMrwHV23839669;     sGcAOJDVWZMrwHV23839669 = sGcAOJDVWZMrwHV94966139;     sGcAOJDVWZMrwHV94966139 = sGcAOJDVWZMrwHV752051;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WWMLbKdkpwNHggUu48204977() {     int olFqAWpJNhmNsxa745580 = 59974076;    int olFqAWpJNhmNsxa47232406 = -300331894;    int olFqAWpJNhmNsxa93053434 = 77112929;    int olFqAWpJNhmNsxa79798711 = -11749164;    int olFqAWpJNhmNsxa55637904 = -880006596;    int olFqAWpJNhmNsxa11829023 = -857416305;    int olFqAWpJNhmNsxa12248164 = -594512537;    int olFqAWpJNhmNsxa82485753 = -806408074;    int olFqAWpJNhmNsxa94694170 = 26846343;    int olFqAWpJNhmNsxa84232245 = -544197365;    int olFqAWpJNhmNsxa44495948 = -580373107;    int olFqAWpJNhmNsxa11191438 = 68508180;    int olFqAWpJNhmNsxa29681304 = -445039089;    int olFqAWpJNhmNsxa30104029 = -976078168;    int olFqAWpJNhmNsxa24114387 = -667063228;    int olFqAWpJNhmNsxa53278275 = -480682779;    int olFqAWpJNhmNsxa22450300 = -356391787;    int olFqAWpJNhmNsxa87067032 = -616545924;    int olFqAWpJNhmNsxa10911366 = -640721273;    int olFqAWpJNhmNsxa18115452 = -100830120;    int olFqAWpJNhmNsxa84723494 = -23793546;    int olFqAWpJNhmNsxa98181037 = 44746489;    int olFqAWpJNhmNsxa62447731 = -237737869;    int olFqAWpJNhmNsxa60271952 = -796823214;    int olFqAWpJNhmNsxa37799709 = -698344368;    int olFqAWpJNhmNsxa53362653 = -769961262;    int olFqAWpJNhmNsxa42771387 = -140868166;    int olFqAWpJNhmNsxa63477893 = -410274381;    int olFqAWpJNhmNsxa14619743 = -436627901;    int olFqAWpJNhmNsxa27135859 = -451807168;    int olFqAWpJNhmNsxa29276813 = -700444967;    int olFqAWpJNhmNsxa9941931 = 80316167;    int olFqAWpJNhmNsxa9577300 = -254168814;    int olFqAWpJNhmNsxa11933115 = -571626488;    int olFqAWpJNhmNsxa7098977 = -521906880;    int olFqAWpJNhmNsxa40737911 = -907577114;    int olFqAWpJNhmNsxa82573572 = -736796165;    int olFqAWpJNhmNsxa43173204 = -995386078;    int olFqAWpJNhmNsxa67865432 = -756921365;    int olFqAWpJNhmNsxa64376678 = -758720995;    int olFqAWpJNhmNsxa8525005 = -600147592;    int olFqAWpJNhmNsxa62678614 = -689704188;    int olFqAWpJNhmNsxa33764394 = -853837310;    int olFqAWpJNhmNsxa74800164 = -922834688;    int olFqAWpJNhmNsxa77941461 = -269748114;    int olFqAWpJNhmNsxa97975418 = -353022582;    int olFqAWpJNhmNsxa81354380 = 92752075;    int olFqAWpJNhmNsxa20924409 = 88994384;    int olFqAWpJNhmNsxa73217461 = -611444785;    int olFqAWpJNhmNsxa81613193 = -470807754;    int olFqAWpJNhmNsxa92239249 = -986306132;    int olFqAWpJNhmNsxa56064097 = -340515434;    int olFqAWpJNhmNsxa64133954 = -631011822;    int olFqAWpJNhmNsxa2940115 = -580877827;    int olFqAWpJNhmNsxa63481483 = -398944571;    int olFqAWpJNhmNsxa2564543 = -984772413;    int olFqAWpJNhmNsxa84784674 = 37405974;    int olFqAWpJNhmNsxa32781483 = -126063858;    int olFqAWpJNhmNsxa41999003 = -313404796;    int olFqAWpJNhmNsxa2275252 = -10045334;    int olFqAWpJNhmNsxa69057635 = -616548140;    int olFqAWpJNhmNsxa48770270 = -84238157;    int olFqAWpJNhmNsxa67866010 = -269780173;    int olFqAWpJNhmNsxa67558311 = -521346490;    int olFqAWpJNhmNsxa54955432 = -843752398;    int olFqAWpJNhmNsxa34554018 = -560689274;    int olFqAWpJNhmNsxa1614139 = -677323007;    int olFqAWpJNhmNsxa17748189 = -873412601;    int olFqAWpJNhmNsxa23005052 = -354171288;    int olFqAWpJNhmNsxa83376475 = -759486114;    int olFqAWpJNhmNsxa70704702 = -743886615;    int olFqAWpJNhmNsxa79277095 = -361005709;    int olFqAWpJNhmNsxa19201601 = -859624559;    int olFqAWpJNhmNsxa46534688 = -882000278;    int olFqAWpJNhmNsxa9590447 = -500682528;    int olFqAWpJNhmNsxa22044880 = -334089359;    int olFqAWpJNhmNsxa64416643 = -101416202;    int olFqAWpJNhmNsxa87647567 = -314903182;    int olFqAWpJNhmNsxa82330490 = -427075100;    int olFqAWpJNhmNsxa39824291 = -245321787;    int olFqAWpJNhmNsxa72008272 = -762713338;    int olFqAWpJNhmNsxa21846979 = -129862550;    int olFqAWpJNhmNsxa90260432 = -798829596;    int olFqAWpJNhmNsxa33006549 = -965820148;    int olFqAWpJNhmNsxa34896609 = -465501037;    int olFqAWpJNhmNsxa73212715 = -259929534;    int olFqAWpJNhmNsxa45807976 = -288672012;    int olFqAWpJNhmNsxa6637185 = -673290987;    int olFqAWpJNhmNsxa48451632 = -72681918;    int olFqAWpJNhmNsxa4534435 = -537134467;    int olFqAWpJNhmNsxa55953236 = -844983089;    int olFqAWpJNhmNsxa49792090 = -510732308;    int olFqAWpJNhmNsxa1174202 = -581981283;    int olFqAWpJNhmNsxa65590181 = -646876032;    int olFqAWpJNhmNsxa95319042 = -42172856;    int olFqAWpJNhmNsxa59754735 = -415909436;    int olFqAWpJNhmNsxa94812603 = -319924015;    int olFqAWpJNhmNsxa66206082 = -232490821;    int olFqAWpJNhmNsxa19844732 = 20917709;    int olFqAWpJNhmNsxa43387444 = 59974076;     olFqAWpJNhmNsxa745580 = olFqAWpJNhmNsxa47232406;     olFqAWpJNhmNsxa47232406 = olFqAWpJNhmNsxa93053434;     olFqAWpJNhmNsxa93053434 = olFqAWpJNhmNsxa79798711;     olFqAWpJNhmNsxa79798711 = olFqAWpJNhmNsxa55637904;     olFqAWpJNhmNsxa55637904 = olFqAWpJNhmNsxa11829023;     olFqAWpJNhmNsxa11829023 = olFqAWpJNhmNsxa12248164;     olFqAWpJNhmNsxa12248164 = olFqAWpJNhmNsxa82485753;     olFqAWpJNhmNsxa82485753 = olFqAWpJNhmNsxa94694170;     olFqAWpJNhmNsxa94694170 = olFqAWpJNhmNsxa84232245;     olFqAWpJNhmNsxa84232245 = olFqAWpJNhmNsxa44495948;     olFqAWpJNhmNsxa44495948 = olFqAWpJNhmNsxa11191438;     olFqAWpJNhmNsxa11191438 = olFqAWpJNhmNsxa29681304;     olFqAWpJNhmNsxa29681304 = olFqAWpJNhmNsxa30104029;     olFqAWpJNhmNsxa30104029 = olFqAWpJNhmNsxa24114387;     olFqAWpJNhmNsxa24114387 = olFqAWpJNhmNsxa53278275;     olFqAWpJNhmNsxa53278275 = olFqAWpJNhmNsxa22450300;     olFqAWpJNhmNsxa22450300 = olFqAWpJNhmNsxa87067032;     olFqAWpJNhmNsxa87067032 = olFqAWpJNhmNsxa10911366;     olFqAWpJNhmNsxa10911366 = olFqAWpJNhmNsxa18115452;     olFqAWpJNhmNsxa18115452 = olFqAWpJNhmNsxa84723494;     olFqAWpJNhmNsxa84723494 = olFqAWpJNhmNsxa98181037;     olFqAWpJNhmNsxa98181037 = olFqAWpJNhmNsxa62447731;     olFqAWpJNhmNsxa62447731 = olFqAWpJNhmNsxa60271952;     olFqAWpJNhmNsxa60271952 = olFqAWpJNhmNsxa37799709;     olFqAWpJNhmNsxa37799709 = olFqAWpJNhmNsxa53362653;     olFqAWpJNhmNsxa53362653 = olFqAWpJNhmNsxa42771387;     olFqAWpJNhmNsxa42771387 = olFqAWpJNhmNsxa63477893;     olFqAWpJNhmNsxa63477893 = olFqAWpJNhmNsxa14619743;     olFqAWpJNhmNsxa14619743 = olFqAWpJNhmNsxa27135859;     olFqAWpJNhmNsxa27135859 = olFqAWpJNhmNsxa29276813;     olFqAWpJNhmNsxa29276813 = olFqAWpJNhmNsxa9941931;     olFqAWpJNhmNsxa9941931 = olFqAWpJNhmNsxa9577300;     olFqAWpJNhmNsxa9577300 = olFqAWpJNhmNsxa11933115;     olFqAWpJNhmNsxa11933115 = olFqAWpJNhmNsxa7098977;     olFqAWpJNhmNsxa7098977 = olFqAWpJNhmNsxa40737911;     olFqAWpJNhmNsxa40737911 = olFqAWpJNhmNsxa82573572;     olFqAWpJNhmNsxa82573572 = olFqAWpJNhmNsxa43173204;     olFqAWpJNhmNsxa43173204 = olFqAWpJNhmNsxa67865432;     olFqAWpJNhmNsxa67865432 = olFqAWpJNhmNsxa64376678;     olFqAWpJNhmNsxa64376678 = olFqAWpJNhmNsxa8525005;     olFqAWpJNhmNsxa8525005 = olFqAWpJNhmNsxa62678614;     olFqAWpJNhmNsxa62678614 = olFqAWpJNhmNsxa33764394;     olFqAWpJNhmNsxa33764394 = olFqAWpJNhmNsxa74800164;     olFqAWpJNhmNsxa74800164 = olFqAWpJNhmNsxa77941461;     olFqAWpJNhmNsxa77941461 = olFqAWpJNhmNsxa97975418;     olFqAWpJNhmNsxa97975418 = olFqAWpJNhmNsxa81354380;     olFqAWpJNhmNsxa81354380 = olFqAWpJNhmNsxa20924409;     olFqAWpJNhmNsxa20924409 = olFqAWpJNhmNsxa73217461;     olFqAWpJNhmNsxa73217461 = olFqAWpJNhmNsxa81613193;     olFqAWpJNhmNsxa81613193 = olFqAWpJNhmNsxa92239249;     olFqAWpJNhmNsxa92239249 = olFqAWpJNhmNsxa56064097;     olFqAWpJNhmNsxa56064097 = olFqAWpJNhmNsxa64133954;     olFqAWpJNhmNsxa64133954 = olFqAWpJNhmNsxa2940115;     olFqAWpJNhmNsxa2940115 = olFqAWpJNhmNsxa63481483;     olFqAWpJNhmNsxa63481483 = olFqAWpJNhmNsxa2564543;     olFqAWpJNhmNsxa2564543 = olFqAWpJNhmNsxa84784674;     olFqAWpJNhmNsxa84784674 = olFqAWpJNhmNsxa32781483;     olFqAWpJNhmNsxa32781483 = olFqAWpJNhmNsxa41999003;     olFqAWpJNhmNsxa41999003 = olFqAWpJNhmNsxa2275252;     olFqAWpJNhmNsxa2275252 = olFqAWpJNhmNsxa69057635;     olFqAWpJNhmNsxa69057635 = olFqAWpJNhmNsxa48770270;     olFqAWpJNhmNsxa48770270 = olFqAWpJNhmNsxa67866010;     olFqAWpJNhmNsxa67866010 = olFqAWpJNhmNsxa67558311;     olFqAWpJNhmNsxa67558311 = olFqAWpJNhmNsxa54955432;     olFqAWpJNhmNsxa54955432 = olFqAWpJNhmNsxa34554018;     olFqAWpJNhmNsxa34554018 = olFqAWpJNhmNsxa1614139;     olFqAWpJNhmNsxa1614139 = olFqAWpJNhmNsxa17748189;     olFqAWpJNhmNsxa17748189 = olFqAWpJNhmNsxa23005052;     olFqAWpJNhmNsxa23005052 = olFqAWpJNhmNsxa83376475;     olFqAWpJNhmNsxa83376475 = olFqAWpJNhmNsxa70704702;     olFqAWpJNhmNsxa70704702 = olFqAWpJNhmNsxa79277095;     olFqAWpJNhmNsxa79277095 = olFqAWpJNhmNsxa19201601;     olFqAWpJNhmNsxa19201601 = olFqAWpJNhmNsxa46534688;     olFqAWpJNhmNsxa46534688 = olFqAWpJNhmNsxa9590447;     olFqAWpJNhmNsxa9590447 = olFqAWpJNhmNsxa22044880;     olFqAWpJNhmNsxa22044880 = olFqAWpJNhmNsxa64416643;     olFqAWpJNhmNsxa64416643 = olFqAWpJNhmNsxa87647567;     olFqAWpJNhmNsxa87647567 = olFqAWpJNhmNsxa82330490;     olFqAWpJNhmNsxa82330490 = olFqAWpJNhmNsxa39824291;     olFqAWpJNhmNsxa39824291 = olFqAWpJNhmNsxa72008272;     olFqAWpJNhmNsxa72008272 = olFqAWpJNhmNsxa21846979;     olFqAWpJNhmNsxa21846979 = olFqAWpJNhmNsxa90260432;     olFqAWpJNhmNsxa90260432 = olFqAWpJNhmNsxa33006549;     olFqAWpJNhmNsxa33006549 = olFqAWpJNhmNsxa34896609;     olFqAWpJNhmNsxa34896609 = olFqAWpJNhmNsxa73212715;     olFqAWpJNhmNsxa73212715 = olFqAWpJNhmNsxa45807976;     olFqAWpJNhmNsxa45807976 = olFqAWpJNhmNsxa6637185;     olFqAWpJNhmNsxa6637185 = olFqAWpJNhmNsxa48451632;     olFqAWpJNhmNsxa48451632 = olFqAWpJNhmNsxa4534435;     olFqAWpJNhmNsxa4534435 = olFqAWpJNhmNsxa55953236;     olFqAWpJNhmNsxa55953236 = olFqAWpJNhmNsxa49792090;     olFqAWpJNhmNsxa49792090 = olFqAWpJNhmNsxa1174202;     olFqAWpJNhmNsxa1174202 = olFqAWpJNhmNsxa65590181;     olFqAWpJNhmNsxa65590181 = olFqAWpJNhmNsxa95319042;     olFqAWpJNhmNsxa95319042 = olFqAWpJNhmNsxa59754735;     olFqAWpJNhmNsxa59754735 = olFqAWpJNhmNsxa94812603;     olFqAWpJNhmNsxa94812603 = olFqAWpJNhmNsxa66206082;     olFqAWpJNhmNsxa66206082 = olFqAWpJNhmNsxa19844732;     olFqAWpJNhmNsxa19844732 = olFqAWpJNhmNsxa43387444;     olFqAWpJNhmNsxa43387444 = olFqAWpJNhmNsxa745580;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ClAHXulSpIpGpQGl2621191() {     int dayonUytXZcGRWM71397246 = -558175054;    int dayonUytXZcGRWM15905924 = -171975194;    int dayonUytXZcGRWM73022354 = -365181531;    int dayonUytXZcGRWM19729998 = -201887553;    int dayonUytXZcGRWM77088225 = -663412943;    int dayonUytXZcGRWM1828785 = -558997457;    int dayonUytXZcGRWM30627059 = -593288247;    int dayonUytXZcGRWM39328134 = -53225164;    int dayonUytXZcGRWM40559225 = -66500986;    int dayonUytXZcGRWM7198706 = -845292715;    int dayonUytXZcGRWM86745108 = -615275036;    int dayonUytXZcGRWM56738009 = -247255613;    int dayonUytXZcGRWM9508691 = -354576147;    int dayonUytXZcGRWM13951062 = -777106378;    int dayonUytXZcGRWM6834406 = -816813231;    int dayonUytXZcGRWM85806915 = -50666366;    int dayonUytXZcGRWM16303127 = -265576917;    int dayonUytXZcGRWM7360527 = -605712120;    int dayonUytXZcGRWM92619147 = -247909215;    int dayonUytXZcGRWM76902161 = -643196425;    int dayonUytXZcGRWM39859249 = 67527313;    int dayonUytXZcGRWM59576802 = -286538817;    int dayonUytXZcGRWM64391540 = -805581014;    int dayonUytXZcGRWM94121489 = -389224120;    int dayonUytXZcGRWM1924098 = -374249056;    int dayonUytXZcGRWM83960233 = -868245707;    int dayonUytXZcGRWM51032173 = -536045941;    int dayonUytXZcGRWM71260118 = -915398148;    int dayonUytXZcGRWM46762231 = -730986118;    int dayonUytXZcGRWM66307955 = 89797762;    int dayonUytXZcGRWM36348810 = -448878653;    int dayonUytXZcGRWM54453391 = -247980996;    int dayonUytXZcGRWM92522660 = -201130423;    int dayonUytXZcGRWM77298089 = -135156172;    int dayonUytXZcGRWM85123213 = -368853659;    int dayonUytXZcGRWM731170 = -986803662;    int dayonUytXZcGRWM26365058 = -925389373;    int dayonUytXZcGRWM95965692 = -779499186;    int dayonUytXZcGRWM46398721 = -712650002;    int dayonUytXZcGRWM59479976 = -276329358;    int dayonUytXZcGRWM68598289 = -984166471;    int dayonUytXZcGRWM91161605 = -459093257;    int dayonUytXZcGRWM82731354 = -840591362;    int dayonUytXZcGRWM81308464 = -994809660;    int dayonUytXZcGRWM73895520 = -578394509;    int dayonUytXZcGRWM4381253 = -111359736;    int dayonUytXZcGRWM7485584 = -23317724;    int dayonUytXZcGRWM86538618 = -363135912;    int dayonUytXZcGRWM78658913 = -597208755;    int dayonUytXZcGRWM90040680 = -753816632;    int dayonUytXZcGRWM60428574 = -532878253;    int dayonUytXZcGRWM97935601 = -309735156;    int dayonUytXZcGRWM19209611 = -558809864;    int dayonUytXZcGRWM93167419 = -141759324;    int dayonUytXZcGRWM62761942 = -509885485;    int dayonUytXZcGRWM11820445 = -171636238;    int dayonUytXZcGRWM51514383 = -366394180;    int dayonUytXZcGRWM78900864 = -975957411;    int dayonUytXZcGRWM17805901 = -827638497;    int dayonUytXZcGRWM93127992 = -795167237;    int dayonUytXZcGRWM50796612 = 77048484;    int dayonUytXZcGRWM59366940 = -677890099;    int dayonUytXZcGRWM92565903 = -322239046;    int dayonUytXZcGRWM74251270 = -56298748;    int dayonUytXZcGRWM70849896 = -296414063;    int dayonUytXZcGRWM32291718 = -267294041;    int dayonUytXZcGRWM64215349 = 53874809;    int dayonUytXZcGRWM32210601 = -119419975;    int dayonUytXZcGRWM28827849 = -308252719;    int dayonUytXZcGRWM6103236 = -830009569;    int dayonUytXZcGRWM59441857 = -125276994;    int dayonUytXZcGRWM20337434 = -486077732;    int dayonUytXZcGRWM60961806 = -893062118;    int dayonUytXZcGRWM33139172 = -971579858;    int dayonUytXZcGRWM8303873 = -659029955;    int dayonUytXZcGRWM48697643 = -473379430;    int dayonUytXZcGRWM76845448 = -445947455;    int dayonUytXZcGRWM83083075 = -810771355;    int dayonUytXZcGRWM20225969 = -810829612;    int dayonUytXZcGRWM97542844 = -162889320;    int dayonUytXZcGRWM76474649 = -744927983;    int dayonUytXZcGRWM64493554 = -72910030;    int dayonUytXZcGRWM92601205 = -218189394;    int dayonUytXZcGRWM56721550 = -977169487;    int dayonUytXZcGRWM5879381 = -377323986;    int dayonUytXZcGRWM38413208 = -39143498;    int dayonUytXZcGRWM35243780 = -689171133;    int dayonUytXZcGRWM99355241 = 40628900;    int dayonUytXZcGRWM14536148 = -625270688;    int dayonUytXZcGRWM73302768 = -97217422;    int dayonUytXZcGRWM49216786 = -520409482;    int dayonUytXZcGRWM47464194 = -949431962;    int dayonUytXZcGRWM78159792 = -951860689;    int dayonUytXZcGRWM53270729 = -917482766;    int dayonUytXZcGRWM8683364 = -253377842;    int dayonUytXZcGRWM9231349 = -206276372;    int dayonUytXZcGRWM98595702 = -36854212;    int dayonUytXZcGRWM8480084 = -684292615;    int dayonUytXZcGRWM10458569 = -598395597;    int dayonUytXZcGRWM41603803 = -558175054;     dayonUytXZcGRWM71397246 = dayonUytXZcGRWM15905924;     dayonUytXZcGRWM15905924 = dayonUytXZcGRWM73022354;     dayonUytXZcGRWM73022354 = dayonUytXZcGRWM19729998;     dayonUytXZcGRWM19729998 = dayonUytXZcGRWM77088225;     dayonUytXZcGRWM77088225 = dayonUytXZcGRWM1828785;     dayonUytXZcGRWM1828785 = dayonUytXZcGRWM30627059;     dayonUytXZcGRWM30627059 = dayonUytXZcGRWM39328134;     dayonUytXZcGRWM39328134 = dayonUytXZcGRWM40559225;     dayonUytXZcGRWM40559225 = dayonUytXZcGRWM7198706;     dayonUytXZcGRWM7198706 = dayonUytXZcGRWM86745108;     dayonUytXZcGRWM86745108 = dayonUytXZcGRWM56738009;     dayonUytXZcGRWM56738009 = dayonUytXZcGRWM9508691;     dayonUytXZcGRWM9508691 = dayonUytXZcGRWM13951062;     dayonUytXZcGRWM13951062 = dayonUytXZcGRWM6834406;     dayonUytXZcGRWM6834406 = dayonUytXZcGRWM85806915;     dayonUytXZcGRWM85806915 = dayonUytXZcGRWM16303127;     dayonUytXZcGRWM16303127 = dayonUytXZcGRWM7360527;     dayonUytXZcGRWM7360527 = dayonUytXZcGRWM92619147;     dayonUytXZcGRWM92619147 = dayonUytXZcGRWM76902161;     dayonUytXZcGRWM76902161 = dayonUytXZcGRWM39859249;     dayonUytXZcGRWM39859249 = dayonUytXZcGRWM59576802;     dayonUytXZcGRWM59576802 = dayonUytXZcGRWM64391540;     dayonUytXZcGRWM64391540 = dayonUytXZcGRWM94121489;     dayonUytXZcGRWM94121489 = dayonUytXZcGRWM1924098;     dayonUytXZcGRWM1924098 = dayonUytXZcGRWM83960233;     dayonUytXZcGRWM83960233 = dayonUytXZcGRWM51032173;     dayonUytXZcGRWM51032173 = dayonUytXZcGRWM71260118;     dayonUytXZcGRWM71260118 = dayonUytXZcGRWM46762231;     dayonUytXZcGRWM46762231 = dayonUytXZcGRWM66307955;     dayonUytXZcGRWM66307955 = dayonUytXZcGRWM36348810;     dayonUytXZcGRWM36348810 = dayonUytXZcGRWM54453391;     dayonUytXZcGRWM54453391 = dayonUytXZcGRWM92522660;     dayonUytXZcGRWM92522660 = dayonUytXZcGRWM77298089;     dayonUytXZcGRWM77298089 = dayonUytXZcGRWM85123213;     dayonUytXZcGRWM85123213 = dayonUytXZcGRWM731170;     dayonUytXZcGRWM731170 = dayonUytXZcGRWM26365058;     dayonUytXZcGRWM26365058 = dayonUytXZcGRWM95965692;     dayonUytXZcGRWM95965692 = dayonUytXZcGRWM46398721;     dayonUytXZcGRWM46398721 = dayonUytXZcGRWM59479976;     dayonUytXZcGRWM59479976 = dayonUytXZcGRWM68598289;     dayonUytXZcGRWM68598289 = dayonUytXZcGRWM91161605;     dayonUytXZcGRWM91161605 = dayonUytXZcGRWM82731354;     dayonUytXZcGRWM82731354 = dayonUytXZcGRWM81308464;     dayonUytXZcGRWM81308464 = dayonUytXZcGRWM73895520;     dayonUytXZcGRWM73895520 = dayonUytXZcGRWM4381253;     dayonUytXZcGRWM4381253 = dayonUytXZcGRWM7485584;     dayonUytXZcGRWM7485584 = dayonUytXZcGRWM86538618;     dayonUytXZcGRWM86538618 = dayonUytXZcGRWM78658913;     dayonUytXZcGRWM78658913 = dayonUytXZcGRWM90040680;     dayonUytXZcGRWM90040680 = dayonUytXZcGRWM60428574;     dayonUytXZcGRWM60428574 = dayonUytXZcGRWM97935601;     dayonUytXZcGRWM97935601 = dayonUytXZcGRWM19209611;     dayonUytXZcGRWM19209611 = dayonUytXZcGRWM93167419;     dayonUytXZcGRWM93167419 = dayonUytXZcGRWM62761942;     dayonUytXZcGRWM62761942 = dayonUytXZcGRWM11820445;     dayonUytXZcGRWM11820445 = dayonUytXZcGRWM51514383;     dayonUytXZcGRWM51514383 = dayonUytXZcGRWM78900864;     dayonUytXZcGRWM78900864 = dayonUytXZcGRWM17805901;     dayonUytXZcGRWM17805901 = dayonUytXZcGRWM93127992;     dayonUytXZcGRWM93127992 = dayonUytXZcGRWM50796612;     dayonUytXZcGRWM50796612 = dayonUytXZcGRWM59366940;     dayonUytXZcGRWM59366940 = dayonUytXZcGRWM92565903;     dayonUytXZcGRWM92565903 = dayonUytXZcGRWM74251270;     dayonUytXZcGRWM74251270 = dayonUytXZcGRWM70849896;     dayonUytXZcGRWM70849896 = dayonUytXZcGRWM32291718;     dayonUytXZcGRWM32291718 = dayonUytXZcGRWM64215349;     dayonUytXZcGRWM64215349 = dayonUytXZcGRWM32210601;     dayonUytXZcGRWM32210601 = dayonUytXZcGRWM28827849;     dayonUytXZcGRWM28827849 = dayonUytXZcGRWM6103236;     dayonUytXZcGRWM6103236 = dayonUytXZcGRWM59441857;     dayonUytXZcGRWM59441857 = dayonUytXZcGRWM20337434;     dayonUytXZcGRWM20337434 = dayonUytXZcGRWM60961806;     dayonUytXZcGRWM60961806 = dayonUytXZcGRWM33139172;     dayonUytXZcGRWM33139172 = dayonUytXZcGRWM8303873;     dayonUytXZcGRWM8303873 = dayonUytXZcGRWM48697643;     dayonUytXZcGRWM48697643 = dayonUytXZcGRWM76845448;     dayonUytXZcGRWM76845448 = dayonUytXZcGRWM83083075;     dayonUytXZcGRWM83083075 = dayonUytXZcGRWM20225969;     dayonUytXZcGRWM20225969 = dayonUytXZcGRWM97542844;     dayonUytXZcGRWM97542844 = dayonUytXZcGRWM76474649;     dayonUytXZcGRWM76474649 = dayonUytXZcGRWM64493554;     dayonUytXZcGRWM64493554 = dayonUytXZcGRWM92601205;     dayonUytXZcGRWM92601205 = dayonUytXZcGRWM56721550;     dayonUytXZcGRWM56721550 = dayonUytXZcGRWM5879381;     dayonUytXZcGRWM5879381 = dayonUytXZcGRWM38413208;     dayonUytXZcGRWM38413208 = dayonUytXZcGRWM35243780;     dayonUytXZcGRWM35243780 = dayonUytXZcGRWM99355241;     dayonUytXZcGRWM99355241 = dayonUytXZcGRWM14536148;     dayonUytXZcGRWM14536148 = dayonUytXZcGRWM73302768;     dayonUytXZcGRWM73302768 = dayonUytXZcGRWM49216786;     dayonUytXZcGRWM49216786 = dayonUytXZcGRWM47464194;     dayonUytXZcGRWM47464194 = dayonUytXZcGRWM78159792;     dayonUytXZcGRWM78159792 = dayonUytXZcGRWM53270729;     dayonUytXZcGRWM53270729 = dayonUytXZcGRWM8683364;     dayonUytXZcGRWM8683364 = dayonUytXZcGRWM9231349;     dayonUytXZcGRWM9231349 = dayonUytXZcGRWM98595702;     dayonUytXZcGRWM98595702 = dayonUytXZcGRWM8480084;     dayonUytXZcGRWM8480084 = dayonUytXZcGRWM10458569;     dayonUytXZcGRWM10458569 = dayonUytXZcGRWM41603803;     dayonUytXZcGRWM41603803 = dayonUytXZcGRWM71397246;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void YKMhLrjzbTWtyBsE9279935() {     int dqarjWypKebFEvF71390775 = -430699196;    int dqarjWypKebFEvF54478240 = -214570319;    int dqarjWypKebFEvF74335566 = -322310145;    int dqarjWypKebFEvF4809859 = -22428383;    int dqarjWypKebFEvF85928327 = -31768264;    int dqarjWypKebFEvF36634217 = -258430268;    int dqarjWypKebFEvF99174395 = -297824714;    int dqarjWypKebFEvF8542185 = -857402481;    int dqarjWypKebFEvF23299389 = -880147056;    int dqarjWypKebFEvF3838220 = -482099586;    int dqarjWypKebFEvF7065715 = -574747108;    int dqarjWypKebFEvF71029686 = 17266024;    int dqarjWypKebFEvF41361605 = -376234622;    int dqarjWypKebFEvF67795282 = -22259429;    int dqarjWypKebFEvF52006515 = -779526083;    int dqarjWypKebFEvF60371462 = -383462016;    int dqarjWypKebFEvF72155886 = -85191487;    int dqarjWypKebFEvF30361060 = 97216258;    int dqarjWypKebFEvF13047418 = -865891482;    int dqarjWypKebFEvF48975924 = -155220876;    int dqarjWypKebFEvF19528673 = -742939060;    int dqarjWypKebFEvF37873068 = -796530700;    int dqarjWypKebFEvF1269937 = -164538856;    int dqarjWypKebFEvF37771598 = -270144643;    int dqarjWypKebFEvF30366787 = -800942909;    int dqarjWypKebFEvF534311 = -776451635;    int dqarjWypKebFEvF21253755 = -836845654;    int dqarjWypKebFEvF6663644 = -424365137;    int dqarjWypKebFEvF59440677 = -277056433;    int dqarjWypKebFEvF42397825 = -685633612;    int dqarjWypKebFEvF69761488 = -835971888;    int dqarjWypKebFEvF39177249 = -639802758;    int dqarjWypKebFEvF77975083 = -403074639;    int dqarjWypKebFEvF77580926 = -11837905;    int dqarjWypKebFEvF67481251 = -628167584;    int dqarjWypKebFEvF43559360 = 33430592;    int dqarjWypKebFEvF38306426 = 74957249;    int dqarjWypKebFEvF65883372 = -774761865;    int dqarjWypKebFEvF81712317 = -325331418;    int dqarjWypKebFEvF31352869 = -985761044;    int dqarjWypKebFEvF56453956 = 27889631;    int dqarjWypKebFEvF76859266 = -652885339;    int dqarjWypKebFEvF43014714 = -299721616;    int dqarjWypKebFEvF40540363 = -551774103;    int dqarjWypKebFEvF57913573 = 48346089;    int dqarjWypKebFEvF44809221 = 76595593;    int dqarjWypKebFEvF35183376 = -504598346;    int dqarjWypKebFEvF61922199 = -971897268;    int dqarjWypKebFEvF97317302 = -160972616;    int dqarjWypKebFEvF10086104 = -340841519;    int dqarjWypKebFEvF73687944 = -111433260;    int dqarjWypKebFEvF12482860 = -691880904;    int dqarjWypKebFEvF94657959 = -450863872;    int dqarjWypKebFEvF3417690 = -673679175;    int dqarjWypKebFEvF89751262 = -266784763;    int dqarjWypKebFEvF33517708 = -634168496;    int dqarjWypKebFEvF53208303 = 49968536;    int dqarjWypKebFEvF36563969 = 47834498;    int dqarjWypKebFEvF74443072 = -221485474;    int dqarjWypKebFEvF85394017 = -255316630;    int dqarjWypKebFEvF15380463 = -421584615;    int dqarjWypKebFEvF92510751 = -873459578;    int dqarjWypKebFEvF49101507 = -480346048;    int dqarjWypKebFEvF80901564 = -94513444;    int dqarjWypKebFEvF34076731 = -646127699;    int dqarjWypKebFEvF67888465 = -934944351;    int dqarjWypKebFEvF93054602 = -579659338;    int dqarjWypKebFEvF63780678 = -264396717;    int dqarjWypKebFEvF314031 = -394091846;    int dqarjWypKebFEvF8447156 = -712956675;    int dqarjWypKebFEvF22065037 = -358419266;    int dqarjWypKebFEvF6272514 = -310429623;    int dqarjWypKebFEvF48648742 = -577452324;    int dqarjWypKebFEvF81694548 = -880130438;    int dqarjWypKebFEvF92521967 = -83110508;    int dqarjWypKebFEvF42669406 = 9946278;    int dqarjWypKebFEvF94858353 = -396809084;    int dqarjWypKebFEvF60729574 = -612764753;    int dqarjWypKebFEvF79858025 = -218490733;    int dqarjWypKebFEvF85557565 = -777538503;    int dqarjWypKebFEvF65350934 = -171853289;    int dqarjWypKebFEvF59331555 = -864948387;    int dqarjWypKebFEvF9346342 = -163392521;    int dqarjWypKebFEvF49354574 = -936214915;    int dqarjWypKebFEvF68709881 = -474200353;    int dqarjWypKebFEvF57278629 = -44090984;    int dqarjWypKebFEvF44519289 = -88938886;    int dqarjWypKebFEvF74557393 = -729395464;    int dqarjWypKebFEvF87829664 = -745053143;    int dqarjWypKebFEvF33963544 = -993999088;    int dqarjWypKebFEvF90351056 = 83462055;    int dqarjWypKebFEvF1742458 = -972877249;    int dqarjWypKebFEvF91440300 = -453276391;    int dqarjWypKebFEvF96318300 = 29985211;    int dqarjWypKebFEvF15972406 = -464176429;    int dqarjWypKebFEvF63943205 = -98650792;    int dqarjWypKebFEvF27757760 = -72539292;    int dqarjWypKebFEvF62113150 = -105208173;    int dqarjWypKebFEvF6463632 = -905646404;    int dqarjWypKebFEvF90025107 = -430699196;     dqarjWypKebFEvF71390775 = dqarjWypKebFEvF54478240;     dqarjWypKebFEvF54478240 = dqarjWypKebFEvF74335566;     dqarjWypKebFEvF74335566 = dqarjWypKebFEvF4809859;     dqarjWypKebFEvF4809859 = dqarjWypKebFEvF85928327;     dqarjWypKebFEvF85928327 = dqarjWypKebFEvF36634217;     dqarjWypKebFEvF36634217 = dqarjWypKebFEvF99174395;     dqarjWypKebFEvF99174395 = dqarjWypKebFEvF8542185;     dqarjWypKebFEvF8542185 = dqarjWypKebFEvF23299389;     dqarjWypKebFEvF23299389 = dqarjWypKebFEvF3838220;     dqarjWypKebFEvF3838220 = dqarjWypKebFEvF7065715;     dqarjWypKebFEvF7065715 = dqarjWypKebFEvF71029686;     dqarjWypKebFEvF71029686 = dqarjWypKebFEvF41361605;     dqarjWypKebFEvF41361605 = dqarjWypKebFEvF67795282;     dqarjWypKebFEvF67795282 = dqarjWypKebFEvF52006515;     dqarjWypKebFEvF52006515 = dqarjWypKebFEvF60371462;     dqarjWypKebFEvF60371462 = dqarjWypKebFEvF72155886;     dqarjWypKebFEvF72155886 = dqarjWypKebFEvF30361060;     dqarjWypKebFEvF30361060 = dqarjWypKebFEvF13047418;     dqarjWypKebFEvF13047418 = dqarjWypKebFEvF48975924;     dqarjWypKebFEvF48975924 = dqarjWypKebFEvF19528673;     dqarjWypKebFEvF19528673 = dqarjWypKebFEvF37873068;     dqarjWypKebFEvF37873068 = dqarjWypKebFEvF1269937;     dqarjWypKebFEvF1269937 = dqarjWypKebFEvF37771598;     dqarjWypKebFEvF37771598 = dqarjWypKebFEvF30366787;     dqarjWypKebFEvF30366787 = dqarjWypKebFEvF534311;     dqarjWypKebFEvF534311 = dqarjWypKebFEvF21253755;     dqarjWypKebFEvF21253755 = dqarjWypKebFEvF6663644;     dqarjWypKebFEvF6663644 = dqarjWypKebFEvF59440677;     dqarjWypKebFEvF59440677 = dqarjWypKebFEvF42397825;     dqarjWypKebFEvF42397825 = dqarjWypKebFEvF69761488;     dqarjWypKebFEvF69761488 = dqarjWypKebFEvF39177249;     dqarjWypKebFEvF39177249 = dqarjWypKebFEvF77975083;     dqarjWypKebFEvF77975083 = dqarjWypKebFEvF77580926;     dqarjWypKebFEvF77580926 = dqarjWypKebFEvF67481251;     dqarjWypKebFEvF67481251 = dqarjWypKebFEvF43559360;     dqarjWypKebFEvF43559360 = dqarjWypKebFEvF38306426;     dqarjWypKebFEvF38306426 = dqarjWypKebFEvF65883372;     dqarjWypKebFEvF65883372 = dqarjWypKebFEvF81712317;     dqarjWypKebFEvF81712317 = dqarjWypKebFEvF31352869;     dqarjWypKebFEvF31352869 = dqarjWypKebFEvF56453956;     dqarjWypKebFEvF56453956 = dqarjWypKebFEvF76859266;     dqarjWypKebFEvF76859266 = dqarjWypKebFEvF43014714;     dqarjWypKebFEvF43014714 = dqarjWypKebFEvF40540363;     dqarjWypKebFEvF40540363 = dqarjWypKebFEvF57913573;     dqarjWypKebFEvF57913573 = dqarjWypKebFEvF44809221;     dqarjWypKebFEvF44809221 = dqarjWypKebFEvF35183376;     dqarjWypKebFEvF35183376 = dqarjWypKebFEvF61922199;     dqarjWypKebFEvF61922199 = dqarjWypKebFEvF97317302;     dqarjWypKebFEvF97317302 = dqarjWypKebFEvF10086104;     dqarjWypKebFEvF10086104 = dqarjWypKebFEvF73687944;     dqarjWypKebFEvF73687944 = dqarjWypKebFEvF12482860;     dqarjWypKebFEvF12482860 = dqarjWypKebFEvF94657959;     dqarjWypKebFEvF94657959 = dqarjWypKebFEvF3417690;     dqarjWypKebFEvF3417690 = dqarjWypKebFEvF89751262;     dqarjWypKebFEvF89751262 = dqarjWypKebFEvF33517708;     dqarjWypKebFEvF33517708 = dqarjWypKebFEvF53208303;     dqarjWypKebFEvF53208303 = dqarjWypKebFEvF36563969;     dqarjWypKebFEvF36563969 = dqarjWypKebFEvF74443072;     dqarjWypKebFEvF74443072 = dqarjWypKebFEvF85394017;     dqarjWypKebFEvF85394017 = dqarjWypKebFEvF15380463;     dqarjWypKebFEvF15380463 = dqarjWypKebFEvF92510751;     dqarjWypKebFEvF92510751 = dqarjWypKebFEvF49101507;     dqarjWypKebFEvF49101507 = dqarjWypKebFEvF80901564;     dqarjWypKebFEvF80901564 = dqarjWypKebFEvF34076731;     dqarjWypKebFEvF34076731 = dqarjWypKebFEvF67888465;     dqarjWypKebFEvF67888465 = dqarjWypKebFEvF93054602;     dqarjWypKebFEvF93054602 = dqarjWypKebFEvF63780678;     dqarjWypKebFEvF63780678 = dqarjWypKebFEvF314031;     dqarjWypKebFEvF314031 = dqarjWypKebFEvF8447156;     dqarjWypKebFEvF8447156 = dqarjWypKebFEvF22065037;     dqarjWypKebFEvF22065037 = dqarjWypKebFEvF6272514;     dqarjWypKebFEvF6272514 = dqarjWypKebFEvF48648742;     dqarjWypKebFEvF48648742 = dqarjWypKebFEvF81694548;     dqarjWypKebFEvF81694548 = dqarjWypKebFEvF92521967;     dqarjWypKebFEvF92521967 = dqarjWypKebFEvF42669406;     dqarjWypKebFEvF42669406 = dqarjWypKebFEvF94858353;     dqarjWypKebFEvF94858353 = dqarjWypKebFEvF60729574;     dqarjWypKebFEvF60729574 = dqarjWypKebFEvF79858025;     dqarjWypKebFEvF79858025 = dqarjWypKebFEvF85557565;     dqarjWypKebFEvF85557565 = dqarjWypKebFEvF65350934;     dqarjWypKebFEvF65350934 = dqarjWypKebFEvF59331555;     dqarjWypKebFEvF59331555 = dqarjWypKebFEvF9346342;     dqarjWypKebFEvF9346342 = dqarjWypKebFEvF49354574;     dqarjWypKebFEvF49354574 = dqarjWypKebFEvF68709881;     dqarjWypKebFEvF68709881 = dqarjWypKebFEvF57278629;     dqarjWypKebFEvF57278629 = dqarjWypKebFEvF44519289;     dqarjWypKebFEvF44519289 = dqarjWypKebFEvF74557393;     dqarjWypKebFEvF74557393 = dqarjWypKebFEvF87829664;     dqarjWypKebFEvF87829664 = dqarjWypKebFEvF33963544;     dqarjWypKebFEvF33963544 = dqarjWypKebFEvF90351056;     dqarjWypKebFEvF90351056 = dqarjWypKebFEvF1742458;     dqarjWypKebFEvF1742458 = dqarjWypKebFEvF91440300;     dqarjWypKebFEvF91440300 = dqarjWypKebFEvF96318300;     dqarjWypKebFEvF96318300 = dqarjWypKebFEvF15972406;     dqarjWypKebFEvF15972406 = dqarjWypKebFEvF63943205;     dqarjWypKebFEvF63943205 = dqarjWypKebFEvF27757760;     dqarjWypKebFEvF27757760 = dqarjWypKebFEvF62113150;     dqarjWypKebFEvF62113150 = dqarjWypKebFEvF6463632;     dqarjWypKebFEvF6463632 = dqarjWypKebFEvF90025107;     dqarjWypKebFEvF90025107 = dqarjWypKebFEvF71390775;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void BihSwABvBYqLExRv63696148() {     int vMEQLINyjzXEiTD42042442 = 51151674;    int vMEQLINyjzXEiTD23151758 = -86213618;    int vMEQLINyjzXEiTD54304485 = -764604605;    int vMEQLINyjzXEiTD44741146 = -212566772;    int vMEQLINyjzXEiTD7378649 = -915174612;    int vMEQLINyjzXEiTD26633980 = 39988580;    int vMEQLINyjzXEiTD17553291 = -296600424;    int vMEQLINyjzXEiTD65384565 = -104219571;    int vMEQLINyjzXEiTD69164443 = -973494384;    int vMEQLINyjzXEiTD26804680 = -783194937;    int vMEQLINyjzXEiTD49314875 = -609649038;    int vMEQLINyjzXEiTD16576258 = -298497769;    int vMEQLINyjzXEiTD21188992 = -285771680;    int vMEQLINyjzXEiTD51642316 = -923287639;    int vMEQLINyjzXEiTD34726534 = -929276086;    int vMEQLINyjzXEiTD92900102 = 46554397;    int vMEQLINyjzXEiTD66008713 = 5623383;    int vMEQLINyjzXEiTD50654554 = -991949938;    int vMEQLINyjzXEiTD94755198 = -473079424;    int vMEQLINyjzXEiTD7762634 = -697587181;    int vMEQLINyjzXEiTD74664427 = -651618201;    int vMEQLINyjzXEiTD99268832 = -27816006;    int vMEQLINyjzXEiTD3213746 = -732382001;    int vMEQLINyjzXEiTD71621134 = -962545549;    int vMEQLINyjzXEiTD94491175 = -476847597;    int vMEQLINyjzXEiTD31131891 = -874736080;    int vMEQLINyjzXEiTD29514541 = -132023429;    int vMEQLINyjzXEiTD14445869 = -929488904;    int vMEQLINyjzXEiTD91583165 = -571414650;    int vMEQLINyjzXEiTD81569920 = -144028683;    int vMEQLINyjzXEiTD76833485 = -584405573;    int vMEQLINyjzXEiTD83688709 = -968099920;    int vMEQLINyjzXEiTD60920444 = -350036248;    int vMEQLINyjzXEiTD42945902 = -675367589;    int vMEQLINyjzXEiTD45505488 = -475114363;    int vMEQLINyjzXEiTD3552619 = -45795956;    int vMEQLINyjzXEiTD82097912 = -113635958;    int vMEQLINyjzXEiTD18675861 = -558874972;    int vMEQLINyjzXEiTD60245606 = -281060055;    int vMEQLINyjzXEiTD26456167 = -503369407;    int vMEQLINyjzXEiTD16527241 = -356129247;    int vMEQLINyjzXEiTD5342258 = -422274409;    int vMEQLINyjzXEiTD91981674 = -286475669;    int vMEQLINyjzXEiTD47048663 = -623749074;    int vMEQLINyjzXEiTD53867631 = -260300305;    int vMEQLINyjzXEiTD51215056 = -781741562;    int vMEQLINyjzXEiTD61314579 = -620668145;    int vMEQLINyjzXEiTD27536410 = -324027563;    int vMEQLINyjzXEiTD2758755 = -146736585;    int vMEQLINyjzXEiTD18513590 = -623850397;    int vMEQLINyjzXEiTD41877268 = -758005381;    int vMEQLINyjzXEiTD54354364 = -661100626;    int vMEQLINyjzXEiTD49733616 = -378661914;    int vMEQLINyjzXEiTD93644994 = -234560672;    int vMEQLINyjzXEiTD89031720 = -377725677;    int vMEQLINyjzXEiTD42773610 = -921032321;    int vMEQLINyjzXEiTD19938012 = -353831618;    int vMEQLINyjzXEiTD82683350 = -802059056;    int vMEQLINyjzXEiTD50249970 = -735719176;    int vMEQLINyjzXEiTD76246758 = 59561467;    int vMEQLINyjzXEiTD97119439 = -827987991;    int vMEQLINyjzXEiTD3107422 = -367111520;    int vMEQLINyjzXEiTD73801399 = -532804921;    int vMEQLINyjzXEiTD87594522 = -729465702;    int vMEQLINyjzXEiTD49971195 = -98789364;    int vMEQLINyjzXEiTD65626165 = -641549118;    int vMEQLINyjzXEiTD55655813 = -948461522;    int vMEQLINyjzXEiTD78243090 = -610404091;    int vMEQLINyjzXEiTD6136829 = -348173276;    int vMEQLINyjzXEiTD31173916 = -783480130;    int vMEQLINyjzXEiTD10802191 = -839809645;    int vMEQLINyjzXEiTD47332852 = -435501645;    int vMEQLINyjzXEiTD90408947 = -610889883;    int vMEQLINyjzXEiTD68299032 = -969710018;    int vMEQLINyjzXEiTD91235392 = -241457934;    int vMEQLINyjzXEiTD69322169 = -129343793;    int vMEQLINyjzXEiTD7287159 = -741340338;    int vMEQLINyjzXEiTD56165082 = -8632927;    int vMEQLINyjzXEiTD17753503 = -602245245;    int vMEQLINyjzXEiTD43276119 = -695106035;    int vMEQLINyjzXEiTD69817311 = -154067935;    int vMEQLINyjzXEiTD1978131 = -807995866;    int vMEQLINyjzXEiTD11687115 = -682752320;    int vMEQLINyjzXEiTD73069576 = -947564254;    int vMEQLINyjzXEiTD39692652 = -386023302;    int vMEQLINyjzXEiTD22479121 = -923304948;    int vMEQLINyjzXEiTD33955093 = -489438007;    int vMEQLINyjzXEiTD67275450 = -15475577;    int vMEQLINyjzXEiTD53914181 = -197641913;    int vMEQLINyjzXEiTD2731879 = -554082043;    int vMEQLINyjzXEiTD83614606 = -691964339;    int vMEQLINyjzXEiTD99414561 = -311576903;    int vMEQLINyjzXEiTD68425891 = -823155797;    int vMEQLINyjzXEiTD83998848 = -240621523;    int vMEQLINyjzXEiTD29336728 = -675381416;    int vMEQLINyjzXEiTD13419819 = -989017728;    int vMEQLINyjzXEiTD31540858 = -889469488;    int vMEQLINyjzXEiTD4387152 = -557009967;    int vMEQLINyjzXEiTD97077468 = -424959711;    int vMEQLINyjzXEiTD88241465 = 51151674;     vMEQLINyjzXEiTD42042442 = vMEQLINyjzXEiTD23151758;     vMEQLINyjzXEiTD23151758 = vMEQLINyjzXEiTD54304485;     vMEQLINyjzXEiTD54304485 = vMEQLINyjzXEiTD44741146;     vMEQLINyjzXEiTD44741146 = vMEQLINyjzXEiTD7378649;     vMEQLINyjzXEiTD7378649 = vMEQLINyjzXEiTD26633980;     vMEQLINyjzXEiTD26633980 = vMEQLINyjzXEiTD17553291;     vMEQLINyjzXEiTD17553291 = vMEQLINyjzXEiTD65384565;     vMEQLINyjzXEiTD65384565 = vMEQLINyjzXEiTD69164443;     vMEQLINyjzXEiTD69164443 = vMEQLINyjzXEiTD26804680;     vMEQLINyjzXEiTD26804680 = vMEQLINyjzXEiTD49314875;     vMEQLINyjzXEiTD49314875 = vMEQLINyjzXEiTD16576258;     vMEQLINyjzXEiTD16576258 = vMEQLINyjzXEiTD21188992;     vMEQLINyjzXEiTD21188992 = vMEQLINyjzXEiTD51642316;     vMEQLINyjzXEiTD51642316 = vMEQLINyjzXEiTD34726534;     vMEQLINyjzXEiTD34726534 = vMEQLINyjzXEiTD92900102;     vMEQLINyjzXEiTD92900102 = vMEQLINyjzXEiTD66008713;     vMEQLINyjzXEiTD66008713 = vMEQLINyjzXEiTD50654554;     vMEQLINyjzXEiTD50654554 = vMEQLINyjzXEiTD94755198;     vMEQLINyjzXEiTD94755198 = vMEQLINyjzXEiTD7762634;     vMEQLINyjzXEiTD7762634 = vMEQLINyjzXEiTD74664427;     vMEQLINyjzXEiTD74664427 = vMEQLINyjzXEiTD99268832;     vMEQLINyjzXEiTD99268832 = vMEQLINyjzXEiTD3213746;     vMEQLINyjzXEiTD3213746 = vMEQLINyjzXEiTD71621134;     vMEQLINyjzXEiTD71621134 = vMEQLINyjzXEiTD94491175;     vMEQLINyjzXEiTD94491175 = vMEQLINyjzXEiTD31131891;     vMEQLINyjzXEiTD31131891 = vMEQLINyjzXEiTD29514541;     vMEQLINyjzXEiTD29514541 = vMEQLINyjzXEiTD14445869;     vMEQLINyjzXEiTD14445869 = vMEQLINyjzXEiTD91583165;     vMEQLINyjzXEiTD91583165 = vMEQLINyjzXEiTD81569920;     vMEQLINyjzXEiTD81569920 = vMEQLINyjzXEiTD76833485;     vMEQLINyjzXEiTD76833485 = vMEQLINyjzXEiTD83688709;     vMEQLINyjzXEiTD83688709 = vMEQLINyjzXEiTD60920444;     vMEQLINyjzXEiTD60920444 = vMEQLINyjzXEiTD42945902;     vMEQLINyjzXEiTD42945902 = vMEQLINyjzXEiTD45505488;     vMEQLINyjzXEiTD45505488 = vMEQLINyjzXEiTD3552619;     vMEQLINyjzXEiTD3552619 = vMEQLINyjzXEiTD82097912;     vMEQLINyjzXEiTD82097912 = vMEQLINyjzXEiTD18675861;     vMEQLINyjzXEiTD18675861 = vMEQLINyjzXEiTD60245606;     vMEQLINyjzXEiTD60245606 = vMEQLINyjzXEiTD26456167;     vMEQLINyjzXEiTD26456167 = vMEQLINyjzXEiTD16527241;     vMEQLINyjzXEiTD16527241 = vMEQLINyjzXEiTD5342258;     vMEQLINyjzXEiTD5342258 = vMEQLINyjzXEiTD91981674;     vMEQLINyjzXEiTD91981674 = vMEQLINyjzXEiTD47048663;     vMEQLINyjzXEiTD47048663 = vMEQLINyjzXEiTD53867631;     vMEQLINyjzXEiTD53867631 = vMEQLINyjzXEiTD51215056;     vMEQLINyjzXEiTD51215056 = vMEQLINyjzXEiTD61314579;     vMEQLINyjzXEiTD61314579 = vMEQLINyjzXEiTD27536410;     vMEQLINyjzXEiTD27536410 = vMEQLINyjzXEiTD2758755;     vMEQLINyjzXEiTD2758755 = vMEQLINyjzXEiTD18513590;     vMEQLINyjzXEiTD18513590 = vMEQLINyjzXEiTD41877268;     vMEQLINyjzXEiTD41877268 = vMEQLINyjzXEiTD54354364;     vMEQLINyjzXEiTD54354364 = vMEQLINyjzXEiTD49733616;     vMEQLINyjzXEiTD49733616 = vMEQLINyjzXEiTD93644994;     vMEQLINyjzXEiTD93644994 = vMEQLINyjzXEiTD89031720;     vMEQLINyjzXEiTD89031720 = vMEQLINyjzXEiTD42773610;     vMEQLINyjzXEiTD42773610 = vMEQLINyjzXEiTD19938012;     vMEQLINyjzXEiTD19938012 = vMEQLINyjzXEiTD82683350;     vMEQLINyjzXEiTD82683350 = vMEQLINyjzXEiTD50249970;     vMEQLINyjzXEiTD50249970 = vMEQLINyjzXEiTD76246758;     vMEQLINyjzXEiTD76246758 = vMEQLINyjzXEiTD97119439;     vMEQLINyjzXEiTD97119439 = vMEQLINyjzXEiTD3107422;     vMEQLINyjzXEiTD3107422 = vMEQLINyjzXEiTD73801399;     vMEQLINyjzXEiTD73801399 = vMEQLINyjzXEiTD87594522;     vMEQLINyjzXEiTD87594522 = vMEQLINyjzXEiTD49971195;     vMEQLINyjzXEiTD49971195 = vMEQLINyjzXEiTD65626165;     vMEQLINyjzXEiTD65626165 = vMEQLINyjzXEiTD55655813;     vMEQLINyjzXEiTD55655813 = vMEQLINyjzXEiTD78243090;     vMEQLINyjzXEiTD78243090 = vMEQLINyjzXEiTD6136829;     vMEQLINyjzXEiTD6136829 = vMEQLINyjzXEiTD31173916;     vMEQLINyjzXEiTD31173916 = vMEQLINyjzXEiTD10802191;     vMEQLINyjzXEiTD10802191 = vMEQLINyjzXEiTD47332852;     vMEQLINyjzXEiTD47332852 = vMEQLINyjzXEiTD90408947;     vMEQLINyjzXEiTD90408947 = vMEQLINyjzXEiTD68299032;     vMEQLINyjzXEiTD68299032 = vMEQLINyjzXEiTD91235392;     vMEQLINyjzXEiTD91235392 = vMEQLINyjzXEiTD69322169;     vMEQLINyjzXEiTD69322169 = vMEQLINyjzXEiTD7287159;     vMEQLINyjzXEiTD7287159 = vMEQLINyjzXEiTD56165082;     vMEQLINyjzXEiTD56165082 = vMEQLINyjzXEiTD17753503;     vMEQLINyjzXEiTD17753503 = vMEQLINyjzXEiTD43276119;     vMEQLINyjzXEiTD43276119 = vMEQLINyjzXEiTD69817311;     vMEQLINyjzXEiTD69817311 = vMEQLINyjzXEiTD1978131;     vMEQLINyjzXEiTD1978131 = vMEQLINyjzXEiTD11687115;     vMEQLINyjzXEiTD11687115 = vMEQLINyjzXEiTD73069576;     vMEQLINyjzXEiTD73069576 = vMEQLINyjzXEiTD39692652;     vMEQLINyjzXEiTD39692652 = vMEQLINyjzXEiTD22479121;     vMEQLINyjzXEiTD22479121 = vMEQLINyjzXEiTD33955093;     vMEQLINyjzXEiTD33955093 = vMEQLINyjzXEiTD67275450;     vMEQLINyjzXEiTD67275450 = vMEQLINyjzXEiTD53914181;     vMEQLINyjzXEiTD53914181 = vMEQLINyjzXEiTD2731879;     vMEQLINyjzXEiTD2731879 = vMEQLINyjzXEiTD83614606;     vMEQLINyjzXEiTD83614606 = vMEQLINyjzXEiTD99414561;     vMEQLINyjzXEiTD99414561 = vMEQLINyjzXEiTD68425891;     vMEQLINyjzXEiTD68425891 = vMEQLINyjzXEiTD83998848;     vMEQLINyjzXEiTD83998848 = vMEQLINyjzXEiTD29336728;     vMEQLINyjzXEiTD29336728 = vMEQLINyjzXEiTD13419819;     vMEQLINyjzXEiTD13419819 = vMEQLINyjzXEiTD31540858;     vMEQLINyjzXEiTD31540858 = vMEQLINyjzXEiTD4387152;     vMEQLINyjzXEiTD4387152 = vMEQLINyjzXEiTD97077468;     vMEQLINyjzXEiTD97077468 = vMEQLINyjzXEiTD88241465;     vMEQLINyjzXEiTD88241465 = vMEQLINyjzXEiTD42042442;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void kIQomPTIHPVOCAMJ70354892() {     int HRKKMppyEdWGUBR42035971 = -921372468;    int HRKKMppyEdWGUBR61724074 = -128808743;    int HRKKMppyEdWGUBR55617697 = -721733218;    int HRKKMppyEdWGUBR29821006 = -33107602;    int HRKKMppyEdWGUBR16218752 = -283529933;    int HRKKMppyEdWGUBR61439412 = -759444232;    int HRKKMppyEdWGUBR86100626 = -1136891;    int HRKKMppyEdWGUBR34598616 = -908396888;    int HRKKMppyEdWGUBR51904607 = -687140454;    int HRKKMppyEdWGUBR23444194 = -420001808;    int HRKKMppyEdWGUBR69635480 = -569121110;    int HRKKMppyEdWGUBR30867934 = -33976132;    int HRKKMppyEdWGUBR53041906 = -307430155;    int HRKKMppyEdWGUBR5486536 = -168440691;    int HRKKMppyEdWGUBR79898643 = -891988938;    int HRKKMppyEdWGUBR67464650 = -286241254;    int HRKKMppyEdWGUBR21861473 = -913991187;    int HRKKMppyEdWGUBR73655087 = -289021560;    int HRKKMppyEdWGUBR15183469 = 8938310;    int HRKKMppyEdWGUBR79836396 = -209611632;    int HRKKMppyEdWGUBR54333851 = -362084575;    int HRKKMppyEdWGUBR77565098 = -537807889;    int HRKKMppyEdWGUBR40092142 = -91339843;    int HRKKMppyEdWGUBR15271243 = -843466072;    int HRKKMppyEdWGUBR22933865 = -903541450;    int HRKKMppyEdWGUBR47705968 = -782942008;    int HRKKMppyEdWGUBR99736121 = -432823142;    int HRKKMppyEdWGUBR49849394 = -438455893;    int HRKKMppyEdWGUBR4261613 = -117484965;    int HRKKMppyEdWGUBR57659790 = -919460056;    int HRKKMppyEdWGUBR10246164 = -971498808;    int HRKKMppyEdWGUBR68412567 = -259921682;    int HRKKMppyEdWGUBR46372867 = -551980463;    int HRKKMppyEdWGUBR43228739 = -552049322;    int HRKKMppyEdWGUBR27863526 = -734428288;    int HRKKMppyEdWGUBR46380808 = -125561703;    int HRKKMppyEdWGUBR94039279 = -213289337;    int HRKKMppyEdWGUBR88593540 = -554137651;    int HRKKMppyEdWGUBR95559203 = -993741471;    int HRKKMppyEdWGUBR98329059 = -112801093;    int HRKKMppyEdWGUBR4382908 = -444073146;    int HRKKMppyEdWGUBR91039918 = -616066491;    int HRKKMppyEdWGUBR52265034 = -845605923;    int HRKKMppyEdWGUBR6280562 = -180713518;    int HRKKMppyEdWGUBR37885684 = -733559707;    int HRKKMppyEdWGUBR91643024 = -593786233;    int HRKKMppyEdWGUBR89012371 = -1948768;    int HRKKMppyEdWGUBR2919991 = -932788919;    int HRKKMppyEdWGUBR21417144 = -810500446;    int HRKKMppyEdWGUBR38559013 = -210875284;    int HRKKMppyEdWGUBR55136638 = -336560388;    int HRKKMppyEdWGUBR68901621 = 56753626;    int HRKKMppyEdWGUBR25181965 = -270715922;    int HRKKMppyEdWGUBR3895266 = -766480523;    int HRKKMppyEdWGUBR16021042 = -134624955;    int HRKKMppyEdWGUBR64470872 = -283564579;    int HRKKMppyEdWGUBR21631932 = 62531099;    int HRKKMppyEdWGUBR40346454 = -878267147;    int HRKKMppyEdWGUBR6887142 = -129566153;    int HRKKMppyEdWGUBR68512783 = -500587926;    int HRKKMppyEdWGUBR61703290 = -226621090;    int HRKKMppyEdWGUBR36251233 = -562680999;    int HRKKMppyEdWGUBR30337004 = -690911923;    int HRKKMppyEdWGUBR94244817 = -767680398;    int HRKKMppyEdWGUBR13198031 = -448503001;    int HRKKMppyEdWGUBR1222914 = -209199428;    int HRKKMppyEdWGUBR84495067 = -481995669;    int HRKKMppyEdWGUBR9813167 = -755380833;    int HRKKMppyEdWGUBR77623010 = -434012403;    int HRKKMppyEdWGUBR33517835 = -666427236;    int HRKKMppyEdWGUBR73425370 = 27048082;    int HRKKMppyEdWGUBR33267932 = -259853536;    int HRKKMppyEdWGUBR78095884 = -295280089;    int HRKKMppyEdWGUBR16854409 = -878260598;    int HRKKMppyEdWGUBR75453488 = -765538487;    int HRKKMppyEdWGUBR63293933 = -746018084;    int HRKKMppyEdWGUBR25300065 = -692201967;    int HRKKMppyEdWGUBR33811580 = -910626325;    int HRKKMppyEdWGUBR77385559 = -9906365;    int HRKKMppyEdWGUBR31290841 = -209755218;    int HRKKMppyEdWGUBR58693596 = -680993241;    int HRKKMppyEdWGUBR96816131 = -500034223;    int HRKKMppyEdWGUBR28432250 = -627955447;    int HRKKMppyEdWGUBR65702600 = -906609682;    int HRKKMppyEdWGUBR2523153 = -482899669;    int HRKKMppyEdWGUBR41344542 = -928252434;    int HRKKMppyEdWGUBR43230602 = -989205760;    int HRKKMppyEdWGUBR42477602 = -785499941;    int HRKKMppyEdWGUBR27207698 = -317424368;    int HRKKMppyEdWGUBR63392653 = -350863709;    int HRKKMppyEdWGUBR24748876 = -88092802;    int HRKKMppyEdWGUBR53692825 = -335022190;    int HRKKMppyEdWGUBR81706399 = -324571499;    int HRKKMppyEdWGUBR27046420 = -393153546;    int HRKKMppyEdWGUBR36625770 = -886180003;    int HRKKMppyEdWGUBR68131675 = -881392148;    int HRKKMppyEdWGUBR60702915 = -925154568;    int HRKKMppyEdWGUBR58020217 = 22074475;    int HRKKMppyEdWGUBR93082531 = -732210518;    int HRKKMppyEdWGUBR36662770 = -921372468;     HRKKMppyEdWGUBR42035971 = HRKKMppyEdWGUBR61724074;     HRKKMppyEdWGUBR61724074 = HRKKMppyEdWGUBR55617697;     HRKKMppyEdWGUBR55617697 = HRKKMppyEdWGUBR29821006;     HRKKMppyEdWGUBR29821006 = HRKKMppyEdWGUBR16218752;     HRKKMppyEdWGUBR16218752 = HRKKMppyEdWGUBR61439412;     HRKKMppyEdWGUBR61439412 = HRKKMppyEdWGUBR86100626;     HRKKMppyEdWGUBR86100626 = HRKKMppyEdWGUBR34598616;     HRKKMppyEdWGUBR34598616 = HRKKMppyEdWGUBR51904607;     HRKKMppyEdWGUBR51904607 = HRKKMppyEdWGUBR23444194;     HRKKMppyEdWGUBR23444194 = HRKKMppyEdWGUBR69635480;     HRKKMppyEdWGUBR69635480 = HRKKMppyEdWGUBR30867934;     HRKKMppyEdWGUBR30867934 = HRKKMppyEdWGUBR53041906;     HRKKMppyEdWGUBR53041906 = HRKKMppyEdWGUBR5486536;     HRKKMppyEdWGUBR5486536 = HRKKMppyEdWGUBR79898643;     HRKKMppyEdWGUBR79898643 = HRKKMppyEdWGUBR67464650;     HRKKMppyEdWGUBR67464650 = HRKKMppyEdWGUBR21861473;     HRKKMppyEdWGUBR21861473 = HRKKMppyEdWGUBR73655087;     HRKKMppyEdWGUBR73655087 = HRKKMppyEdWGUBR15183469;     HRKKMppyEdWGUBR15183469 = HRKKMppyEdWGUBR79836396;     HRKKMppyEdWGUBR79836396 = HRKKMppyEdWGUBR54333851;     HRKKMppyEdWGUBR54333851 = HRKKMppyEdWGUBR77565098;     HRKKMppyEdWGUBR77565098 = HRKKMppyEdWGUBR40092142;     HRKKMppyEdWGUBR40092142 = HRKKMppyEdWGUBR15271243;     HRKKMppyEdWGUBR15271243 = HRKKMppyEdWGUBR22933865;     HRKKMppyEdWGUBR22933865 = HRKKMppyEdWGUBR47705968;     HRKKMppyEdWGUBR47705968 = HRKKMppyEdWGUBR99736121;     HRKKMppyEdWGUBR99736121 = HRKKMppyEdWGUBR49849394;     HRKKMppyEdWGUBR49849394 = HRKKMppyEdWGUBR4261613;     HRKKMppyEdWGUBR4261613 = HRKKMppyEdWGUBR57659790;     HRKKMppyEdWGUBR57659790 = HRKKMppyEdWGUBR10246164;     HRKKMppyEdWGUBR10246164 = HRKKMppyEdWGUBR68412567;     HRKKMppyEdWGUBR68412567 = HRKKMppyEdWGUBR46372867;     HRKKMppyEdWGUBR46372867 = HRKKMppyEdWGUBR43228739;     HRKKMppyEdWGUBR43228739 = HRKKMppyEdWGUBR27863526;     HRKKMppyEdWGUBR27863526 = HRKKMppyEdWGUBR46380808;     HRKKMppyEdWGUBR46380808 = HRKKMppyEdWGUBR94039279;     HRKKMppyEdWGUBR94039279 = HRKKMppyEdWGUBR88593540;     HRKKMppyEdWGUBR88593540 = HRKKMppyEdWGUBR95559203;     HRKKMppyEdWGUBR95559203 = HRKKMppyEdWGUBR98329059;     HRKKMppyEdWGUBR98329059 = HRKKMppyEdWGUBR4382908;     HRKKMppyEdWGUBR4382908 = HRKKMppyEdWGUBR91039918;     HRKKMppyEdWGUBR91039918 = HRKKMppyEdWGUBR52265034;     HRKKMppyEdWGUBR52265034 = HRKKMppyEdWGUBR6280562;     HRKKMppyEdWGUBR6280562 = HRKKMppyEdWGUBR37885684;     HRKKMppyEdWGUBR37885684 = HRKKMppyEdWGUBR91643024;     HRKKMppyEdWGUBR91643024 = HRKKMppyEdWGUBR89012371;     HRKKMppyEdWGUBR89012371 = HRKKMppyEdWGUBR2919991;     HRKKMppyEdWGUBR2919991 = HRKKMppyEdWGUBR21417144;     HRKKMppyEdWGUBR21417144 = HRKKMppyEdWGUBR38559013;     HRKKMppyEdWGUBR38559013 = HRKKMppyEdWGUBR55136638;     HRKKMppyEdWGUBR55136638 = HRKKMppyEdWGUBR68901621;     HRKKMppyEdWGUBR68901621 = HRKKMppyEdWGUBR25181965;     HRKKMppyEdWGUBR25181965 = HRKKMppyEdWGUBR3895266;     HRKKMppyEdWGUBR3895266 = HRKKMppyEdWGUBR16021042;     HRKKMppyEdWGUBR16021042 = HRKKMppyEdWGUBR64470872;     HRKKMppyEdWGUBR64470872 = HRKKMppyEdWGUBR21631932;     HRKKMppyEdWGUBR21631932 = HRKKMppyEdWGUBR40346454;     HRKKMppyEdWGUBR40346454 = HRKKMppyEdWGUBR6887142;     HRKKMppyEdWGUBR6887142 = HRKKMppyEdWGUBR68512783;     HRKKMppyEdWGUBR68512783 = HRKKMppyEdWGUBR61703290;     HRKKMppyEdWGUBR61703290 = HRKKMppyEdWGUBR36251233;     HRKKMppyEdWGUBR36251233 = HRKKMppyEdWGUBR30337004;     HRKKMppyEdWGUBR30337004 = HRKKMppyEdWGUBR94244817;     HRKKMppyEdWGUBR94244817 = HRKKMppyEdWGUBR13198031;     HRKKMppyEdWGUBR13198031 = HRKKMppyEdWGUBR1222914;     HRKKMppyEdWGUBR1222914 = HRKKMppyEdWGUBR84495067;     HRKKMppyEdWGUBR84495067 = HRKKMppyEdWGUBR9813167;     HRKKMppyEdWGUBR9813167 = HRKKMppyEdWGUBR77623010;     HRKKMppyEdWGUBR77623010 = HRKKMppyEdWGUBR33517835;     HRKKMppyEdWGUBR33517835 = HRKKMppyEdWGUBR73425370;     HRKKMppyEdWGUBR73425370 = HRKKMppyEdWGUBR33267932;     HRKKMppyEdWGUBR33267932 = HRKKMppyEdWGUBR78095884;     HRKKMppyEdWGUBR78095884 = HRKKMppyEdWGUBR16854409;     HRKKMppyEdWGUBR16854409 = HRKKMppyEdWGUBR75453488;     HRKKMppyEdWGUBR75453488 = HRKKMppyEdWGUBR63293933;     HRKKMppyEdWGUBR63293933 = HRKKMppyEdWGUBR25300065;     HRKKMppyEdWGUBR25300065 = HRKKMppyEdWGUBR33811580;     HRKKMppyEdWGUBR33811580 = HRKKMppyEdWGUBR77385559;     HRKKMppyEdWGUBR77385559 = HRKKMppyEdWGUBR31290841;     HRKKMppyEdWGUBR31290841 = HRKKMppyEdWGUBR58693596;     HRKKMppyEdWGUBR58693596 = HRKKMppyEdWGUBR96816131;     HRKKMppyEdWGUBR96816131 = HRKKMppyEdWGUBR28432250;     HRKKMppyEdWGUBR28432250 = HRKKMppyEdWGUBR65702600;     HRKKMppyEdWGUBR65702600 = HRKKMppyEdWGUBR2523153;     HRKKMppyEdWGUBR2523153 = HRKKMppyEdWGUBR41344542;     HRKKMppyEdWGUBR41344542 = HRKKMppyEdWGUBR43230602;     HRKKMppyEdWGUBR43230602 = HRKKMppyEdWGUBR42477602;     HRKKMppyEdWGUBR42477602 = HRKKMppyEdWGUBR27207698;     HRKKMppyEdWGUBR27207698 = HRKKMppyEdWGUBR63392653;     HRKKMppyEdWGUBR63392653 = HRKKMppyEdWGUBR24748876;     HRKKMppyEdWGUBR24748876 = HRKKMppyEdWGUBR53692825;     HRKKMppyEdWGUBR53692825 = HRKKMppyEdWGUBR81706399;     HRKKMppyEdWGUBR81706399 = HRKKMppyEdWGUBR27046420;     HRKKMppyEdWGUBR27046420 = HRKKMppyEdWGUBR36625770;     HRKKMppyEdWGUBR36625770 = HRKKMppyEdWGUBR68131675;     HRKKMppyEdWGUBR68131675 = HRKKMppyEdWGUBR60702915;     HRKKMppyEdWGUBR60702915 = HRKKMppyEdWGUBR58020217;     HRKKMppyEdWGUBR58020217 = HRKKMppyEdWGUBR93082531;     HRKKMppyEdWGUBR93082531 = HRKKMppyEdWGUBR36662770;     HRKKMppyEdWGUBR36662770 = HRKKMppyEdWGUBR42035971;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ulEaPXoDfDTYApwO24771106() {     int jNJbhLXiAniOhYf12687638 = -439521598;    int jNJbhLXiAniOhYf30397592 = -452043;    int jNJbhLXiAniOhYf35586616 = -64027678;    int jNJbhLXiAniOhYf69752293 = -223245992;    int jNJbhLXiAniOhYf37669073 = -66936281;    int jNJbhLXiAniOhYf51439175 = -461025383;    int jNJbhLXiAniOhYf4479523 = 87399;    int jNJbhLXiAniOhYf91440996 = -155213978;    int jNJbhLXiAniOhYf97769661 = -780487783;    int jNJbhLXiAniOhYf46410655 = -721097158;    int jNJbhLXiAniOhYf11884641 = -604023039;    int jNJbhLXiAniOhYf76414505 = -349739925;    int jNJbhLXiAniOhYf32869293 = -216967213;    int jNJbhLXiAniOhYf89333569 = 30531099;    int jNJbhLXiAniOhYf62618662 = 58261059;    int jNJbhLXiAniOhYf99993289 = -956224841;    int jNJbhLXiAniOhYf15714299 = -823176317;    int jNJbhLXiAniOhYf93948581 = -278187756;    int jNJbhLXiAniOhYf96891250 = -698249632;    int jNJbhLXiAniOhYf38623105 = -751977937;    int jNJbhLXiAniOhYf9469606 = -270763715;    int jNJbhLXiAniOhYf38960863 = -869093195;    int jNJbhLXiAniOhYf42035951 = -659182987;    int jNJbhLXiAniOhYf49120780 = -435866978;    int jNJbhLXiAniOhYf87058253 = -579446138;    int jNJbhLXiAniOhYf78303548 = -881226453;    int jNJbhLXiAniOhYf7996908 = -828000917;    int jNJbhLXiAniOhYf57631619 = -943579660;    int jNJbhLXiAniOhYf36404100 = -411843182;    int jNJbhLXiAniOhYf96831886 = -377855127;    int jNJbhLXiAniOhYf17318160 = -719932493;    int jNJbhLXiAniOhYf12924028 = -588218845;    int jNJbhLXiAniOhYf29318228 = -498942072;    int jNJbhLXiAniOhYf8593714 = -115579006;    int jNJbhLXiAniOhYf5887762 = -581375068;    int jNJbhLXiAniOhYf6374067 = -204788251;    int jNJbhLXiAniOhYf37830766 = -401882544;    int jNJbhLXiAniOhYf41386029 = -338250759;    int jNJbhLXiAniOhYf74092492 = -949470108;    int jNJbhLXiAniOhYf93432357 = -730409455;    int jNJbhLXiAniOhYf64456192 = -828092024;    int jNJbhLXiAniOhYf19522910 = -385455560;    int jNJbhLXiAniOhYf1231995 = -832359975;    int jNJbhLXiAniOhYf12788863 = -252688489;    int jNJbhLXiAniOhYf33839743 = 57793899;    int jNJbhLXiAniOhYf98048859 = -352123388;    int jNJbhLXiAniOhYf15143575 = -118018567;    int jNJbhLXiAniOhYf68534200 = -284919214;    int jNJbhLXiAniOhYf26858597 = -796264416;    int jNJbhLXiAniOhYf46986499 = -493884162;    int jNJbhLXiAniOhYf23325963 = -983132509;    int jNJbhLXiAniOhYf10773126 = 87533904;    int jNJbhLXiAniOhYf80257621 = -198513964;    int jNJbhLXiAniOhYf94122569 = -327362019;    int jNJbhLXiAniOhYf15301500 = -245565869;    int jNJbhLXiAniOhYf73726774 = -570428404;    int jNJbhLXiAniOhYf88361640 = -341269056;    int jNJbhLXiAniOhYf86465836 = -628160701;    int jNJbhLXiAniOhYf82694039 = -643799854;    int jNJbhLXiAniOhYf59365524 = -185709828;    int jNJbhLXiAniOhYf43442267 = -633024467;    int jNJbhLXiAniOhYf46847903 = -56332941;    int jNJbhLXiAniOhYf55036896 = -743370796;    int jNJbhLXiAniOhYf937776 = -302632656;    int jNJbhLXiAniOhYf29092495 = 98835334;    int jNJbhLXiAniOhYf98960613 = 84195805;    int jNJbhLXiAniOhYf47096277 = -850797853;    int jNJbhLXiAniOhYf24275579 = -1388207;    int jNJbhLXiAniOhYf83445807 = -388093834;    int jNJbhLXiAniOhYf56244595 = -736950691;    int jNJbhLXiAniOhYf62162524 = -454342297;    int jNJbhLXiAniOhYf74328270 = -384925558;    int jNJbhLXiAniOhYf19856090 = -328717648;    int jNJbhLXiAniOhYf3458893 = -967840178;    int jNJbhLXiAniOhYf74166913 = -923885913;    int jNJbhLXiAniOhYf89946696 = -885308155;    int jNJbhLXiAniOhYf37728869 = 63266780;    int jNJbhLXiAniOhYf29247089 = -306494498;    int jNJbhLXiAniOhYf15281038 = -393660877;    int jNJbhLXiAniOhYf89009394 = -127322751;    int jNJbhLXiAniOhYf63159973 = -663207887;    int jNJbhLXiAniOhYf39462707 = -443081703;    int jNJbhLXiAniOhYf30773023 = -47315245;    int jNJbhLXiAniOhYf89417601 = -917959021;    int jNJbhLXiAniOhYf73505924 = -394722618;    int jNJbhLXiAniOhYf6545035 = -707466398;    int jNJbhLXiAniOhYf32666406 = -289704881;    int jNJbhLXiAniOhYf35195659 = -71580053;    int jNJbhLXiAniOhYf93292213 = -870013138;    int jNJbhLXiAniOhYf32160988 = 89053336;    int jNJbhLXiAniOhYf18012427 = -863519195;    int jNJbhLXiAniOhYf51364929 = -773721844;    int jNJbhLXiAniOhYf58691990 = -694450905;    int jNJbhLXiAniOhYf14726968 = -663760280;    int jNJbhLXiAniOhYf49990091 = 2615011;    int jNJbhLXiAniOhYf17608289 = -671759084;    int jNJbhLXiAniOhYf64486013 = -642084765;    int jNJbhLXiAniOhYf294219 = -429727319;    int jNJbhLXiAniOhYf83696367 = -251523824;    int jNJbhLXiAniOhYf34879129 = -439521598;     jNJbhLXiAniOhYf12687638 = jNJbhLXiAniOhYf30397592;     jNJbhLXiAniOhYf30397592 = jNJbhLXiAniOhYf35586616;     jNJbhLXiAniOhYf35586616 = jNJbhLXiAniOhYf69752293;     jNJbhLXiAniOhYf69752293 = jNJbhLXiAniOhYf37669073;     jNJbhLXiAniOhYf37669073 = jNJbhLXiAniOhYf51439175;     jNJbhLXiAniOhYf51439175 = jNJbhLXiAniOhYf4479523;     jNJbhLXiAniOhYf4479523 = jNJbhLXiAniOhYf91440996;     jNJbhLXiAniOhYf91440996 = jNJbhLXiAniOhYf97769661;     jNJbhLXiAniOhYf97769661 = jNJbhLXiAniOhYf46410655;     jNJbhLXiAniOhYf46410655 = jNJbhLXiAniOhYf11884641;     jNJbhLXiAniOhYf11884641 = jNJbhLXiAniOhYf76414505;     jNJbhLXiAniOhYf76414505 = jNJbhLXiAniOhYf32869293;     jNJbhLXiAniOhYf32869293 = jNJbhLXiAniOhYf89333569;     jNJbhLXiAniOhYf89333569 = jNJbhLXiAniOhYf62618662;     jNJbhLXiAniOhYf62618662 = jNJbhLXiAniOhYf99993289;     jNJbhLXiAniOhYf99993289 = jNJbhLXiAniOhYf15714299;     jNJbhLXiAniOhYf15714299 = jNJbhLXiAniOhYf93948581;     jNJbhLXiAniOhYf93948581 = jNJbhLXiAniOhYf96891250;     jNJbhLXiAniOhYf96891250 = jNJbhLXiAniOhYf38623105;     jNJbhLXiAniOhYf38623105 = jNJbhLXiAniOhYf9469606;     jNJbhLXiAniOhYf9469606 = jNJbhLXiAniOhYf38960863;     jNJbhLXiAniOhYf38960863 = jNJbhLXiAniOhYf42035951;     jNJbhLXiAniOhYf42035951 = jNJbhLXiAniOhYf49120780;     jNJbhLXiAniOhYf49120780 = jNJbhLXiAniOhYf87058253;     jNJbhLXiAniOhYf87058253 = jNJbhLXiAniOhYf78303548;     jNJbhLXiAniOhYf78303548 = jNJbhLXiAniOhYf7996908;     jNJbhLXiAniOhYf7996908 = jNJbhLXiAniOhYf57631619;     jNJbhLXiAniOhYf57631619 = jNJbhLXiAniOhYf36404100;     jNJbhLXiAniOhYf36404100 = jNJbhLXiAniOhYf96831886;     jNJbhLXiAniOhYf96831886 = jNJbhLXiAniOhYf17318160;     jNJbhLXiAniOhYf17318160 = jNJbhLXiAniOhYf12924028;     jNJbhLXiAniOhYf12924028 = jNJbhLXiAniOhYf29318228;     jNJbhLXiAniOhYf29318228 = jNJbhLXiAniOhYf8593714;     jNJbhLXiAniOhYf8593714 = jNJbhLXiAniOhYf5887762;     jNJbhLXiAniOhYf5887762 = jNJbhLXiAniOhYf6374067;     jNJbhLXiAniOhYf6374067 = jNJbhLXiAniOhYf37830766;     jNJbhLXiAniOhYf37830766 = jNJbhLXiAniOhYf41386029;     jNJbhLXiAniOhYf41386029 = jNJbhLXiAniOhYf74092492;     jNJbhLXiAniOhYf74092492 = jNJbhLXiAniOhYf93432357;     jNJbhLXiAniOhYf93432357 = jNJbhLXiAniOhYf64456192;     jNJbhLXiAniOhYf64456192 = jNJbhLXiAniOhYf19522910;     jNJbhLXiAniOhYf19522910 = jNJbhLXiAniOhYf1231995;     jNJbhLXiAniOhYf1231995 = jNJbhLXiAniOhYf12788863;     jNJbhLXiAniOhYf12788863 = jNJbhLXiAniOhYf33839743;     jNJbhLXiAniOhYf33839743 = jNJbhLXiAniOhYf98048859;     jNJbhLXiAniOhYf98048859 = jNJbhLXiAniOhYf15143575;     jNJbhLXiAniOhYf15143575 = jNJbhLXiAniOhYf68534200;     jNJbhLXiAniOhYf68534200 = jNJbhLXiAniOhYf26858597;     jNJbhLXiAniOhYf26858597 = jNJbhLXiAniOhYf46986499;     jNJbhLXiAniOhYf46986499 = jNJbhLXiAniOhYf23325963;     jNJbhLXiAniOhYf23325963 = jNJbhLXiAniOhYf10773126;     jNJbhLXiAniOhYf10773126 = jNJbhLXiAniOhYf80257621;     jNJbhLXiAniOhYf80257621 = jNJbhLXiAniOhYf94122569;     jNJbhLXiAniOhYf94122569 = jNJbhLXiAniOhYf15301500;     jNJbhLXiAniOhYf15301500 = jNJbhLXiAniOhYf73726774;     jNJbhLXiAniOhYf73726774 = jNJbhLXiAniOhYf88361640;     jNJbhLXiAniOhYf88361640 = jNJbhLXiAniOhYf86465836;     jNJbhLXiAniOhYf86465836 = jNJbhLXiAniOhYf82694039;     jNJbhLXiAniOhYf82694039 = jNJbhLXiAniOhYf59365524;     jNJbhLXiAniOhYf59365524 = jNJbhLXiAniOhYf43442267;     jNJbhLXiAniOhYf43442267 = jNJbhLXiAniOhYf46847903;     jNJbhLXiAniOhYf46847903 = jNJbhLXiAniOhYf55036896;     jNJbhLXiAniOhYf55036896 = jNJbhLXiAniOhYf937776;     jNJbhLXiAniOhYf937776 = jNJbhLXiAniOhYf29092495;     jNJbhLXiAniOhYf29092495 = jNJbhLXiAniOhYf98960613;     jNJbhLXiAniOhYf98960613 = jNJbhLXiAniOhYf47096277;     jNJbhLXiAniOhYf47096277 = jNJbhLXiAniOhYf24275579;     jNJbhLXiAniOhYf24275579 = jNJbhLXiAniOhYf83445807;     jNJbhLXiAniOhYf83445807 = jNJbhLXiAniOhYf56244595;     jNJbhLXiAniOhYf56244595 = jNJbhLXiAniOhYf62162524;     jNJbhLXiAniOhYf62162524 = jNJbhLXiAniOhYf74328270;     jNJbhLXiAniOhYf74328270 = jNJbhLXiAniOhYf19856090;     jNJbhLXiAniOhYf19856090 = jNJbhLXiAniOhYf3458893;     jNJbhLXiAniOhYf3458893 = jNJbhLXiAniOhYf74166913;     jNJbhLXiAniOhYf74166913 = jNJbhLXiAniOhYf89946696;     jNJbhLXiAniOhYf89946696 = jNJbhLXiAniOhYf37728869;     jNJbhLXiAniOhYf37728869 = jNJbhLXiAniOhYf29247089;     jNJbhLXiAniOhYf29247089 = jNJbhLXiAniOhYf15281038;     jNJbhLXiAniOhYf15281038 = jNJbhLXiAniOhYf89009394;     jNJbhLXiAniOhYf89009394 = jNJbhLXiAniOhYf63159973;     jNJbhLXiAniOhYf63159973 = jNJbhLXiAniOhYf39462707;     jNJbhLXiAniOhYf39462707 = jNJbhLXiAniOhYf30773023;     jNJbhLXiAniOhYf30773023 = jNJbhLXiAniOhYf89417601;     jNJbhLXiAniOhYf89417601 = jNJbhLXiAniOhYf73505924;     jNJbhLXiAniOhYf73505924 = jNJbhLXiAniOhYf6545035;     jNJbhLXiAniOhYf6545035 = jNJbhLXiAniOhYf32666406;     jNJbhLXiAniOhYf32666406 = jNJbhLXiAniOhYf35195659;     jNJbhLXiAniOhYf35195659 = jNJbhLXiAniOhYf93292213;     jNJbhLXiAniOhYf93292213 = jNJbhLXiAniOhYf32160988;     jNJbhLXiAniOhYf32160988 = jNJbhLXiAniOhYf18012427;     jNJbhLXiAniOhYf18012427 = jNJbhLXiAniOhYf51364929;     jNJbhLXiAniOhYf51364929 = jNJbhLXiAniOhYf58691990;     jNJbhLXiAniOhYf58691990 = jNJbhLXiAniOhYf14726968;     jNJbhLXiAniOhYf14726968 = jNJbhLXiAniOhYf49990091;     jNJbhLXiAniOhYf49990091 = jNJbhLXiAniOhYf17608289;     jNJbhLXiAniOhYf17608289 = jNJbhLXiAniOhYf64486013;     jNJbhLXiAniOhYf64486013 = jNJbhLXiAniOhYf294219;     jNJbhLXiAniOhYf294219 = jNJbhLXiAniOhYf83696367;     jNJbhLXiAniOhYf83696367 = jNJbhLXiAniOhYf34879129;     jNJbhLXiAniOhYf34879129 = jNJbhLXiAniOhYf12687638;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void doUbdnYLuBmPzEFY31429850() {     int RbmiNjcTMMdoHxU12681167 = -312045739;    int RbmiNjcTMMdoHxU68969908 = -43047168;    int RbmiNjcTMMdoHxU36899828 = -21156292;    int RbmiNjcTMMdoHxU54832154 = -43786822;    int RbmiNjcTMMdoHxU46509175 = -535291602;    int RbmiNjcTMMdoHxU86244607 = -160458195;    int RbmiNjcTMMdoHxU73026858 = -804449068;    int RbmiNjcTMMdoHxU60655047 = -959391295;    int RbmiNjcTMMdoHxU80509826 = -494133853;    int RbmiNjcTMMdoHxU43050168 = -357904029;    int RbmiNjcTMMdoHxU32205246 = -563495111;    int RbmiNjcTMMdoHxU90706182 = -85218287;    int RbmiNjcTMMdoHxU64722206 = -238625687;    int RbmiNjcTMMdoHxU43177790 = -314621952;    int RbmiNjcTMMdoHxU7790772 = 95548207;    int RbmiNjcTMMdoHxU74557837 = -189020491;    int RbmiNjcTMMdoHxU71567058 = -642790886;    int RbmiNjcTMMdoHxU16949115 = -675259378;    int RbmiNjcTMMdoHxU17319521 = -216231899;    int RbmiNjcTMMdoHxU10696869 = -264002388;    int RbmiNjcTMMdoHxU89139030 = 18769911;    int RbmiNjcTMMdoHxU17257129 = -279085078;    int RbmiNjcTMMdoHxU78914347 = -18140829;    int RbmiNjcTMMdoHxU92770888 = -316787500;    int RbmiNjcTMMdoHxU15500943 = 93860009;    int RbmiNjcTMMdoHxU94877625 = -789432381;    int RbmiNjcTMMdoHxU78218489 = -28800630;    int RbmiNjcTMMdoHxU93035144 = -452546648;    int RbmiNjcTMMdoHxU49082547 = 42086503;    int RbmiNjcTMMdoHxU72921756 = -53286501;    int RbmiNjcTMMdoHxU50730839 = -7025728;    int RbmiNjcTMMdoHxU97647884 = -980040606;    int RbmiNjcTMMdoHxU14770652 = -700886288;    int RbmiNjcTMMdoHxU8876551 = 7739262;    int RbmiNjcTMMdoHxU88245800 = -840688992;    int RbmiNjcTMMdoHxU49202257 = -284553997;    int RbmiNjcTMMdoHxU49772133 = -501535922;    int RbmiNjcTMMdoHxU11303709 = -333513438;    int RbmiNjcTMMdoHxU9406089 = -562151524;    int RbmiNjcTMMdoHxU65305251 = -339841141;    int RbmiNjcTMMdoHxU52311859 = -916035923;    int RbmiNjcTMMdoHxU5220571 = -579247643;    int RbmiNjcTMMdoHxU61515354 = -291490229;    int RbmiNjcTMMdoHxU72020760 = -909652933;    int RbmiNjcTMMdoHxU17857795 = -415465503;    int RbmiNjcTMMdoHxU38476828 = -164168059;    int RbmiNjcTMMdoHxU42841368 = -599299189;    int RbmiNjcTMMdoHxU43917782 = -893680570;    int RbmiNjcTMMdoHxU45516986 = -360028277;    int RbmiNjcTMMdoHxU67031922 = -80909048;    int RbmiNjcTMMdoHxU36585332 = -561687517;    int RbmiNjcTMMdoHxU25320384 = -294611844;    int RbmiNjcTMMdoHxU55705970 = -90567972;    int RbmiNjcTMMdoHxU4372841 = -859281871;    int RbmiNjcTMMdoHxU42290820 = -2465147;    int RbmiNjcTMMdoHxU95424037 = 67039338;    int RbmiNjcTMMdoHxU90055560 = 75093661;    int RbmiNjcTMMdoHxU44128940 = -704368792;    int RbmiNjcTMMdoHxU39331211 = -37646831;    int RbmiNjcTMMdoHxU51631549 = -745859221;    int RbmiNjcTMMdoHxU8026118 = -31657565;    int RbmiNjcTMMdoHxU79991714 = -251902420;    int RbmiNjcTMMdoHxU11572500 = -901477798;    int RbmiNjcTMMdoHxU7588070 = -340847352;    int RbmiNjcTMMdoHxU92319329 = -250878302;    int RbmiNjcTMMdoHxU34557362 = -583454505;    int RbmiNjcTMMdoHxU75935531 = -384331999;    int RbmiNjcTMMdoHxU55845655 = -146364949;    int RbmiNjcTMMdoHxU54931990 = -473932961;    int RbmiNjcTMMdoHxU58588515 = -619897797;    int RbmiNjcTMMdoHxU24785704 = -687484569;    int RbmiNjcTMMdoHxU60263350 = -209277449;    int RbmiNjcTMMdoHxU7543026 = -13107854;    int RbmiNjcTMMdoHxU52014269 = -876390758;    int RbmiNjcTMMdoHxU58385009 = -347966466;    int RbmiNjcTMMdoHxU83918459 = -401982446;    int RbmiNjcTMMdoHxU55741775 = -987594849;    int RbmiNjcTMMdoHxU6893587 = -108487897;    int RbmiNjcTMMdoHxU74913093 = -901321998;    int RbmiNjcTMMdoHxU77024115 = -741971933;    int RbmiNjcTMMdoHxU52036258 = -90133193;    int RbmiNjcTMMdoHxU34300708 = -135120060;    int RbmiNjcTMMdoHxU47518158 = 7481628;    int RbmiNjcTMMdoHxU82050625 = -877004449;    int RbmiNjcTMMdoHxU36336424 = -491598984;    int RbmiNjcTMMdoHxU25410456 = -712413884;    int RbmiNjcTMMdoHxU41941915 = -789472635;    int RbmiNjcTMMdoHxU10397811 = -841604418;    int RbmiNjcTMMdoHxU66585731 = -989795592;    int RbmiNjcTMMdoHxU92821762 = -807728331;    int RbmiNjcTMMdoHxU59146696 = -259647658;    int RbmiNjcTMMdoHxU5643194 = -797167131;    int RbmiNjcTMMdoHxU71972498 = -195866607;    int RbmiNjcTMMdoHxU57774539 = -816292303;    int RbmiNjcTMMdoHxU57279133 = -208183577;    int RbmiNjcTMMdoHxU72320145 = -564133504;    int RbmiNjcTMMdoHxU93648070 = -677769845;    int RbmiNjcTMMdoHxU53927284 = -950642877;    int RbmiNjcTMMdoHxU79701431 = -558774631;    int RbmiNjcTMMdoHxU83300433 = -312045739;     RbmiNjcTMMdoHxU12681167 = RbmiNjcTMMdoHxU68969908;     RbmiNjcTMMdoHxU68969908 = RbmiNjcTMMdoHxU36899828;     RbmiNjcTMMdoHxU36899828 = RbmiNjcTMMdoHxU54832154;     RbmiNjcTMMdoHxU54832154 = RbmiNjcTMMdoHxU46509175;     RbmiNjcTMMdoHxU46509175 = RbmiNjcTMMdoHxU86244607;     RbmiNjcTMMdoHxU86244607 = RbmiNjcTMMdoHxU73026858;     RbmiNjcTMMdoHxU73026858 = RbmiNjcTMMdoHxU60655047;     RbmiNjcTMMdoHxU60655047 = RbmiNjcTMMdoHxU80509826;     RbmiNjcTMMdoHxU80509826 = RbmiNjcTMMdoHxU43050168;     RbmiNjcTMMdoHxU43050168 = RbmiNjcTMMdoHxU32205246;     RbmiNjcTMMdoHxU32205246 = RbmiNjcTMMdoHxU90706182;     RbmiNjcTMMdoHxU90706182 = RbmiNjcTMMdoHxU64722206;     RbmiNjcTMMdoHxU64722206 = RbmiNjcTMMdoHxU43177790;     RbmiNjcTMMdoHxU43177790 = RbmiNjcTMMdoHxU7790772;     RbmiNjcTMMdoHxU7790772 = RbmiNjcTMMdoHxU74557837;     RbmiNjcTMMdoHxU74557837 = RbmiNjcTMMdoHxU71567058;     RbmiNjcTMMdoHxU71567058 = RbmiNjcTMMdoHxU16949115;     RbmiNjcTMMdoHxU16949115 = RbmiNjcTMMdoHxU17319521;     RbmiNjcTMMdoHxU17319521 = RbmiNjcTMMdoHxU10696869;     RbmiNjcTMMdoHxU10696869 = RbmiNjcTMMdoHxU89139030;     RbmiNjcTMMdoHxU89139030 = RbmiNjcTMMdoHxU17257129;     RbmiNjcTMMdoHxU17257129 = RbmiNjcTMMdoHxU78914347;     RbmiNjcTMMdoHxU78914347 = RbmiNjcTMMdoHxU92770888;     RbmiNjcTMMdoHxU92770888 = RbmiNjcTMMdoHxU15500943;     RbmiNjcTMMdoHxU15500943 = RbmiNjcTMMdoHxU94877625;     RbmiNjcTMMdoHxU94877625 = RbmiNjcTMMdoHxU78218489;     RbmiNjcTMMdoHxU78218489 = RbmiNjcTMMdoHxU93035144;     RbmiNjcTMMdoHxU93035144 = RbmiNjcTMMdoHxU49082547;     RbmiNjcTMMdoHxU49082547 = RbmiNjcTMMdoHxU72921756;     RbmiNjcTMMdoHxU72921756 = RbmiNjcTMMdoHxU50730839;     RbmiNjcTMMdoHxU50730839 = RbmiNjcTMMdoHxU97647884;     RbmiNjcTMMdoHxU97647884 = RbmiNjcTMMdoHxU14770652;     RbmiNjcTMMdoHxU14770652 = RbmiNjcTMMdoHxU8876551;     RbmiNjcTMMdoHxU8876551 = RbmiNjcTMMdoHxU88245800;     RbmiNjcTMMdoHxU88245800 = RbmiNjcTMMdoHxU49202257;     RbmiNjcTMMdoHxU49202257 = RbmiNjcTMMdoHxU49772133;     RbmiNjcTMMdoHxU49772133 = RbmiNjcTMMdoHxU11303709;     RbmiNjcTMMdoHxU11303709 = RbmiNjcTMMdoHxU9406089;     RbmiNjcTMMdoHxU9406089 = RbmiNjcTMMdoHxU65305251;     RbmiNjcTMMdoHxU65305251 = RbmiNjcTMMdoHxU52311859;     RbmiNjcTMMdoHxU52311859 = RbmiNjcTMMdoHxU5220571;     RbmiNjcTMMdoHxU5220571 = RbmiNjcTMMdoHxU61515354;     RbmiNjcTMMdoHxU61515354 = RbmiNjcTMMdoHxU72020760;     RbmiNjcTMMdoHxU72020760 = RbmiNjcTMMdoHxU17857795;     RbmiNjcTMMdoHxU17857795 = RbmiNjcTMMdoHxU38476828;     RbmiNjcTMMdoHxU38476828 = RbmiNjcTMMdoHxU42841368;     RbmiNjcTMMdoHxU42841368 = RbmiNjcTMMdoHxU43917782;     RbmiNjcTMMdoHxU43917782 = RbmiNjcTMMdoHxU45516986;     RbmiNjcTMMdoHxU45516986 = RbmiNjcTMMdoHxU67031922;     RbmiNjcTMMdoHxU67031922 = RbmiNjcTMMdoHxU36585332;     RbmiNjcTMMdoHxU36585332 = RbmiNjcTMMdoHxU25320384;     RbmiNjcTMMdoHxU25320384 = RbmiNjcTMMdoHxU55705970;     RbmiNjcTMMdoHxU55705970 = RbmiNjcTMMdoHxU4372841;     RbmiNjcTMMdoHxU4372841 = RbmiNjcTMMdoHxU42290820;     RbmiNjcTMMdoHxU42290820 = RbmiNjcTMMdoHxU95424037;     RbmiNjcTMMdoHxU95424037 = RbmiNjcTMMdoHxU90055560;     RbmiNjcTMMdoHxU90055560 = RbmiNjcTMMdoHxU44128940;     RbmiNjcTMMdoHxU44128940 = RbmiNjcTMMdoHxU39331211;     RbmiNjcTMMdoHxU39331211 = RbmiNjcTMMdoHxU51631549;     RbmiNjcTMMdoHxU51631549 = RbmiNjcTMMdoHxU8026118;     RbmiNjcTMMdoHxU8026118 = RbmiNjcTMMdoHxU79991714;     RbmiNjcTMMdoHxU79991714 = RbmiNjcTMMdoHxU11572500;     RbmiNjcTMMdoHxU11572500 = RbmiNjcTMMdoHxU7588070;     RbmiNjcTMMdoHxU7588070 = RbmiNjcTMMdoHxU92319329;     RbmiNjcTMMdoHxU92319329 = RbmiNjcTMMdoHxU34557362;     RbmiNjcTMMdoHxU34557362 = RbmiNjcTMMdoHxU75935531;     RbmiNjcTMMdoHxU75935531 = RbmiNjcTMMdoHxU55845655;     RbmiNjcTMMdoHxU55845655 = RbmiNjcTMMdoHxU54931990;     RbmiNjcTMMdoHxU54931990 = RbmiNjcTMMdoHxU58588515;     RbmiNjcTMMdoHxU58588515 = RbmiNjcTMMdoHxU24785704;     RbmiNjcTMMdoHxU24785704 = RbmiNjcTMMdoHxU60263350;     RbmiNjcTMMdoHxU60263350 = RbmiNjcTMMdoHxU7543026;     RbmiNjcTMMdoHxU7543026 = RbmiNjcTMMdoHxU52014269;     RbmiNjcTMMdoHxU52014269 = RbmiNjcTMMdoHxU58385009;     RbmiNjcTMMdoHxU58385009 = RbmiNjcTMMdoHxU83918459;     RbmiNjcTMMdoHxU83918459 = RbmiNjcTMMdoHxU55741775;     RbmiNjcTMMdoHxU55741775 = RbmiNjcTMMdoHxU6893587;     RbmiNjcTMMdoHxU6893587 = RbmiNjcTMMdoHxU74913093;     RbmiNjcTMMdoHxU74913093 = RbmiNjcTMMdoHxU77024115;     RbmiNjcTMMdoHxU77024115 = RbmiNjcTMMdoHxU52036258;     RbmiNjcTMMdoHxU52036258 = RbmiNjcTMMdoHxU34300708;     RbmiNjcTMMdoHxU34300708 = RbmiNjcTMMdoHxU47518158;     RbmiNjcTMMdoHxU47518158 = RbmiNjcTMMdoHxU82050625;     RbmiNjcTMMdoHxU82050625 = RbmiNjcTMMdoHxU36336424;     RbmiNjcTMMdoHxU36336424 = RbmiNjcTMMdoHxU25410456;     RbmiNjcTMMdoHxU25410456 = RbmiNjcTMMdoHxU41941915;     RbmiNjcTMMdoHxU41941915 = RbmiNjcTMMdoHxU10397811;     RbmiNjcTMMdoHxU10397811 = RbmiNjcTMMdoHxU66585731;     RbmiNjcTMMdoHxU66585731 = RbmiNjcTMMdoHxU92821762;     RbmiNjcTMMdoHxU92821762 = RbmiNjcTMMdoHxU59146696;     RbmiNjcTMMdoHxU59146696 = RbmiNjcTMMdoHxU5643194;     RbmiNjcTMMdoHxU5643194 = RbmiNjcTMMdoHxU71972498;     RbmiNjcTMMdoHxU71972498 = RbmiNjcTMMdoHxU57774539;     RbmiNjcTMMdoHxU57774539 = RbmiNjcTMMdoHxU57279133;     RbmiNjcTMMdoHxU57279133 = RbmiNjcTMMdoHxU72320145;     RbmiNjcTMMdoHxU72320145 = RbmiNjcTMMdoHxU93648070;     RbmiNjcTMMdoHxU93648070 = RbmiNjcTMMdoHxU53927284;     RbmiNjcTMMdoHxU53927284 = RbmiNjcTMMdoHxU79701431;     RbmiNjcTMMdoHxU79701431 = RbmiNjcTMMdoHxU83300433;     RbmiNjcTMMdoHxU83300433 = RbmiNjcTMMdoHxU12681167;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void uvYrTljldxWBKRnj16162491() {     int XWEyRJlhQzgkIWp51065553 = -127071249;    int XWEyRJlhQzgkIWp74993306 = -383668708;    int XWEyRJlhQzgkIWp84905379 = -317719982;    int XWEyRJlhQzgkIWp38335187 = -234230331;    int XWEyRJlhQzgkIWp91682079 = -137319711;    int XWEyRJlhQzgkIWp76953089 = -693496888;    int XWEyRJlhQzgkIWp62460789 = -323319411;    int XWEyRJlhQzgkIWp63956182 = -176236796;    int XWEyRJlhQzgkIWp35763601 = -424823850;    int XWEyRJlhQzgkIWp75148228 = -122939443;    int XWEyRJlhQzgkIWp33384972 = 30335131;    int XWEyRJlhQzgkIWp29390990 = -465303285;    int XWEyRJlhQzgkIWp47740459 = -963339761;    int XWEyRJlhQzgkIWp76673145 = 68744659;    int XWEyRJlhQzgkIWp97021994 = -874557878;    int XWEyRJlhQzgkIWp61574854 = -510512056;    int XWEyRJlhQzgkIWp15411474 = -355656008;    int XWEyRJlhQzgkIWp75622438 = 47396203;    int XWEyRJlhQzgkIWp39088332 = -772710419;    int XWEyRJlhQzgkIWp30365305 = -713637000;    int XWEyRJlhQzgkIWp10983504 = -413313387;    int XWEyRJlhQzgkIWp91215523 = -948692589;    int XWEyRJlhQzgkIWp47681647 = -175321145;    int XWEyRJlhQzgkIWp51691843 = -774140447;    int XWEyRJlhQzgkIWp19412963 = 69309648;    int XWEyRJlhQzgkIWp72537253 = -227902265;    int XWEyRJlhQzgkIWp31578771 = -726720619;    int XWEyRJlhQzgkIWp53479819 = -738073009;    int XWEyRJlhQzgkIWp48219919 = -216283958;    int XWEyRJlhQzgkIWp26815623 = -241219470;    int XWEyRJlhQzgkIWp50388112 = -387903039;    int XWEyRJlhQzgkIWp68708926 = -448912596;    int XWEyRJlhQzgkIWp11098806 = -620673778;    int XWEyRJlhQzgkIWp38974321 = -199796463;    int XWEyRJlhQzgkIWp7995245 = -533528935;    int XWEyRJlhQzgkIWp63561843 = -431180324;    int XWEyRJlhQzgkIWp12298844 = -572650461;    int XWEyRJlhQzgkIWp4745060 = -111322996;    int XWEyRJlhQzgkIWp85477860 = -536977591;    int XWEyRJlhQzgkIWp56607869 = -335364934;    int XWEyRJlhQzgkIWp85183113 = -590682309;    int XWEyRJlhQzgkIWp85537294 = -599013316;    int XWEyRJlhQzgkIWp25032324 = -325269547;    int XWEyRJlhQzgkIWp68978781 = -405311888;    int XWEyRJlhQzgkIWp21811057 = -400737777;    int XWEyRJlhQzgkIWp97649342 = -35944694;    int XWEyRJlhQzgkIWp64796256 = -166721857;    int XWEyRJlhQzgkIWp30703357 = -24693484;    int XWEyRJlhQzgkIWp3075577 = -395778756;    int XWEyRJlhQzgkIWp27701492 = -265918892;    int XWEyRJlhQzgkIWp38530334 = -806120413;    int XWEyRJlhQzgkIWp51660995 = -179584865;    int XWEyRJlhQzgkIWp57368027 = 49638213;    int XWEyRJlhQzgkIWp97470932 = -799957692;    int XWEyRJlhQzgkIWp5178987 = -958201495;    int XWEyRJlhQzgkIWp59850030 = -178378661;    int XWEyRJlhQzgkIWp27311659 = -108347563;    int XWEyRJlhQzgkIWp33213536 = -543579535;    int XWEyRJlhQzgkIWp18922225 = -203539980;    int XWEyRJlhQzgkIWp19144827 = -909417447;    int XWEyRJlhQzgkIWp45374318 = -966776270;    int XWEyRJlhQzgkIWp8980970 = -585246403;    int XWEyRJlhQzgkIWp15736264 = -959952839;    int XWEyRJlhQzgkIWp8947979 = -83604380;    int XWEyRJlhQzgkIWp24760117 = -735036405;    int XWEyRJlhQzgkIWp64676046 = -520752274;    int XWEyRJlhQzgkIWp18292184 = -844629507;    int XWEyRJlhQzgkIWp8766139 = -663543298;    int XWEyRJlhQzgkIWp68677901 = -397726407;    int XWEyRJlhQzgkIWp33460152 = -343377554;    int XWEyRJlhQzgkIWp49276011 = -937861596;    int XWEyRJlhQzgkIWp10666415 = -144333012;    int XWEyRJlhQzgkIWp90144578 = -415626207;    int XWEyRJlhQzgkIWp82480463 = -337345485;    int XWEyRJlhQzgkIWp45182192 = -22954692;    int XWEyRJlhQzgkIWp25446209 = -814300071;    int XWEyRJlhQzgkIWp66183200 = -523423042;    int XWEyRJlhQzgkIWp78702866 = -770009258;    int XWEyRJlhQzgkIWp29880787 = -273402671;    int XWEyRJlhQzgkIWp21763620 = -894745658;    int XWEyRJlhQzgkIWp7740998 = 38819592;    int XWEyRJlhQzgkIWp875415 = -602027135;    int XWEyRJlhQzgkIWp50404243 = -242294254;    int XWEyRJlhQzgkIWp20518427 = -950365067;    int XWEyRJlhQzgkIWp88285289 = -435099057;    int XWEyRJlhQzgkIWp98727116 = -108318175;    int XWEyRJlhQzgkIWp11340899 = -398550809;    int XWEyRJlhQzgkIWp13627873 = -820716087;    int XWEyRJlhQzgkIWp33795334 = -241594969;    int XWEyRJlhQzgkIWp48145214 = -255150274;    int XWEyRJlhQzgkIWp36250184 = -222832762;    int XWEyRJlhQzgkIWp79085307 = 70929074;    int XWEyRJlhQzgkIWp85822834 = -907783016;    int XWEyRJlhQzgkIWp66333033 = -627560145;    int XWEyRJlhQzgkIWp11233551 = -368588665;    int XWEyRJlhQzgkIWp76202143 = 94564093;    int XWEyRJlhQzgkIWp69801031 = -639060478;    int XWEyRJlhQzgkIWp16084345 = -141665167;    int XWEyRJlhQzgkIWp44218665 = -670275484;    int XWEyRJlhQzgkIWp57135011 = -127071249;     XWEyRJlhQzgkIWp51065553 = XWEyRJlhQzgkIWp74993306;     XWEyRJlhQzgkIWp74993306 = XWEyRJlhQzgkIWp84905379;     XWEyRJlhQzgkIWp84905379 = XWEyRJlhQzgkIWp38335187;     XWEyRJlhQzgkIWp38335187 = XWEyRJlhQzgkIWp91682079;     XWEyRJlhQzgkIWp91682079 = XWEyRJlhQzgkIWp76953089;     XWEyRJlhQzgkIWp76953089 = XWEyRJlhQzgkIWp62460789;     XWEyRJlhQzgkIWp62460789 = XWEyRJlhQzgkIWp63956182;     XWEyRJlhQzgkIWp63956182 = XWEyRJlhQzgkIWp35763601;     XWEyRJlhQzgkIWp35763601 = XWEyRJlhQzgkIWp75148228;     XWEyRJlhQzgkIWp75148228 = XWEyRJlhQzgkIWp33384972;     XWEyRJlhQzgkIWp33384972 = XWEyRJlhQzgkIWp29390990;     XWEyRJlhQzgkIWp29390990 = XWEyRJlhQzgkIWp47740459;     XWEyRJlhQzgkIWp47740459 = XWEyRJlhQzgkIWp76673145;     XWEyRJlhQzgkIWp76673145 = XWEyRJlhQzgkIWp97021994;     XWEyRJlhQzgkIWp97021994 = XWEyRJlhQzgkIWp61574854;     XWEyRJlhQzgkIWp61574854 = XWEyRJlhQzgkIWp15411474;     XWEyRJlhQzgkIWp15411474 = XWEyRJlhQzgkIWp75622438;     XWEyRJlhQzgkIWp75622438 = XWEyRJlhQzgkIWp39088332;     XWEyRJlhQzgkIWp39088332 = XWEyRJlhQzgkIWp30365305;     XWEyRJlhQzgkIWp30365305 = XWEyRJlhQzgkIWp10983504;     XWEyRJlhQzgkIWp10983504 = XWEyRJlhQzgkIWp91215523;     XWEyRJlhQzgkIWp91215523 = XWEyRJlhQzgkIWp47681647;     XWEyRJlhQzgkIWp47681647 = XWEyRJlhQzgkIWp51691843;     XWEyRJlhQzgkIWp51691843 = XWEyRJlhQzgkIWp19412963;     XWEyRJlhQzgkIWp19412963 = XWEyRJlhQzgkIWp72537253;     XWEyRJlhQzgkIWp72537253 = XWEyRJlhQzgkIWp31578771;     XWEyRJlhQzgkIWp31578771 = XWEyRJlhQzgkIWp53479819;     XWEyRJlhQzgkIWp53479819 = XWEyRJlhQzgkIWp48219919;     XWEyRJlhQzgkIWp48219919 = XWEyRJlhQzgkIWp26815623;     XWEyRJlhQzgkIWp26815623 = XWEyRJlhQzgkIWp50388112;     XWEyRJlhQzgkIWp50388112 = XWEyRJlhQzgkIWp68708926;     XWEyRJlhQzgkIWp68708926 = XWEyRJlhQzgkIWp11098806;     XWEyRJlhQzgkIWp11098806 = XWEyRJlhQzgkIWp38974321;     XWEyRJlhQzgkIWp38974321 = XWEyRJlhQzgkIWp7995245;     XWEyRJlhQzgkIWp7995245 = XWEyRJlhQzgkIWp63561843;     XWEyRJlhQzgkIWp63561843 = XWEyRJlhQzgkIWp12298844;     XWEyRJlhQzgkIWp12298844 = XWEyRJlhQzgkIWp4745060;     XWEyRJlhQzgkIWp4745060 = XWEyRJlhQzgkIWp85477860;     XWEyRJlhQzgkIWp85477860 = XWEyRJlhQzgkIWp56607869;     XWEyRJlhQzgkIWp56607869 = XWEyRJlhQzgkIWp85183113;     XWEyRJlhQzgkIWp85183113 = XWEyRJlhQzgkIWp85537294;     XWEyRJlhQzgkIWp85537294 = XWEyRJlhQzgkIWp25032324;     XWEyRJlhQzgkIWp25032324 = XWEyRJlhQzgkIWp68978781;     XWEyRJlhQzgkIWp68978781 = XWEyRJlhQzgkIWp21811057;     XWEyRJlhQzgkIWp21811057 = XWEyRJlhQzgkIWp97649342;     XWEyRJlhQzgkIWp97649342 = XWEyRJlhQzgkIWp64796256;     XWEyRJlhQzgkIWp64796256 = XWEyRJlhQzgkIWp30703357;     XWEyRJlhQzgkIWp30703357 = XWEyRJlhQzgkIWp3075577;     XWEyRJlhQzgkIWp3075577 = XWEyRJlhQzgkIWp27701492;     XWEyRJlhQzgkIWp27701492 = XWEyRJlhQzgkIWp38530334;     XWEyRJlhQzgkIWp38530334 = XWEyRJlhQzgkIWp51660995;     XWEyRJlhQzgkIWp51660995 = XWEyRJlhQzgkIWp57368027;     XWEyRJlhQzgkIWp57368027 = XWEyRJlhQzgkIWp97470932;     XWEyRJlhQzgkIWp97470932 = XWEyRJlhQzgkIWp5178987;     XWEyRJlhQzgkIWp5178987 = XWEyRJlhQzgkIWp59850030;     XWEyRJlhQzgkIWp59850030 = XWEyRJlhQzgkIWp27311659;     XWEyRJlhQzgkIWp27311659 = XWEyRJlhQzgkIWp33213536;     XWEyRJlhQzgkIWp33213536 = XWEyRJlhQzgkIWp18922225;     XWEyRJlhQzgkIWp18922225 = XWEyRJlhQzgkIWp19144827;     XWEyRJlhQzgkIWp19144827 = XWEyRJlhQzgkIWp45374318;     XWEyRJlhQzgkIWp45374318 = XWEyRJlhQzgkIWp8980970;     XWEyRJlhQzgkIWp8980970 = XWEyRJlhQzgkIWp15736264;     XWEyRJlhQzgkIWp15736264 = XWEyRJlhQzgkIWp8947979;     XWEyRJlhQzgkIWp8947979 = XWEyRJlhQzgkIWp24760117;     XWEyRJlhQzgkIWp24760117 = XWEyRJlhQzgkIWp64676046;     XWEyRJlhQzgkIWp64676046 = XWEyRJlhQzgkIWp18292184;     XWEyRJlhQzgkIWp18292184 = XWEyRJlhQzgkIWp8766139;     XWEyRJlhQzgkIWp8766139 = XWEyRJlhQzgkIWp68677901;     XWEyRJlhQzgkIWp68677901 = XWEyRJlhQzgkIWp33460152;     XWEyRJlhQzgkIWp33460152 = XWEyRJlhQzgkIWp49276011;     XWEyRJlhQzgkIWp49276011 = XWEyRJlhQzgkIWp10666415;     XWEyRJlhQzgkIWp10666415 = XWEyRJlhQzgkIWp90144578;     XWEyRJlhQzgkIWp90144578 = XWEyRJlhQzgkIWp82480463;     XWEyRJlhQzgkIWp82480463 = XWEyRJlhQzgkIWp45182192;     XWEyRJlhQzgkIWp45182192 = XWEyRJlhQzgkIWp25446209;     XWEyRJlhQzgkIWp25446209 = XWEyRJlhQzgkIWp66183200;     XWEyRJlhQzgkIWp66183200 = XWEyRJlhQzgkIWp78702866;     XWEyRJlhQzgkIWp78702866 = XWEyRJlhQzgkIWp29880787;     XWEyRJlhQzgkIWp29880787 = XWEyRJlhQzgkIWp21763620;     XWEyRJlhQzgkIWp21763620 = XWEyRJlhQzgkIWp7740998;     XWEyRJlhQzgkIWp7740998 = XWEyRJlhQzgkIWp875415;     XWEyRJlhQzgkIWp875415 = XWEyRJlhQzgkIWp50404243;     XWEyRJlhQzgkIWp50404243 = XWEyRJlhQzgkIWp20518427;     XWEyRJlhQzgkIWp20518427 = XWEyRJlhQzgkIWp88285289;     XWEyRJlhQzgkIWp88285289 = XWEyRJlhQzgkIWp98727116;     XWEyRJlhQzgkIWp98727116 = XWEyRJlhQzgkIWp11340899;     XWEyRJlhQzgkIWp11340899 = XWEyRJlhQzgkIWp13627873;     XWEyRJlhQzgkIWp13627873 = XWEyRJlhQzgkIWp33795334;     XWEyRJlhQzgkIWp33795334 = XWEyRJlhQzgkIWp48145214;     XWEyRJlhQzgkIWp48145214 = XWEyRJlhQzgkIWp36250184;     XWEyRJlhQzgkIWp36250184 = XWEyRJlhQzgkIWp79085307;     XWEyRJlhQzgkIWp79085307 = XWEyRJlhQzgkIWp85822834;     XWEyRJlhQzgkIWp85822834 = XWEyRJlhQzgkIWp66333033;     XWEyRJlhQzgkIWp66333033 = XWEyRJlhQzgkIWp11233551;     XWEyRJlhQzgkIWp11233551 = XWEyRJlhQzgkIWp76202143;     XWEyRJlhQzgkIWp76202143 = XWEyRJlhQzgkIWp69801031;     XWEyRJlhQzgkIWp69801031 = XWEyRJlhQzgkIWp16084345;     XWEyRJlhQzgkIWp16084345 = XWEyRJlhQzgkIWp44218665;     XWEyRJlhQzgkIWp44218665 = XWEyRJlhQzgkIWp57135011;     XWEyRJlhQzgkIWp57135011 = XWEyRJlhQzgkIWp51065553;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void pPiovhmMalnXjMSm22821236() {     float EPLOylaGSukMXYG51059082 = 404612;    float EPLOylaGSukMXYG13565623 = -426263827;    float EPLOylaGSukMXYG86218591 = -274848596;    float EPLOylaGSukMXYG23415048 = -54771165;    float EPLOylaGSukMXYG522182 = -605675025;    float EPLOylaGSukMXYG11758522 = -392929700;    float EPLOylaGSukMXYG31008125 = -27855878;    float EPLOylaGSukMXYG33170233 = -980414098;    float EPLOylaGSukMXYG18503765 = -138469918;    float EPLOylaGSukMXYG71787742 = -859746315;    float EPLOylaGSukMXYG53705577 = 70863060;    float EPLOylaGSukMXYG43682666 = -200781638;    float EPLOylaGSukMXYG79593373 = -984998235;    float EPLOylaGSukMXYG30517365 = -276408392;    float EPLOylaGSukMXYG42194104 = -837270711;    float EPLOylaGSukMXYG36139401 = -843307707;    float EPLOylaGSukMXYG71264233 = -175270578;    float EPLOylaGSukMXYG98622970 = -349675415;    float EPLOylaGSukMXYG59516601 = -290692686;    float EPLOylaGSukMXYG2439069 = -225661452;    float EPLOylaGSukMXYG90652928 = -123779755;    float EPLOylaGSukMXYG69511789 = -358684472;    float EPLOylaGSukMXYG84560043 = -634278987;    float EPLOylaGSukMXYG95341951 = -655060970;    float EPLOylaGSukMXYG47855652 = -357384192;    float EPLOylaGSukMXYG89111329 = -136108198;    float EPLOylaGSukMXYG1800353 = 72479668;    float EPLOylaGSukMXYG88883344 = -247039991;    float EPLOylaGSukMXYG60898366 = -862354288;    float EPLOylaGSukMXYG2905493 = 83349157;    float EPLOylaGSukMXYG83800790 = -774996274;    float EPLOylaGSukMXYG53432784 = -840734360;    float EPLOylaGSukMXYG96551229 = -822617997;    float EPLOylaGSukMXYG39257158 = -76478196;    float EPLOylaGSukMXYG90353282 = -792842863;    float EPLOylaGSukMXYG6390032 = -510946089;    float EPLOylaGSukMXYG24240212 = -672303839;    float EPLOylaGSukMXYG74662739 = -106585672;    float EPLOylaGSukMXYG20791457 = -149659019;    float EPLOylaGSukMXYG28480762 = 55203380;    float EPLOylaGSukMXYG73038781 = -678626207;    float EPLOylaGSukMXYG71234955 = -792805419;    float EPLOylaGSukMXYG85315683 = -884399803;    float EPLOylaGSukMXYG28210679 = 37723669;    float EPLOylaGSukMXYG5829109 = -873997180;    float EPLOylaGSukMXYG38077311 = -947989383;    float EPLOylaGSukMXYG92494049 = -648002480;    float EPLOylaGSukMXYG6086938 = -633454842;    float EPLOylaGSukMXYG21733966 = 40457358;    float EPLOylaGSukMXYG47746915 = -952943773;    float EPLOylaGSukMXYG51789703 = -384675420;    float EPLOylaGSukMXYG66208253 = -561730618;    float EPLOylaGSukMXYG32816376 = -942415792;    float EPLOylaGSukMXYG7721204 = -231877543;    float EPLOylaGSukMXYG32168308 = -715100778;    float EPLOylaGSukMXYG81547293 = -640910917;    float EPLOylaGSukMXYG29005580 = -791984840;    float EPLOylaGSukMXYG90876639 = -619787626;    float EPLOylaGSukMXYG75559396 = -697386973;    float EPLOylaGSukMXYG11410853 = -369566827;    float EPLOylaGSukMXYG9958169 = -365409368;    float EPLOylaGSukMXYG42124781 = -780815887;    float EPLOylaGSukMXYG72271867 = -18059810;    float EPLOylaGSukMXYG15598273 = -121819076;    float EPLOylaGSukMXYG87986951 = 15249959;    float EPLOylaGSukMXYG272794 = -88402580;    float EPLOylaGSukMXYG47131437 = -378163642;    float EPLOylaGSukMXYG40336215 = -808520040;    float EPLOylaGSukMXYG40164083 = -483565530;    float EPLOylaGSukMXYG35804072 = -226324623;    float EPLOylaGSukMXYG11899190 = -71003868;    float EPLOylaGSukMXYG96601494 = 31315094;    float EPLOylaGSukMXYG77831514 = -100016396;    float EPLOylaGSukMXYG31035840 = -245896067;    float EPLOylaGSukMXYG29400288 = -547035245;    float EPLOylaGSukMXYG19417973 = -330974336;    float EPLOylaGSukMXYG84196105 = -474284669;    float EPLOylaGSukMXYG56349364 = -572002657;    float EPLOylaGSukMXYG89512843 = -781063791;    float EPLOylaGSukMXYG9778341 = -409394809;    float EPLOylaGSukMXYG96617280 = -488105718;    float EPLOylaGSukMXYG95713415 = -294065491;    float EPLOylaGSukMXYG67149378 = -187497350;    float EPLOylaGSukMXYG13151451 = -909410516;    float EPLOylaGSukMXYG51115789 = -531975424;    float EPLOylaGSukMXYG17592538 = -113265657;    float EPLOylaGSukMXYG20616408 = -898318568;    float EPLOylaGSukMXYG88830025 = -490740454;    float EPLOylaGSukMXYG7088851 = -361377418;    float EPLOylaGSukMXYG8805990 = -51931946;    float EPLOylaGSukMXYG77384452 = -718961249;    float EPLOylaGSukMXYG33363572 = 47483787;    float EPLOylaGSukMXYG99103342 = -409198700;    float EPLOylaGSukMXYG9380605 = -780092192;    float EPLOylaGSukMXYG18522593 = -579387252;    float EPLOylaGSukMXYG30914000 = -897810321;    float EPLOylaGSukMXYG98963088 = -674745609;    float EPLOylaGSukMXYG69717411 = -662580728;    float EPLOylaGSukMXYG40223728 = -977526291;    float EPLOylaGSukMXYG5556316 = 404612;     EPLOylaGSukMXYG51059082 = EPLOylaGSukMXYG13565623;     EPLOylaGSukMXYG13565623 = EPLOylaGSukMXYG86218591;     EPLOylaGSukMXYG86218591 = EPLOylaGSukMXYG23415048;     EPLOylaGSukMXYG23415048 = EPLOylaGSukMXYG522182;     EPLOylaGSukMXYG522182 = EPLOylaGSukMXYG11758522;     EPLOylaGSukMXYG11758522 = EPLOylaGSukMXYG31008125;     EPLOylaGSukMXYG31008125 = EPLOylaGSukMXYG33170233;     EPLOylaGSukMXYG33170233 = EPLOylaGSukMXYG18503765;     EPLOylaGSukMXYG18503765 = EPLOylaGSukMXYG71787742;     EPLOylaGSukMXYG71787742 = EPLOylaGSukMXYG53705577;     EPLOylaGSukMXYG53705577 = EPLOylaGSukMXYG43682666;     EPLOylaGSukMXYG43682666 = EPLOylaGSukMXYG79593373;     EPLOylaGSukMXYG79593373 = EPLOylaGSukMXYG30517365;     EPLOylaGSukMXYG30517365 = EPLOylaGSukMXYG42194104;     EPLOylaGSukMXYG42194104 = EPLOylaGSukMXYG36139401;     EPLOylaGSukMXYG36139401 = EPLOylaGSukMXYG71264233;     EPLOylaGSukMXYG71264233 = EPLOylaGSukMXYG98622970;     EPLOylaGSukMXYG98622970 = EPLOylaGSukMXYG59516601;     EPLOylaGSukMXYG59516601 = EPLOylaGSukMXYG2439069;     EPLOylaGSukMXYG2439069 = EPLOylaGSukMXYG90652928;     EPLOylaGSukMXYG90652928 = EPLOylaGSukMXYG69511789;     EPLOylaGSukMXYG69511789 = EPLOylaGSukMXYG84560043;     EPLOylaGSukMXYG84560043 = EPLOylaGSukMXYG95341951;     EPLOylaGSukMXYG95341951 = EPLOylaGSukMXYG47855652;     EPLOylaGSukMXYG47855652 = EPLOylaGSukMXYG89111329;     EPLOylaGSukMXYG89111329 = EPLOylaGSukMXYG1800353;     EPLOylaGSukMXYG1800353 = EPLOylaGSukMXYG88883344;     EPLOylaGSukMXYG88883344 = EPLOylaGSukMXYG60898366;     EPLOylaGSukMXYG60898366 = EPLOylaGSukMXYG2905493;     EPLOylaGSukMXYG2905493 = EPLOylaGSukMXYG83800790;     EPLOylaGSukMXYG83800790 = EPLOylaGSukMXYG53432784;     EPLOylaGSukMXYG53432784 = EPLOylaGSukMXYG96551229;     EPLOylaGSukMXYG96551229 = EPLOylaGSukMXYG39257158;     EPLOylaGSukMXYG39257158 = EPLOylaGSukMXYG90353282;     EPLOylaGSukMXYG90353282 = EPLOylaGSukMXYG6390032;     EPLOylaGSukMXYG6390032 = EPLOylaGSukMXYG24240212;     EPLOylaGSukMXYG24240212 = EPLOylaGSukMXYG74662739;     EPLOylaGSukMXYG74662739 = EPLOylaGSukMXYG20791457;     EPLOylaGSukMXYG20791457 = EPLOylaGSukMXYG28480762;     EPLOylaGSukMXYG28480762 = EPLOylaGSukMXYG73038781;     EPLOylaGSukMXYG73038781 = EPLOylaGSukMXYG71234955;     EPLOylaGSukMXYG71234955 = EPLOylaGSukMXYG85315683;     EPLOylaGSukMXYG85315683 = EPLOylaGSukMXYG28210679;     EPLOylaGSukMXYG28210679 = EPLOylaGSukMXYG5829109;     EPLOylaGSukMXYG5829109 = EPLOylaGSukMXYG38077311;     EPLOylaGSukMXYG38077311 = EPLOylaGSukMXYG92494049;     EPLOylaGSukMXYG92494049 = EPLOylaGSukMXYG6086938;     EPLOylaGSukMXYG6086938 = EPLOylaGSukMXYG21733966;     EPLOylaGSukMXYG21733966 = EPLOylaGSukMXYG47746915;     EPLOylaGSukMXYG47746915 = EPLOylaGSukMXYG51789703;     EPLOylaGSukMXYG51789703 = EPLOylaGSukMXYG66208253;     EPLOylaGSukMXYG66208253 = EPLOylaGSukMXYG32816376;     EPLOylaGSukMXYG32816376 = EPLOylaGSukMXYG7721204;     EPLOylaGSukMXYG7721204 = EPLOylaGSukMXYG32168308;     EPLOylaGSukMXYG32168308 = EPLOylaGSukMXYG81547293;     EPLOylaGSukMXYG81547293 = EPLOylaGSukMXYG29005580;     EPLOylaGSukMXYG29005580 = EPLOylaGSukMXYG90876639;     EPLOylaGSukMXYG90876639 = EPLOylaGSukMXYG75559396;     EPLOylaGSukMXYG75559396 = EPLOylaGSukMXYG11410853;     EPLOylaGSukMXYG11410853 = EPLOylaGSukMXYG9958169;     EPLOylaGSukMXYG9958169 = EPLOylaGSukMXYG42124781;     EPLOylaGSukMXYG42124781 = EPLOylaGSukMXYG72271867;     EPLOylaGSukMXYG72271867 = EPLOylaGSukMXYG15598273;     EPLOylaGSukMXYG15598273 = EPLOylaGSukMXYG87986951;     EPLOylaGSukMXYG87986951 = EPLOylaGSukMXYG272794;     EPLOylaGSukMXYG272794 = EPLOylaGSukMXYG47131437;     EPLOylaGSukMXYG47131437 = EPLOylaGSukMXYG40336215;     EPLOylaGSukMXYG40336215 = EPLOylaGSukMXYG40164083;     EPLOylaGSukMXYG40164083 = EPLOylaGSukMXYG35804072;     EPLOylaGSukMXYG35804072 = EPLOylaGSukMXYG11899190;     EPLOylaGSukMXYG11899190 = EPLOylaGSukMXYG96601494;     EPLOylaGSukMXYG96601494 = EPLOylaGSukMXYG77831514;     EPLOylaGSukMXYG77831514 = EPLOylaGSukMXYG31035840;     EPLOylaGSukMXYG31035840 = EPLOylaGSukMXYG29400288;     EPLOylaGSukMXYG29400288 = EPLOylaGSukMXYG19417973;     EPLOylaGSukMXYG19417973 = EPLOylaGSukMXYG84196105;     EPLOylaGSukMXYG84196105 = EPLOylaGSukMXYG56349364;     EPLOylaGSukMXYG56349364 = EPLOylaGSukMXYG89512843;     EPLOylaGSukMXYG89512843 = EPLOylaGSukMXYG9778341;     EPLOylaGSukMXYG9778341 = EPLOylaGSukMXYG96617280;     EPLOylaGSukMXYG96617280 = EPLOylaGSukMXYG95713415;     EPLOylaGSukMXYG95713415 = EPLOylaGSukMXYG67149378;     EPLOylaGSukMXYG67149378 = EPLOylaGSukMXYG13151451;     EPLOylaGSukMXYG13151451 = EPLOylaGSukMXYG51115789;     EPLOylaGSukMXYG51115789 = EPLOylaGSukMXYG17592538;     EPLOylaGSukMXYG17592538 = EPLOylaGSukMXYG20616408;     EPLOylaGSukMXYG20616408 = EPLOylaGSukMXYG88830025;     EPLOylaGSukMXYG88830025 = EPLOylaGSukMXYG7088851;     EPLOylaGSukMXYG7088851 = EPLOylaGSukMXYG8805990;     EPLOylaGSukMXYG8805990 = EPLOylaGSukMXYG77384452;     EPLOylaGSukMXYG77384452 = EPLOylaGSukMXYG33363572;     EPLOylaGSukMXYG33363572 = EPLOylaGSukMXYG99103342;     EPLOylaGSukMXYG99103342 = EPLOylaGSukMXYG9380605;     EPLOylaGSukMXYG9380605 = EPLOylaGSukMXYG18522593;     EPLOylaGSukMXYG18522593 = EPLOylaGSukMXYG30914000;     EPLOylaGSukMXYG30914000 = EPLOylaGSukMXYG98963088;     EPLOylaGSukMXYG98963088 = EPLOylaGSukMXYG69717411;     EPLOylaGSukMXYG69717411 = EPLOylaGSukMXYG40223728;     EPLOylaGSukMXYG40223728 = EPLOylaGSukMXYG5556316;     EPLOylaGSukMXYG5556316 = EPLOylaGSukMXYG51059082;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void gYzqrWNlosyznhGD77237448() {     float tGqNAfRvMYADduD21710749 = -617744521;    float tGqNAfRvMYADduD82239140 = -297907132;    float tGqNAfRvMYADduD66187511 = -717143056;    float tGqNAfRvMYADduD63346335 = -244909551;    float tGqNAfRvMYADduD21972504 = -389081380;    float tGqNAfRvMYADduD1758285 = -94510851;    float tGqNAfRvMYADduD49387021 = -26631588;    float tGqNAfRvMYADduD90012613 = -227231203;    float tGqNAfRvMYADduD64368819 = -231817248;    float tGqNAfRvMYADduD94754202 = -60841665;    float tGqNAfRvMYADduD95954737 = 35961130;    float tGqNAfRvMYADduD89229237 = -516545440;    float tGqNAfRvMYADduD59420760 = -894535294;    float tGqNAfRvMYADduD14364399 = -77436603;    float tGqNAfRvMYADduD24914123 = -987020733;    float tGqNAfRvMYADduD68668042 = -413291294;    float tGqNAfRvMYADduD65117060 = -84455708;    float tGqNAfRvMYADduD18916465 = -338841615;    float tGqNAfRvMYADduD41224383 = -997880627;    float tGqNAfRvMYADduD61225777 = -768027756;    float tGqNAfRvMYADduD45788682 = -32458901;    float tGqNAfRvMYADduD30907554 = -689969777;    float tGqNAfRvMYADduD86503852 = -102122132;    float tGqNAfRvMYADduD29191489 = -247461876;    float tGqNAfRvMYADduD11980041 = -33288892;    float tGqNAfRvMYADduD19708911 = -234392638;    float tGqNAfRvMYADduD10061139 = -322698107;    float tGqNAfRvMYADduD96665569 = -752163765;    float tGqNAfRvMYADduD93040853 = -56712490;    float tGqNAfRvMYADduD42077588 = -475045914;    float tGqNAfRvMYADduD90872787 = -523429959;    float tGqNAfRvMYADduD97944244 = -69031520;    float tGqNAfRvMYADduD79496590 = -769579603;    float tGqNAfRvMYADduD4622133 = -740007880;    float tGqNAfRvMYADduD68377518 = -639789639;    float tGqNAfRvMYADduD66383291 = -590172619;    float tGqNAfRvMYADduD68031697 = -860897047;    float tGqNAfRvMYADduD27455228 = -990698783;    float tGqNAfRvMYADduD99324745 = -105387644;    float tGqNAfRvMYADduD23584060 = -562404983;    float tGqNAfRvMYADduD33112065 = 37354914;    float tGqNAfRvMYADduD99717946 = -562194468;    float tGqNAfRvMYADduD34282643 = -871153853;    float tGqNAfRvMYADduD34718980 = -34251303;    float tGqNAfRvMYADduD1783168 = -82643573;    float tGqNAfRvMYADduD44483146 = -706326520;    float tGqNAfRvMYADduD18625252 = -764072279;    float tGqNAfRvMYADduD71701148 = 14414864;    float tGqNAfRvMYADduD27175418 = 54693414;    float tGqNAfRvMYADduD56174401 = -135952657;    float tGqNAfRvMYADduD19979028 = 68752459;    float tGqNAfRvMYADduD8079758 = -530950335;    float tGqNAfRvMYADduD87892032 = -870213837;    float tGqNAfRvMYADduD97948508 = -892759040;    float tGqNAfRvMYADduD31448766 = -826041687;    float tGqNAfRvMYADduD90803195 = -927774744;    float tGqNAfRvMYADduD95735287 = -95785001;    float tGqNAfRvMYADduD36996022 = -369681180;    float tGqNAfRvMYADduD51366294 = -111620659;    float tGqNAfRvMYADduD2263593 = -54688742;    float tGqNAfRvMYADduD91697146 = -771812745;    float tGqNAfRvMYADduD52721451 = -274467823;    float tGqNAfRvMYADduD96971760 = -70518714;    float tGqNAfRvMYADduD22291231 = -756771335;    float tGqNAfRvMYADduD3881416 = -537411706;    float tGqNAfRvMYADduD98010493 = -895007351;    float tGqNAfRvMYADduD9732648 = -746965838;    float tGqNAfRvMYADduD54798627 = -54527414;    float tGqNAfRvMYADduD45986880 = -437646965;    float tGqNAfRvMYADduD58530832 = -296848115;    float tGqNAfRvMYADduD636345 = -552394247;    float tGqNAfRvMYADduD37661832 = -93756925;    float tGqNAfRvMYADduD19591720 = -133453971;    float tGqNAfRvMYADduD17640324 = -335475645;    float tGqNAfRvMYADduD28113713 = -705382671;    float tGqNAfRvMYADduD46070736 = -470264433;    float tGqNAfRvMYADduD96624910 = -818815925;    float tGqNAfRvMYADduD51784873 = 32129170;    float tGqNAfRvMYADduD27408321 = -64818303;    float tGqNAfRvMYADduD67496894 = -326962373;    float tGqNAfRvMYADduD1083660 = -470320360;    float tGqNAfRvMYADduD38359991 = -237112972;    float tGqNAfRvMYADduD69490151 = -706857179;    float tGqNAfRvMYADduD36866453 = -920759833;    float tGqNAfRvMYADduD22098561 = -443798373;    float tGqNAfRvMYADduD82793029 = -992479625;    float tGqNAfRvMYADduD10052212 = -198817683;    float tGqNAfRvMYADduD81548081 = -876820564;    float tGqNAfRvMYADduD73173366 = -913966194;    float tGqNAfRvMYADduD77574323 = -712014896;    float tGqNAfRvMYADduD70648004 = -394387618;    float tGqNAfRvMYADduD31035675 = -391215867;    float tGqNAfRvMYADduD76088933 = -779078125;    float tGqNAfRvMYADduD97061152 = 49301098;    float tGqNAfRvMYADduD31886914 = -790592239;    float tGqNAfRvMYADduD80390614 = -688177263;    float tGqNAfRvMYADduD2746187 = -391675755;    float tGqNAfRvMYADduD11991413 = -14382519;    float tGqNAfRvMYADduD30837564 = -496839597;    float tGqNAfRvMYADduD3772674 = -617744521;     tGqNAfRvMYADduD21710749 = tGqNAfRvMYADduD82239140;     tGqNAfRvMYADduD82239140 = tGqNAfRvMYADduD66187511;     tGqNAfRvMYADduD66187511 = tGqNAfRvMYADduD63346335;     tGqNAfRvMYADduD63346335 = tGqNAfRvMYADduD21972504;     tGqNAfRvMYADduD21972504 = tGqNAfRvMYADduD1758285;     tGqNAfRvMYADduD1758285 = tGqNAfRvMYADduD49387021;     tGqNAfRvMYADduD49387021 = tGqNAfRvMYADduD90012613;     tGqNAfRvMYADduD90012613 = tGqNAfRvMYADduD64368819;     tGqNAfRvMYADduD64368819 = tGqNAfRvMYADduD94754202;     tGqNAfRvMYADduD94754202 = tGqNAfRvMYADduD95954737;     tGqNAfRvMYADduD95954737 = tGqNAfRvMYADduD89229237;     tGqNAfRvMYADduD89229237 = tGqNAfRvMYADduD59420760;     tGqNAfRvMYADduD59420760 = tGqNAfRvMYADduD14364399;     tGqNAfRvMYADduD14364399 = tGqNAfRvMYADduD24914123;     tGqNAfRvMYADduD24914123 = tGqNAfRvMYADduD68668042;     tGqNAfRvMYADduD68668042 = tGqNAfRvMYADduD65117060;     tGqNAfRvMYADduD65117060 = tGqNAfRvMYADduD18916465;     tGqNAfRvMYADduD18916465 = tGqNAfRvMYADduD41224383;     tGqNAfRvMYADduD41224383 = tGqNAfRvMYADduD61225777;     tGqNAfRvMYADduD61225777 = tGqNAfRvMYADduD45788682;     tGqNAfRvMYADduD45788682 = tGqNAfRvMYADduD30907554;     tGqNAfRvMYADduD30907554 = tGqNAfRvMYADduD86503852;     tGqNAfRvMYADduD86503852 = tGqNAfRvMYADduD29191489;     tGqNAfRvMYADduD29191489 = tGqNAfRvMYADduD11980041;     tGqNAfRvMYADduD11980041 = tGqNAfRvMYADduD19708911;     tGqNAfRvMYADduD19708911 = tGqNAfRvMYADduD10061139;     tGqNAfRvMYADduD10061139 = tGqNAfRvMYADduD96665569;     tGqNAfRvMYADduD96665569 = tGqNAfRvMYADduD93040853;     tGqNAfRvMYADduD93040853 = tGqNAfRvMYADduD42077588;     tGqNAfRvMYADduD42077588 = tGqNAfRvMYADduD90872787;     tGqNAfRvMYADduD90872787 = tGqNAfRvMYADduD97944244;     tGqNAfRvMYADduD97944244 = tGqNAfRvMYADduD79496590;     tGqNAfRvMYADduD79496590 = tGqNAfRvMYADduD4622133;     tGqNAfRvMYADduD4622133 = tGqNAfRvMYADduD68377518;     tGqNAfRvMYADduD68377518 = tGqNAfRvMYADduD66383291;     tGqNAfRvMYADduD66383291 = tGqNAfRvMYADduD68031697;     tGqNAfRvMYADduD68031697 = tGqNAfRvMYADduD27455228;     tGqNAfRvMYADduD27455228 = tGqNAfRvMYADduD99324745;     tGqNAfRvMYADduD99324745 = tGqNAfRvMYADduD23584060;     tGqNAfRvMYADduD23584060 = tGqNAfRvMYADduD33112065;     tGqNAfRvMYADduD33112065 = tGqNAfRvMYADduD99717946;     tGqNAfRvMYADduD99717946 = tGqNAfRvMYADduD34282643;     tGqNAfRvMYADduD34282643 = tGqNAfRvMYADduD34718980;     tGqNAfRvMYADduD34718980 = tGqNAfRvMYADduD1783168;     tGqNAfRvMYADduD1783168 = tGqNAfRvMYADduD44483146;     tGqNAfRvMYADduD44483146 = tGqNAfRvMYADduD18625252;     tGqNAfRvMYADduD18625252 = tGqNAfRvMYADduD71701148;     tGqNAfRvMYADduD71701148 = tGqNAfRvMYADduD27175418;     tGqNAfRvMYADduD27175418 = tGqNAfRvMYADduD56174401;     tGqNAfRvMYADduD56174401 = tGqNAfRvMYADduD19979028;     tGqNAfRvMYADduD19979028 = tGqNAfRvMYADduD8079758;     tGqNAfRvMYADduD8079758 = tGqNAfRvMYADduD87892032;     tGqNAfRvMYADduD87892032 = tGqNAfRvMYADduD97948508;     tGqNAfRvMYADduD97948508 = tGqNAfRvMYADduD31448766;     tGqNAfRvMYADduD31448766 = tGqNAfRvMYADduD90803195;     tGqNAfRvMYADduD90803195 = tGqNAfRvMYADduD95735287;     tGqNAfRvMYADduD95735287 = tGqNAfRvMYADduD36996022;     tGqNAfRvMYADduD36996022 = tGqNAfRvMYADduD51366294;     tGqNAfRvMYADduD51366294 = tGqNAfRvMYADduD2263593;     tGqNAfRvMYADduD2263593 = tGqNAfRvMYADduD91697146;     tGqNAfRvMYADduD91697146 = tGqNAfRvMYADduD52721451;     tGqNAfRvMYADduD52721451 = tGqNAfRvMYADduD96971760;     tGqNAfRvMYADduD96971760 = tGqNAfRvMYADduD22291231;     tGqNAfRvMYADduD22291231 = tGqNAfRvMYADduD3881416;     tGqNAfRvMYADduD3881416 = tGqNAfRvMYADduD98010493;     tGqNAfRvMYADduD98010493 = tGqNAfRvMYADduD9732648;     tGqNAfRvMYADduD9732648 = tGqNAfRvMYADduD54798627;     tGqNAfRvMYADduD54798627 = tGqNAfRvMYADduD45986880;     tGqNAfRvMYADduD45986880 = tGqNAfRvMYADduD58530832;     tGqNAfRvMYADduD58530832 = tGqNAfRvMYADduD636345;     tGqNAfRvMYADduD636345 = tGqNAfRvMYADduD37661832;     tGqNAfRvMYADduD37661832 = tGqNAfRvMYADduD19591720;     tGqNAfRvMYADduD19591720 = tGqNAfRvMYADduD17640324;     tGqNAfRvMYADduD17640324 = tGqNAfRvMYADduD28113713;     tGqNAfRvMYADduD28113713 = tGqNAfRvMYADduD46070736;     tGqNAfRvMYADduD46070736 = tGqNAfRvMYADduD96624910;     tGqNAfRvMYADduD96624910 = tGqNAfRvMYADduD51784873;     tGqNAfRvMYADduD51784873 = tGqNAfRvMYADduD27408321;     tGqNAfRvMYADduD27408321 = tGqNAfRvMYADduD67496894;     tGqNAfRvMYADduD67496894 = tGqNAfRvMYADduD1083660;     tGqNAfRvMYADduD1083660 = tGqNAfRvMYADduD38359991;     tGqNAfRvMYADduD38359991 = tGqNAfRvMYADduD69490151;     tGqNAfRvMYADduD69490151 = tGqNAfRvMYADduD36866453;     tGqNAfRvMYADduD36866453 = tGqNAfRvMYADduD22098561;     tGqNAfRvMYADduD22098561 = tGqNAfRvMYADduD82793029;     tGqNAfRvMYADduD82793029 = tGqNAfRvMYADduD10052212;     tGqNAfRvMYADduD10052212 = tGqNAfRvMYADduD81548081;     tGqNAfRvMYADduD81548081 = tGqNAfRvMYADduD73173366;     tGqNAfRvMYADduD73173366 = tGqNAfRvMYADduD77574323;     tGqNAfRvMYADduD77574323 = tGqNAfRvMYADduD70648004;     tGqNAfRvMYADduD70648004 = tGqNAfRvMYADduD31035675;     tGqNAfRvMYADduD31035675 = tGqNAfRvMYADduD76088933;     tGqNAfRvMYADduD76088933 = tGqNAfRvMYADduD97061152;     tGqNAfRvMYADduD97061152 = tGqNAfRvMYADduD31886914;     tGqNAfRvMYADduD31886914 = tGqNAfRvMYADduD80390614;     tGqNAfRvMYADduD80390614 = tGqNAfRvMYADduD2746187;     tGqNAfRvMYADduD2746187 = tGqNAfRvMYADduD11991413;     tGqNAfRvMYADduD11991413 = tGqNAfRvMYADduD30837564;     tGqNAfRvMYADduD30837564 = tGqNAfRvMYADduD3772674;     tGqNAfRvMYADduD3772674 = tGqNAfRvMYADduD21710749;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void HSreOJnwElsBFHvR53249568() {     float HtVqzLynHGAtfNh8632427 = -758395724;    float HtVqzLynHGAtfNh91886879 = -408493699;    float HtVqzLynHGAtfNh58883525 = -148258438;    float HtVqzLynHGAtfNh91209529 = -250706844;    float HtVqzLynHGAtfNh72701590 = -242894849;    float HtVqzLynHGAtfNh15223962 = -492204145;    float HtVqzLynHGAtfNh99432689 = -808429627;    float HtVqzLynHGAtfNh72728962 = -757771009;    float HtVqzLynHGAtfNh42754509 = -441327949;    float HtVqzLynHGAtfNh68254589 = -875702871;    float HtVqzLynHGAtfNh15635468 = -118127613;    float HtVqzLynHGAtfNh58855715 = -638648315;    float HtVqzLynHGAtfNh20047210 = -982898583;    float HtVqzLynHGAtfNh57682508 = -973935001;    float HtVqzLynHGAtfNh48626993 = -73786264;    float HtVqzLynHGAtfNh3947201 = -391942880;    float HtVqzLynHGAtfNh14957235 = -754375545;    float HtVqzLynHGAtfNh48133223 = -14227855;    float HtVqzLynHGAtfNh2383954 = -334401599;    float HtVqzLynHGAtfNh17978605 = -106125595;    float HtVqzLynHGAtfNh13254351 = -77137889;    float HtVqzLynHGAtfNh69597513 = -518091680;    float HtVqzLynHGAtfNh56150192 = 471618;    float HtVqzLynHGAtfNh5548440 = -181550652;    float HtVqzLynHGAtfNh67945025 = -57556659;    float HtVqzLynHGAtfNh13887811 = -897915988;    float HtVqzLynHGAtfNh66951566 = -574800172;    float HtVqzLynHGAtfNh97252119 = -979813026;    float HtVqzLynHGAtfNh65943646 = -472945137;    float HtVqzLynHGAtfNh21791227 = -36265984;    float HtVqzLynHGAtfNh49993039 = -439858859;    float HtVqzLynHGAtfNh2386275 = -239953225;    float HtVqzLynHGAtfNh33769673 = -253271339;    float HtVqzLynHGAtfNh34545231 = -876122649;    float HtVqzLynHGAtfNh61156467 = 88240261;    float HtVqzLynHGAtfNh99343505 = -770768454;    float HtVqzLynHGAtfNh24000961 = -828802336;    float HtVqzLynHGAtfNh99783604 = -870931350;    float HtVqzLynHGAtfNh52555912 = -468238828;    float HtVqzLynHGAtfNh51371135 = -842798152;    float HtVqzLynHGAtfNh16273496 = -784567736;    float HtVqzLynHGAtfNh84558872 = -919349971;    float HtVqzLynHGAtfNh10732818 = -664633907;    float HtVqzLynHGAtfNh53263659 = -84246985;    float HtVqzLynHGAtfNh53768028 = -538535292;    float HtVqzLynHGAtfNh47050068 = -661676672;    float HtVqzLynHGAtfNh39275279 = -239776793;    float HtVqzLynHGAtfNh73957091 = -184354890;    float HtVqzLynHGAtfNh17401047 = -895050290;    float HtVqzLynHGAtfNh48773981 = -473970981;    float HtVqzLynHGAtfNh61336890 = 9397732;    float HtVqzLynHGAtfNh12992800 = -30263023;    float HtVqzLynHGAtfNh73033636 = -678133519;    float HtVqzLynHGAtfNh52493478 = -408851200;    float HtVqzLynHGAtfNh39995218 = -377154939;    float HtVqzLynHGAtfNh39034913 = -140304044;    float HtVqzLynHGAtfNh35736687 = -308965318;    float HtVqzLynHGAtfNh53335086 = -966707787;    float HtVqzLynHGAtfNh23264504 = -93150185;    float HtVqzLynHGAtfNh58813780 = -344978862;    float HtVqzLynHGAtfNh48272395 = -917403974;    float HtVqzLynHGAtfNh2180570 = -828616601;    float HtVqzLynHGAtfNh6785316 = -184825873;    float HtVqzLynHGAtfNh20963283 = -305061966;    float HtVqzLynHGAtfNh18261550 = -335844013;    float HtVqzLynHGAtfNh13249194 = -878174388;    float HtVqzLynHGAtfNh25086043 = -285376977;    float HtVqzLynHGAtfNh85501978 = -6775934;    float HtVqzLynHGAtfNh96526040 = -962175263;    float HtVqzLynHGAtfNh49283487 = -303017811;    float HtVqzLynHGAtfNh79946239 = -563140544;    float HtVqzLynHGAtfNh15173631 = -883444195;    float HtVqzLynHGAtfNh95577310 = -545989027;    float HtVqzLynHGAtfNh51012819 = -491603447;    float HtVqzLynHGAtfNh1705110 = -321557860;    float HtVqzLynHGAtfNh28695479 = -157787918;    float HtVqzLynHGAtfNh58864696 = -853457773;    float HtVqzLynHGAtfNh2886534 = -915281397;    float HtVqzLynHGAtfNh51780411 = -643015360;    float HtVqzLynHGAtfNh20894958 = -395879988;    float HtVqzLynHGAtfNh74612531 = -558139195;    float HtVqzLynHGAtfNh92994475 = -290445282;    float HtVqzLynHGAtfNh79851073 = 15237263;    float HtVqzLynHGAtfNh17169666 = -998974156;    float HtVqzLynHGAtfNh60454337 = 54336284;    float HtVqzLynHGAtfNh37000240 = -309595836;    float HtVqzLynHGAtfNh29352638 = -561819707;    float HtVqzLynHGAtfNh81276195 = -844420140;    float HtVqzLynHGAtfNh94550013 = -398967710;    float HtVqzLynHGAtfNh22121554 = -771455695;    float HtVqzLynHGAtfNh63606819 = -361803136;    float HtVqzLynHGAtfNh70665875 = -862094549;    float HtVqzLynHGAtfNh76519101 = -677781165;    float HtVqzLynHGAtfNh93742131 = -23259966;    float HtVqzLynHGAtfNh3098740 = -925394179;    float HtVqzLynHGAtfNh14092927 = -955951135;    float HtVqzLynHGAtfNh77773556 = -634524099;    float HtVqzLynHGAtfNh89769534 = -259571942;    float HtVqzLynHGAtfNh35002110 = -748402973;    float HtVqzLynHGAtfNh40518834 = -758395724;     HtVqzLynHGAtfNh8632427 = HtVqzLynHGAtfNh91886879;     HtVqzLynHGAtfNh91886879 = HtVqzLynHGAtfNh58883525;     HtVqzLynHGAtfNh58883525 = HtVqzLynHGAtfNh91209529;     HtVqzLynHGAtfNh91209529 = HtVqzLynHGAtfNh72701590;     HtVqzLynHGAtfNh72701590 = HtVqzLynHGAtfNh15223962;     HtVqzLynHGAtfNh15223962 = HtVqzLynHGAtfNh99432689;     HtVqzLynHGAtfNh99432689 = HtVqzLynHGAtfNh72728962;     HtVqzLynHGAtfNh72728962 = HtVqzLynHGAtfNh42754509;     HtVqzLynHGAtfNh42754509 = HtVqzLynHGAtfNh68254589;     HtVqzLynHGAtfNh68254589 = HtVqzLynHGAtfNh15635468;     HtVqzLynHGAtfNh15635468 = HtVqzLynHGAtfNh58855715;     HtVqzLynHGAtfNh58855715 = HtVqzLynHGAtfNh20047210;     HtVqzLynHGAtfNh20047210 = HtVqzLynHGAtfNh57682508;     HtVqzLynHGAtfNh57682508 = HtVqzLynHGAtfNh48626993;     HtVqzLynHGAtfNh48626993 = HtVqzLynHGAtfNh3947201;     HtVqzLynHGAtfNh3947201 = HtVqzLynHGAtfNh14957235;     HtVqzLynHGAtfNh14957235 = HtVqzLynHGAtfNh48133223;     HtVqzLynHGAtfNh48133223 = HtVqzLynHGAtfNh2383954;     HtVqzLynHGAtfNh2383954 = HtVqzLynHGAtfNh17978605;     HtVqzLynHGAtfNh17978605 = HtVqzLynHGAtfNh13254351;     HtVqzLynHGAtfNh13254351 = HtVqzLynHGAtfNh69597513;     HtVqzLynHGAtfNh69597513 = HtVqzLynHGAtfNh56150192;     HtVqzLynHGAtfNh56150192 = HtVqzLynHGAtfNh5548440;     HtVqzLynHGAtfNh5548440 = HtVqzLynHGAtfNh67945025;     HtVqzLynHGAtfNh67945025 = HtVqzLynHGAtfNh13887811;     HtVqzLynHGAtfNh13887811 = HtVqzLynHGAtfNh66951566;     HtVqzLynHGAtfNh66951566 = HtVqzLynHGAtfNh97252119;     HtVqzLynHGAtfNh97252119 = HtVqzLynHGAtfNh65943646;     HtVqzLynHGAtfNh65943646 = HtVqzLynHGAtfNh21791227;     HtVqzLynHGAtfNh21791227 = HtVqzLynHGAtfNh49993039;     HtVqzLynHGAtfNh49993039 = HtVqzLynHGAtfNh2386275;     HtVqzLynHGAtfNh2386275 = HtVqzLynHGAtfNh33769673;     HtVqzLynHGAtfNh33769673 = HtVqzLynHGAtfNh34545231;     HtVqzLynHGAtfNh34545231 = HtVqzLynHGAtfNh61156467;     HtVqzLynHGAtfNh61156467 = HtVqzLynHGAtfNh99343505;     HtVqzLynHGAtfNh99343505 = HtVqzLynHGAtfNh24000961;     HtVqzLynHGAtfNh24000961 = HtVqzLynHGAtfNh99783604;     HtVqzLynHGAtfNh99783604 = HtVqzLynHGAtfNh52555912;     HtVqzLynHGAtfNh52555912 = HtVqzLynHGAtfNh51371135;     HtVqzLynHGAtfNh51371135 = HtVqzLynHGAtfNh16273496;     HtVqzLynHGAtfNh16273496 = HtVqzLynHGAtfNh84558872;     HtVqzLynHGAtfNh84558872 = HtVqzLynHGAtfNh10732818;     HtVqzLynHGAtfNh10732818 = HtVqzLynHGAtfNh53263659;     HtVqzLynHGAtfNh53263659 = HtVqzLynHGAtfNh53768028;     HtVqzLynHGAtfNh53768028 = HtVqzLynHGAtfNh47050068;     HtVqzLynHGAtfNh47050068 = HtVqzLynHGAtfNh39275279;     HtVqzLynHGAtfNh39275279 = HtVqzLynHGAtfNh73957091;     HtVqzLynHGAtfNh73957091 = HtVqzLynHGAtfNh17401047;     HtVqzLynHGAtfNh17401047 = HtVqzLynHGAtfNh48773981;     HtVqzLynHGAtfNh48773981 = HtVqzLynHGAtfNh61336890;     HtVqzLynHGAtfNh61336890 = HtVqzLynHGAtfNh12992800;     HtVqzLynHGAtfNh12992800 = HtVqzLynHGAtfNh73033636;     HtVqzLynHGAtfNh73033636 = HtVqzLynHGAtfNh52493478;     HtVqzLynHGAtfNh52493478 = HtVqzLynHGAtfNh39995218;     HtVqzLynHGAtfNh39995218 = HtVqzLynHGAtfNh39034913;     HtVqzLynHGAtfNh39034913 = HtVqzLynHGAtfNh35736687;     HtVqzLynHGAtfNh35736687 = HtVqzLynHGAtfNh53335086;     HtVqzLynHGAtfNh53335086 = HtVqzLynHGAtfNh23264504;     HtVqzLynHGAtfNh23264504 = HtVqzLynHGAtfNh58813780;     HtVqzLynHGAtfNh58813780 = HtVqzLynHGAtfNh48272395;     HtVqzLynHGAtfNh48272395 = HtVqzLynHGAtfNh2180570;     HtVqzLynHGAtfNh2180570 = HtVqzLynHGAtfNh6785316;     HtVqzLynHGAtfNh6785316 = HtVqzLynHGAtfNh20963283;     HtVqzLynHGAtfNh20963283 = HtVqzLynHGAtfNh18261550;     HtVqzLynHGAtfNh18261550 = HtVqzLynHGAtfNh13249194;     HtVqzLynHGAtfNh13249194 = HtVqzLynHGAtfNh25086043;     HtVqzLynHGAtfNh25086043 = HtVqzLynHGAtfNh85501978;     HtVqzLynHGAtfNh85501978 = HtVqzLynHGAtfNh96526040;     HtVqzLynHGAtfNh96526040 = HtVqzLynHGAtfNh49283487;     HtVqzLynHGAtfNh49283487 = HtVqzLynHGAtfNh79946239;     HtVqzLynHGAtfNh79946239 = HtVqzLynHGAtfNh15173631;     HtVqzLynHGAtfNh15173631 = HtVqzLynHGAtfNh95577310;     HtVqzLynHGAtfNh95577310 = HtVqzLynHGAtfNh51012819;     HtVqzLynHGAtfNh51012819 = HtVqzLynHGAtfNh1705110;     HtVqzLynHGAtfNh1705110 = HtVqzLynHGAtfNh28695479;     HtVqzLynHGAtfNh28695479 = HtVqzLynHGAtfNh58864696;     HtVqzLynHGAtfNh58864696 = HtVqzLynHGAtfNh2886534;     HtVqzLynHGAtfNh2886534 = HtVqzLynHGAtfNh51780411;     HtVqzLynHGAtfNh51780411 = HtVqzLynHGAtfNh20894958;     HtVqzLynHGAtfNh20894958 = HtVqzLynHGAtfNh74612531;     HtVqzLynHGAtfNh74612531 = HtVqzLynHGAtfNh92994475;     HtVqzLynHGAtfNh92994475 = HtVqzLynHGAtfNh79851073;     HtVqzLynHGAtfNh79851073 = HtVqzLynHGAtfNh17169666;     HtVqzLynHGAtfNh17169666 = HtVqzLynHGAtfNh60454337;     HtVqzLynHGAtfNh60454337 = HtVqzLynHGAtfNh37000240;     HtVqzLynHGAtfNh37000240 = HtVqzLynHGAtfNh29352638;     HtVqzLynHGAtfNh29352638 = HtVqzLynHGAtfNh81276195;     HtVqzLynHGAtfNh81276195 = HtVqzLynHGAtfNh94550013;     HtVqzLynHGAtfNh94550013 = HtVqzLynHGAtfNh22121554;     HtVqzLynHGAtfNh22121554 = HtVqzLynHGAtfNh63606819;     HtVqzLynHGAtfNh63606819 = HtVqzLynHGAtfNh70665875;     HtVqzLynHGAtfNh70665875 = HtVqzLynHGAtfNh76519101;     HtVqzLynHGAtfNh76519101 = HtVqzLynHGAtfNh93742131;     HtVqzLynHGAtfNh93742131 = HtVqzLynHGAtfNh3098740;     HtVqzLynHGAtfNh3098740 = HtVqzLynHGAtfNh14092927;     HtVqzLynHGAtfNh14092927 = HtVqzLynHGAtfNh77773556;     HtVqzLynHGAtfNh77773556 = HtVqzLynHGAtfNh89769534;     HtVqzLynHGAtfNh89769534 = HtVqzLynHGAtfNh35002110;     HtVqzLynHGAtfNh35002110 = HtVqzLynHGAtfNh40518834;     HtVqzLynHGAtfNh40518834 = HtVqzLynHGAtfNh8632427;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void oFNBSiIUrrVwwgXZ90224740() {     float CSxQTvTKoQcbntI76358675 = -927796246;    float CSxQTvTKoQcbntI67809076 = -920067071;    float CSxQTvTKoQcbntI28233369 = 40343717;    float CSxQTvTKoQcbntI19861137 = -71552792;    float CSxQTvTKoQcbntI5264277 = -529871941;    float CSxQTvTKoQcbntI50738114 = 76905501;    float CSxQTvTKoQcbntI39035060 = -33060727;    float CSxQTvTKoQcbntI88401767 = -431976753;    float CSxQTvTKoQcbntI34883394 = 7683311;    float CSxQTvTKoQcbntI74025701 = 23550194;    float CSxQTvTKoQcbntI94886638 = -548867515;    float CSxQTvTKoQcbntI66285628 = -438447892;    float CSxQTvTKoQcbntI55090989 = -719734073;    float CSxQTvTKoQcbntI61175049 = -34693233;    float CSxQTvTKoQcbntI310306 = -856855216;    float CSxQTvTKoQcbntI33000125 = -376246509;    float CSxQTvTKoQcbntI20801583 = -377670106;    float CSxQTvTKoQcbntI9513585 = -799477704;    float CSxQTvTKoQcbntI62873254 = -801674442;    float CSxQTvTKoQcbntI50934096 = -625418354;    float CSxQTvTKoQcbntI59632494 = -311008425;    float CSxQTvTKoQcbntI60456409 = -266405769;    float CSxQTvTKoQcbntI59852081 = -47823395;    float CSxQTvTKoQcbntI74269966 = -927423215;    float CSxQTvTKoQcbntI36175346 = -832896197;    float CSxQTvTKoQcbntI77523935 = -146307351;    float CSxQTvTKoQcbntI82272644 = -78342099;    float CSxQTvTKoQcbntI85318094 = -269182614;    float CSxQTvTKoQcbntI45616977 = 16972320;    float CSxQTvTKoQcbntI12602868 = -441235256;    float CSxQTvTKoQcbntI75990994 = -359395720;    float CSxQTvTKoQcbntI13659713 = -872349810;    float CSxQTvTKoQcbntI32604890 = -428041433;    float CSxQTvTKoQcbntI99560862 = -296810422;    float CSxQTvTKoQcbntI85239713 = -16966823;    float CSxQTvTKoQcbntI96538022 = -917933961;    float CSxQTvTKoQcbntI54677553 = -810977045;    float CSxQTvTKoQcbntI10350146 = -859890483;    float CSxQTvTKoQcbntI85407991 = -100017661;    float CSxQTvTKoQcbntI19443348 = -930145268;    float CSxQTvTKoQcbntI76927133 = -163139142;    float CSxQTvTKoQcbntI22090266 = -263518637;    float CSxQTvTKoQcbntI85566186 = -170789425;    float CSxQTvTKoQcbntI2945278 = -164895412;    float CSxQTvTKoQcbntI45785283 = -688420573;    float CSxQTvTKoQcbntI40244716 = -587160805;    float CSxQTvTKoQcbntI62796756 = -172410285;    float CSxQTvTKoQcbntI70512038 = -571998864;    float CSxQTvTKoQcbntI88176574 = -508800636;    float CSxQTvTKoQcbntI21061487 = 37003163;    float CSxQTvTKoQcbntI8351937 = -267018050;    float CSxQTvTKoQcbntI12009165 = -328162066;    float CSxQTvTKoQcbntI95068383 = -502183303;    float CSxQTvTKoQcbntI65614536 = -220565375;    float CSxQTvTKoQcbntI30592246 = -978849646;    float CSxQTvTKoQcbntI15902267 = -561390478;    float CSxQTvTKoQcbntI7956996 = -772243677;    float CSxQTvTKoQcbntI53963403 = -32233068;    float CSxQTvTKoQcbntI83685791 = -238656595;    float CSxQTvTKoQcbntI27740341 = -283564590;    float CSxQTvTKoQcbntI68465469 = -844752401;    float CSxQTvTKoQcbntI53716965 = -763878114;    float CSxQTvTKoQcbntI42784791 = -348949074;    float CSxQTvTKoQcbntI22280527 = -551081433;    float CSxQTvTKoQcbntI98034707 = -617054086;    float CSxQTvTKoQcbntI81226926 = -676517705;    float CSxQTvTKoQcbntI33680738 = 89593541;    float CSxQTvTKoQcbntI55530126 = -322923651;    float CSxQTvTKoQcbntI75935336 = 82273590;    float CSxQTvTKoQcbntI3772283 = -938921256;    float CSxQTvTKoQcbntI78322572 = -565269464;    float CSxQTvTKoQcbntI10451437 = -517779623;    float CSxQTvTKoQcbntI24105594 = -599460043;    float CSxQTvTKoQcbntI43429907 = -871529174;    float CSxQTvTKoQcbntI74006962 = -362279212;    float CSxQTvTKoQcbntI37542228 = 52510211;    float CSxQTvTKoQcbntI74890222 = 4383656;    float CSxQTvTKoQcbntI56906804 = -882927984;    float CSxQTvTKoQcbntI28484683 = -139002643;    float CSxQTvTKoQcbntI95930630 = -145735393;    float CSxQTvTKoQcbntI14727179 = -973897067;    float CSxQTvTKoQcbntI11760607 = -506343235;    float CSxQTvTKoQcbntI97141520 = -760381978;    float CSxQTvTKoQcbntI24555491 = 79969157;    float CSxQTvTKoQcbntI4250931 = -74217206;    float CSxQTvTKoQcbntI63981830 = 68766346;    float CSxQTvTKoQcbntI18591329 = -270166507;    float CSxQTvTKoQcbntI66990353 = -107476058;    float CSxQTvTKoQcbntI68968617 = -317960777;    float CSxQTvTKoQcbntI69337447 = -455576346;    float CSxQTvTKoQcbntI88581027 = -45690285;    float CSxQTvTKoQcbntI714150 = -678743978;    float CSxQTvTKoQcbntI26664355 = -521233888;    float CSxQTvTKoQcbntI57667650 = -816453072;    float CSxQTvTKoQcbntI50977878 = 14607132;    float CSxQTvTKoQcbntI23210168 = -399261029;    float CSxQTvTKoQcbntI79305475 = -914569564;    float CSxQTvTKoQcbntI63285660 = -619707993;    float CSxQTvTKoQcbntI4910571 = -547841326;    float CSxQTvTKoQcbntI64558357 = -927796246;     CSxQTvTKoQcbntI76358675 = CSxQTvTKoQcbntI67809076;     CSxQTvTKoQcbntI67809076 = CSxQTvTKoQcbntI28233369;     CSxQTvTKoQcbntI28233369 = CSxQTvTKoQcbntI19861137;     CSxQTvTKoQcbntI19861137 = CSxQTvTKoQcbntI5264277;     CSxQTvTKoQcbntI5264277 = CSxQTvTKoQcbntI50738114;     CSxQTvTKoQcbntI50738114 = CSxQTvTKoQcbntI39035060;     CSxQTvTKoQcbntI39035060 = CSxQTvTKoQcbntI88401767;     CSxQTvTKoQcbntI88401767 = CSxQTvTKoQcbntI34883394;     CSxQTvTKoQcbntI34883394 = CSxQTvTKoQcbntI74025701;     CSxQTvTKoQcbntI74025701 = CSxQTvTKoQcbntI94886638;     CSxQTvTKoQcbntI94886638 = CSxQTvTKoQcbntI66285628;     CSxQTvTKoQcbntI66285628 = CSxQTvTKoQcbntI55090989;     CSxQTvTKoQcbntI55090989 = CSxQTvTKoQcbntI61175049;     CSxQTvTKoQcbntI61175049 = CSxQTvTKoQcbntI310306;     CSxQTvTKoQcbntI310306 = CSxQTvTKoQcbntI33000125;     CSxQTvTKoQcbntI33000125 = CSxQTvTKoQcbntI20801583;     CSxQTvTKoQcbntI20801583 = CSxQTvTKoQcbntI9513585;     CSxQTvTKoQcbntI9513585 = CSxQTvTKoQcbntI62873254;     CSxQTvTKoQcbntI62873254 = CSxQTvTKoQcbntI50934096;     CSxQTvTKoQcbntI50934096 = CSxQTvTKoQcbntI59632494;     CSxQTvTKoQcbntI59632494 = CSxQTvTKoQcbntI60456409;     CSxQTvTKoQcbntI60456409 = CSxQTvTKoQcbntI59852081;     CSxQTvTKoQcbntI59852081 = CSxQTvTKoQcbntI74269966;     CSxQTvTKoQcbntI74269966 = CSxQTvTKoQcbntI36175346;     CSxQTvTKoQcbntI36175346 = CSxQTvTKoQcbntI77523935;     CSxQTvTKoQcbntI77523935 = CSxQTvTKoQcbntI82272644;     CSxQTvTKoQcbntI82272644 = CSxQTvTKoQcbntI85318094;     CSxQTvTKoQcbntI85318094 = CSxQTvTKoQcbntI45616977;     CSxQTvTKoQcbntI45616977 = CSxQTvTKoQcbntI12602868;     CSxQTvTKoQcbntI12602868 = CSxQTvTKoQcbntI75990994;     CSxQTvTKoQcbntI75990994 = CSxQTvTKoQcbntI13659713;     CSxQTvTKoQcbntI13659713 = CSxQTvTKoQcbntI32604890;     CSxQTvTKoQcbntI32604890 = CSxQTvTKoQcbntI99560862;     CSxQTvTKoQcbntI99560862 = CSxQTvTKoQcbntI85239713;     CSxQTvTKoQcbntI85239713 = CSxQTvTKoQcbntI96538022;     CSxQTvTKoQcbntI96538022 = CSxQTvTKoQcbntI54677553;     CSxQTvTKoQcbntI54677553 = CSxQTvTKoQcbntI10350146;     CSxQTvTKoQcbntI10350146 = CSxQTvTKoQcbntI85407991;     CSxQTvTKoQcbntI85407991 = CSxQTvTKoQcbntI19443348;     CSxQTvTKoQcbntI19443348 = CSxQTvTKoQcbntI76927133;     CSxQTvTKoQcbntI76927133 = CSxQTvTKoQcbntI22090266;     CSxQTvTKoQcbntI22090266 = CSxQTvTKoQcbntI85566186;     CSxQTvTKoQcbntI85566186 = CSxQTvTKoQcbntI2945278;     CSxQTvTKoQcbntI2945278 = CSxQTvTKoQcbntI45785283;     CSxQTvTKoQcbntI45785283 = CSxQTvTKoQcbntI40244716;     CSxQTvTKoQcbntI40244716 = CSxQTvTKoQcbntI62796756;     CSxQTvTKoQcbntI62796756 = CSxQTvTKoQcbntI70512038;     CSxQTvTKoQcbntI70512038 = CSxQTvTKoQcbntI88176574;     CSxQTvTKoQcbntI88176574 = CSxQTvTKoQcbntI21061487;     CSxQTvTKoQcbntI21061487 = CSxQTvTKoQcbntI8351937;     CSxQTvTKoQcbntI8351937 = CSxQTvTKoQcbntI12009165;     CSxQTvTKoQcbntI12009165 = CSxQTvTKoQcbntI95068383;     CSxQTvTKoQcbntI95068383 = CSxQTvTKoQcbntI65614536;     CSxQTvTKoQcbntI65614536 = CSxQTvTKoQcbntI30592246;     CSxQTvTKoQcbntI30592246 = CSxQTvTKoQcbntI15902267;     CSxQTvTKoQcbntI15902267 = CSxQTvTKoQcbntI7956996;     CSxQTvTKoQcbntI7956996 = CSxQTvTKoQcbntI53963403;     CSxQTvTKoQcbntI53963403 = CSxQTvTKoQcbntI83685791;     CSxQTvTKoQcbntI83685791 = CSxQTvTKoQcbntI27740341;     CSxQTvTKoQcbntI27740341 = CSxQTvTKoQcbntI68465469;     CSxQTvTKoQcbntI68465469 = CSxQTvTKoQcbntI53716965;     CSxQTvTKoQcbntI53716965 = CSxQTvTKoQcbntI42784791;     CSxQTvTKoQcbntI42784791 = CSxQTvTKoQcbntI22280527;     CSxQTvTKoQcbntI22280527 = CSxQTvTKoQcbntI98034707;     CSxQTvTKoQcbntI98034707 = CSxQTvTKoQcbntI81226926;     CSxQTvTKoQcbntI81226926 = CSxQTvTKoQcbntI33680738;     CSxQTvTKoQcbntI33680738 = CSxQTvTKoQcbntI55530126;     CSxQTvTKoQcbntI55530126 = CSxQTvTKoQcbntI75935336;     CSxQTvTKoQcbntI75935336 = CSxQTvTKoQcbntI3772283;     CSxQTvTKoQcbntI3772283 = CSxQTvTKoQcbntI78322572;     CSxQTvTKoQcbntI78322572 = CSxQTvTKoQcbntI10451437;     CSxQTvTKoQcbntI10451437 = CSxQTvTKoQcbntI24105594;     CSxQTvTKoQcbntI24105594 = CSxQTvTKoQcbntI43429907;     CSxQTvTKoQcbntI43429907 = CSxQTvTKoQcbntI74006962;     CSxQTvTKoQcbntI74006962 = CSxQTvTKoQcbntI37542228;     CSxQTvTKoQcbntI37542228 = CSxQTvTKoQcbntI74890222;     CSxQTvTKoQcbntI74890222 = CSxQTvTKoQcbntI56906804;     CSxQTvTKoQcbntI56906804 = CSxQTvTKoQcbntI28484683;     CSxQTvTKoQcbntI28484683 = CSxQTvTKoQcbntI95930630;     CSxQTvTKoQcbntI95930630 = CSxQTvTKoQcbntI14727179;     CSxQTvTKoQcbntI14727179 = CSxQTvTKoQcbntI11760607;     CSxQTvTKoQcbntI11760607 = CSxQTvTKoQcbntI97141520;     CSxQTvTKoQcbntI97141520 = CSxQTvTKoQcbntI24555491;     CSxQTvTKoQcbntI24555491 = CSxQTvTKoQcbntI4250931;     CSxQTvTKoQcbntI4250931 = CSxQTvTKoQcbntI63981830;     CSxQTvTKoQcbntI63981830 = CSxQTvTKoQcbntI18591329;     CSxQTvTKoQcbntI18591329 = CSxQTvTKoQcbntI66990353;     CSxQTvTKoQcbntI66990353 = CSxQTvTKoQcbntI68968617;     CSxQTvTKoQcbntI68968617 = CSxQTvTKoQcbntI69337447;     CSxQTvTKoQcbntI69337447 = CSxQTvTKoQcbntI88581027;     CSxQTvTKoQcbntI88581027 = CSxQTvTKoQcbntI714150;     CSxQTvTKoQcbntI714150 = CSxQTvTKoQcbntI26664355;     CSxQTvTKoQcbntI26664355 = CSxQTvTKoQcbntI57667650;     CSxQTvTKoQcbntI57667650 = CSxQTvTKoQcbntI50977878;     CSxQTvTKoQcbntI50977878 = CSxQTvTKoQcbntI23210168;     CSxQTvTKoQcbntI23210168 = CSxQTvTKoQcbntI79305475;     CSxQTvTKoQcbntI79305475 = CSxQTvTKoQcbntI63285660;     CSxQTvTKoQcbntI63285660 = CSxQTvTKoQcbntI4910571;     CSxQTvTKoQcbntI4910571 = CSxQTvTKoQcbntI64558357;     CSxQTvTKoQcbntI64558357 = CSxQTvTKoQcbntI76358675;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void sOWbxmAOegPWhQOA44640954() {     float sNXkpoypOywtJkk47010342 = -445945374;    float sNXkpoypOywtJkk36482594 = -791710365;    float sNXkpoypOywtJkk8202289 = -401950743;    float sNXkpoypOywtJkk59792423 = -261691184;    float sNXkpoypOywtJkk26714598 = -313278280;    float sNXkpoypOywtJkk40737877 = -724675650;    float sNXkpoypOywtJkk57413956 = -31836437;    float sNXkpoypOywtJkk45244148 = -778793828;    float sNXkpoypOywtJkk80748448 = -85664016;    float sNXkpoypOywtJkk96992162 = -277545156;    float sNXkpoypOywtJkk37135799 = -583769443;    float sNXkpoypOywtJkk11832199 = -754211675;    float sNXkpoypOywtJkk34918376 = -629271131;    float sNXkpoypOywtJkk45022083 = -935721442;    float sNXkpoypOywtJkk83030324 = 93394799;    float sNXkpoypOywtJkk65528764 = 53769905;    float sNXkpoypOywtJkk14654410 = -286855236;    float sNXkpoypOywtJkk29807080 = -788643896;    float sNXkpoypOywtJkk44581035 = -408862385;    float sNXkpoypOywtJkk9720805 = -67784659;    float sNXkpoypOywtJkk14768249 = -219687560;    float sNXkpoypOywtJkk21852174 = -597691074;    float sNXkpoypOywtJkk61795889 = -615666540;    float sNXkpoypOywtJkk8119503 = -519824121;    float sNXkpoypOywtJkk299735 = -508800873;    float sNXkpoypOywtJkk8121516 = -244591800;    float sNXkpoypOywtJkk90533430 = -473519874;    float sNXkpoypOywtJkk93100320 = -774306375;    float sNXkpoypOywtJkk77759465 = -277385912;    float sNXkpoypOywtJkk51774963 = -999630326;    float sNXkpoypOywtJkk83062991 = -107829405;    float sNXkpoypOywtJkk58171173 = -100646976;    float sNXkpoypOywtJkk15550251 = -375003045;    float sNXkpoypOywtJkk64925837 = -960340106;    float sNXkpoypOywtJkk63263950 = -963913606;    float sNXkpoypOywtJkk56531282 = -997160528;    float sNXkpoypOywtJkk98469038 = -999570253;    float sNXkpoypOywtJkk63142634 = -644003587;    float sNXkpoypOywtJkk63941280 = -55746310;    float sNXkpoypOywtJkk14546646 = -447753631;    float sNXkpoypOywtJkk37000417 = -547158021;    float sNXkpoypOywtJkk50573257 = -32907727;    float sNXkpoypOywtJkk34533146 = -157543479;    float sNXkpoypOywtJkk9453578 = -236870383;    float sNXkpoypOywtJkk41739342 = -997066968;    float sNXkpoypOywtJkk46650551 = -345497979;    float sNXkpoypOywtJkk88927959 = -288480084;    float sNXkpoypOywtJkk36126248 = 75870840;    float sNXkpoypOywtJkk93618026 = -494564630;    float sNXkpoypOywtJkk29488974 = -246005711;    float sNXkpoypOywtJkk76541261 = -913590171;    float sNXkpoypOywtJkk53880669 = -297381792;    float sNXkpoypOywtJkk50144041 = -429981342;    float sNXkpoypOywtJkk55841841 = -881446872;    float sNXkpoypOywtJkk29872705 = 10209435;    float sNXkpoypOywtJkk25158169 = -848254301;    float sNXkpoypOywtJkk74686704 = -76043825;    float sNXkpoypOywtJkk82786 = -882126622;    float sNXkpoypOywtJkk59492689 = -752890312;    float sNXkpoypOywtJkk18593083 = 31313520;    float sNXkpoypOywtJkk50204446 = -151155777;    float sNXkpoypOywtJkk64313636 = -257530062;    float sNXkpoypOywtJkk67484683 = -401407916;    float sNXkpoypOywtJkk28973485 = -86033690;    float sNXkpoypOywtJkk13929172 = -69715751;    float sNXkpoypOywtJkk78964626 = -383122468;    float sNXkpoypOywtJkk96281948 = -279208631;    float sNXkpoypOywtJkk69992538 = -668931025;    float sNXkpoypOywtJkk81758133 = -971807836;    float sNXkpoypOywtJkk26499043 = 90555326;    float sNXkpoypOywtJkk67059726 = 53340157;    float sNXkpoypOywtJkk51511775 = -642851649;    float sNXkpoypOywtJkk65865799 = -632897586;    float sNXkpoypOywtJkk30034390 = -961108754;    float sNXkpoypOywtJkk72720388 = -520626639;    float sNXkpoypOywtJkk64194992 = -86779834;    float sNXkpoypOywtJkk87319027 = -340147595;    float sNXkpoypOywtJkk52342311 = -278796157;    float sNXkpoypOywtJkk66380161 = -522757153;    float sNXkpoypOywtJkk53649184 = -63302895;    float sNXkpoypOywtJkk19193556 = -956111717;    float sNXkpoypOywtJkk54407182 = -449390714;    float sNXkpoypOywtJkk99482293 = -179741746;    float sNXkpoypOywtJkk48270492 = 68619798;    float sNXkpoypOywtJkk75233702 = 13959844;    float sNXkpoypOywtJkk29182322 = -810447613;    float sNXkpoypOywtJkk8027132 = -670665634;    float sNXkpoypOywtJkk59708410 = -493556173;    float sNXkpoypOywtJkk35053133 = -870549542;    float sNXkpoypOywtJkk38105781 = -15659305;    float sNXkpoypOywtJkk81844577 = -821116703;    float sNXkpoypOywtJkk98386253 = -17443632;    float sNXkpoypOywtJkk3649946 = -891113276;    float sNXkpoypOywtJkk45348197 = 12940169;    float sNXkpoypOywtJkk64342199 = -196597854;    float sNXkpoypOywtJkk72686781 = -189627959;    float sNXkpoypOywtJkk83088574 = -631499812;    float sNXkpoypOywtJkk5559662 = 28490210;    float sNXkpoypOywtJkk95524406 = -67154632;    float sNXkpoypOywtJkk62774716 = -445945374;     sNXkpoypOywtJkk47010342 = sNXkpoypOywtJkk36482594;     sNXkpoypOywtJkk36482594 = sNXkpoypOywtJkk8202289;     sNXkpoypOywtJkk8202289 = sNXkpoypOywtJkk59792423;     sNXkpoypOywtJkk59792423 = sNXkpoypOywtJkk26714598;     sNXkpoypOywtJkk26714598 = sNXkpoypOywtJkk40737877;     sNXkpoypOywtJkk40737877 = sNXkpoypOywtJkk57413956;     sNXkpoypOywtJkk57413956 = sNXkpoypOywtJkk45244148;     sNXkpoypOywtJkk45244148 = sNXkpoypOywtJkk80748448;     sNXkpoypOywtJkk80748448 = sNXkpoypOywtJkk96992162;     sNXkpoypOywtJkk96992162 = sNXkpoypOywtJkk37135799;     sNXkpoypOywtJkk37135799 = sNXkpoypOywtJkk11832199;     sNXkpoypOywtJkk11832199 = sNXkpoypOywtJkk34918376;     sNXkpoypOywtJkk34918376 = sNXkpoypOywtJkk45022083;     sNXkpoypOywtJkk45022083 = sNXkpoypOywtJkk83030324;     sNXkpoypOywtJkk83030324 = sNXkpoypOywtJkk65528764;     sNXkpoypOywtJkk65528764 = sNXkpoypOywtJkk14654410;     sNXkpoypOywtJkk14654410 = sNXkpoypOywtJkk29807080;     sNXkpoypOywtJkk29807080 = sNXkpoypOywtJkk44581035;     sNXkpoypOywtJkk44581035 = sNXkpoypOywtJkk9720805;     sNXkpoypOywtJkk9720805 = sNXkpoypOywtJkk14768249;     sNXkpoypOywtJkk14768249 = sNXkpoypOywtJkk21852174;     sNXkpoypOywtJkk21852174 = sNXkpoypOywtJkk61795889;     sNXkpoypOywtJkk61795889 = sNXkpoypOywtJkk8119503;     sNXkpoypOywtJkk8119503 = sNXkpoypOywtJkk299735;     sNXkpoypOywtJkk299735 = sNXkpoypOywtJkk8121516;     sNXkpoypOywtJkk8121516 = sNXkpoypOywtJkk90533430;     sNXkpoypOywtJkk90533430 = sNXkpoypOywtJkk93100320;     sNXkpoypOywtJkk93100320 = sNXkpoypOywtJkk77759465;     sNXkpoypOywtJkk77759465 = sNXkpoypOywtJkk51774963;     sNXkpoypOywtJkk51774963 = sNXkpoypOywtJkk83062991;     sNXkpoypOywtJkk83062991 = sNXkpoypOywtJkk58171173;     sNXkpoypOywtJkk58171173 = sNXkpoypOywtJkk15550251;     sNXkpoypOywtJkk15550251 = sNXkpoypOywtJkk64925837;     sNXkpoypOywtJkk64925837 = sNXkpoypOywtJkk63263950;     sNXkpoypOywtJkk63263950 = sNXkpoypOywtJkk56531282;     sNXkpoypOywtJkk56531282 = sNXkpoypOywtJkk98469038;     sNXkpoypOywtJkk98469038 = sNXkpoypOywtJkk63142634;     sNXkpoypOywtJkk63142634 = sNXkpoypOywtJkk63941280;     sNXkpoypOywtJkk63941280 = sNXkpoypOywtJkk14546646;     sNXkpoypOywtJkk14546646 = sNXkpoypOywtJkk37000417;     sNXkpoypOywtJkk37000417 = sNXkpoypOywtJkk50573257;     sNXkpoypOywtJkk50573257 = sNXkpoypOywtJkk34533146;     sNXkpoypOywtJkk34533146 = sNXkpoypOywtJkk9453578;     sNXkpoypOywtJkk9453578 = sNXkpoypOywtJkk41739342;     sNXkpoypOywtJkk41739342 = sNXkpoypOywtJkk46650551;     sNXkpoypOywtJkk46650551 = sNXkpoypOywtJkk88927959;     sNXkpoypOywtJkk88927959 = sNXkpoypOywtJkk36126248;     sNXkpoypOywtJkk36126248 = sNXkpoypOywtJkk93618026;     sNXkpoypOywtJkk93618026 = sNXkpoypOywtJkk29488974;     sNXkpoypOywtJkk29488974 = sNXkpoypOywtJkk76541261;     sNXkpoypOywtJkk76541261 = sNXkpoypOywtJkk53880669;     sNXkpoypOywtJkk53880669 = sNXkpoypOywtJkk50144041;     sNXkpoypOywtJkk50144041 = sNXkpoypOywtJkk55841841;     sNXkpoypOywtJkk55841841 = sNXkpoypOywtJkk29872705;     sNXkpoypOywtJkk29872705 = sNXkpoypOywtJkk25158169;     sNXkpoypOywtJkk25158169 = sNXkpoypOywtJkk74686704;     sNXkpoypOywtJkk74686704 = sNXkpoypOywtJkk82786;     sNXkpoypOywtJkk82786 = sNXkpoypOywtJkk59492689;     sNXkpoypOywtJkk59492689 = sNXkpoypOywtJkk18593083;     sNXkpoypOywtJkk18593083 = sNXkpoypOywtJkk50204446;     sNXkpoypOywtJkk50204446 = sNXkpoypOywtJkk64313636;     sNXkpoypOywtJkk64313636 = sNXkpoypOywtJkk67484683;     sNXkpoypOywtJkk67484683 = sNXkpoypOywtJkk28973485;     sNXkpoypOywtJkk28973485 = sNXkpoypOywtJkk13929172;     sNXkpoypOywtJkk13929172 = sNXkpoypOywtJkk78964626;     sNXkpoypOywtJkk78964626 = sNXkpoypOywtJkk96281948;     sNXkpoypOywtJkk96281948 = sNXkpoypOywtJkk69992538;     sNXkpoypOywtJkk69992538 = sNXkpoypOywtJkk81758133;     sNXkpoypOywtJkk81758133 = sNXkpoypOywtJkk26499043;     sNXkpoypOywtJkk26499043 = sNXkpoypOywtJkk67059726;     sNXkpoypOywtJkk67059726 = sNXkpoypOywtJkk51511775;     sNXkpoypOywtJkk51511775 = sNXkpoypOywtJkk65865799;     sNXkpoypOywtJkk65865799 = sNXkpoypOywtJkk30034390;     sNXkpoypOywtJkk30034390 = sNXkpoypOywtJkk72720388;     sNXkpoypOywtJkk72720388 = sNXkpoypOywtJkk64194992;     sNXkpoypOywtJkk64194992 = sNXkpoypOywtJkk87319027;     sNXkpoypOywtJkk87319027 = sNXkpoypOywtJkk52342311;     sNXkpoypOywtJkk52342311 = sNXkpoypOywtJkk66380161;     sNXkpoypOywtJkk66380161 = sNXkpoypOywtJkk53649184;     sNXkpoypOywtJkk53649184 = sNXkpoypOywtJkk19193556;     sNXkpoypOywtJkk19193556 = sNXkpoypOywtJkk54407182;     sNXkpoypOywtJkk54407182 = sNXkpoypOywtJkk99482293;     sNXkpoypOywtJkk99482293 = sNXkpoypOywtJkk48270492;     sNXkpoypOywtJkk48270492 = sNXkpoypOywtJkk75233702;     sNXkpoypOywtJkk75233702 = sNXkpoypOywtJkk29182322;     sNXkpoypOywtJkk29182322 = sNXkpoypOywtJkk8027132;     sNXkpoypOywtJkk8027132 = sNXkpoypOywtJkk59708410;     sNXkpoypOywtJkk59708410 = sNXkpoypOywtJkk35053133;     sNXkpoypOywtJkk35053133 = sNXkpoypOywtJkk38105781;     sNXkpoypOywtJkk38105781 = sNXkpoypOywtJkk81844577;     sNXkpoypOywtJkk81844577 = sNXkpoypOywtJkk98386253;     sNXkpoypOywtJkk98386253 = sNXkpoypOywtJkk3649946;     sNXkpoypOywtJkk3649946 = sNXkpoypOywtJkk45348197;     sNXkpoypOywtJkk45348197 = sNXkpoypOywtJkk64342199;     sNXkpoypOywtJkk64342199 = sNXkpoypOywtJkk72686781;     sNXkpoypOywtJkk72686781 = sNXkpoypOywtJkk83088574;     sNXkpoypOywtJkk83088574 = sNXkpoypOywtJkk5559662;     sNXkpoypOywtJkk5559662 = sNXkpoypOywtJkk95524406;     sNXkpoypOywtJkk95524406 = sNXkpoypOywtJkk62774716;     sNXkpoypOywtJkk62774716 = sNXkpoypOywtJkk47010342;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void jMBdaUkfjfCeyIoI51299698() {     float BIoXBfPYJUHecxU47003871 = -318469518;    float BIoXBfPYJUHecxU75054910 = -834305496;    float BIoXBfPYJUHecxU9515501 = -359079356;    float BIoXBfPYJUHecxU44872284 = -82232012;    float BIoXBfPYJUHecxU35554700 = -781633610;    float BIoXBfPYJUHecxU75543309 = -424108462;    float BIoXBfPYJUHecxU25961292 = -836372904;    float BIoXBfPYJUHecxU14458199 = -482971160;    float BIoXBfPYJUHecxU63488612 = -899310087;    float BIoXBfPYJUHecxU93631676 = 85647973;    float BIoXBfPYJUHecxU57456404 = -543241516;    float BIoXBfPYJUHecxU26123876 = -489690047;    float BIoXBfPYJUHecxU66771290 = -650929605;    float BIoXBfPYJUHecxU98866303 = -180874494;    float BIoXBfPYJUHecxU28202434 = -969318072;    float BIoXBfPYJUHecxU40093312 = -279025746;    float BIoXBfPYJUHecxU70507169 = -106469806;    float BIoXBfPYJUHecxU52807612 = -85715522;    float BIoXBfPYJUHecxU65009306 = 73155349;    float BIoXBfPYJUHecxU81794568 = -679809110;    float BIoXBfPYJUHecxU94437672 = 69846061;    float BIoXBfPYJUHecxU148440 = -7682957;    float BIoXBfPYJUHecxU98674286 = 25375618;    float BIoXBfPYJUHecxU51769611 = -400744644;    float BIoXBfPYJUHecxU28742424 = -935494738;    float BIoXBfPYJUHecxU24695593 = -152797725;    float BIoXBfPYJUHecxU60755012 = -774319587;    float BIoXBfPYJUHecxU28503845 = -283273370;    float BIoXBfPYJUHecxU90437912 = -923456212;    float BIoXBfPYJUHecxU27864833 = -675061700;    float BIoXBfPYJUHecxU16475670 = -494922640;    float BIoXBfPYJUHecxU42895030 = -492468734;    float BIoXBfPYJUHecxU1002674 = -576947258;    float BIoXBfPYJUHecxU65208675 = -837021839;    float BIoXBfPYJUHecxU45621988 = -123227527;    float BIoXBfPYJUHecxU99359471 = 23073745;    float BIoXBfPYJUHecxU10410407 = 776369;    float BIoXBfPYJUHecxU33060314 = -639266270;    float BIoXBfPYJUHecxU99254876 = -768427714;    float BIoXBfPYJUHecxU86419538 = -57185317;    float BIoXBfPYJUHecxU24856085 = -635101919;    float BIoXBfPYJUHecxU36270918 = -226699789;    float BIoXBfPYJUHecxU94816506 = -716673731;    float BIoXBfPYJUHecxU68685476 = -893834827;    float BIoXBfPYJUHecxU25757395 = -370326369;    float BIoXBfPYJUHecxU87078519 = -157542631;    float BIoXBfPYJUHecxU16625752 = -769760706;    float BIoXBfPYJUHecxU11509829 = -532890515;    float BIoXBfPYJUHecxU12276416 = -58328467;    float BIoXBfPYJUHecxU49534396 = -933030602;    float BIoXBfPYJUHecxU89800631 = -492145179;    float BIoXBfPYJUHecxU68427926 = -679527537;    float BIoXBfPYJUHecxU25592389 = -322035353;    float BIoXBfPYJUHecxU66092112 = -313366723;    float BIoXBfPYJUHecxU56862025 = -846689838;    float BIoXBfPYJUHecxU46855432 = -210786561;    float BIoXBfPYJUHecxU76380624 = -759681115;    float BIoXBfPYJUHecxU57745889 = -958334713;    float BIoXBfPYJUHecxU16129861 = -146737274;    float BIoXBfPYJUHecxU10859108 = -528835886;    float BIoXBfPYJUHecxU14788297 = -649788876;    float BIoXBfPYJUHecxU97457446 = -453099535;    float BIoXBfPYJUHecxU24020287 = -559514949;    float BIoXBfPYJUHecxU35623779 = -124248387;    float BIoXBfPYJUHecxU77156006 = -419429388;    float BIoXBfPYJUHecxU14561374 = 49227218;    float BIoXBfPYJUHecxU25121203 = -912742790;    float BIoXBfPYJUHecxU1562616 = -813907767;    float BIoXBfPYJUHecxU53244316 = 42353033;    float BIoXBfPYJUHecxU28842962 = -892391817;    float BIoXBfPYJUHecxU29682906 = -179802115;    float BIoXBfPYJUHecxU37446855 = -467203537;    float BIoXBfPYJUHecxU53552736 = -317287808;    float BIoXBfPYJUHecxU78589767 = -869659334;    float BIoXBfPYJUHecxU56938483 = 55292809;    float BIoXBfPYJUHecxU58166755 = -703454151;    float BIoXBfPYJUHecxU5331933 = -291009226;    float BIoXBfPYJUHecxU29988810 = -80789556;    float BIoXBfPYJUHecxU26012217 = 69581725;    float BIoXBfPYJUHecxU41663905 = -677952108;    float BIoXBfPYJUHecxU8069841 = -383037019;    float BIoXBfPYJUHecxU49245183 = -141429072;    float BIoXBfPYJUHecxU16227429 = -124944903;    float BIoXBfPYJUHecxU40903516 = -990425610;    float BIoXBfPYJUHecxU38064202 = -82916522;    float BIoXBfPYJUHecxU48047743 = -815395104;    float BIoXBfPYJUHecxU17302641 = -70433382;    float BIoXBfPYJUHecxU34910562 = -163580535;    float BIoXBfPYJUHecxU8346650 = -990332002;    float BIoXBfPYJUHecxU98766556 = -912440967;    float BIoXBfPYJUHecxU22978848 = -217245141;    float BIoXBfPYJUHecxU52664517 = -40888919;    float BIoXBfPYJUHecxU16930454 = -392528996;    float BIoXBfPYJUHecxU88395769 = -139591829;    float BIoXBfPYJUHecxU71631241 = -407396442;    float BIoXBfPYJUHecxU27398638 = -82002385;    float BIoXBfPYJUHecxU12250632 = -667184841;    float BIoXBfPYJUHecxU59192727 = -492425345;    float BIoXBfPYJUHecxU91529469 = -374405439;    float BIoXBfPYJUHecxU11196021 = -318469518;     BIoXBfPYJUHecxU47003871 = BIoXBfPYJUHecxU75054910;     BIoXBfPYJUHecxU75054910 = BIoXBfPYJUHecxU9515501;     BIoXBfPYJUHecxU9515501 = BIoXBfPYJUHecxU44872284;     BIoXBfPYJUHecxU44872284 = BIoXBfPYJUHecxU35554700;     BIoXBfPYJUHecxU35554700 = BIoXBfPYJUHecxU75543309;     BIoXBfPYJUHecxU75543309 = BIoXBfPYJUHecxU25961292;     BIoXBfPYJUHecxU25961292 = BIoXBfPYJUHecxU14458199;     BIoXBfPYJUHecxU14458199 = BIoXBfPYJUHecxU63488612;     BIoXBfPYJUHecxU63488612 = BIoXBfPYJUHecxU93631676;     BIoXBfPYJUHecxU93631676 = BIoXBfPYJUHecxU57456404;     BIoXBfPYJUHecxU57456404 = BIoXBfPYJUHecxU26123876;     BIoXBfPYJUHecxU26123876 = BIoXBfPYJUHecxU66771290;     BIoXBfPYJUHecxU66771290 = BIoXBfPYJUHecxU98866303;     BIoXBfPYJUHecxU98866303 = BIoXBfPYJUHecxU28202434;     BIoXBfPYJUHecxU28202434 = BIoXBfPYJUHecxU40093312;     BIoXBfPYJUHecxU40093312 = BIoXBfPYJUHecxU70507169;     BIoXBfPYJUHecxU70507169 = BIoXBfPYJUHecxU52807612;     BIoXBfPYJUHecxU52807612 = BIoXBfPYJUHecxU65009306;     BIoXBfPYJUHecxU65009306 = BIoXBfPYJUHecxU81794568;     BIoXBfPYJUHecxU81794568 = BIoXBfPYJUHecxU94437672;     BIoXBfPYJUHecxU94437672 = BIoXBfPYJUHecxU148440;     BIoXBfPYJUHecxU148440 = BIoXBfPYJUHecxU98674286;     BIoXBfPYJUHecxU98674286 = BIoXBfPYJUHecxU51769611;     BIoXBfPYJUHecxU51769611 = BIoXBfPYJUHecxU28742424;     BIoXBfPYJUHecxU28742424 = BIoXBfPYJUHecxU24695593;     BIoXBfPYJUHecxU24695593 = BIoXBfPYJUHecxU60755012;     BIoXBfPYJUHecxU60755012 = BIoXBfPYJUHecxU28503845;     BIoXBfPYJUHecxU28503845 = BIoXBfPYJUHecxU90437912;     BIoXBfPYJUHecxU90437912 = BIoXBfPYJUHecxU27864833;     BIoXBfPYJUHecxU27864833 = BIoXBfPYJUHecxU16475670;     BIoXBfPYJUHecxU16475670 = BIoXBfPYJUHecxU42895030;     BIoXBfPYJUHecxU42895030 = BIoXBfPYJUHecxU1002674;     BIoXBfPYJUHecxU1002674 = BIoXBfPYJUHecxU65208675;     BIoXBfPYJUHecxU65208675 = BIoXBfPYJUHecxU45621988;     BIoXBfPYJUHecxU45621988 = BIoXBfPYJUHecxU99359471;     BIoXBfPYJUHecxU99359471 = BIoXBfPYJUHecxU10410407;     BIoXBfPYJUHecxU10410407 = BIoXBfPYJUHecxU33060314;     BIoXBfPYJUHecxU33060314 = BIoXBfPYJUHecxU99254876;     BIoXBfPYJUHecxU99254876 = BIoXBfPYJUHecxU86419538;     BIoXBfPYJUHecxU86419538 = BIoXBfPYJUHecxU24856085;     BIoXBfPYJUHecxU24856085 = BIoXBfPYJUHecxU36270918;     BIoXBfPYJUHecxU36270918 = BIoXBfPYJUHecxU94816506;     BIoXBfPYJUHecxU94816506 = BIoXBfPYJUHecxU68685476;     BIoXBfPYJUHecxU68685476 = BIoXBfPYJUHecxU25757395;     BIoXBfPYJUHecxU25757395 = BIoXBfPYJUHecxU87078519;     BIoXBfPYJUHecxU87078519 = BIoXBfPYJUHecxU16625752;     BIoXBfPYJUHecxU16625752 = BIoXBfPYJUHecxU11509829;     BIoXBfPYJUHecxU11509829 = BIoXBfPYJUHecxU12276416;     BIoXBfPYJUHecxU12276416 = BIoXBfPYJUHecxU49534396;     BIoXBfPYJUHecxU49534396 = BIoXBfPYJUHecxU89800631;     BIoXBfPYJUHecxU89800631 = BIoXBfPYJUHecxU68427926;     BIoXBfPYJUHecxU68427926 = BIoXBfPYJUHecxU25592389;     BIoXBfPYJUHecxU25592389 = BIoXBfPYJUHecxU66092112;     BIoXBfPYJUHecxU66092112 = BIoXBfPYJUHecxU56862025;     BIoXBfPYJUHecxU56862025 = BIoXBfPYJUHecxU46855432;     BIoXBfPYJUHecxU46855432 = BIoXBfPYJUHecxU76380624;     BIoXBfPYJUHecxU76380624 = BIoXBfPYJUHecxU57745889;     BIoXBfPYJUHecxU57745889 = BIoXBfPYJUHecxU16129861;     BIoXBfPYJUHecxU16129861 = BIoXBfPYJUHecxU10859108;     BIoXBfPYJUHecxU10859108 = BIoXBfPYJUHecxU14788297;     BIoXBfPYJUHecxU14788297 = BIoXBfPYJUHecxU97457446;     BIoXBfPYJUHecxU97457446 = BIoXBfPYJUHecxU24020287;     BIoXBfPYJUHecxU24020287 = BIoXBfPYJUHecxU35623779;     BIoXBfPYJUHecxU35623779 = BIoXBfPYJUHecxU77156006;     BIoXBfPYJUHecxU77156006 = BIoXBfPYJUHecxU14561374;     BIoXBfPYJUHecxU14561374 = BIoXBfPYJUHecxU25121203;     BIoXBfPYJUHecxU25121203 = BIoXBfPYJUHecxU1562616;     BIoXBfPYJUHecxU1562616 = BIoXBfPYJUHecxU53244316;     BIoXBfPYJUHecxU53244316 = BIoXBfPYJUHecxU28842962;     BIoXBfPYJUHecxU28842962 = BIoXBfPYJUHecxU29682906;     BIoXBfPYJUHecxU29682906 = BIoXBfPYJUHecxU37446855;     BIoXBfPYJUHecxU37446855 = BIoXBfPYJUHecxU53552736;     BIoXBfPYJUHecxU53552736 = BIoXBfPYJUHecxU78589767;     BIoXBfPYJUHecxU78589767 = BIoXBfPYJUHecxU56938483;     BIoXBfPYJUHecxU56938483 = BIoXBfPYJUHecxU58166755;     BIoXBfPYJUHecxU58166755 = BIoXBfPYJUHecxU5331933;     BIoXBfPYJUHecxU5331933 = BIoXBfPYJUHecxU29988810;     BIoXBfPYJUHecxU29988810 = BIoXBfPYJUHecxU26012217;     BIoXBfPYJUHecxU26012217 = BIoXBfPYJUHecxU41663905;     BIoXBfPYJUHecxU41663905 = BIoXBfPYJUHecxU8069841;     BIoXBfPYJUHecxU8069841 = BIoXBfPYJUHecxU49245183;     BIoXBfPYJUHecxU49245183 = BIoXBfPYJUHecxU16227429;     BIoXBfPYJUHecxU16227429 = BIoXBfPYJUHecxU40903516;     BIoXBfPYJUHecxU40903516 = BIoXBfPYJUHecxU38064202;     BIoXBfPYJUHecxU38064202 = BIoXBfPYJUHecxU48047743;     BIoXBfPYJUHecxU48047743 = BIoXBfPYJUHecxU17302641;     BIoXBfPYJUHecxU17302641 = BIoXBfPYJUHecxU34910562;     BIoXBfPYJUHecxU34910562 = BIoXBfPYJUHecxU8346650;     BIoXBfPYJUHecxU8346650 = BIoXBfPYJUHecxU98766556;     BIoXBfPYJUHecxU98766556 = BIoXBfPYJUHecxU22978848;     BIoXBfPYJUHecxU22978848 = BIoXBfPYJUHecxU52664517;     BIoXBfPYJUHecxU52664517 = BIoXBfPYJUHecxU16930454;     BIoXBfPYJUHecxU16930454 = BIoXBfPYJUHecxU88395769;     BIoXBfPYJUHecxU88395769 = BIoXBfPYJUHecxU71631241;     BIoXBfPYJUHecxU71631241 = BIoXBfPYJUHecxU27398638;     BIoXBfPYJUHecxU27398638 = BIoXBfPYJUHecxU12250632;     BIoXBfPYJUHecxU12250632 = BIoXBfPYJUHecxU59192727;     BIoXBfPYJUHecxU59192727 = BIoXBfPYJUHecxU91529469;     BIoXBfPYJUHecxU91529469 = BIoXBfPYJUHecxU11196021;     BIoXBfPYJUHecxU11196021 = BIoXBfPYJUHecxU47003871;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WkpNtWcjFQVaeepE57628245() {     float xJxRztObxFqnSkY1658269 = -755997102;    float xJxRztObxFqnSkY22052530 = -313870310;    float xJxRztObxFqnSkY70248146 = -744463970;    float xJxRztObxFqnSkY16307226 = -88334423;    float xJxRztObxFqnSkY10006371 = -454068849;    float xJxRztObxFqnSkY89717706 = -553259298;    float xJxRztObxFqnSkY47061995 = -38265577;    float xJxRztObxFqnSkY43633303 = -983539393;    float xJxRztObxFqnSkY51263023 = -946163458;    float xJxRztObxFqnSkY76263661 = -193153297;    float xJxRztObxFqnSkY36067699 = -68598088;    float xJxRztObxFqnSkY88888589 = -676114136;    float xJxRztObxFqnSkY30588605 = -454469910;    float xJxRztObxFqnSkY91832734 = -892978072;    float xJxRztObxFqnSkY58426507 = -876439703;    float xJxRztObxFqnSkY29860848 = 90814690;    float xJxRztObxFqnSkY70338932 = -580069634;    float xJxRztObxFqnSkY20404199 = -149279989;    float xJxRztObxFqnSkY66229906 = -212656198;    float xJxRztObxFqnSkY99429123 = 74824744;    float xJxRztObxFqnSkY28612061 = -498237090;    float xJxRztObxFqnSkY51401028 = -174127065;    float xJxRztObxFqnSkY35144118 = -561367803;    float xJxRztObxFqnSkY53197980 = -99785460;    float xJxRztObxFqnSkY24495040 = -208408190;    float xJxRztObxFqnSkY65936540 = -156506509;    float xJxRztObxFqnSkY62744936 = -229163866;    float xJxRztObxFqnSkY81752845 = -291325230;    float xJxRztObxFqnSkY30335589 = -203701087;    float xJxRztObxFqnSkY22300243 = -965819668;    float xJxRztObxFqnSkY68181198 = 56204835;    float xJxRztObxFqnSkY73886640 = -903965263;    float xJxRztObxFqnSkY68658550 = -33464872;    float xJxRztObxFqnSkY59864567 = -517142649;    float xJxRztObxFqnSkY80126144 = -341090787;    float xJxRztObxFqnSkY86686013 = -224921852;    float xJxRztObxFqnSkY85114894 = -949650252;    float xJxRztObxFqnSkY46037553 = -513195290;    float xJxRztObxFqnSkY50024525 = -50376316;    float xJxRztObxFqnSkY10405934 = -815493916;    float xJxRztObxFqnSkY80815485 = -747652077;    float xJxRztObxFqnSkY72945576 = -834231876;    float xJxRztObxFqnSkY85816689 = -557179049;    float xJxRztObxFqnSkY77679875 = -367514492;    float xJxRztObxFqnSkY85741457 = -502843967;    float xJxRztObxFqnSkY42412121 = -226332246;    float xJxRztObxFqnSkY33099464 = -796818090;    float xJxRztObxFqnSkY34937138 = -510542888;    float xJxRztObxFqnSkY54619182 = 41941344;    float xJxRztObxFqnSkY94376058 = -73049897;    float xJxRztObxFqnSkY64914171 = -149360681;    float xJxRztObxFqnSkY57810076 = -94593519;    float xJxRztObxFqnSkY57320392 = -61950811;    float xJxRztObxFqnSkY23507869 = -209253208;    float xJxRztObxFqnSkY29016185 = -142598519;    float xJxRztObxFqnSkY50257240 = -481870037;    float xJxRztObxFqnSkY86908411 = -752502508;    float xJxRztObxFqnSkY17050167 = -544678510;    float xJxRztObxFqnSkY91812185 = -879926233;    float xJxRztObxFqnSkY44069831 = -197562340;    float xJxRztObxFqnSkY26972770 = -224095433;    float xJxRztObxFqnSkY65309150 = -746940347;    float xJxRztObxFqnSkY13297714 = -679838306;    float xJxRztObxFqnSkY28962781 = -980343790;    float xJxRztObxFqnSkY8082463 = -149358132;    float xJxRztObxFqnSkY62181058 = -164632826;    float xJxRztObxFqnSkY20230039 = -542649265;    float xJxRztObxFqnSkY70724037 = -937327262;    float xJxRztObxFqnSkY11706590 = -451887286;    float xJxRztObxFqnSkY71740493 = -551517851;    float xJxRztObxFqnSkY44745954 = 40464941;    float xJxRztObxFqnSkY24301380 = 33125656;    float xJxRztObxFqnSkY70379674 = 1096327;    float xJxRztObxFqnSkY55823973 = -397162282;    float xJxRztObxFqnSkY18613638 = -177523180;    float xJxRztObxFqnSkY55666484 = -664005215;    float xJxRztObxFqnSkY65584339 = -616948016;    float xJxRztObxFqnSkY57464242 = -93853311;    float xJxRztObxFqnSkY67456522 = -596941494;    float xJxRztObxFqnSkY82082919 = -982075945;    float xJxRztObxFqnSkY32837076 = -359688420;    float xJxRztObxFqnSkY27807798 = -718620978;    float xJxRztObxFqnSkY27133663 = -233266575;    float xJxRztObxFqnSkY35959531 = -30651191;    float xJxRztObxFqnSkY57386071 = -716458988;    float xJxRztObxFqnSkY10371123 = -849201647;    float xJxRztObxFqnSkY16566249 = -742014453;    float xJxRztObxFqnSkY45150681 = -824211664;    float xJxRztObxFqnSkY30848383 = -274544130;    float xJxRztObxFqnSkY29868904 = -859220750;    float xJxRztObxFqnSkY99777601 = -472419345;    float xJxRztObxFqnSkY68064727 = -304971742;    float xJxRztObxFqnSkY54225367 = -633269058;    float xJxRztObxFqnSkY5954695 = -852813976;    float xJxRztObxFqnSkY83433163 = -491398484;    float xJxRztObxFqnSkY15506335 = 99288269;    float xJxRztObxFqnSkY59647863 = -54393570;    float xJxRztObxFqnSkY56853908 = -576835260;    float xJxRztObxFqnSkY69597412 = -118156361;    float xJxRztObxFqnSkY23560400 = -755997102;     xJxRztObxFqnSkY1658269 = xJxRztObxFqnSkY22052530;     xJxRztObxFqnSkY22052530 = xJxRztObxFqnSkY70248146;     xJxRztObxFqnSkY70248146 = xJxRztObxFqnSkY16307226;     xJxRztObxFqnSkY16307226 = xJxRztObxFqnSkY10006371;     xJxRztObxFqnSkY10006371 = xJxRztObxFqnSkY89717706;     xJxRztObxFqnSkY89717706 = xJxRztObxFqnSkY47061995;     xJxRztObxFqnSkY47061995 = xJxRztObxFqnSkY43633303;     xJxRztObxFqnSkY43633303 = xJxRztObxFqnSkY51263023;     xJxRztObxFqnSkY51263023 = xJxRztObxFqnSkY76263661;     xJxRztObxFqnSkY76263661 = xJxRztObxFqnSkY36067699;     xJxRztObxFqnSkY36067699 = xJxRztObxFqnSkY88888589;     xJxRztObxFqnSkY88888589 = xJxRztObxFqnSkY30588605;     xJxRztObxFqnSkY30588605 = xJxRztObxFqnSkY91832734;     xJxRztObxFqnSkY91832734 = xJxRztObxFqnSkY58426507;     xJxRztObxFqnSkY58426507 = xJxRztObxFqnSkY29860848;     xJxRztObxFqnSkY29860848 = xJxRztObxFqnSkY70338932;     xJxRztObxFqnSkY70338932 = xJxRztObxFqnSkY20404199;     xJxRztObxFqnSkY20404199 = xJxRztObxFqnSkY66229906;     xJxRztObxFqnSkY66229906 = xJxRztObxFqnSkY99429123;     xJxRztObxFqnSkY99429123 = xJxRztObxFqnSkY28612061;     xJxRztObxFqnSkY28612061 = xJxRztObxFqnSkY51401028;     xJxRztObxFqnSkY51401028 = xJxRztObxFqnSkY35144118;     xJxRztObxFqnSkY35144118 = xJxRztObxFqnSkY53197980;     xJxRztObxFqnSkY53197980 = xJxRztObxFqnSkY24495040;     xJxRztObxFqnSkY24495040 = xJxRztObxFqnSkY65936540;     xJxRztObxFqnSkY65936540 = xJxRztObxFqnSkY62744936;     xJxRztObxFqnSkY62744936 = xJxRztObxFqnSkY81752845;     xJxRztObxFqnSkY81752845 = xJxRztObxFqnSkY30335589;     xJxRztObxFqnSkY30335589 = xJxRztObxFqnSkY22300243;     xJxRztObxFqnSkY22300243 = xJxRztObxFqnSkY68181198;     xJxRztObxFqnSkY68181198 = xJxRztObxFqnSkY73886640;     xJxRztObxFqnSkY73886640 = xJxRztObxFqnSkY68658550;     xJxRztObxFqnSkY68658550 = xJxRztObxFqnSkY59864567;     xJxRztObxFqnSkY59864567 = xJxRztObxFqnSkY80126144;     xJxRztObxFqnSkY80126144 = xJxRztObxFqnSkY86686013;     xJxRztObxFqnSkY86686013 = xJxRztObxFqnSkY85114894;     xJxRztObxFqnSkY85114894 = xJxRztObxFqnSkY46037553;     xJxRztObxFqnSkY46037553 = xJxRztObxFqnSkY50024525;     xJxRztObxFqnSkY50024525 = xJxRztObxFqnSkY10405934;     xJxRztObxFqnSkY10405934 = xJxRztObxFqnSkY80815485;     xJxRztObxFqnSkY80815485 = xJxRztObxFqnSkY72945576;     xJxRztObxFqnSkY72945576 = xJxRztObxFqnSkY85816689;     xJxRztObxFqnSkY85816689 = xJxRztObxFqnSkY77679875;     xJxRztObxFqnSkY77679875 = xJxRztObxFqnSkY85741457;     xJxRztObxFqnSkY85741457 = xJxRztObxFqnSkY42412121;     xJxRztObxFqnSkY42412121 = xJxRztObxFqnSkY33099464;     xJxRztObxFqnSkY33099464 = xJxRztObxFqnSkY34937138;     xJxRztObxFqnSkY34937138 = xJxRztObxFqnSkY54619182;     xJxRztObxFqnSkY54619182 = xJxRztObxFqnSkY94376058;     xJxRztObxFqnSkY94376058 = xJxRztObxFqnSkY64914171;     xJxRztObxFqnSkY64914171 = xJxRztObxFqnSkY57810076;     xJxRztObxFqnSkY57810076 = xJxRztObxFqnSkY57320392;     xJxRztObxFqnSkY57320392 = xJxRztObxFqnSkY23507869;     xJxRztObxFqnSkY23507869 = xJxRztObxFqnSkY29016185;     xJxRztObxFqnSkY29016185 = xJxRztObxFqnSkY50257240;     xJxRztObxFqnSkY50257240 = xJxRztObxFqnSkY86908411;     xJxRztObxFqnSkY86908411 = xJxRztObxFqnSkY17050167;     xJxRztObxFqnSkY17050167 = xJxRztObxFqnSkY91812185;     xJxRztObxFqnSkY91812185 = xJxRztObxFqnSkY44069831;     xJxRztObxFqnSkY44069831 = xJxRztObxFqnSkY26972770;     xJxRztObxFqnSkY26972770 = xJxRztObxFqnSkY65309150;     xJxRztObxFqnSkY65309150 = xJxRztObxFqnSkY13297714;     xJxRztObxFqnSkY13297714 = xJxRztObxFqnSkY28962781;     xJxRztObxFqnSkY28962781 = xJxRztObxFqnSkY8082463;     xJxRztObxFqnSkY8082463 = xJxRztObxFqnSkY62181058;     xJxRztObxFqnSkY62181058 = xJxRztObxFqnSkY20230039;     xJxRztObxFqnSkY20230039 = xJxRztObxFqnSkY70724037;     xJxRztObxFqnSkY70724037 = xJxRztObxFqnSkY11706590;     xJxRztObxFqnSkY11706590 = xJxRztObxFqnSkY71740493;     xJxRztObxFqnSkY71740493 = xJxRztObxFqnSkY44745954;     xJxRztObxFqnSkY44745954 = xJxRztObxFqnSkY24301380;     xJxRztObxFqnSkY24301380 = xJxRztObxFqnSkY70379674;     xJxRztObxFqnSkY70379674 = xJxRztObxFqnSkY55823973;     xJxRztObxFqnSkY55823973 = xJxRztObxFqnSkY18613638;     xJxRztObxFqnSkY18613638 = xJxRztObxFqnSkY55666484;     xJxRztObxFqnSkY55666484 = xJxRztObxFqnSkY65584339;     xJxRztObxFqnSkY65584339 = xJxRztObxFqnSkY57464242;     xJxRztObxFqnSkY57464242 = xJxRztObxFqnSkY67456522;     xJxRztObxFqnSkY67456522 = xJxRztObxFqnSkY82082919;     xJxRztObxFqnSkY82082919 = xJxRztObxFqnSkY32837076;     xJxRztObxFqnSkY32837076 = xJxRztObxFqnSkY27807798;     xJxRztObxFqnSkY27807798 = xJxRztObxFqnSkY27133663;     xJxRztObxFqnSkY27133663 = xJxRztObxFqnSkY35959531;     xJxRztObxFqnSkY35959531 = xJxRztObxFqnSkY57386071;     xJxRztObxFqnSkY57386071 = xJxRztObxFqnSkY10371123;     xJxRztObxFqnSkY10371123 = xJxRztObxFqnSkY16566249;     xJxRztObxFqnSkY16566249 = xJxRztObxFqnSkY45150681;     xJxRztObxFqnSkY45150681 = xJxRztObxFqnSkY30848383;     xJxRztObxFqnSkY30848383 = xJxRztObxFqnSkY29868904;     xJxRztObxFqnSkY29868904 = xJxRztObxFqnSkY99777601;     xJxRztObxFqnSkY99777601 = xJxRztObxFqnSkY68064727;     xJxRztObxFqnSkY68064727 = xJxRztObxFqnSkY54225367;     xJxRztObxFqnSkY54225367 = xJxRztObxFqnSkY5954695;     xJxRztObxFqnSkY5954695 = xJxRztObxFqnSkY83433163;     xJxRztObxFqnSkY83433163 = xJxRztObxFqnSkY15506335;     xJxRztObxFqnSkY15506335 = xJxRztObxFqnSkY59647863;     xJxRztObxFqnSkY59647863 = xJxRztObxFqnSkY56853908;     xJxRztObxFqnSkY56853908 = xJxRztObxFqnSkY69597412;     xJxRztObxFqnSkY69597412 = xJxRztObxFqnSkY23560400;     xJxRztObxFqnSkY23560400 = xJxRztObxFqnSkY1658269;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ePgkbWHMMucLqoIQ3323938() {     float FwxeDYTYpCGiZjJ20847227 = -599771927;    float FwxeDYTYpCGiZjJ94350386 = 44521357;    float FwxeDYTYpCGiZjJ94907528 = -321310122;    float FwxeDYTYpCGiZjJ598673 = -93826593;    float FwxeDYTYpCGiZjJ37012874 = -489260564;    float FwxeDYTYpCGiZjJ2474664 = -119495051;    float FwxeDYTYpCGiZjJ26052629 = -199968982;    float FwxeDYTYpCGiZjJ79890895 = -444050802;    float FwxeDYTYpCGiZjJ20259993 = -218331491;    float FwxeDYTYpCGiZjJ40632448 = -444074439;    float FwxeDYTYpCGiZjJ96817864 = -851419003;    float FwxeDYTYpCGiZjJ65376831 = -733895816;    float FwxeDYTYpCGiZjJ88024188 = -827656184;    float FwxeDYTYpCGiZjJ85502521 = -873871292;    float FwxeDYTYpCGiZjJ75628173 = -242849171;    float FwxeDYTYpCGiZjJ10651631 = -236328918;    float FwxeDYTYpCGiZjJ70187520 = -346309480;    float FwxeDYTYpCGiZjJ11241128 = -536488010;    float FwxeDYTYpCGiZjJ87328447 = -799886591;    float FwxeDYTYpCGiZjJ95300223 = -456004788;    float FwxeDYTYpCGiZjJ29369010 = -19511926;    float FwxeDYTYpCGiZjJ77528358 = -763926762;    float FwxeDYTYpCGiZjJ37966966 = -869436882;    float FwxeDYTYpCGiZjJ4483512 = -268922195;    float FwxeDYTYpCGiZjJ40672395 = -984030297;    float FwxeDYTYpCGiZjJ13053393 = -379844415;    float FwxeDYTYpCGiZjJ74535868 = -178523716;    float FwxeDYTYpCGiZjJ29676945 = -738571905;    float FwxeDYTYpCGiZjJ36243499 = -655921475;    float FwxeDYTYpCGiZjJ87292110 = -897501840;    float FwxeDYTYpCGiZjJ34716175 = -327780439;    float FwxeDYTYpCGiZjJ51779090 = -834312138;    float FwxeDYTYpCGiZjJ9548840 = -644330724;    float FwxeDYTYpCGiZjJ25054871 = -9251377;    float FwxeDYTYpCGiZjJ31179886 = -867167720;    float FwxeDYTYpCGiZjJ65279901 = -338117889;    float FwxeDYTYpCGiZjJ22348933 = 64965790;    float FwxeDYTYpCGiZjJ77717068 = -399731409;    float FwxeDYTYpCGiZjJ5717210 = -394130057;    float FwxeDYTYpCGiZjJ41993689 = -617971656;    float FwxeDYTYpCGiZjJ91178946 = -78947220;    float FwxeDYTYpCGiZjJ5952769 = -941010754;    float FwxeDYTYpCGiZjJ47716854 = -303633835;    float FwxeDYTYpCGiZjJ5774835 = -993826192;    float FwxeDYTYpCGiZjJ29727115 = -182109805;    float FwxeDYTYpCGiZjJ92212362 = -68242899;    float FwxeDYTYpCGiZjJ57925804 = -821169735;    float FwxeDYTYpCGiZjJ16021716 = -930430023;    float FwxeDYTYpCGiZjJ92727672 = -857815826;    float FwxeDYTYpCGiZjJ34733555 = -509067261;    float FwxeDYTYpCGiZjJ72516356 = -610854632;    float FwxeDYTYpCGiZjJ78254010 = -778152904;    float FwxeDYTYpCGiZjJ95875595 = 62125278;    float FwxeDYTYpCGiZjJ75182051 = -445551044;    float FwxeDYTYpCGiZjJ73954928 = 51083668;    float FwxeDYTYpCGiZjJ43318868 = -835845166;    float FwxeDYTYpCGiZjJ56383421 = -86041761;    float FwxeDYTYpCGiZjJ90424016 = 47612073;    float FwxeDYTYpCGiZjJ59926278 = -109796296;    float FwxeDYTYpCGiZjJ23959482 = -9416150;    float FwxeDYTYpCGiZjJ27938796 = -940971335;    float FwxeDYTYpCGiZjJ96375683 = -461397078;    float FwxeDYTYpCGiZjJ43647397 = -788129327;    float FwxeDYTYpCGiZjJ32967882 = -320829652;    float FwxeDYTYpCGiZjJ5916274 = -16294001;    float FwxeDYTYpCGiZjJ45038775 = 82893134;    float FwxeDYTYpCGiZjJ55827992 = 10434908;    float FwxeDYTYpCGiZjJ62969317 = -718404807;    float FwxeDYTYpCGiZjJ54322636 = 93296428;    float FwxeDYTYpCGiZjJ10348272 = -904731283;    float FwxeDYTYpCGiZjJ88302697 = -201294708;    float FwxeDYTYpCGiZjJ92470451 = -946578071;    float FwxeDYTYpCGiZjJ5523919 = -42357953;    float FwxeDYTYpCGiZjJ45334759 = -81914936;    float FwxeDYTYpCGiZjJ4121278 = -277057569;    float FwxeDYTYpCGiZjJ23416241 = -78501173;    float FwxeDYTYpCGiZjJ29811505 = -360292927;    float FwxeDYTYpCGiZjJ32192132 = -875610691;    float FwxeDYTYpCGiZjJ74756397 = 13187609;    float FwxeDYTYpCGiZjJ48460032 = -815787399;    float FwxeDYTYpCGiZjJ55127588 = -558674681;    float FwxeDYTYpCGiZjJ58514152 = -248093694;    float FwxeDYTYpCGiZjJ36949273 = -880756080;    float FwxeDYTYpCGiZjJ1509944 = -46854214;    float FwxeDYTYpCGiZjJ14775754 = -186647208;    float FwxeDYTYpCGiZjJ56462164 = -549627535;    float FwxeDYTYpCGiZjJ55903495 = -796437416;    float FwxeDYTYpCGiZjJ34366789 = -98779681;    float FwxeDYTYpCGiZjJ51099943 = 39664954;    float FwxeDYTYpCGiZjJ87861017 = 68677445;    float FwxeDYTYpCGiZjJ8896481 = -152076128;    float FwxeDYTYpCGiZjJ31924917 = -982646283;    float FwxeDYTYpCGiZjJ17790790 = -189935114;    float FwxeDYTYpCGiZjJ81757727 = -284713908;    float FwxeDYTYpCGiZjJ14054893 = -677000322;    float FwxeDYTYpCGiZjJ94803262 = -617550142;    float FwxeDYTYpCGiZjJ62305371 = -52881427;    float FwxeDYTYpCGiZjJ14748972 = -982804184;    float FwxeDYTYpCGiZjJ99858560 = -877532191;    float FwxeDYTYpCGiZjJ84688340 = -599771927;     FwxeDYTYpCGiZjJ20847227 = FwxeDYTYpCGiZjJ94350386;     FwxeDYTYpCGiZjJ94350386 = FwxeDYTYpCGiZjJ94907528;     FwxeDYTYpCGiZjJ94907528 = FwxeDYTYpCGiZjJ598673;     FwxeDYTYpCGiZjJ598673 = FwxeDYTYpCGiZjJ37012874;     FwxeDYTYpCGiZjJ37012874 = FwxeDYTYpCGiZjJ2474664;     FwxeDYTYpCGiZjJ2474664 = FwxeDYTYpCGiZjJ26052629;     FwxeDYTYpCGiZjJ26052629 = FwxeDYTYpCGiZjJ79890895;     FwxeDYTYpCGiZjJ79890895 = FwxeDYTYpCGiZjJ20259993;     FwxeDYTYpCGiZjJ20259993 = FwxeDYTYpCGiZjJ40632448;     FwxeDYTYpCGiZjJ40632448 = FwxeDYTYpCGiZjJ96817864;     FwxeDYTYpCGiZjJ96817864 = FwxeDYTYpCGiZjJ65376831;     FwxeDYTYpCGiZjJ65376831 = FwxeDYTYpCGiZjJ88024188;     FwxeDYTYpCGiZjJ88024188 = FwxeDYTYpCGiZjJ85502521;     FwxeDYTYpCGiZjJ85502521 = FwxeDYTYpCGiZjJ75628173;     FwxeDYTYpCGiZjJ75628173 = FwxeDYTYpCGiZjJ10651631;     FwxeDYTYpCGiZjJ10651631 = FwxeDYTYpCGiZjJ70187520;     FwxeDYTYpCGiZjJ70187520 = FwxeDYTYpCGiZjJ11241128;     FwxeDYTYpCGiZjJ11241128 = FwxeDYTYpCGiZjJ87328447;     FwxeDYTYpCGiZjJ87328447 = FwxeDYTYpCGiZjJ95300223;     FwxeDYTYpCGiZjJ95300223 = FwxeDYTYpCGiZjJ29369010;     FwxeDYTYpCGiZjJ29369010 = FwxeDYTYpCGiZjJ77528358;     FwxeDYTYpCGiZjJ77528358 = FwxeDYTYpCGiZjJ37966966;     FwxeDYTYpCGiZjJ37966966 = FwxeDYTYpCGiZjJ4483512;     FwxeDYTYpCGiZjJ4483512 = FwxeDYTYpCGiZjJ40672395;     FwxeDYTYpCGiZjJ40672395 = FwxeDYTYpCGiZjJ13053393;     FwxeDYTYpCGiZjJ13053393 = FwxeDYTYpCGiZjJ74535868;     FwxeDYTYpCGiZjJ74535868 = FwxeDYTYpCGiZjJ29676945;     FwxeDYTYpCGiZjJ29676945 = FwxeDYTYpCGiZjJ36243499;     FwxeDYTYpCGiZjJ36243499 = FwxeDYTYpCGiZjJ87292110;     FwxeDYTYpCGiZjJ87292110 = FwxeDYTYpCGiZjJ34716175;     FwxeDYTYpCGiZjJ34716175 = FwxeDYTYpCGiZjJ51779090;     FwxeDYTYpCGiZjJ51779090 = FwxeDYTYpCGiZjJ9548840;     FwxeDYTYpCGiZjJ9548840 = FwxeDYTYpCGiZjJ25054871;     FwxeDYTYpCGiZjJ25054871 = FwxeDYTYpCGiZjJ31179886;     FwxeDYTYpCGiZjJ31179886 = FwxeDYTYpCGiZjJ65279901;     FwxeDYTYpCGiZjJ65279901 = FwxeDYTYpCGiZjJ22348933;     FwxeDYTYpCGiZjJ22348933 = FwxeDYTYpCGiZjJ77717068;     FwxeDYTYpCGiZjJ77717068 = FwxeDYTYpCGiZjJ5717210;     FwxeDYTYpCGiZjJ5717210 = FwxeDYTYpCGiZjJ41993689;     FwxeDYTYpCGiZjJ41993689 = FwxeDYTYpCGiZjJ91178946;     FwxeDYTYpCGiZjJ91178946 = FwxeDYTYpCGiZjJ5952769;     FwxeDYTYpCGiZjJ5952769 = FwxeDYTYpCGiZjJ47716854;     FwxeDYTYpCGiZjJ47716854 = FwxeDYTYpCGiZjJ5774835;     FwxeDYTYpCGiZjJ5774835 = FwxeDYTYpCGiZjJ29727115;     FwxeDYTYpCGiZjJ29727115 = FwxeDYTYpCGiZjJ92212362;     FwxeDYTYpCGiZjJ92212362 = FwxeDYTYpCGiZjJ57925804;     FwxeDYTYpCGiZjJ57925804 = FwxeDYTYpCGiZjJ16021716;     FwxeDYTYpCGiZjJ16021716 = FwxeDYTYpCGiZjJ92727672;     FwxeDYTYpCGiZjJ92727672 = FwxeDYTYpCGiZjJ34733555;     FwxeDYTYpCGiZjJ34733555 = FwxeDYTYpCGiZjJ72516356;     FwxeDYTYpCGiZjJ72516356 = FwxeDYTYpCGiZjJ78254010;     FwxeDYTYpCGiZjJ78254010 = FwxeDYTYpCGiZjJ95875595;     FwxeDYTYpCGiZjJ95875595 = FwxeDYTYpCGiZjJ75182051;     FwxeDYTYpCGiZjJ75182051 = FwxeDYTYpCGiZjJ73954928;     FwxeDYTYpCGiZjJ73954928 = FwxeDYTYpCGiZjJ43318868;     FwxeDYTYpCGiZjJ43318868 = FwxeDYTYpCGiZjJ56383421;     FwxeDYTYpCGiZjJ56383421 = FwxeDYTYpCGiZjJ90424016;     FwxeDYTYpCGiZjJ90424016 = FwxeDYTYpCGiZjJ59926278;     FwxeDYTYpCGiZjJ59926278 = FwxeDYTYpCGiZjJ23959482;     FwxeDYTYpCGiZjJ23959482 = FwxeDYTYpCGiZjJ27938796;     FwxeDYTYpCGiZjJ27938796 = FwxeDYTYpCGiZjJ96375683;     FwxeDYTYpCGiZjJ96375683 = FwxeDYTYpCGiZjJ43647397;     FwxeDYTYpCGiZjJ43647397 = FwxeDYTYpCGiZjJ32967882;     FwxeDYTYpCGiZjJ32967882 = FwxeDYTYpCGiZjJ5916274;     FwxeDYTYpCGiZjJ5916274 = FwxeDYTYpCGiZjJ45038775;     FwxeDYTYpCGiZjJ45038775 = FwxeDYTYpCGiZjJ55827992;     FwxeDYTYpCGiZjJ55827992 = FwxeDYTYpCGiZjJ62969317;     FwxeDYTYpCGiZjJ62969317 = FwxeDYTYpCGiZjJ54322636;     FwxeDYTYpCGiZjJ54322636 = FwxeDYTYpCGiZjJ10348272;     FwxeDYTYpCGiZjJ10348272 = FwxeDYTYpCGiZjJ88302697;     FwxeDYTYpCGiZjJ88302697 = FwxeDYTYpCGiZjJ92470451;     FwxeDYTYpCGiZjJ92470451 = FwxeDYTYpCGiZjJ5523919;     FwxeDYTYpCGiZjJ5523919 = FwxeDYTYpCGiZjJ45334759;     FwxeDYTYpCGiZjJ45334759 = FwxeDYTYpCGiZjJ4121278;     FwxeDYTYpCGiZjJ4121278 = FwxeDYTYpCGiZjJ23416241;     FwxeDYTYpCGiZjJ23416241 = FwxeDYTYpCGiZjJ29811505;     FwxeDYTYpCGiZjJ29811505 = FwxeDYTYpCGiZjJ32192132;     FwxeDYTYpCGiZjJ32192132 = FwxeDYTYpCGiZjJ74756397;     FwxeDYTYpCGiZjJ74756397 = FwxeDYTYpCGiZjJ48460032;     FwxeDYTYpCGiZjJ48460032 = FwxeDYTYpCGiZjJ55127588;     FwxeDYTYpCGiZjJ55127588 = FwxeDYTYpCGiZjJ58514152;     FwxeDYTYpCGiZjJ58514152 = FwxeDYTYpCGiZjJ36949273;     FwxeDYTYpCGiZjJ36949273 = FwxeDYTYpCGiZjJ1509944;     FwxeDYTYpCGiZjJ1509944 = FwxeDYTYpCGiZjJ14775754;     FwxeDYTYpCGiZjJ14775754 = FwxeDYTYpCGiZjJ56462164;     FwxeDYTYpCGiZjJ56462164 = FwxeDYTYpCGiZjJ55903495;     FwxeDYTYpCGiZjJ55903495 = FwxeDYTYpCGiZjJ34366789;     FwxeDYTYpCGiZjJ34366789 = FwxeDYTYpCGiZjJ51099943;     FwxeDYTYpCGiZjJ51099943 = FwxeDYTYpCGiZjJ87861017;     FwxeDYTYpCGiZjJ87861017 = FwxeDYTYpCGiZjJ8896481;     FwxeDYTYpCGiZjJ8896481 = FwxeDYTYpCGiZjJ31924917;     FwxeDYTYpCGiZjJ31924917 = FwxeDYTYpCGiZjJ17790790;     FwxeDYTYpCGiZjJ17790790 = FwxeDYTYpCGiZjJ81757727;     FwxeDYTYpCGiZjJ81757727 = FwxeDYTYpCGiZjJ14054893;     FwxeDYTYpCGiZjJ14054893 = FwxeDYTYpCGiZjJ94803262;     FwxeDYTYpCGiZjJ94803262 = FwxeDYTYpCGiZjJ62305371;     FwxeDYTYpCGiZjJ62305371 = FwxeDYTYpCGiZjJ14748972;     FwxeDYTYpCGiZjJ14748972 = FwxeDYTYpCGiZjJ99858560;     FwxeDYTYpCGiZjJ99858560 = FwxeDYTYpCGiZjJ84688340;     FwxeDYTYpCGiZjJ84688340 = FwxeDYTYpCGiZjJ20847227;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void OPaFkVtueQoMVexu57740151() {     float mdkzpQZLNRJRjDK91498893 = -117921058;    float mdkzpQZLNRJRjDK63023904 = -927121942;    float mdkzpQZLNRJRjDK74876447 = -763604582;    float mdkzpQZLNRJRjDK40529960 = -283964982;    float mdkzpQZLNRJRjDK58463195 = -272666912;    float mdkzpQZLNRJRjDK92474426 = -921076202;    float mdkzpQZLNRJRjDK44431524 = -198744692;    float mdkzpQZLNRJRjDK36733276 = -790867892;    float mdkzpQZLNRJRjDK66125047 = -311678820;    float mdkzpQZLNRJRjDK63598909 = -745169790;    float mdkzpQZLNRJRjDK39067026 = -886320933;    float mdkzpQZLNRJRjDK10923403 = 50340391;    float mdkzpQZLNRJRjDK67851575 = -737193242;    float mdkzpQZLNRJRjDK69349555 = -674899502;    float mdkzpQZLNRJRjDK58348192 = -392599175;    float mdkzpQZLNRJRjDK43180270 = -906312505;    float mdkzpQZLNRJRjDK64040346 = -255494610;    float mdkzpQZLNRJRjDK31534622 = -525654206;    float mdkzpQZLNRJRjDK69036229 = -407074533;    float mdkzpQZLNRJRjDK54086933 = -998371093;    float mdkzpQZLNRJRjDK84504764 = 71808934;    float mdkzpQZLNRJRjDK38924123 = 4787932;    float mdkzpQZLNRJRjDK39910775 = -337280026;    float mdkzpQZLNRJRjDK38333049 = -961323101;    float mdkzpQZLNRJRjDK4796783 = -659934985;    float mdkzpQZLNRJRjDK43650973 = -478128860;    float mdkzpQZLNRJRjDK82796653 = -573701492;    float mdkzpQZLNRJRjDK37459171 = -143695672;    float mdkzpQZLNRJRjDK68385986 = -950279692;    float mdkzpQZLNRJRjDK26464206 = -355896910;    float mdkzpQZLNRJRjDK41788171 = -76214124;    float mdkzpQZLNRJRjDK96290550 = -62609301;    float mdkzpQZLNRJRjDK92494200 = -591292333;    float mdkzpQZLNRJRjDK90419845 = -672781061;    float mdkzpQZLNRJRjDK9204122 = -714114500;    float mdkzpQZLNRJRjDK25273161 = -417344437;    float mdkzpQZLNRJRjDK66140419 = -123627418;    float mdkzpQZLNRJRjDK30509557 = -183844517;    float mdkzpQZLNRJRjDK84250498 = -349858694;    float mdkzpQZLNRJRjDK37096987 = -135580018;    float mdkzpQZLNRJRjDK51252230 = -462966098;    float mdkzpQZLNRJRjDK34435760 = -710399823;    float mdkzpQZLNRJRjDK96683813 = -290387887;    float mdkzpQZLNRJRjDK12283136 = 34198837;    float mdkzpQZLNRJRjDK25681174 = -490756199;    float mdkzpQZLNRJRjDK98618197 = -926580054;    float mdkzpQZLNRJRjDK84057007 = -937239535;    float mdkzpQZLNRJRjDK81635926 = -282560318;    float mdkzpQZLNRJRjDK98169124 = -843579795;    float mdkzpQZLNRJRjDK43161041 = -792076140;    float mdkzpQZLNRJRjDK40705681 = -157426753;    float mdkzpQZLNRJRjDK20125516 = -747372626;    float mdkzpQZLNRJRjDK50951252 = -965672764;    float mdkzpQZLNRJRjDK65409355 = -6432541;    float mdkzpQZLNRJRjDK73235387 = -59857246;    float mdkzpQZLNRJRjDK52574770 = -22708990;    float mdkzpQZLNRJRjDK23113130 = -489841916;    float mdkzpQZLNRJRjDK36543399 = -802281481;    float mdkzpQZLNRJRjDK35733177 = -624029997;    float mdkzpQZLNRJRjDK14812223 = -794538052;    float mdkzpQZLNRJRjDK9677773 = -247374711;    float mdkzpQZLNRJRjDK6972354 = 44950980;    float mdkzpQZLNRJRjDK68347290 = -840588200;    float mdkzpQZLNRJRjDK39660841 = -955781910;    float mdkzpQZLNRJRjDK21810738 = -568955666;    float mdkzpQZLNRJRjDK42776475 = -723711632;    float mdkzpQZLNRJRjDK18429203 = -358367276;    float mdkzpQZLNRJRjDK77431729 = 35587819;    float mdkzpQZLNRJRjDK60145433 = -960785003;    float mdkzpQZLNRJRjDK33075032 = -975254738;    float mdkzpQZLNRJRjDK77039851 = -682685088;    float mdkzpQZLNRJRjDK33530790 = 28349907;    float mdkzpQZLNRJRjDK47284124 = -75795512;    float mdkzpQZLNRJRjDK31939242 = -171494515;    float mdkzpQZLNRJRjDK2834703 = -435404995;    float mdkzpQZLNRJRjDK50069004 = -217791244;    float mdkzpQZLNRJRjDK42240309 = -704824181;    float mdkzpQZLNRJRjDK27627640 = -271478864;    float mdkzpQZLNRJRjDK12651876 = -370566902;    float mdkzpQZLNRJRjDK6178586 = -733354932;    float mdkzpQZLNRJRjDK59593966 = -540889326;    float mdkzpQZLNRJRjDK1160728 = -191141174;    float mdkzpQZLNRJRjDK39290046 = -300115878;    float mdkzpQZLNRJRjDK25224945 = -58203552;    float mdkzpQZLNRJRjDK85758525 = -98470157;    float mdkzpQZLNRJRjDK21662656 = -328841499;    float mdkzpQZLNRJRjDK45339299 = -96936537;    float mdkzpQZLNRJRjDK27084845 = -484859793;    float mdkzpQZLNRJRjDK17184459 = -512923816;    float mdkzpQZLNRJRjDK56629352 = -591405510;    float mdkzpQZLNRJRjDK2160031 = -927502522;    float mdkzpQZLNRJRjDK29597021 = -321345937;    float mdkzpQZLNRJRjDK94776380 = -559814520;    float mdkzpQZLNRJRjDK69438275 = -555320643;    float mdkzpQZLNRJRjDK27419214 = -888205308;    float mdkzpQZLNRJRjDK44279876 = -407917078;    float mdkzpQZLNRJRjDK66088470 = -869811623;    float mdkzpQZLNRJRjDK57022973 = -334605978;    float mdkzpQZLNRJRjDK90472397 = -396845497;    float mdkzpQZLNRJRjDK82904699 = -117921058;     mdkzpQZLNRJRjDK91498893 = mdkzpQZLNRJRjDK63023904;     mdkzpQZLNRJRjDK63023904 = mdkzpQZLNRJRjDK74876447;     mdkzpQZLNRJRjDK74876447 = mdkzpQZLNRJRjDK40529960;     mdkzpQZLNRJRjDK40529960 = mdkzpQZLNRJRjDK58463195;     mdkzpQZLNRJRjDK58463195 = mdkzpQZLNRJRjDK92474426;     mdkzpQZLNRJRjDK92474426 = mdkzpQZLNRJRjDK44431524;     mdkzpQZLNRJRjDK44431524 = mdkzpQZLNRJRjDK36733276;     mdkzpQZLNRJRjDK36733276 = mdkzpQZLNRJRjDK66125047;     mdkzpQZLNRJRjDK66125047 = mdkzpQZLNRJRjDK63598909;     mdkzpQZLNRJRjDK63598909 = mdkzpQZLNRJRjDK39067026;     mdkzpQZLNRJRjDK39067026 = mdkzpQZLNRJRjDK10923403;     mdkzpQZLNRJRjDK10923403 = mdkzpQZLNRJRjDK67851575;     mdkzpQZLNRJRjDK67851575 = mdkzpQZLNRJRjDK69349555;     mdkzpQZLNRJRjDK69349555 = mdkzpQZLNRJRjDK58348192;     mdkzpQZLNRJRjDK58348192 = mdkzpQZLNRJRjDK43180270;     mdkzpQZLNRJRjDK43180270 = mdkzpQZLNRJRjDK64040346;     mdkzpQZLNRJRjDK64040346 = mdkzpQZLNRJRjDK31534622;     mdkzpQZLNRJRjDK31534622 = mdkzpQZLNRJRjDK69036229;     mdkzpQZLNRJRjDK69036229 = mdkzpQZLNRJRjDK54086933;     mdkzpQZLNRJRjDK54086933 = mdkzpQZLNRJRjDK84504764;     mdkzpQZLNRJRjDK84504764 = mdkzpQZLNRJRjDK38924123;     mdkzpQZLNRJRjDK38924123 = mdkzpQZLNRJRjDK39910775;     mdkzpQZLNRJRjDK39910775 = mdkzpQZLNRJRjDK38333049;     mdkzpQZLNRJRjDK38333049 = mdkzpQZLNRJRjDK4796783;     mdkzpQZLNRJRjDK4796783 = mdkzpQZLNRJRjDK43650973;     mdkzpQZLNRJRjDK43650973 = mdkzpQZLNRJRjDK82796653;     mdkzpQZLNRJRjDK82796653 = mdkzpQZLNRJRjDK37459171;     mdkzpQZLNRJRjDK37459171 = mdkzpQZLNRJRjDK68385986;     mdkzpQZLNRJRjDK68385986 = mdkzpQZLNRJRjDK26464206;     mdkzpQZLNRJRjDK26464206 = mdkzpQZLNRJRjDK41788171;     mdkzpQZLNRJRjDK41788171 = mdkzpQZLNRJRjDK96290550;     mdkzpQZLNRJRjDK96290550 = mdkzpQZLNRJRjDK92494200;     mdkzpQZLNRJRjDK92494200 = mdkzpQZLNRJRjDK90419845;     mdkzpQZLNRJRjDK90419845 = mdkzpQZLNRJRjDK9204122;     mdkzpQZLNRJRjDK9204122 = mdkzpQZLNRJRjDK25273161;     mdkzpQZLNRJRjDK25273161 = mdkzpQZLNRJRjDK66140419;     mdkzpQZLNRJRjDK66140419 = mdkzpQZLNRJRjDK30509557;     mdkzpQZLNRJRjDK30509557 = mdkzpQZLNRJRjDK84250498;     mdkzpQZLNRJRjDK84250498 = mdkzpQZLNRJRjDK37096987;     mdkzpQZLNRJRjDK37096987 = mdkzpQZLNRJRjDK51252230;     mdkzpQZLNRJRjDK51252230 = mdkzpQZLNRJRjDK34435760;     mdkzpQZLNRJRjDK34435760 = mdkzpQZLNRJRjDK96683813;     mdkzpQZLNRJRjDK96683813 = mdkzpQZLNRJRjDK12283136;     mdkzpQZLNRJRjDK12283136 = mdkzpQZLNRJRjDK25681174;     mdkzpQZLNRJRjDK25681174 = mdkzpQZLNRJRjDK98618197;     mdkzpQZLNRJRjDK98618197 = mdkzpQZLNRJRjDK84057007;     mdkzpQZLNRJRjDK84057007 = mdkzpQZLNRJRjDK81635926;     mdkzpQZLNRJRjDK81635926 = mdkzpQZLNRJRjDK98169124;     mdkzpQZLNRJRjDK98169124 = mdkzpQZLNRJRjDK43161041;     mdkzpQZLNRJRjDK43161041 = mdkzpQZLNRJRjDK40705681;     mdkzpQZLNRJRjDK40705681 = mdkzpQZLNRJRjDK20125516;     mdkzpQZLNRJRjDK20125516 = mdkzpQZLNRJRjDK50951252;     mdkzpQZLNRJRjDK50951252 = mdkzpQZLNRJRjDK65409355;     mdkzpQZLNRJRjDK65409355 = mdkzpQZLNRJRjDK73235387;     mdkzpQZLNRJRjDK73235387 = mdkzpQZLNRJRjDK52574770;     mdkzpQZLNRJRjDK52574770 = mdkzpQZLNRJRjDK23113130;     mdkzpQZLNRJRjDK23113130 = mdkzpQZLNRJRjDK36543399;     mdkzpQZLNRJRjDK36543399 = mdkzpQZLNRJRjDK35733177;     mdkzpQZLNRJRjDK35733177 = mdkzpQZLNRJRjDK14812223;     mdkzpQZLNRJRjDK14812223 = mdkzpQZLNRJRjDK9677773;     mdkzpQZLNRJRjDK9677773 = mdkzpQZLNRJRjDK6972354;     mdkzpQZLNRJRjDK6972354 = mdkzpQZLNRJRjDK68347290;     mdkzpQZLNRJRjDK68347290 = mdkzpQZLNRJRjDK39660841;     mdkzpQZLNRJRjDK39660841 = mdkzpQZLNRJRjDK21810738;     mdkzpQZLNRJRjDK21810738 = mdkzpQZLNRJRjDK42776475;     mdkzpQZLNRJRjDK42776475 = mdkzpQZLNRJRjDK18429203;     mdkzpQZLNRJRjDK18429203 = mdkzpQZLNRJRjDK77431729;     mdkzpQZLNRJRjDK77431729 = mdkzpQZLNRJRjDK60145433;     mdkzpQZLNRJRjDK60145433 = mdkzpQZLNRJRjDK33075032;     mdkzpQZLNRJRjDK33075032 = mdkzpQZLNRJRjDK77039851;     mdkzpQZLNRJRjDK77039851 = mdkzpQZLNRJRjDK33530790;     mdkzpQZLNRJRjDK33530790 = mdkzpQZLNRJRjDK47284124;     mdkzpQZLNRJRjDK47284124 = mdkzpQZLNRJRjDK31939242;     mdkzpQZLNRJRjDK31939242 = mdkzpQZLNRJRjDK2834703;     mdkzpQZLNRJRjDK2834703 = mdkzpQZLNRJRjDK50069004;     mdkzpQZLNRJRjDK50069004 = mdkzpQZLNRJRjDK42240309;     mdkzpQZLNRJRjDK42240309 = mdkzpQZLNRJRjDK27627640;     mdkzpQZLNRJRjDK27627640 = mdkzpQZLNRJRjDK12651876;     mdkzpQZLNRJRjDK12651876 = mdkzpQZLNRJRjDK6178586;     mdkzpQZLNRJRjDK6178586 = mdkzpQZLNRJRjDK59593966;     mdkzpQZLNRJRjDK59593966 = mdkzpQZLNRJRjDK1160728;     mdkzpQZLNRJRjDK1160728 = mdkzpQZLNRJRjDK39290046;     mdkzpQZLNRJRjDK39290046 = mdkzpQZLNRJRjDK25224945;     mdkzpQZLNRJRjDK25224945 = mdkzpQZLNRJRjDK85758525;     mdkzpQZLNRJRjDK85758525 = mdkzpQZLNRJRjDK21662656;     mdkzpQZLNRJRjDK21662656 = mdkzpQZLNRJRjDK45339299;     mdkzpQZLNRJRjDK45339299 = mdkzpQZLNRJRjDK27084845;     mdkzpQZLNRJRjDK27084845 = mdkzpQZLNRJRjDK17184459;     mdkzpQZLNRJRjDK17184459 = mdkzpQZLNRJRjDK56629352;     mdkzpQZLNRJRjDK56629352 = mdkzpQZLNRJRjDK2160031;     mdkzpQZLNRJRjDK2160031 = mdkzpQZLNRJRjDK29597021;     mdkzpQZLNRJRjDK29597021 = mdkzpQZLNRJRjDK94776380;     mdkzpQZLNRJRjDK94776380 = mdkzpQZLNRJRjDK69438275;     mdkzpQZLNRJRjDK69438275 = mdkzpQZLNRJRjDK27419214;     mdkzpQZLNRJRjDK27419214 = mdkzpQZLNRJRjDK44279876;     mdkzpQZLNRJRjDK44279876 = mdkzpQZLNRJRjDK66088470;     mdkzpQZLNRJRjDK66088470 = mdkzpQZLNRJRjDK57022973;     mdkzpQZLNRJRjDK57022973 = mdkzpQZLNRJRjDK90472397;     mdkzpQZLNRJRjDK90472397 = mdkzpQZLNRJRjDK82904699;     mdkzpQZLNRJRjDK82904699 = mdkzpQZLNRJRjDK91498893;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void auIVywNPLPnnQBhg64398896() {     float kxObHBYRDXetLkc91492421 = 9554801;    float kxObHBYRDXetLkc1596221 = -969717067;    float kxObHBYRDXetLkc76189659 = -720733195;    float kxObHBYRDXetLkc25609820 = -104505812;    float kxObHBYRDXetLkc67303298 = -741022233;    float kxObHBYRDXetLkc27279859 = -620509014;    float kxObHBYRDXetLkc12978861 = 96718841;    float kxObHBYRDXetLkc5947327 = -495045209;    float kxObHBYRDXetLkc48865211 = -25324890;    float kxObHBYRDXetLkc60238422 = -381976661;    float kxObHBYRDXetLkc59387631 = -845793005;    float kxObHBYRDXetLkc25215080 = -785137972;    float kxObHBYRDXetLkc99704489 = -758851717;    float kxObHBYRDXetLkc23193776 = 79947446;    float kxObHBYRDXetLkc3520302 = -355312027;    float kxObHBYRDXetLkc17744818 = -139108155;    float kxObHBYRDXetLkc19893106 = -75109179;    float kxObHBYRDXetLkc54535155 = -922725827;    float kxObHBYRDXetLkc89464498 = 74943200;    float kxObHBYRDXetLkc26160696 = -510395544;    float kxObHBYRDXetLkc64174188 = -738657440;    float kxObHBYRDXetLkc17220389 = -505203951;    float kxObHBYRDXetLkc76789171 = -796237868;    float kxObHBYRDXetLkc81983157 = -842243624;    float kxObHBYRDXetLkc33239473 = 13371162;    float kxObHBYRDXetLkc60225050 = -386334789;    float kxObHBYRDXetLkc53018235 = -874501205;    float kxObHBYRDXetLkc72862695 = -752662661;    float kxObHBYRDXetLkc81064433 = -496350007;    float kxObHBYRDXetLkc2554077 = -31328284;    float kxObHBYRDXetLkc75200849 = -463307359;    float kxObHBYRDXetLkc81014408 = -454431062;    float kxObHBYRDXetLkc77946623 = -793236549;    float kxObHBYRDXetLkc90702683 = -549462794;    float kxObHBYRDXetLkc91562159 = -973428425;    float kxObHBYRDXetLkc68101350 = -497110183;    float kxObHBYRDXetLkc78081787 = -223280796;    float kxObHBYRDXetLkc427237 = -179107196;    float kxObHBYRDXetLkc19564095 = 37459890;    float kxObHBYRDXetLkc8969880 = -845011705;    float kxObHBYRDXetLkc39107898 = -550909996;    float kxObHBYRDXetLkc20133421 = -904191905;    float kxObHBYRDXetLkc56967174 = -849518142;    float kxObHBYRDXetLkc71515033 = -622765607;    float kxObHBYRDXetLkc9699226 = -964015601;    float kxObHBYRDXetLkc39046166 = -738624725;    float kxObHBYRDXetLkc11754800 = -318520157;    float kxObHBYRDXetLkc57019507 = -891321674;    float kxObHBYRDXetLkc16827515 = -407343656;    float kxObHBYRDXetLkc63206464 = -379101026;    float kxObHBYRDXetLkc53965050 = -835981761;    float kxObHBYRDXetLkc34672773 = -29518374;    float kxObHBYRDXetLkc26399601 = -857726772;    float kxObHBYRDXetLkc75659626 = -538352392;    float kxObHBYRDXetLkc224708 = -916756524;    float kxObHBYRDXetLkc74272033 = -485241249;    float kxObHBYRDXetLkc24807050 = -73479199;    float kxObHBYRDXetLkc94206502 = -878489572;    float kxObHBYRDXetLkc92370347 = -17876974;    float kxObHBYRDXetLkc7078248 = -254687445;    float kxObHBYRDXetLkc74261623 = -746007810;    float kxObHBYRDXetLkc40116165 = -150618499;    float kxObHBYRDXetLkc24882894 = -998695203;    float kxObHBYRDXetLkc46311135 = -993996606;    float kxObHBYRDXetLkc85037573 = -918669303;    float kxObHBYRDXetLkc78373222 = -291361943;    float kxObHBYRDXetLkc47268456 = -991901423;    float kxObHBYRDXetLkc9001806 = -109388923;    float kxObHBYRDXetLkc31631616 = 53375870;    float kxObHBYRDXetLkc35418952 = -858201844;    float kxObHBYRDXetLkc39663031 = -915827360;    float kxObHBYRDXetLkc19465870 = -896001984;    float kxObHBYRDXetLkc34971060 = -860185718;    float kxObHBYRDXetLkc80494619 = -80045096;    float kxObHBYRDXetLkc87052798 = -959485548;    float kxObHBYRDXetLkc44040767 = -834465535;    float kxObHBYRDXetLkc60253215 = -655685810;    float kxObHBYRDXetLkc5274138 = -73472262;    float kxObHBYRDXetLkc72283931 = -878228023;    float kxObHBYRDXetLkc94193307 = -248004114;    float kxObHBYRDXetLkc48470250 = 32185368;    float kxObHBYRDXetLkc95998728 = -983179531;    float kxObHBYRDXetLkc56035181 = -245319005;    float kxObHBYRDXetLkc17857969 = -17248981;    float kxObHBYRDXetLkc48589026 = -195346524;    float kxObHBYRDXetLkc40528077 = -333788985;    float kxObHBYRDXetLkc54614808 = -596704291;    float kxObHBYRDXetLkc2286998 = -154884158;    float kxObHBYRDXetLkc90477975 = -632706270;    float kxObHBYRDXetLkc17290127 = -388187177;    float kxObHBYRDXetLkc43294301 = -323630985;    float kxObHBYRDXetLkc83875284 = -344791224;    float kxObHBYRDXetLkc8056889 = -61230222;    float kxObHBYRDXetLkc12485847 = -707852666;    float kxObHBYRDXetLkc34708256 = 996105;    float kxObHBYRDXetLkc98991732 = -300291498;    float kxObHBYRDXetLkc95250527 = -905496703;    float kxObHBYRDXetLkc10656039 = -855521536;    float kxObHBYRDXetLkc86477460 = -704096304;    float kxObHBYRDXetLkc31326004 = 9554801;     kxObHBYRDXetLkc91492421 = kxObHBYRDXetLkc1596221;     kxObHBYRDXetLkc1596221 = kxObHBYRDXetLkc76189659;     kxObHBYRDXetLkc76189659 = kxObHBYRDXetLkc25609820;     kxObHBYRDXetLkc25609820 = kxObHBYRDXetLkc67303298;     kxObHBYRDXetLkc67303298 = kxObHBYRDXetLkc27279859;     kxObHBYRDXetLkc27279859 = kxObHBYRDXetLkc12978861;     kxObHBYRDXetLkc12978861 = kxObHBYRDXetLkc5947327;     kxObHBYRDXetLkc5947327 = kxObHBYRDXetLkc48865211;     kxObHBYRDXetLkc48865211 = kxObHBYRDXetLkc60238422;     kxObHBYRDXetLkc60238422 = kxObHBYRDXetLkc59387631;     kxObHBYRDXetLkc59387631 = kxObHBYRDXetLkc25215080;     kxObHBYRDXetLkc25215080 = kxObHBYRDXetLkc99704489;     kxObHBYRDXetLkc99704489 = kxObHBYRDXetLkc23193776;     kxObHBYRDXetLkc23193776 = kxObHBYRDXetLkc3520302;     kxObHBYRDXetLkc3520302 = kxObHBYRDXetLkc17744818;     kxObHBYRDXetLkc17744818 = kxObHBYRDXetLkc19893106;     kxObHBYRDXetLkc19893106 = kxObHBYRDXetLkc54535155;     kxObHBYRDXetLkc54535155 = kxObHBYRDXetLkc89464498;     kxObHBYRDXetLkc89464498 = kxObHBYRDXetLkc26160696;     kxObHBYRDXetLkc26160696 = kxObHBYRDXetLkc64174188;     kxObHBYRDXetLkc64174188 = kxObHBYRDXetLkc17220389;     kxObHBYRDXetLkc17220389 = kxObHBYRDXetLkc76789171;     kxObHBYRDXetLkc76789171 = kxObHBYRDXetLkc81983157;     kxObHBYRDXetLkc81983157 = kxObHBYRDXetLkc33239473;     kxObHBYRDXetLkc33239473 = kxObHBYRDXetLkc60225050;     kxObHBYRDXetLkc60225050 = kxObHBYRDXetLkc53018235;     kxObHBYRDXetLkc53018235 = kxObHBYRDXetLkc72862695;     kxObHBYRDXetLkc72862695 = kxObHBYRDXetLkc81064433;     kxObHBYRDXetLkc81064433 = kxObHBYRDXetLkc2554077;     kxObHBYRDXetLkc2554077 = kxObHBYRDXetLkc75200849;     kxObHBYRDXetLkc75200849 = kxObHBYRDXetLkc81014408;     kxObHBYRDXetLkc81014408 = kxObHBYRDXetLkc77946623;     kxObHBYRDXetLkc77946623 = kxObHBYRDXetLkc90702683;     kxObHBYRDXetLkc90702683 = kxObHBYRDXetLkc91562159;     kxObHBYRDXetLkc91562159 = kxObHBYRDXetLkc68101350;     kxObHBYRDXetLkc68101350 = kxObHBYRDXetLkc78081787;     kxObHBYRDXetLkc78081787 = kxObHBYRDXetLkc427237;     kxObHBYRDXetLkc427237 = kxObHBYRDXetLkc19564095;     kxObHBYRDXetLkc19564095 = kxObHBYRDXetLkc8969880;     kxObHBYRDXetLkc8969880 = kxObHBYRDXetLkc39107898;     kxObHBYRDXetLkc39107898 = kxObHBYRDXetLkc20133421;     kxObHBYRDXetLkc20133421 = kxObHBYRDXetLkc56967174;     kxObHBYRDXetLkc56967174 = kxObHBYRDXetLkc71515033;     kxObHBYRDXetLkc71515033 = kxObHBYRDXetLkc9699226;     kxObHBYRDXetLkc9699226 = kxObHBYRDXetLkc39046166;     kxObHBYRDXetLkc39046166 = kxObHBYRDXetLkc11754800;     kxObHBYRDXetLkc11754800 = kxObHBYRDXetLkc57019507;     kxObHBYRDXetLkc57019507 = kxObHBYRDXetLkc16827515;     kxObHBYRDXetLkc16827515 = kxObHBYRDXetLkc63206464;     kxObHBYRDXetLkc63206464 = kxObHBYRDXetLkc53965050;     kxObHBYRDXetLkc53965050 = kxObHBYRDXetLkc34672773;     kxObHBYRDXetLkc34672773 = kxObHBYRDXetLkc26399601;     kxObHBYRDXetLkc26399601 = kxObHBYRDXetLkc75659626;     kxObHBYRDXetLkc75659626 = kxObHBYRDXetLkc224708;     kxObHBYRDXetLkc224708 = kxObHBYRDXetLkc74272033;     kxObHBYRDXetLkc74272033 = kxObHBYRDXetLkc24807050;     kxObHBYRDXetLkc24807050 = kxObHBYRDXetLkc94206502;     kxObHBYRDXetLkc94206502 = kxObHBYRDXetLkc92370347;     kxObHBYRDXetLkc92370347 = kxObHBYRDXetLkc7078248;     kxObHBYRDXetLkc7078248 = kxObHBYRDXetLkc74261623;     kxObHBYRDXetLkc74261623 = kxObHBYRDXetLkc40116165;     kxObHBYRDXetLkc40116165 = kxObHBYRDXetLkc24882894;     kxObHBYRDXetLkc24882894 = kxObHBYRDXetLkc46311135;     kxObHBYRDXetLkc46311135 = kxObHBYRDXetLkc85037573;     kxObHBYRDXetLkc85037573 = kxObHBYRDXetLkc78373222;     kxObHBYRDXetLkc78373222 = kxObHBYRDXetLkc47268456;     kxObHBYRDXetLkc47268456 = kxObHBYRDXetLkc9001806;     kxObHBYRDXetLkc9001806 = kxObHBYRDXetLkc31631616;     kxObHBYRDXetLkc31631616 = kxObHBYRDXetLkc35418952;     kxObHBYRDXetLkc35418952 = kxObHBYRDXetLkc39663031;     kxObHBYRDXetLkc39663031 = kxObHBYRDXetLkc19465870;     kxObHBYRDXetLkc19465870 = kxObHBYRDXetLkc34971060;     kxObHBYRDXetLkc34971060 = kxObHBYRDXetLkc80494619;     kxObHBYRDXetLkc80494619 = kxObHBYRDXetLkc87052798;     kxObHBYRDXetLkc87052798 = kxObHBYRDXetLkc44040767;     kxObHBYRDXetLkc44040767 = kxObHBYRDXetLkc60253215;     kxObHBYRDXetLkc60253215 = kxObHBYRDXetLkc5274138;     kxObHBYRDXetLkc5274138 = kxObHBYRDXetLkc72283931;     kxObHBYRDXetLkc72283931 = kxObHBYRDXetLkc94193307;     kxObHBYRDXetLkc94193307 = kxObHBYRDXetLkc48470250;     kxObHBYRDXetLkc48470250 = kxObHBYRDXetLkc95998728;     kxObHBYRDXetLkc95998728 = kxObHBYRDXetLkc56035181;     kxObHBYRDXetLkc56035181 = kxObHBYRDXetLkc17857969;     kxObHBYRDXetLkc17857969 = kxObHBYRDXetLkc48589026;     kxObHBYRDXetLkc48589026 = kxObHBYRDXetLkc40528077;     kxObHBYRDXetLkc40528077 = kxObHBYRDXetLkc54614808;     kxObHBYRDXetLkc54614808 = kxObHBYRDXetLkc2286998;     kxObHBYRDXetLkc2286998 = kxObHBYRDXetLkc90477975;     kxObHBYRDXetLkc90477975 = kxObHBYRDXetLkc17290127;     kxObHBYRDXetLkc17290127 = kxObHBYRDXetLkc43294301;     kxObHBYRDXetLkc43294301 = kxObHBYRDXetLkc83875284;     kxObHBYRDXetLkc83875284 = kxObHBYRDXetLkc8056889;     kxObHBYRDXetLkc8056889 = kxObHBYRDXetLkc12485847;     kxObHBYRDXetLkc12485847 = kxObHBYRDXetLkc34708256;     kxObHBYRDXetLkc34708256 = kxObHBYRDXetLkc98991732;     kxObHBYRDXetLkc98991732 = kxObHBYRDXetLkc95250527;     kxObHBYRDXetLkc95250527 = kxObHBYRDXetLkc10656039;     kxObHBYRDXetLkc10656039 = kxObHBYRDXetLkc86477460;     kxObHBYRDXetLkc86477460 = kxObHBYRDXetLkc31326004;     kxObHBYRDXetLkc31326004 = kxObHBYRDXetLkc91492421;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ucVCKfvaxdHMOhOJ18492740() {     float djFrCULXBMoMPrk44406516 = 58888944;    float djFrCULXBMoMPrk36849095 = -404328469;    float djFrCULXBMoMPrk14106856 = -451604885;    float djFrCULXBMoMPrk10779256 = -984744703;    float djFrCULXBMoMPrk71661112 = -507073310;    float djFrCULXBMoMPrk42674976 = -365207232;    float djFrCULXBMoMPrk86713222 = -435364585;    float djFrCULXBMoMPrk18241490 = -99966875;    float djFrCULXBMoMPrk97396667 = -891564102;    float djFrCULXBMoMPrk71552919 = -121737101;    float djFrCULXBMoMPrk57946681 = -929031159;    float djFrCULXBMoMPrk88342173 = -288189332;    float djFrCULXBMoMPrk50230696 = -144455615;    float djFrCULXBMoMPrk22775653 = -714340374;    float djFrCULXBMoMPrk63608199 = -756740587;    float djFrCULXBMoMPrk76708491 = -32180502;    float djFrCULXBMoMPrk5643915 = -595325245;    float djFrCULXBMoMPrk78699219 = 3482484;    float djFrCULXBMoMPrk71432453 = -696440865;    float djFrCULXBMoMPrk28121000 = 45488120;    float djFrCULXBMoMPrk12353292 = -456782203;    float djFrCULXBMoMPrk99775723 = -55660964;    float djFrCULXBMoMPrk39568286 = -193962224;    float djFrCULXBMoMPrk73498857 = -30382881;    float djFrCULXBMoMPrk3908248 = -458908243;    float djFrCULXBMoMPrk31191141 = -525588240;    float djFrCULXBMoMPrk93845711 = -92536550;    float djFrCULXBMoMPrk54535114 = -284109076;    float djFrCULXBMoMPrk5720873 = -486248716;    float djFrCULXBMoMPrk9071804 = -716758987;    float djFrCULXBMoMPrk14747079 = -419617344;    float djFrCULXBMoMPrk87519181 = -286019720;    float djFrCULXBMoMPrk61598508 = -93830439;    float djFrCULXBMoMPrk91546991 = -751497737;    float djFrCULXBMoMPrk39629717 = -68967485;    float djFrCULXBMoMPrk5849425 = -293761261;    float djFrCULXBMoMPrk37094074 = -879092706;    float djFrCULXBMoMPrk15009123 = 34029138;    float djFrCULXBMoMPrk77044098 = -158682379;    float djFrCULXBMoMPrk86778527 = -966778402;    float djFrCULXBMoMPrk17274963 = -885259291;    float djFrCULXBMoMPrk86055924 = -568548432;    float djFrCULXBMoMPrk61596645 = -658594618;    float djFrCULXBMoMPrk6997538 = -938779927;    float djFrCULXBMoMPrk10667621 = -25980667;    float djFrCULXBMoMPrk23006297 = -517555902;    float djFrCULXBMoMPrk12835662 = -794197553;    float djFrCULXBMoMPrk33000037 = -96140368;    float djFrCULXBMoMPrk59145620 = -840944012;    float djFrCULXBMoMPrk84425934 = -450377605;    float djFrCULXBMoMPrk48343225 = -468609362;    float djFrCULXBMoMPrk52985613 = -94594108;    float djFrCULXBMoMPrk66475118 = -497569548;    float djFrCULXBMoMPrk20703846 = -393379453;    float djFrCULXBMoMPrk6139449 = -576463;    float djFrCULXBMoMPrk44630792 = -885450093;    float djFrCULXBMoMPrk97280808 = -110366246;    float djFrCULXBMoMPrk40607999 = -321222004;    float djFrCULXBMoMPrk6871008 = -425836461;    float djFrCULXBMoMPrk40469971 = -981485070;    float djFrCULXBMoMPrk48829264 = -172670682;    float djFrCULXBMoMPrk32178108 = -51255509;    float djFrCULXBMoMPrk12520618 = -613718159;    float djFrCULXBMoMPrk88324863 = -74805116;    float djFrCULXBMoMPrk56805840 = -702119758;    float djFrCULXBMoMPrk70427499 = -543011440;    float djFrCULXBMoMPrk26743665 = -94358893;    float djFrCULXBMoMPrk58683704 = -392957879;    float djFrCULXBMoMPrk83145936 = -545372890;    float djFrCULXBMoMPrk57758775 = -362979327;    float djFrCULXBMoMPrk39614418 = -153087796;    float djFrCULXBMoMPrk90634791 = -529354384;    float djFrCULXBMoMPrk1655121 = -837835138;    float djFrCULXBMoMPrk84653925 = -729662463;    float djFrCULXBMoMPrk10846037 = -69252590;    float djFrCULXBMoMPrk26297367 = -888233772;    float djFrCULXBMoMPrk38179079 = -397066346;    float djFrCULXBMoMPrk32570748 = -255182297;    float djFrCULXBMoMPrk62831236 = 95597785;    float djFrCULXBMoMPrk80901951 = -941352342;    float djFrCULXBMoMPrk18355479 = -731390687;    float djFrCULXBMoMPrk60845674 = -996396183;    float djFrCULXBMoMPrk95389494 = -443165065;    float djFrCULXBMoMPrk21294938 = 64128888;    float djFrCULXBMoMPrk60728579 = -148149625;    float djFrCULXBMoMPrk61761466 = -225023236;    float djFrCULXBMoMPrk21044063 = -788450173;    float djFrCULXBMoMPrk40894662 = -700450987;    float djFrCULXBMoMPrk85407543 = -650921275;    float djFrCULXBMoMPrk94998925 = -183517392;    float djFrCULXBMoMPrk8568616 = -83395015;    float djFrCULXBMoMPrk96486075 = -457870702;    float djFrCULXBMoMPrk8138115 = -540134401;    float djFrCULXBMoMPrk36574128 = -177197310;    float djFrCULXBMoMPrk37949263 = -694107720;    float djFrCULXBMoMPrk85096854 = -734003782;    float djFrCULXBMoMPrk73535307 = -954830273;    float djFrCULXBMoMPrk73271781 = -483789503;    float djFrCULXBMoMPrk50191697 = -136660170;    float djFrCULXBMoMPrk40240121 = 58888944;     djFrCULXBMoMPrk44406516 = djFrCULXBMoMPrk36849095;     djFrCULXBMoMPrk36849095 = djFrCULXBMoMPrk14106856;     djFrCULXBMoMPrk14106856 = djFrCULXBMoMPrk10779256;     djFrCULXBMoMPrk10779256 = djFrCULXBMoMPrk71661112;     djFrCULXBMoMPrk71661112 = djFrCULXBMoMPrk42674976;     djFrCULXBMoMPrk42674976 = djFrCULXBMoMPrk86713222;     djFrCULXBMoMPrk86713222 = djFrCULXBMoMPrk18241490;     djFrCULXBMoMPrk18241490 = djFrCULXBMoMPrk97396667;     djFrCULXBMoMPrk97396667 = djFrCULXBMoMPrk71552919;     djFrCULXBMoMPrk71552919 = djFrCULXBMoMPrk57946681;     djFrCULXBMoMPrk57946681 = djFrCULXBMoMPrk88342173;     djFrCULXBMoMPrk88342173 = djFrCULXBMoMPrk50230696;     djFrCULXBMoMPrk50230696 = djFrCULXBMoMPrk22775653;     djFrCULXBMoMPrk22775653 = djFrCULXBMoMPrk63608199;     djFrCULXBMoMPrk63608199 = djFrCULXBMoMPrk76708491;     djFrCULXBMoMPrk76708491 = djFrCULXBMoMPrk5643915;     djFrCULXBMoMPrk5643915 = djFrCULXBMoMPrk78699219;     djFrCULXBMoMPrk78699219 = djFrCULXBMoMPrk71432453;     djFrCULXBMoMPrk71432453 = djFrCULXBMoMPrk28121000;     djFrCULXBMoMPrk28121000 = djFrCULXBMoMPrk12353292;     djFrCULXBMoMPrk12353292 = djFrCULXBMoMPrk99775723;     djFrCULXBMoMPrk99775723 = djFrCULXBMoMPrk39568286;     djFrCULXBMoMPrk39568286 = djFrCULXBMoMPrk73498857;     djFrCULXBMoMPrk73498857 = djFrCULXBMoMPrk3908248;     djFrCULXBMoMPrk3908248 = djFrCULXBMoMPrk31191141;     djFrCULXBMoMPrk31191141 = djFrCULXBMoMPrk93845711;     djFrCULXBMoMPrk93845711 = djFrCULXBMoMPrk54535114;     djFrCULXBMoMPrk54535114 = djFrCULXBMoMPrk5720873;     djFrCULXBMoMPrk5720873 = djFrCULXBMoMPrk9071804;     djFrCULXBMoMPrk9071804 = djFrCULXBMoMPrk14747079;     djFrCULXBMoMPrk14747079 = djFrCULXBMoMPrk87519181;     djFrCULXBMoMPrk87519181 = djFrCULXBMoMPrk61598508;     djFrCULXBMoMPrk61598508 = djFrCULXBMoMPrk91546991;     djFrCULXBMoMPrk91546991 = djFrCULXBMoMPrk39629717;     djFrCULXBMoMPrk39629717 = djFrCULXBMoMPrk5849425;     djFrCULXBMoMPrk5849425 = djFrCULXBMoMPrk37094074;     djFrCULXBMoMPrk37094074 = djFrCULXBMoMPrk15009123;     djFrCULXBMoMPrk15009123 = djFrCULXBMoMPrk77044098;     djFrCULXBMoMPrk77044098 = djFrCULXBMoMPrk86778527;     djFrCULXBMoMPrk86778527 = djFrCULXBMoMPrk17274963;     djFrCULXBMoMPrk17274963 = djFrCULXBMoMPrk86055924;     djFrCULXBMoMPrk86055924 = djFrCULXBMoMPrk61596645;     djFrCULXBMoMPrk61596645 = djFrCULXBMoMPrk6997538;     djFrCULXBMoMPrk6997538 = djFrCULXBMoMPrk10667621;     djFrCULXBMoMPrk10667621 = djFrCULXBMoMPrk23006297;     djFrCULXBMoMPrk23006297 = djFrCULXBMoMPrk12835662;     djFrCULXBMoMPrk12835662 = djFrCULXBMoMPrk33000037;     djFrCULXBMoMPrk33000037 = djFrCULXBMoMPrk59145620;     djFrCULXBMoMPrk59145620 = djFrCULXBMoMPrk84425934;     djFrCULXBMoMPrk84425934 = djFrCULXBMoMPrk48343225;     djFrCULXBMoMPrk48343225 = djFrCULXBMoMPrk52985613;     djFrCULXBMoMPrk52985613 = djFrCULXBMoMPrk66475118;     djFrCULXBMoMPrk66475118 = djFrCULXBMoMPrk20703846;     djFrCULXBMoMPrk20703846 = djFrCULXBMoMPrk6139449;     djFrCULXBMoMPrk6139449 = djFrCULXBMoMPrk44630792;     djFrCULXBMoMPrk44630792 = djFrCULXBMoMPrk97280808;     djFrCULXBMoMPrk97280808 = djFrCULXBMoMPrk40607999;     djFrCULXBMoMPrk40607999 = djFrCULXBMoMPrk6871008;     djFrCULXBMoMPrk6871008 = djFrCULXBMoMPrk40469971;     djFrCULXBMoMPrk40469971 = djFrCULXBMoMPrk48829264;     djFrCULXBMoMPrk48829264 = djFrCULXBMoMPrk32178108;     djFrCULXBMoMPrk32178108 = djFrCULXBMoMPrk12520618;     djFrCULXBMoMPrk12520618 = djFrCULXBMoMPrk88324863;     djFrCULXBMoMPrk88324863 = djFrCULXBMoMPrk56805840;     djFrCULXBMoMPrk56805840 = djFrCULXBMoMPrk70427499;     djFrCULXBMoMPrk70427499 = djFrCULXBMoMPrk26743665;     djFrCULXBMoMPrk26743665 = djFrCULXBMoMPrk58683704;     djFrCULXBMoMPrk58683704 = djFrCULXBMoMPrk83145936;     djFrCULXBMoMPrk83145936 = djFrCULXBMoMPrk57758775;     djFrCULXBMoMPrk57758775 = djFrCULXBMoMPrk39614418;     djFrCULXBMoMPrk39614418 = djFrCULXBMoMPrk90634791;     djFrCULXBMoMPrk90634791 = djFrCULXBMoMPrk1655121;     djFrCULXBMoMPrk1655121 = djFrCULXBMoMPrk84653925;     djFrCULXBMoMPrk84653925 = djFrCULXBMoMPrk10846037;     djFrCULXBMoMPrk10846037 = djFrCULXBMoMPrk26297367;     djFrCULXBMoMPrk26297367 = djFrCULXBMoMPrk38179079;     djFrCULXBMoMPrk38179079 = djFrCULXBMoMPrk32570748;     djFrCULXBMoMPrk32570748 = djFrCULXBMoMPrk62831236;     djFrCULXBMoMPrk62831236 = djFrCULXBMoMPrk80901951;     djFrCULXBMoMPrk80901951 = djFrCULXBMoMPrk18355479;     djFrCULXBMoMPrk18355479 = djFrCULXBMoMPrk60845674;     djFrCULXBMoMPrk60845674 = djFrCULXBMoMPrk95389494;     djFrCULXBMoMPrk95389494 = djFrCULXBMoMPrk21294938;     djFrCULXBMoMPrk21294938 = djFrCULXBMoMPrk60728579;     djFrCULXBMoMPrk60728579 = djFrCULXBMoMPrk61761466;     djFrCULXBMoMPrk61761466 = djFrCULXBMoMPrk21044063;     djFrCULXBMoMPrk21044063 = djFrCULXBMoMPrk40894662;     djFrCULXBMoMPrk40894662 = djFrCULXBMoMPrk85407543;     djFrCULXBMoMPrk85407543 = djFrCULXBMoMPrk94998925;     djFrCULXBMoMPrk94998925 = djFrCULXBMoMPrk8568616;     djFrCULXBMoMPrk8568616 = djFrCULXBMoMPrk96486075;     djFrCULXBMoMPrk96486075 = djFrCULXBMoMPrk8138115;     djFrCULXBMoMPrk8138115 = djFrCULXBMoMPrk36574128;     djFrCULXBMoMPrk36574128 = djFrCULXBMoMPrk37949263;     djFrCULXBMoMPrk37949263 = djFrCULXBMoMPrk85096854;     djFrCULXBMoMPrk85096854 = djFrCULXBMoMPrk73535307;     djFrCULXBMoMPrk73535307 = djFrCULXBMoMPrk73271781;     djFrCULXBMoMPrk73271781 = djFrCULXBMoMPrk50191697;     djFrCULXBMoMPrk50191697 = djFrCULXBMoMPrk40240121;     djFrCULXBMoMPrk40240121 = djFrCULXBMoMPrk44406516;}
// Junk Finished
