#include <fstream>
#include <ShlObj.h>

#include "json/json.h"

#include "Config.h"

Config::Config(const char* name) noexcept
{
    if (PWSTR pathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pathToDocuments))) {
        path = pathToDocuments;
        path /= name;
        CoTaskMemFree(pathToDocuments);
    }

    listConfigs();
}

void Config::load(size_t id) noexcept
{
    Json::Value json;

    if (std::ifstream in{ path / (const char8_t*)configs[id].c_str() }; in.good())
        in >> json;
    else
        return;

    for (size_t i = 0; i < aimbot.size(); i++) {
        const auto& aimbotJson = json["Aimbot"][i];
        auto& aimbotConfig = aimbot[i];

        if (aimbotJson.isMember("Enabled")) aimbotConfig.enabled = aimbotJson["Enabled"].asBool();
        if (aimbotJson.isMember("On key")) aimbotConfig.onKey = aimbotJson["On key"].asBool();
        if (aimbotJson.isMember("Key")) aimbotConfig.key = aimbotJson["Key"].asInt();
        if (aimbotJson.isMember("Key mode")) aimbotConfig.keyMode = aimbotJson["Key mode"].asInt();
        if (aimbotJson.isMember("Aimlock")) aimbotConfig.aimlock = aimbotJson["Aimlock"].asBool();
        if (aimbotJson.isMember("Silent")) aimbotConfig.silent = aimbotJson["Silent"].asBool();
        if (aimbotJson.isMember("Friendly fire")) aimbotConfig.friendlyFire = aimbotJson["Friendly fire"].asBool();
        if (aimbotJson.isMember("Visible only")) aimbotConfig.visibleOnly = aimbotJson["Visible only"].asBool();
        if (aimbotJson.isMember("Scoped only")) aimbotConfig.scopedOnly = aimbotJson["Scoped only"].asBool();
        if (aimbotJson.isMember("Ignore flash")) aimbotConfig.ignoreFlash = aimbotJson["Ignore flash"].asBool();
        if (aimbotJson.isMember("Ignore smoke")) aimbotConfig.ignoreSmoke = aimbotJson["Ignore smoke"].asBool();
        if (aimbotJson.isMember("Auto shot")) aimbotConfig.autoShot = aimbotJson["Auto shot"].asBool();
        if (aimbotJson.isMember("Auto scope")) aimbotConfig.autoScope = aimbotJson["Auto scope"].asBool();
        if (aimbotJson.isMember("Fov")) aimbotConfig.fov = aimbotJson["Fov"].asFloat();
        if (aimbotJson.isMember("Smooth")) aimbotConfig.smooth = aimbotJson["Smooth"].asFloat();
        if (aimbotJson.isMember("Bone")) aimbotConfig.bone = aimbotJson["Bone"].asInt();
        if (aimbotJson.isMember("Recoil control X")) aimbotConfig.recoilControlX = aimbotJson["Recoil control X"].asFloat();
        if (aimbotJson.isMember("Recoil control Y")) aimbotConfig.recoilControlY = aimbotJson["Recoil control Y"].asFloat();
        if (aimbotJson.isMember("Hitchance")) aimbotConfig.hitchance = aimbotJson["Hitchance"].asInt();
        if (aimbotJson.isMember("Min damage")) aimbotConfig.minDamage = aimbotJson["Min damage"].asInt();
        if (aimbotJson.isMember("First Shot Delay")) aimbotConfig.firstShotDelay = aimbotJson["First Shot Delay"].asInt();
        if (aimbotJson.isMember("Kill Delay")) aimbotConfig.killDelay = aimbotJson["Kill Delay"].asInt();
        if (aimbotJson.isMember("Killshot")) aimbotConfig.killshot = aimbotJson["Killshot"].asBool();
        if (aimbotJson.isMember("Between shots")) aimbotConfig.betweenShots = aimbotJson["Between shots"].asBool();
        if (aimbotJson.isMember("Standalone RCS")) aimbotConfig.standaloneRCS = aimbotJson["Standalone RCS"].asBool();
        if (aimbotJson.isMember("Standalone RCS Ignore Shots")) aimbotConfig.shotsFired = aimbotJson["Standalone RCS Ignore Shots"].asInt();
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        const auto& triggerbotJson = json["Triggerbot"][i];
        auto& triggerbotConfig = triggerbot[i];

        if (triggerbotJson.isMember("Enabled")) triggerbotConfig.enabled = triggerbotJson["Enabled"].asBool();
        if (triggerbotJson.isMember("On key")) triggerbotConfig.onKey = triggerbotJson["On key"].asBool();
        if (triggerbotJson.isMember("Key")) triggerbotConfig.key = triggerbotJson["Key"].asInt();
        if (triggerbotJson.isMember("Friendly fire")) triggerbotConfig.friendlyFire = triggerbotJson["Friendly fire"].asBool();
        if (triggerbotJson.isMember("Scoped only")) triggerbotConfig.scopedOnly = triggerbotJson["Scoped only"].asBool();
        if (triggerbotJson.isMember("Hitchance enable")) triggerbotConfig.hitChance = triggerbotJson["Hitchance enable"].asBool();
        if (triggerbotJson.isMember("Hitchance ammount")) triggerbotConfig.hitChanceAmmount = triggerbotJson["Hitchance ammount"].asInt();
        if (triggerbotJson.isMember("Ignore flash")) triggerbotConfig.ignoreFlash = triggerbotJson["Ignore flash"].asBool();
        if (triggerbotJson.isMember("Ignore smoke")) triggerbotConfig.ignoreSmoke = triggerbotJson["Ignore smoke"].asBool();
        if (triggerbotJson.isMember("Hitgroup")) triggerbotConfig.hitgroup = triggerbotJson["Hitgroup"].asInt();
        if (triggerbotJson.isMember("Shot delay")) triggerbotConfig.shotDelay = triggerbotJson["Shot delay"].asInt();
        if (triggerbotJson.isMember("Min damage")) triggerbotConfig.minDamage = triggerbotJson["Min damage"].asInt();
        if (triggerbotJson.isMember("Killshot")) triggerbotConfig.killshot = triggerbotJson["Killshot"].asBool();
        if (triggerbotJson.isMember("Burst Time")) triggerbotConfig.burstTime = triggerbotJson["Burst Time"].asFloat();
    }

    {
        const auto& backtrackJson = json["Backtrack"];
        if (backtrackJson.isMember("Enabled")) backtrack.enabled = backtrackJson["Enabled"].asBool();
        if (backtrackJson.isMember("Ignore smoke")) backtrack.ignoreSmoke = backtrackJson["Ignore smoke"].asBool();
        if (backtrackJson.isMember("Recoil based fov")) backtrack.recoilBasedFov = backtrackJson["Recoil based fov"].asBool();
        if (backtrackJson.isMember("Time limit")) backtrack.timeLimit = backtrackJson["Time limit"].asInt();
        if (backtrackJson.isMember("Fake Latency")) backtrack.fakeLatency = backtrackJson["Fake Latency"].asBool();
        if (backtrackJson.isMember("Latency Ammount")) backtrack.fakeLatencyAmmount = backtrackJson["Latency Ammount"].asInt();
    }

    {
        const auto& antiAimJson = json["Anti aim"];
        if (antiAimJson.isMember("Enabled")) config->antiAim.enabled = antiAimJson["Enabled"].asBool();
        if (antiAimJson.isMember("Resolver")) antiAim.resolveall = antiAimJson["Resolver"].asBool();
        if (antiAimJson.isMember("Pitch resolver")) antiAim.pitchResolver = antiAimJson["Pitch resolver"].asBool();
        if (antiAimJson.isMember("Pitch")) config->antiAim.pitch = antiAimJson["Pitch"].asBool();
        if (antiAimJson.isMember("Pitch angle")) config->antiAim.pitchAngle = antiAimJson["Pitch angle"].asFloat();
        if (antiAimJson.isMember("Yaw")) config->antiAim.yaw = antiAimJson["Yaw"].asBool();
        if (antiAimJson.isMember("Yaw angle")) config->antiAim.yawAngle = antiAimJson["Yaw angle"].asFloat();
        if (antiAimJson.isMember("Inverse Yaw Key")) config->antiAim.yawInverseAngleKey = antiAimJson["Inverse Yaw Key"].asInt();
        if (antiAimJson.isMember("Inverse Yaw Key Mode")) config->antiAim.yawInverseKeyMode = antiAimJson["Inverse Yaw Key Mode"].asInt();
        if (antiAimJson.isMember("Yaw Real")) config->antiAim.yawReal = antiAimJson["Yaw Real"].asBool();
        if (antiAimJson.isMember("Body Lean")) config->antiAim.bodyLean = antiAimJson["Body Lean"].asFloat();
        if (antiAimJson.isMember("AntiAim Mode")) config->antiAim.mode = antiAimJson["AntiAim Mode"].asInt();
        if (antiAimJson.isMember("Jitter Max")) config->antiAim.jitterMax = antiAimJson["Jitter Max"].asFloat();
        if (antiAimJson.isMember("Jitter Min")) config->antiAim.jitterMin = antiAimJson["Jitter Min"].asFloat();
        if (antiAimJson.isMember("LBY Breaker")) config->antiAim.LBYBreaker = antiAimJson["LBY Breaker"].asBool();
        if (antiAimJson.isMember("LBY Angle")) antiAim.LBYAngle = antiAimJson["LBY Angle"].asFloat();
        const auto& fakeWalkJson = json["FakeWalk"];
        if (fakeWalkJson.isMember("Enabled")) config->antiAim.fakeWalk.enabled = fakeWalkJson["Enabled"].asBool();
        if (fakeWalkJson.isMember("Fakewalk Key")) antiAim.fakeWalk.key = fakeWalkJson["Fakewalk Key"].asInt();
        if (fakeWalkJson.isMember("Fakewalk Key Mode")) antiAim.fakeWalk.keyMode = fakeWalkJson["Fakewalk Key Mode"].asInt();
        if (fakeWalkJson.isMember("Fakewalk Max Choke")) antiAim.fakeWalk.keyMode = fakeWalkJson["Fakewalk Max Choke"].asInt();
    }

    for (size_t i = 0; i < glow.size(); i++) {
        const auto& glowJson = json["glow"][i];
        auto& glowConfig = glow[i];

        if (glowJson.isMember("Enabled")) glowConfig.enabled = glowJson["Enabled"].asBool();
        if (glowJson.isMember("healthBased")) glowConfig.healthBased = glowJson["healthBased"].asBool();
        if (glowJson.isMember("thickness")) glowConfig.thickness = glowJson["thickness"].asFloat();
        if (glowJson.isMember("alpha")) glowConfig.alpha = glowJson["alpha"].asFloat();
        if (glowJson.isMember("style")) glowConfig.style = glowJson["style"].asInt();
        if (glowJson.isMember("Color")) {
            const auto& colorJson = glowJson["Color"];
            auto& colorConfig = glowConfig.color;

            if (colorJson.isMember("Color")) {
                colorConfig.color[0] = colorJson["Color"][0].asFloat();
                colorConfig.color[1] = colorJson["Color"][1].asFloat();
                colorConfig.color[2] = colorJson["Color"][2].asFloat();
            }

            if (colorJson.isMember("Rainbow")) colorConfig.rainbow = colorJson["Rainbow"].asBool();
            if (colorJson.isMember("Rainbow speed")) colorConfig.rainbowSpeed = colorJson["Rainbow speed"].asFloat();
        }
    }

    for (size_t i = 0; i < chams.size(); i++) {
        const auto& chamsJson = json["Chams"][i];
        auto& chamsConfig = chams[i];

        for (size_t j = 0; j < chamsConfig.materials.size(); j++) {
            const auto& materialsJson = chamsJson[j];
            auto& materialsConfig = chams[i].materials[j];

            if (materialsJson.isMember("Enabled")) materialsConfig.enabled = materialsJson["Enabled"].asBool();
            if (materialsJson.isMember("Health based")) materialsConfig.healthBased = materialsJson["Health based"].asBool();
            if (materialsJson.isMember("Blinking")) materialsConfig.blinking = materialsJson["Blinking"].asBool();
            if (materialsJson.isMember("Material")) materialsConfig.material = materialsJson["Material"].asInt();
            if (materialsJson.isMember("Wireframe")) materialsConfig.wireframe = materialsJson["Wireframe"].asBool();
            if (materialsJson.isMember("Color")) {
                const auto& colorJson = materialsJson["Color"];
                auto& colorConfig = materialsConfig; // leftover

                if (colorJson.isMember("Color")) {
                    colorConfig.color[0] = colorJson["Color"][0].asFloat();
                    colorConfig.color[1] = colorJson["Color"][1].asFloat();
                    colorConfig.color[2] = colorJson["Color"][2].asFloat();

                    if (colorJson["Color"].size() == 4)
                        colorConfig.color[3] = colorJson["Color"][3].asFloat();
                }

                if (colorJson.isMember("Rainbow")) colorConfig.rainbow = colorJson["Rainbow"].asBool();
                if (colorJson.isMember("Rainbow speed")) colorConfig.rainbowSpeed = colorJson["Rainbow speed"].asFloat();
            }
            if (materialsJson.isMember("Alpha")) materialsConfig.color[3] = materialsJson["Alpha"].asFloat();
        }
    }

    for (size_t i = 0; i < esp.players.size(); i++) {
        const auto& espJson = json["Esp"]["Players"][i];
        auto& espConfig = esp.players[i];

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("HP side")) espConfig.hpside = espJson["HP side"].asInt();
        if (espJson.isMember("Armor side")) espConfig.armorside = espJson["Armor side"].asInt();

        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Eye traces")) {
            const auto& eyeTracesJson = espJson["Eye traces"];
            auto& eyeTracesConfig = espConfig.eyeTraces;

            if (eyeTracesJson.isMember("Enabled")) eyeTracesConfig.enabled = eyeTracesJson["Enabled"].asBool();

            if (eyeTracesJson.isMember("Color")) {
                eyeTracesConfig.color[0] = eyeTracesJson["Color"][0].asFloat();
                eyeTracesConfig.color[1] = eyeTracesJson["Color"][1].asFloat();
                eyeTracesConfig.color[2] = eyeTracesJson["Color"][2].asFloat();
            }

            if (eyeTracesJson.isMember("Rainbow")) eyeTracesConfig.rainbow = eyeTracesJson["Rainbow"].asBool();
            if (eyeTracesJson.isMember("Rainbow speed")) eyeTracesConfig.rainbowSpeed = eyeTracesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Health")) {
            const auto& healthJson = espJson["Health"];
            auto& healthConfig = espConfig.health;

            if (healthJson.isMember("Enabled")) healthConfig.enabled = healthJson["Enabled"].asBool();

            if (healthJson.isMember("Color")) {
                healthConfig.color[0] = healthJson["Color"][0].asFloat();
                healthConfig.color[1] = healthJson["Color"][1].asFloat();
                healthConfig.color[2] = healthJson["Color"][2].asFloat();
            }

            if (healthJson.isMember("Rainbow")) healthConfig.rainbow = healthJson["Rainbow"].asBool();
            if (healthJson.isMember("Rainbow speed")) healthConfig.rainbowSpeed = healthJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Health bar")) {
            const auto& healthBarJson = espJson["Health bar"];
            auto& healthBarConfig = espConfig.healthBar;

            if (healthBarJson.isMember("Enabled")) healthBarConfig.enabled = healthBarJson["Enabled"].asBool();

            if (healthBarJson.isMember("Color")) {
                healthBarConfig.color[0] = healthBarJson["Color"][0].asFloat();
                healthBarConfig.color[1] = healthBarJson["Color"][1].asFloat();
                healthBarConfig.color[2] = healthBarJson["Color"][2].asFloat();
            }

            if (healthBarJson.isMember("Rainbow")) healthBarConfig.rainbow = healthBarJson["Rainbow"].asBool();
            if (healthBarJson.isMember("Rainbow speed")) healthBarConfig.rainbowSpeed = healthBarJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Armor")) {
            const auto& armorJson = espJson["Armor"];
            auto& armorConfig = espConfig.armor;

            if (armorJson.isMember("Enabled")) armorConfig.enabled = armorJson["Enabled"].asBool();

            if (armorJson.isMember("Color")) {
                armorConfig.color[0] = armorJson["Color"][0].asFloat();
                armorConfig.color[1] = armorJson["Color"][1].asFloat();
                armorConfig.color[2] = armorJson["Color"][2].asFloat();
            }

            if (armorJson.isMember("Rainbow")) armorConfig.rainbow = armorJson["Rainbow"].asBool();
            if (armorJson.isMember("Rainbow speed")) armorConfig.rainbowSpeed = armorJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Armor bar")) {
            const auto& armorBarJson = espJson["Armor bar"];
            auto& armorBarConfig = espConfig.armorBar;

            if (armorBarJson.isMember("Enabled")) armorBarConfig.enabled = armorBarJson["Enabled"].asBool();

            if (armorBarJson.isMember("Color")) {
                armorBarConfig.color[0] = armorBarJson["Color"][0].asFloat();
                armorBarConfig.color[1] = armorBarJson["Color"][1].asFloat();
                armorBarConfig.color[2] = armorBarJson["Color"][2].asFloat();
            }

            if (armorBarJson.isMember("Rainbow")) armorBarConfig.rainbow = armorBarJson["Rainbow"].asBool();
            if (armorBarJson.isMember("Rainbow speed")) armorBarConfig.rainbowSpeed = armorBarJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Money")) {
            const auto& moneyJson = espJson["Money"];
            auto& moneyConfig = espConfig.money;

            if (moneyJson.isMember("Enabled")) moneyConfig.enabled = moneyJson["Enabled"].asBool();

            if (moneyJson.isMember("Color")) {
                moneyConfig.color[0] = moneyJson["Color"][0].asFloat();
                moneyConfig.color[1] = moneyJson["Color"][1].asFloat();
                moneyConfig.color[2] = moneyJson["Color"][2].asFloat();
            }

            if (moneyJson.isMember("Rainbow")) moneyConfig.rainbow = moneyJson["Rainbow"].asBool();
            if (moneyJson.isMember("Rainbow speed")) moneyConfig.rainbowSpeed = moneyJson["Rainbow speed"].asFloat();
        }

       

        if (espJson.isMember("Head dot")) {
            const auto& headDotJson = espJson["Head dot"];
            auto& headDotConfig = espConfig.headDot;

            if (headDotJson.isMember("Enabled")) headDotConfig.enabled = headDotJson["Enabled"].asBool();

            if (headDotJson.isMember("Color")) {
                headDotConfig.color[0] = headDotJson["Color"][0].asFloat();
                headDotConfig.color[1] = headDotJson["Color"][1].asFloat();
                headDotConfig.color[2] = headDotJson["Color"][2].asFloat();
            }

            if (headDotJson.isMember("Rainbow")) headDotConfig.rainbow = headDotJson["Rainbow"].asBool();
            if (headDotJson.isMember("Rainbow speed")) headDotConfig.rainbowSpeed = headDotJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Active weapon")) {
            const auto& activeWeaponJson = espJson["Active weapon"];
            auto& activeWeaponConfig = espConfig.activeWeapon;

            if (activeWeaponJson.isMember("Enabled")) activeWeaponConfig.enabled = activeWeaponJson["Enabled"].asBool();

            if (activeWeaponJson.isMember("Color")) {
                activeWeaponConfig.color[0] = activeWeaponJson["Color"][0].asFloat();
                activeWeaponConfig.color[1] = activeWeaponJson["Color"][1].asFloat();
                activeWeaponConfig.color[2] = activeWeaponJson["Color"][2].asFloat();
            }

            if (activeWeaponJson.isMember("Rainbow")) activeWeaponConfig.rainbow = activeWeaponJson["Rainbow"].asBool();
            if (activeWeaponJson.isMember("Rainbow speed")) activeWeaponConfig.rainbowSpeed = activeWeaponJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }
        if (espJson.isMember("Ammo")) {
            const auto& ammoJson = espJson["Ammo"];
            auto& ammoConfig = espConfig.ammo;

            if (ammoJson.isMember("Enabled")) ammoConfig.enabled = ammoJson["Enabled"].asBool();

            if (ammoJson.isMember("Color")) {
                ammoConfig.color[0] = ammoJson["Color"][0].asFloat();
                ammoConfig.color[1] = ammoJson["Color"][1].asFloat();
                ammoConfig.color[2] = ammoJson["Color"][2].asFloat();
            }

            if (ammoJson.isMember("Rainbow")) ammoConfig.rainbow = ammoJson["Rainbow"].asBool();
            if (ammoJson.isMember("Rainbow speed")) ammoConfig.rainbowSpeed = ammoJson["Rainbow speed"].asFloat();
        }
        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Dead ESP")) espConfig.deadesp = espJson["Dead ESP"].asBool();
        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    {
        const auto& espJson = json["Esp"]["Weapons"];
        auto& espConfig = esp.weapon;

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    for (size_t i = 0; i < esp.dangerZone.size(); i++) {
        const auto& espJson = json["Esp"]["Danger Zone"][i];
        auto& espConfig = esp.dangerZone[i];

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    for (size_t i = 0; i < esp.projectiles.size(); i++) {
        const auto& espJson = json["Esp"]["Projectiles"][i];
        auto& espConfig = esp.projectiles[i];

        if (espJson.isMember("Enabled")) espConfig.enabled = espJson["Enabled"].asBool();
        if (espJson.isMember("Font")) espConfig.font = espJson["Font"].asInt();
        if (espJson.isMember("Snaplines")) {
            const auto& snaplinesJson = espJson["Snaplines"];
            auto& snaplinesConfig = espConfig.snaplines;

            if (snaplinesJson.isMember("Enabled")) snaplinesConfig.enabled = snaplinesJson["Enabled"].asBool();

            if (snaplinesJson.isMember("Color")) {
                snaplinesConfig.color[0] = snaplinesJson["Color"][0].asFloat();
                snaplinesConfig.color[1] = snaplinesJson["Color"][1].asFloat();
                snaplinesConfig.color[2] = snaplinesJson["Color"][2].asFloat();
            }

            if (snaplinesJson.isMember("Rainbow")) snaplinesConfig.rainbow = snaplinesJson["Rainbow"].asBool();
            if (snaplinesJson.isMember("Rainbow speed")) snaplinesConfig.rainbowSpeed = snaplinesJson["Rainbow speed"].asFloat();
        }
        if (espJson.isMember("Box")) {
            const auto& boxJson = espJson["Box"];
            auto& boxConfig = espConfig.box;

            if (boxJson.isMember("Enabled")) boxConfig.enabled = boxJson["Enabled"].asBool();

            if (boxJson.isMember("Color")) {
                boxConfig.color[0] = boxJson["Color"][0].asFloat();
                boxConfig.color[1] = boxJson["Color"][1].asFloat();
                boxConfig.color[2] = boxJson["Color"][2].asFloat();
            }

            if (boxJson.isMember("Rainbow")) boxConfig.rainbow = boxJson["Rainbow"].asBool();
            if (boxJson.isMember("Rainbow speed")) boxConfig.rainbowSpeed = boxJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Box type")) espConfig.boxType = espJson["Box type"].asInt();

        if (espJson.isMember("Outline")) {
            const auto& outlineJson = espJson["Outline"];
            auto& outlineConfig = espConfig.outline;

            if (outlineJson.isMember("Enabled")) outlineConfig.enabled = outlineJson["Enabled"].asBool();

            if (outlineJson.isMember("Color")) {
                outlineConfig.color[0] = outlineJson["Color"][0].asFloat();
                outlineConfig.color[1] = outlineJson["Color"][1].asFloat();
                outlineConfig.color[2] = outlineJson["Color"][2].asFloat();
            }

            if (outlineJson.isMember("Rainbow")) outlineConfig.rainbow = outlineJson["Rainbow"].asBool();
            if (outlineJson.isMember("Rainbow speed")) outlineConfig.rainbowSpeed = outlineJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Name")) {
            const auto& nameJson = espJson["Name"];
            auto& nameConfig = espConfig.name;

            if (nameJson.isMember("Enabled")) nameConfig.enabled = nameJson["Enabled"].asBool();

            if (nameJson.isMember("Color")) {
                nameConfig.color[0] = nameJson["Color"][0].asFloat();
                nameConfig.color[1] = nameJson["Color"][1].asFloat();
                nameConfig.color[2] = nameJson["Color"][2].asFloat();
            }

            if (nameJson.isMember("Rainbow")) nameConfig.rainbow = nameJson["Rainbow"].asBool();
            if (nameJson.isMember("Rainbow speed")) nameConfig.rainbowSpeed = nameJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Distance")) {
            const auto& distanceJson = espJson["Distance"];
            auto& distanceConfig = espConfig.distance;

            if (distanceJson.isMember("Enabled")) distanceConfig.enabled = distanceJson["Enabled"].asBool();

            if (distanceJson.isMember("Color")) {
                distanceConfig.color[0] = distanceJson["Color"][0].asFloat();
                distanceConfig.color[1] = distanceJson["Color"][1].asFloat();
                distanceConfig.color[2] = distanceJson["Color"][2].asFloat();
            }

            if (distanceJson.isMember("Rainbow")) distanceConfig.rainbow = distanceJson["Rainbow"].asBool();
            if (distanceJson.isMember("Rainbow speed")) distanceConfig.rainbowSpeed = distanceJson["Rainbow speed"].asFloat();
        }

        if (espJson.isMember("Max distance")) espConfig.maxDistance = espJson["Max distance"].asFloat();
    }

    {
        const auto& visualsJson = json["visuals"];
        if (visualsJson.isMember("disablePostProcessing")) visuals.disablePostProcessing = visualsJson["disablePostProcessing"].asBool();
        if (visualsJson.isMember("inverseRagdollGravity")) visuals.inverseRagdollGravity = visualsJson["inverseRagdollGravity"].asBool();
        if (visualsJson.isMember("noFog")) visuals.noFog = visualsJson["noFog"].asBool();
        if (visualsJson.isMember("no3dSky")) visuals.no3dSky = visualsJson["no3dSky"].asBool();
        if (visualsJson.isMember("No aim punch")) visuals.noAimPunch = visualsJson["No aim punch"].asBool();
        if (visualsJson.isMember("No view punch")) visuals.noViewPunch = visualsJson["No view punch"].asBool();
        if (visualsJson.isMember("noHands")) visuals.noHands = visualsJson["noHands"].asBool();
        if (visualsJson.isMember("noSleeves")) visuals.noSleeves = visualsJson["noSleeves"].asBool();
        if (visualsJson.isMember("noWeapons")) visuals.noWeapons = visualsJson["noWeapons"].asBool();
        if (visualsJson.isMember("noSmoke")) visuals.noSmoke = visualsJson["noSmoke"].asBool();
        if (visualsJson.isMember("noBlur")) visuals.noBlur = visualsJson["noBlur"].asBool();
        if (visualsJson.isMember("noBloom")) visuals.noBloom = visualsJson["noBloom"].asBool();
        if (visualsJson.isMember("noBloom")) visuals.noBloom = visualsJson["noBloom"].asBool();
        if (visualsJson.isMember("noScopeOverlay")) visuals.noScopeOverlay = visualsJson["noScopeOverlay"].asBool();
        if (visualsJson.isMember("noGrass")) visuals.noGrass = visualsJson["noGrass"].asBool();
        if (visualsJson.isMember("noShadows")) visuals.noShadows = visualsJson["noShadows"].asBool();
        if (visualsJson.isMember("wireframeSmoke")) visuals.wireframeSmoke = visualsJson["wireframeSmoke"].asBool();
        if (visualsJson.isMember("Zoom")) visuals.zoom = visualsJson["Zoom"].asBool();
        if (visualsJson.isMember("Zoom key")) visuals.zoomKey = visualsJson["Zoom key"].asInt();
        if (visualsJson.isMember("thirdperson")) visuals.thirdperson = visualsJson["thirdperson"].asBool();
        if (visualsJson.isMember("Night Mode")) visuals.nightMode = visualsJson["Night Mode"].asBool();
        if (visualsJson.isMember("thirdpersonKey")) visuals.thirdpersonKey = visualsJson["thirdpersonKey"].asInt();
        if (visualsJson.isMember("thirdpersonDistance")) visuals.thirdpersonDistance = visualsJson["thirdpersonDistance"].asInt();
        if (visualsJson.isMember("viewmodelFov")) visuals.viewmodelFov = visualsJson["viewmodelFov"].asInt();
        if (visualsJson.isMember("Fov")) visuals.fov = visualsJson["Fov"].asInt();
        if (visualsJson.isMember("farZ")) visuals.farZ = visualsJson["farZ"].asInt();
        if (visualsJson.isMember("flashReduction")) visuals.flashReduction = visualsJson["flashReduction"].asInt();
        if (visualsJson.isMember("brightness")) visuals.brightness = visualsJson["brightness"].asFloat();
        if (visualsJson.isMember("skybox")) visuals.skybox = visualsJson["skybox"].asInt();
        if (visualsJson.isMember("customSkybox")) visuals.customSkybox = visualsJson["customSkybox"].asString();
        if (visualsJson.isMember("Thirdperson Mode")) config->antiAim.thirdpersonMode = visualsJson["Thirdperson Mode"].asInt();
        if (visualsJson.isMember("Indicators Enabled")) visuals.indicatorsEnabled = visualsJson["Indicators Enabled"].asBool();
        if (visualsJson.isMember("Desync Indicator")) visuals.selectedIndicators[0] = visualsJson["Desync Indicator"].asBool();
        if (visualsJson.isMember("LBY Indicator")) visuals.selectedIndicators[1] = visualsJson["LBY Indicator"].asBool();
        if (visualsJson.isMember("Fakelag Indicator")) visuals.selectedIndicators[2] = visualsJson["Fakelag Indicator"].asBool();
        if (visualsJson.isMember("Fakeduck Indicator")) visuals.selectedIndicators[3] = visualsJson["Fakeduck Indicator"].asBool();
        if (visualsJson.isMember("World")) {
            const auto& worldJson = visualsJson["World"];

            if (worldJson.isMember("Enabled")) visuals.world.enabled = worldJson["Enabled"].asBool();

            if (worldJson.isMember("Color")) {
                visuals.world.color[0] = worldJson["Color"][0].asFloat();
                visuals.world.color[1] = worldJson["Color"][1].asFloat();
                visuals.world.color[2] = worldJson["Color"][2].asFloat();
            }
            if (worldJson.isMember("Rainbow")) visuals.world.rainbow = worldJson["Rainbow"].asBool();
            if (worldJson.isMember("Rainbow speed")) visuals.world.rainbowSpeed = worldJson["Rainbow speed"].asFloat();
        }
        if (visualsJson.isMember("Sky")) {
            const auto& skyJson = visualsJson["Sky"];

            if (skyJson.isMember("Enabled")) visuals.sky.enabled = skyJson["Enabled"].asBool();

            if (skyJson.isMember("Color")) {
                visuals.sky.color[0] = skyJson["Color"][0].asFloat();
                visuals.sky.color[1] = skyJson["Color"][1].asFloat();
                visuals.sky.color[2] = skyJson["Color"][2].asFloat();
            }
            if (skyJson.isMember("Rainbow")) visuals.sky.rainbow = skyJson["Rainbow"].asBool();
            if (skyJson.isMember("Rainbow speed")) visuals.sky.rainbowSpeed = skyJson["Rainbow speed"].asFloat();
        }
        if (visualsJson.isMember("Deagle spinner")) visuals.deagleSpinner = visualsJson["Deagle spinner"].asBool();
        if (visualsJson.isMember("Screen effect")) visuals.screenEffect = visualsJson["Screen effect"].asInt();
        if (visualsJson.isMember("Hit effect")) visuals.hitEffect = visualsJson["Hit effect"].asInt();
        if (visualsJson.isMember("Hit effect time")) visuals.hitEffectTime = visualsJson["Hit effect time"].asFloat();
        if (visualsJson.isMember("Hit marker")) visuals.hitMarker = visualsJson["Hit marker"].asInt();
        if (visualsJson.isMember("Hit marker time")) visuals.hitMarkerTime = visualsJson["Hit marker time"].asFloat();

        if (const auto& hitMarkerIndicator{ visualsJson["Hit marker Damage Indicator"] }; hitMarkerIndicator.isObject()) {
            if (const auto& enabled{ hitMarkerIndicator["Enabled"] }; enabled.isBool())
                visuals.hitMarkerDamageIndicator.enabled = enabled.asBool();

            if (const auto& color{ hitMarkerIndicator["Color"] }; color.isArray()) {
                visuals.hitMarkerDamageIndicator.color[0] = color[0].asFloat();
                visuals.hitMarkerDamageIndicator.color[1] = color[1].asFloat();
                visuals.hitMarkerDamageIndicator.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ hitMarkerIndicator["Rainbow"] }; rainbow.isBool())
                visuals.hitMarkerDamageIndicator.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ hitMarkerIndicator["Rainbow speed"] }; rainbowSpeed.isDouble())
                visuals.hitMarkerDamageIndicator.rainbowSpeed = rainbowSpeed.asFloat();
        }

        if (visualsJson.isMember("Customize Hitmarker")) visuals.hitMarkerDamageIndicatorCustomize = visualsJson["Customize Hitmarker"].asBool();
        if (visualsJson.isMember("Font")) visuals.hitMarkerDamageIndicatorFont = visualsJson["Font"].asInt();
        if (visualsJson.isMember("Alpha")) visuals.hitMarkerDamageIndicatorAlpha = visualsJson["Alpha"].asInt();
        if (visualsJson.isMember("Dist")) visuals.hitMarkerDamageIndicatorDist = visualsJson["Dist"].asInt();
        if (visualsJson.isMember("Text X")) visuals.hitMarkerDamageIndicatorTextX = visualsJson["Text X"].asInt();
        if (visualsJson.isMember("Text Y")) visuals.hitMarkerDamageIndicatorTextY = visualsJson["Text Y"].asInt();
        if (visualsJson.isMember("Ratio")) visuals.hitMarkerDamageIndicatorRatio = visualsJson["Ratio"].asFloat();
        //hitmarker indicator ends here

        if (const auto& velocityConfig{ visualsJson["Show Velocity"] }; velocityConfig.isObject()) {
            if (const auto& enabled{ velocityConfig["Enabled"] }; enabled.isBool())
                visuals.showvelocity.enabled = enabled.asBool();

            if (const auto& color{ velocityConfig["Color"] }; color.isArray()) {
                visuals.showvelocity.color[0] = color[0].asFloat();
                visuals.showvelocity.color[1] = color[1].asFloat();
                visuals.showvelocity.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ velocityConfig["Rainbow"] }; rainbow.isBool())
                visuals.showvelocity.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ velocityConfig["Rainbow speed"] }; rainbowSpeed.isDouble())
                visuals.showvelocity.rainbowSpeed = rainbowSpeed.asFloat();
        }
        if (visualsJson.isMember("Custom Pos")) visuals.showvelocityM = visualsJson["Custom Pos"].asBool();
        if (visualsJson.isMember("Res X")) visuals.showvelocityResX = visualsJson["Res X"].asInt();
        if (visualsJson.isMember("Res Y")) visuals.showvelocityResY = visualsJson["Res Y"].asInt();
        //show velocity ends here

        if (visualsJson.isMember("Playermodel T")) visuals.playerModelT = visualsJson["Playermodel T"].asInt();
        if (visualsJson.isMember("Playermodel CT")) visuals.playerModelCT = visualsJson["Playermodel CT"].asInt();
        if (visualsJson.isMember("Bullet Beams")) {
            const auto& worldJson = visualsJson["Bullet Beams"];

            if (worldJson.isMember("Enabled")) visuals.bulletTracers.enabled = worldJson["Enabled"].asBool();

            if (worldJson.isMember("Color")) {
                visuals.bulletTracers.color[0] = worldJson["Color"][0].asFloat();
                visuals.bulletTracers.color[1] = worldJson["Color"][1].asFloat();
                visuals.bulletTracers.color[2] = worldJson["Color"][2].asFloat();
            }
            if (worldJson.isMember("Rainbow")) visuals.bulletTracers.rainbow = worldJson["Rainbow"].asBool();
            if (worldJson.isMember("Rainbow speed")) visuals.bulletTracers.rainbowSpeed = worldJson["Rainbow speed"].asFloat();
        }
        if (visualsJson.isMember("Color correction")) {
            const auto& cc = visualsJson["Color correction"];

            if (cc.isMember("Enabled")) visuals.colorCorrection.enabled = cc["Enabled"].asBool();
            if (cc.isMember("Blue")) visuals.colorCorrection.blue = cc["Blue"].asFloat();
            if (cc.isMember("Red")) visuals.colorCorrection.red = cc["Red"].asFloat();
            if (cc.isMember("Mono")) visuals.colorCorrection.mono = cc["Mono"].asFloat();
            if (cc.isMember("Saturation")) visuals.colorCorrection.saturation = cc["Saturation"].asFloat();
            if (cc.isMember("Ghost")) visuals.colorCorrection.ghost = cc["Ghost"].asFloat();
            if (cc.isMember("Green")) visuals.colorCorrection.green = cc["Green"].asFloat();
            if (cc.isMember("Yellow")) visuals.colorCorrection.yellow = cc["Yellow"].asFloat();
        }
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        const auto& skinChangerJson = json["skinChanger"][i];
        auto& skinChangerConfig = skinChanger[i];

        if (skinChangerJson.isMember("Enabled")) skinChangerConfig.enabled = skinChangerJson["Enabled"].asBool();
        if (skinChangerJson.isMember("definition_vector_index")) skinChangerConfig.itemIdIndex = skinChangerJson["definition_vector_index"].asInt();
        if (skinChangerJson.isMember("definition_index")) skinChangerConfig.itemId = skinChangerJson["definition_index"].asInt();
        if (skinChangerJson.isMember("entity_quality_vector_index")) skinChangerConfig.entity_quality_vector_index = skinChangerJson["entity_quality_vector_index"].asInt();
        if (skinChangerJson.isMember("entity_quality_index")) skinChangerConfig.quality = skinChangerJson["entity_quality_index"].asInt();
        if (skinChangerJson.isMember("paint_kit_vector_index")) skinChangerConfig.paint_kit_vector_index = skinChangerJson["paint_kit_vector_index"].asInt();
        if (skinChangerJson.isMember("paint_kit_index")) skinChangerConfig.paintKit = skinChangerJson["paint_kit_index"].asInt();
        if (skinChangerJson.isMember("definition_override_vector_index")) skinChangerConfig.definition_override_vector_index = skinChangerJson["definition_override_vector_index"].asInt();
        if (skinChangerJson.isMember("definition_override_index")) skinChangerConfig.definition_override_index = skinChangerJson["definition_override_index"].asInt();
        if (skinChangerJson.isMember("seed")) skinChangerConfig.seed = skinChangerJson["seed"].asInt();
        if (skinChangerJson.isMember("stat_trak")) skinChangerConfig.stat_trak = skinChangerJson["stat_trak"].asInt();
        if (skinChangerJson.isMember("wear")) skinChangerConfig.wear = skinChangerJson["wear"].asFloat();
        if (skinChangerJson.isMember("custom_name")) strcpy_s(skinChangerConfig.custom_name, sizeof(skinChangerConfig.custom_name), skinChangerJson["custom_name"].asCString());

        if (skinChangerJson.isMember("stickers")) {
            for (size_t j = 0; j < skinChangerConfig.stickers.size(); j++) {
                const auto& stickerJson = skinChangerJson["stickers"][j];
                auto& stickerConfig = skinChangerConfig.stickers[j];

                if (stickerJson.isMember("kit")) stickerConfig.kit = stickerJson["kit"].asInt();
                if (stickerJson.isMember("kit_vector_index")) stickerConfig.kit_vector_index = stickerJson["kit_vector_index"].asInt();
                if (stickerJson.isMember("wear")) stickerConfig.wear = stickerJson["wear"].asFloat();
                if (stickerJson.isMember("scale")) stickerConfig.scale = stickerJson["scale"].asFloat();
                if (stickerJson.isMember("rotation")) stickerConfig.rotation = stickerJson["rotation"].asFloat();
            }
        }
    }

    {
        const auto& soundJson = json["Sound"];

        if (soundJson.isMember("Chicken volume")) sound.chickenVolume = soundJson["Chicken volume"].asInt();

        if (soundJson.isMember("Players")) {
            for (size_t i = 0; i < sound.players.size(); i++) {
                const auto& playerJson = soundJson["Players"][i];
                auto& playerConfig = sound.players[i];

                if (playerJson.isMember("Master volume")) playerConfig.masterVolume = playerJson["Master volume"].asInt();
                if (playerJson.isMember("Headshot volume")) playerConfig.headshotVolume = playerJson["Headshot volume"].asInt();
                if (playerJson.isMember("Weapon volume")) playerConfig.weaponVolume = playerJson["Weapon volume"].asInt();
                if (playerJson.isMember("Footstep volume")) playerConfig.footstepVolume = playerJson["Footstep volume"].asInt();
            }
        }
    }


    {
        const auto& styleJson = json["Style"];

        if (styleJson.isMember("Menu style")) style.menuStyle = styleJson["Menu style"].asInt();
        if (styleJson.isMember("Menu colors")) style.menuColors = styleJson["Menu colors"].asInt();

        if (styleJson.isMember("Colors")) {
            const auto& colorsJson = styleJson["Colors"];

            ImGuiStyle& style = ImGui::GetStyle();

            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                if (const char* name = ImGui::GetStyleColorName(i); colorsJson.isMember(name)) {
                    const auto& colorJson = styleJson["Colors"][name];
                    style.Colors[i].x = colorJson[0].asFloat();
                    style.Colors[i].y = colorJson[1].asFloat();
                    style.Colors[i].z = colorJson[2].asFloat();
                    style.Colors[i].w = colorJson[3].asFloat();
                }
            }
        }
    }

    {
        const auto& miscJson = json["Misc"];

        if (miscJson.isMember("Menu key")) misc.menuKey = miscJson["Menu key"].asInt();
        if (miscJson.isMember("Anti AFK kick")) misc.antiAfkKick = miscJson["Anti AFK kick"].asBool();
        //if (miscJson.isMember("resolver")) misc.resolver = miscJson["resolver"].asBool();
        if (miscJson.isMember("Auto strafe")) misc.autoStrafe = miscJson["Auto strafe"].asBool();
        if (miscJson.isMember("Strafe Key")) misc.autoStrafeKey = miscJson["Strafe Key"].asInt();
        if (miscJson.isMember("Strafe Style")) misc.autoStrafeStyle = miscJson["Strafe Style"].asBool();
        if (miscJson.isMember("Bunny hop")) misc.bunnyHop = miscJson["Bunny hop"].asBool();
        if (miscJson.isMember("Custom View")) misc.CustomViewModel = miscJson["Custom View"].asBool();
        if (miscJson.isMember("View x")) misc.viewmodel_x = miscJson["View x"].asFloat();
        if (miscJson.isMember("View y")) misc.viewmodel_y = miscJson["View y"].asFloat();
        if (miscJson.isMember("View z")) misc.viewmodel_z = miscJson["View z"].asFloat();
        if (miscJson.isMember("Door Spam")) misc.usespam = miscJson["Door Spam"].asBool();
        if (miscJson.isMember("BlockBot")) misc.playerBlocker = miscJson["BlockBot"].asBool();
        if (miscJson.isMember("BlockBot Key")) misc.playerBlockerKey = miscJson["BlockBot Key"].asInt();
        if (miscJson.isMember("Custom clan tag")) misc.customClanTag = miscJson["Custom clan tag"].asBool();
        if (miscJson.isMember("Clock tag")) misc.clocktag = miscJson["Clock tag"].asBool();
        if (miscJson.isMember("Clan tag")) strncpy_s(misc.clanTag, miscJson["Clan tag"].asCString(), _TRUNCATE);
        if (miscJson.isMember("Animated clan tag")) misc.animatedClanTag = miscJson["Animated clan tag"].asBool();
        if (miscJson.isMember("Tag Anim Type")) misc.tagAnimType = miscJson["Tag Anim Type"].asInt();
        if (miscJson.isMember("Fast duck")) misc.fastDuck = miscJson["Fast duck"].asBool();
        if (miscJson.isMember("Fast Stop")) misc.fastStop = miscJson["Fast Stop"].asBool();
        if (miscJson.isMember("Moonwalk")) misc.moonwalk = miscJson["Moonwalk"].asBool();
        if (miscJson.isMember("Jumpbug")) misc.jumpbug = miscJson["Jumpbug"].asBool();
        if (miscJson.isMember("Jumpbug Key")) misc.jumpbugkey = miscJson["Jumpbug Key"].asInt();
        if (miscJson.isMember("Edge Jump")) misc.edgejump = miscJson["Edge Jump"].asBool();
        if (miscJson.isMember("Edge Jump Key")) misc.edgejumpkey = miscJson["Edge Jump Key"].asInt();    
        if (miscJson.isMember("Slowwalk")) misc.slowwalk = miscJson["Slowwalk"].asBool();
        if (miscJson.isMember("Slowwalk key")) misc.slowwalkKey = miscJson["Slowwalk key"].asInt();
        if (miscJson.isMember("Sniper crosshair")) misc.sniperCrosshair = miscJson["Sniper crosshair"].asBool();
        if (miscJson.isMember("Recoil crosshair")) misc.recoilCrosshair = miscJson["Recoil crosshair"].asBool();
        if (miscJson.isMember("Auto pistol")) misc.autoPistol = miscJson["Auto pistol"].asBool();
        if (miscJson.isMember("Auto reload")) misc.autoReload = miscJson["Auto reload"].asBool();
        if (miscJson.isMember("Auto accept")) misc.autoAccept = miscJson["Auto accept"].asBool();
        if (miscJson.isMember("Radar hack")) misc.radarHack = miscJson["Radar hack"].asBool();
        if (miscJson.isMember("Reveal ranks")) misc.revealRanks = miscJson["Reveal ranks"].asBool();
        if (miscJson.isMember("Reveal money")) misc.revealMoney = miscJson["Reveal money"].asBool();
        if (miscJson.isMember("Reveal suspect")) misc.revealSuspect = miscJson["Reveal suspect"].asBool();
        if (miscJson.isMember("Spammer")) misc.chat_spammer = miscJson["Spammer"].asInt();
        if (miscJson.isMember("Spectator List")) misc.spectatorList = miscJson["Spectator List"].asBool();
        if (miscJson.isMember("Watermark")) misc.watermark = miscJson["Watermark"].asBool();


        /*if (const auto& watermark{ miscJson["Watermark"] }; watermark.isObject()) {
            if (const auto& enabled{ watermark["Enabled"] }; enabled.isBool())
                misc.watermark.enabled = enabled.asBool();

            if (const auto& color{ watermark["Color"] }; color.isArray()) {
                misc.watermark.color[0] = color[0].asFloat();
                misc.watermark.color[1] = color[1].asFloat();
                misc.watermark.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ watermark["Rainbow"] }; rainbow.isBool())
                misc.watermark.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ watermark["Rainbow speed"] }; rainbowSpeed.isDouble())
                misc.watermark.rainbowSpeed = rainbowSpeed.asFloat();
        }*/

        if (miscJson.isMember("Fix animation LOD")) misc.fixAnimationLOD = miscJson["Fix animation LOD"].asBool();
        if (miscJson.isMember("Fix bone matrix")) misc.fixBoneMatrix = miscJson["Fix bone matrix"].asBool();
        if (miscJson.isMember("Fix movement")) misc.fixMovement = miscJson["Fix movement"].asBool();
        if (miscJson.isMember("Disable model occlusion")) misc.disableModelOcclusion = miscJson["Disable model occlusion"].asBool();
        if (miscJson.isMember("Aspect Ratio")) misc.aspectratio = miscJson["Aspect Ratio"].asFloat();
        if (miscJson.isMember("Kill message")) misc.killMessage = miscJson["Kill message"].asBool();
        if (miscJson.isMember("Kill message string")) misc.killMessageString = miscJson["Kill message string"].asString();
        if (miscJson.isMember("Name stealer"))  misc.nameStealer = miscJson["Name stealer"].asBool();
        if (miscJson.isMember("Disable HUD blur"))  misc.disablePanoramablur = miscJson["Disable HUD blur"].asBool();
        if (miscJson.isMember("Ban color")) misc.banColor = miscJson["Ban color"].asInt();
        if (miscJson.isMember("Ban text")) misc.banText = miscJson["Ban text"].asString();
        if (miscJson.isMember("Fast plant")) misc.fastPlant = miscJson["Fast plant"].asBool();

        if (const auto& bombTimer{ miscJson["Bomb timer"] }; bombTimer.isObject()) {
            if (const auto& enabled{ bombTimer["Enabled"] }; enabled.isBool())
                misc.bombTimer.enabled = enabled.asBool();

            if (const auto& color{ bombTimer["Color"] }; color.isArray()) {
                misc.bombTimer.color[0] = color[0].asFloat();
                misc.bombTimer.color[1] = color[1].asFloat();
                misc.bombTimer.color[2] = color[2].asFloat();
            }
            if (const auto& rainbow{ bombTimer["Rainbow"] }; rainbow.isBool())
                misc.bombTimer.rainbow = rainbow.asBool();

            if (const auto& rainbowSpeed{ bombTimer["Rainbow speed"] }; rainbowSpeed.isDouble())
                misc.bombTimer.rainbowSpeed = rainbowSpeed.asFloat();
        }

        if (miscJson.isMember("Bomb Damage")) misc.bombDmg = miscJson["Bomb Damage"].asBool();
        if (miscJson.isMember("Quick reload")) misc.quickReload = miscJson["Quick reload"].asBool();
        if (miscJson.isMember("Prepare revolver")) misc.prepareRevolver = miscJson["Prepare revolver"].asBool();
        if (miscJson.isMember("Prepare revolver key")) misc.prepareRevolverKey = miscJson["Prepare revolver key"].asInt();
        if (miscJson.isMember("Hit sound")) misc.hitSound = miscJson["Hit sound"].asInt();
        if (miscJson.isMember("Fakelag Ticks")) misc.fakeLagTicks = miscJson["Fakelag Ticks"].asInt();
        if (miscJson.isMember("Fakelag Mode")) misc.fakeLagMode = miscJson["Fakelag Mode"].asInt();
        if (miscJson.isMember("Fakelag While Shooting")) misc.fakeLagSelectedFlags[0] = miscJson["Fakelag While Shooting"].asBool();
        if (miscJson.isMember("Fakelag While Standing")) misc.fakeLagSelectedFlags[1] = miscJson["Fakelag While Standing"].asBool();
        if (miscJson.isMember("Fakelag While Moving")) misc.fakeLagSelectedFlags[2] = miscJson["Fakelag While Moving"].asBool();
        if (miscJson.isMember("Fakelag In Air")) misc.fakeLagSelectedFlags[3] = miscJson["Fakelag In Air"].asBool();
        if (miscJson.isMember("Fakelag key")) misc.fakeLagKey = miscJson["Fakelag key"].asInt();
        if (miscJson.isMember("Quick healthshot key")) misc.quickHealthshotKey = miscJson["Quick healthshot key"].asInt();
        if (miscJson.isMember("Grenade predict")) misc.nadePredict = miscJson["Grenade predict"].asBool();
        if (miscJson.isMember("Fix tablet signal")) misc.fixTabletSignal = miscJson["Fix tablet signal"].asBool();
        if (miscJson.isMember("Max angle delta")) misc.maxAngleDelta = miscJson["Max angle delta"].asFloat();
        if (miscJson.isMember("Fake prime")) misc.fakePrime = miscJson["Fake prime"].asBool();
        if (miscJson.isMember("Sv pure bypass")) misc.svpurebypass = miscJson["Sv pure bypass"].asBool();
        if (miscJson.isMember("Draw aimbot FOV")) misc.drawAimbotFov = miscJson["Draw aimbot FOV"].asBool();
        if (miscJson.isMember("Autozeus")) misc.autoZeus = miscJson["Autozeus"].asBool();
        if (miscJson.isMember("Autozeus BAIM Only")) misc.autoZeusBaimOnly = miscJson["Autozeus BAIM Only"].asBool();
        if (miscJson.isMember("Autozeus Max Pen Dist")) misc.autoZeusMaxPenDist = miscJson["Autozeus Max Pen Dist"].asInt();
        if (miscJson.isMember("Custom Hit Sound")) misc.customHitSound = miscJson["Custom Hit Sound"].asString();
        if (miscJson.isMember("Kill sound")) misc.killSound = miscJson["Kill sound"].asInt();
        if (miscJson.isMember("Custom Kill Sound")) misc.customKillSound = miscJson["Custom Kill Sound"].asString();
        if (miscJson.isMember("Fake Duck")) misc.fakeDuck = miscJson["Fake Duck"].asBool();
        if (miscJson.isMember("Fake Duck Key")) misc.fakeDuckKey = miscJson["Fake Duck Key"].asInt();

        if (const auto& purchaseList = miscJson["Purchase List"]; purchaseList.isObject()) {
            if (const auto& enabled{ purchaseList["Enabled"] }; enabled.isBool())
                misc.purchaseList.enabled = enabled.asBool();
            if (const auto& onlyDuringFreezeTime{ purchaseList["Only During Freeze Time"] }; onlyDuringFreezeTime.isBool())
                misc.purchaseList.onlyDuringFreezeTime = onlyDuringFreezeTime.asBool();
            if (const auto& showPrices{ purchaseList["Show Prices"] }; showPrices.isBool())
                misc.purchaseList.showPrices = showPrices.asBool();
            if (const auto& noTitleBar{ purchaseList["No Title Bar"] }; noTitleBar.isBool())
                misc.purchaseList.noTitleBar = noTitleBar.asBool();
            if (const auto& mode{ purchaseList["Mode"] }; mode.isInt())
                misc.purchaseList.mode = mode.asInt();
        }
        if (miscJson.isMember("wpos Aimbot X")) misc.wposAimbotX = miscJson["wpos Aimbot X"].asFloat(); // "wpos Config" //
        if (miscJson.isMember("wpos Aimbot Y")) misc.wposAimbotY = miscJson["wpos Aimbot Y"].asFloat();
        if (miscJson.isMember("wpos AntiAim X")) misc.wposAntiAimX = miscJson["wpos AntiAim X"].asFloat();
        if (miscJson.isMember("wpos AntiAim Y")) misc.wposAntiAimY = miscJson["wpos AntiAim Y"].asFloat();
        if (miscJson.isMember("wpos TriggerBot X")) misc.wposTriggerBotX = miscJson["wpos TriggerBot X"].asFloat();
        if (miscJson.isMember("wpos TriggerBot Y")) misc.wposTriggerBotY = miscJson["wpos TriggerBot Y"].asFloat();
        if (miscJson.isMember("wpos Backtrack X")) misc.wposBacktrackX = miscJson["wpos Backtrack X"].asFloat();
        if (miscJson.isMember("wpos Backtrack Y")) misc.wposBacktrackY = miscJson["wpos Backtrack Y"].asFloat();
        if (miscJson.isMember("wpos Glow X")) misc.wposGlowX = miscJson["wpos Glow X"].asFloat();
        if (miscJson.isMember("wpos Glow Y")) misc.wposGlowY = miscJson["wpos Glow Y"].asFloat();
        if (miscJson.isMember("wpos Chams X")) misc.wposChamsX = miscJson["wpos Chams X"].asFloat();
        if (miscJson.isMember("wpos Chams Y")) misc.wposChamsY = miscJson["wpos Chams Y"].asFloat();
        if (miscJson.isMember("wpos ESP X")) misc.wposEspX = miscJson["wpos ESP X"].asFloat();
        if (miscJson.isMember("wpos ESP Y")) misc.wposEspY = miscJson["wpos ESP Y"].asFloat();
        if (miscJson.isMember("wpos Visuals X")) misc.wposVisualsX = miscJson["wpos Visuals X"].asFloat();
        if (miscJson.isMember("wpos Visuals Y")) misc.wposVisualsY = miscJson["wpos Visuals Y"].asFloat();
        if (miscJson.isMember("wpos Skinchanger X")) misc.wposSkinchangerX = miscJson["wpos Skinchanger X"].asFloat();
        if (miscJson.isMember("wpos Skinchanger Y")) misc.wposSkinchangerY = miscJson["wpos Skinchanger Y"].asFloat();
        if (miscJson.isMember("wpos Sound X")) misc.wposSoundX = miscJson["wpos Sound X"].asFloat();
        if (miscJson.isMember("wpos Sound Y")) misc.wposSoundY = miscJson["wpos Sound Y"].asFloat();
        if (miscJson.isMember("wpos Style X")) misc.wposStyleX = miscJson["wpos Style X"].asFloat();
        if (miscJson.isMember("wpos Style Y")) misc.wposStyleY = miscJson["wpos Style Y"].asFloat();
        if (miscJson.isMember("wpos Misc X")) misc.wposMiscX = miscJson["wpos Misc X"].asFloat();
        if (miscJson.isMember("wpos Misc Y")) misc.wposMiscY = miscJson["wpos Misc Y"].asFloat();
        if (miscJson.isMember("wpos Reportbot X")) misc.wposReportbotX = miscJson["wpos Reportbot X"].asFloat();
        if (miscJson.isMember("wpos Reportbot Y")) misc.wposReportbotY = miscJson["wpos Reportbot Y"].asFloat();
        if (miscJson.isMember("wpos Config X")) misc.wposConfigX = miscJson["wpos Config X"].asFloat();
        if (miscJson.isMember("wpos Config Y")) misc.wposConfigY = miscJson["wpos Config Y"].asFloat();
        if (miscJson.isMember("wpos Style2 X")) misc.wposStyle2X = miscJson["wpos Style2 X"].asFloat();
        if (miscJson.isMember("wpos Style2 Y")) misc.wposStyle2Y = miscJson["wpos Style2 Y"].asFloat();
        if (miscJson.isMember("wpos PurchaseList X")) misc.wposPurchaseListX = miscJson["wpos PurchaseList X"].asFloat();
        if (miscJson.isMember("wpos PurchaseList Y")) misc.wposPurchaseListY = miscJson["wpos PurchaseList Y"].asFloat();
        if (miscJson.isMember("wpos PurchaseList ScaleX")) misc.wposPurchaseListScaleX = miscJson["wpos PurchaseList ScaleX"].asFloat();
        if (miscJson.isMember("wpos PurchaseList ScaleY")) misc.wposPurchaseListScaleY = miscJson["wpos PurchaseList ScaleY"].asFloat();
        if (miscJson.isMember("wpos SpectatorList X")) misc.wposSpectatorListX = miscJson["wpos SpectatorList X"].asFloat();
        if (miscJson.isMember("wpos SpectatorList Y")) misc.wposSpectatorListY = miscJson["wpos SpectatorList Y"].asFloat();
        if (miscJson.isMember("wpos SpectatorList ScaleX")) misc.wposSpectatorListScaleX = miscJson["wpos SpectatorList ScaleX"].asFloat();
        if (miscJson.isMember("wpos SpectatorList ScaleY")) misc.wposSpectatorListScaleY = miscJson["wpos SpectatorList ScaleY"].asFloat();
        if (miscJson.isMember("wpos Aimbot Lock")) misc.wposLockSelectedFlags[0] = miscJson["wpos Aimbot Lock"].asBool();   // "wpos Locks" //
        if (miscJson.isMember("wpos Anti Aim Lock")) misc.wposLockSelectedFlags[1] = miscJson["wpos Anti Aim Lock"].asBool();
        if (miscJson.isMember("wpos Triggerbot Lock")) misc.wposLockSelectedFlags[2] = miscJson["wpos Triggerbot Lock"].asBool();
        if (miscJson.isMember("wpos Backtrack Lock")) misc.wposLockSelectedFlags[3] = miscJson["wpos Backtrack Lock"].asBool();
        if (miscJson.isMember("wpos Glow Lock")) misc.wposLockSelectedFlags[4] = miscJson["wpos Glow Lock"].asBool();
        if (miscJson.isMember("wpos Chams Lock")) misc.wposLockSelectedFlags[5] = miscJson["wpos Chams Lock"].asBool();
        if (miscJson.isMember("wpos Esp Lock")) misc.wposLockSelectedFlags[6] = miscJson["wpos Esp Lock"].asBool();
        if (miscJson.isMember("wpos Visuals Lock")) misc.wposLockSelectedFlags[7] = miscJson["wpos Visuals Lock"].asBool();
        if (miscJson.isMember("wpos Skinchanger Lock")) misc.wposLockSelectedFlags[8] = miscJson["wpos Skinchanger Lock"].asBool();
        if (miscJson.isMember("wpos Sound Lock")) misc.wposLockSelectedFlags[9] = miscJson["wpos Sound Lock"].asBool();
        if (miscJson.isMember("wpos Style Lock")) misc.wposLockSelectedFlags[10] = miscJson["wpos Style Lock"].asBool();
        if (miscJson.isMember("wpos Misc Lock")) misc.wposLockSelectedFlags[11] = miscJson["wpos Misc Lock"].asBool();
        if (miscJson.isMember("wpos Reportbot Lock")) misc.wposLockSelectedFlags[12] = miscJson["wpos Reportbot Lock"].asBool();
        if (miscJson.isMember("wpos Config Lock")) misc.wposLockSelectedFlags[13] = miscJson["wpos Config Lock"].asBool();
        if (miscJson.isMember("wpos Style2 Lock")) misc.wposLockSelectedFlags[14] = miscJson["wpos Style2 Lock"].asBool();
        if (miscJson.isMember("wpos PurchaseList Lock")) misc.wposLockSelectedFlags[15] = miscJson["wpos PurchaseList Lock"].asBool();
        if (miscJson.isMember("wpos SpectatorList Lock")) misc.wposLockSelectedFlags[16] = miscJson["wpos SpectatorList Lock"].asBool();


    }

    {
        const auto& reportbotJson = json["Reportbot"];

        if (reportbotJson.isMember("Enabled")) reportbot.enabled = reportbotJson["Enabled"].asBool();
        if (reportbotJson.isMember("Target")) reportbot.target = reportbotJson["Target"].asInt();
        if (reportbotJson.isMember("Delay")) reportbot.delay = reportbotJson["Delay"].asInt();
        if (reportbotJson.isMember("Rounds")) reportbot.rounds = reportbotJson["Rounds"].asInt();
        if (reportbotJson.isMember("Abusive Communications")) reportbot.textAbuse = reportbotJson["Abusive Communications"].asBool();
        if (reportbotJson.isMember("Griefing")) reportbot.griefing = reportbotJson["Griefing"].asBool();
        if (reportbotJson.isMember("Wall Hacking")) reportbot.wallhack = reportbotJson["Wall Hacking"].asBool();
        if (reportbotJson.isMember("Aim Hacking")) reportbot.aimbot = reportbotJson["Aim Hacking"].asBool();
        if (reportbotJson.isMember("Other Hacking")) reportbot.other = reportbotJson["Other Hacking"].asBool();
    }
}

void Config::save(size_t id) const noexcept
{
    Json::Value json;

    for (size_t i = 0; i < aimbot.size(); i++) {
        auto& aimbotJson = json["Aimbot"][i];
        const auto& aimbotConfig = aimbot[i];

        aimbotJson["Enabled"] = aimbotConfig.enabled;
        aimbotJson["On key"] = aimbotConfig.onKey;
        aimbotJson["Key"] = aimbotConfig.key;
        aimbotJson["Key mode"] = aimbotConfig.keyMode;
        aimbotJson["Aimlock"] = aimbotConfig.aimlock;
        aimbotJson["Silent"] = aimbotConfig.silent;
        aimbotJson["Friendly fire"] = aimbotConfig.friendlyFire;
        aimbotJson["Visible only"] = aimbotConfig.visibleOnly;
        aimbotJson["Scoped only"] = aimbotConfig.scopedOnly;
        aimbotJson["Ignore flash"] = aimbotConfig.ignoreFlash;;
        aimbotJson["Ignore smoke"] = aimbotConfig.ignoreSmoke;
        aimbotJson["Auto shot"] = aimbotConfig.autoShot;
        aimbotJson["Auto scope"] = aimbotConfig.autoScope;
        aimbotJson["Fov"] = aimbotConfig.fov;
        aimbotJson["Smooth"] = aimbotConfig.smooth;
        aimbotJson["Bone"] = aimbotConfig.bone;
        aimbotJson["Recoil control X"] = aimbotConfig.recoilControlX;
        aimbotJson["Recoil control Y"] = aimbotConfig.recoilControlY;
        aimbotJson["Hitchance"] = aimbotConfig.hitchance;
        aimbotJson["Min damage"] = aimbotConfig.minDamage;
        aimbotJson["Killshot"] = aimbotConfig.killshot;
        aimbotJson["Between shots"] = aimbotConfig.betweenShots;
        aimbotJson["First Shot Delay"] = aimbotConfig.firstShotDelay;
        aimbotJson["Kill Delay"] = aimbotConfig.killDelay;
        aimbotJson["Standalone RCS"] = aimbotConfig.standaloneRCS;
        aimbotJson["Standalone RCS Ignore Shots"] = aimbotConfig.shotsFired;
    }

    for (size_t i = 0; i < triggerbot.size(); i++) {
        auto& triggerbotJson = json["Triggerbot"][i];
        const auto& triggerbotConfig = triggerbot[i];

        triggerbotJson["Enabled"] = triggerbotConfig.enabled;
        triggerbotJson["On key"] = triggerbotConfig.onKey;
        triggerbotJson["Key"] = triggerbotConfig.key;
        triggerbotJson["Friendly fire"] = triggerbotConfig.friendlyFire;
        triggerbotJson["Scoped only"] = triggerbotConfig.scopedOnly;
        triggerbotJson["Hitchance enable"] = triggerbotConfig.hitChance;
        triggerbotJson["Hitchance ammount"] = triggerbotConfig.hitChanceAmmount;
        triggerbotJson["Ignore flash"] = triggerbotConfig.ignoreFlash;
        triggerbotJson["Ignore smoke"] = triggerbotConfig.ignoreSmoke;
        triggerbotJson["Hitgroup"] = triggerbotConfig.hitgroup;
        triggerbotJson["Shot delay"] = triggerbotConfig.shotDelay;
        triggerbotJson["Min damage"] = triggerbotConfig.minDamage;
        triggerbotJson["Killshot"] = triggerbotConfig.killshot;
        triggerbotJson["Burst Time"] = triggerbotConfig.burstTime;
    }

    {
        auto& backtrackJson = json["Backtrack"];
        backtrackJson["Enabled"] = backtrack.enabled;
        backtrackJson["Ignore smoke"] = backtrack.ignoreSmoke;
        backtrackJson["Recoil based fov"] = backtrack.recoilBasedFov;
        backtrackJson["Time limit"] = backtrack.timeLimit;
        backtrackJson["Fake Latency"] = backtrack.fakeLatency;
        backtrackJson["Latency Ammount"] = backtrack.fakeLatencyAmmount;
    }

    {
        auto& antiAimJson = json["Anti aim"];
        antiAimJson["Enabled"] = antiAim.enabled;
        antiAimJson["Resolver"] = antiAim.resolveall;
        antiAimJson["Pitch Resolver"] = antiAim.pitchResolver;
        antiAimJson["Pitch"] = antiAim.pitch;
        antiAimJson["Pitch angle"] = antiAim.pitchAngle;
        antiAimJson["Yaw"] = antiAim.yaw;
        antiAimJson["Yaw angle"] = antiAim.yawAngle;
        antiAimJson["Inverse Yaw Key"] = antiAim.yawInverseAngleKey;
        antiAimJson["Inverse Yaw Key Mode"] = antiAim.yawInverseKeyMode;
        antiAimJson["Yaw Real"] = antiAim.yawReal;
        antiAimJson["Body Lean"] = antiAim.bodyLean;
        antiAimJson["AntiAim Mode"] = antiAim.mode;
        antiAimJson["Jitter Max"] = antiAim.jitterMax;
        antiAimJson["Jitter Min"] = antiAim.jitterMin;
        antiAimJson["LBY Breaker"] = antiAim.LBYBreaker;
        antiAimJson["LBY Angle"] = antiAim.LBYAngle;
        auto& fakeWalkJson = json["FakeWalk"];
        fakeWalkJson["Enabled"] = antiAim.fakeWalk.enabled;
        fakeWalkJson["Fakewalk Key"] = antiAim.fakeWalk.key;
        fakeWalkJson["Fakewalk Key Mode"] = antiAim.fakeWalk.keyMode;
        fakeWalkJson["Fakewalk Max Choke"] = antiAim.fakeWalk.maxChoke;
    }

    for (size_t i = 0; i < glow.size(); i++) {
        auto& glowJson = json["glow"][i];
        const auto& glowConfig = glow[i];

        glowJson["Enabled"] = glowConfig.enabled;
        glowJson["healthBased"] = glowConfig.healthBased;
        glowJson["thickness"] = glowConfig.thickness;
        glowJson["alpha"] = glowConfig.alpha;
        glowJson["style"] = glowConfig.style;

        {
            auto& colorJson = glowJson["Color"];
            const auto& colorConfig = glowConfig.color;

            colorJson["Color"][0] = colorConfig.color[0];
            colorJson["Color"][1] = colorConfig.color[1];
            colorJson["Color"][2] = colorConfig.color[2];

            colorJson["Rainbow"] = colorConfig.rainbow;
            colorJson["Rainbow speed"] = colorConfig.rainbowSpeed;
        }
    }

    for (size_t i = 0; i < chams.size(); i++) {
        auto& chamsJson = json["Chams"][i];
        const auto& chamsConfig = chams[i];

        for (size_t j = 0; j < chamsConfig.materials.size(); j++) {
            auto& materialsJson = chamsJson[j];
            const auto& materialsConfig = chams[i].materials[j];

            materialsJson["Enabled"] = materialsConfig.enabled;
            materialsJson["Health based"] = materialsConfig.healthBased;
            materialsJson["Blinking"] = materialsConfig.blinking;
            materialsJson["Material"] = materialsConfig.material;
            materialsJson["Wireframe"] = materialsConfig.wireframe;

            {
                auto& colorJson = materialsJson["Color"];
                const auto& colorConfig = materialsConfig; // leftover

                colorJson["Color"][0] = colorConfig.color[0];
                colorJson["Color"][1] = colorConfig.color[1];
                colorJson["Color"][2] = colorConfig.color[2];
                colorJson["Color"][3] = colorConfig.color[3];

                colorJson["Rainbow"] = colorConfig.rainbow;
                colorJson["Rainbow speed"] = colorConfig.rainbowSpeed;
            }
        }
    }

    for (size_t i = 0; i < esp.players.size(); i++) {
        auto& espJson = json["Esp"]["Players"][i];
        const auto& espConfig = esp.players[i];

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        espJson["HP side"] = espConfig.hpside;
        espJson["Armor side"] = espConfig.armorside;

        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& eyeTracesJson = espJson["Eye traces"];
            const auto& eyeTracesConfig = espConfig.eyeTraces;

            eyeTracesJson["Enabled"] = eyeTracesConfig.enabled;
            eyeTracesJson["Color"][0] = eyeTracesConfig.color[0];
            eyeTracesJson["Color"][1] = eyeTracesConfig.color[1];
            eyeTracesJson["Color"][2] = eyeTracesConfig.color[2];
            eyeTracesJson["Rainbow"] = eyeTracesConfig.rainbow;
            eyeTracesJson["Rainbow speed"] = eyeTracesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& healthJson = espJson["Health"];
            const auto& healthConfig = espConfig.health;

            healthJson["Enabled"] = healthConfig.enabled;
            healthJson["Color"][0] = healthConfig.color[0];
            healthJson["Color"][1] = healthConfig.color[1];
            healthJson["Color"][2] = healthConfig.color[2];
            healthJson["Rainbow"] = healthConfig.rainbow;
            healthJson["Rainbow speed"] = healthConfig.rainbowSpeed;
        }

        {
            auto& healthBarJson = espJson["Health bar"];
            const auto& healthBarConfig = espConfig.healthBar;

            healthBarJson["Enabled"] = healthBarConfig.enabled;
            healthBarJson["Color"][0] = healthBarConfig.color[0];
            healthBarJson["Color"][1] = healthBarConfig.color[1];
            healthBarJson["Color"][2] = healthBarConfig.color[2];
            healthBarJson["Rainbow"] = healthBarConfig.rainbow;
            healthBarJson["Rainbow speed"] = healthBarConfig.rainbowSpeed;
        }

        {
            auto& armorJson = espJson["Armor"];
            const auto& armorConfig = espConfig.armor;

            armorJson["Enabled"] = armorConfig.enabled;
            armorJson["Color"][0] = armorConfig.color[0];
            armorJson["Color"][1] = armorConfig.color[1];
            armorJson["Color"][2] = armorConfig.color[2];
            armorJson["Rainbow"] = armorConfig.rainbow;
            armorJson["Rainbow speed"] = armorConfig.rainbowSpeed;
        }

        {
            auto& armorBarJson = espJson["Armor bar"];
            const auto& armorBarConfig = espConfig.armorBar;

            armorBarJson["Enabled"] = armorBarConfig.enabled;
            armorBarJson["Color"][0] = armorBarConfig.color[0];
            armorBarJson["Color"][1] = armorBarConfig.color[1];
            armorBarJson["Color"][2] = armorBarConfig.color[2];
            armorBarJson["Rainbow"] = armorBarConfig.rainbow;
            armorBarJson["Rainbow speed"] = armorBarConfig.rainbowSpeed;
        }

        {
            auto& moneyJson = espJson["Money"];
            const auto& moneyConfig = espConfig.money;

            moneyJson["Enabled"] = moneyConfig.enabled;
            moneyJson["Color"][0] = moneyConfig.color[0];
            moneyJson["Color"][1] = moneyConfig.color[1];
            moneyJson["Color"][2] = moneyConfig.color[2];
            moneyJson["Rainbow"] = moneyConfig.rainbow;
            moneyJson["Rainbow speed"] = moneyConfig.rainbowSpeed;
        }
    

        {
            auto& headDotJson = espJson["Head dot"];
            const auto& headDotConfig = espConfig.headDot;

            headDotJson["Enabled"] = headDotConfig.enabled;
            headDotJson["Color"][0] = headDotConfig.color[0];
            headDotJson["Color"][1] = headDotConfig.color[1];
            headDotJson["Color"][2] = headDotConfig.color[2];
            headDotJson["Rainbow"] = headDotConfig.rainbow;
            headDotJson["Rainbow speed"] = headDotConfig.rainbowSpeed;
        }

        {
            auto& activeWeaponJson = espJson["Active weapon"];
            const auto& activeWeaponConfig = espConfig.activeWeapon;

            activeWeaponJson["Enabled"] = activeWeaponConfig.enabled;
            activeWeaponJson["Color"][0] = activeWeaponConfig.color[0];
            activeWeaponJson["Color"][1] = activeWeaponConfig.color[1];
            activeWeaponJson["Color"][2] = activeWeaponConfig.color[2];
            activeWeaponJson["Rainbow"] = activeWeaponConfig.rainbow;
            activeWeaponJson["Rainbow speed"] = activeWeaponConfig.rainbowSpeed;
        }

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }
        {
            auto& ammoJson = espJson["Ammo"];
            const auto& ammoConfig = espConfig.ammo;

            ammoJson["Enabled"] = ammoConfig.enabled;
            ammoJson["Color"][0] = ammoConfig.color[0];
            ammoJson["Color"][1] = ammoConfig.color[1];
            ammoJson["Color"][2] = ammoConfig.color[2];
            ammoJson["Rainbow"] = ammoConfig.rainbow;
            ammoJson["Rainbow speed"] = ammoConfig.rainbowSpeed;
        }
        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Dead ESP"] = espConfig.deadesp;
        espJson["Max distance"] = espConfig.maxDistance;
    }

    {
        auto& espJson = json["Esp"]["Weapons"];
        const auto& espConfig = esp.weapon;

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Max distance"] = espConfig.maxDistance;
    }

    for (size_t i = 0; i < esp.dangerZone.size(); i++) {
        auto& espJson = json["Esp"]["Danger Zone"][i];
        const auto& espConfig = esp.dangerZone[i];

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Max distance"] = espConfig.maxDistance;
    }

    for (size_t i = 0; i < esp.projectiles.size(); i++) {
        auto& espJson = json["Esp"]["Projectiles"][i];
        const auto& espConfig = esp.projectiles[i];

        espJson["Enabled"] = espConfig.enabled;
        espJson["Font"] = espConfig.font;
        {
            auto& snaplinesJson = espJson["Snaplines"];
            const auto& snaplinesConfig = espConfig.snaplines;

            snaplinesJson["Enabled"] = snaplinesConfig.enabled;
            snaplinesJson["Color"][0] = snaplinesConfig.color[0];
            snaplinesJson["Color"][1] = snaplinesConfig.color[1];
            snaplinesJson["Color"][2] = snaplinesConfig.color[2];
            snaplinesJson["Rainbow"] = snaplinesConfig.rainbow;
            snaplinesJson["Rainbow speed"] = snaplinesConfig.rainbowSpeed;
        }

        {
            auto& boxJson = espJson["Box"];
            const auto& boxConfig = espConfig.box;

            boxJson["Enabled"] = boxConfig.enabled;
            boxJson["Color"][0] = boxConfig.color[0];
            boxJson["Color"][1] = boxConfig.color[1];
            boxJson["Color"][2] = boxConfig.color[2];
            boxJson["Rainbow"] = boxConfig.rainbow;
            boxJson["Rainbow speed"] = boxConfig.rainbowSpeed;
        }

        espJson["Box type"] = espConfig.boxType;

        {
            auto& outlineJson = espJson["Outline"];
            const auto& outlineConfig = espConfig.outline;

            outlineJson["Enabled"] = outlineConfig.enabled;
            outlineJson["Color"][0] = outlineConfig.color[0];
            outlineJson["Color"][1] = outlineConfig.color[1];
            outlineJson["Color"][2] = outlineConfig.color[2];
            outlineJson["Rainbow"] = outlineConfig.rainbow;
            outlineJson["Rainbow speed"] = outlineConfig.rainbowSpeed;
        }

        {
            auto& nameJson = espJson["Name"];
            const auto& nameConfig = espConfig.name;

            nameJson["Enabled"] = nameConfig.enabled;
            nameJson["Color"][0] = nameConfig.color[0];
            nameJson["Color"][1] = nameConfig.color[1];
            nameJson["Color"][2] = nameConfig.color[2];
            nameJson["Rainbow"] = nameConfig.rainbow;
            nameJson["Rainbow speed"] = nameConfig.rainbowSpeed;
        }

        {
            auto& distanceJson = espJson["Distance"];
            const auto& distanceConfig = espConfig.distance;

            distanceJson["Enabled"] = distanceConfig.enabled;
            distanceJson["Color"][0] = distanceConfig.color[0];
            distanceJson["Color"][1] = distanceConfig.color[1];
            distanceJson["Color"][2] = distanceConfig.color[2];
            distanceJson["Rainbow"] = distanceConfig.rainbow;
            distanceJson["Rainbow speed"] = distanceConfig.rainbowSpeed;
        }

        espJson["Max distance"] = espConfig.maxDistance;
    }

    {
        auto& visualsJson = json["visuals"];
        visualsJson["disablePostProcessing"] = visuals.disablePostProcessing;
        visualsJson["inverseRagdollGravity"] = visuals.inverseRagdollGravity;
        visualsJson["noFog"] = visuals.noFog;
        visualsJson["no3dSky"] = visuals.no3dSky;
        visualsJson["No aim punch"] = visuals.noAimPunch;
        visualsJson["No view punch"] = visuals.noViewPunch;
        visualsJson["noHands"] = visuals.noHands;
        visualsJson["noSleeves"] = visuals.noSleeves;
        visualsJson["noWeapons"] = visuals.noWeapons;
        visualsJson["noSmoke"] = visuals.noSmoke;
        visualsJson["noBlur"] = visuals.noBlur;
        visualsJson["noBloom"] = visuals.noBloom;
        visualsJson["noScopeOverlay"] = visuals.noScopeOverlay;
        visualsJson["noGrass"] = visuals.noGrass;
        visualsJson["noShadows"] = visuals.noShadows;
        visualsJson["wireframeSmoke"] = visuals.wireframeSmoke;
        visualsJson["Zoom"] = visuals.zoom;
        visualsJson["Zoom key"] = visuals.zoomKey;
        visualsJson["thirdperson"] = visuals.thirdperson;
        visualsJson["Night Mode"] = visuals.nightMode;
        visualsJson["thirdpersonKey"] = visuals.thirdpersonKey;
        visualsJson["thirdpersonDistance"] = visuals.thirdpersonDistance;
        visualsJson["viewmodelFov"] = visuals.viewmodelFov;
        visualsJson["Fov"] = visuals.fov;
        visualsJson["farZ"] = visuals.farZ;
        visualsJson["flashReduction"] = visuals.flashReduction;
        visualsJson["brightness"] = visuals.brightness;
        visualsJson["skybox"] = visuals.skybox;
        visualsJson["customSkybox"] = visuals.customSkybox;
        visualsJson["Thirdperson Mode"] = config->antiAim.thirdpersonMode;
        visualsJson["Indicators Enabled"] = visuals.indicatorsEnabled;
        visualsJson["Desync Indicator"] = visuals.selectedIndicators[0];
        visualsJson["LBY Indicator"] = visuals.selectedIndicators[1];
        visualsJson["Fakelag Indicator"] = visuals.selectedIndicators[2];
        visualsJson["Fakeduck Indicator"] = visuals.selectedIndicators[3];

        {   
            auto& worldJson = visualsJson["World"];
            worldJson["Enabled"] = visuals.world.enabled;
            worldJson["Color"][0] = visuals.world.color[0];
            worldJson["Color"][1] = visuals.world.color[1];
            worldJson["Color"][2] = visuals.world.color[2];
            worldJson["Rainbow"] = visuals.world.rainbow;
            worldJson["Rainbow speed"] = visuals.world.rainbowSpeed;
        }

        {
            auto& skyJson = visualsJson["Sky"];
            skyJson["Enabled"] = visuals.sky.enabled;
            skyJson["Color"][0] = visuals.sky.color[0];
            skyJson["Color"][1] = visuals.sky.color[1];
            skyJson["Color"][2] = visuals.sky.color[2];
            skyJson["Rainbow"] = visuals.sky.rainbow;
            skyJson["Rainbow speed"] = visuals.sky.rainbowSpeed;
        }

        {
            auto& skyJson = visualsJson["Bullet Beams"];
            skyJson["Enabled"] = visuals.bulletTracers.enabled;
            skyJson["Color"][0] = visuals.bulletTracers.color[0];
            skyJson["Color"][1] = visuals.bulletTracers.color[1];
            skyJson["Color"][2] = visuals.bulletTracers.color[2];
            skyJson["Rainbow"] = visuals.bulletTracers.rainbow;
            skyJson["Rainbow speed"] = visuals.bulletTracers.rainbowSpeed;
        }

        visualsJson["Deagle spinner"] = visuals.deagleSpinner;
        visualsJson["Screen effect"] = visuals.screenEffect;
        visualsJson["Hit effect"] = visuals.hitEffect;
        visualsJson["Hit effect time"] = visuals.hitEffectTime;
        visualsJson["Hit marker"] = visuals.hitMarker;
        visualsJson["Hit marker time"] = visuals.hitMarkerTime;
        {
            auto& hitMarkerIndicator = visualsJson["Hit marker Damage Indicator"];
            hitMarkerIndicator["Enabled"] = visuals.hitMarkerDamageIndicator.enabled;
            hitMarkerIndicator["Color"][0] = visuals.hitMarkerDamageIndicator.color[0];
            hitMarkerIndicator["Color"][1] = visuals.hitMarkerDamageIndicator.color[1];
            hitMarkerIndicator["Color"][2] = visuals.hitMarkerDamageIndicator.color[2];
            hitMarkerIndicator["Rainbow"] = visuals.hitMarkerDamageIndicator.rainbow;
            hitMarkerIndicator["Rainbow speed"] = visuals.hitMarkerDamageIndicator.rainbowSpeed;
        }
        visualsJson["Customize Hitmarker"] = visuals.hitMarkerDamageIndicatorCustomize;
        visualsJson["Font"] = visuals.hitMarkerDamageIndicatorFont;
        visualsJson["Alpha"] = visuals.hitMarkerDamageIndicatorAlpha;
        visualsJson["Dist"] = visuals.hitMarkerDamageIndicatorDist;
        visualsJson["Text X"] = visuals.hitMarkerDamageIndicatorTextX;
        visualsJson["Text Y"] = visuals.hitMarkerDamageIndicatorTextY;
        visualsJson["Ratio"] = visuals.hitMarkerDamageIndicatorRatio;
        //hitmarker ends here
        {
            auto& velocityConfig = visualsJson["Show Velocity"];
            velocityConfig["Enabled"] = visuals.showvelocity.enabled;
            velocityConfig["Color"][0] = visuals.showvelocity.color[0];
            velocityConfig["Color"][1] = visuals.showvelocity.color[1];
            velocityConfig["Color"][2] = visuals.showvelocity.color[2];
            velocityConfig["Rainbow"] = visuals.showvelocity.rainbow;
            velocityConfig["Rainbow speed"] = visuals.showvelocity.rainbowSpeed;
        }
        visualsJson["Custom Pos"] = visuals.showvelocityM;
        visualsJson["Res X"] = visuals.showvelocityResX;
        visualsJson["Res Y"] = visuals.showvelocityResY;
        //velocity ends here
        visualsJson["Playermodel T"] = visuals.playerModelT;
        visualsJson["Playermodel CT"] = visuals.playerModelCT;

        {
            auto& cc = visualsJson["Color correction"];
            cc["Enabled"] = visuals.colorCorrection.enabled;
            cc["Blue"] = visuals.colorCorrection.blue;
            cc["Red"] = visuals.colorCorrection.red;
            cc["Mono"] = visuals.colorCorrection.mono;
            cc["Saturation"] = visuals.colorCorrection.saturation;
            cc["Ghost"] = visuals.colorCorrection.ghost;
            cc["Green"] = visuals.colorCorrection.green;
            cc["Yellow"] = visuals.colorCorrection.yellow;
        }
    }

    for (size_t i = 0; i < skinChanger.size(); i++) {
        auto& skinChangerJson = json["skinChanger"][i];
        const auto& skinChangerConfig = skinChanger[i];

        skinChangerJson["Enabled"] = skinChangerConfig.enabled;
        skinChangerJson["definition_vector_index"] = skinChangerConfig.itemIdIndex;
        skinChangerJson["definition_index"] = skinChangerConfig.itemId;
        skinChangerJson["entity_quality_vector_index"] = skinChangerConfig.entity_quality_vector_index;
        skinChangerJson["entity_quality_index"] = skinChangerConfig.quality;
        skinChangerJson["paint_kit_vector_index"] = skinChangerConfig.paint_kit_vector_index;
        skinChangerJson["paint_kit_index"] = skinChangerConfig.paintKit;
        skinChangerJson["definition_override_vector_index"] = skinChangerConfig.definition_override_vector_index;
        skinChangerJson["definition_override_index"] = skinChangerConfig.definition_override_index;
        skinChangerJson["seed"] = skinChangerConfig.seed;
        skinChangerJson["stat_trak"] = skinChangerConfig.stat_trak;
        skinChangerJson["wear"] = skinChangerConfig.wear;
        skinChangerJson["custom_name"] = skinChangerConfig.custom_name;

        for (size_t j = 0; j < skinChangerConfig.stickers.size(); j++) {
            auto& stickerJson = skinChangerJson["stickers"][j];
            const auto& stickerConfig = skinChangerConfig.stickers[j];

            stickerJson["kit"] = stickerConfig.kit;
            stickerJson["kit_vector_index"] = stickerConfig.kit_vector_index;
            stickerJson["wear"] = stickerConfig.wear;
            stickerJson["scale"] = stickerConfig.scale;
            stickerJson["rotation"] = stickerConfig.rotation;
        }
    }

    {
        auto& soundJson = json["Sound"];

        soundJson["Chicken volume"] = sound.chickenVolume;

        for (size_t i = 0; i < sound.players.size(); i++) {
            auto& playerJson = soundJson["Players"][i];
            const auto& playerConfig = sound.players[i];

            playerJson["Master volume"] = playerConfig.masterVolume;
            playerJson["Headshot volume"] = playerConfig.headshotVolume;
            playerJson["Weapon volume"] = playerConfig.weaponVolume;
            playerJson["Footstep volume"] = playerConfig.footstepVolume;
        }
    }

    {
        auto& styleJson = json["Style"];

        styleJson["Menu style"] = style.menuStyle;
        styleJson["Menu colors"] = style.menuColors;

        auto& colorsJson = styleJson["Colors"];

        const ImGuiStyle& style = ImGui::GetStyle();

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            auto& colorJson = styleJson["Colors"][ImGui::GetStyleColorName(i)];
            colorJson[0] = style.Colors[i].x;
            colorJson[1] = style.Colors[i].y;
            colorJson[2] = style.Colors[i].z;
            colorJson[3] = style.Colors[i].w;
        }
    }

    {
        auto& miscJson = json["Misc"];

        miscJson["Menu key"] = misc.menuKey;
        miscJson["Anti AFK kick"] = misc.antiAfkKick;
       // miscJson["resolver"] = misc.resolver;
        miscJson["Auto strafe"] = misc.autoStrafe;
        miscJson["Strafe Key"] = misc.autoStrafeKey;
        miscJson["Strafe Style"] = misc.autoStrafeStyle;
        miscJson["Bunny hop"] = misc.bunnyHop;
        miscJson["Custom View"] = misc.CustomViewModel;
        miscJson["View x"] = misc.viewmodel_x;
        miscJson["View y"] = misc.viewmodel_y;
        miscJson["View z"] = misc.viewmodel_z;
        miscJson["Door Spam"] = misc.usespam;
        miscJson["BlockBot"] = misc.playerBlocker;
        miscJson["BlockBot Key"] = misc.playerBlockerKey;
        miscJson["Custom clan tag"] = misc.customClanTag;
        miscJson["Clock tag"] = misc.clocktag;
        miscJson["Clan tag"] = misc.clanTag;
        miscJson["Animated clan tag"] = misc.animatedClanTag;
        miscJson["Fast duck"] = misc.fastDuck;
        miscJson["Fast Stop"] = misc.fastStop;
        miscJson["Moonwalk"] = misc.moonwalk;
        miscJson["Jumpbug"] = misc.jumpbug;
        miscJson["Jumpbug Key"] = misc.jumpbugkey;
        miscJson["Edge Jump"] = misc.edgejump;
        miscJson["Edge Jump Key"] = misc.edgejumpkey;
     
        miscJson["Slowwalk"] = misc.slowwalk;
        miscJson["Slowwalk key"] = misc.slowwalkKey;
        miscJson["Sniper crosshair"] = misc.sniperCrosshair;
        miscJson["Recoil crosshair"] = misc.recoilCrosshair;
        miscJson["Auto pistol"] = misc.autoPistol;
        miscJson["Auto reload"] = misc.autoReload;
        miscJson["Auto accept"] = misc.autoAccept;
        miscJson["Radar hack"] = misc.radarHack;
        miscJson["Reveal ranks"] = misc.revealRanks;
        miscJson["Reveal money"] = misc.revealMoney;
        miscJson["Reveal suspect"] = misc.revealSuspect;
        miscJson["Spammer"] = misc.chat_spammer;
        miscJson["Spectator List"] = misc.spectatorList;
        miscJson["Watermark"] = misc.watermark;

        /*{
            auto& watermarkJson = miscJson["Watermark"];
            watermarkJson["Enabled"] = misc.watermark.enabled;
            watermarkJson["Color"][0] = misc.watermark.color[0];
            watermarkJson["Color"][1] = misc.watermark.color[1];
            watermarkJson["Color"][2] = misc.watermark.color[2];
            watermarkJson["Rainbow"] = misc.watermark.rainbow;
            watermarkJson["Rainbow speed"] = misc.watermark.rainbowSpeed;
        }*/

        miscJson["Fix animation LOD"] = misc.fixAnimationLOD;
        miscJson["Fix bone matrix"] = misc.fixBoneMatrix;
        miscJson["Fix movement"] = misc.fixMovement;
        miscJson["Disable model occlusion"] = misc.disableModelOcclusion;
        miscJson["Aspect Ratio"] = misc.aspectratio;
        miscJson["Kill message"] = misc.killMessage;
        miscJson["Kill message string"] = misc.killMessageString;
        miscJson["Name stealer"] = misc.nameStealer;
        miscJson["Disable HUD blur"] = misc.disablePanoramablur;
        miscJson["Ban color"] = misc.banColor;
        miscJson["Ban text"] = misc.banText;
        miscJson["Fast plant"] = misc.fastPlant;

        {
            auto& bombTimerJson = miscJson["Bomb timer"];
            bombTimerJson["Enabled"] = misc.bombTimer.enabled;
            bombTimerJson["Color"][0] = misc.bombTimer.color[0];
            bombTimerJson["Color"][1] = misc.bombTimer.color[1];
            bombTimerJson["Color"][2] = misc.bombTimer.color[2];
            bombTimerJson["Rainbow"] = misc.bombTimer.rainbow;
            bombTimerJson["Rainbow speed"] = misc.bombTimer.rainbowSpeed;
        }

        miscJson["Bomb Damage"] = misc.bombDmg;
        miscJson["Quick reload"] = misc.quickReload;
        miscJson["Prepare revolver"] = misc.prepareRevolver;
        miscJson["Prepare revolver key"] = misc.prepareRevolverKey;
        miscJson["Hit sound"] = misc.hitSound;
        miscJson["Fakelag Ticks"] = misc.fakeLagTicks;
        miscJson["Fakelag Mode"] = misc.fakeLagMode;
        miscJson["Fakelag While Shooting"] = misc.fakeLagSelectedFlags[0];
        miscJson["Fakelag While Standing"] = misc.fakeLagSelectedFlags[1];
        miscJson["Fakelag While Moving"] = misc.fakeLagSelectedFlags[2];
        miscJson["Fakelag In Air"] = misc.fakeLagSelectedFlags[3];
        miscJson["Fakelag key"] = misc.fakeLagKey;
        miscJson["Quick healthshot key"] = misc.quickHealthshotKey;
        miscJson["Grenade predict"] = misc.nadePredict;
        miscJson["Fix tablet signal"] = misc.fixTabletSignal;
        miscJson["Max angle delta"] = misc.maxAngleDelta;
        miscJson["Fake prime"] = misc.fakePrime;
        miscJson["Sv pure bypass"] = misc.svpurebypass;
        miscJson["Draw aimbot FOV"] = misc.drawAimbotFov;
        miscJson["Draw aimbot Actual FOV"] = misc.actualFov;
        miscJson["Autozeus"] = misc.autoZeus;
        miscJson["Autozeus BAIM Only"] = misc.autoZeusBaimOnly;
        miscJson["Autozeus Max Pen Dist"] = misc.autoZeusMaxPenDist;
        miscJson["Custom Hit Sound"] = misc.customHitSound;
        miscJson["Kill sound"] = misc.killSound;
        miscJson["Custom Kill Sound"] = misc.customKillSound;
        miscJson["Fake Duck"] = misc.fakeDuck;
        miscJson["Fake Duck Key"] = misc.fakeDuckKey;

        {
            auto& purchaseListJson = miscJson["Purchase List"];
            purchaseListJson["Enabled"] = misc.purchaseList.enabled;
            purchaseListJson["Only During Freeze Time"] = misc.purchaseList.onlyDuringFreezeTime;
            purchaseListJson["Show Prices"] = misc.purchaseList.showPrices;
            purchaseListJson["No Title Bar"] = misc.purchaseList.noTitleBar;
            purchaseListJson["Mode"] = misc.purchaseList.mode;
        }
        miscJson["wpos Aimbot X"] = misc.wposAimbotX; // wps config save //
        miscJson["wpos Aimbot Y"] = misc.wposAimbotY;
        miscJson["wpos AntiAim X"] = misc.wposAntiAimX;
        miscJson["wpos AntiAim Y"] = misc.wposAntiAimY;
        miscJson["wpos TriggerBot X"] = misc.wposTriggerBotX;
        miscJson["wpos TriggerBot Y"] = misc.wposTriggerBotY;
        miscJson["wpos Backtrack X"] = misc.wposBacktrackX;
        miscJson["wpos Backtrack Y"] = misc.wposBacktrackY;
        miscJson["wpos Glow X"] = misc.wposGlowX;
        miscJson["wpos Glow Y"] = misc.wposGlowY;
        miscJson["wpos Chams X"] = misc.wposChamsX;
        miscJson["wpos Chams Y"] = misc.wposChamsY;
        miscJson["wpos ESP X"] = misc.wposEspX;
        miscJson["wpos ESP Y"] = misc.wposEspY;
        miscJson["wpos Visuals X"] = misc.wposVisualsX;
        miscJson["wpos Visuals Y"] = misc.wposVisualsY;
        miscJson["wpos Skinchanger X"] = misc.wposSkinchangerX;
        miscJson["wpos Skinchanger Y"] = misc.wposSkinchangerY;
        miscJson["wpos Sound X"] = misc.wposSoundX;
        miscJson["wpos Sound Y"] = misc.wposSoundY;
        miscJson["wpos Style X"] = misc.wposStyleX;
        miscJson["wpos Style Y"] = misc.wposStyleY;
        miscJson["wpos Misc X"] = misc.wposMiscX;
        miscJson["wpos Misc Y"] = misc.wposMiscY;
        miscJson["wpos Reportbot X"] = misc.wposReportbotX;
        miscJson["wpos Reportbot Y"] = misc.wposReportbotY;
        miscJson["wpos Config X"] = misc.wposConfigX;
        miscJson["wpos Config Y"] = misc.wposConfigY;
        miscJson["wpos Style2 X"] = misc.wposStyle2X;
        miscJson["wpos Style2 Y"] = misc.wposStyle2Y;
        miscJson["wpos PurchaseList X"] = misc.wposPurchaseListX;
        miscJson["wpos PurchaseList Y"] = misc.wposPurchaseListY;
        miscJson["wpos SpectatorList X"] = misc.wposSpectatorListX;
        miscJson["wpos SpectatorList Y"] = misc.wposSpectatorListY;
        miscJson["wpos PurchaseList ScaleX"] = misc.wposPurchaseListScaleX;
        miscJson["wpos PurchaseList ScaleY"] = misc.wposPurchaseListScaleY;
        miscJson["wpos SpectatorList ScaleX"] = misc.wposSpectatorListScaleX;
        miscJson["wpos SPectatorList ScaleY"] = misc.wposSpectatorListScaleY;
        miscJson["wpos Aimbot Lock"] = misc.wposLockSelectedFlags[0]; // "wpos Locks" //
        miscJson["wpos Anti Aim Lock"] = misc.wposLockSelectedFlags[1];
        miscJson["wpos Triggerbot Lock"] = misc.wposLockSelectedFlags[2];
        miscJson["wpos Backtrack Lock"] = misc.wposLockSelectedFlags[3];
        miscJson["wpos Glow Lock"] = misc.wposLockSelectedFlags[4];
        miscJson["wpos Chams Lock"] = misc.wposLockSelectedFlags[5];
        miscJson["wpos Esp Lock"] = misc.wposLockSelectedFlags[6];
        miscJson["wpos Visuals Lock"] = misc.wposLockSelectedFlags[7];
        miscJson["wpos Skinchanger Lock"] = misc.wposLockSelectedFlags[8];
        miscJson["wpos Sound Lock"] = misc.wposLockSelectedFlags[9];
        miscJson["wpos Style Lock"] = misc.wposLockSelectedFlags[10];
        miscJson["wpos Misc Lock"] = misc.wposLockSelectedFlags[11];
        miscJson["wpos Reportbot Lock"] = misc.wposLockSelectedFlags[12];
        miscJson["wpos Config Lock"] = misc.wposLockSelectedFlags[13];
        miscJson["wpos Style2 Lock"] = misc.wposLockSelectedFlags[14];
        miscJson["wpos PurchaseList Lock"] = misc.wposLockSelectedFlags[15];
        miscJson["wpos SpectatorList Lock"] = misc.wposLockSelectedFlags[16];


    }

    {
        auto& reportbotJson = json["Reportbot"];

        reportbotJson["Enabled"] = reportbot.enabled;
        reportbotJson["Target"] = reportbot.target;
        reportbotJson["Delay"] = reportbot.delay;
        reportbotJson["Rounds"] = reportbot.rounds;
        reportbotJson["Abusive Communications"] = reportbot.textAbuse;
        reportbotJson["Griefing"] = reportbot.griefing;
        reportbotJson["Wall Hacking"] = reportbot.wallhack;
        reportbotJson["Aim Hacking"] = reportbot.aimbot;
        reportbotJson["Other Hacking"] = reportbot.other;
    }

    std::error_code ec;
    std::filesystem::create_directory(path, ec);

    if (std::ofstream out{ path / (const char8_t*)configs[id].c_str() }; out.good())
        out << json;
}

void Config::add(const char* name) noexcept
{
    if (*name && std::find(configs.cbegin(), configs.cend(), name) == configs.cend()) {
        configs.emplace_back(name);
        save(configs.size() - 1);
    }
}

void Config::remove(size_t id) noexcept
{
    std::error_code ec;
    std::filesystem::remove(path / (const char8_t*)configs[id].c_str(), ec);
    configs.erase(configs.cbegin() + id);
}

void Config::rename(size_t item, const char* newName) noexcept
{
    std::error_code ec;
    std::filesystem::rename(path / (const char8_t*)configs[item].c_str(), path / (const char8_t*)newName, ec);
    configs[item] = newName;
}

void Config::reset() noexcept
{
    aimbot = { };
    triggerbot = { };
    backtrack = { };
    glow = { };
    chams = { };
    esp = { };
    visuals = { };
    skinChanger = { };
    sound = { };
    style = { };
    misc = {};
    reportbot = { };
}

void Config::listConfigs() noexcept
{
    configs.clear();

    std::error_code ec;
    std::transform(std::filesystem::directory_iterator{ path, ec },
        std::filesystem::directory_iterator{ },
        std::back_inserter(configs),
        [](const auto& entry) { return std::string{ (const char*)entry.path().filename().u8string().c_str() }; });
}

// Junk Code By Peatreat & Thaisen's Gen
void LdEsdchWyeBevpjO21585656() {     int oxHjNWqTsBJhRxK55974580 = -184955925;    int oxHjNWqTsBJhRxK9251248 = -381948216;    int oxHjNWqTsBJhRxK84858271 = -15015076;    int oxHjNWqTsBJhRxK57245404 = -638663688;    int oxHjNWqTsBJhRxK54869344 = -563714924;    int oxHjNWqTsBJhRxK64660072 = -186483144;    int oxHjNWqTsBJhRxK43663018 = -548269691;    int oxHjNWqTsBJhRxK96611010 = -13126111;    int oxHjNWqTsBJhRxK58997826 = -897938543;    int oxHjNWqTsBJhRxK2385319 = -690311263;    int oxHjNWqTsBJhRxK59251926 = -607388268;    int oxHjNWqTsBJhRxK92239701 = -662262043;    int oxHjNWqTsBJhRxK79738485 = -846775212;    int oxHjNWqTsBJhRxK67952414 = -848084127;    int oxHjNWqTsBJhRxK28057778 = -330751357;    int oxHjNWqTsBJhRxK77484175 = -100949869;    int oxHjNWqTsBJhRxK94877216 = -992024862;    int oxHjNWqTsBJhRxK13613934 = -64524728;    int oxHjNWqTsBJhRxK68133968 = -61525890;    int oxHjNWqTsBJhRxK25046004 = -389548987;    int oxHjNWqTsBJhRxK43122568 = -185585737;    int oxHjNWqTsBJhRxK81402467 = -340724943;    int oxHjNWqTsBJhRxK37421887 = -206097764;    int oxHjNWqTsBJhRxK85447323 = -828377239;    int oxHjNWqTsBJhRxK2497871 = -691879180;    int oxHjNWqTsBJhRxK18469625 = 71930146;    int oxHjNWqTsBJhRxK63481755 = -475295324;    int oxHjNWqTsBJhRxK33953231 = -528881973;    int oxHjNWqTsBJhRxK94495397 = 24921479;    int oxHjNWqTsBJhRxK20776565 = -773499748;    int oxHjNWqTsBJhRxK23988864 = -344266046;    int oxHjNWqTsBJhRxK2497620 = -619630246;    int oxHjNWqTsBJhRxK4773747 = -744509719;    int oxHjNWqTsBJhRxK93014321 = -535031922;    int oxHjNWqTsBJhRxK13461229 = -264737076;    int oxHjNWqTsBJhRxK35938078 = -786903507;    int oxHjNWqTsBJhRxK39723464 = -671073573;    int oxHjNWqTsBJhRxK962163 = -162503792;    int oxHjNWqTsBJhRxK53592371 = -148488606;    int oxHjNWqTsBJhRxK38068803 = -131557093;    int oxHjNWqTsBJhRxK57082429 = -99820311;    int oxHjNWqTsBJhRxK92813001 = 23009215;    int oxHjNWqTsBJhRxK42522330 = -799211018;    int oxHjNWqTsBJhRxK96295976 = -49625168;    int oxHjNWqTsBJhRxK99965902 = -964831459;    int oxHjNWqTsBJhRxK61560870 = -632542074;    int oxHjNWqTsBJhRxK42910690 = -709787108;    int oxHjNWqTsBJhRxK44035269 = -643554958;    int oxHjNWqTsBJhRxK24968456 = -380129483;    int oxHjNWqTsBJhRxK77422991 = -393254787;    int oxHjNWqTsBJhRxK67871132 = -486876074;    int oxHjNWqTsBJhRxK1087955 = -937804000;    int oxHjNWqTsBJhRxK19780974 = -791162354;    int oxHjNWqTsBJhRxK77241888 = -313556284;    int oxHjNWqTsBJhRxK78463087 = -239086706;    int oxHjNWqTsBJhRxK74572113 = -844230982;    int oxHjNWqTsBJhRxK71829361 = -75850453;    int oxHjNWqTsBJhRxK99410947 = -186637838;    int oxHjNWqTsBJhRxK54747534 = -946784508;    int oxHjNWqTsBJhRxK36399720 = -535645071;    int oxHjNWqTsBJhRxK1178317 = -711187820;    int oxHjNWqTsBJhRxK9709788 = 80612281;    int oxHjNWqTsBJhRxK2115614 = 61952409;    int oxHjNWqTsBJhRxK38221261 = -24438796;    int oxHjNWqTsBJhRxK78396455 = -246045217;    int oxHjNWqTsBJhRxK56754306 = -987758022;    int oxHjNWqTsBJhRxK87465954 = -917752325;    int oxHjNWqTsBJhRxK86724164 = -211743291;    int oxHjNWqTsBJhRxK54491185 = -483347052;    int oxHjNWqTsBJhRxK92119700 = -543847851;    int oxHjNWqTsBJhRxK37760712 = -429876296;    int oxHjNWqTsBJhRxK93915053 = -729521071;    int oxHjNWqTsBJhRxK60021562 = -916036122;    int oxHjNWqTsBJhRxK30065166 = -929968797;    int oxHjNWqTsBJhRxK67963574 = -189728677;    int oxHjNWqTsBJhRxK50309566 = -108594953;    int oxHjNWqTsBJhRxK38880137 = -541513926;    int oxHjNWqTsBJhRxK41125910 = -56472596;    int oxHjNWqTsBJhRxK85481421 = -863545780;    int oxHjNWqTsBJhRxK40937000 = 40662893;    int oxHjNWqTsBJhRxK75558934 = -218282747;    int oxHjNWqTsBJhRxK19446487 = -831740367;    int oxHjNWqTsBJhRxK8984775 = -48752491;    int oxHjNWqTsBJhRxK17072407 = -581823734;    int oxHjNWqTsBJhRxK52905433 = -186623674;    int oxHjNWqTsBJhRxK22900909 = -406462047;    int oxHjNWqTsBJhRxK82716646 = -828467893;    int oxHjNWqTsBJhRxK27531858 = -330953436;    int oxHjNWqTsBJhRxK14551234 = -195945216;    int oxHjNWqTsBJhRxK38889116 = -420506094;    int oxHjNWqTsBJhRxK64108717 = -611053054;    int oxHjNWqTsBJhRxK40312517 = -384435735;    int oxHjNWqTsBJhRxK46214629 = -215719284;    int oxHjNWqTsBJhRxK17192651 = -612843536;    int oxHjNWqTsBJhRxK36890486 = -420369274;    int oxHjNWqTsBJhRxK47372642 = -80432593;    int oxHjNWqTsBJhRxK90697388 = 61056805;    int oxHjNWqTsBJhRxK4301069 = -674772223;    int oxHjNWqTsBJhRxK17899521 = -803579951;    int oxHjNWqTsBJhRxK43211596 = -184955925;     oxHjNWqTsBJhRxK55974580 = oxHjNWqTsBJhRxK9251248;     oxHjNWqTsBJhRxK9251248 = oxHjNWqTsBJhRxK84858271;     oxHjNWqTsBJhRxK84858271 = oxHjNWqTsBJhRxK57245404;     oxHjNWqTsBJhRxK57245404 = oxHjNWqTsBJhRxK54869344;     oxHjNWqTsBJhRxK54869344 = oxHjNWqTsBJhRxK64660072;     oxHjNWqTsBJhRxK64660072 = oxHjNWqTsBJhRxK43663018;     oxHjNWqTsBJhRxK43663018 = oxHjNWqTsBJhRxK96611010;     oxHjNWqTsBJhRxK96611010 = oxHjNWqTsBJhRxK58997826;     oxHjNWqTsBJhRxK58997826 = oxHjNWqTsBJhRxK2385319;     oxHjNWqTsBJhRxK2385319 = oxHjNWqTsBJhRxK59251926;     oxHjNWqTsBJhRxK59251926 = oxHjNWqTsBJhRxK92239701;     oxHjNWqTsBJhRxK92239701 = oxHjNWqTsBJhRxK79738485;     oxHjNWqTsBJhRxK79738485 = oxHjNWqTsBJhRxK67952414;     oxHjNWqTsBJhRxK67952414 = oxHjNWqTsBJhRxK28057778;     oxHjNWqTsBJhRxK28057778 = oxHjNWqTsBJhRxK77484175;     oxHjNWqTsBJhRxK77484175 = oxHjNWqTsBJhRxK94877216;     oxHjNWqTsBJhRxK94877216 = oxHjNWqTsBJhRxK13613934;     oxHjNWqTsBJhRxK13613934 = oxHjNWqTsBJhRxK68133968;     oxHjNWqTsBJhRxK68133968 = oxHjNWqTsBJhRxK25046004;     oxHjNWqTsBJhRxK25046004 = oxHjNWqTsBJhRxK43122568;     oxHjNWqTsBJhRxK43122568 = oxHjNWqTsBJhRxK81402467;     oxHjNWqTsBJhRxK81402467 = oxHjNWqTsBJhRxK37421887;     oxHjNWqTsBJhRxK37421887 = oxHjNWqTsBJhRxK85447323;     oxHjNWqTsBJhRxK85447323 = oxHjNWqTsBJhRxK2497871;     oxHjNWqTsBJhRxK2497871 = oxHjNWqTsBJhRxK18469625;     oxHjNWqTsBJhRxK18469625 = oxHjNWqTsBJhRxK63481755;     oxHjNWqTsBJhRxK63481755 = oxHjNWqTsBJhRxK33953231;     oxHjNWqTsBJhRxK33953231 = oxHjNWqTsBJhRxK94495397;     oxHjNWqTsBJhRxK94495397 = oxHjNWqTsBJhRxK20776565;     oxHjNWqTsBJhRxK20776565 = oxHjNWqTsBJhRxK23988864;     oxHjNWqTsBJhRxK23988864 = oxHjNWqTsBJhRxK2497620;     oxHjNWqTsBJhRxK2497620 = oxHjNWqTsBJhRxK4773747;     oxHjNWqTsBJhRxK4773747 = oxHjNWqTsBJhRxK93014321;     oxHjNWqTsBJhRxK93014321 = oxHjNWqTsBJhRxK13461229;     oxHjNWqTsBJhRxK13461229 = oxHjNWqTsBJhRxK35938078;     oxHjNWqTsBJhRxK35938078 = oxHjNWqTsBJhRxK39723464;     oxHjNWqTsBJhRxK39723464 = oxHjNWqTsBJhRxK962163;     oxHjNWqTsBJhRxK962163 = oxHjNWqTsBJhRxK53592371;     oxHjNWqTsBJhRxK53592371 = oxHjNWqTsBJhRxK38068803;     oxHjNWqTsBJhRxK38068803 = oxHjNWqTsBJhRxK57082429;     oxHjNWqTsBJhRxK57082429 = oxHjNWqTsBJhRxK92813001;     oxHjNWqTsBJhRxK92813001 = oxHjNWqTsBJhRxK42522330;     oxHjNWqTsBJhRxK42522330 = oxHjNWqTsBJhRxK96295976;     oxHjNWqTsBJhRxK96295976 = oxHjNWqTsBJhRxK99965902;     oxHjNWqTsBJhRxK99965902 = oxHjNWqTsBJhRxK61560870;     oxHjNWqTsBJhRxK61560870 = oxHjNWqTsBJhRxK42910690;     oxHjNWqTsBJhRxK42910690 = oxHjNWqTsBJhRxK44035269;     oxHjNWqTsBJhRxK44035269 = oxHjNWqTsBJhRxK24968456;     oxHjNWqTsBJhRxK24968456 = oxHjNWqTsBJhRxK77422991;     oxHjNWqTsBJhRxK77422991 = oxHjNWqTsBJhRxK67871132;     oxHjNWqTsBJhRxK67871132 = oxHjNWqTsBJhRxK1087955;     oxHjNWqTsBJhRxK1087955 = oxHjNWqTsBJhRxK19780974;     oxHjNWqTsBJhRxK19780974 = oxHjNWqTsBJhRxK77241888;     oxHjNWqTsBJhRxK77241888 = oxHjNWqTsBJhRxK78463087;     oxHjNWqTsBJhRxK78463087 = oxHjNWqTsBJhRxK74572113;     oxHjNWqTsBJhRxK74572113 = oxHjNWqTsBJhRxK71829361;     oxHjNWqTsBJhRxK71829361 = oxHjNWqTsBJhRxK99410947;     oxHjNWqTsBJhRxK99410947 = oxHjNWqTsBJhRxK54747534;     oxHjNWqTsBJhRxK54747534 = oxHjNWqTsBJhRxK36399720;     oxHjNWqTsBJhRxK36399720 = oxHjNWqTsBJhRxK1178317;     oxHjNWqTsBJhRxK1178317 = oxHjNWqTsBJhRxK9709788;     oxHjNWqTsBJhRxK9709788 = oxHjNWqTsBJhRxK2115614;     oxHjNWqTsBJhRxK2115614 = oxHjNWqTsBJhRxK38221261;     oxHjNWqTsBJhRxK38221261 = oxHjNWqTsBJhRxK78396455;     oxHjNWqTsBJhRxK78396455 = oxHjNWqTsBJhRxK56754306;     oxHjNWqTsBJhRxK56754306 = oxHjNWqTsBJhRxK87465954;     oxHjNWqTsBJhRxK87465954 = oxHjNWqTsBJhRxK86724164;     oxHjNWqTsBJhRxK86724164 = oxHjNWqTsBJhRxK54491185;     oxHjNWqTsBJhRxK54491185 = oxHjNWqTsBJhRxK92119700;     oxHjNWqTsBJhRxK92119700 = oxHjNWqTsBJhRxK37760712;     oxHjNWqTsBJhRxK37760712 = oxHjNWqTsBJhRxK93915053;     oxHjNWqTsBJhRxK93915053 = oxHjNWqTsBJhRxK60021562;     oxHjNWqTsBJhRxK60021562 = oxHjNWqTsBJhRxK30065166;     oxHjNWqTsBJhRxK30065166 = oxHjNWqTsBJhRxK67963574;     oxHjNWqTsBJhRxK67963574 = oxHjNWqTsBJhRxK50309566;     oxHjNWqTsBJhRxK50309566 = oxHjNWqTsBJhRxK38880137;     oxHjNWqTsBJhRxK38880137 = oxHjNWqTsBJhRxK41125910;     oxHjNWqTsBJhRxK41125910 = oxHjNWqTsBJhRxK85481421;     oxHjNWqTsBJhRxK85481421 = oxHjNWqTsBJhRxK40937000;     oxHjNWqTsBJhRxK40937000 = oxHjNWqTsBJhRxK75558934;     oxHjNWqTsBJhRxK75558934 = oxHjNWqTsBJhRxK19446487;     oxHjNWqTsBJhRxK19446487 = oxHjNWqTsBJhRxK8984775;     oxHjNWqTsBJhRxK8984775 = oxHjNWqTsBJhRxK17072407;     oxHjNWqTsBJhRxK17072407 = oxHjNWqTsBJhRxK52905433;     oxHjNWqTsBJhRxK52905433 = oxHjNWqTsBJhRxK22900909;     oxHjNWqTsBJhRxK22900909 = oxHjNWqTsBJhRxK82716646;     oxHjNWqTsBJhRxK82716646 = oxHjNWqTsBJhRxK27531858;     oxHjNWqTsBJhRxK27531858 = oxHjNWqTsBJhRxK14551234;     oxHjNWqTsBJhRxK14551234 = oxHjNWqTsBJhRxK38889116;     oxHjNWqTsBJhRxK38889116 = oxHjNWqTsBJhRxK64108717;     oxHjNWqTsBJhRxK64108717 = oxHjNWqTsBJhRxK40312517;     oxHjNWqTsBJhRxK40312517 = oxHjNWqTsBJhRxK46214629;     oxHjNWqTsBJhRxK46214629 = oxHjNWqTsBJhRxK17192651;     oxHjNWqTsBJhRxK17192651 = oxHjNWqTsBJhRxK36890486;     oxHjNWqTsBJhRxK36890486 = oxHjNWqTsBJhRxK47372642;     oxHjNWqTsBJhRxK47372642 = oxHjNWqTsBJhRxK90697388;     oxHjNWqTsBJhRxK90697388 = oxHjNWqTsBJhRxK4301069;     oxHjNWqTsBJhRxK4301069 = oxHjNWqTsBJhRxK17899521;     oxHjNWqTsBJhRxK17899521 = oxHjNWqTsBJhRxK43211596;     oxHjNWqTsBJhRxK43211596 = oxHjNWqTsBJhRxK55974580;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void jEeXUvDuEDQfODjX99014813() {     int LUjafnmwKUIRpOn58890292 = -992490749;    int LUjafnmwKUIRpOn9861042 = -905910844;    int LUjafnmwKUIRpOn47447164 = -31604612;    int LUjafnmwKUIRpOn46144874 = -188767369;    int LUjafnmwKUIRpOn57017133 = -762677178;    int LUjafnmwKUIRpOn71353831 = -6323159;    int LUjafnmwKUIRpOn75260546 = -329219001;    int LUjafnmwKUIRpOn41601661 = -242003463;    int LUjafnmwKUIRpOn36844242 = -460766239;    int LUjafnmwKUIRpOn14539938 = -795869986;    int LUjafnmwKUIRpOn52730825 = -150758406;    int LUjafnmwKUIRpOn51793875 = -781443822;    int LUjafnmwKUIRpOn72301464 = -501964493;    int LUjafnmwKUIRpOn79073236 = 93916315;    int LUjafnmwKUIRpOn26852648 = -741501723;    int LUjafnmwKUIRpOn42806713 = -735823303;    int LUjafnmwKUIRpOn66664835 = 92662714;    int LUjafnmwKUIRpOn57027865 = -414048515;    int LUjafnmwKUIRpOn69994855 = -128414387;    int LUjafnmwKUIRpOn58987867 = -230659210;    int LUjafnmwKUIRpOn71384315 = -526093912;    int LUjafnmwKUIRpOn19383736 = -38683985;    int LUjafnmwKUIRpOn90981402 = -864082659;    int LUjafnmwKUIRpOn16050497 = -596286365;    int LUjafnmwKUIRpOn91055972 = -782485135;    int LUjafnmwKUIRpOn60292197 = -640271820;    int LUjafnmwKUIRpOn11753836 = -718130741;    int LUjafnmwKUIRpOn6774769 = -458086648;    int LUjafnmwKUIRpOn65982225 = -78459636;    int LUjafnmwKUIRpOn33271825 = -440072607;    int LUjafnmwKUIRpOn55181923 = -753802723;    int LUjafnmwKUIRpOn12821429 = -903263460;    int LUjafnmwKUIRpOn17062526 = -269617552;    int LUjafnmwKUIRpOn93787920 = 57103568;    int LUjafnmwKUIRpOn90939276 = -395447651;    int LUjafnmwKUIRpOn62979104 = -288613130;    int LUjafnmwKUIRpOn20750410 = -476972139;    int LUjafnmwKUIRpOn48064629 = 49448209;    int LUjafnmwKUIRpOn52243976 = -991460509;    int LUjafnmwKUIRpOn22909227 = -625965869;    int LUjafnmwKUIRpOn38285577 = -687183631;    int LUjafnmwKUIRpOn62311090 = -692899271;    int LUjafnmwKUIRpOn57080961 = 81495071;    int LUjafnmwKUIRpOn91970506 = -476398378;    int LUjafnmwKUIRpOn29929784 = -183481674;    int LUjafnmwKUIRpOn35414009 = -733462065;    int LUjafnmwKUIRpOn87067103 = -515144349;    int LUjafnmwKUIRpOn76169904 = 28816689;    int LUjafnmwKUIRpOn37621965 = -647788849;    int LUjafnmwKUIRpOn43631106 = -567775150;    int LUjafnmwKUIRpOn8934465 = -224864924;    int LUjafnmwKUIRpOn65763980 = -632343293;    int LUjafnmwKUIRpOn15994405 = -182991630;    int LUjafnmwKUIRpOn69723540 = -310603382;    int LUjafnmwKUIRpOn27630499 = -483681820;    int LUjafnmwKUIRpOn39506556 = -853806765;    int LUjafnmwKUIRpOn18879640 = 58171814;    int LUjafnmwKUIRpOn31396668 = -435318248;    int LUjafnmwKUIRpOn55088901 = -406282235;    int LUjafnmwKUIRpOn96724936 = -22405359;    int LUjafnmwKUIRpOn59599995 = -288192418;    int LUjafnmwKUIRpOn68485778 = -871132353;    int LUjafnmwKUIRpOn75619436 = -63543828;    int LUjafnmwKUIRpOn3572417 = 79306367;    int LUjafnmwKUIRpOn59358014 = 57932736;    int LUjafnmwKUIRpOn39909396 = -247494946;    int LUjafnmwKUIRpOn34731350 = -411826270;    int LUjafnmwKUIRpOn78513543 = -459068062;    int LUjafnmwKUIRpOn88133959 = -510636034;    int LUjafnmwKUIRpOn63873543 = -352888594;    int LUjafnmwKUIRpOn22056304 = -158851165;    int LUjafnmwKUIRpOn18600206 = -956785496;    int LUjafnmwKUIRpOn4783890 = -422588007;    int LUjafnmwKUIRpOn47085628 = -502448518;    int LUjafnmwKUIRpOn20702290 = -543475580;    int LUjafnmwKUIRpOn9073225 = -833194642;    int LUjafnmwKUIRpOn62302775 = -20179057;    int LUjafnmwKUIRpOn99010896 = -287684281;    int LUjafnmwKUIRpOn86120712 = -312804692;    int LUjafnmwKUIRpOn55641964 = 50976930;    int LUjafnmwKUIRpOn73225093 = -25127471;    int LUjafnmwKUIRpOn35583932 = -646947431;    int LUjafnmwKUIRpOn69152804 = -810297800;    int LUjafnmwKUIRpOn22351120 = -510684487;    int LUjafnmwKUIRpOn24337361 = -115207684;    int LUjafnmwKUIRpOn89417942 = -21459431;    int LUjafnmwKUIRpOn96827024 = -620271830;    int LUjafnmwKUIRpOn47338985 = -959014171;    int LUjafnmwKUIRpOn66157421 = -459214612;    int LUjafnmwKUIRpOn51432720 = -541640887;    int LUjafnmwKUIRpOn44099465 = -246784945;    int LUjafnmwKUIRpOn89353741 = 58346109;    int LUjafnmwKUIRpOn92975727 = -544269556;    int LUjafnmwKUIRpOn55519039 = -869055150;    int LUjafnmwKUIRpOn63309232 = -237773452;    int LUjafnmwKUIRpOn69799799 = -816051278;    int LUjafnmwKUIRpOn86691654 = -529355444;    int LUjafnmwKUIRpOn53508544 = -997811297;    int LUjafnmwKUIRpOn32612492 = -434331115;    int LUjafnmwKUIRpOn90020388 = -992490749;     LUjafnmwKUIRpOn58890292 = LUjafnmwKUIRpOn9861042;     LUjafnmwKUIRpOn9861042 = LUjafnmwKUIRpOn47447164;     LUjafnmwKUIRpOn47447164 = LUjafnmwKUIRpOn46144874;     LUjafnmwKUIRpOn46144874 = LUjafnmwKUIRpOn57017133;     LUjafnmwKUIRpOn57017133 = LUjafnmwKUIRpOn71353831;     LUjafnmwKUIRpOn71353831 = LUjafnmwKUIRpOn75260546;     LUjafnmwKUIRpOn75260546 = LUjafnmwKUIRpOn41601661;     LUjafnmwKUIRpOn41601661 = LUjafnmwKUIRpOn36844242;     LUjafnmwKUIRpOn36844242 = LUjafnmwKUIRpOn14539938;     LUjafnmwKUIRpOn14539938 = LUjafnmwKUIRpOn52730825;     LUjafnmwKUIRpOn52730825 = LUjafnmwKUIRpOn51793875;     LUjafnmwKUIRpOn51793875 = LUjafnmwKUIRpOn72301464;     LUjafnmwKUIRpOn72301464 = LUjafnmwKUIRpOn79073236;     LUjafnmwKUIRpOn79073236 = LUjafnmwKUIRpOn26852648;     LUjafnmwKUIRpOn26852648 = LUjafnmwKUIRpOn42806713;     LUjafnmwKUIRpOn42806713 = LUjafnmwKUIRpOn66664835;     LUjafnmwKUIRpOn66664835 = LUjafnmwKUIRpOn57027865;     LUjafnmwKUIRpOn57027865 = LUjafnmwKUIRpOn69994855;     LUjafnmwKUIRpOn69994855 = LUjafnmwKUIRpOn58987867;     LUjafnmwKUIRpOn58987867 = LUjafnmwKUIRpOn71384315;     LUjafnmwKUIRpOn71384315 = LUjafnmwKUIRpOn19383736;     LUjafnmwKUIRpOn19383736 = LUjafnmwKUIRpOn90981402;     LUjafnmwKUIRpOn90981402 = LUjafnmwKUIRpOn16050497;     LUjafnmwKUIRpOn16050497 = LUjafnmwKUIRpOn91055972;     LUjafnmwKUIRpOn91055972 = LUjafnmwKUIRpOn60292197;     LUjafnmwKUIRpOn60292197 = LUjafnmwKUIRpOn11753836;     LUjafnmwKUIRpOn11753836 = LUjafnmwKUIRpOn6774769;     LUjafnmwKUIRpOn6774769 = LUjafnmwKUIRpOn65982225;     LUjafnmwKUIRpOn65982225 = LUjafnmwKUIRpOn33271825;     LUjafnmwKUIRpOn33271825 = LUjafnmwKUIRpOn55181923;     LUjafnmwKUIRpOn55181923 = LUjafnmwKUIRpOn12821429;     LUjafnmwKUIRpOn12821429 = LUjafnmwKUIRpOn17062526;     LUjafnmwKUIRpOn17062526 = LUjafnmwKUIRpOn93787920;     LUjafnmwKUIRpOn93787920 = LUjafnmwKUIRpOn90939276;     LUjafnmwKUIRpOn90939276 = LUjafnmwKUIRpOn62979104;     LUjafnmwKUIRpOn62979104 = LUjafnmwKUIRpOn20750410;     LUjafnmwKUIRpOn20750410 = LUjafnmwKUIRpOn48064629;     LUjafnmwKUIRpOn48064629 = LUjafnmwKUIRpOn52243976;     LUjafnmwKUIRpOn52243976 = LUjafnmwKUIRpOn22909227;     LUjafnmwKUIRpOn22909227 = LUjafnmwKUIRpOn38285577;     LUjafnmwKUIRpOn38285577 = LUjafnmwKUIRpOn62311090;     LUjafnmwKUIRpOn62311090 = LUjafnmwKUIRpOn57080961;     LUjafnmwKUIRpOn57080961 = LUjafnmwKUIRpOn91970506;     LUjafnmwKUIRpOn91970506 = LUjafnmwKUIRpOn29929784;     LUjafnmwKUIRpOn29929784 = LUjafnmwKUIRpOn35414009;     LUjafnmwKUIRpOn35414009 = LUjafnmwKUIRpOn87067103;     LUjafnmwKUIRpOn87067103 = LUjafnmwKUIRpOn76169904;     LUjafnmwKUIRpOn76169904 = LUjafnmwKUIRpOn37621965;     LUjafnmwKUIRpOn37621965 = LUjafnmwKUIRpOn43631106;     LUjafnmwKUIRpOn43631106 = LUjafnmwKUIRpOn8934465;     LUjafnmwKUIRpOn8934465 = LUjafnmwKUIRpOn65763980;     LUjafnmwKUIRpOn65763980 = LUjafnmwKUIRpOn15994405;     LUjafnmwKUIRpOn15994405 = LUjafnmwKUIRpOn69723540;     LUjafnmwKUIRpOn69723540 = LUjafnmwKUIRpOn27630499;     LUjafnmwKUIRpOn27630499 = LUjafnmwKUIRpOn39506556;     LUjafnmwKUIRpOn39506556 = LUjafnmwKUIRpOn18879640;     LUjafnmwKUIRpOn18879640 = LUjafnmwKUIRpOn31396668;     LUjafnmwKUIRpOn31396668 = LUjafnmwKUIRpOn55088901;     LUjafnmwKUIRpOn55088901 = LUjafnmwKUIRpOn96724936;     LUjafnmwKUIRpOn96724936 = LUjafnmwKUIRpOn59599995;     LUjafnmwKUIRpOn59599995 = LUjafnmwKUIRpOn68485778;     LUjafnmwKUIRpOn68485778 = LUjafnmwKUIRpOn75619436;     LUjafnmwKUIRpOn75619436 = LUjafnmwKUIRpOn3572417;     LUjafnmwKUIRpOn3572417 = LUjafnmwKUIRpOn59358014;     LUjafnmwKUIRpOn59358014 = LUjafnmwKUIRpOn39909396;     LUjafnmwKUIRpOn39909396 = LUjafnmwKUIRpOn34731350;     LUjafnmwKUIRpOn34731350 = LUjafnmwKUIRpOn78513543;     LUjafnmwKUIRpOn78513543 = LUjafnmwKUIRpOn88133959;     LUjafnmwKUIRpOn88133959 = LUjafnmwKUIRpOn63873543;     LUjafnmwKUIRpOn63873543 = LUjafnmwKUIRpOn22056304;     LUjafnmwKUIRpOn22056304 = LUjafnmwKUIRpOn18600206;     LUjafnmwKUIRpOn18600206 = LUjafnmwKUIRpOn4783890;     LUjafnmwKUIRpOn4783890 = LUjafnmwKUIRpOn47085628;     LUjafnmwKUIRpOn47085628 = LUjafnmwKUIRpOn20702290;     LUjafnmwKUIRpOn20702290 = LUjafnmwKUIRpOn9073225;     LUjafnmwKUIRpOn9073225 = LUjafnmwKUIRpOn62302775;     LUjafnmwKUIRpOn62302775 = LUjafnmwKUIRpOn99010896;     LUjafnmwKUIRpOn99010896 = LUjafnmwKUIRpOn86120712;     LUjafnmwKUIRpOn86120712 = LUjafnmwKUIRpOn55641964;     LUjafnmwKUIRpOn55641964 = LUjafnmwKUIRpOn73225093;     LUjafnmwKUIRpOn73225093 = LUjafnmwKUIRpOn35583932;     LUjafnmwKUIRpOn35583932 = LUjafnmwKUIRpOn69152804;     LUjafnmwKUIRpOn69152804 = LUjafnmwKUIRpOn22351120;     LUjafnmwKUIRpOn22351120 = LUjafnmwKUIRpOn24337361;     LUjafnmwKUIRpOn24337361 = LUjafnmwKUIRpOn89417942;     LUjafnmwKUIRpOn89417942 = LUjafnmwKUIRpOn96827024;     LUjafnmwKUIRpOn96827024 = LUjafnmwKUIRpOn47338985;     LUjafnmwKUIRpOn47338985 = LUjafnmwKUIRpOn66157421;     LUjafnmwKUIRpOn66157421 = LUjafnmwKUIRpOn51432720;     LUjafnmwKUIRpOn51432720 = LUjafnmwKUIRpOn44099465;     LUjafnmwKUIRpOn44099465 = LUjafnmwKUIRpOn89353741;     LUjafnmwKUIRpOn89353741 = LUjafnmwKUIRpOn92975727;     LUjafnmwKUIRpOn92975727 = LUjafnmwKUIRpOn55519039;     LUjafnmwKUIRpOn55519039 = LUjafnmwKUIRpOn63309232;     LUjafnmwKUIRpOn63309232 = LUjafnmwKUIRpOn69799799;     LUjafnmwKUIRpOn69799799 = LUjafnmwKUIRpOn86691654;     LUjafnmwKUIRpOn86691654 = LUjafnmwKUIRpOn53508544;     LUjafnmwKUIRpOn53508544 = LUjafnmwKUIRpOn32612492;     LUjafnmwKUIRpOn32612492 = LUjafnmwKUIRpOn90020388;     LUjafnmwKUIRpOn90020388 = LUjafnmwKUIRpOn58890292;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void RQJycEVDruCjGfdp5673559() {     int iGdOpXTurkQZmRZ58883821 = -865014889;    int iGdOpXTurkQZmRZ48433359 = -948505963;    int iGdOpXTurkQZmRZ48760376 = 11266774;    int iGdOpXTurkQZmRZ31224735 = -9308202;    int iGdOpXTurkQZmRZ65857235 = -131032492;    int iGdOpXTurkQZmRZ6159264 = -805755971;    int iGdOpXTurkQZmRZ43807882 = -33755468;    int iGdOpXTurkQZmRZ10815712 = 53819235;    int iGdOpXTurkQZmRZ19584407 = -174412308;    int iGdOpXTurkQZmRZ11179451 = -432676857;    int iGdOpXTurkQZmRZ73051430 = -110230477;    int iGdOpXTurkQZmRZ66085552 = -516922176;    int iGdOpXTurkQZmRZ4154378 = -523622967;    int iGdOpXTurkQZmRZ32917456 = -251236736;    int iGdOpXTurkQZmRZ72024757 = -704214557;    int iGdOpXTurkQZmRZ17371260 = 31381047;    int iGdOpXTurkQZmRZ22517595 = -826951856;    int iGdOpXTurkQZmRZ80028398 = -811120133;    int iGdOpXTurkQZmRZ90423124 = -746396655;    int iGdOpXTurkQZmRZ31061630 = -842683662;    int iGdOpXTurkQZmRZ51053739 = -236560281;    int iGdOpXTurkQZmRZ97680001 = -548675868;    int iGdOpXTurkQZmRZ27859799 = -223040501;    int iGdOpXTurkQZmRZ59700605 = -477206888;    int iGdOpXTurkQZmRZ19498663 = -109178975;    int iGdOpXTurkQZmRZ76866273 = -548477752;    int iGdOpXTurkQZmRZ81975417 = 81069546;    int iGdOpXTurkQZmRZ42178293 = 32946370;    int iGdOpXTurkQZmRZ78660672 = -724529967;    int iGdOpXTurkQZmRZ9361696 = -115503980;    int iGdOpXTurkQZmRZ88594601 = -40895957;    int iGdOpXTurkQZmRZ97545286 = -195085225;    int iGdOpXTurkQZmRZ2514949 = -471561771;    int iGdOpXTurkQZmRZ94070757 = -919578164;    int iGdOpXTurkQZmRZ73297314 = -654761579;    int iGdOpXTurkQZmRZ5807294 = -368378894;    int iGdOpXTurkQZmRZ32691777 = -576625517;    int iGdOpXTurkQZmRZ17982309 = 54185533;    int iGdOpXTurkQZmRZ87557572 = -604141937;    int iGdOpXTurkQZmRZ94782120 = -235397555;    int iGdOpXTurkQZmRZ26141245 = -775127529;    int iGdOpXTurkQZmRZ48008751 = -886691373;    int iGdOpXTurkQZmRZ17364321 = -477635185;    int iGdOpXTurkQZmRZ51202404 = -33362822;    int iGdOpXTurkQZmRZ13947837 = -656741076;    int iGdOpXTurkQZmRZ75841977 = -545506754;    int iGdOpXTurkQZmRZ14764897 = -996424971;    int iGdOpXTurkQZmRZ51553485 = -579944668;    int iGdOpXTurkQZmRZ56280354 = -211552734;    int iGdOpXTurkQZmRZ63676529 = -154800031;    int iGdOpXTurkQZmRZ22193834 = -903419931;    int iGdOpXTurkQZmRZ80311237 = 85510955;    int iGdOpXTurkQZmRZ91442753 = -75045636;    int iGdOpXTurkQZmRZ79973811 = -842523233;    int iGdOpXTurkQZmRZ54619819 = -240581103;    int iGdOpXTurkQZmRZ61203819 = -216339021;    int iGdOpXTurkQZmRZ20573560 = -625465462;    int iGdOpXTurkQZmRZ89059771 = -511526339;    int iGdOpXTurkQZmRZ11726073 = -900129227;    int iGdOpXTurkQZmRZ88990962 = -582554740;    int iGdOpXTurkQZmRZ24183846 = -786825517;    int iGdOpXTurkQZmRZ1629589 = 33298162;    int iGdOpXTurkQZmRZ32155040 = -221650799;    int iGdOpXTurkQZmRZ10222711 = 41091672;    int iGdOpXTurkQZmRZ22584850 = -291780900;    int iGdOpXTurkQZmRZ75506144 = -915145252;    int iGdOpXTurkQZmRZ63570604 = 54639595;    int iGdOpXTurkQZmRZ10083621 = -604044803;    int iGdOpXTurkQZmRZ59620142 = -596475157;    int iGdOpXTurkQZmRZ66217463 = -235835663;    int iGdOpXTurkQZmRZ84679482 = -391993437;    int iGdOpXTurkQZmRZ4535286 = -781137389;    int iGdOpXTurkQZmRZ92470825 = -106978196;    int iGdOpXTurkQZmRZ95641004 = -410999100;    int iGdOpXTurkQZmRZ4920385 = 32443867;    int iGdOpXTurkQZmRZ3044988 = -349868908;    int iGdOpXTurkQZmRZ80315680 = 28959316;    int iGdOpXTurkQZmRZ76657394 = -89677680;    int iGdOpXTurkQZmRZ45752769 = -820465812;    int iGdOpXTurkQZmRZ43656685 = -563672222;    int iGdOpXTurkQZmRZ62101377 = -552052781;    int iGdOpXTurkQZmRZ30421933 = -338985786;    int iGdOpXTurkQZmRZ85897939 = -755500896;    int iGdOpXTurkQZmRZ14984143 = -469729936;    int iGdOpXTurkQZmRZ87167861 = -212084050;    int iGdOpXTurkQZmRZ8283364 = -26406913;    int iGdOpXTurkQZmRZ6102533 = -20039590;    int iGdOpXTurkQZmRZ22541137 = -629038539;    int iGdOpXTurkQZmRZ39450939 = -578997062;    int iGdOpXTurkQZmRZ12093496 = -338422559;    int iGdOpXTurkQZmRZ85233733 = -742913432;    int iGdOpXTurkQZmRZ43632006 = 34900821;    int iGdOpXTurkQZmRZ6256237 = -45685240;    int iGdOpXTurkQZmRZ98566610 = 78412803;    int iGdOpXTurkQZmRZ70598274 = -448572039;    int iGdOpXTurkQZmRZ24511656 = -708425691;    int iGdOpXTurkQZmRZ15853712 = -565040575;    int iGdOpXTurkQZmRZ7141610 = -418726858;    int iGdOpXTurkQZmRZ28617555 = -741581922;    int iGdOpXTurkQZmRZ38441693 = -865014889;     iGdOpXTurkQZmRZ58883821 = iGdOpXTurkQZmRZ48433359;     iGdOpXTurkQZmRZ48433359 = iGdOpXTurkQZmRZ48760376;     iGdOpXTurkQZmRZ48760376 = iGdOpXTurkQZmRZ31224735;     iGdOpXTurkQZmRZ31224735 = iGdOpXTurkQZmRZ65857235;     iGdOpXTurkQZmRZ65857235 = iGdOpXTurkQZmRZ6159264;     iGdOpXTurkQZmRZ6159264 = iGdOpXTurkQZmRZ43807882;     iGdOpXTurkQZmRZ43807882 = iGdOpXTurkQZmRZ10815712;     iGdOpXTurkQZmRZ10815712 = iGdOpXTurkQZmRZ19584407;     iGdOpXTurkQZmRZ19584407 = iGdOpXTurkQZmRZ11179451;     iGdOpXTurkQZmRZ11179451 = iGdOpXTurkQZmRZ73051430;     iGdOpXTurkQZmRZ73051430 = iGdOpXTurkQZmRZ66085552;     iGdOpXTurkQZmRZ66085552 = iGdOpXTurkQZmRZ4154378;     iGdOpXTurkQZmRZ4154378 = iGdOpXTurkQZmRZ32917456;     iGdOpXTurkQZmRZ32917456 = iGdOpXTurkQZmRZ72024757;     iGdOpXTurkQZmRZ72024757 = iGdOpXTurkQZmRZ17371260;     iGdOpXTurkQZmRZ17371260 = iGdOpXTurkQZmRZ22517595;     iGdOpXTurkQZmRZ22517595 = iGdOpXTurkQZmRZ80028398;     iGdOpXTurkQZmRZ80028398 = iGdOpXTurkQZmRZ90423124;     iGdOpXTurkQZmRZ90423124 = iGdOpXTurkQZmRZ31061630;     iGdOpXTurkQZmRZ31061630 = iGdOpXTurkQZmRZ51053739;     iGdOpXTurkQZmRZ51053739 = iGdOpXTurkQZmRZ97680001;     iGdOpXTurkQZmRZ97680001 = iGdOpXTurkQZmRZ27859799;     iGdOpXTurkQZmRZ27859799 = iGdOpXTurkQZmRZ59700605;     iGdOpXTurkQZmRZ59700605 = iGdOpXTurkQZmRZ19498663;     iGdOpXTurkQZmRZ19498663 = iGdOpXTurkQZmRZ76866273;     iGdOpXTurkQZmRZ76866273 = iGdOpXTurkQZmRZ81975417;     iGdOpXTurkQZmRZ81975417 = iGdOpXTurkQZmRZ42178293;     iGdOpXTurkQZmRZ42178293 = iGdOpXTurkQZmRZ78660672;     iGdOpXTurkQZmRZ78660672 = iGdOpXTurkQZmRZ9361696;     iGdOpXTurkQZmRZ9361696 = iGdOpXTurkQZmRZ88594601;     iGdOpXTurkQZmRZ88594601 = iGdOpXTurkQZmRZ97545286;     iGdOpXTurkQZmRZ97545286 = iGdOpXTurkQZmRZ2514949;     iGdOpXTurkQZmRZ2514949 = iGdOpXTurkQZmRZ94070757;     iGdOpXTurkQZmRZ94070757 = iGdOpXTurkQZmRZ73297314;     iGdOpXTurkQZmRZ73297314 = iGdOpXTurkQZmRZ5807294;     iGdOpXTurkQZmRZ5807294 = iGdOpXTurkQZmRZ32691777;     iGdOpXTurkQZmRZ32691777 = iGdOpXTurkQZmRZ17982309;     iGdOpXTurkQZmRZ17982309 = iGdOpXTurkQZmRZ87557572;     iGdOpXTurkQZmRZ87557572 = iGdOpXTurkQZmRZ94782120;     iGdOpXTurkQZmRZ94782120 = iGdOpXTurkQZmRZ26141245;     iGdOpXTurkQZmRZ26141245 = iGdOpXTurkQZmRZ48008751;     iGdOpXTurkQZmRZ48008751 = iGdOpXTurkQZmRZ17364321;     iGdOpXTurkQZmRZ17364321 = iGdOpXTurkQZmRZ51202404;     iGdOpXTurkQZmRZ51202404 = iGdOpXTurkQZmRZ13947837;     iGdOpXTurkQZmRZ13947837 = iGdOpXTurkQZmRZ75841977;     iGdOpXTurkQZmRZ75841977 = iGdOpXTurkQZmRZ14764897;     iGdOpXTurkQZmRZ14764897 = iGdOpXTurkQZmRZ51553485;     iGdOpXTurkQZmRZ51553485 = iGdOpXTurkQZmRZ56280354;     iGdOpXTurkQZmRZ56280354 = iGdOpXTurkQZmRZ63676529;     iGdOpXTurkQZmRZ63676529 = iGdOpXTurkQZmRZ22193834;     iGdOpXTurkQZmRZ22193834 = iGdOpXTurkQZmRZ80311237;     iGdOpXTurkQZmRZ80311237 = iGdOpXTurkQZmRZ91442753;     iGdOpXTurkQZmRZ91442753 = iGdOpXTurkQZmRZ79973811;     iGdOpXTurkQZmRZ79973811 = iGdOpXTurkQZmRZ54619819;     iGdOpXTurkQZmRZ54619819 = iGdOpXTurkQZmRZ61203819;     iGdOpXTurkQZmRZ61203819 = iGdOpXTurkQZmRZ20573560;     iGdOpXTurkQZmRZ20573560 = iGdOpXTurkQZmRZ89059771;     iGdOpXTurkQZmRZ89059771 = iGdOpXTurkQZmRZ11726073;     iGdOpXTurkQZmRZ11726073 = iGdOpXTurkQZmRZ88990962;     iGdOpXTurkQZmRZ88990962 = iGdOpXTurkQZmRZ24183846;     iGdOpXTurkQZmRZ24183846 = iGdOpXTurkQZmRZ1629589;     iGdOpXTurkQZmRZ1629589 = iGdOpXTurkQZmRZ32155040;     iGdOpXTurkQZmRZ32155040 = iGdOpXTurkQZmRZ10222711;     iGdOpXTurkQZmRZ10222711 = iGdOpXTurkQZmRZ22584850;     iGdOpXTurkQZmRZ22584850 = iGdOpXTurkQZmRZ75506144;     iGdOpXTurkQZmRZ75506144 = iGdOpXTurkQZmRZ63570604;     iGdOpXTurkQZmRZ63570604 = iGdOpXTurkQZmRZ10083621;     iGdOpXTurkQZmRZ10083621 = iGdOpXTurkQZmRZ59620142;     iGdOpXTurkQZmRZ59620142 = iGdOpXTurkQZmRZ66217463;     iGdOpXTurkQZmRZ66217463 = iGdOpXTurkQZmRZ84679482;     iGdOpXTurkQZmRZ84679482 = iGdOpXTurkQZmRZ4535286;     iGdOpXTurkQZmRZ4535286 = iGdOpXTurkQZmRZ92470825;     iGdOpXTurkQZmRZ92470825 = iGdOpXTurkQZmRZ95641004;     iGdOpXTurkQZmRZ95641004 = iGdOpXTurkQZmRZ4920385;     iGdOpXTurkQZmRZ4920385 = iGdOpXTurkQZmRZ3044988;     iGdOpXTurkQZmRZ3044988 = iGdOpXTurkQZmRZ80315680;     iGdOpXTurkQZmRZ80315680 = iGdOpXTurkQZmRZ76657394;     iGdOpXTurkQZmRZ76657394 = iGdOpXTurkQZmRZ45752769;     iGdOpXTurkQZmRZ45752769 = iGdOpXTurkQZmRZ43656685;     iGdOpXTurkQZmRZ43656685 = iGdOpXTurkQZmRZ62101377;     iGdOpXTurkQZmRZ62101377 = iGdOpXTurkQZmRZ30421933;     iGdOpXTurkQZmRZ30421933 = iGdOpXTurkQZmRZ85897939;     iGdOpXTurkQZmRZ85897939 = iGdOpXTurkQZmRZ14984143;     iGdOpXTurkQZmRZ14984143 = iGdOpXTurkQZmRZ87167861;     iGdOpXTurkQZmRZ87167861 = iGdOpXTurkQZmRZ8283364;     iGdOpXTurkQZmRZ8283364 = iGdOpXTurkQZmRZ6102533;     iGdOpXTurkQZmRZ6102533 = iGdOpXTurkQZmRZ22541137;     iGdOpXTurkQZmRZ22541137 = iGdOpXTurkQZmRZ39450939;     iGdOpXTurkQZmRZ39450939 = iGdOpXTurkQZmRZ12093496;     iGdOpXTurkQZmRZ12093496 = iGdOpXTurkQZmRZ85233733;     iGdOpXTurkQZmRZ85233733 = iGdOpXTurkQZmRZ43632006;     iGdOpXTurkQZmRZ43632006 = iGdOpXTurkQZmRZ6256237;     iGdOpXTurkQZmRZ6256237 = iGdOpXTurkQZmRZ98566610;     iGdOpXTurkQZmRZ98566610 = iGdOpXTurkQZmRZ70598274;     iGdOpXTurkQZmRZ70598274 = iGdOpXTurkQZmRZ24511656;     iGdOpXTurkQZmRZ24511656 = iGdOpXTurkQZmRZ15853712;     iGdOpXTurkQZmRZ15853712 = iGdOpXTurkQZmRZ7141610;     iGdOpXTurkQZmRZ7141610 = iGdOpXTurkQZmRZ28617555;     iGdOpXTurkQZmRZ28617555 = iGdOpXTurkQZmRZ38441693;     iGdOpXTurkQZmRZ38441693 = iGdOpXTurkQZmRZ58883821;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void yPVIXZTyszkArnMy60089771() {     int xEzPonZExmUrieU29535488 = -383164019;    int xEzPonZExmUrieU17106876 = -820149262;    int xEzPonZExmUrieU28729295 = -431027686;    int xEzPonZExmUrieU71156021 = -199446591;    int xEzPonZExmUrieU87307557 = 85561161;    int xEzPonZExmUrieU96159026 = -507337122;    int xEzPonZExmUrieU62186778 = -32531178;    int xEzPonZExmUrieU67658092 = -292997855;    int xEzPonZExmUrieU65449460 = -267759636;    int xEzPonZExmUrieU34145912 = -733772207;    int xEzPonZExmUrieU15300591 = -145132406;    int xEzPonZExmUrieU11632124 = -832685969;    int xEzPonZExmUrieU83981764 = -433160026;    int xEzPonZExmUrieU16764490 = -52264946;    int xEzPonZExmUrieU54744776 = -853964560;    int xEzPonZExmUrieU49899900 = -638602540;    int xEzPonZExmUrieU16370422 = -736136986;    int xEzPonZExmUrieU321893 = -800286329;    int xEzPonZExmUrieU72130906 = -353584597;    int xEzPonZExmUrieU89848338 = -285049967;    int xEzPonZExmUrieU6189494 = -145239421;    int xEzPonZExmUrieU59075766 = -879961174;    int xEzPonZExmUrieU29803607 = -790883645;    int xEzPonZExmUrieU93550141 = -69607794;    int xEzPonZExmUrieU83623051 = -885083663;    int xEzPonZExmUrieU7463854 = -646762197;    int xEzPonZExmUrieU90236203 = -314108229;    int xEzPonZExmUrieU49960519 = -472177398;    int xEzPonZExmUrieU10803160 = 81111816;    int xEzPonZExmUrieU48533791 = -673899051;    int xEzPonZExmUrieU95666598 = -889329643;    int xEzPonZExmUrieU42056747 = -523382388;    int xEzPonZExmUrieU85460309 = -418523380;    int xEzPonZExmUrieU59435732 = -483107848;    int xEzPonZExmUrieU51321551 = -501708359;    int xEzPonZExmUrieU65800553 = -447605442;    int xEzPonZExmUrieU76483263 = -765218724;    int xEzPonZExmUrieU70774797 = -829927574;    int xEzPonZExmUrieU66090861 = -559870574;    int xEzPonZExmUrieU89885418 = -853005918;    int xEzPonZExmUrieU86214528 = -59146407;    int xEzPonZExmUrieU76491742 = -656080443;    int xEzPonZExmUrieU66331281 = -464389237;    int xEzPonZExmUrieU57710705 = -105337793;    int xEzPonZExmUrieU9901895 = -965387471;    int xEzPonZExmUrieU82247812 = -303843909;    int xEzPonZExmUrieU40896100 = -12494770;    int xEzPonZExmUrieU17167695 = 67925036;    int xEzPonZExmUrieU61721806 = -197316704;    int xEzPonZExmUrieU72104015 = -437808909;    int xEzPonZExmUrieU90383158 = -449992052;    int xEzPonZExmUrieU22182742 = -983708767;    int xEzPonZExmUrieU46518410 = -2843678;    int xEzPonZExmUrieU70201116 = -403404730;    int xEzPonZExmUrieU53900278 = -351522017;    int xEzPonZExmUrieU70459721 = -503202846;    int xEzPonZExmUrieU87303268 = 70734383;    int xEzPonZExmUrieU35179154 = -261419892;    int xEzPonZExmUrieU87532970 = -314362929;    int xEzPonZExmUrieU79843703 = -267676643;    int xEzPonZExmUrieU5922823 = -93228893;    int xEzPonZExmUrieU12226259 = -560353781;    int xEzPonZExmUrieU56854933 = -274109672;    int xEzPonZExmUrieU16915670 = -593860586;    int xEzPonZExmUrieU38479314 = -844442565;    int xEzPonZExmUrieU73243844 = -621750019;    int xEzPonZExmUrieU26171814 = -314162589;    int xEzPonZExmUrieU24546033 = -950052178;    int xEzPonZExmUrieU65442939 = -550556588;    int xEzPonZExmUrieU88944223 = -306359118;    int xEzPonZExmUrieU73416637 = -873383816;    int xEzPonZExmUrieU45595624 = -906209412;    int xEzPonZExmUrieU34231031 = -140415756;    int xEzPonZExmUrieU82245488 = -500578679;    int xEzPonZExmUrieU3633811 = -125903560;    int xEzPonZExmUrieU29697751 = -489158979;    int xEzPonZExmUrieU92744485 = -315571937;    int xEzPonZExmUrieU72092902 = -585545853;    int xEzPonZExmUrieU83648246 = -104220324;    int xEzPonZExmUrieU1375239 = -481239755;    int xEzPonZExmUrieU66567754 = -534267427;    int xEzPonZExmUrieU73068508 = -282033266;    int xEzPonZExmUrieU88238712 = -174860695;    int xEzPonZExmUrieU38699144 = -481079275;    int xEzPonZExmUrieU58150633 = -123907000;    int xEzPonZExmUrieU73483856 = -905620876;    int xEzPonZExmUrieU95538336 = -420538711;    int xEzPonZExmUrieU15259194 = 84881349;    int xEzPonZExmUrieU5535455 = -31585832;    int xEzPonZExmUrieU80861829 = -998505513;    int xEzPonZExmUrieU78497284 = -418339826;    int xEzPonZExmUrieU41304110 = -403798832;    int xEzPonZExmUrieU83241826 = -415564646;    int xEzPonZExmUrieU86247158 = -192193932;    int xEzPonZExmUrieU83962595 = -659777025;    int xEzPonZExmUrieU73988269 = -498792627;    int xEzPonZExmUrieU19636810 = -281970771;    int xEzPonZExmUrieU49415612 = -870528652;    int xEzPonZExmUrieU19231392 = -260895229;    int xEzPonZExmUrieU36658051 = -383164019;     xEzPonZExmUrieU29535488 = xEzPonZExmUrieU17106876;     xEzPonZExmUrieU17106876 = xEzPonZExmUrieU28729295;     xEzPonZExmUrieU28729295 = xEzPonZExmUrieU71156021;     xEzPonZExmUrieU71156021 = xEzPonZExmUrieU87307557;     xEzPonZExmUrieU87307557 = xEzPonZExmUrieU96159026;     xEzPonZExmUrieU96159026 = xEzPonZExmUrieU62186778;     xEzPonZExmUrieU62186778 = xEzPonZExmUrieU67658092;     xEzPonZExmUrieU67658092 = xEzPonZExmUrieU65449460;     xEzPonZExmUrieU65449460 = xEzPonZExmUrieU34145912;     xEzPonZExmUrieU34145912 = xEzPonZExmUrieU15300591;     xEzPonZExmUrieU15300591 = xEzPonZExmUrieU11632124;     xEzPonZExmUrieU11632124 = xEzPonZExmUrieU83981764;     xEzPonZExmUrieU83981764 = xEzPonZExmUrieU16764490;     xEzPonZExmUrieU16764490 = xEzPonZExmUrieU54744776;     xEzPonZExmUrieU54744776 = xEzPonZExmUrieU49899900;     xEzPonZExmUrieU49899900 = xEzPonZExmUrieU16370422;     xEzPonZExmUrieU16370422 = xEzPonZExmUrieU321893;     xEzPonZExmUrieU321893 = xEzPonZExmUrieU72130906;     xEzPonZExmUrieU72130906 = xEzPonZExmUrieU89848338;     xEzPonZExmUrieU89848338 = xEzPonZExmUrieU6189494;     xEzPonZExmUrieU6189494 = xEzPonZExmUrieU59075766;     xEzPonZExmUrieU59075766 = xEzPonZExmUrieU29803607;     xEzPonZExmUrieU29803607 = xEzPonZExmUrieU93550141;     xEzPonZExmUrieU93550141 = xEzPonZExmUrieU83623051;     xEzPonZExmUrieU83623051 = xEzPonZExmUrieU7463854;     xEzPonZExmUrieU7463854 = xEzPonZExmUrieU90236203;     xEzPonZExmUrieU90236203 = xEzPonZExmUrieU49960519;     xEzPonZExmUrieU49960519 = xEzPonZExmUrieU10803160;     xEzPonZExmUrieU10803160 = xEzPonZExmUrieU48533791;     xEzPonZExmUrieU48533791 = xEzPonZExmUrieU95666598;     xEzPonZExmUrieU95666598 = xEzPonZExmUrieU42056747;     xEzPonZExmUrieU42056747 = xEzPonZExmUrieU85460309;     xEzPonZExmUrieU85460309 = xEzPonZExmUrieU59435732;     xEzPonZExmUrieU59435732 = xEzPonZExmUrieU51321551;     xEzPonZExmUrieU51321551 = xEzPonZExmUrieU65800553;     xEzPonZExmUrieU65800553 = xEzPonZExmUrieU76483263;     xEzPonZExmUrieU76483263 = xEzPonZExmUrieU70774797;     xEzPonZExmUrieU70774797 = xEzPonZExmUrieU66090861;     xEzPonZExmUrieU66090861 = xEzPonZExmUrieU89885418;     xEzPonZExmUrieU89885418 = xEzPonZExmUrieU86214528;     xEzPonZExmUrieU86214528 = xEzPonZExmUrieU76491742;     xEzPonZExmUrieU76491742 = xEzPonZExmUrieU66331281;     xEzPonZExmUrieU66331281 = xEzPonZExmUrieU57710705;     xEzPonZExmUrieU57710705 = xEzPonZExmUrieU9901895;     xEzPonZExmUrieU9901895 = xEzPonZExmUrieU82247812;     xEzPonZExmUrieU82247812 = xEzPonZExmUrieU40896100;     xEzPonZExmUrieU40896100 = xEzPonZExmUrieU17167695;     xEzPonZExmUrieU17167695 = xEzPonZExmUrieU61721806;     xEzPonZExmUrieU61721806 = xEzPonZExmUrieU72104015;     xEzPonZExmUrieU72104015 = xEzPonZExmUrieU90383158;     xEzPonZExmUrieU90383158 = xEzPonZExmUrieU22182742;     xEzPonZExmUrieU22182742 = xEzPonZExmUrieU46518410;     xEzPonZExmUrieU46518410 = xEzPonZExmUrieU70201116;     xEzPonZExmUrieU70201116 = xEzPonZExmUrieU53900278;     xEzPonZExmUrieU53900278 = xEzPonZExmUrieU70459721;     xEzPonZExmUrieU70459721 = xEzPonZExmUrieU87303268;     xEzPonZExmUrieU87303268 = xEzPonZExmUrieU35179154;     xEzPonZExmUrieU35179154 = xEzPonZExmUrieU87532970;     xEzPonZExmUrieU87532970 = xEzPonZExmUrieU79843703;     xEzPonZExmUrieU79843703 = xEzPonZExmUrieU5922823;     xEzPonZExmUrieU5922823 = xEzPonZExmUrieU12226259;     xEzPonZExmUrieU12226259 = xEzPonZExmUrieU56854933;     xEzPonZExmUrieU56854933 = xEzPonZExmUrieU16915670;     xEzPonZExmUrieU16915670 = xEzPonZExmUrieU38479314;     xEzPonZExmUrieU38479314 = xEzPonZExmUrieU73243844;     xEzPonZExmUrieU73243844 = xEzPonZExmUrieU26171814;     xEzPonZExmUrieU26171814 = xEzPonZExmUrieU24546033;     xEzPonZExmUrieU24546033 = xEzPonZExmUrieU65442939;     xEzPonZExmUrieU65442939 = xEzPonZExmUrieU88944223;     xEzPonZExmUrieU88944223 = xEzPonZExmUrieU73416637;     xEzPonZExmUrieU73416637 = xEzPonZExmUrieU45595624;     xEzPonZExmUrieU45595624 = xEzPonZExmUrieU34231031;     xEzPonZExmUrieU34231031 = xEzPonZExmUrieU82245488;     xEzPonZExmUrieU82245488 = xEzPonZExmUrieU3633811;     xEzPonZExmUrieU3633811 = xEzPonZExmUrieU29697751;     xEzPonZExmUrieU29697751 = xEzPonZExmUrieU92744485;     xEzPonZExmUrieU92744485 = xEzPonZExmUrieU72092902;     xEzPonZExmUrieU72092902 = xEzPonZExmUrieU83648246;     xEzPonZExmUrieU83648246 = xEzPonZExmUrieU1375239;     xEzPonZExmUrieU1375239 = xEzPonZExmUrieU66567754;     xEzPonZExmUrieU66567754 = xEzPonZExmUrieU73068508;     xEzPonZExmUrieU73068508 = xEzPonZExmUrieU88238712;     xEzPonZExmUrieU88238712 = xEzPonZExmUrieU38699144;     xEzPonZExmUrieU38699144 = xEzPonZExmUrieU58150633;     xEzPonZExmUrieU58150633 = xEzPonZExmUrieU73483856;     xEzPonZExmUrieU73483856 = xEzPonZExmUrieU95538336;     xEzPonZExmUrieU95538336 = xEzPonZExmUrieU15259194;     xEzPonZExmUrieU15259194 = xEzPonZExmUrieU5535455;     xEzPonZExmUrieU5535455 = xEzPonZExmUrieU80861829;     xEzPonZExmUrieU80861829 = xEzPonZExmUrieU78497284;     xEzPonZExmUrieU78497284 = xEzPonZExmUrieU41304110;     xEzPonZExmUrieU41304110 = xEzPonZExmUrieU83241826;     xEzPonZExmUrieU83241826 = xEzPonZExmUrieU86247158;     xEzPonZExmUrieU86247158 = xEzPonZExmUrieU83962595;     xEzPonZExmUrieU83962595 = xEzPonZExmUrieU73988269;     xEzPonZExmUrieU73988269 = xEzPonZExmUrieU19636810;     xEzPonZExmUrieU19636810 = xEzPonZExmUrieU49415612;     xEzPonZExmUrieU49415612 = xEzPonZExmUrieU19231392;     xEzPonZExmUrieU19231392 = xEzPonZExmUrieU36658051;     xEzPonZExmUrieU36658051 = xEzPonZExmUrieU29535488;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void EllqUCycMtcfmuIu66748516() {     int AVXgivMorSZjdGE29529016 = -255688162;    int AVXgivMorSZjdGE55679192 = -862744393;    int AVXgivMorSZjdGE30042508 = -388156299;    int AVXgivMorSZjdGE56235882 = -19987418;    int AVXgivMorSZjdGE96147659 = -382794169;    int AVXgivMorSZjdGE30964459 = -206769934;    int AVXgivMorSZjdGE30734114 = -837067645;    int AVXgivMorSZjdGE36872143 = 2824812;    int AVXgivMorSZjdGE48189625 = 18594293;    int AVXgivMorSZjdGE30785425 = -370579078;    int AVXgivMorSZjdGE35621196 = -104604479;    int AVXgivMorSZjdGE25923801 = -568164340;    int AVXgivMorSZjdGE15834679 = -454818500;    int AVXgivMorSZjdGE70608710 = -397417998;    int AVXgivMorSZjdGE99916885 = -816677430;    int AVXgivMorSZjdGE24464448 = -971398191;    int AVXgivMorSZjdGE72223180 = -555751555;    int AVXgivMorSZjdGE23322425 = -97357955;    int AVXgivMorSZjdGE92559176 = -971566863;    int AVXgivMorSZjdGE61922102 = -897074418;    int AVXgivMorSZjdGE85858917 = -955705800;    int AVXgivMorSZjdGE37372032 = -289953057;    int AVXgivMorSZjdGE66682004 = -149841488;    int AVXgivMorSZjdGE37200250 = 49471684;    int AVXgivMorSZjdGE12065741 = -211777529;    int AVXgivMorSZjdGE24037932 = -554968121;    int AVXgivMorSZjdGE60457785 = -614907942;    int AVXgivMorSZjdGE85364043 = 18855608;    int AVXgivMorSZjdGE23481607 = -564958483;    int AVXgivMorSZjdGE24623661 = -349330425;    int AVXgivMorSZjdGE29079277 = -176422877;    int AVXgivMorSZjdGE26780605 = -915204146;    int AVXgivMorSZjdGE70912732 = -620467593;    int AVXgivMorSZjdGE59718570 = -359789581;    int AVXgivMorSZjdGE33679589 = -761022280;    int AVXgivMorSZjdGE8628743 = -527371170;    int AVXgivMorSZjdGE88424630 = -864872103;    int AVXgivMorSZjdGE40692477 = -825190256;    int AVXgivMorSZjdGE1404459 = -172551978;    int AVXgivMorSZjdGE61758311 = -462437604;    int AVXgivMorSZjdGE74070196 = -147090306;    int AVXgivMorSZjdGE62189403 = -849872505;    int AVXgivMorSZjdGE26614641 = 76480511;    int AVXgivMorSZjdGE16942604 = -762302237;    int AVXgivMorSZjdGE93919947 = -338646871;    int AVXgivMorSZjdGE22675781 = -115888561;    int AVXgivMorSZjdGE68593891 = -493775393;    int AVXgivMorSZjdGE92551276 = -540836319;    int AVXgivMorSZjdGE80380195 = -861080540;    int AVXgivMorSZjdGE92149438 = -24833801;    int AVXgivMorSZjdGE3642529 = -28547059;    int AVXgivMorSZjdGE36730000 = -265854511;    int AVXgivMorSZjdGE21966758 = -994897689;    int AVXgivMorSZjdGE80451387 = -935324581;    int AVXgivMorSZjdGE80889598 = -108421290;    int AVXgivMorSZjdGE92156984 = -965735106;    int AVXgivMorSZjdGE88997188 = -612902906;    int AVXgivMorSZjdGE92842257 = -337627984;    int AVXgivMorSZjdGE44170142 = -808209890;    int AVXgivMorSZjdGE72109728 = -827826048;    int AVXgivMorSZjdGE70506673 = -591861992;    int AVXgivMorSZjdGE45370070 = -755923253;    int AVXgivMorSZjdGE13390537 = -432216705;    int AVXgivMorSZjdGE23565964 = -632075283;    int AVXgivMorSZjdGE1706149 = -94156202;    int AVXgivMorSZjdGE8840592 = -189400333;    int AVXgivMorSZjdGE55011068 = -947696748;    int AVXgivMorSZjdGE56116109 = 4971081;    int AVXgivMorSZjdGE36929121 = -636395718;    int AVXgivMorSZjdGE91288142 = -189306261;    int AVXgivMorSZjdGE36039817 = -6526089;    int AVXgivMorSZjdGE31530704 = -730561300;    int AVXgivMorSZjdGE21917967 = -924805978;    int AVXgivMorSZjdGE30800866 = -409129259;    int AVXgivMorSZjdGE87851905 = -649984112;    int AVXgivMorSZjdGE23669515 = -5833296;    int AVXgivMorSZjdGE10757392 = -266433568;    int AVXgivMorSZjdGE49739401 = -387539251;    int AVXgivMorSZjdGE43280303 = -611881445;    int AVXgivMorSZjdGE89389959 = 4111032;    int AVXgivMorSZjdGE55444040 = 38807271;    int AVXgivMorSZjdGE67906509 = 25928376;    int AVXgivMorSZjdGE4983848 = -120063853;    int AVXgivMorSZjdGE31332169 = -440124683;    int AVXgivMorSZjdGE20981133 = -220783366;    int AVXgivMorSZjdGE92349277 = -910568367;    int AVXgivMorSZjdGE4813847 = -920306458;    int AVXgivMorSZjdGE90461345 = -685143012;    int AVXgivMorSZjdGE78828971 = -151368291;    int AVXgivMorSZjdGE41522605 = -795287175;    int AVXgivMorSZjdGE19631555 = -914468264;    int AVXgivMorSZjdGE95582373 = -427244120;    int AVXgivMorSZjdGE96522334 = 83019634;    int AVXgivMorSZjdGE29294730 = -344725930;    int AVXgivMorSZjdGE91251637 = -870575612;    int AVXgivMorSZjdGE28700126 = -391167053;    int AVXgivMorSZjdGE48798867 = -317655800;    int AVXgivMorSZjdGE3048678 = -291444207;    int AVXgivMorSZjdGE15236455 = -568146035;    int AVXgivMorSZjdGE85079355 = -255688162;     AVXgivMorSZjdGE29529016 = AVXgivMorSZjdGE55679192;     AVXgivMorSZjdGE55679192 = AVXgivMorSZjdGE30042508;     AVXgivMorSZjdGE30042508 = AVXgivMorSZjdGE56235882;     AVXgivMorSZjdGE56235882 = AVXgivMorSZjdGE96147659;     AVXgivMorSZjdGE96147659 = AVXgivMorSZjdGE30964459;     AVXgivMorSZjdGE30964459 = AVXgivMorSZjdGE30734114;     AVXgivMorSZjdGE30734114 = AVXgivMorSZjdGE36872143;     AVXgivMorSZjdGE36872143 = AVXgivMorSZjdGE48189625;     AVXgivMorSZjdGE48189625 = AVXgivMorSZjdGE30785425;     AVXgivMorSZjdGE30785425 = AVXgivMorSZjdGE35621196;     AVXgivMorSZjdGE35621196 = AVXgivMorSZjdGE25923801;     AVXgivMorSZjdGE25923801 = AVXgivMorSZjdGE15834679;     AVXgivMorSZjdGE15834679 = AVXgivMorSZjdGE70608710;     AVXgivMorSZjdGE70608710 = AVXgivMorSZjdGE99916885;     AVXgivMorSZjdGE99916885 = AVXgivMorSZjdGE24464448;     AVXgivMorSZjdGE24464448 = AVXgivMorSZjdGE72223180;     AVXgivMorSZjdGE72223180 = AVXgivMorSZjdGE23322425;     AVXgivMorSZjdGE23322425 = AVXgivMorSZjdGE92559176;     AVXgivMorSZjdGE92559176 = AVXgivMorSZjdGE61922102;     AVXgivMorSZjdGE61922102 = AVXgivMorSZjdGE85858917;     AVXgivMorSZjdGE85858917 = AVXgivMorSZjdGE37372032;     AVXgivMorSZjdGE37372032 = AVXgivMorSZjdGE66682004;     AVXgivMorSZjdGE66682004 = AVXgivMorSZjdGE37200250;     AVXgivMorSZjdGE37200250 = AVXgivMorSZjdGE12065741;     AVXgivMorSZjdGE12065741 = AVXgivMorSZjdGE24037932;     AVXgivMorSZjdGE24037932 = AVXgivMorSZjdGE60457785;     AVXgivMorSZjdGE60457785 = AVXgivMorSZjdGE85364043;     AVXgivMorSZjdGE85364043 = AVXgivMorSZjdGE23481607;     AVXgivMorSZjdGE23481607 = AVXgivMorSZjdGE24623661;     AVXgivMorSZjdGE24623661 = AVXgivMorSZjdGE29079277;     AVXgivMorSZjdGE29079277 = AVXgivMorSZjdGE26780605;     AVXgivMorSZjdGE26780605 = AVXgivMorSZjdGE70912732;     AVXgivMorSZjdGE70912732 = AVXgivMorSZjdGE59718570;     AVXgivMorSZjdGE59718570 = AVXgivMorSZjdGE33679589;     AVXgivMorSZjdGE33679589 = AVXgivMorSZjdGE8628743;     AVXgivMorSZjdGE8628743 = AVXgivMorSZjdGE88424630;     AVXgivMorSZjdGE88424630 = AVXgivMorSZjdGE40692477;     AVXgivMorSZjdGE40692477 = AVXgivMorSZjdGE1404459;     AVXgivMorSZjdGE1404459 = AVXgivMorSZjdGE61758311;     AVXgivMorSZjdGE61758311 = AVXgivMorSZjdGE74070196;     AVXgivMorSZjdGE74070196 = AVXgivMorSZjdGE62189403;     AVXgivMorSZjdGE62189403 = AVXgivMorSZjdGE26614641;     AVXgivMorSZjdGE26614641 = AVXgivMorSZjdGE16942604;     AVXgivMorSZjdGE16942604 = AVXgivMorSZjdGE93919947;     AVXgivMorSZjdGE93919947 = AVXgivMorSZjdGE22675781;     AVXgivMorSZjdGE22675781 = AVXgivMorSZjdGE68593891;     AVXgivMorSZjdGE68593891 = AVXgivMorSZjdGE92551276;     AVXgivMorSZjdGE92551276 = AVXgivMorSZjdGE80380195;     AVXgivMorSZjdGE80380195 = AVXgivMorSZjdGE92149438;     AVXgivMorSZjdGE92149438 = AVXgivMorSZjdGE3642529;     AVXgivMorSZjdGE3642529 = AVXgivMorSZjdGE36730000;     AVXgivMorSZjdGE36730000 = AVXgivMorSZjdGE21966758;     AVXgivMorSZjdGE21966758 = AVXgivMorSZjdGE80451387;     AVXgivMorSZjdGE80451387 = AVXgivMorSZjdGE80889598;     AVXgivMorSZjdGE80889598 = AVXgivMorSZjdGE92156984;     AVXgivMorSZjdGE92156984 = AVXgivMorSZjdGE88997188;     AVXgivMorSZjdGE88997188 = AVXgivMorSZjdGE92842257;     AVXgivMorSZjdGE92842257 = AVXgivMorSZjdGE44170142;     AVXgivMorSZjdGE44170142 = AVXgivMorSZjdGE72109728;     AVXgivMorSZjdGE72109728 = AVXgivMorSZjdGE70506673;     AVXgivMorSZjdGE70506673 = AVXgivMorSZjdGE45370070;     AVXgivMorSZjdGE45370070 = AVXgivMorSZjdGE13390537;     AVXgivMorSZjdGE13390537 = AVXgivMorSZjdGE23565964;     AVXgivMorSZjdGE23565964 = AVXgivMorSZjdGE1706149;     AVXgivMorSZjdGE1706149 = AVXgivMorSZjdGE8840592;     AVXgivMorSZjdGE8840592 = AVXgivMorSZjdGE55011068;     AVXgivMorSZjdGE55011068 = AVXgivMorSZjdGE56116109;     AVXgivMorSZjdGE56116109 = AVXgivMorSZjdGE36929121;     AVXgivMorSZjdGE36929121 = AVXgivMorSZjdGE91288142;     AVXgivMorSZjdGE91288142 = AVXgivMorSZjdGE36039817;     AVXgivMorSZjdGE36039817 = AVXgivMorSZjdGE31530704;     AVXgivMorSZjdGE31530704 = AVXgivMorSZjdGE21917967;     AVXgivMorSZjdGE21917967 = AVXgivMorSZjdGE30800866;     AVXgivMorSZjdGE30800866 = AVXgivMorSZjdGE87851905;     AVXgivMorSZjdGE87851905 = AVXgivMorSZjdGE23669515;     AVXgivMorSZjdGE23669515 = AVXgivMorSZjdGE10757392;     AVXgivMorSZjdGE10757392 = AVXgivMorSZjdGE49739401;     AVXgivMorSZjdGE49739401 = AVXgivMorSZjdGE43280303;     AVXgivMorSZjdGE43280303 = AVXgivMorSZjdGE89389959;     AVXgivMorSZjdGE89389959 = AVXgivMorSZjdGE55444040;     AVXgivMorSZjdGE55444040 = AVXgivMorSZjdGE67906509;     AVXgivMorSZjdGE67906509 = AVXgivMorSZjdGE4983848;     AVXgivMorSZjdGE4983848 = AVXgivMorSZjdGE31332169;     AVXgivMorSZjdGE31332169 = AVXgivMorSZjdGE20981133;     AVXgivMorSZjdGE20981133 = AVXgivMorSZjdGE92349277;     AVXgivMorSZjdGE92349277 = AVXgivMorSZjdGE4813847;     AVXgivMorSZjdGE4813847 = AVXgivMorSZjdGE90461345;     AVXgivMorSZjdGE90461345 = AVXgivMorSZjdGE78828971;     AVXgivMorSZjdGE78828971 = AVXgivMorSZjdGE41522605;     AVXgivMorSZjdGE41522605 = AVXgivMorSZjdGE19631555;     AVXgivMorSZjdGE19631555 = AVXgivMorSZjdGE95582373;     AVXgivMorSZjdGE95582373 = AVXgivMorSZjdGE96522334;     AVXgivMorSZjdGE96522334 = AVXgivMorSZjdGE29294730;     AVXgivMorSZjdGE29294730 = AVXgivMorSZjdGE91251637;     AVXgivMorSZjdGE91251637 = AVXgivMorSZjdGE28700126;     AVXgivMorSZjdGE28700126 = AVXgivMorSZjdGE48798867;     AVXgivMorSZjdGE48798867 = AVXgivMorSZjdGE3048678;     AVXgivMorSZjdGE3048678 = AVXgivMorSZjdGE15236455;     AVXgivMorSZjdGE15236455 = AVXgivMorSZjdGE85079355;     AVXgivMorSZjdGE85079355 = AVXgivMorSZjdGE29529016;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void WAqqdSnTvdrQDxjw21164729() {     int lssMGVumyUxKfYV180684 = -873837291;    int lssMGVumyUxKfYV24352710 = -734387686;    int lssMGVumyUxKfYV10011427 = -830450759;    int lssMGVumyUxKfYV96167169 = -210125811;    int lssMGVumyUxKfYV17597981 = -166200508;    int lssMGVumyUxKfYV20964221 = 91648915;    int lssMGVumyUxKfYV49113009 = -835843355;    int lssMGVumyUxKfYV93714523 = -343992262;    int lssMGVumyUxKfYV94054679 = -74753035;    int lssMGVumyUxKfYV53751886 = -671674429;    int lssMGVumyUxKfYV77870357 = -139506408;    int lssMGVumyUxKfYV71470372 = -883928124;    int lssMGVumyUxKfYV95662065 = -364355558;    int lssMGVumyUxKfYV54455744 = -198446207;    int lssMGVumyUxKfYV82636905 = -966427415;    int lssMGVumyUxKfYV56993088 = -541381778;    int lssMGVumyUxKfYV66076007 = -464936685;    int lssMGVumyUxKfYV43615920 = -86524147;    int lssMGVumyUxKfYV74266958 = -578754806;    int lssMGVumyUxKfYV20708811 = -339440723;    int lssMGVumyUxKfYV40994672 = -864384935;    int lssMGVumyUxKfYV98767797 = -621238363;    int lssMGVumyUxKfYV68625812 = -717684632;    int lssMGVumyUxKfYV71049787 = -642929223;    int lssMGVumyUxKfYV76190129 = -987682204;    int lssMGVumyUxKfYV54635511 = -653252570;    int lssMGVumyUxKfYV68718570 = 89914283;    int lssMGVumyUxKfYV93146268 = -486268154;    int lssMGVumyUxKfYV55624095 = -859316716;    int lssMGVumyUxKfYV63795757 = -907725495;    int lssMGVumyUxKfYV36151273 = 75143437;    int lssMGVumyUxKfYV71292065 = -143501312;    int lssMGVumyUxKfYV53858093 = -567429205;    int lssMGVumyUxKfYV25083545 = 76680735;    int lssMGVumyUxKfYV11703825 = -607969063;    int lssMGVumyUxKfYV68622001 = -606597736;    int lssMGVumyUxKfYV32216117 = 46534690;    int lssMGVumyUxKfYV93484965 = -609303361;    int lssMGVumyUxKfYV79937747 = -128280627;    int lssMGVumyUxKfYV56861609 = 19954033;    int lssMGVumyUxKfYV34143480 = -531109184;    int lssMGVumyUxKfYV90672394 = -619261595;    int lssMGVumyUxKfYV75581601 = 89726457;    int lssMGVumyUxKfYV23450904 = -834277208;    int lssMGVumyUxKfYV89874006 = -647293267;    int lssMGVumyUxKfYV29081616 = -974225735;    int lssMGVumyUxKfYV94725095 = -609845192;    int lssMGVumyUxKfYV58165486 = -992966615;    int lssMGVumyUxKfYV85821648 = -846844534;    int lssMGVumyUxKfYV576925 = -307842674;    int lssMGVumyUxKfYV71831852 = -675119180;    int lssMGVumyUxKfYV78601504 = -235074237;    int lssMGVumyUxKfYV77042415 = -922695728;    int lssMGVumyUxKfYV70678691 = -496206078;    int lssMGVumyUxKfYV80170057 = -219362210;    int lssMGVumyUxKfYV1412887 = -152598929;    int lssMGVumyUxKfYV55726897 = 83296945;    int lssMGVumyUxKfYV38961640 = -87521537;    int lssMGVumyUxKfYV19977040 = -222443607;    int lssMGVumyUxKfYV62962469 = -512947938;    int lssMGVumyUxKfYV52245650 = -998265368;    int lssMGVumyUxKfYV55966740 = -249575201;    int lssMGVumyUxKfYV38090429 = -484675547;    int lssMGVumyUxKfYV30258922 = -167027540;    int lssMGVumyUxKfYV17600613 = -646817867;    int lssMGVumyUxKfYV6578292 = -996005096;    int lssMGVumyUxKfYV17612279 = -216498920;    int lssMGVumyUxKfYV70578521 = -341036294;    int lssMGVumyUxKfYV42751919 = -590477145;    int lssMGVumyUxKfYV14014904 = -259829679;    int lssMGVumyUxKfYV24776971 = -487916468;    int lssMGVumyUxKfYV72591042 = -855633325;    int lssMGVumyUxKfYV63678172 = -958243521;    int lssMGVumyUxKfYV17405349 = -498708839;    int lssMGVumyUxKfYV86565331 = -808331539;    int lssMGVumyUxKfYV50322278 = -145123341;    int lssMGVumyUxKfYV23186196 = -610964820;    int lssMGVumyUxKfYV45174909 = -883407425;    int lssMGVumyUxKfYV81175781 = -995635956;    int lssMGVumyUxKfYV47108513 = 86543530;    int lssMGVumyUxKfYV59910416 = 56592621;    int lssMGVumyUxKfYV10553085 = 82880897;    int lssMGVumyUxKfYV7324621 = -639423620;    int lssMGVumyUxKfYV55047170 = -451474042;    int lssMGVumyUxKfYV91963904 = -132606316;    int lssMGVumyUxKfYV57549769 = -689782326;    int lssMGVumyUxKfYV94249649 = -220805585;    int lssMGVumyUxKfYV83179402 = 28776872;    int lssMGVumyUxKfYV44913488 = -703957056;    int lssMGVumyUxKfYV10290939 = -355370135;    int lssMGVumyUxKfYV12895104 = -589894682;    int lssMGVumyUxKfYV93254477 = -865943773;    int lssMGVumyUxKfYV73507925 = -286859754;    int lssMGVumyUxKfYV16975278 = -615332689;    int lssMGVumyUxKfYV4615959 = 18219401;    int lssMGVumyUxKfYV78176739 = -181533983;    int lssMGVumyUxKfYV52581965 = -34586048;    int lssMGVumyUxKfYV45322679 = -743246004;    int lssMGVumyUxKfYV5850291 = -87459342;    int lssMGVumyUxKfYV83295714 = -873837291;     lssMGVumyUxKfYV180684 = lssMGVumyUxKfYV24352710;     lssMGVumyUxKfYV24352710 = lssMGVumyUxKfYV10011427;     lssMGVumyUxKfYV10011427 = lssMGVumyUxKfYV96167169;     lssMGVumyUxKfYV96167169 = lssMGVumyUxKfYV17597981;     lssMGVumyUxKfYV17597981 = lssMGVumyUxKfYV20964221;     lssMGVumyUxKfYV20964221 = lssMGVumyUxKfYV49113009;     lssMGVumyUxKfYV49113009 = lssMGVumyUxKfYV93714523;     lssMGVumyUxKfYV93714523 = lssMGVumyUxKfYV94054679;     lssMGVumyUxKfYV94054679 = lssMGVumyUxKfYV53751886;     lssMGVumyUxKfYV53751886 = lssMGVumyUxKfYV77870357;     lssMGVumyUxKfYV77870357 = lssMGVumyUxKfYV71470372;     lssMGVumyUxKfYV71470372 = lssMGVumyUxKfYV95662065;     lssMGVumyUxKfYV95662065 = lssMGVumyUxKfYV54455744;     lssMGVumyUxKfYV54455744 = lssMGVumyUxKfYV82636905;     lssMGVumyUxKfYV82636905 = lssMGVumyUxKfYV56993088;     lssMGVumyUxKfYV56993088 = lssMGVumyUxKfYV66076007;     lssMGVumyUxKfYV66076007 = lssMGVumyUxKfYV43615920;     lssMGVumyUxKfYV43615920 = lssMGVumyUxKfYV74266958;     lssMGVumyUxKfYV74266958 = lssMGVumyUxKfYV20708811;     lssMGVumyUxKfYV20708811 = lssMGVumyUxKfYV40994672;     lssMGVumyUxKfYV40994672 = lssMGVumyUxKfYV98767797;     lssMGVumyUxKfYV98767797 = lssMGVumyUxKfYV68625812;     lssMGVumyUxKfYV68625812 = lssMGVumyUxKfYV71049787;     lssMGVumyUxKfYV71049787 = lssMGVumyUxKfYV76190129;     lssMGVumyUxKfYV76190129 = lssMGVumyUxKfYV54635511;     lssMGVumyUxKfYV54635511 = lssMGVumyUxKfYV68718570;     lssMGVumyUxKfYV68718570 = lssMGVumyUxKfYV93146268;     lssMGVumyUxKfYV93146268 = lssMGVumyUxKfYV55624095;     lssMGVumyUxKfYV55624095 = lssMGVumyUxKfYV63795757;     lssMGVumyUxKfYV63795757 = lssMGVumyUxKfYV36151273;     lssMGVumyUxKfYV36151273 = lssMGVumyUxKfYV71292065;     lssMGVumyUxKfYV71292065 = lssMGVumyUxKfYV53858093;     lssMGVumyUxKfYV53858093 = lssMGVumyUxKfYV25083545;     lssMGVumyUxKfYV25083545 = lssMGVumyUxKfYV11703825;     lssMGVumyUxKfYV11703825 = lssMGVumyUxKfYV68622001;     lssMGVumyUxKfYV68622001 = lssMGVumyUxKfYV32216117;     lssMGVumyUxKfYV32216117 = lssMGVumyUxKfYV93484965;     lssMGVumyUxKfYV93484965 = lssMGVumyUxKfYV79937747;     lssMGVumyUxKfYV79937747 = lssMGVumyUxKfYV56861609;     lssMGVumyUxKfYV56861609 = lssMGVumyUxKfYV34143480;     lssMGVumyUxKfYV34143480 = lssMGVumyUxKfYV90672394;     lssMGVumyUxKfYV90672394 = lssMGVumyUxKfYV75581601;     lssMGVumyUxKfYV75581601 = lssMGVumyUxKfYV23450904;     lssMGVumyUxKfYV23450904 = lssMGVumyUxKfYV89874006;     lssMGVumyUxKfYV89874006 = lssMGVumyUxKfYV29081616;     lssMGVumyUxKfYV29081616 = lssMGVumyUxKfYV94725095;     lssMGVumyUxKfYV94725095 = lssMGVumyUxKfYV58165486;     lssMGVumyUxKfYV58165486 = lssMGVumyUxKfYV85821648;     lssMGVumyUxKfYV85821648 = lssMGVumyUxKfYV576925;     lssMGVumyUxKfYV576925 = lssMGVumyUxKfYV71831852;     lssMGVumyUxKfYV71831852 = lssMGVumyUxKfYV78601504;     lssMGVumyUxKfYV78601504 = lssMGVumyUxKfYV77042415;     lssMGVumyUxKfYV77042415 = lssMGVumyUxKfYV70678691;     lssMGVumyUxKfYV70678691 = lssMGVumyUxKfYV80170057;     lssMGVumyUxKfYV80170057 = lssMGVumyUxKfYV1412887;     lssMGVumyUxKfYV1412887 = lssMGVumyUxKfYV55726897;     lssMGVumyUxKfYV55726897 = lssMGVumyUxKfYV38961640;     lssMGVumyUxKfYV38961640 = lssMGVumyUxKfYV19977040;     lssMGVumyUxKfYV19977040 = lssMGVumyUxKfYV62962469;     lssMGVumyUxKfYV62962469 = lssMGVumyUxKfYV52245650;     lssMGVumyUxKfYV52245650 = lssMGVumyUxKfYV55966740;     lssMGVumyUxKfYV55966740 = lssMGVumyUxKfYV38090429;     lssMGVumyUxKfYV38090429 = lssMGVumyUxKfYV30258922;     lssMGVumyUxKfYV30258922 = lssMGVumyUxKfYV17600613;     lssMGVumyUxKfYV17600613 = lssMGVumyUxKfYV6578292;     lssMGVumyUxKfYV6578292 = lssMGVumyUxKfYV17612279;     lssMGVumyUxKfYV17612279 = lssMGVumyUxKfYV70578521;     lssMGVumyUxKfYV70578521 = lssMGVumyUxKfYV42751919;     lssMGVumyUxKfYV42751919 = lssMGVumyUxKfYV14014904;     lssMGVumyUxKfYV14014904 = lssMGVumyUxKfYV24776971;     lssMGVumyUxKfYV24776971 = lssMGVumyUxKfYV72591042;     lssMGVumyUxKfYV72591042 = lssMGVumyUxKfYV63678172;     lssMGVumyUxKfYV63678172 = lssMGVumyUxKfYV17405349;     lssMGVumyUxKfYV17405349 = lssMGVumyUxKfYV86565331;     lssMGVumyUxKfYV86565331 = lssMGVumyUxKfYV50322278;     lssMGVumyUxKfYV50322278 = lssMGVumyUxKfYV23186196;     lssMGVumyUxKfYV23186196 = lssMGVumyUxKfYV45174909;     lssMGVumyUxKfYV45174909 = lssMGVumyUxKfYV81175781;     lssMGVumyUxKfYV81175781 = lssMGVumyUxKfYV47108513;     lssMGVumyUxKfYV47108513 = lssMGVumyUxKfYV59910416;     lssMGVumyUxKfYV59910416 = lssMGVumyUxKfYV10553085;     lssMGVumyUxKfYV10553085 = lssMGVumyUxKfYV7324621;     lssMGVumyUxKfYV7324621 = lssMGVumyUxKfYV55047170;     lssMGVumyUxKfYV55047170 = lssMGVumyUxKfYV91963904;     lssMGVumyUxKfYV91963904 = lssMGVumyUxKfYV57549769;     lssMGVumyUxKfYV57549769 = lssMGVumyUxKfYV94249649;     lssMGVumyUxKfYV94249649 = lssMGVumyUxKfYV83179402;     lssMGVumyUxKfYV83179402 = lssMGVumyUxKfYV44913488;     lssMGVumyUxKfYV44913488 = lssMGVumyUxKfYV10290939;     lssMGVumyUxKfYV10290939 = lssMGVumyUxKfYV12895104;     lssMGVumyUxKfYV12895104 = lssMGVumyUxKfYV93254477;     lssMGVumyUxKfYV93254477 = lssMGVumyUxKfYV73507925;     lssMGVumyUxKfYV73507925 = lssMGVumyUxKfYV16975278;     lssMGVumyUxKfYV16975278 = lssMGVumyUxKfYV4615959;     lssMGVumyUxKfYV4615959 = lssMGVumyUxKfYV78176739;     lssMGVumyUxKfYV78176739 = lssMGVumyUxKfYV52581965;     lssMGVumyUxKfYV52581965 = lssMGVumyUxKfYV45322679;     lssMGVumyUxKfYV45322679 = lssMGVumyUxKfYV5850291;     lssMGVumyUxKfYV5850291 = lssMGVumyUxKfYV83295714;     lssMGVumyUxKfYV83295714 = lssMGVumyUxKfYV180684;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void sfWAikqNLppjmsjl27823474() {     int KTHERwlfwCjVRPq174212 = -746361434;    int KTHERwlfwCjVRPq62925026 = -776982818;    int KTHERwlfwCjVRPq11324639 = -787579373;    int KTHERwlfwCjVRPq81247029 = -30666638;    int KTHERwlfwCjVRPq26438083 = -634555837;    int KTHERwlfwCjVRPq55769653 = -707783897;    int KTHERwlfwCjVRPq17660346 = -540379822;    int KTHERwlfwCjVRPq62928574 = -48169595;    int KTHERwlfwCjVRPq76794843 = -888399106;    int KTHERwlfwCjVRPq50391400 = -308481300;    int KTHERwlfwCjVRPq98190962 = -98978481;    int KTHERwlfwCjVRPq85762049 = -619406496;    int KTHERwlfwCjVRPq27514980 = -386014033;    int KTHERwlfwCjVRPq8299964 = -543599260;    int KTHERwlfwCjVRPq27809014 = -929140285;    int KTHERwlfwCjVRPq31557636 = -874177428;    int KTHERwlfwCjVRPq21928767 = -284551255;    int KTHERwlfwCjVRPq66616452 = -483595773;    int KTHERwlfwCjVRPq94695228 = -96737071;    int KTHERwlfwCjVRPq92782573 = -951465174;    int KTHERwlfwCjVRPq20664097 = -574851314;    int KTHERwlfwCjVRPq77064063 = -31230246;    int KTHERwlfwCjVRPq5504210 = -76642474;    int KTHERwlfwCjVRPq14699896 = -523849745;    int KTHERwlfwCjVRPq4632819 = -314376069;    int KTHERwlfwCjVRPq71209589 = -561458494;    int KTHERwlfwCjVRPq38940152 = -210885430;    int KTHERwlfwCjVRPq28549794 = 4764852;    int KTHERwlfwCjVRPq68302541 = -405387015;    int KTHERwlfwCjVRPq39885627 = -583156869;    int KTHERwlfwCjVRPq69563952 = -311949797;    int KTHERwlfwCjVRPq56015923 = -535323071;    int KTHERwlfwCjVRPq39310517 = -769373418;    int KTHERwlfwCjVRPq25366382 = -900000998;    int KTHERwlfwCjVRPq94061863 = -867282984;    int KTHERwlfwCjVRPq11450192 = -686363464;    int KTHERwlfwCjVRPq44157485 = -53118688;    int KTHERwlfwCjVRPq63402645 = -604566043;    int KTHERwlfwCjVRPq15251344 = -840962031;    int KTHERwlfwCjVRPq28734502 = -689477653;    int KTHERwlfwCjVRPq21999148 = -619053083;    int KTHERwlfwCjVRPq76370055 = -813053656;    int KTHERwlfwCjVRPq35864961 = -469403796;    int KTHERwlfwCjVRPq82682802 = -391241652;    int KTHERwlfwCjVRPq73892058 = -20552668;    int KTHERwlfwCjVRPq69509584 = -786270387;    int KTHERwlfwCjVRPq22422887 = 8874186;    int KTHERwlfwCjVRPq33549067 = -501727970;    int KTHERwlfwCjVRPq4480038 = -410608371;    int KTHERwlfwCjVRPq20622348 = -994867566;    int KTHERwlfwCjVRPq85091222 = -253674188;    int KTHERwlfwCjVRPq93148761 = -617219981;    int KTHERwlfwCjVRPq52490763 = -814749739;    int KTHERwlfwCjVRPq80928962 = 71874071;    int KTHERwlfwCjVRPq7159378 = 23738518;    int KTHERwlfwCjVRPq23110149 = -615131189;    int KTHERwlfwCjVRPq57420817 = -600340344;    int KTHERwlfwCjVRPq96624743 = -163729628;    int KTHERwlfwCjVRPq76614211 = -716290569;    int KTHERwlfwCjVRPq55228494 = 26902656;    int KTHERwlfwCjVRPq16829501 = -396898467;    int KTHERwlfwCjVRPq89110551 = -445144674;    int KTHERwlfwCjVRPq94626032 = -642782580;    int KTHERwlfwCjVRPq36909217 = -205242237;    int KTHERwlfwCjVRPq80827447 = -996531503;    int KTHERwlfwCjVRPq42175040 = -563655410;    int KTHERwlfwCjVRPq46451532 = -850033079;    int KTHERwlfwCjVRPq2148598 = -486013035;    int KTHERwlfwCjVRPq14238101 = -676316276;    int KTHERwlfwCjVRPq16358823 = -142776822;    int KTHERwlfwCjVRPq87400151 = -721058740;    int KTHERwlfwCjVRPq58526122 = -679985213;    int KTHERwlfwCjVRPq51365109 = -642633743;    int KTHERwlfwCjVRPq65960726 = -407259419;    int KTHERwlfwCjVRPq70783426 = -232412092;    int KTHERwlfwCjVRPq44294041 = -761797658;    int KTHERwlfwCjVRPq41199102 = -561826451;    int KTHERwlfwCjVRPq22821408 = -685400823;    int KTHERwlfwCjVRPq40807837 = -403297078;    int KTHERwlfwCjVRPq35123235 = -528105683;    int KTHERwlfwCjVRPq48786702 = -470332681;    int KTHERwlfwCjVRPq5391086 = -709157461;    int KTHERwlfwCjVRPq24069757 = -584626778;    int KTHERwlfwCjVRPq47680194 = -410519450;    int KTHERwlfwCjVRPq54794405 = -229482682;    int KTHERwlfwCjVRPq76415190 = -694729817;    int KTHERwlfwCjVRPq3525160 = -720573332;    int KTHERwlfwCjVRPq58381554 = -741247489;    int KTHERwlfwCjVRPq18207005 = -823739516;    int KTHERwlfwCjVRPq70951714 = -152151796;    int KTHERwlfwCjVRPq54029374 = 13976880;    int KTHERwlfwCjVRPq47532741 = -889389061;    int KTHERwlfwCjVRPq86788433 = -888275475;    int KTHERwlfwCjVRPq60022849 = -767864687;    int KTHERwlfwCjVRPq11905001 = -192579186;    int KTHERwlfwCjVRPq32888596 = -73908409;    int KTHERwlfwCjVRPq81744022 = -70271077;    int KTHERwlfwCjVRPq98955744 = -164161559;    int KTHERwlfwCjVRPq1855355 = -394710149;    int KTHERwlfwCjVRPq31717019 = -746361434;     KTHERwlfwCjVRPq174212 = KTHERwlfwCjVRPq62925026;     KTHERwlfwCjVRPq62925026 = KTHERwlfwCjVRPq11324639;     KTHERwlfwCjVRPq11324639 = KTHERwlfwCjVRPq81247029;     KTHERwlfwCjVRPq81247029 = KTHERwlfwCjVRPq26438083;     KTHERwlfwCjVRPq26438083 = KTHERwlfwCjVRPq55769653;     KTHERwlfwCjVRPq55769653 = KTHERwlfwCjVRPq17660346;     KTHERwlfwCjVRPq17660346 = KTHERwlfwCjVRPq62928574;     KTHERwlfwCjVRPq62928574 = KTHERwlfwCjVRPq76794843;     KTHERwlfwCjVRPq76794843 = KTHERwlfwCjVRPq50391400;     KTHERwlfwCjVRPq50391400 = KTHERwlfwCjVRPq98190962;     KTHERwlfwCjVRPq98190962 = KTHERwlfwCjVRPq85762049;     KTHERwlfwCjVRPq85762049 = KTHERwlfwCjVRPq27514980;     KTHERwlfwCjVRPq27514980 = KTHERwlfwCjVRPq8299964;     KTHERwlfwCjVRPq8299964 = KTHERwlfwCjVRPq27809014;     KTHERwlfwCjVRPq27809014 = KTHERwlfwCjVRPq31557636;     KTHERwlfwCjVRPq31557636 = KTHERwlfwCjVRPq21928767;     KTHERwlfwCjVRPq21928767 = KTHERwlfwCjVRPq66616452;     KTHERwlfwCjVRPq66616452 = KTHERwlfwCjVRPq94695228;     KTHERwlfwCjVRPq94695228 = KTHERwlfwCjVRPq92782573;     KTHERwlfwCjVRPq92782573 = KTHERwlfwCjVRPq20664097;     KTHERwlfwCjVRPq20664097 = KTHERwlfwCjVRPq77064063;     KTHERwlfwCjVRPq77064063 = KTHERwlfwCjVRPq5504210;     KTHERwlfwCjVRPq5504210 = KTHERwlfwCjVRPq14699896;     KTHERwlfwCjVRPq14699896 = KTHERwlfwCjVRPq4632819;     KTHERwlfwCjVRPq4632819 = KTHERwlfwCjVRPq71209589;     KTHERwlfwCjVRPq71209589 = KTHERwlfwCjVRPq38940152;     KTHERwlfwCjVRPq38940152 = KTHERwlfwCjVRPq28549794;     KTHERwlfwCjVRPq28549794 = KTHERwlfwCjVRPq68302541;     KTHERwlfwCjVRPq68302541 = KTHERwlfwCjVRPq39885627;     KTHERwlfwCjVRPq39885627 = KTHERwlfwCjVRPq69563952;     KTHERwlfwCjVRPq69563952 = KTHERwlfwCjVRPq56015923;     KTHERwlfwCjVRPq56015923 = KTHERwlfwCjVRPq39310517;     KTHERwlfwCjVRPq39310517 = KTHERwlfwCjVRPq25366382;     KTHERwlfwCjVRPq25366382 = KTHERwlfwCjVRPq94061863;     KTHERwlfwCjVRPq94061863 = KTHERwlfwCjVRPq11450192;     KTHERwlfwCjVRPq11450192 = KTHERwlfwCjVRPq44157485;     KTHERwlfwCjVRPq44157485 = KTHERwlfwCjVRPq63402645;     KTHERwlfwCjVRPq63402645 = KTHERwlfwCjVRPq15251344;     KTHERwlfwCjVRPq15251344 = KTHERwlfwCjVRPq28734502;     KTHERwlfwCjVRPq28734502 = KTHERwlfwCjVRPq21999148;     KTHERwlfwCjVRPq21999148 = KTHERwlfwCjVRPq76370055;     KTHERwlfwCjVRPq76370055 = KTHERwlfwCjVRPq35864961;     KTHERwlfwCjVRPq35864961 = KTHERwlfwCjVRPq82682802;     KTHERwlfwCjVRPq82682802 = KTHERwlfwCjVRPq73892058;     KTHERwlfwCjVRPq73892058 = KTHERwlfwCjVRPq69509584;     KTHERwlfwCjVRPq69509584 = KTHERwlfwCjVRPq22422887;     KTHERwlfwCjVRPq22422887 = KTHERwlfwCjVRPq33549067;     KTHERwlfwCjVRPq33549067 = KTHERwlfwCjVRPq4480038;     KTHERwlfwCjVRPq4480038 = KTHERwlfwCjVRPq20622348;     KTHERwlfwCjVRPq20622348 = KTHERwlfwCjVRPq85091222;     KTHERwlfwCjVRPq85091222 = KTHERwlfwCjVRPq93148761;     KTHERwlfwCjVRPq93148761 = KTHERwlfwCjVRPq52490763;     KTHERwlfwCjVRPq52490763 = KTHERwlfwCjVRPq80928962;     KTHERwlfwCjVRPq80928962 = KTHERwlfwCjVRPq7159378;     KTHERwlfwCjVRPq7159378 = KTHERwlfwCjVRPq23110149;     KTHERwlfwCjVRPq23110149 = KTHERwlfwCjVRPq57420817;     KTHERwlfwCjVRPq57420817 = KTHERwlfwCjVRPq96624743;     KTHERwlfwCjVRPq96624743 = KTHERwlfwCjVRPq76614211;     KTHERwlfwCjVRPq76614211 = KTHERwlfwCjVRPq55228494;     KTHERwlfwCjVRPq55228494 = KTHERwlfwCjVRPq16829501;     KTHERwlfwCjVRPq16829501 = KTHERwlfwCjVRPq89110551;     KTHERwlfwCjVRPq89110551 = KTHERwlfwCjVRPq94626032;     KTHERwlfwCjVRPq94626032 = KTHERwlfwCjVRPq36909217;     KTHERwlfwCjVRPq36909217 = KTHERwlfwCjVRPq80827447;     KTHERwlfwCjVRPq80827447 = KTHERwlfwCjVRPq42175040;     KTHERwlfwCjVRPq42175040 = KTHERwlfwCjVRPq46451532;     KTHERwlfwCjVRPq46451532 = KTHERwlfwCjVRPq2148598;     KTHERwlfwCjVRPq2148598 = KTHERwlfwCjVRPq14238101;     KTHERwlfwCjVRPq14238101 = KTHERwlfwCjVRPq16358823;     KTHERwlfwCjVRPq16358823 = KTHERwlfwCjVRPq87400151;     KTHERwlfwCjVRPq87400151 = KTHERwlfwCjVRPq58526122;     KTHERwlfwCjVRPq58526122 = KTHERwlfwCjVRPq51365109;     KTHERwlfwCjVRPq51365109 = KTHERwlfwCjVRPq65960726;     KTHERwlfwCjVRPq65960726 = KTHERwlfwCjVRPq70783426;     KTHERwlfwCjVRPq70783426 = KTHERwlfwCjVRPq44294041;     KTHERwlfwCjVRPq44294041 = KTHERwlfwCjVRPq41199102;     KTHERwlfwCjVRPq41199102 = KTHERwlfwCjVRPq22821408;     KTHERwlfwCjVRPq22821408 = KTHERwlfwCjVRPq40807837;     KTHERwlfwCjVRPq40807837 = KTHERwlfwCjVRPq35123235;     KTHERwlfwCjVRPq35123235 = KTHERwlfwCjVRPq48786702;     KTHERwlfwCjVRPq48786702 = KTHERwlfwCjVRPq5391086;     KTHERwlfwCjVRPq5391086 = KTHERwlfwCjVRPq24069757;     KTHERwlfwCjVRPq24069757 = KTHERwlfwCjVRPq47680194;     KTHERwlfwCjVRPq47680194 = KTHERwlfwCjVRPq54794405;     KTHERwlfwCjVRPq54794405 = KTHERwlfwCjVRPq76415190;     KTHERwlfwCjVRPq76415190 = KTHERwlfwCjVRPq3525160;     KTHERwlfwCjVRPq3525160 = KTHERwlfwCjVRPq58381554;     KTHERwlfwCjVRPq58381554 = KTHERwlfwCjVRPq18207005;     KTHERwlfwCjVRPq18207005 = KTHERwlfwCjVRPq70951714;     KTHERwlfwCjVRPq70951714 = KTHERwlfwCjVRPq54029374;     KTHERwlfwCjVRPq54029374 = KTHERwlfwCjVRPq47532741;     KTHERwlfwCjVRPq47532741 = KTHERwlfwCjVRPq86788433;     KTHERwlfwCjVRPq86788433 = KTHERwlfwCjVRPq60022849;     KTHERwlfwCjVRPq60022849 = KTHERwlfwCjVRPq11905001;     KTHERwlfwCjVRPq11905001 = KTHERwlfwCjVRPq32888596;     KTHERwlfwCjVRPq32888596 = KTHERwlfwCjVRPq81744022;     KTHERwlfwCjVRPq81744022 = KTHERwlfwCjVRPq98955744;     KTHERwlfwCjVRPq98955744 = KTHERwlfwCjVRPq1855355;     KTHERwlfwCjVRPq1855355 = KTHERwlfwCjVRPq31717019;     KTHERwlfwCjVRPq31717019 = KTHERwlfwCjVRPq174212;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void NhKUgrwvJdkPqcXb82239686() {     int arMgNJAeJqmIWQY70825879 = -264510562;    int arMgNJAeJqmIWQY31598544 = -648626111;    int arMgNJAeJqmIWQY91293557 = -129873833;    int arMgNJAeJqmIWQY21178317 = -220805030;    int arMgNJAeJqmIWQY47888404 = -417962177;    int arMgNJAeJqmIWQY45769416 = -409365048;    int arMgNJAeJqmIWQY36039241 = -539155532;    int arMgNJAeJqmIWQY19770955 = -394986669;    int arMgNJAeJqmIWQY22659898 = -981746433;    int arMgNJAeJqmIWQY73357860 = -609576651;    int arMgNJAeJqmIWQY40440123 = -133880409;    int arMgNJAeJqmIWQY31308620 = -935170280;    int arMgNJAeJqmIWQY7342367 = -295551091;    int arMgNJAeJqmIWQY92146997 = -344627469;    int arMgNJAeJqmIWQY10529034 = 21109730;    int arMgNJAeJqmIWQY64086275 = -444161015;    int arMgNJAeJqmIWQY15781594 = -193736385;    int arMgNJAeJqmIWQY86909946 = -472761965;    int arMgNJAeJqmIWQY76403009 = -803925014;    int arMgNJAeJqmIWQY51569283 = -393831479;    int arMgNJAeJqmIWQY75799850 = -483530449;    int arMgNJAeJqmIWQY38459828 = -362515551;    int arMgNJAeJqmIWQY7448018 = -644485619;    int arMgNJAeJqmIWQY48549432 = -116250651;    int arMgNJAeJqmIWQY68757207 = 9719255;    int arMgNJAeJqmIWQY1807169 = -659742943;    int arMgNJAeJqmIWQY47200938 = -606063205;    int arMgNJAeJqmIWQY36332019 = -500358910;    int arMgNJAeJqmIWQY445030 = -699745247;    int arMgNJAeJqmIWQY79057722 = -41551940;    int arMgNJAeJqmIWQY76635948 = -60383483;    int arMgNJAeJqmIWQY527384 = -863620237;    int arMgNJAeJqmIWQY22255878 = -716335030;    int arMgNJAeJqmIWQY90731356 = -463530682;    int arMgNJAeJqmIWQY72086099 = -714229767;    int arMgNJAeJqmIWQY71443450 = -765590030;    int arMgNJAeJqmIWQY87948970 = -241711896;    int arMgNJAeJqmIWQY16195134 = -388679147;    int arMgNJAeJqmIWQY93784632 = -796690680;    int arMgNJAeJqmIWQY23837801 = -207086016;    int arMgNJAeJqmIWQY82072431 = 96928039;    int arMgNJAeJqmIWQY4853047 = -582442746;    int arMgNJAeJqmIWQY84831921 = -456157850;    int arMgNJAeJqmIWQY89191102 = -463216623;    int arMgNJAeJqmIWQY69846117 = -329199063;    int arMgNJAeJqmIWQY75915418 = -544607560;    int arMgNJAeJqmIWQY48554091 = -107195613;    int arMgNJAeJqmIWQY99163276 = -953858266;    int arMgNJAeJqmIWQY9921490 = -396372365;    int arMgNJAeJqmIWQY29049835 = -177876439;    int arMgNJAeJqmIWQY53280547 = -900246309;    int arMgNJAeJqmIWQY35020266 = -586439707;    int arMgNJAeJqmIWQY7566421 = -742547778;    int arMgNJAeJqmIWQY71156267 = -589007426;    int arMgNJAeJqmIWQY6439837 = -87202402;    int arMgNJAeJqmIWQY32366051 = -901995012;    int arMgNJAeJqmIWQY24150526 = 95859508;    int arMgNJAeJqmIWQY42744125 = 86376818;    int arMgNJAeJqmIWQY52421109 = -130524286;    int arMgNJAeJqmIWQY46081236 = -758219234;    int arMgNJAeJqmIWQY98568478 = -803301844;    int arMgNJAeJqmIWQY99707221 = 61203378;    int arMgNJAeJqmIWQY19325926 = -695241422;    int arMgNJAeJqmIWQY43602175 = -840194494;    int arMgNJAeJqmIWQY96721911 = -449193168;    int arMgNJAeJqmIWQY39912740 = -270260173;    int arMgNJAeJqmIWQY9052743 = -118835250;    int arMgNJAeJqmIWQY16611010 = -832020410;    int arMgNJAeJqmIWQY20060898 = -630397702;    int arMgNJAeJqmIWQY39085583 = -213300240;    int arMgNJAeJqmIWQY76137305 = -102449119;    int arMgNJAeJqmIWQY99586460 = -805057238;    int arMgNJAeJqmIWQY93125314 = -676071286;    int arMgNJAeJqmIWQY52565209 = -496838999;    int arMgNJAeJqmIWQY69496851 = -390759518;    int arMgNJAeJqmIWQY70946804 = -901087704;    int arMgNJAeJqmIWQY53627907 = -906357702;    int arMgNJAeJqmIWQY18256916 = -81268996;    int arMgNJAeJqmIWQY78703315 = -787051589;    int arMgNJAeJqmIWQY92841788 = -445673185;    int arMgNJAeJqmIWQY53253078 = -452547330;    int arMgNJAeJqmIWQY48037661 = -652204939;    int arMgNJAeJqmIWQY26410530 = -3986546;    int arMgNJAeJqmIWQY71395195 = -421868809;    int arMgNJAeJqmIWQY25777176 = -141305631;    int arMgNJAeJqmIWQY41615683 = -473943776;    int arMgNJAeJqmIWQY92960962 = -21072459;    int arMgNJAeJqmIWQY51099611 = -27327605;    int arMgNJAeJqmIWQY84291520 = -276328281;    int arMgNJAeJqmIWQY39720048 = -812234756;    int arMgNJAeJqmIWQY47292924 = -761449538;    int arMgNJAeJqmIWQY45204845 = -228088715;    int arMgNJAeJqmIWQY63774024 = -158154862;    int arMgNJAeJqmIWQY47703397 = 61528554;    int arMgNJAeJqmIWQY25269323 = -403784173;    int arMgNJAeJqmIWQY82365209 = -964275339;    int arMgNJAeJqmIWQY85527120 = -887201324;    int arMgNJAeJqmIWQY41229746 = -615963356;    int arMgNJAeJqmIWQY92469190 = 85976545;    int arMgNJAeJqmIWQY29933377 = -264510562;     arMgNJAeJqmIWQY70825879 = arMgNJAeJqmIWQY31598544;     arMgNJAeJqmIWQY31598544 = arMgNJAeJqmIWQY91293557;     arMgNJAeJqmIWQY91293557 = arMgNJAeJqmIWQY21178317;     arMgNJAeJqmIWQY21178317 = arMgNJAeJqmIWQY47888404;     arMgNJAeJqmIWQY47888404 = arMgNJAeJqmIWQY45769416;     arMgNJAeJqmIWQY45769416 = arMgNJAeJqmIWQY36039241;     arMgNJAeJqmIWQY36039241 = arMgNJAeJqmIWQY19770955;     arMgNJAeJqmIWQY19770955 = arMgNJAeJqmIWQY22659898;     arMgNJAeJqmIWQY22659898 = arMgNJAeJqmIWQY73357860;     arMgNJAeJqmIWQY73357860 = arMgNJAeJqmIWQY40440123;     arMgNJAeJqmIWQY40440123 = arMgNJAeJqmIWQY31308620;     arMgNJAeJqmIWQY31308620 = arMgNJAeJqmIWQY7342367;     arMgNJAeJqmIWQY7342367 = arMgNJAeJqmIWQY92146997;     arMgNJAeJqmIWQY92146997 = arMgNJAeJqmIWQY10529034;     arMgNJAeJqmIWQY10529034 = arMgNJAeJqmIWQY64086275;     arMgNJAeJqmIWQY64086275 = arMgNJAeJqmIWQY15781594;     arMgNJAeJqmIWQY15781594 = arMgNJAeJqmIWQY86909946;     arMgNJAeJqmIWQY86909946 = arMgNJAeJqmIWQY76403009;     arMgNJAeJqmIWQY76403009 = arMgNJAeJqmIWQY51569283;     arMgNJAeJqmIWQY51569283 = arMgNJAeJqmIWQY75799850;     arMgNJAeJqmIWQY75799850 = arMgNJAeJqmIWQY38459828;     arMgNJAeJqmIWQY38459828 = arMgNJAeJqmIWQY7448018;     arMgNJAeJqmIWQY7448018 = arMgNJAeJqmIWQY48549432;     arMgNJAeJqmIWQY48549432 = arMgNJAeJqmIWQY68757207;     arMgNJAeJqmIWQY68757207 = arMgNJAeJqmIWQY1807169;     arMgNJAeJqmIWQY1807169 = arMgNJAeJqmIWQY47200938;     arMgNJAeJqmIWQY47200938 = arMgNJAeJqmIWQY36332019;     arMgNJAeJqmIWQY36332019 = arMgNJAeJqmIWQY445030;     arMgNJAeJqmIWQY445030 = arMgNJAeJqmIWQY79057722;     arMgNJAeJqmIWQY79057722 = arMgNJAeJqmIWQY76635948;     arMgNJAeJqmIWQY76635948 = arMgNJAeJqmIWQY527384;     arMgNJAeJqmIWQY527384 = arMgNJAeJqmIWQY22255878;     arMgNJAeJqmIWQY22255878 = arMgNJAeJqmIWQY90731356;     arMgNJAeJqmIWQY90731356 = arMgNJAeJqmIWQY72086099;     arMgNJAeJqmIWQY72086099 = arMgNJAeJqmIWQY71443450;     arMgNJAeJqmIWQY71443450 = arMgNJAeJqmIWQY87948970;     arMgNJAeJqmIWQY87948970 = arMgNJAeJqmIWQY16195134;     arMgNJAeJqmIWQY16195134 = arMgNJAeJqmIWQY93784632;     arMgNJAeJqmIWQY93784632 = arMgNJAeJqmIWQY23837801;     arMgNJAeJqmIWQY23837801 = arMgNJAeJqmIWQY82072431;     arMgNJAeJqmIWQY82072431 = arMgNJAeJqmIWQY4853047;     arMgNJAeJqmIWQY4853047 = arMgNJAeJqmIWQY84831921;     arMgNJAeJqmIWQY84831921 = arMgNJAeJqmIWQY89191102;     arMgNJAeJqmIWQY89191102 = arMgNJAeJqmIWQY69846117;     arMgNJAeJqmIWQY69846117 = arMgNJAeJqmIWQY75915418;     arMgNJAeJqmIWQY75915418 = arMgNJAeJqmIWQY48554091;     arMgNJAeJqmIWQY48554091 = arMgNJAeJqmIWQY99163276;     arMgNJAeJqmIWQY99163276 = arMgNJAeJqmIWQY9921490;     arMgNJAeJqmIWQY9921490 = arMgNJAeJqmIWQY29049835;     arMgNJAeJqmIWQY29049835 = arMgNJAeJqmIWQY53280547;     arMgNJAeJqmIWQY53280547 = arMgNJAeJqmIWQY35020266;     arMgNJAeJqmIWQY35020266 = arMgNJAeJqmIWQY7566421;     arMgNJAeJqmIWQY7566421 = arMgNJAeJqmIWQY71156267;     arMgNJAeJqmIWQY71156267 = arMgNJAeJqmIWQY6439837;     arMgNJAeJqmIWQY6439837 = arMgNJAeJqmIWQY32366051;     arMgNJAeJqmIWQY32366051 = arMgNJAeJqmIWQY24150526;     arMgNJAeJqmIWQY24150526 = arMgNJAeJqmIWQY42744125;     arMgNJAeJqmIWQY42744125 = arMgNJAeJqmIWQY52421109;     arMgNJAeJqmIWQY52421109 = arMgNJAeJqmIWQY46081236;     arMgNJAeJqmIWQY46081236 = arMgNJAeJqmIWQY98568478;     arMgNJAeJqmIWQY98568478 = arMgNJAeJqmIWQY99707221;     arMgNJAeJqmIWQY99707221 = arMgNJAeJqmIWQY19325926;     arMgNJAeJqmIWQY19325926 = arMgNJAeJqmIWQY43602175;     arMgNJAeJqmIWQY43602175 = arMgNJAeJqmIWQY96721911;     arMgNJAeJqmIWQY96721911 = arMgNJAeJqmIWQY39912740;     arMgNJAeJqmIWQY39912740 = arMgNJAeJqmIWQY9052743;     arMgNJAeJqmIWQY9052743 = arMgNJAeJqmIWQY16611010;     arMgNJAeJqmIWQY16611010 = arMgNJAeJqmIWQY20060898;     arMgNJAeJqmIWQY20060898 = arMgNJAeJqmIWQY39085583;     arMgNJAeJqmIWQY39085583 = arMgNJAeJqmIWQY76137305;     arMgNJAeJqmIWQY76137305 = arMgNJAeJqmIWQY99586460;     arMgNJAeJqmIWQY99586460 = arMgNJAeJqmIWQY93125314;     arMgNJAeJqmIWQY93125314 = arMgNJAeJqmIWQY52565209;     arMgNJAeJqmIWQY52565209 = arMgNJAeJqmIWQY69496851;     arMgNJAeJqmIWQY69496851 = arMgNJAeJqmIWQY70946804;     arMgNJAeJqmIWQY70946804 = arMgNJAeJqmIWQY53627907;     arMgNJAeJqmIWQY53627907 = arMgNJAeJqmIWQY18256916;     arMgNJAeJqmIWQY18256916 = arMgNJAeJqmIWQY78703315;     arMgNJAeJqmIWQY78703315 = arMgNJAeJqmIWQY92841788;     arMgNJAeJqmIWQY92841788 = arMgNJAeJqmIWQY53253078;     arMgNJAeJqmIWQY53253078 = arMgNJAeJqmIWQY48037661;     arMgNJAeJqmIWQY48037661 = arMgNJAeJqmIWQY26410530;     arMgNJAeJqmIWQY26410530 = arMgNJAeJqmIWQY71395195;     arMgNJAeJqmIWQY71395195 = arMgNJAeJqmIWQY25777176;     arMgNJAeJqmIWQY25777176 = arMgNJAeJqmIWQY41615683;     arMgNJAeJqmIWQY41615683 = arMgNJAeJqmIWQY92960962;     arMgNJAeJqmIWQY92960962 = arMgNJAeJqmIWQY51099611;     arMgNJAeJqmIWQY51099611 = arMgNJAeJqmIWQY84291520;     arMgNJAeJqmIWQY84291520 = arMgNJAeJqmIWQY39720048;     arMgNJAeJqmIWQY39720048 = arMgNJAeJqmIWQY47292924;     arMgNJAeJqmIWQY47292924 = arMgNJAeJqmIWQY45204845;     arMgNJAeJqmIWQY45204845 = arMgNJAeJqmIWQY63774024;     arMgNJAeJqmIWQY63774024 = arMgNJAeJqmIWQY47703397;     arMgNJAeJqmIWQY47703397 = arMgNJAeJqmIWQY25269323;     arMgNJAeJqmIWQY25269323 = arMgNJAeJqmIWQY82365209;     arMgNJAeJqmIWQY82365209 = arMgNJAeJqmIWQY85527120;     arMgNJAeJqmIWQY85527120 = arMgNJAeJqmIWQY41229746;     arMgNJAeJqmIWQY41229746 = arMgNJAeJqmIWQY92469190;     arMgNJAeJqmIWQY92469190 = arMgNJAeJqmIWQY29933377;     arMgNJAeJqmIWQY29933377 = arMgNJAeJqmIWQY70825879;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ZLAjPWNtxvCwflsp88898431() {     int RTTPUuMeYQTJWBB70819407 = -137034706;    int RTTPUuMeYQTJWBB70170860 = -691221242;    int RTTPUuMeYQTJWBB92606769 = -87002446;    int RTTPUuMeYQTJWBB6258177 = -41345857;    int RTTPUuMeYQTJWBB56728506 = -886317506;    int RTTPUuMeYQTJWBB80574848 = -108797860;    int RTTPUuMeYQTJWBB4586577 = -243691999;    int RTTPUuMeYQTJWBB88985005 = -99164002;    int RTTPUuMeYQTJWBB5400062 = -695392504;    int RTTPUuMeYQTJWBB69997374 = -246383522;    int RTTPUuMeYQTJWBB60760728 = -93352482;    int RTTPUuMeYQTJWBB45600297 = -670648652;    int RTTPUuMeYQTJWBB39195281 = -317209566;    int RTTPUuMeYQTJWBB45991218 = -689780521;    int RTTPUuMeYQTJWBB55701142 = 58396860;    int RTTPUuMeYQTJWBB38650823 = -776956666;    int RTTPUuMeYQTJWBB71634353 = -13350955;    int RTTPUuMeYQTJWBB9910480 = -869833591;    int RTTPUuMeYQTJWBB96831279 = -321907280;    int RTTPUuMeYQTJWBB23643046 = 94144070;    int RTTPUuMeYQTJWBB55469275 = -193996828;    int RTTPUuMeYQTJWBB16756094 = -872507435;    int RTTPUuMeYQTJWBB44326415 = -3443461;    int RTTPUuMeYQTJWBB92199540 = 2828826;    int RTTPUuMeYQTJWBB97199896 = -416974610;    int RTTPUuMeYQTJWBB18381247 = -567948867;    int RTTPUuMeYQTJWBB17422520 = -906862918;    int RTTPUuMeYQTJWBB71735544 = -9325904;    int RTTPUuMeYQTJWBB13123477 = -245815547;    int RTTPUuMeYQTJWBB55147593 = -816983313;    int RTTPUuMeYQTJWBB10048628 = -447476717;    int RTTPUuMeYQTJWBB85251240 = -155441995;    int RTTPUuMeYQTJWBB7708301 = -918279243;    int RTTPUuMeYQTJWBB91014194 = -340212415;    int RTTPUuMeYQTJWBB54444137 = -973543689;    int RTTPUuMeYQTJWBB14271640 = -845355758;    int RTTPUuMeYQTJWBB99890338 = -341365274;    int RTTPUuMeYQTJWBB86112813 = -383941830;    int RTTPUuMeYQTJWBB29098229 = -409372083;    int RTTPUuMeYQTJWBB95710692 = -916517702;    int RTTPUuMeYQTJWBB69928099 = 8984141;    int RTTPUuMeYQTJWBB90550707 = -776234808;    int RTTPUuMeYQTJWBB45115281 = 84711898;    int RTTPUuMeYQTJWBB48423001 = -20181067;    int RTTPUuMeYQTJWBB53864169 = -802458464;    int RTTPUuMeYQTJWBB16343387 = -356652213;    int RTTPUuMeYQTJWBB76251883 = -588476236;    int RTTPUuMeYQTJWBB74546858 = -462619622;    int RTTPUuMeYQTJWBB28579879 = 39863799;    int RTTPUuMeYQTJWBB49095257 = -864901331;    int RTTPUuMeYQTJWBB66539916 = -478801316;    int RTTPUuMeYQTJWBB49567523 = -968585451;    int RTTPUuMeYQTJWBB83014768 = -634601789;    int RTTPUuMeYQTJWBB81406537 = -20927277;    int RTTPUuMeYQTJWBB33429157 = -944101674;    int RTTPUuMeYQTJWBB54063314 = -264527272;    int RTTPUuMeYQTJWBB25844446 = -587777782;    int RTTPUuMeYQTJWBB407230 = 10168727;    int RTTPUuMeYQTJWBB9058281 = -624371247;    int RTTPUuMeYQTJWBB38347260 = -218368639;    int RTTPUuMeYQTJWBB63152329 = -201934942;    int RTTPUuMeYQTJWBB32851033 = -134366095;    int RTTPUuMeYQTJWBB75861529 = -853348455;    int RTTPUuMeYQTJWBB50252469 = -878409191;    int RTTPUuMeYQTJWBB59948747 = -798906805;    int RTTPUuMeYQTJWBB75509487 = -937910487;    int RTTPUuMeYQTJWBB37891997 = -752369409;    int RTTPUuMeYQTJWBB48181087 = -976997151;    int RTTPUuMeYQTJWBB91547080 = -716236833;    int RTTPUuMeYQTJWBB41429503 = -96247383;    int RTTPUuMeYQTJWBB38760485 = -335591392;    int RTTPUuMeYQTJWBB85521540 = -629409126;    int RTTPUuMeYQTJWBB80812250 = -360461508;    int RTTPUuMeYQTJWBB1120588 = -405389579;    int RTTPUuMeYQTJWBB53714947 = -914840071;    int RTTPUuMeYQTJWBB64918567 = -417762021;    int RTTPUuMeYQTJWBB71640812 = -857219333;    int RTTPUuMeYQTJWBB95903413 = -983262395;    int RTTPUuMeYQTJWBB38335371 = -194712711;    int RTTPUuMeYQTJWBB80856509 = 39677602;    int RTTPUuMeYQTJWBB42129364 = -979472632;    int RTTPUuMeYQTJWBB42875662 = -344243297;    int RTTPUuMeYQTJWBB43155665 = 50810297;    int RTTPUuMeYQTJWBB64028219 = -380914217;    int RTTPUuMeYQTJWBB88607676 = -238181998;    int RTTPUuMeYQTJWBB60481104 = -478891267;    int RTTPUuMeYQTJWBB2236472 = -520840206;    int RTTPUuMeYQTJWBB26301763 = -797351966;    int RTTPUuMeYQTJWBB57585037 = -396110741;    int RTTPUuMeYQTJWBB380824 = -609016417;    int RTTPUuMeYQTJWBB88427194 = -157577977;    int RTTPUuMeYQTJWBB99483109 = -251534002;    int RTTPUuMeYQTJWBB77054532 = -759570583;    int RTTPUuMeYQTJWBB90750969 = -91003444;    int RTTPUuMeYQTJWBB32558364 = -614582760;    int RTTPUuMeYQTJWBB37077066 = -856649765;    int RTTPUuMeYQTJWBB14689178 = -922886353;    int RTTPUuMeYQTJWBB94862812 = -36878911;    int RTTPUuMeYQTJWBB88474254 = -221274262;    int RTTPUuMeYQTJWBB78354681 = -137034706;     RTTPUuMeYQTJWBB70819407 = RTTPUuMeYQTJWBB70170860;     RTTPUuMeYQTJWBB70170860 = RTTPUuMeYQTJWBB92606769;     RTTPUuMeYQTJWBB92606769 = RTTPUuMeYQTJWBB6258177;     RTTPUuMeYQTJWBB6258177 = RTTPUuMeYQTJWBB56728506;     RTTPUuMeYQTJWBB56728506 = RTTPUuMeYQTJWBB80574848;     RTTPUuMeYQTJWBB80574848 = RTTPUuMeYQTJWBB4586577;     RTTPUuMeYQTJWBB4586577 = RTTPUuMeYQTJWBB88985005;     RTTPUuMeYQTJWBB88985005 = RTTPUuMeYQTJWBB5400062;     RTTPUuMeYQTJWBB5400062 = RTTPUuMeYQTJWBB69997374;     RTTPUuMeYQTJWBB69997374 = RTTPUuMeYQTJWBB60760728;     RTTPUuMeYQTJWBB60760728 = RTTPUuMeYQTJWBB45600297;     RTTPUuMeYQTJWBB45600297 = RTTPUuMeYQTJWBB39195281;     RTTPUuMeYQTJWBB39195281 = RTTPUuMeYQTJWBB45991218;     RTTPUuMeYQTJWBB45991218 = RTTPUuMeYQTJWBB55701142;     RTTPUuMeYQTJWBB55701142 = RTTPUuMeYQTJWBB38650823;     RTTPUuMeYQTJWBB38650823 = RTTPUuMeYQTJWBB71634353;     RTTPUuMeYQTJWBB71634353 = RTTPUuMeYQTJWBB9910480;     RTTPUuMeYQTJWBB9910480 = RTTPUuMeYQTJWBB96831279;     RTTPUuMeYQTJWBB96831279 = RTTPUuMeYQTJWBB23643046;     RTTPUuMeYQTJWBB23643046 = RTTPUuMeYQTJWBB55469275;     RTTPUuMeYQTJWBB55469275 = RTTPUuMeYQTJWBB16756094;     RTTPUuMeYQTJWBB16756094 = RTTPUuMeYQTJWBB44326415;     RTTPUuMeYQTJWBB44326415 = RTTPUuMeYQTJWBB92199540;     RTTPUuMeYQTJWBB92199540 = RTTPUuMeYQTJWBB97199896;     RTTPUuMeYQTJWBB97199896 = RTTPUuMeYQTJWBB18381247;     RTTPUuMeYQTJWBB18381247 = RTTPUuMeYQTJWBB17422520;     RTTPUuMeYQTJWBB17422520 = RTTPUuMeYQTJWBB71735544;     RTTPUuMeYQTJWBB71735544 = RTTPUuMeYQTJWBB13123477;     RTTPUuMeYQTJWBB13123477 = RTTPUuMeYQTJWBB55147593;     RTTPUuMeYQTJWBB55147593 = RTTPUuMeYQTJWBB10048628;     RTTPUuMeYQTJWBB10048628 = RTTPUuMeYQTJWBB85251240;     RTTPUuMeYQTJWBB85251240 = RTTPUuMeYQTJWBB7708301;     RTTPUuMeYQTJWBB7708301 = RTTPUuMeYQTJWBB91014194;     RTTPUuMeYQTJWBB91014194 = RTTPUuMeYQTJWBB54444137;     RTTPUuMeYQTJWBB54444137 = RTTPUuMeYQTJWBB14271640;     RTTPUuMeYQTJWBB14271640 = RTTPUuMeYQTJWBB99890338;     RTTPUuMeYQTJWBB99890338 = RTTPUuMeYQTJWBB86112813;     RTTPUuMeYQTJWBB86112813 = RTTPUuMeYQTJWBB29098229;     RTTPUuMeYQTJWBB29098229 = RTTPUuMeYQTJWBB95710692;     RTTPUuMeYQTJWBB95710692 = RTTPUuMeYQTJWBB69928099;     RTTPUuMeYQTJWBB69928099 = RTTPUuMeYQTJWBB90550707;     RTTPUuMeYQTJWBB90550707 = RTTPUuMeYQTJWBB45115281;     RTTPUuMeYQTJWBB45115281 = RTTPUuMeYQTJWBB48423001;     RTTPUuMeYQTJWBB48423001 = RTTPUuMeYQTJWBB53864169;     RTTPUuMeYQTJWBB53864169 = RTTPUuMeYQTJWBB16343387;     RTTPUuMeYQTJWBB16343387 = RTTPUuMeYQTJWBB76251883;     RTTPUuMeYQTJWBB76251883 = RTTPUuMeYQTJWBB74546858;     RTTPUuMeYQTJWBB74546858 = RTTPUuMeYQTJWBB28579879;     RTTPUuMeYQTJWBB28579879 = RTTPUuMeYQTJWBB49095257;     RTTPUuMeYQTJWBB49095257 = RTTPUuMeYQTJWBB66539916;     RTTPUuMeYQTJWBB66539916 = RTTPUuMeYQTJWBB49567523;     RTTPUuMeYQTJWBB49567523 = RTTPUuMeYQTJWBB83014768;     RTTPUuMeYQTJWBB83014768 = RTTPUuMeYQTJWBB81406537;     RTTPUuMeYQTJWBB81406537 = RTTPUuMeYQTJWBB33429157;     RTTPUuMeYQTJWBB33429157 = RTTPUuMeYQTJWBB54063314;     RTTPUuMeYQTJWBB54063314 = RTTPUuMeYQTJWBB25844446;     RTTPUuMeYQTJWBB25844446 = RTTPUuMeYQTJWBB407230;     RTTPUuMeYQTJWBB407230 = RTTPUuMeYQTJWBB9058281;     RTTPUuMeYQTJWBB9058281 = RTTPUuMeYQTJWBB38347260;     RTTPUuMeYQTJWBB38347260 = RTTPUuMeYQTJWBB63152329;     RTTPUuMeYQTJWBB63152329 = RTTPUuMeYQTJWBB32851033;     RTTPUuMeYQTJWBB32851033 = RTTPUuMeYQTJWBB75861529;     RTTPUuMeYQTJWBB75861529 = RTTPUuMeYQTJWBB50252469;     RTTPUuMeYQTJWBB50252469 = RTTPUuMeYQTJWBB59948747;     RTTPUuMeYQTJWBB59948747 = RTTPUuMeYQTJWBB75509487;     RTTPUuMeYQTJWBB75509487 = RTTPUuMeYQTJWBB37891997;     RTTPUuMeYQTJWBB37891997 = RTTPUuMeYQTJWBB48181087;     RTTPUuMeYQTJWBB48181087 = RTTPUuMeYQTJWBB91547080;     RTTPUuMeYQTJWBB91547080 = RTTPUuMeYQTJWBB41429503;     RTTPUuMeYQTJWBB41429503 = RTTPUuMeYQTJWBB38760485;     RTTPUuMeYQTJWBB38760485 = RTTPUuMeYQTJWBB85521540;     RTTPUuMeYQTJWBB85521540 = RTTPUuMeYQTJWBB80812250;     RTTPUuMeYQTJWBB80812250 = RTTPUuMeYQTJWBB1120588;     RTTPUuMeYQTJWBB1120588 = RTTPUuMeYQTJWBB53714947;     RTTPUuMeYQTJWBB53714947 = RTTPUuMeYQTJWBB64918567;     RTTPUuMeYQTJWBB64918567 = RTTPUuMeYQTJWBB71640812;     RTTPUuMeYQTJWBB71640812 = RTTPUuMeYQTJWBB95903413;     RTTPUuMeYQTJWBB95903413 = RTTPUuMeYQTJWBB38335371;     RTTPUuMeYQTJWBB38335371 = RTTPUuMeYQTJWBB80856509;     RTTPUuMeYQTJWBB80856509 = RTTPUuMeYQTJWBB42129364;     RTTPUuMeYQTJWBB42129364 = RTTPUuMeYQTJWBB42875662;     RTTPUuMeYQTJWBB42875662 = RTTPUuMeYQTJWBB43155665;     RTTPUuMeYQTJWBB43155665 = RTTPUuMeYQTJWBB64028219;     RTTPUuMeYQTJWBB64028219 = RTTPUuMeYQTJWBB88607676;     RTTPUuMeYQTJWBB88607676 = RTTPUuMeYQTJWBB60481104;     RTTPUuMeYQTJWBB60481104 = RTTPUuMeYQTJWBB2236472;     RTTPUuMeYQTJWBB2236472 = RTTPUuMeYQTJWBB26301763;     RTTPUuMeYQTJWBB26301763 = RTTPUuMeYQTJWBB57585037;     RTTPUuMeYQTJWBB57585037 = RTTPUuMeYQTJWBB380824;     RTTPUuMeYQTJWBB380824 = RTTPUuMeYQTJWBB88427194;     RTTPUuMeYQTJWBB88427194 = RTTPUuMeYQTJWBB99483109;     RTTPUuMeYQTJWBB99483109 = RTTPUuMeYQTJWBB77054532;     RTTPUuMeYQTJWBB77054532 = RTTPUuMeYQTJWBB90750969;     RTTPUuMeYQTJWBB90750969 = RTTPUuMeYQTJWBB32558364;     RTTPUuMeYQTJWBB32558364 = RTTPUuMeYQTJWBB37077066;     RTTPUuMeYQTJWBB37077066 = RTTPUuMeYQTJWBB14689178;     RTTPUuMeYQTJWBB14689178 = RTTPUuMeYQTJWBB94862812;     RTTPUuMeYQTJWBB94862812 = RTTPUuMeYQTJWBB88474254;     RTTPUuMeYQTJWBB88474254 = RTTPUuMeYQTJWBB78354681;     RTTPUuMeYQTJWBB78354681 = RTTPUuMeYQTJWBB70819407;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void GOQqhelqRCSHZOpC73631072() {     int YRtsPxbDsfVxOPK9203795 = 47939784;    int YRtsPxbDsfVxOPK76194258 = 68157218;    int YRtsPxbDsfVxOPK40612321 = -383566137;    int YRtsPxbDsfVxOPK89761210 = -231789367;    int YRtsPxbDsfVxOPK1901412 = -488345616;    int YRtsPxbDsfVxOPK71283331 = -641836553;    int YRtsPxbDsfVxOPK94020507 = -862562342;    int YRtsPxbDsfVxOPK92286141 = -416009503;    int YRtsPxbDsfVxOPK60653837 = -626082501;    int YRtsPxbDsfVxOPK2095435 = -11418936;    int YRtsPxbDsfVxOPK61940454 = -599522240;    int YRtsPxbDsfVxOPK84285104 = 49266351;    int YRtsPxbDsfVxOPK22213534 = 58076361;    int YRtsPxbDsfVxOPK79486572 = -306413910;    int YRtsPxbDsfVxOPK44932365 = -911709225;    int YRtsPxbDsfVxOPK25667840 = 1551769;    int YRtsPxbDsfVxOPK15478768 = -826216076;    int YRtsPxbDsfVxOPK68583803 = -147178010;    int YRtsPxbDsfVxOPK18600092 = -878385799;    int YRtsPxbDsfVxOPK43311483 = -355490542;    int YRtsPxbDsfVxOPK77313748 = -626080126;    int YRtsPxbDsfVxOPK90714487 = -442114946;    int YRtsPxbDsfVxOPK13093715 = -160623777;    int YRtsPxbDsfVxOPK51120496 = -454524121;    int YRtsPxbDsfVxOPK1111916 = -441524971;    int YRtsPxbDsfVxOPK96040874 = -6418751;    int YRtsPxbDsfVxOPK70782801 = -504782907;    int YRtsPxbDsfVxOPK32180220 = -294852265;    int YRtsPxbDsfVxOPK12260848 = -504186008;    int YRtsPxbDsfVxOPK9041459 = 95083718;    int YRtsPxbDsfVxOPK9705901 = -828354029;    int YRtsPxbDsfVxOPK56312282 = -724313984;    int YRtsPxbDsfVxOPK4036456 = -838066732;    int YRtsPxbDsfVxOPK21111964 = -547748139;    int YRtsPxbDsfVxOPK74193581 = -666383631;    int YRtsPxbDsfVxOPK28631226 = -991982086;    int YRtsPxbDsfVxOPK62417048 = -412479813;    int YRtsPxbDsfVxOPK79554163 = -161751388;    int YRtsPxbDsfVxOPK5170001 = -384198150;    int YRtsPxbDsfVxOPK87013310 = -912041494;    int YRtsPxbDsfVxOPK2799353 = -765662246;    int YRtsPxbDsfVxOPK70867431 = -796000482;    int YRtsPxbDsfVxOPK8632251 = 50932580;    int YRtsPxbDsfVxOPK45381021 = -615840021;    int YRtsPxbDsfVxOPK57817431 = -787730738;    int YRtsPxbDsfVxOPK75515902 = -228428848;    int YRtsPxbDsfVxOPK98206771 = -155898904;    int YRtsPxbDsfVxOPK61332433 = -693632535;    int YRtsPxbDsfVxOPK86138469 = 4113320;    int YRtsPxbDsfVxOPK9764827 = 50088826;    int YRtsPxbDsfVxOPK68484918 = -723234212;    int YRtsPxbDsfVxOPK75908135 = -853558472;    int YRtsPxbDsfVxOPK84676826 = -494395604;    int YRtsPxbDsfVxOPK74504630 = 38396902;    int YRtsPxbDsfVxOPK96317323 = -799838023;    int YRtsPxbDsfVxOPK18489307 = -509945270;    int YRtsPxbDsfVxOPK63100544 = -771219006;    int YRtsPxbDsfVxOPK89491825 = -929042017;    int YRtsPxbDsfVxOPK88649295 = -790264397;    int YRtsPxbDsfVxOPK5860538 = -381926865;    int YRtsPxbDsfVxOPK500530 = -37053647;    int YRtsPxbDsfVxOPK61840288 = -467710078;    int YRtsPxbDsfVxOPK80025293 = -911823496;    int YRtsPxbDsfVxOPK51612378 = -621166219;    int YRtsPxbDsfVxOPK92389533 = -183064907;    int YRtsPxbDsfVxOPK5628172 = -875208256;    int YRtsPxbDsfVxOPK80248648 = -112666917;    int YRtsPxbDsfVxOPK1101570 = -394175500;    int YRtsPxbDsfVxOPK5292992 = -640030280;    int YRtsPxbDsfVxOPK16301140 = -919727140;    int YRtsPxbDsfVxOPK63250791 = -585968418;    int YRtsPxbDsfVxOPK35924605 = -564464689;    int YRtsPxbDsfVxOPK63413803 = -762979860;    int YRtsPxbDsfVxOPK31586781 = -966344306;    int YRtsPxbDsfVxOPK40512130 = -589828297;    int YRtsPxbDsfVxOPK6446318 = -830079645;    int YRtsPxbDsfVxOPK82082237 = -393047526;    int YRtsPxbDsfVxOPK67712693 = -544783756;    int YRtsPxbDsfVxOPK93303064 = -666793383;    int YRtsPxbDsfVxOPK25596014 = -113096123;    int YRtsPxbDsfVxOPK97834103 = -850519848;    int YRtsPxbDsfVxOPK9450369 = -811150372;    int YRtsPxbDsfVxOPK46041750 = -198965585;    int YRtsPxbDsfVxOPK2496022 = -454274834;    int YRtsPxbDsfVxOPK40556541 = -181682071;    int YRtsPxbDsfVxOPK33797765 = -974795557;    int YRtsPxbDsfVxOPK71635456 = -129918381;    int YRtsPxbDsfVxOPK29531826 = -776463635;    int YRtsPxbDsfVxOPK24794641 = -747910117;    int YRtsPxbDsfVxOPK55704275 = -56438361;    int YRtsPxbDsfVxOPK65530682 = -120763080;    int YRtsPxbDsfVxOPK72925223 = -483437797;    int YRtsPxbDsfVxOPK90904868 = -371486992;    int YRtsPxbDsfVxOPK99309463 = 97728714;    int YRtsPxbDsfVxOPK86512781 = -774987848;    int YRtsPxbDsfVxOPK40959065 = -197952168;    int YRtsPxbDsfVxOPK90842138 = -884176986;    int YRtsPxbDsfVxOPK57019872 = -327901201;    int YRtsPxbDsfVxOPK52991487 = -332775115;    int YRtsPxbDsfVxOPK52189259 = 47939784;     YRtsPxbDsfVxOPK9203795 = YRtsPxbDsfVxOPK76194258;     YRtsPxbDsfVxOPK76194258 = YRtsPxbDsfVxOPK40612321;     YRtsPxbDsfVxOPK40612321 = YRtsPxbDsfVxOPK89761210;     YRtsPxbDsfVxOPK89761210 = YRtsPxbDsfVxOPK1901412;     YRtsPxbDsfVxOPK1901412 = YRtsPxbDsfVxOPK71283331;     YRtsPxbDsfVxOPK71283331 = YRtsPxbDsfVxOPK94020507;     YRtsPxbDsfVxOPK94020507 = YRtsPxbDsfVxOPK92286141;     YRtsPxbDsfVxOPK92286141 = YRtsPxbDsfVxOPK60653837;     YRtsPxbDsfVxOPK60653837 = YRtsPxbDsfVxOPK2095435;     YRtsPxbDsfVxOPK2095435 = YRtsPxbDsfVxOPK61940454;     YRtsPxbDsfVxOPK61940454 = YRtsPxbDsfVxOPK84285104;     YRtsPxbDsfVxOPK84285104 = YRtsPxbDsfVxOPK22213534;     YRtsPxbDsfVxOPK22213534 = YRtsPxbDsfVxOPK79486572;     YRtsPxbDsfVxOPK79486572 = YRtsPxbDsfVxOPK44932365;     YRtsPxbDsfVxOPK44932365 = YRtsPxbDsfVxOPK25667840;     YRtsPxbDsfVxOPK25667840 = YRtsPxbDsfVxOPK15478768;     YRtsPxbDsfVxOPK15478768 = YRtsPxbDsfVxOPK68583803;     YRtsPxbDsfVxOPK68583803 = YRtsPxbDsfVxOPK18600092;     YRtsPxbDsfVxOPK18600092 = YRtsPxbDsfVxOPK43311483;     YRtsPxbDsfVxOPK43311483 = YRtsPxbDsfVxOPK77313748;     YRtsPxbDsfVxOPK77313748 = YRtsPxbDsfVxOPK90714487;     YRtsPxbDsfVxOPK90714487 = YRtsPxbDsfVxOPK13093715;     YRtsPxbDsfVxOPK13093715 = YRtsPxbDsfVxOPK51120496;     YRtsPxbDsfVxOPK51120496 = YRtsPxbDsfVxOPK1111916;     YRtsPxbDsfVxOPK1111916 = YRtsPxbDsfVxOPK96040874;     YRtsPxbDsfVxOPK96040874 = YRtsPxbDsfVxOPK70782801;     YRtsPxbDsfVxOPK70782801 = YRtsPxbDsfVxOPK32180220;     YRtsPxbDsfVxOPK32180220 = YRtsPxbDsfVxOPK12260848;     YRtsPxbDsfVxOPK12260848 = YRtsPxbDsfVxOPK9041459;     YRtsPxbDsfVxOPK9041459 = YRtsPxbDsfVxOPK9705901;     YRtsPxbDsfVxOPK9705901 = YRtsPxbDsfVxOPK56312282;     YRtsPxbDsfVxOPK56312282 = YRtsPxbDsfVxOPK4036456;     YRtsPxbDsfVxOPK4036456 = YRtsPxbDsfVxOPK21111964;     YRtsPxbDsfVxOPK21111964 = YRtsPxbDsfVxOPK74193581;     YRtsPxbDsfVxOPK74193581 = YRtsPxbDsfVxOPK28631226;     YRtsPxbDsfVxOPK28631226 = YRtsPxbDsfVxOPK62417048;     YRtsPxbDsfVxOPK62417048 = YRtsPxbDsfVxOPK79554163;     YRtsPxbDsfVxOPK79554163 = YRtsPxbDsfVxOPK5170001;     YRtsPxbDsfVxOPK5170001 = YRtsPxbDsfVxOPK87013310;     YRtsPxbDsfVxOPK87013310 = YRtsPxbDsfVxOPK2799353;     YRtsPxbDsfVxOPK2799353 = YRtsPxbDsfVxOPK70867431;     YRtsPxbDsfVxOPK70867431 = YRtsPxbDsfVxOPK8632251;     YRtsPxbDsfVxOPK8632251 = YRtsPxbDsfVxOPK45381021;     YRtsPxbDsfVxOPK45381021 = YRtsPxbDsfVxOPK57817431;     YRtsPxbDsfVxOPK57817431 = YRtsPxbDsfVxOPK75515902;     YRtsPxbDsfVxOPK75515902 = YRtsPxbDsfVxOPK98206771;     YRtsPxbDsfVxOPK98206771 = YRtsPxbDsfVxOPK61332433;     YRtsPxbDsfVxOPK61332433 = YRtsPxbDsfVxOPK86138469;     YRtsPxbDsfVxOPK86138469 = YRtsPxbDsfVxOPK9764827;     YRtsPxbDsfVxOPK9764827 = YRtsPxbDsfVxOPK68484918;     YRtsPxbDsfVxOPK68484918 = YRtsPxbDsfVxOPK75908135;     YRtsPxbDsfVxOPK75908135 = YRtsPxbDsfVxOPK84676826;     YRtsPxbDsfVxOPK84676826 = YRtsPxbDsfVxOPK74504630;     YRtsPxbDsfVxOPK74504630 = YRtsPxbDsfVxOPK96317323;     YRtsPxbDsfVxOPK96317323 = YRtsPxbDsfVxOPK18489307;     YRtsPxbDsfVxOPK18489307 = YRtsPxbDsfVxOPK63100544;     YRtsPxbDsfVxOPK63100544 = YRtsPxbDsfVxOPK89491825;     YRtsPxbDsfVxOPK89491825 = YRtsPxbDsfVxOPK88649295;     YRtsPxbDsfVxOPK88649295 = YRtsPxbDsfVxOPK5860538;     YRtsPxbDsfVxOPK5860538 = YRtsPxbDsfVxOPK500530;     YRtsPxbDsfVxOPK500530 = YRtsPxbDsfVxOPK61840288;     YRtsPxbDsfVxOPK61840288 = YRtsPxbDsfVxOPK80025293;     YRtsPxbDsfVxOPK80025293 = YRtsPxbDsfVxOPK51612378;     YRtsPxbDsfVxOPK51612378 = YRtsPxbDsfVxOPK92389533;     YRtsPxbDsfVxOPK92389533 = YRtsPxbDsfVxOPK5628172;     YRtsPxbDsfVxOPK5628172 = YRtsPxbDsfVxOPK80248648;     YRtsPxbDsfVxOPK80248648 = YRtsPxbDsfVxOPK1101570;     YRtsPxbDsfVxOPK1101570 = YRtsPxbDsfVxOPK5292992;     YRtsPxbDsfVxOPK5292992 = YRtsPxbDsfVxOPK16301140;     YRtsPxbDsfVxOPK16301140 = YRtsPxbDsfVxOPK63250791;     YRtsPxbDsfVxOPK63250791 = YRtsPxbDsfVxOPK35924605;     YRtsPxbDsfVxOPK35924605 = YRtsPxbDsfVxOPK63413803;     YRtsPxbDsfVxOPK63413803 = YRtsPxbDsfVxOPK31586781;     YRtsPxbDsfVxOPK31586781 = YRtsPxbDsfVxOPK40512130;     YRtsPxbDsfVxOPK40512130 = YRtsPxbDsfVxOPK6446318;     YRtsPxbDsfVxOPK6446318 = YRtsPxbDsfVxOPK82082237;     YRtsPxbDsfVxOPK82082237 = YRtsPxbDsfVxOPK67712693;     YRtsPxbDsfVxOPK67712693 = YRtsPxbDsfVxOPK93303064;     YRtsPxbDsfVxOPK93303064 = YRtsPxbDsfVxOPK25596014;     YRtsPxbDsfVxOPK25596014 = YRtsPxbDsfVxOPK97834103;     YRtsPxbDsfVxOPK97834103 = YRtsPxbDsfVxOPK9450369;     YRtsPxbDsfVxOPK9450369 = YRtsPxbDsfVxOPK46041750;     YRtsPxbDsfVxOPK46041750 = YRtsPxbDsfVxOPK2496022;     YRtsPxbDsfVxOPK2496022 = YRtsPxbDsfVxOPK40556541;     YRtsPxbDsfVxOPK40556541 = YRtsPxbDsfVxOPK33797765;     YRtsPxbDsfVxOPK33797765 = YRtsPxbDsfVxOPK71635456;     YRtsPxbDsfVxOPK71635456 = YRtsPxbDsfVxOPK29531826;     YRtsPxbDsfVxOPK29531826 = YRtsPxbDsfVxOPK24794641;     YRtsPxbDsfVxOPK24794641 = YRtsPxbDsfVxOPK55704275;     YRtsPxbDsfVxOPK55704275 = YRtsPxbDsfVxOPK65530682;     YRtsPxbDsfVxOPK65530682 = YRtsPxbDsfVxOPK72925223;     YRtsPxbDsfVxOPK72925223 = YRtsPxbDsfVxOPK90904868;     YRtsPxbDsfVxOPK90904868 = YRtsPxbDsfVxOPK99309463;     YRtsPxbDsfVxOPK99309463 = YRtsPxbDsfVxOPK86512781;     YRtsPxbDsfVxOPK86512781 = YRtsPxbDsfVxOPK40959065;     YRtsPxbDsfVxOPK40959065 = YRtsPxbDsfVxOPK90842138;     YRtsPxbDsfVxOPK90842138 = YRtsPxbDsfVxOPK57019872;     YRtsPxbDsfVxOPK57019872 = YRtsPxbDsfVxOPK52991487;     YRtsPxbDsfVxOPK52991487 = YRtsPxbDsfVxOPK52189259;     YRtsPxbDsfVxOPK52189259 = YRtsPxbDsfVxOPK9203795;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ssevAkcWsWNzsGPq80289816() {     float pNdeHuJSXJAzgwv9197324 = -924584357;    float pNdeHuJSXJAzgwv14766575 = 25562093;    float pNdeHuJSXJAzgwv41925533 = -340694751;    float pNdeHuJSXJAzgwv74841071 = -52330197;    float pNdeHuJSXJAzgwv10741514 = -956700937;    float pNdeHuJSXJAzgwv6088764 = -341269365;    float pNdeHuJSXJAzgwv62567843 = -567098809;    float pNdeHuJSXJAzgwv61500192 = -120186820;    float pNdeHuJSXJAzgwv43394001 = -339728571;    float pNdeHuJSXJAzgwv98734947 = -748225807;    float pNdeHuJSXJAzgwv82261059 = -558994312;    float pNdeHuJSXJAzgwv98576781 = -786212012;    float pNdeHuJSXJAzgwv54066447 = 36417886;    float pNdeHuJSXJAzgwv33330793 = -651566962;    float pNdeHuJSXJAzgwv90104474 = -874422077;    float pNdeHuJSXJAzgwv232388 = -331243881;    float pNdeHuJSXJAzgwv71331527 = -645830646;    float pNdeHuJSXJAzgwv91584336 = -544249632;    float pNdeHuJSXJAzgwv39028362 = -396368066;    float pNdeHuJSXJAzgwv15385246 = -967514993;    float pNdeHuJSXJAzgwv56983173 = -336546500;    float pNdeHuJSXJAzgwv69010753 = -952106829;    float pNdeHuJSXJAzgwv49972111 = -619581619;    float pNdeHuJSXJAzgwv94770604 = -335444643;    float pNdeHuJSXJAzgwv29554606 = -868218824;    float pNdeHuJSXJAzgwv12614952 = 85375320;    float pNdeHuJSXJAzgwv41004383 = -805582620;    float pNdeHuJSXJAzgwv67583744 = -903819253;    float pNdeHuJSXJAzgwv24939295 = -50256323;    float pNdeHuJSXJAzgwv85131329 = -680347656;    float pNdeHuJSXJAzgwv43118579 = -115447264;    float pNdeHuJSXJAzgwv41036140 = -16135746;    float pNdeHuJSXJAzgwv89488878 = 59989052;    float pNdeHuJSXJAzgwv21394801 = -424429872;    float pNdeHuJSXJAzgwv56551619 = -925697556;    float pNdeHuJSXJAzgwv71459415 = 28252168;    float pNdeHuJSXJAzgwv74358416 = -512133191;    float pNdeHuJSXJAzgwv49471843 = -157014067;    float pNdeHuJSXJAzgwv40483597 = 3120434;    float pNdeHuJSXJAzgwv58886203 = -521473181;    float pNdeHuJSXJAzgwv90655020 = -853606144;    float pNdeHuJSXJAzgwv56565092 = -989792564;    float pNdeHuJSXJAzgwv68915610 = -508197674;    float pNdeHuJSXJAzgwv4612920 = -172804465;    float pNdeHuJSXJAzgwv41835484 = -160990140;    float pNdeHuJSXJAzgwv15943870 = -40473519;    float pNdeHuJSXJAzgwv25904564 = -637179526;    float pNdeHuJSXJAzgwv36716014 = -202393892;    float pNdeHuJSXJAzgwv4796860 = -659650541;    float pNdeHuJSXJAzgwv29810250 = -636936060;    float pNdeHuJSXJAzgwv81744287 = -301789219;    float pNdeHuJSXJAzgwv90455392 = -135704220;    float pNdeHuJSXJAzgwv60125174 = -386449613;    float pNdeHuJSXJAzgwv84754900 = -493522949;    float pNdeHuJSXJAzgwv23306644 = -556737300;    float pNdeHuJSXJAzgwv40186570 = -972477529;    float pNdeHuJSXJAzgwv64794463 = -354856289;    float pNdeHuJSXJAzgwv47154929 = 94749892;    float pNdeHuJSXJAzgwv45286466 = -184111374;    float pNdeHuJSXJAzgwv98126562 = -942076258;    float pNdeHuJSXJAzgwv65084380 = -535686745;    float pNdeHuJSXJAzgwv94984099 = -663279556;    float pNdeHuJSXJAzgwv36560897 = 30069502;    float pNdeHuJSXJAzgwv58262672 = -659380916;    float pNdeHuJSXJAzgwv55616369 = -532778544;    float pNdeHuJSXJAzgwv41224920 = -442858567;    float pNdeHuJSXJAzgwv9087903 = -746201064;    float pNdeHuJSXJAzgwv32671647 = -539152242;    float pNdeHuJSXJAzgwv76779173 = -725869407;    float pNdeHuJSXJAzgwv18645059 = -802674246;    float pNdeHuJSXJAzgwv25873971 = -819110691;    float pNdeHuJSXJAzgwv21859685 = -388816580;    float pNdeHuJSXJAzgwv51100739 = -447370066;    float pNdeHuJSXJAzgwv80142158 = -874894886;    float pNdeHuJSXJAzgwv24730226 = -13908850;    float pNdeHuJSXJAzgwv418081 = -346753936;    float pNdeHuJSXJAzgwv95144 = -343909155;    float pNdeHuJSXJAzgwv45359192 = -346777155;    float pNdeHuJSXJAzgwv52935121 = -74454504;    float pNdeHuJSXJAzgwv13610736 = -727745305;    float pNdeHuJSXJAzgwv86710387 = -277445154;    float pNdeHuJSXJAzgwv4288370 = -503188729;    float pNdeHuJSXJAzgwv62786885 = -144168712;    float pNdeHuJSXJAzgwv95129045 = -413320263;    float pNdeHuJSXJAzgwv3387042 = -278558437;    float pNdeHuJSXJAzgwv52663186 = -979743044;    float pNdeHuJSXJAzgwv80910965 = -629686134;    float pNdeHuJSXJAzgwv4733978 = -446488000;    float pNdeHuJSXJAzgwv98088157 = -867692572;    float pNdeHuJSXJAzgwv16365050 = -953220028;    float pNdeHuJSXJAzgwv6664952 = -616891543;    float pNdeHuJSXJAzgwv27203488 = -506883084;    float pNdeHuJSXJAzgwv4185378 = -972902694;    float pNdeHuJSXJAzgwv42357035 = -54803309;    float pNdeHuJSXJAzgwv93801823 = -985786436;    float pNdeHuJSXJAzgwv95670921 = -90326588;    float pNdeHuJSXJAzgwv20004196 = -919862066;    float pNdeHuJSXJAzgwv10652939 = -848816759;    float pNdeHuJSXJAzgwv48996551 = -640025922;    float pNdeHuJSXJAzgwv610564 = -924584357;     pNdeHuJSXJAzgwv9197324 = pNdeHuJSXJAzgwv14766575;     pNdeHuJSXJAzgwv14766575 = pNdeHuJSXJAzgwv41925533;     pNdeHuJSXJAzgwv41925533 = pNdeHuJSXJAzgwv74841071;     pNdeHuJSXJAzgwv74841071 = pNdeHuJSXJAzgwv10741514;     pNdeHuJSXJAzgwv10741514 = pNdeHuJSXJAzgwv6088764;     pNdeHuJSXJAzgwv6088764 = pNdeHuJSXJAzgwv62567843;     pNdeHuJSXJAzgwv62567843 = pNdeHuJSXJAzgwv61500192;     pNdeHuJSXJAzgwv61500192 = pNdeHuJSXJAzgwv43394001;     pNdeHuJSXJAzgwv43394001 = pNdeHuJSXJAzgwv98734947;     pNdeHuJSXJAzgwv98734947 = pNdeHuJSXJAzgwv82261059;     pNdeHuJSXJAzgwv82261059 = pNdeHuJSXJAzgwv98576781;     pNdeHuJSXJAzgwv98576781 = pNdeHuJSXJAzgwv54066447;     pNdeHuJSXJAzgwv54066447 = pNdeHuJSXJAzgwv33330793;     pNdeHuJSXJAzgwv33330793 = pNdeHuJSXJAzgwv90104474;     pNdeHuJSXJAzgwv90104474 = pNdeHuJSXJAzgwv232388;     pNdeHuJSXJAzgwv232388 = pNdeHuJSXJAzgwv71331527;     pNdeHuJSXJAzgwv71331527 = pNdeHuJSXJAzgwv91584336;     pNdeHuJSXJAzgwv91584336 = pNdeHuJSXJAzgwv39028362;     pNdeHuJSXJAzgwv39028362 = pNdeHuJSXJAzgwv15385246;     pNdeHuJSXJAzgwv15385246 = pNdeHuJSXJAzgwv56983173;     pNdeHuJSXJAzgwv56983173 = pNdeHuJSXJAzgwv69010753;     pNdeHuJSXJAzgwv69010753 = pNdeHuJSXJAzgwv49972111;     pNdeHuJSXJAzgwv49972111 = pNdeHuJSXJAzgwv94770604;     pNdeHuJSXJAzgwv94770604 = pNdeHuJSXJAzgwv29554606;     pNdeHuJSXJAzgwv29554606 = pNdeHuJSXJAzgwv12614952;     pNdeHuJSXJAzgwv12614952 = pNdeHuJSXJAzgwv41004383;     pNdeHuJSXJAzgwv41004383 = pNdeHuJSXJAzgwv67583744;     pNdeHuJSXJAzgwv67583744 = pNdeHuJSXJAzgwv24939295;     pNdeHuJSXJAzgwv24939295 = pNdeHuJSXJAzgwv85131329;     pNdeHuJSXJAzgwv85131329 = pNdeHuJSXJAzgwv43118579;     pNdeHuJSXJAzgwv43118579 = pNdeHuJSXJAzgwv41036140;     pNdeHuJSXJAzgwv41036140 = pNdeHuJSXJAzgwv89488878;     pNdeHuJSXJAzgwv89488878 = pNdeHuJSXJAzgwv21394801;     pNdeHuJSXJAzgwv21394801 = pNdeHuJSXJAzgwv56551619;     pNdeHuJSXJAzgwv56551619 = pNdeHuJSXJAzgwv71459415;     pNdeHuJSXJAzgwv71459415 = pNdeHuJSXJAzgwv74358416;     pNdeHuJSXJAzgwv74358416 = pNdeHuJSXJAzgwv49471843;     pNdeHuJSXJAzgwv49471843 = pNdeHuJSXJAzgwv40483597;     pNdeHuJSXJAzgwv40483597 = pNdeHuJSXJAzgwv58886203;     pNdeHuJSXJAzgwv58886203 = pNdeHuJSXJAzgwv90655020;     pNdeHuJSXJAzgwv90655020 = pNdeHuJSXJAzgwv56565092;     pNdeHuJSXJAzgwv56565092 = pNdeHuJSXJAzgwv68915610;     pNdeHuJSXJAzgwv68915610 = pNdeHuJSXJAzgwv4612920;     pNdeHuJSXJAzgwv4612920 = pNdeHuJSXJAzgwv41835484;     pNdeHuJSXJAzgwv41835484 = pNdeHuJSXJAzgwv15943870;     pNdeHuJSXJAzgwv15943870 = pNdeHuJSXJAzgwv25904564;     pNdeHuJSXJAzgwv25904564 = pNdeHuJSXJAzgwv36716014;     pNdeHuJSXJAzgwv36716014 = pNdeHuJSXJAzgwv4796860;     pNdeHuJSXJAzgwv4796860 = pNdeHuJSXJAzgwv29810250;     pNdeHuJSXJAzgwv29810250 = pNdeHuJSXJAzgwv81744287;     pNdeHuJSXJAzgwv81744287 = pNdeHuJSXJAzgwv90455392;     pNdeHuJSXJAzgwv90455392 = pNdeHuJSXJAzgwv60125174;     pNdeHuJSXJAzgwv60125174 = pNdeHuJSXJAzgwv84754900;     pNdeHuJSXJAzgwv84754900 = pNdeHuJSXJAzgwv23306644;     pNdeHuJSXJAzgwv23306644 = pNdeHuJSXJAzgwv40186570;     pNdeHuJSXJAzgwv40186570 = pNdeHuJSXJAzgwv64794463;     pNdeHuJSXJAzgwv64794463 = pNdeHuJSXJAzgwv47154929;     pNdeHuJSXJAzgwv47154929 = pNdeHuJSXJAzgwv45286466;     pNdeHuJSXJAzgwv45286466 = pNdeHuJSXJAzgwv98126562;     pNdeHuJSXJAzgwv98126562 = pNdeHuJSXJAzgwv65084380;     pNdeHuJSXJAzgwv65084380 = pNdeHuJSXJAzgwv94984099;     pNdeHuJSXJAzgwv94984099 = pNdeHuJSXJAzgwv36560897;     pNdeHuJSXJAzgwv36560897 = pNdeHuJSXJAzgwv58262672;     pNdeHuJSXJAzgwv58262672 = pNdeHuJSXJAzgwv55616369;     pNdeHuJSXJAzgwv55616369 = pNdeHuJSXJAzgwv41224920;     pNdeHuJSXJAzgwv41224920 = pNdeHuJSXJAzgwv9087903;     pNdeHuJSXJAzgwv9087903 = pNdeHuJSXJAzgwv32671647;     pNdeHuJSXJAzgwv32671647 = pNdeHuJSXJAzgwv76779173;     pNdeHuJSXJAzgwv76779173 = pNdeHuJSXJAzgwv18645059;     pNdeHuJSXJAzgwv18645059 = pNdeHuJSXJAzgwv25873971;     pNdeHuJSXJAzgwv25873971 = pNdeHuJSXJAzgwv21859685;     pNdeHuJSXJAzgwv21859685 = pNdeHuJSXJAzgwv51100739;     pNdeHuJSXJAzgwv51100739 = pNdeHuJSXJAzgwv80142158;     pNdeHuJSXJAzgwv80142158 = pNdeHuJSXJAzgwv24730226;     pNdeHuJSXJAzgwv24730226 = pNdeHuJSXJAzgwv418081;     pNdeHuJSXJAzgwv418081 = pNdeHuJSXJAzgwv95144;     pNdeHuJSXJAzgwv95144 = pNdeHuJSXJAzgwv45359192;     pNdeHuJSXJAzgwv45359192 = pNdeHuJSXJAzgwv52935121;     pNdeHuJSXJAzgwv52935121 = pNdeHuJSXJAzgwv13610736;     pNdeHuJSXJAzgwv13610736 = pNdeHuJSXJAzgwv86710387;     pNdeHuJSXJAzgwv86710387 = pNdeHuJSXJAzgwv4288370;     pNdeHuJSXJAzgwv4288370 = pNdeHuJSXJAzgwv62786885;     pNdeHuJSXJAzgwv62786885 = pNdeHuJSXJAzgwv95129045;     pNdeHuJSXJAzgwv95129045 = pNdeHuJSXJAzgwv3387042;     pNdeHuJSXJAzgwv3387042 = pNdeHuJSXJAzgwv52663186;     pNdeHuJSXJAzgwv52663186 = pNdeHuJSXJAzgwv80910965;     pNdeHuJSXJAzgwv80910965 = pNdeHuJSXJAzgwv4733978;     pNdeHuJSXJAzgwv4733978 = pNdeHuJSXJAzgwv98088157;     pNdeHuJSXJAzgwv98088157 = pNdeHuJSXJAzgwv16365050;     pNdeHuJSXJAzgwv16365050 = pNdeHuJSXJAzgwv6664952;     pNdeHuJSXJAzgwv6664952 = pNdeHuJSXJAzgwv27203488;     pNdeHuJSXJAzgwv27203488 = pNdeHuJSXJAzgwv4185378;     pNdeHuJSXJAzgwv4185378 = pNdeHuJSXJAzgwv42357035;     pNdeHuJSXJAzgwv42357035 = pNdeHuJSXJAzgwv93801823;     pNdeHuJSXJAzgwv93801823 = pNdeHuJSXJAzgwv95670921;     pNdeHuJSXJAzgwv95670921 = pNdeHuJSXJAzgwv20004196;     pNdeHuJSXJAzgwv20004196 = pNdeHuJSXJAzgwv10652939;     pNdeHuJSXJAzgwv10652939 = pNdeHuJSXJAzgwv48996551;     pNdeHuJSXJAzgwv48996551 = pNdeHuJSXJAzgwv610564;     pNdeHuJSXJAzgwv610564 = pNdeHuJSXJAzgwv9197324;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void ZcwPmiLGIbgstXlm34706030() {     float NIADpiZGndGylLe79848990 = -442733487;    float NIADpiZGndGylLe83440092 = -946081207;    float NIADpiZGndGylLe21894452 = -782989210;    float NIADpiZGndGylLe14772359 = -242468586;    float NIADpiZGndGylLe32191835 = -740107284;    float NIADpiZGndGylLe96088525 = -42850517;    float NIADpiZGndGylLe80946739 = -565874519;    float NIADpiZGndGylLe18342573 = -467003910;    float NIADpiZGndGylLe89259055 = -433075900;    float NIADpiZGndGylLe21701409 = 50678843;    float NIADpiZGndGylLe24510220 = -593896241;    float NIADpiZGndGylLe44123352 = -1975805;    float NIADpiZGndGylLe33893834 = -973119172;    float NIADpiZGndGylLe17177827 = -452595172;    float NIADpiZGndGylLe72824493 = 75827920;    float NIADpiZGndGylLe32761028 = 98772532;    float NIADpiZGndGylLe65184354 = -555015776;    float NIADpiZGndGylLe11877831 = -533415828;    float NIADpiZGndGylLe20736143 = -3556008;    float NIADpiZGndGylLe74171955 = -409881298;    float NIADpiZGndGylLe12118928 = -245225640;    float NIADpiZGndGylLe30406519 = -183392134;    float NIADpiZGndGylLe51915920 = -87424764;    float NIADpiZGndGylLe28620142 = 72154450;    float NIADpiZGndGylLe93678993 = -544123512;    float NIADpiZGndGylLe43212532 = -12909125;    float NIADpiZGndGylLe49265169 = -100760395;    float NIADpiZGndGylLe75365969 = -308943021;    float NIADpiZGndGylLe57081783 = -344614540;    float NIADpiZGndGylLe24303425 = -138742727;    float NIADpiZGndGylLe50190576 = -963880949;    float NIADpiZGndGylLe85547600 = -344432909;    float NIADpiZGndGylLe72434239 = -986972557;    float NIADpiZGndGylLe86759775 = 12040444;    float NIADpiZGndGylLe34575856 = -772644335;    float NIADpiZGndGylLe31452675 = -50974380;    float NIADpiZGndGylLe18149902 = -700726399;    float NIADpiZGndGylLe2264332 = 58872825;    float NIADpiZGndGylLe19016886 = 47391797;    float NIADpiZGndGylLe53989502 = -39081543;    float NIADpiZGndGylLe50728304 = -137625022;    float NIADpiZGndGylLe85048083 = -759181634;    float NIADpiZGndGylLe17882570 = -494951726;    float NIADpiZGndGylLe11121220 = -244779436;    float NIADpiZGndGylLe37789542 = -469636534;    float NIADpiZGndGylLe22349705 = -898810674;    float NIADpiZGndGylLe52035767 = -753249325;    float NIADpiZGndGylLe2330225 = -654524187;    float NIADpiZGndGylLe10238312 = -645414511;    float NIADpiZGndGylLe38237736 = -919944939;    float NIADpiZGndGylLe49933612 = -948361340;    float NIADpiZGndGylLe32326898 = -104923942;    float NIADpiZGndGylLe15200832 = -314247655;    float NIADpiZGndGylLe74982205 = -54404446;    float NIADpiZGndGylLe22587103 = -667678215;    float NIADpiZGndGylLe49442472 = -159341353;    float NIADpiZGndGylLe31524173 = -758656444;    float NIADpiZGndGylLe93274310 = -755143661;    float NIADpiZGndGylLe21093365 = -698345075;    float NIADpiZGndGylLe88979303 = -627198160;    float NIADpiZGndGylLe46823357 = -942090122;    float NIADpiZGndGylLe5580770 = -156931499;    float NIADpiZGndGylLe61260790 = -22389371;    float NIADpiZGndGylLe64955630 = -194333174;    float NIADpiZGndGylLe71510833 = 14559791;    float NIADpiZGndGylLe38962620 = -149463333;    float NIADpiZGndGylLe71689113 = -15003248;    float NIADpiZGndGylLe47134059 = -885159616;    float NIADpiZGndGylLe82601970 = -679950837;    float NIADpiZGndGylLe41371819 = -873197701;    float NIADpiZGndGylLe14611126 = -200501070;    float NIADpiZGndGylLe62920022 = -513888602;    float NIADpiZGndGylLe92860944 = -480807625;    float NIADpiZGndGylLe66746641 = -964474465;    float NIADpiZGndGylLe23443651 = -172256276;    float NIADpiZGndGylLe27070844 = -486044007;    float NIADpiZGndGylLe12523949 = -688440409;    float NIADpiZGndGylLe40794700 = -842645328;    float NIADpiZGndGylLe90830599 = -458209016;    float NIADpiZGndGylLe71329289 = -645312838;    float NIADpiZGndGylLe91176765 = -259659800;    float NIADpiZGndGylLe46934945 = -446236209;    float NIADpiZGndGylLe65127658 = -663528511;    float NIADpiZGndGylLe18844047 = -424669601;    float NIADpiZGndGylLe74369813 = -190381387;    float NIADpiZGndGylLe17863678 = -758957007;    float NIADpiZGndGylLe70346769 = 69814745;    float NIADpiZGndGylLe97452034 = -832568112;    float NIADpiZGndGylLe64172673 = -320281342;    float NIADpiZGndGylLe85133384 = -513302982;    float NIADpiZGndGylLe99928502 = -292317937;    float NIADpiZGndGylLe24875591 = -945582738;    float NIADpiZGndGylLe81170967 = -242782100;    float NIADpiZGndGylLe30037583 = -325410043;    float NIADpiZGndGylLe7166145 = -96991422;    float NIADpiZGndGylLe45147535 = -980693524;    float NIADpiZGndGylLe23787294 = -636792263;    float NIADpiZGndGylLe52926940 = -200618553;    float NIADpiZGndGylLe39610387 = -159339228;    float NIADpiZGndGylLe98826922 = -442733487;     NIADpiZGndGylLe79848990 = NIADpiZGndGylLe83440092;     NIADpiZGndGylLe83440092 = NIADpiZGndGylLe21894452;     NIADpiZGndGylLe21894452 = NIADpiZGndGylLe14772359;     NIADpiZGndGylLe14772359 = NIADpiZGndGylLe32191835;     NIADpiZGndGylLe32191835 = NIADpiZGndGylLe96088525;     NIADpiZGndGylLe96088525 = NIADpiZGndGylLe80946739;     NIADpiZGndGylLe80946739 = NIADpiZGndGylLe18342573;     NIADpiZGndGylLe18342573 = NIADpiZGndGylLe89259055;     NIADpiZGndGylLe89259055 = NIADpiZGndGylLe21701409;     NIADpiZGndGylLe21701409 = NIADpiZGndGylLe24510220;     NIADpiZGndGylLe24510220 = NIADpiZGndGylLe44123352;     NIADpiZGndGylLe44123352 = NIADpiZGndGylLe33893834;     NIADpiZGndGylLe33893834 = NIADpiZGndGylLe17177827;     NIADpiZGndGylLe17177827 = NIADpiZGndGylLe72824493;     NIADpiZGndGylLe72824493 = NIADpiZGndGylLe32761028;     NIADpiZGndGylLe32761028 = NIADpiZGndGylLe65184354;     NIADpiZGndGylLe65184354 = NIADpiZGndGylLe11877831;     NIADpiZGndGylLe11877831 = NIADpiZGndGylLe20736143;     NIADpiZGndGylLe20736143 = NIADpiZGndGylLe74171955;     NIADpiZGndGylLe74171955 = NIADpiZGndGylLe12118928;     NIADpiZGndGylLe12118928 = NIADpiZGndGylLe30406519;     NIADpiZGndGylLe30406519 = NIADpiZGndGylLe51915920;     NIADpiZGndGylLe51915920 = NIADpiZGndGylLe28620142;     NIADpiZGndGylLe28620142 = NIADpiZGndGylLe93678993;     NIADpiZGndGylLe93678993 = NIADpiZGndGylLe43212532;     NIADpiZGndGylLe43212532 = NIADpiZGndGylLe49265169;     NIADpiZGndGylLe49265169 = NIADpiZGndGylLe75365969;     NIADpiZGndGylLe75365969 = NIADpiZGndGylLe57081783;     NIADpiZGndGylLe57081783 = NIADpiZGndGylLe24303425;     NIADpiZGndGylLe24303425 = NIADpiZGndGylLe50190576;     NIADpiZGndGylLe50190576 = NIADpiZGndGylLe85547600;     NIADpiZGndGylLe85547600 = NIADpiZGndGylLe72434239;     NIADpiZGndGylLe72434239 = NIADpiZGndGylLe86759775;     NIADpiZGndGylLe86759775 = NIADpiZGndGylLe34575856;     NIADpiZGndGylLe34575856 = NIADpiZGndGylLe31452675;     NIADpiZGndGylLe31452675 = NIADpiZGndGylLe18149902;     NIADpiZGndGylLe18149902 = NIADpiZGndGylLe2264332;     NIADpiZGndGylLe2264332 = NIADpiZGndGylLe19016886;     NIADpiZGndGylLe19016886 = NIADpiZGndGylLe53989502;     NIADpiZGndGylLe53989502 = NIADpiZGndGylLe50728304;     NIADpiZGndGylLe50728304 = NIADpiZGndGylLe85048083;     NIADpiZGndGylLe85048083 = NIADpiZGndGylLe17882570;     NIADpiZGndGylLe17882570 = NIADpiZGndGylLe11121220;     NIADpiZGndGylLe11121220 = NIADpiZGndGylLe37789542;     NIADpiZGndGylLe37789542 = NIADpiZGndGylLe22349705;     NIADpiZGndGylLe22349705 = NIADpiZGndGylLe52035767;     NIADpiZGndGylLe52035767 = NIADpiZGndGylLe2330225;     NIADpiZGndGylLe2330225 = NIADpiZGndGylLe10238312;     NIADpiZGndGylLe10238312 = NIADpiZGndGylLe38237736;     NIADpiZGndGylLe38237736 = NIADpiZGndGylLe49933612;     NIADpiZGndGylLe49933612 = NIADpiZGndGylLe32326898;     NIADpiZGndGylLe32326898 = NIADpiZGndGylLe15200832;     NIADpiZGndGylLe15200832 = NIADpiZGndGylLe74982205;     NIADpiZGndGylLe74982205 = NIADpiZGndGylLe22587103;     NIADpiZGndGylLe22587103 = NIADpiZGndGylLe49442472;     NIADpiZGndGylLe49442472 = NIADpiZGndGylLe31524173;     NIADpiZGndGylLe31524173 = NIADpiZGndGylLe93274310;     NIADpiZGndGylLe93274310 = NIADpiZGndGylLe21093365;     NIADpiZGndGylLe21093365 = NIADpiZGndGylLe88979303;     NIADpiZGndGylLe88979303 = NIADpiZGndGylLe46823357;     NIADpiZGndGylLe46823357 = NIADpiZGndGylLe5580770;     NIADpiZGndGylLe5580770 = NIADpiZGndGylLe61260790;     NIADpiZGndGylLe61260790 = NIADpiZGndGylLe64955630;     NIADpiZGndGylLe64955630 = NIADpiZGndGylLe71510833;     NIADpiZGndGylLe71510833 = NIADpiZGndGylLe38962620;     NIADpiZGndGylLe38962620 = NIADpiZGndGylLe71689113;     NIADpiZGndGylLe71689113 = NIADpiZGndGylLe47134059;     NIADpiZGndGylLe47134059 = NIADpiZGndGylLe82601970;     NIADpiZGndGylLe82601970 = NIADpiZGndGylLe41371819;     NIADpiZGndGylLe41371819 = NIADpiZGndGylLe14611126;     NIADpiZGndGylLe14611126 = NIADpiZGndGylLe62920022;     NIADpiZGndGylLe62920022 = NIADpiZGndGylLe92860944;     NIADpiZGndGylLe92860944 = NIADpiZGndGylLe66746641;     NIADpiZGndGylLe66746641 = NIADpiZGndGylLe23443651;     NIADpiZGndGylLe23443651 = NIADpiZGndGylLe27070844;     NIADpiZGndGylLe27070844 = NIADpiZGndGylLe12523949;     NIADpiZGndGylLe12523949 = NIADpiZGndGylLe40794700;     NIADpiZGndGylLe40794700 = NIADpiZGndGylLe90830599;     NIADpiZGndGylLe90830599 = NIADpiZGndGylLe71329289;     NIADpiZGndGylLe71329289 = NIADpiZGndGylLe91176765;     NIADpiZGndGylLe91176765 = NIADpiZGndGylLe46934945;     NIADpiZGndGylLe46934945 = NIADpiZGndGylLe65127658;     NIADpiZGndGylLe65127658 = NIADpiZGndGylLe18844047;     NIADpiZGndGylLe18844047 = NIADpiZGndGylLe74369813;     NIADpiZGndGylLe74369813 = NIADpiZGndGylLe17863678;     NIADpiZGndGylLe17863678 = NIADpiZGndGylLe70346769;     NIADpiZGndGylLe70346769 = NIADpiZGndGylLe97452034;     NIADpiZGndGylLe97452034 = NIADpiZGndGylLe64172673;     NIADpiZGndGylLe64172673 = NIADpiZGndGylLe85133384;     NIADpiZGndGylLe85133384 = NIADpiZGndGylLe99928502;     NIADpiZGndGylLe99928502 = NIADpiZGndGylLe24875591;     NIADpiZGndGylLe24875591 = NIADpiZGndGylLe81170967;     NIADpiZGndGylLe81170967 = NIADpiZGndGylLe30037583;     NIADpiZGndGylLe30037583 = NIADpiZGndGylLe7166145;     NIADpiZGndGylLe7166145 = NIADpiZGndGylLe45147535;     NIADpiZGndGylLe45147535 = NIADpiZGndGylLe23787294;     NIADpiZGndGylLe23787294 = NIADpiZGndGylLe52926940;     NIADpiZGndGylLe52926940 = NIADpiZGndGylLe39610387;     NIADpiZGndGylLe39610387 = NIADpiZGndGylLe98826922;     NIADpiZGndGylLe98826922 = NIADpiZGndGylLe79848990;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void yvbrdzWGiiYdRxVq10718150() {     float fcbcLwqRAyZCDjx66770668 = -583384692;    float fcbcLwqRAyZCDjx93087830 = 43332220;    float fcbcLwqRAyZCDjx14590467 = -214104593;    float fcbcLwqRAyZCDjx42635553 = -248265877;    float fcbcLwqRAyZCDjx82920922 = -593920762;    float fcbcLwqRAyZCDjx9554203 = -440543811;    float fcbcLwqRAyZCDjx30992408 = -247672558;    float fcbcLwqRAyZCDjx1058921 = -997543731;    float fcbcLwqRAyZCDjx67644745 = -642586602;    float fcbcLwqRAyZCDjx95201794 = -764182363;    float fcbcLwqRAyZCDjx44190950 = -747984985;    float fcbcLwqRAyZCDjx13749830 = -124078689;    float fcbcLwqRAyZCDjx94520283 = 38517539;    float fcbcLwqRAyZCDjx60495935 = -249093571;    float fcbcLwqRAyZCDjx96537363 = -110937630;    float fcbcLwqRAyZCDjx68040186 = -979879054;    float fcbcLwqRAyZCDjx15024530 = -124935613;    float fcbcLwqRAyZCDjx41094588 = -208802072;    float fcbcLwqRAyZCDjx81895714 = -440076979;    float fcbcLwqRAyZCDjx30924783 = -847979137;    float fcbcLwqRAyZCDjx79584595 = -289904634;    float fcbcLwqRAyZCDjx69096478 = -11514037;    float fcbcLwqRAyZCDjx21562260 = 15168986;    float fcbcLwqRAyZCDjx4977092 = -961934325;    float fcbcLwqRAyZCDjx49643979 = -568391291;    float fcbcLwqRAyZCDjx37391432 = -676432470;    float fcbcLwqRAyZCDjx6155597 = -352862460;    float fcbcLwqRAyZCDjx75952519 = -536592288;    float fcbcLwqRAyZCDjx29984576 = -760847171;    float fcbcLwqRAyZCDjx4017064 = -799962796;    float fcbcLwqRAyZCDjx9310828 = -880309849;    float fcbcLwqRAyZCDjx89989630 = -515354611;    float fcbcLwqRAyZCDjx26707322 = -470664290;    float fcbcLwqRAyZCDjx16682874 = -124074325;    float fcbcLwqRAyZCDjx27354805 = -44614432;    float fcbcLwqRAyZCDjx64412889 = -231570197;    float fcbcLwqRAyZCDjx74119165 = -668631688;    float fcbcLwqRAyZCDjx74592709 = -921359744;    float fcbcLwqRAyZCDjx72248052 = -315459375;    float fcbcLwqRAyZCDjx81776576 = -319474713;    float fcbcLwqRAyZCDjx33889735 = -959547673;    float fcbcLwqRAyZCDjx69889009 = -16337116;    float fcbcLwqRAyZCDjx94332743 = -288431778;    float fcbcLwqRAyZCDjx29665899 = -294775119;    float fcbcLwqRAyZCDjx89774402 = -925528252;    float fcbcLwqRAyZCDjx24916627 = -854160808;    float fcbcLwqRAyZCDjx72685793 = -228953840;    float fcbcLwqRAyZCDjx4586168 = -853293940;    float fcbcLwqRAyZCDjx463940 = -495158190;    float fcbcLwqRAyZCDjx30837316 = -157963269;    float fcbcLwqRAyZCDjx91291474 = 92283933;    float fcbcLwqRAyZCDjx37239940 = -704236626;    float fcbcLwqRAyZCDjx342435 = -122167339;    float fcbcLwqRAyZCDjx29527175 = -670496606;    float fcbcLwqRAyZCDjx31133554 = -218791462;    float fcbcLwqRAyZCDjx97674189 = -471870656;    float fcbcLwqRAyZCDjx71525571 = -971836767;    float fcbcLwqRAyZCDjx9613375 = -252170268;    float fcbcLwqRAyZCDjx92991573 = -679874586;    float fcbcLwqRAyZCDjx45529490 = -917488292;    float fcbcLwqRAyZCDjx3398607 = 12318649;    float fcbcLwqRAyZCDjx55039888 = -711080270;    float fcbcLwqRAyZCDjx71074345 = -136696561;    float fcbcLwqRAyZCDjx63627682 = -842623806;    float fcbcLwqRAyZCDjx85890966 = -883872515;    float fcbcLwqRAyZCDjx54201320 = -132630375;    float fcbcLwqRAyZCDjx87042507 = -653414399;    float fcbcLwqRAyZCDjx77837409 = -837408136;    float fcbcLwqRAyZCDjx33141131 = -104479140;    float fcbcLwqRAyZCDjx32124474 = -879367434;    float fcbcLwqRAyZCDjx93921021 = -211247367;    float fcbcLwqRAyZCDjx40431821 = -203575869;    float fcbcLwqRAyZCDjx68846536 = -893342698;    float fcbcLwqRAyZCDjx119138 = -20602267;    float fcbcLwqRAyZCDjx97035047 = -888431465;    float fcbcLwqRAyZCDjx9695587 = -173567518;    float fcbcLwqRAyZCDjx74763734 = -723082259;    float fcbcLwqRAyZCDjx91896360 = -690055895;    float fcbcLwqRAyZCDjx15202690 = 63593927;    float fcbcLwqRAyZCDjx24727353 = -714230483;    float fcbcLwqRAyZCDjx64705638 = -347478630;    float fcbcLwqRAyZCDjx1569430 = -499568520;    float fcbcLwqRAyZCDjx75488580 = 58565901;    float fcbcLwqRAyZCDjx99147260 = -502883903;    float fcbcLwqRAyZCDjx12725589 = -792246730;    float fcbcLwqRAyZCDjx72070888 = -76073223;    float fcbcLwqRAyZCDjx89647195 = -293187272;    float fcbcLwqRAyZCDjx97180147 = -800167685;    float fcbcLwqRAyZCDjx85549320 = -905282864;    float fcbcLwqRAyZCDjx29680615 = -572743776;    float fcbcLwqRAyZCDjx92887318 = -259733430;    float fcbcLwqRAyZCDjx64505791 = -316461420;    float fcbcLwqRAyZCDjx81601135 = -141485159;    float fcbcLwqRAyZCDjx26718562 = -397971083;    float fcbcLwqRAyZCDjx78377970 = -231793362;    float fcbcLwqRAyZCDjx78849847 = -148467403;    float fcbcLwqRAyZCDjx98814663 = -879640556;    float fcbcLwqRAyZCDjx30705062 = -445807973;    float fcbcLwqRAyZCDjx43774933 = -410902604;    float fcbcLwqRAyZCDjx35573082 = -583384692;     fcbcLwqRAyZCDjx66770668 = fcbcLwqRAyZCDjx93087830;     fcbcLwqRAyZCDjx93087830 = fcbcLwqRAyZCDjx14590467;     fcbcLwqRAyZCDjx14590467 = fcbcLwqRAyZCDjx42635553;     fcbcLwqRAyZCDjx42635553 = fcbcLwqRAyZCDjx82920922;     fcbcLwqRAyZCDjx82920922 = fcbcLwqRAyZCDjx9554203;     fcbcLwqRAyZCDjx9554203 = fcbcLwqRAyZCDjx30992408;     fcbcLwqRAyZCDjx30992408 = fcbcLwqRAyZCDjx1058921;     fcbcLwqRAyZCDjx1058921 = fcbcLwqRAyZCDjx67644745;     fcbcLwqRAyZCDjx67644745 = fcbcLwqRAyZCDjx95201794;     fcbcLwqRAyZCDjx95201794 = fcbcLwqRAyZCDjx44190950;     fcbcLwqRAyZCDjx44190950 = fcbcLwqRAyZCDjx13749830;     fcbcLwqRAyZCDjx13749830 = fcbcLwqRAyZCDjx94520283;     fcbcLwqRAyZCDjx94520283 = fcbcLwqRAyZCDjx60495935;     fcbcLwqRAyZCDjx60495935 = fcbcLwqRAyZCDjx96537363;     fcbcLwqRAyZCDjx96537363 = fcbcLwqRAyZCDjx68040186;     fcbcLwqRAyZCDjx68040186 = fcbcLwqRAyZCDjx15024530;     fcbcLwqRAyZCDjx15024530 = fcbcLwqRAyZCDjx41094588;     fcbcLwqRAyZCDjx41094588 = fcbcLwqRAyZCDjx81895714;     fcbcLwqRAyZCDjx81895714 = fcbcLwqRAyZCDjx30924783;     fcbcLwqRAyZCDjx30924783 = fcbcLwqRAyZCDjx79584595;     fcbcLwqRAyZCDjx79584595 = fcbcLwqRAyZCDjx69096478;     fcbcLwqRAyZCDjx69096478 = fcbcLwqRAyZCDjx21562260;     fcbcLwqRAyZCDjx21562260 = fcbcLwqRAyZCDjx4977092;     fcbcLwqRAyZCDjx4977092 = fcbcLwqRAyZCDjx49643979;     fcbcLwqRAyZCDjx49643979 = fcbcLwqRAyZCDjx37391432;     fcbcLwqRAyZCDjx37391432 = fcbcLwqRAyZCDjx6155597;     fcbcLwqRAyZCDjx6155597 = fcbcLwqRAyZCDjx75952519;     fcbcLwqRAyZCDjx75952519 = fcbcLwqRAyZCDjx29984576;     fcbcLwqRAyZCDjx29984576 = fcbcLwqRAyZCDjx4017064;     fcbcLwqRAyZCDjx4017064 = fcbcLwqRAyZCDjx9310828;     fcbcLwqRAyZCDjx9310828 = fcbcLwqRAyZCDjx89989630;     fcbcLwqRAyZCDjx89989630 = fcbcLwqRAyZCDjx26707322;     fcbcLwqRAyZCDjx26707322 = fcbcLwqRAyZCDjx16682874;     fcbcLwqRAyZCDjx16682874 = fcbcLwqRAyZCDjx27354805;     fcbcLwqRAyZCDjx27354805 = fcbcLwqRAyZCDjx64412889;     fcbcLwqRAyZCDjx64412889 = fcbcLwqRAyZCDjx74119165;     fcbcLwqRAyZCDjx74119165 = fcbcLwqRAyZCDjx74592709;     fcbcLwqRAyZCDjx74592709 = fcbcLwqRAyZCDjx72248052;     fcbcLwqRAyZCDjx72248052 = fcbcLwqRAyZCDjx81776576;     fcbcLwqRAyZCDjx81776576 = fcbcLwqRAyZCDjx33889735;     fcbcLwqRAyZCDjx33889735 = fcbcLwqRAyZCDjx69889009;     fcbcLwqRAyZCDjx69889009 = fcbcLwqRAyZCDjx94332743;     fcbcLwqRAyZCDjx94332743 = fcbcLwqRAyZCDjx29665899;     fcbcLwqRAyZCDjx29665899 = fcbcLwqRAyZCDjx89774402;     fcbcLwqRAyZCDjx89774402 = fcbcLwqRAyZCDjx24916627;     fcbcLwqRAyZCDjx24916627 = fcbcLwqRAyZCDjx72685793;     fcbcLwqRAyZCDjx72685793 = fcbcLwqRAyZCDjx4586168;     fcbcLwqRAyZCDjx4586168 = fcbcLwqRAyZCDjx463940;     fcbcLwqRAyZCDjx463940 = fcbcLwqRAyZCDjx30837316;     fcbcLwqRAyZCDjx30837316 = fcbcLwqRAyZCDjx91291474;     fcbcLwqRAyZCDjx91291474 = fcbcLwqRAyZCDjx37239940;     fcbcLwqRAyZCDjx37239940 = fcbcLwqRAyZCDjx342435;     fcbcLwqRAyZCDjx342435 = fcbcLwqRAyZCDjx29527175;     fcbcLwqRAyZCDjx29527175 = fcbcLwqRAyZCDjx31133554;     fcbcLwqRAyZCDjx31133554 = fcbcLwqRAyZCDjx97674189;     fcbcLwqRAyZCDjx97674189 = fcbcLwqRAyZCDjx71525571;     fcbcLwqRAyZCDjx71525571 = fcbcLwqRAyZCDjx9613375;     fcbcLwqRAyZCDjx9613375 = fcbcLwqRAyZCDjx92991573;     fcbcLwqRAyZCDjx92991573 = fcbcLwqRAyZCDjx45529490;     fcbcLwqRAyZCDjx45529490 = fcbcLwqRAyZCDjx3398607;     fcbcLwqRAyZCDjx3398607 = fcbcLwqRAyZCDjx55039888;     fcbcLwqRAyZCDjx55039888 = fcbcLwqRAyZCDjx71074345;     fcbcLwqRAyZCDjx71074345 = fcbcLwqRAyZCDjx63627682;     fcbcLwqRAyZCDjx63627682 = fcbcLwqRAyZCDjx85890966;     fcbcLwqRAyZCDjx85890966 = fcbcLwqRAyZCDjx54201320;     fcbcLwqRAyZCDjx54201320 = fcbcLwqRAyZCDjx87042507;     fcbcLwqRAyZCDjx87042507 = fcbcLwqRAyZCDjx77837409;     fcbcLwqRAyZCDjx77837409 = fcbcLwqRAyZCDjx33141131;     fcbcLwqRAyZCDjx33141131 = fcbcLwqRAyZCDjx32124474;     fcbcLwqRAyZCDjx32124474 = fcbcLwqRAyZCDjx93921021;     fcbcLwqRAyZCDjx93921021 = fcbcLwqRAyZCDjx40431821;     fcbcLwqRAyZCDjx40431821 = fcbcLwqRAyZCDjx68846536;     fcbcLwqRAyZCDjx68846536 = fcbcLwqRAyZCDjx119138;     fcbcLwqRAyZCDjx119138 = fcbcLwqRAyZCDjx97035047;     fcbcLwqRAyZCDjx97035047 = fcbcLwqRAyZCDjx9695587;     fcbcLwqRAyZCDjx9695587 = fcbcLwqRAyZCDjx74763734;     fcbcLwqRAyZCDjx74763734 = fcbcLwqRAyZCDjx91896360;     fcbcLwqRAyZCDjx91896360 = fcbcLwqRAyZCDjx15202690;     fcbcLwqRAyZCDjx15202690 = fcbcLwqRAyZCDjx24727353;     fcbcLwqRAyZCDjx24727353 = fcbcLwqRAyZCDjx64705638;     fcbcLwqRAyZCDjx64705638 = fcbcLwqRAyZCDjx1569430;     fcbcLwqRAyZCDjx1569430 = fcbcLwqRAyZCDjx75488580;     fcbcLwqRAyZCDjx75488580 = fcbcLwqRAyZCDjx99147260;     fcbcLwqRAyZCDjx99147260 = fcbcLwqRAyZCDjx12725589;     fcbcLwqRAyZCDjx12725589 = fcbcLwqRAyZCDjx72070888;     fcbcLwqRAyZCDjx72070888 = fcbcLwqRAyZCDjx89647195;     fcbcLwqRAyZCDjx89647195 = fcbcLwqRAyZCDjx97180147;     fcbcLwqRAyZCDjx97180147 = fcbcLwqRAyZCDjx85549320;     fcbcLwqRAyZCDjx85549320 = fcbcLwqRAyZCDjx29680615;     fcbcLwqRAyZCDjx29680615 = fcbcLwqRAyZCDjx92887318;     fcbcLwqRAyZCDjx92887318 = fcbcLwqRAyZCDjx64505791;     fcbcLwqRAyZCDjx64505791 = fcbcLwqRAyZCDjx81601135;     fcbcLwqRAyZCDjx81601135 = fcbcLwqRAyZCDjx26718562;     fcbcLwqRAyZCDjx26718562 = fcbcLwqRAyZCDjx78377970;     fcbcLwqRAyZCDjx78377970 = fcbcLwqRAyZCDjx78849847;     fcbcLwqRAyZCDjx78849847 = fcbcLwqRAyZCDjx98814663;     fcbcLwqRAyZCDjx98814663 = fcbcLwqRAyZCDjx30705062;     fcbcLwqRAyZCDjx30705062 = fcbcLwqRAyZCDjx43774933;     fcbcLwqRAyZCDjx43774933 = fcbcLwqRAyZCDjx35573082;     fcbcLwqRAyZCDjx35573082 = fcbcLwqRAyZCDjx66770668;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void dADqbSQNqdVAzLOL47693321() {     float ibAhidavhFKViNJ34496916 = -752785213;    float ibAhidavhFKViNJ69010028 = -468241146;    float ibAhidavhFKViNJ83940310 = -25502437;    float ibAhidavhFKViNJ71287160 = -69111828;    float ibAhidavhFKViNJ15483608 = -880897845;    float ibAhidavhFKViNJ45068355 = -971434164;    float ibAhidavhFKViNJ70594778 = -572303658;    float ibAhidavhFKViNJ16731727 = -671749460;    float ibAhidavhFKViNJ59773630 = -193575340;    float ibAhidavhFKViNJ972908 = -964929298;    float ibAhidavhFKViNJ23442120 = -78724886;    float ibAhidavhFKViNJ21179743 = 76121744;    float ibAhidavhFKViNJ29564063 = -798317951;    float ibAhidavhFKViNJ63988477 = -409851801;    float ibAhidavhFKViNJ48220676 = -894006564;    float ibAhidavhFKViNJ97093110 = -964182683;    float ibAhidavhFKViNJ20868878 = -848230174;    float ibAhidavhFKViNJ2474951 = -994051917;    float ibAhidavhFKViNJ42385014 = -907349823;    float ibAhidavhFKViNJ63880274 = -267271895;    float ibAhidavhFKViNJ25962739 = -523775165;    float ibAhidavhFKViNJ59955373 = -859828125;    float ibAhidavhFKViNJ25264148 = -33126027;    float ibAhidavhFKViNJ73698618 = -607806888;    float ibAhidavhFKViNJ17874300 = -243730817;    float ibAhidavhFKViNJ1027557 = 75176162;    float ibAhidavhFKViNJ21476675 = -956404387;    float ibAhidavhFKViNJ64018494 = -925961870;    float ibAhidavhFKViNJ9657907 = -270929730;    float ibAhidavhFKViNJ94828703 = -104932069;    float ibAhidavhFKViNJ35308783 = -799846709;    float ibAhidavhFKViNJ1263068 = -47751199;    float ibAhidavhFKViNJ25542539 = -645434387;    float ibAhidavhFKViNJ81698505 = -644762098;    float ibAhidavhFKViNJ51438051 = -149821519;    float ibAhidavhFKViNJ61607406 = -378735723;    float ibAhidavhFKViNJ4795758 = -650806397;    float ibAhidavhFKViNJ85159250 = -910318875;    float ibAhidavhFKViNJ5100132 = 52761779;    float ibAhidavhFKViNJ49848789 = -406821829;    float ibAhidavhFKViNJ94543372 = -338119079;    float ibAhidavhFKViNJ7420403 = -460505802;    float ibAhidavhFKViNJ69166113 = -894587298;    float ibAhidavhFKViNJ79347517 = -375423546;    float ibAhidavhFKViNJ81791658 = 24586466;    float ibAhidavhFKViNJ18111275 = -779644959;    float ibAhidavhFKViNJ96207271 = -161587331;    float ibAhidavhFKViNJ1141115 = -140937915;    float ibAhidavhFKViNJ71239467 = -108908561;    float ibAhidavhFKViNJ3124822 = -746989120;    float ibAhidavhFKViNJ38306521 = -184131850;    float ibAhidavhFKViNJ36256305 = 97864327;    float ibAhidavhFKViNJ22377183 = 53782880;    float ibAhidavhFKViNJ42648234 = -482210782;    float ibAhidavhFKViNJ21730583 = -820486173;    float ibAhidavhFKViNJ74541543 = -892957088;    float ibAhidavhFKViNJ43745880 = -335115120;    float ibAhidavhFKViNJ10241693 = -417695549;    float ibAhidavhFKViNJ53412861 = -825381011;    float ibAhidavhFKViNJ14456052 = -856074008;    float ibAhidavhFKViNJ23591681 = 84970222;    float ibAhidavhFKViNJ6576285 = -646341789;    float ibAhidavhFKViNJ7073820 = -300819731;    float ibAhidavhFKViNJ64944926 = 11356728;    float ibAhidavhFKViNJ65664124 = -65082589;    float ibAhidavhFKViNJ22179052 = 69026312;    float ibAhidavhFKViNJ95637203 = -278443869;    float ibAhidavhFKViNJ47865557 = -53555853;    float ibAhidavhFKViNJ12550427 = -160030282;    float ibAhidavhFKViNJ86613270 = -415270842;    float ibAhidavhFKViNJ92297352 = -213376286;    float ibAhidavhFKViNJ35709627 = -937911300;    float ibAhidavhFKViNJ97374818 = -946813697;    float ibAhidavhFKViNJ92536224 = -400527995;    float ibAhidavhFKViNJ69336901 = -929152817;    float ibAhidavhFKViNJ18542337 = 36730637;    float ibAhidavhFKViNJ90789260 = -965240828;    float ibAhidavhFKViNJ45916631 = -657702482;    float ibAhidavhFKViNJ91906960 = -532393355;    float ibAhidavhFKViNJ99763024 = -464085858;    float ibAhidavhFKViNJ4820285 = -763236507;    float ibAhidavhFKViNJ20335561 = -715466472;    float ibAhidavhFKViNJ92779027 = -717053309;    float ibAhidavhFKViNJ6533085 = -523940611;    float ibAhidavhFKViNJ56522182 = -920800220;    float ibAhidavhFKViNJ99052478 = -797711037;    float ibAhidavhFKViNJ78885885 = -1534079;    float ibAhidavhFKViNJ82894305 = -63223606;    float ibAhidavhFKViNJ59967923 = -824275925;    float ibAhidavhFKViNJ76896507 = -256864432;    float ibAhidavhFKViNJ17861526 = 56379397;    float ibAhidavhFKViNJ94554065 = -133110848;    float ibAhidavhFKViNJ31746390 = 15062136;    float ibAhidavhFKViNJ90644079 = -91164213;    float ibAhidavhFKViNJ26257109 = -391792051;    float ibAhidavhFKViNJ87967088 = -691777290;    float ibAhidavhFKViNJ346583 = -59686072;    float ibAhidavhFKViNJ4221188 = -805944026;    float ibAhidavhFKViNJ13683393 = -210340957;    float ibAhidavhFKViNJ59612606 = -752785213;     ibAhidavhFKViNJ34496916 = ibAhidavhFKViNJ69010028;     ibAhidavhFKViNJ69010028 = ibAhidavhFKViNJ83940310;     ibAhidavhFKViNJ83940310 = ibAhidavhFKViNJ71287160;     ibAhidavhFKViNJ71287160 = ibAhidavhFKViNJ15483608;     ibAhidavhFKViNJ15483608 = ibAhidavhFKViNJ45068355;     ibAhidavhFKViNJ45068355 = ibAhidavhFKViNJ70594778;     ibAhidavhFKViNJ70594778 = ibAhidavhFKViNJ16731727;     ibAhidavhFKViNJ16731727 = ibAhidavhFKViNJ59773630;     ibAhidavhFKViNJ59773630 = ibAhidavhFKViNJ972908;     ibAhidavhFKViNJ972908 = ibAhidavhFKViNJ23442120;     ibAhidavhFKViNJ23442120 = ibAhidavhFKViNJ21179743;     ibAhidavhFKViNJ21179743 = ibAhidavhFKViNJ29564063;     ibAhidavhFKViNJ29564063 = ibAhidavhFKViNJ63988477;     ibAhidavhFKViNJ63988477 = ibAhidavhFKViNJ48220676;     ibAhidavhFKViNJ48220676 = ibAhidavhFKViNJ97093110;     ibAhidavhFKViNJ97093110 = ibAhidavhFKViNJ20868878;     ibAhidavhFKViNJ20868878 = ibAhidavhFKViNJ2474951;     ibAhidavhFKViNJ2474951 = ibAhidavhFKViNJ42385014;     ibAhidavhFKViNJ42385014 = ibAhidavhFKViNJ63880274;     ibAhidavhFKViNJ63880274 = ibAhidavhFKViNJ25962739;     ibAhidavhFKViNJ25962739 = ibAhidavhFKViNJ59955373;     ibAhidavhFKViNJ59955373 = ibAhidavhFKViNJ25264148;     ibAhidavhFKViNJ25264148 = ibAhidavhFKViNJ73698618;     ibAhidavhFKViNJ73698618 = ibAhidavhFKViNJ17874300;     ibAhidavhFKViNJ17874300 = ibAhidavhFKViNJ1027557;     ibAhidavhFKViNJ1027557 = ibAhidavhFKViNJ21476675;     ibAhidavhFKViNJ21476675 = ibAhidavhFKViNJ64018494;     ibAhidavhFKViNJ64018494 = ibAhidavhFKViNJ9657907;     ibAhidavhFKViNJ9657907 = ibAhidavhFKViNJ94828703;     ibAhidavhFKViNJ94828703 = ibAhidavhFKViNJ35308783;     ibAhidavhFKViNJ35308783 = ibAhidavhFKViNJ1263068;     ibAhidavhFKViNJ1263068 = ibAhidavhFKViNJ25542539;     ibAhidavhFKViNJ25542539 = ibAhidavhFKViNJ81698505;     ibAhidavhFKViNJ81698505 = ibAhidavhFKViNJ51438051;     ibAhidavhFKViNJ51438051 = ibAhidavhFKViNJ61607406;     ibAhidavhFKViNJ61607406 = ibAhidavhFKViNJ4795758;     ibAhidavhFKViNJ4795758 = ibAhidavhFKViNJ85159250;     ibAhidavhFKViNJ85159250 = ibAhidavhFKViNJ5100132;     ibAhidavhFKViNJ5100132 = ibAhidavhFKViNJ49848789;     ibAhidavhFKViNJ49848789 = ibAhidavhFKViNJ94543372;     ibAhidavhFKViNJ94543372 = ibAhidavhFKViNJ7420403;     ibAhidavhFKViNJ7420403 = ibAhidavhFKViNJ69166113;     ibAhidavhFKViNJ69166113 = ibAhidavhFKViNJ79347517;     ibAhidavhFKViNJ79347517 = ibAhidavhFKViNJ81791658;     ibAhidavhFKViNJ81791658 = ibAhidavhFKViNJ18111275;     ibAhidavhFKViNJ18111275 = ibAhidavhFKViNJ96207271;     ibAhidavhFKViNJ96207271 = ibAhidavhFKViNJ1141115;     ibAhidavhFKViNJ1141115 = ibAhidavhFKViNJ71239467;     ibAhidavhFKViNJ71239467 = ibAhidavhFKViNJ3124822;     ibAhidavhFKViNJ3124822 = ibAhidavhFKViNJ38306521;     ibAhidavhFKViNJ38306521 = ibAhidavhFKViNJ36256305;     ibAhidavhFKViNJ36256305 = ibAhidavhFKViNJ22377183;     ibAhidavhFKViNJ22377183 = ibAhidavhFKViNJ42648234;     ibAhidavhFKViNJ42648234 = ibAhidavhFKViNJ21730583;     ibAhidavhFKViNJ21730583 = ibAhidavhFKViNJ74541543;     ibAhidavhFKViNJ74541543 = ibAhidavhFKViNJ43745880;     ibAhidavhFKViNJ43745880 = ibAhidavhFKViNJ10241693;     ibAhidavhFKViNJ10241693 = ibAhidavhFKViNJ53412861;     ibAhidavhFKViNJ53412861 = ibAhidavhFKViNJ14456052;     ibAhidavhFKViNJ14456052 = ibAhidavhFKViNJ23591681;     ibAhidavhFKViNJ23591681 = ibAhidavhFKViNJ6576285;     ibAhidavhFKViNJ6576285 = ibAhidavhFKViNJ7073820;     ibAhidavhFKViNJ7073820 = ibAhidavhFKViNJ64944926;     ibAhidavhFKViNJ64944926 = ibAhidavhFKViNJ65664124;     ibAhidavhFKViNJ65664124 = ibAhidavhFKViNJ22179052;     ibAhidavhFKViNJ22179052 = ibAhidavhFKViNJ95637203;     ibAhidavhFKViNJ95637203 = ibAhidavhFKViNJ47865557;     ibAhidavhFKViNJ47865557 = ibAhidavhFKViNJ12550427;     ibAhidavhFKViNJ12550427 = ibAhidavhFKViNJ86613270;     ibAhidavhFKViNJ86613270 = ibAhidavhFKViNJ92297352;     ibAhidavhFKViNJ92297352 = ibAhidavhFKViNJ35709627;     ibAhidavhFKViNJ35709627 = ibAhidavhFKViNJ97374818;     ibAhidavhFKViNJ97374818 = ibAhidavhFKViNJ92536224;     ibAhidavhFKViNJ92536224 = ibAhidavhFKViNJ69336901;     ibAhidavhFKViNJ69336901 = ibAhidavhFKViNJ18542337;     ibAhidavhFKViNJ18542337 = ibAhidavhFKViNJ90789260;     ibAhidavhFKViNJ90789260 = ibAhidavhFKViNJ45916631;     ibAhidavhFKViNJ45916631 = ibAhidavhFKViNJ91906960;     ibAhidavhFKViNJ91906960 = ibAhidavhFKViNJ99763024;     ibAhidavhFKViNJ99763024 = ibAhidavhFKViNJ4820285;     ibAhidavhFKViNJ4820285 = ibAhidavhFKViNJ20335561;     ibAhidavhFKViNJ20335561 = ibAhidavhFKViNJ92779027;     ibAhidavhFKViNJ92779027 = ibAhidavhFKViNJ6533085;     ibAhidavhFKViNJ6533085 = ibAhidavhFKViNJ56522182;     ibAhidavhFKViNJ56522182 = ibAhidavhFKViNJ99052478;     ibAhidavhFKViNJ99052478 = ibAhidavhFKViNJ78885885;     ibAhidavhFKViNJ78885885 = ibAhidavhFKViNJ82894305;     ibAhidavhFKViNJ82894305 = ibAhidavhFKViNJ59967923;     ibAhidavhFKViNJ59967923 = ibAhidavhFKViNJ76896507;     ibAhidavhFKViNJ76896507 = ibAhidavhFKViNJ17861526;     ibAhidavhFKViNJ17861526 = ibAhidavhFKViNJ94554065;     ibAhidavhFKViNJ94554065 = ibAhidavhFKViNJ31746390;     ibAhidavhFKViNJ31746390 = ibAhidavhFKViNJ90644079;     ibAhidavhFKViNJ90644079 = ibAhidavhFKViNJ26257109;     ibAhidavhFKViNJ26257109 = ibAhidavhFKViNJ87967088;     ibAhidavhFKViNJ87967088 = ibAhidavhFKViNJ346583;     ibAhidavhFKViNJ346583 = ibAhidavhFKViNJ4221188;     ibAhidavhFKViNJ4221188 = ibAhidavhFKViNJ13683393;     ibAhidavhFKViNJ13683393 = ibAhidavhFKViNJ59612606;     ibAhidavhFKViNJ59612606 = ibAhidavhFKViNJ34496916;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void vsrExjwGqyaQClTQ2109535() {     float tsImkWlcrKywDGE5148584 = -270934343;    float tsImkWlcrKywDGE37683546 = -339884445;    float tsImkWlcrKywDGE63909229 = -467796897;    float tsImkWlcrKywDGE11218447 = -259250217;    float tsImkWlcrKywDGE36933930 = -664304193;    float tsImkWlcrKywDGE35068118 = -673015316;    float tsImkWlcrKywDGE88973674 = -571079368;    float tsImkWlcrKywDGE73574107 = 81433450;    float tsImkWlcrKywDGE5638685 = -286922669;    float tsImkWlcrKywDGE23939368 = -166024648;    float tsImkWlcrKywDGE65691281 = -113626815;    float tsImkWlcrKywDGE66726314 = -239642049;    float tsImkWlcrKywDGE9391451 = -707855009;    float tsImkWlcrKywDGE47835511 = -210880011;    float tsImkWlcrKywDGE30940696 = 56243433;    float tsImkWlcrKywDGE29621751 = -534166270;    float tsImkWlcrKywDGE14721704 = -757415304;    float tsImkWlcrKywDGE22768445 = -983218113;    float tsImkWlcrKywDGE24092796 = -514537765;    float tsImkWlcrKywDGE22666983 = -809638200;    float tsImkWlcrKywDGE81098493 = -432454305;    float tsImkWlcrKywDGE21351138 = -91113431;    float tsImkWlcrKywDGE27207957 = -600969171;    float tsImkWlcrKywDGE7548156 = -200207795;    float tsImkWlcrKywDGE81998688 = 80364496;    float tsImkWlcrKywDGE31625137 = -23108282;    float tsImkWlcrKywDGE29737461 = -251582162;    float tsImkWlcrKywDGE71800720 = -331085637;    float tsImkWlcrKywDGE41800394 = -565287947;    float tsImkWlcrKywDGE34000800 = -663327139;    float tsImkWlcrKywDGE42380780 = -548280395;    float tsImkWlcrKywDGE45774529 = -376048362;    float tsImkWlcrKywDGE8487900 = -592395996;    float tsImkWlcrKywDGE47063481 = -208291783;    float tsImkWlcrKywDGE29462287 = 3231701;    float tsImkWlcrKywDGE21600665 = -457962271;    float tsImkWlcrKywDGE48587244 = -839399605;    float tsImkWlcrKywDGE37951739 = -694431982;    float tsImkWlcrKywDGE83633420 = 97033142;    float tsImkWlcrKywDGE44952088 = 75569809;    float tsImkWlcrKywDGE54616657 = -722137957;    float tsImkWlcrKywDGE35903394 = -229894872;    float tsImkWlcrKywDGE18133073 = -881341350;    float tsImkWlcrKywDGE85855818 = -447398517;    float tsImkWlcrKywDGE77745716 = -284059928;    float tsImkWlcrKywDGE24517110 = -537982114;    float tsImkWlcrKywDGE22338475 = -277657131;    float tsImkWlcrKywDGE66755324 = -593068210;    float tsImkWlcrKywDGE76680919 = -94672530;    float tsImkWlcrKywDGE11552308 = 70002002;    float tsImkWlcrKywDGE6495846 = -830703971;    float tsImkWlcrKywDGE78127809 = -971355395;    float tsImkWlcrKywDGE77452839 = -974015162;    float tsImkWlcrKywDGE32875538 = -43092278;    float tsImkWlcrKywDGE21011041 = -931427088;    float tsImkWlcrKywDGE83797445 = -79820912;    float tsImkWlcrKywDGE10475589 = -738915274;    float tsImkWlcrKywDGE56361074 = -167589103;    float tsImkWlcrKywDGE29219759 = -239614713;    float tsImkWlcrKywDGE5308793 = -541195911;    float tsImkWlcrKywDGE5330658 = -321433154;    float tsImkWlcrKywDGE17172955 = -139993732;    float tsImkWlcrKywDGE31773713 = -353278604;    float tsImkWlcrKywDGE71637884 = -623595530;    float tsImkWlcrKywDGE81558588 = -617744254;    float tsImkWlcrKywDGE19916753 = -737578454;    float tsImkWlcrKywDGE58238414 = -647246053;    float tsImkWlcrKywDGE62327969 = -399563227;    float tsImkWlcrKywDGE18373224 = -114111713;    float tsImkWlcrKywDGE9340031 = -485794297;    float tsImkWlcrKywDGE81034507 = -694766665;    float tsImkWlcrKywDGE76769965 = 37016677;    float tsImkWlcrKywDGE39135024 = -980251256;    float tsImkWlcrKywDGE79140708 = -490107574;    float tsImkWlcrKywDGE68050326 = 12499757;    float tsImkWlcrKywDGE45195100 = -102559434;    float tsImkWlcrKywDGE3218065 = -209772081;    float tsImkWlcrKywDGE41352139 = -53570655;    float tsImkWlcrKywDGE29802439 = -916147867;    float tsImkWlcrKywDGE57481578 = -381653391;    float tsImkWlcrKywDGE9286663 = -745451152;    float tsImkWlcrKywDGE62982136 = -658513952;    float tsImkWlcrKywDGE95119800 = -136413108;    float tsImkWlcrKywDGE30248087 = -535289949;    float tsImkWlcrKywDGE27504954 = -832623169;    float tsImkWlcrKywDGE64252970 = -576925000;    float tsImkWlcrKywDGE68321689 = -402033200;    float tsImkWlcrKywDGE75612362 = -449303718;    float tsImkWlcrKywDGE26052440 = -276864695;    float tsImkWlcrKywDGE45664842 = -916947387;    float tsImkWlcrKywDGE11125077 = -719046997;    float tsImkWlcrKywDGE92226169 = -571810502;    float tsImkWlcrKywDGE8731980 = -354817270;    float tsImkWlcrKywDGE78324627 = -361770947;    float tsImkWlcrKywDGE39621430 = -602997038;    float tsImkWlcrKywDGE37443702 = -482144226;    float tsImkWlcrKywDGE4129682 = -876616269;    float tsImkWlcrKywDGE46495189 = -157745820;    float tsImkWlcrKywDGE4297230 = -829654264;    float tsImkWlcrKywDGE57828964 = -270934343;     tsImkWlcrKywDGE5148584 = tsImkWlcrKywDGE37683546;     tsImkWlcrKywDGE37683546 = tsImkWlcrKywDGE63909229;     tsImkWlcrKywDGE63909229 = tsImkWlcrKywDGE11218447;     tsImkWlcrKywDGE11218447 = tsImkWlcrKywDGE36933930;     tsImkWlcrKywDGE36933930 = tsImkWlcrKywDGE35068118;     tsImkWlcrKywDGE35068118 = tsImkWlcrKywDGE88973674;     tsImkWlcrKywDGE88973674 = tsImkWlcrKywDGE73574107;     tsImkWlcrKywDGE73574107 = tsImkWlcrKywDGE5638685;     tsImkWlcrKywDGE5638685 = tsImkWlcrKywDGE23939368;     tsImkWlcrKywDGE23939368 = tsImkWlcrKywDGE65691281;     tsImkWlcrKywDGE65691281 = tsImkWlcrKywDGE66726314;     tsImkWlcrKywDGE66726314 = tsImkWlcrKywDGE9391451;     tsImkWlcrKywDGE9391451 = tsImkWlcrKywDGE47835511;     tsImkWlcrKywDGE47835511 = tsImkWlcrKywDGE30940696;     tsImkWlcrKywDGE30940696 = tsImkWlcrKywDGE29621751;     tsImkWlcrKywDGE29621751 = tsImkWlcrKywDGE14721704;     tsImkWlcrKywDGE14721704 = tsImkWlcrKywDGE22768445;     tsImkWlcrKywDGE22768445 = tsImkWlcrKywDGE24092796;     tsImkWlcrKywDGE24092796 = tsImkWlcrKywDGE22666983;     tsImkWlcrKywDGE22666983 = tsImkWlcrKywDGE81098493;     tsImkWlcrKywDGE81098493 = tsImkWlcrKywDGE21351138;     tsImkWlcrKywDGE21351138 = tsImkWlcrKywDGE27207957;     tsImkWlcrKywDGE27207957 = tsImkWlcrKywDGE7548156;     tsImkWlcrKywDGE7548156 = tsImkWlcrKywDGE81998688;     tsImkWlcrKywDGE81998688 = tsImkWlcrKywDGE31625137;     tsImkWlcrKywDGE31625137 = tsImkWlcrKywDGE29737461;     tsImkWlcrKywDGE29737461 = tsImkWlcrKywDGE71800720;     tsImkWlcrKywDGE71800720 = tsImkWlcrKywDGE41800394;     tsImkWlcrKywDGE41800394 = tsImkWlcrKywDGE34000800;     tsImkWlcrKywDGE34000800 = tsImkWlcrKywDGE42380780;     tsImkWlcrKywDGE42380780 = tsImkWlcrKywDGE45774529;     tsImkWlcrKywDGE45774529 = tsImkWlcrKywDGE8487900;     tsImkWlcrKywDGE8487900 = tsImkWlcrKywDGE47063481;     tsImkWlcrKywDGE47063481 = tsImkWlcrKywDGE29462287;     tsImkWlcrKywDGE29462287 = tsImkWlcrKywDGE21600665;     tsImkWlcrKywDGE21600665 = tsImkWlcrKywDGE48587244;     tsImkWlcrKywDGE48587244 = tsImkWlcrKywDGE37951739;     tsImkWlcrKywDGE37951739 = tsImkWlcrKywDGE83633420;     tsImkWlcrKywDGE83633420 = tsImkWlcrKywDGE44952088;     tsImkWlcrKywDGE44952088 = tsImkWlcrKywDGE54616657;     tsImkWlcrKywDGE54616657 = tsImkWlcrKywDGE35903394;     tsImkWlcrKywDGE35903394 = tsImkWlcrKywDGE18133073;     tsImkWlcrKywDGE18133073 = tsImkWlcrKywDGE85855818;     tsImkWlcrKywDGE85855818 = tsImkWlcrKywDGE77745716;     tsImkWlcrKywDGE77745716 = tsImkWlcrKywDGE24517110;     tsImkWlcrKywDGE24517110 = tsImkWlcrKywDGE22338475;     tsImkWlcrKywDGE22338475 = tsImkWlcrKywDGE66755324;     tsImkWlcrKywDGE66755324 = tsImkWlcrKywDGE76680919;     tsImkWlcrKywDGE76680919 = tsImkWlcrKywDGE11552308;     tsImkWlcrKywDGE11552308 = tsImkWlcrKywDGE6495846;     tsImkWlcrKywDGE6495846 = tsImkWlcrKywDGE78127809;     tsImkWlcrKywDGE78127809 = tsImkWlcrKywDGE77452839;     tsImkWlcrKywDGE77452839 = tsImkWlcrKywDGE32875538;     tsImkWlcrKywDGE32875538 = tsImkWlcrKywDGE21011041;     tsImkWlcrKywDGE21011041 = tsImkWlcrKywDGE83797445;     tsImkWlcrKywDGE83797445 = tsImkWlcrKywDGE10475589;     tsImkWlcrKywDGE10475589 = tsImkWlcrKywDGE56361074;     tsImkWlcrKywDGE56361074 = tsImkWlcrKywDGE29219759;     tsImkWlcrKywDGE29219759 = tsImkWlcrKywDGE5308793;     tsImkWlcrKywDGE5308793 = tsImkWlcrKywDGE5330658;     tsImkWlcrKywDGE5330658 = tsImkWlcrKywDGE17172955;     tsImkWlcrKywDGE17172955 = tsImkWlcrKywDGE31773713;     tsImkWlcrKywDGE31773713 = tsImkWlcrKywDGE71637884;     tsImkWlcrKywDGE71637884 = tsImkWlcrKywDGE81558588;     tsImkWlcrKywDGE81558588 = tsImkWlcrKywDGE19916753;     tsImkWlcrKywDGE19916753 = tsImkWlcrKywDGE58238414;     tsImkWlcrKywDGE58238414 = tsImkWlcrKywDGE62327969;     tsImkWlcrKywDGE62327969 = tsImkWlcrKywDGE18373224;     tsImkWlcrKywDGE18373224 = tsImkWlcrKywDGE9340031;     tsImkWlcrKywDGE9340031 = tsImkWlcrKywDGE81034507;     tsImkWlcrKywDGE81034507 = tsImkWlcrKywDGE76769965;     tsImkWlcrKywDGE76769965 = tsImkWlcrKywDGE39135024;     tsImkWlcrKywDGE39135024 = tsImkWlcrKywDGE79140708;     tsImkWlcrKywDGE79140708 = tsImkWlcrKywDGE68050326;     tsImkWlcrKywDGE68050326 = tsImkWlcrKywDGE45195100;     tsImkWlcrKywDGE45195100 = tsImkWlcrKywDGE3218065;     tsImkWlcrKywDGE3218065 = tsImkWlcrKywDGE41352139;     tsImkWlcrKywDGE41352139 = tsImkWlcrKywDGE29802439;     tsImkWlcrKywDGE29802439 = tsImkWlcrKywDGE57481578;     tsImkWlcrKywDGE57481578 = tsImkWlcrKywDGE9286663;     tsImkWlcrKywDGE9286663 = tsImkWlcrKywDGE62982136;     tsImkWlcrKywDGE62982136 = tsImkWlcrKywDGE95119800;     tsImkWlcrKywDGE95119800 = tsImkWlcrKywDGE30248087;     tsImkWlcrKywDGE30248087 = tsImkWlcrKywDGE27504954;     tsImkWlcrKywDGE27504954 = tsImkWlcrKywDGE64252970;     tsImkWlcrKywDGE64252970 = tsImkWlcrKywDGE68321689;     tsImkWlcrKywDGE68321689 = tsImkWlcrKywDGE75612362;     tsImkWlcrKywDGE75612362 = tsImkWlcrKywDGE26052440;     tsImkWlcrKywDGE26052440 = tsImkWlcrKywDGE45664842;     tsImkWlcrKywDGE45664842 = tsImkWlcrKywDGE11125077;     tsImkWlcrKywDGE11125077 = tsImkWlcrKywDGE92226169;     tsImkWlcrKywDGE92226169 = tsImkWlcrKywDGE8731980;     tsImkWlcrKywDGE8731980 = tsImkWlcrKywDGE78324627;     tsImkWlcrKywDGE78324627 = tsImkWlcrKywDGE39621430;     tsImkWlcrKywDGE39621430 = tsImkWlcrKywDGE37443702;     tsImkWlcrKywDGE37443702 = tsImkWlcrKywDGE4129682;     tsImkWlcrKywDGE4129682 = tsImkWlcrKywDGE46495189;     tsImkWlcrKywDGE46495189 = tsImkWlcrKywDGE4297230;     tsImkWlcrKywDGE4297230 = tsImkWlcrKywDGE57828964;     tsImkWlcrKywDGE57828964 = tsImkWlcrKywDGE5148584;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void QbemsTTLXhKojMVD8768280() {     float EYxfUyUjlhShwsV5142112 = -143458484;    float EYxfUyUjlhShwsV76255862 = -382479570;    float EYxfUyUjlhShwsV65222442 = -424925511;    float EYxfUyUjlhShwsV96298307 = -79791047;    float EYxfUyUjlhShwsV45774032 = -32659514;    float EYxfUyUjlhShwsV69873550 = -372448128;    float EYxfUyUjlhShwsV57521010 = -275615835;    float EYxfUyUjlhShwsV42788158 = -722743867;    float EYxfUyUjlhShwsV88378848 = -568739;    float EYxfUyUjlhShwsV20578882 = -902831519;    float EYxfUyUjlhShwsV86011886 = -73098887;    float EYxfUyUjlhShwsV81017991 = 24879588;    float EYxfUyUjlhShwsV41244364 = -729513484;    float EYxfUyUjlhShwsV1679732 = -556033063;    float EYxfUyUjlhShwsV76112804 = 93530581;    float EYxfUyUjlhShwsV4186298 = -866961920;    float EYxfUyUjlhShwsV70574463 = -577029874;    float EYxfUyUjlhShwsV45768977 = -280289735;    float EYxfUyUjlhShwsV44521065 = -32520031;    float EYxfUyUjlhShwsV94740745 = -321662651;    float EYxfUyUjlhShwsV60767918 = -142920679;    float EYxfUyUjlhShwsV99647403 = -601105314;    float EYxfUyUjlhShwsV64086353 = 40072987;    float EYxfUyUjlhShwsV51198264 = -81128317;    float EYxfUyUjlhShwsV10441378 = -346329358;    float EYxfUyUjlhShwsV48199214 = 68685789;    float EYxfUyUjlhShwsV99959042 = -552381875;    float EYxfUyUjlhShwsV7204245 = -940052626;    float EYxfUyUjlhShwsV54478841 = -111358262;    float EYxfUyUjlhShwsV10090670 = -338758513;    float EYxfUyUjlhShwsV75793458 = -935373629;    float EYxfUyUjlhShwsV30498386 = -767870123;    float EYxfUyUjlhShwsV93940323 = -794340212;    float EYxfUyUjlhShwsV47346318 = -84973515;    float EYxfUyUjlhShwsV11820325 = -256082224;    float EYxfUyUjlhShwsV64428855 = -537728017;    float EYxfUyUjlhShwsV60528611 = -939052983;    float EYxfUyUjlhShwsV7869419 = -689694661;    float EYxfUyUjlhShwsV18947017 = -615648274;    float EYxfUyUjlhShwsV16824981 = -633861877;    float EYxfUyUjlhShwsV42472324 = -810081856;    float EYxfUyUjlhShwsV21601055 = -423686954;    float EYxfUyUjlhShwsV78416433 = -340471604;    float EYxfUyUjlhShwsV45087716 = -4362960;    float EYxfUyUjlhShwsV61763769 = -757319330;    float EYxfUyUjlhShwsV64945078 = -350026785;    float EYxfUyUjlhShwsV50036267 = -758937753;    float EYxfUyUjlhShwsV42138905 = -101829567;    float EYxfUyUjlhShwsV95339309 = -758436391;    float EYxfUyUjlhShwsV31597731 = -617022885;    float EYxfUyUjlhShwsV19755216 = -409258978;    float EYxfUyUjlhShwsV92675066 = -253501143;    float EYxfUyUjlhShwsV52901188 = -866069170;    float EYxfUyUjlhShwsV43125809 = -575012129;    float EYxfUyUjlhShwsV48000361 = -688326365;    float EYxfUyUjlhShwsV5494709 = -542353171;    float EYxfUyUjlhShwsV12169509 = -322552557;    float EYxfUyUjlhShwsV14024178 = -243797194;    float EYxfUyUjlhShwsV85856930 = -733461690;    float EYxfUyUjlhShwsV97574817 = -1345304;    float EYxfUyUjlhShwsV69914508 = -820066253;    float EYxfUyUjlhShwsV50316765 = -335563210;    float EYxfUyUjlhShwsV88309316 = -511385606;    float EYxfUyUjlhShwsV78288178 = -661810226;    float EYxfUyUjlhShwsV44785424 = -967457891;    float EYxfUyUjlhShwsV55513500 = -305228765;    float EYxfUyUjlhShwsV87077667 = -180780200;    float EYxfUyUjlhShwsV93898046 = -544539969;    float EYxfUyUjlhShwsV89859406 = -199950840;    float EYxfUyUjlhShwsV11683950 = -368741403;    float EYxfUyUjlhShwsV43657687 = -927908938;    float EYxfUyUjlhShwsV62705045 = -887335214;    float EYxfUyUjlhShwsV26821961 = -664641461;    float EYxfUyUjlhShwsV27696085 = -398658154;    float EYxfUyUjlhShwsV52268421 = -511580796;    float EYxfUyUjlhShwsV39166863 = -719233725;    float EYxfUyUjlhShwsV21230971 = -160633710;    float EYxfUyUjlhShwsV18998637 = -955564053;    float EYxfUyUjlhShwsV89434494 = -323808988;    float EYxfUyUjlhShwsV45496299 = -996302573;    float EYxfUyUjlhShwsV98162946 = -172376458;    float EYxfUyUjlhShwsV57820137 = -350552309;    float EYxfUyUjlhShwsV11864936 = -81616235;    float EYxfUyUjlhShwsV22881111 = -494335378;    float EYxfUyUjlhShwsV90335454 = -929499535;    float EYxfUyUjlhShwsV83118391 = -581872487;    float EYxfUyUjlhShwsV77597198 = -901800953;    float EYxfUyUjlhShwsV50814514 = -119328083;    float EYxfUyUjlhShwsV99345956 = -396647150;    float EYxfUyUjlhShwsV6325617 = -713729053;    float EYxfUyUjlhShwsV52259346 = -115175460;    float EYxfUyUjlhShwsV46504433 = -595255789;    float EYxfUyUjlhShwsV22012489 = -956232972;    float EYxfUyUjlhShwsV21372199 = -514302970;    float EYxfUyUjlhShwsV46910472 = -813795625;    float EYxfUyUjlhShwsV92155559 = -374518646;    float EYxfUyUjlhShwsV33291739 = -912301349;    float EYxfUyUjlhShwsV128255 = -678661378;    float EYxfUyUjlhShwsV302293 = -36905070;    float EYxfUyUjlhShwsV6250269 = -143458484;     EYxfUyUjlhShwsV5142112 = EYxfUyUjlhShwsV76255862;     EYxfUyUjlhShwsV76255862 = EYxfUyUjlhShwsV65222442;     EYxfUyUjlhShwsV65222442 = EYxfUyUjlhShwsV96298307;     EYxfUyUjlhShwsV96298307 = EYxfUyUjlhShwsV45774032;     EYxfUyUjlhShwsV45774032 = EYxfUyUjlhShwsV69873550;     EYxfUyUjlhShwsV69873550 = EYxfUyUjlhShwsV57521010;     EYxfUyUjlhShwsV57521010 = EYxfUyUjlhShwsV42788158;     EYxfUyUjlhShwsV42788158 = EYxfUyUjlhShwsV88378848;     EYxfUyUjlhShwsV88378848 = EYxfUyUjlhShwsV20578882;     EYxfUyUjlhShwsV20578882 = EYxfUyUjlhShwsV86011886;     EYxfUyUjlhShwsV86011886 = EYxfUyUjlhShwsV81017991;     EYxfUyUjlhShwsV81017991 = EYxfUyUjlhShwsV41244364;     EYxfUyUjlhShwsV41244364 = EYxfUyUjlhShwsV1679732;     EYxfUyUjlhShwsV1679732 = EYxfUyUjlhShwsV76112804;     EYxfUyUjlhShwsV76112804 = EYxfUyUjlhShwsV4186298;     EYxfUyUjlhShwsV4186298 = EYxfUyUjlhShwsV70574463;     EYxfUyUjlhShwsV70574463 = EYxfUyUjlhShwsV45768977;     EYxfUyUjlhShwsV45768977 = EYxfUyUjlhShwsV44521065;     EYxfUyUjlhShwsV44521065 = EYxfUyUjlhShwsV94740745;     EYxfUyUjlhShwsV94740745 = EYxfUyUjlhShwsV60767918;     EYxfUyUjlhShwsV60767918 = EYxfUyUjlhShwsV99647403;     EYxfUyUjlhShwsV99647403 = EYxfUyUjlhShwsV64086353;     EYxfUyUjlhShwsV64086353 = EYxfUyUjlhShwsV51198264;     EYxfUyUjlhShwsV51198264 = EYxfUyUjlhShwsV10441378;     EYxfUyUjlhShwsV10441378 = EYxfUyUjlhShwsV48199214;     EYxfUyUjlhShwsV48199214 = EYxfUyUjlhShwsV99959042;     EYxfUyUjlhShwsV99959042 = EYxfUyUjlhShwsV7204245;     EYxfUyUjlhShwsV7204245 = EYxfUyUjlhShwsV54478841;     EYxfUyUjlhShwsV54478841 = EYxfUyUjlhShwsV10090670;     EYxfUyUjlhShwsV10090670 = EYxfUyUjlhShwsV75793458;     EYxfUyUjlhShwsV75793458 = EYxfUyUjlhShwsV30498386;     EYxfUyUjlhShwsV30498386 = EYxfUyUjlhShwsV93940323;     EYxfUyUjlhShwsV93940323 = EYxfUyUjlhShwsV47346318;     EYxfUyUjlhShwsV47346318 = EYxfUyUjlhShwsV11820325;     EYxfUyUjlhShwsV11820325 = EYxfUyUjlhShwsV64428855;     EYxfUyUjlhShwsV64428855 = EYxfUyUjlhShwsV60528611;     EYxfUyUjlhShwsV60528611 = EYxfUyUjlhShwsV7869419;     EYxfUyUjlhShwsV7869419 = EYxfUyUjlhShwsV18947017;     EYxfUyUjlhShwsV18947017 = EYxfUyUjlhShwsV16824981;     EYxfUyUjlhShwsV16824981 = EYxfUyUjlhShwsV42472324;     EYxfUyUjlhShwsV42472324 = EYxfUyUjlhShwsV21601055;     EYxfUyUjlhShwsV21601055 = EYxfUyUjlhShwsV78416433;     EYxfUyUjlhShwsV78416433 = EYxfUyUjlhShwsV45087716;     EYxfUyUjlhShwsV45087716 = EYxfUyUjlhShwsV61763769;     EYxfUyUjlhShwsV61763769 = EYxfUyUjlhShwsV64945078;     EYxfUyUjlhShwsV64945078 = EYxfUyUjlhShwsV50036267;     EYxfUyUjlhShwsV50036267 = EYxfUyUjlhShwsV42138905;     EYxfUyUjlhShwsV42138905 = EYxfUyUjlhShwsV95339309;     EYxfUyUjlhShwsV95339309 = EYxfUyUjlhShwsV31597731;     EYxfUyUjlhShwsV31597731 = EYxfUyUjlhShwsV19755216;     EYxfUyUjlhShwsV19755216 = EYxfUyUjlhShwsV92675066;     EYxfUyUjlhShwsV92675066 = EYxfUyUjlhShwsV52901188;     EYxfUyUjlhShwsV52901188 = EYxfUyUjlhShwsV43125809;     EYxfUyUjlhShwsV43125809 = EYxfUyUjlhShwsV48000361;     EYxfUyUjlhShwsV48000361 = EYxfUyUjlhShwsV5494709;     EYxfUyUjlhShwsV5494709 = EYxfUyUjlhShwsV12169509;     EYxfUyUjlhShwsV12169509 = EYxfUyUjlhShwsV14024178;     EYxfUyUjlhShwsV14024178 = EYxfUyUjlhShwsV85856930;     EYxfUyUjlhShwsV85856930 = EYxfUyUjlhShwsV97574817;     EYxfUyUjlhShwsV97574817 = EYxfUyUjlhShwsV69914508;     EYxfUyUjlhShwsV69914508 = EYxfUyUjlhShwsV50316765;     EYxfUyUjlhShwsV50316765 = EYxfUyUjlhShwsV88309316;     EYxfUyUjlhShwsV88309316 = EYxfUyUjlhShwsV78288178;     EYxfUyUjlhShwsV78288178 = EYxfUyUjlhShwsV44785424;     EYxfUyUjlhShwsV44785424 = EYxfUyUjlhShwsV55513500;     EYxfUyUjlhShwsV55513500 = EYxfUyUjlhShwsV87077667;     EYxfUyUjlhShwsV87077667 = EYxfUyUjlhShwsV93898046;     EYxfUyUjlhShwsV93898046 = EYxfUyUjlhShwsV89859406;     EYxfUyUjlhShwsV89859406 = EYxfUyUjlhShwsV11683950;     EYxfUyUjlhShwsV11683950 = EYxfUyUjlhShwsV43657687;     EYxfUyUjlhShwsV43657687 = EYxfUyUjlhShwsV62705045;     EYxfUyUjlhShwsV62705045 = EYxfUyUjlhShwsV26821961;     EYxfUyUjlhShwsV26821961 = EYxfUyUjlhShwsV27696085;     EYxfUyUjlhShwsV27696085 = EYxfUyUjlhShwsV52268421;     EYxfUyUjlhShwsV52268421 = EYxfUyUjlhShwsV39166863;     EYxfUyUjlhShwsV39166863 = EYxfUyUjlhShwsV21230971;     EYxfUyUjlhShwsV21230971 = EYxfUyUjlhShwsV18998637;     EYxfUyUjlhShwsV18998637 = EYxfUyUjlhShwsV89434494;     EYxfUyUjlhShwsV89434494 = EYxfUyUjlhShwsV45496299;     EYxfUyUjlhShwsV45496299 = EYxfUyUjlhShwsV98162946;     EYxfUyUjlhShwsV98162946 = EYxfUyUjlhShwsV57820137;     EYxfUyUjlhShwsV57820137 = EYxfUyUjlhShwsV11864936;     EYxfUyUjlhShwsV11864936 = EYxfUyUjlhShwsV22881111;     EYxfUyUjlhShwsV22881111 = EYxfUyUjlhShwsV90335454;     EYxfUyUjlhShwsV90335454 = EYxfUyUjlhShwsV83118391;     EYxfUyUjlhShwsV83118391 = EYxfUyUjlhShwsV77597198;     EYxfUyUjlhShwsV77597198 = EYxfUyUjlhShwsV50814514;     EYxfUyUjlhShwsV50814514 = EYxfUyUjlhShwsV99345956;     EYxfUyUjlhShwsV99345956 = EYxfUyUjlhShwsV6325617;     EYxfUyUjlhShwsV6325617 = EYxfUyUjlhShwsV52259346;     EYxfUyUjlhShwsV52259346 = EYxfUyUjlhShwsV46504433;     EYxfUyUjlhShwsV46504433 = EYxfUyUjlhShwsV22012489;     EYxfUyUjlhShwsV22012489 = EYxfUyUjlhShwsV21372199;     EYxfUyUjlhShwsV21372199 = EYxfUyUjlhShwsV46910472;     EYxfUyUjlhShwsV46910472 = EYxfUyUjlhShwsV92155559;     EYxfUyUjlhShwsV92155559 = EYxfUyUjlhShwsV33291739;     EYxfUyUjlhShwsV33291739 = EYxfUyUjlhShwsV128255;     EYxfUyUjlhShwsV128255 = EYxfUyUjlhShwsV302293;     EYxfUyUjlhShwsV302293 = EYxfUyUjlhShwsV6250269;     EYxfUyUjlhShwsV6250269 = EYxfUyUjlhShwsV5142112;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void eoCyQauCpFWvxvNq63184492() {     float lTAiFjpMybIptac75793779 = -761607615;    float lTAiFjpMybIptac44929380 = -254122870;    float lTAiFjpMybIptac45191361 = -867219971;    float lTAiFjpMybIptac36229594 = -269929436;    float lTAiFjpMybIptac67224353 = -916065861;    float lTAiFjpMybIptac59873313 = -74029279;    float lTAiFjpMybIptac75899906 = -274391545;    float lTAiFjpMybIptac99630538 = 30439043;    float lTAiFjpMybIptac34243903 = -93916067;    float lTAiFjpMybIptac43545343 = -103926870;    float lTAiFjpMybIptac28261047 = -108000816;    float lTAiFjpMybIptac26564562 = -290884205;    float lTAiFjpMybIptac21071751 = -639050542;    float lTAiFjpMybIptac85526764 = -357061273;    float lTAiFjpMybIptac58832824 = -56219422;    float lTAiFjpMybIptac36714938 = -436945507;    float lTAiFjpMybIptac64427290 = -486215004;    float lTAiFjpMybIptac66062472 = -269455931;    float lTAiFjpMybIptac26228847 = -739707973;    float lTAiFjpMybIptac53527455 = -864028956;    float lTAiFjpMybIptac15903673 = -51599819;    float lTAiFjpMybIptac61043168 = -932390620;    float lTAiFjpMybIptac66030162 = -527770158;    float lTAiFjpMybIptac85047800 = -773529223;    float lTAiFjpMybIptac74565766 = -22234045;    float lTAiFjpMybIptac78796794 = -29598656;    float lTAiFjpMybIptac8219828 = -947559650;    float lTAiFjpMybIptac14986471 = -345176393;    float lTAiFjpMybIptac86621329 = -405716479;    float lTAiFjpMybIptac49262765 = -897153584;    float lTAiFjpMybIptac82865454 = -683807315;    float lTAiFjpMybIptac75009847 = 3832714;    float lTAiFjpMybIptac76885684 = -741301821;    float lTAiFjpMybIptac12711293 = -748503199;    float lTAiFjpMybIptac89844561 = -103029003;    float lTAiFjpMybIptac24422114 = -616954565;    float lTAiFjpMybIptac4320098 = -27646191;    float lTAiFjpMybIptac60661907 = -473807769;    float lTAiFjpMybIptac97480305 = -571376910;    float lTAiFjpMybIptac11928279 = -151470240;    float lTAiFjpMybIptac2545609 = -94100734;    float lTAiFjpMybIptac50084046 = -193076024;    float lTAiFjpMybIptac27383393 = -327225656;    float lTAiFjpMybIptac51596017 = -76337932;    float lTAiFjpMybIptac57717828 = 34034276;    float lTAiFjpMybIptac71350913 = -108363940;    float lTAiFjpMybIptac76167470 = -875007552;    float lTAiFjpMybIptac7753116 = -553959862;    float lTAiFjpMybIptac780762 = -744200361;    float lTAiFjpMybIptac40025217 = -900031763;    float lTAiFjpMybIptac87944539 = 44168901;    float lTAiFjpMybIptac34546571 = -222720865;    float lTAiFjpMybIptac7976846 = -793867212;    float lTAiFjpMybIptac33353114 = -135893626;    float lTAiFjpMybIptac47280820 = -799267280;    float lTAiFjpMybIptac14750611 = -829216996;    float lTAiFjpMybIptac78899217 = -726352712;    float lTAiFjpMybIptac60143560 = 6309252;    float lTAiFjpMybIptac61663828 = -147695391;    float lTAiFjpMybIptac88427558 = -786467206;    float lTAiFjpMybIptac51653485 = -126469629;    float lTAiFjpMybIptac60913436 = -929215152;    float lTAiFjpMybIptac13009209 = -563844479;    float lTAiFjpMybIptac84981137 = -196762484;    float lTAiFjpMybIptac60679888 = -420119556;    float lTAiFjpMybIptac53251200 = -11833531;    float lTAiFjpMybIptac49678878 = -549582384;    float lTAiFjpMybIptac8360459 = -890547343;    float lTAiFjpMybIptac95682203 = -154032270;    float lTAiFjpMybIptac34410710 = -439264858;    float lTAiFjpMybIptac32394841 = -309299317;    float lTAiFjpMybIptac3765384 = 87592764;    float lTAiFjpMybIptac68582166 = -698079021;    float lTAiFjpMybIptac14300569 = -488237734;    float lTAiFjpMybIptac50981847 = -669928223;    float lTAiFjpMybIptac65819626 = -858523796;    float lTAiFjpMybIptac33659776 = -505164964;    float lTAiFjpMybIptac14434146 = -351432227;    float lTAiFjpMybIptac27329973 = -707563499;    float lTAiFjpMybIptac3214853 = -913870106;    float lTAiFjpMybIptac2629325 = -154591104;    float lTAiFjpMybIptac466713 = -293599789;    float lTAiFjpMybIptac14205709 = -600976033;    float lTAiFjpMybIptac46596112 = -505684716;    float lTAiFjpMybIptac61318226 = -841322485;    float lTAiFjpMybIptac48318884 = -361086450;    float lTAiFjpMybIptac67033002 = -202300074;    float lTAiFjpMybIptac43532571 = -505408195;    float lTAiFjpMybIptac65430472 = -949235920;    float lTAiFjpMybIptac75093951 = -273812008;    float lTAiFjpMybIptac45522896 = -890601853;    float lTAiFjpMybIptac44176537 = 66044557;    float lTAiFjpMybIptac98998078 = -226112378;    float lTAiFjpMybIptac9052748 = -784909705;    float lTAiFjpMybIptac60274794 = 74999389;    float lTAiFjpMybIptac41632173 = -164885582;    float lTAiFjpMybIptac37074837 = -629231545;    float lTAiFjpMybIptac42402256 = -30463172;    float lTAiFjpMybIptac90916129 = -656218377;    float lTAiFjpMybIptac4466628 = -761607615;     lTAiFjpMybIptac75793779 = lTAiFjpMybIptac44929380;     lTAiFjpMybIptac44929380 = lTAiFjpMybIptac45191361;     lTAiFjpMybIptac45191361 = lTAiFjpMybIptac36229594;     lTAiFjpMybIptac36229594 = lTAiFjpMybIptac67224353;     lTAiFjpMybIptac67224353 = lTAiFjpMybIptac59873313;     lTAiFjpMybIptac59873313 = lTAiFjpMybIptac75899906;     lTAiFjpMybIptac75899906 = lTAiFjpMybIptac99630538;     lTAiFjpMybIptac99630538 = lTAiFjpMybIptac34243903;     lTAiFjpMybIptac34243903 = lTAiFjpMybIptac43545343;     lTAiFjpMybIptac43545343 = lTAiFjpMybIptac28261047;     lTAiFjpMybIptac28261047 = lTAiFjpMybIptac26564562;     lTAiFjpMybIptac26564562 = lTAiFjpMybIptac21071751;     lTAiFjpMybIptac21071751 = lTAiFjpMybIptac85526764;     lTAiFjpMybIptac85526764 = lTAiFjpMybIptac58832824;     lTAiFjpMybIptac58832824 = lTAiFjpMybIptac36714938;     lTAiFjpMybIptac36714938 = lTAiFjpMybIptac64427290;     lTAiFjpMybIptac64427290 = lTAiFjpMybIptac66062472;     lTAiFjpMybIptac66062472 = lTAiFjpMybIptac26228847;     lTAiFjpMybIptac26228847 = lTAiFjpMybIptac53527455;     lTAiFjpMybIptac53527455 = lTAiFjpMybIptac15903673;     lTAiFjpMybIptac15903673 = lTAiFjpMybIptac61043168;     lTAiFjpMybIptac61043168 = lTAiFjpMybIptac66030162;     lTAiFjpMybIptac66030162 = lTAiFjpMybIptac85047800;     lTAiFjpMybIptac85047800 = lTAiFjpMybIptac74565766;     lTAiFjpMybIptac74565766 = lTAiFjpMybIptac78796794;     lTAiFjpMybIptac78796794 = lTAiFjpMybIptac8219828;     lTAiFjpMybIptac8219828 = lTAiFjpMybIptac14986471;     lTAiFjpMybIptac14986471 = lTAiFjpMybIptac86621329;     lTAiFjpMybIptac86621329 = lTAiFjpMybIptac49262765;     lTAiFjpMybIptac49262765 = lTAiFjpMybIptac82865454;     lTAiFjpMybIptac82865454 = lTAiFjpMybIptac75009847;     lTAiFjpMybIptac75009847 = lTAiFjpMybIptac76885684;     lTAiFjpMybIptac76885684 = lTAiFjpMybIptac12711293;     lTAiFjpMybIptac12711293 = lTAiFjpMybIptac89844561;     lTAiFjpMybIptac89844561 = lTAiFjpMybIptac24422114;     lTAiFjpMybIptac24422114 = lTAiFjpMybIptac4320098;     lTAiFjpMybIptac4320098 = lTAiFjpMybIptac60661907;     lTAiFjpMybIptac60661907 = lTAiFjpMybIptac97480305;     lTAiFjpMybIptac97480305 = lTAiFjpMybIptac11928279;     lTAiFjpMybIptac11928279 = lTAiFjpMybIptac2545609;     lTAiFjpMybIptac2545609 = lTAiFjpMybIptac50084046;     lTAiFjpMybIptac50084046 = lTAiFjpMybIptac27383393;     lTAiFjpMybIptac27383393 = lTAiFjpMybIptac51596017;     lTAiFjpMybIptac51596017 = lTAiFjpMybIptac57717828;     lTAiFjpMybIptac57717828 = lTAiFjpMybIptac71350913;     lTAiFjpMybIptac71350913 = lTAiFjpMybIptac76167470;     lTAiFjpMybIptac76167470 = lTAiFjpMybIptac7753116;     lTAiFjpMybIptac7753116 = lTAiFjpMybIptac780762;     lTAiFjpMybIptac780762 = lTAiFjpMybIptac40025217;     lTAiFjpMybIptac40025217 = lTAiFjpMybIptac87944539;     lTAiFjpMybIptac87944539 = lTAiFjpMybIptac34546571;     lTAiFjpMybIptac34546571 = lTAiFjpMybIptac7976846;     lTAiFjpMybIptac7976846 = lTAiFjpMybIptac33353114;     lTAiFjpMybIptac33353114 = lTAiFjpMybIptac47280820;     lTAiFjpMybIptac47280820 = lTAiFjpMybIptac14750611;     lTAiFjpMybIptac14750611 = lTAiFjpMybIptac78899217;     lTAiFjpMybIptac78899217 = lTAiFjpMybIptac60143560;     lTAiFjpMybIptac60143560 = lTAiFjpMybIptac61663828;     lTAiFjpMybIptac61663828 = lTAiFjpMybIptac88427558;     lTAiFjpMybIptac88427558 = lTAiFjpMybIptac51653485;     lTAiFjpMybIptac51653485 = lTAiFjpMybIptac60913436;     lTAiFjpMybIptac60913436 = lTAiFjpMybIptac13009209;     lTAiFjpMybIptac13009209 = lTAiFjpMybIptac84981137;     lTAiFjpMybIptac84981137 = lTAiFjpMybIptac60679888;     lTAiFjpMybIptac60679888 = lTAiFjpMybIptac53251200;     lTAiFjpMybIptac53251200 = lTAiFjpMybIptac49678878;     lTAiFjpMybIptac49678878 = lTAiFjpMybIptac8360459;     lTAiFjpMybIptac8360459 = lTAiFjpMybIptac95682203;     lTAiFjpMybIptac95682203 = lTAiFjpMybIptac34410710;     lTAiFjpMybIptac34410710 = lTAiFjpMybIptac32394841;     lTAiFjpMybIptac32394841 = lTAiFjpMybIptac3765384;     lTAiFjpMybIptac3765384 = lTAiFjpMybIptac68582166;     lTAiFjpMybIptac68582166 = lTAiFjpMybIptac14300569;     lTAiFjpMybIptac14300569 = lTAiFjpMybIptac50981847;     lTAiFjpMybIptac50981847 = lTAiFjpMybIptac65819626;     lTAiFjpMybIptac65819626 = lTAiFjpMybIptac33659776;     lTAiFjpMybIptac33659776 = lTAiFjpMybIptac14434146;     lTAiFjpMybIptac14434146 = lTAiFjpMybIptac27329973;     lTAiFjpMybIptac27329973 = lTAiFjpMybIptac3214853;     lTAiFjpMybIptac3214853 = lTAiFjpMybIptac2629325;     lTAiFjpMybIptac2629325 = lTAiFjpMybIptac466713;     lTAiFjpMybIptac466713 = lTAiFjpMybIptac14205709;     lTAiFjpMybIptac14205709 = lTAiFjpMybIptac46596112;     lTAiFjpMybIptac46596112 = lTAiFjpMybIptac61318226;     lTAiFjpMybIptac61318226 = lTAiFjpMybIptac48318884;     lTAiFjpMybIptac48318884 = lTAiFjpMybIptac67033002;     lTAiFjpMybIptac67033002 = lTAiFjpMybIptac43532571;     lTAiFjpMybIptac43532571 = lTAiFjpMybIptac65430472;     lTAiFjpMybIptac65430472 = lTAiFjpMybIptac75093951;     lTAiFjpMybIptac75093951 = lTAiFjpMybIptac45522896;     lTAiFjpMybIptac45522896 = lTAiFjpMybIptac44176537;     lTAiFjpMybIptac44176537 = lTAiFjpMybIptac98998078;     lTAiFjpMybIptac98998078 = lTAiFjpMybIptac9052748;     lTAiFjpMybIptac9052748 = lTAiFjpMybIptac60274794;     lTAiFjpMybIptac60274794 = lTAiFjpMybIptac41632173;     lTAiFjpMybIptac41632173 = lTAiFjpMybIptac37074837;     lTAiFjpMybIptac37074837 = lTAiFjpMybIptac42402256;     lTAiFjpMybIptac42402256 = lTAiFjpMybIptac90916129;     lTAiFjpMybIptac90916129 = lTAiFjpMybIptac4466628;     lTAiFjpMybIptac4466628 = lTAiFjpMybIptac75793779;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void SevyCuswvqSxcMNz69513039() {     float TQidgkrZgEEELhV30448177 = -99135199;    float TQidgkrZgEEELhV91926999 = -833687684;    float TQidgkrZgEEELhV5924008 = -152604584;    float TQidgkrZgEEELhV7664536 = -276031847;    float TQidgkrZgEEELhV41676024 = -588501101;    float TQidgkrZgEEELhV74047710 = -203180115;    float TQidgkrZgEEELhV97000609 = -576284218;    float TQidgkrZgEEELhV28805642 = -470129190;    float TQidgkrZgEEELhV22018313 = -140769438;    float TQidgkrZgEEELhV26177328 = -382728139;    float TQidgkrZgEEELhV6872342 = -733357389;    float TQidgkrZgEEELhV89329275 = -477308293;    float TQidgkrZgEEELhV84889066 = -442590846;    float TQidgkrZgEEELhV78493195 = 30835149;    float TQidgkrZgEEELhV89056897 = 36658946;    float TQidgkrZgEEELhV26482474 = -67105071;    float TQidgkrZgEEELhV64259054 = -959814833;    float TQidgkrZgEEELhV33659059 = -333020398;    float TQidgkrZgEEELhV27449448 = 74480479;    float TQidgkrZgEEELhV71162010 = -109395103;    float TQidgkrZgEEELhV50078060 = -619682970;    float TQidgkrZgEEELhV12295758 = 1165272;    float TQidgkrZgEEELhV2499994 = -14513579;    float TQidgkrZgEEELhV86476169 = -472570040;    float TQidgkrZgEEELhV70318382 = -395147497;    float TQidgkrZgEEELhV20037742 = -33307440;    float TQidgkrZgEEELhV10209753 = -402403929;    float TQidgkrZgEEELhV68235470 = -353228254;    float TQidgkrZgEEELhV26519006 = -785961354;    float TQidgkrZgEEELhV43698175 = -87911552;    float TQidgkrZgEEELhV34570984 = -132679841;    float TQidgkrZgEEELhV6001458 = -407663814;    float TQidgkrZgEEELhV44541561 = -197819435;    float TQidgkrZgEEELhV7367186 = -428624009;    float TQidgkrZgEEELhV24348718 = -320892262;    float TQidgkrZgEEELhV11748656 = -864950161;    float TQidgkrZgEEELhV79024585 = -978072811;    float TQidgkrZgEEELhV73639146 = -347736790;    float TQidgkrZgEEELhV48249955 = -953325512;    float TQidgkrZgEEELhV35914674 = -909778839;    float TQidgkrZgEEELhV58505009 = -206650892;    float TQidgkrZgEEELhV86758704 = -800608110;    float TQidgkrZgEEELhV18383576 = -167730974;    float TQidgkrZgEEELhV60590416 = -650017598;    float TQidgkrZgEEELhV17701891 = -98483322;    float TQidgkrZgEEELhV26684515 = -177153555;    float TQidgkrZgEEELhV92641182 = -902064936;    float TQidgkrZgEEELhV31180424 = -531612234;    float TQidgkrZgEEELhV43123528 = -643930549;    float TQidgkrZgEEELhV84866880 = -40051058;    float TQidgkrZgEEELhV63058079 = -713046601;    float TQidgkrZgEEELhV23928721 = -737786848;    float TQidgkrZgEEELhV39704848 = -533782670;    float TQidgkrZgEEELhV90768870 = -31780111;    float TQidgkrZgEEELhV19434980 = -95175961;    float TQidgkrZgEEELhV18152419 = -300472;    float TQidgkrZgEEELhV89427005 = -719174105;    float TQidgkrZgEEELhV19447838 = -680034545;    float TQidgkrZgEEELhV37346154 = -880884351;    float TQidgkrZgEEELhV21638282 = -455193661;    float TQidgkrZgEEELhV63837958 = -800776186;    float TQidgkrZgEEELhV28765139 = -123055964;    float TQidgkrZgEEELhV2286636 = -684167836;    float TQidgkrZgEEELhV78320138 = 47142113;    float TQidgkrZgEEELhV91606344 = -150048299;    float TQidgkrZgEEELhV870885 = -225693575;    float TQidgkrZgEEELhV44787715 = -179488859;    float TQidgkrZgEEELhV77521880 = 86033162;    float TQidgkrZgEEELhV54144477 = -648272589;    float TQidgkrZgEEELhV77308241 = -98390893;    float TQidgkrZgEEELhV47457889 = -89032261;    float TQidgkrZgEEELhV90619908 = -512078044;    float TQidgkrZgEEELhV85409104 = -379694886;    float TQidgkrZgEEELhV91534774 = -15740682;    float TQidgkrZgEEELhV12657002 = -902744211;    float TQidgkrZgEEELhV63319356 = -819074860;    float TQidgkrZgEEELhV93912181 = -831103754;    float TQidgkrZgEEELhV41909578 = -364495982;    float TQidgkrZgEEELhV68774278 = -274086718;    float TQidgkrZgEEELhV43633867 = -117993943;    float TQidgkrZgEEELhV27396560 = -131242505;    float TQidgkrZgEEELhV79029328 = -870791696;    float TQidgkrZgEEELhV25111942 = -709297705;    float TQidgkrZgEEELhV41652126 = -645910297;    float TQidgkrZgEEELhV80640095 = -374864951;    float TQidgkrZgEEELhV10642263 = -394892993;    float TQidgkrZgEEELhV66296609 = -873881145;    float TQidgkrZgEEELhV53772690 = -66039325;    float TQidgkrZgEEELhV87932205 = -233448049;    float TQidgkrZgEEELhV6196300 = -220591791;    float TQidgkrZgEEELhV22321651 = -45776057;    float TQidgkrZgEEELhV59576747 = -198038267;    float TQidgkrZgEEELhV36292992 = -466852439;    float TQidgkrZgEEELhV26611673 = -398131852;    float TQidgkrZgEEELhV72076715 = -9002653;    float TQidgkrZgEEELhV29739870 = 16405072;    float TQidgkrZgEEELhV84472068 = -16440275;    float TQidgkrZgEEELhV40063437 = -114873088;    float TQidgkrZgEEELhV68984071 = -399969299;    float TQidgkrZgEEELhV16831007 = -99135199;     TQidgkrZgEEELhV30448177 = TQidgkrZgEEELhV91926999;     TQidgkrZgEEELhV91926999 = TQidgkrZgEEELhV5924008;     TQidgkrZgEEELhV5924008 = TQidgkrZgEEELhV7664536;     TQidgkrZgEEELhV7664536 = TQidgkrZgEEELhV41676024;     TQidgkrZgEEELhV41676024 = TQidgkrZgEEELhV74047710;     TQidgkrZgEEELhV74047710 = TQidgkrZgEEELhV97000609;     TQidgkrZgEEELhV97000609 = TQidgkrZgEEELhV28805642;     TQidgkrZgEEELhV28805642 = TQidgkrZgEEELhV22018313;     TQidgkrZgEEELhV22018313 = TQidgkrZgEEELhV26177328;     TQidgkrZgEEELhV26177328 = TQidgkrZgEEELhV6872342;     TQidgkrZgEEELhV6872342 = TQidgkrZgEEELhV89329275;     TQidgkrZgEEELhV89329275 = TQidgkrZgEEELhV84889066;     TQidgkrZgEEELhV84889066 = TQidgkrZgEEELhV78493195;     TQidgkrZgEEELhV78493195 = TQidgkrZgEEELhV89056897;     TQidgkrZgEEELhV89056897 = TQidgkrZgEEELhV26482474;     TQidgkrZgEEELhV26482474 = TQidgkrZgEEELhV64259054;     TQidgkrZgEEELhV64259054 = TQidgkrZgEEELhV33659059;     TQidgkrZgEEELhV33659059 = TQidgkrZgEEELhV27449448;     TQidgkrZgEEELhV27449448 = TQidgkrZgEEELhV71162010;     TQidgkrZgEEELhV71162010 = TQidgkrZgEEELhV50078060;     TQidgkrZgEEELhV50078060 = TQidgkrZgEEELhV12295758;     TQidgkrZgEEELhV12295758 = TQidgkrZgEEELhV2499994;     TQidgkrZgEEELhV2499994 = TQidgkrZgEEELhV86476169;     TQidgkrZgEEELhV86476169 = TQidgkrZgEEELhV70318382;     TQidgkrZgEEELhV70318382 = TQidgkrZgEEELhV20037742;     TQidgkrZgEEELhV20037742 = TQidgkrZgEEELhV10209753;     TQidgkrZgEEELhV10209753 = TQidgkrZgEEELhV68235470;     TQidgkrZgEEELhV68235470 = TQidgkrZgEEELhV26519006;     TQidgkrZgEEELhV26519006 = TQidgkrZgEEELhV43698175;     TQidgkrZgEEELhV43698175 = TQidgkrZgEEELhV34570984;     TQidgkrZgEEELhV34570984 = TQidgkrZgEEELhV6001458;     TQidgkrZgEEELhV6001458 = TQidgkrZgEEELhV44541561;     TQidgkrZgEEELhV44541561 = TQidgkrZgEEELhV7367186;     TQidgkrZgEEELhV7367186 = TQidgkrZgEEELhV24348718;     TQidgkrZgEEELhV24348718 = TQidgkrZgEEELhV11748656;     TQidgkrZgEEELhV11748656 = TQidgkrZgEEELhV79024585;     TQidgkrZgEEELhV79024585 = TQidgkrZgEEELhV73639146;     TQidgkrZgEEELhV73639146 = TQidgkrZgEEELhV48249955;     TQidgkrZgEEELhV48249955 = TQidgkrZgEEELhV35914674;     TQidgkrZgEEELhV35914674 = TQidgkrZgEEELhV58505009;     TQidgkrZgEEELhV58505009 = TQidgkrZgEEELhV86758704;     TQidgkrZgEEELhV86758704 = TQidgkrZgEEELhV18383576;     TQidgkrZgEEELhV18383576 = TQidgkrZgEEELhV60590416;     TQidgkrZgEEELhV60590416 = TQidgkrZgEEELhV17701891;     TQidgkrZgEEELhV17701891 = TQidgkrZgEEELhV26684515;     TQidgkrZgEEELhV26684515 = TQidgkrZgEEELhV92641182;     TQidgkrZgEEELhV92641182 = TQidgkrZgEEELhV31180424;     TQidgkrZgEEELhV31180424 = TQidgkrZgEEELhV43123528;     TQidgkrZgEEELhV43123528 = TQidgkrZgEEELhV84866880;     TQidgkrZgEEELhV84866880 = TQidgkrZgEEELhV63058079;     TQidgkrZgEEELhV63058079 = TQidgkrZgEEELhV23928721;     TQidgkrZgEEELhV23928721 = TQidgkrZgEEELhV39704848;     TQidgkrZgEEELhV39704848 = TQidgkrZgEEELhV90768870;     TQidgkrZgEEELhV90768870 = TQidgkrZgEEELhV19434980;     TQidgkrZgEEELhV19434980 = TQidgkrZgEEELhV18152419;     TQidgkrZgEEELhV18152419 = TQidgkrZgEEELhV89427005;     TQidgkrZgEEELhV89427005 = TQidgkrZgEEELhV19447838;     TQidgkrZgEEELhV19447838 = TQidgkrZgEEELhV37346154;     TQidgkrZgEEELhV37346154 = TQidgkrZgEEELhV21638282;     TQidgkrZgEEELhV21638282 = TQidgkrZgEEELhV63837958;     TQidgkrZgEEELhV63837958 = TQidgkrZgEEELhV28765139;     TQidgkrZgEEELhV28765139 = TQidgkrZgEEELhV2286636;     TQidgkrZgEEELhV2286636 = TQidgkrZgEEELhV78320138;     TQidgkrZgEEELhV78320138 = TQidgkrZgEEELhV91606344;     TQidgkrZgEEELhV91606344 = TQidgkrZgEEELhV870885;     TQidgkrZgEEELhV870885 = TQidgkrZgEEELhV44787715;     TQidgkrZgEEELhV44787715 = TQidgkrZgEEELhV77521880;     TQidgkrZgEEELhV77521880 = TQidgkrZgEEELhV54144477;     TQidgkrZgEEELhV54144477 = TQidgkrZgEEELhV77308241;     TQidgkrZgEEELhV77308241 = TQidgkrZgEEELhV47457889;     TQidgkrZgEEELhV47457889 = TQidgkrZgEEELhV90619908;     TQidgkrZgEEELhV90619908 = TQidgkrZgEEELhV85409104;     TQidgkrZgEEELhV85409104 = TQidgkrZgEEELhV91534774;     TQidgkrZgEEELhV91534774 = TQidgkrZgEEELhV12657002;     TQidgkrZgEEELhV12657002 = TQidgkrZgEEELhV63319356;     TQidgkrZgEEELhV63319356 = TQidgkrZgEEELhV93912181;     TQidgkrZgEEELhV93912181 = TQidgkrZgEEELhV41909578;     TQidgkrZgEEELhV41909578 = TQidgkrZgEEELhV68774278;     TQidgkrZgEEELhV68774278 = TQidgkrZgEEELhV43633867;     TQidgkrZgEEELhV43633867 = TQidgkrZgEEELhV27396560;     TQidgkrZgEEELhV27396560 = TQidgkrZgEEELhV79029328;     TQidgkrZgEEELhV79029328 = TQidgkrZgEEELhV25111942;     TQidgkrZgEEELhV25111942 = TQidgkrZgEEELhV41652126;     TQidgkrZgEEELhV41652126 = TQidgkrZgEEELhV80640095;     TQidgkrZgEEELhV80640095 = TQidgkrZgEEELhV10642263;     TQidgkrZgEEELhV10642263 = TQidgkrZgEEELhV66296609;     TQidgkrZgEEELhV66296609 = TQidgkrZgEEELhV53772690;     TQidgkrZgEEELhV53772690 = TQidgkrZgEEELhV87932205;     TQidgkrZgEEELhV87932205 = TQidgkrZgEEELhV6196300;     TQidgkrZgEEELhV6196300 = TQidgkrZgEEELhV22321651;     TQidgkrZgEEELhV22321651 = TQidgkrZgEEELhV59576747;     TQidgkrZgEEELhV59576747 = TQidgkrZgEEELhV36292992;     TQidgkrZgEEELhV36292992 = TQidgkrZgEEELhV26611673;     TQidgkrZgEEELhV26611673 = TQidgkrZgEEELhV72076715;     TQidgkrZgEEELhV72076715 = TQidgkrZgEEELhV29739870;     TQidgkrZgEEELhV29739870 = TQidgkrZgEEELhV84472068;     TQidgkrZgEEELhV84472068 = TQidgkrZgEEELhV40063437;     TQidgkrZgEEELhV40063437 = TQidgkrZgEEELhV68984071;     TQidgkrZgEEELhV68984071 = TQidgkrZgEEELhV16831007;     TQidgkrZgEEELhV16831007 = TQidgkrZgEEELhV30448177;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void BPYNLVBTNGwgolUP15208733() {     float zJMmZiHGuvHjUEZ49637134 = 57089978;    float zJMmZiHGuvHjUEZ64224857 = -475296010;    float zJMmZiHGuvHjUEZ30583389 = -829450736;    float zJMmZiHGuvHjUEZ91955983 = -281524020;    float zJMmZiHGuvHjUEZ68682527 = -623692808;    float zJMmZiHGuvHjUEZ86804667 = -869415868;    float zJMmZiHGuvHjUEZ75991243 = -737987623;    float zJMmZiHGuvHjUEZ65063235 = 69359417;    float zJMmZiHGuvHjUEZ91015282 = -512937470;    float zJMmZiHGuvHjUEZ90546114 = -633649282;    float zJMmZiHGuvHjUEZ67622507 = -416178303;    float zJMmZiHGuvHjUEZ65817517 = -535089964;    float zJMmZiHGuvHjUEZ42324649 = -815777120;    float zJMmZiHGuvHjUEZ72162983 = 49941930;    float zJMmZiHGuvHjUEZ6258564 = -429750504;    float zJMmZiHGuvHjUEZ7273256 = -394248679;    float zJMmZiHGuvHjUEZ64107641 = -726054678;    float zJMmZiHGuvHjUEZ24495987 = -720228415;    float zJMmZiHGuvHjUEZ48547988 = -512749915;    float zJMmZiHGuvHjUEZ67033110 = -640224634;    float zJMmZiHGuvHjUEZ50835009 = -140957801;    float zJMmZiHGuvHjUEZ38423088 = -588634425;    float zJMmZiHGuvHjUEZ5322842 = -322582658;    float zJMmZiHGuvHjUEZ37761701 = -641706774;    float zJMmZiHGuvHjUEZ86495736 = -70769592;    float zJMmZiHGuvHjUEZ67154593 = -256645350;    float zJMmZiHGuvHjUEZ22000684 = -351763780;    float zJMmZiHGuvHjUEZ16159571 = -800474922;    float zJMmZiHGuvHjUEZ32426916 = -138181757;    float zJMmZiHGuvHjUEZ8690043 = -19593723;    float zJMmZiHGuvHjUEZ1105960 = -516665114;    float zJMmZiHGuvHjUEZ83893906 = -338010693;    float zJMmZiHGuvHjUEZ85431849 = -808685291;    float zJMmZiHGuvHjUEZ72557489 = 79267262;    float zJMmZiHGuvHjUEZ75402459 = -846969199;    float zJMmZiHGuvHjUEZ90342543 = -978146217;    float zJMmZiHGuvHjUEZ16258624 = 36543231;    float zJMmZiHGuvHjUEZ5318661 = -234272905;    float zJMmZiHGuvHjUEZ3942639 = -197079266;    float zJMmZiHGuvHjUEZ67502429 = -712256579;    float zJMmZiHGuvHjUEZ68868470 = -637946035;    float zJMmZiHGuvHjUEZ19765897 = -907387009;    float zJMmZiHGuvHjUEZ80283740 = 85814238;    float zJMmZiHGuvHjUEZ88685375 = -176329297;    float zJMmZiHGuvHjUEZ61687548 = -877749161;    float zJMmZiHGuvHjUEZ76484756 = -19064226;    float zJMmZiHGuvHjUEZ17467524 = -926416581;    float zJMmZiHGuvHjUEZ12265003 = -951499370;    float zJMmZiHGuvHjUEZ81232018 = -443687744;    float zJMmZiHGuvHjUEZ25224377 = -476068417;    float zJMmZiHGuvHjUEZ70660265 = -74540553;    float zJMmZiHGuvHjUEZ44372655 = -321346237;    float zJMmZiHGuvHjUEZ78260051 = -409706578;    float zJMmZiHGuvHjUEZ42443053 = -268077947;    float zJMmZiHGuvHjUEZ64373723 = 98506221;    float zJMmZiHGuvHjUEZ11214047 = -354275598;    float zJMmZiHGuvHjUEZ58902015 = -52713353;    float zJMmZiHGuvHjUEZ92821687 = -87743962;    float zJMmZiHGuvHjUEZ5460247 = -110754429;    float zJMmZiHGuvHjUEZ1527934 = -267047458;    float zJMmZiHGuvHjUEZ64803983 = -417652088;    float zJMmZiHGuvHjUEZ59831672 = -937512701;    float zJMmZiHGuvHjUEZ32636320 = -792458827;    float zJMmZiHGuvHjUEZ82325240 = -393343748;    float zJMmZiHGuvHjUEZ89440155 = -16984169;    float zJMmZiHGuvHjUEZ83728600 = 21832390;    float zJMmZiHGuvHjUEZ80385667 = -726404674;    float zJMmZiHGuvHjUEZ69767160 = -795044383;    float zJMmZiHGuvHjUEZ96760523 = -103088872;    float zJMmZiHGuvHjUEZ15916020 = -451604287;    float zJMmZiHGuvHjUEZ91014631 = -330791910;    float zJMmZiHGuvHjUEZ58788980 = -391781773;    float zJMmZiHGuvHjUEZ20553349 = -423149149;    float zJMmZiHGuvHjUEZ81045559 = -800493337;    float zJMmZiHGuvHjUEZ98164640 = 97721400;    float zJMmZiHGuvHjUEZ31069113 = -233570793;    float zJMmZiHGuvHjUEZ58139347 = -574448663;    float zJMmZiHGuvHjUEZ16637467 = -46253362;    float zJMmZiHGuvHjUEZ76074153 = -763957614;    float zJMmZiHGuvHjUEZ10010980 = 48294634;    float zJMmZiHGuvHjUEZ49687070 = -330228770;    float zJMmZiHGuvHjUEZ9735682 = -400264411;    float zJMmZiHGuvHjUEZ34927552 = -256787179;    float zJMmZiHGuvHjUEZ7202539 = -662113341;    float zJMmZiHGuvHjUEZ38029778 = -945053171;    float zJMmZiHGuvHjUEZ56733304 = -95318877;    float zJMmZiHGuvHjUEZ5633856 = -928304115;    float zJMmZiHGuvHjUEZ42988797 = -440607345;    float zJMmZiHGuvHjUEZ8183766 = 80761041;    float zJMmZiHGuvHjUEZ64188412 = -392693602;    float zJMmZiHGuvHjUEZ31440528 = -825432865;    float zJMmZiHGuvHjUEZ23436937 = -875712808;    float zJMmZiHGuvHjUEZ99858414 = -23518477;    float zJMmZiHGuvHjUEZ2414705 = -930031808;    float zJMmZiHGuvHjUEZ2698446 = -194604491;    float zJMmZiHGuvHjUEZ9036798 = -700433334;    float zJMmZiHGuvHjUEZ87129577 = -14928183;    float zJMmZiHGuvHjUEZ97958500 = -520842015;    float zJMmZiHGuvHjUEZ99245220 = -59345129;    float zJMmZiHGuvHjUEZ77958947 = 57089978;     zJMmZiHGuvHjUEZ49637134 = zJMmZiHGuvHjUEZ64224857;     zJMmZiHGuvHjUEZ64224857 = zJMmZiHGuvHjUEZ30583389;     zJMmZiHGuvHjUEZ30583389 = zJMmZiHGuvHjUEZ91955983;     zJMmZiHGuvHjUEZ91955983 = zJMmZiHGuvHjUEZ68682527;     zJMmZiHGuvHjUEZ68682527 = zJMmZiHGuvHjUEZ86804667;     zJMmZiHGuvHjUEZ86804667 = zJMmZiHGuvHjUEZ75991243;     zJMmZiHGuvHjUEZ75991243 = zJMmZiHGuvHjUEZ65063235;     zJMmZiHGuvHjUEZ65063235 = zJMmZiHGuvHjUEZ91015282;     zJMmZiHGuvHjUEZ91015282 = zJMmZiHGuvHjUEZ90546114;     zJMmZiHGuvHjUEZ90546114 = zJMmZiHGuvHjUEZ67622507;     zJMmZiHGuvHjUEZ67622507 = zJMmZiHGuvHjUEZ65817517;     zJMmZiHGuvHjUEZ65817517 = zJMmZiHGuvHjUEZ42324649;     zJMmZiHGuvHjUEZ42324649 = zJMmZiHGuvHjUEZ72162983;     zJMmZiHGuvHjUEZ72162983 = zJMmZiHGuvHjUEZ6258564;     zJMmZiHGuvHjUEZ6258564 = zJMmZiHGuvHjUEZ7273256;     zJMmZiHGuvHjUEZ7273256 = zJMmZiHGuvHjUEZ64107641;     zJMmZiHGuvHjUEZ64107641 = zJMmZiHGuvHjUEZ24495987;     zJMmZiHGuvHjUEZ24495987 = zJMmZiHGuvHjUEZ48547988;     zJMmZiHGuvHjUEZ48547988 = zJMmZiHGuvHjUEZ67033110;     zJMmZiHGuvHjUEZ67033110 = zJMmZiHGuvHjUEZ50835009;     zJMmZiHGuvHjUEZ50835009 = zJMmZiHGuvHjUEZ38423088;     zJMmZiHGuvHjUEZ38423088 = zJMmZiHGuvHjUEZ5322842;     zJMmZiHGuvHjUEZ5322842 = zJMmZiHGuvHjUEZ37761701;     zJMmZiHGuvHjUEZ37761701 = zJMmZiHGuvHjUEZ86495736;     zJMmZiHGuvHjUEZ86495736 = zJMmZiHGuvHjUEZ67154593;     zJMmZiHGuvHjUEZ67154593 = zJMmZiHGuvHjUEZ22000684;     zJMmZiHGuvHjUEZ22000684 = zJMmZiHGuvHjUEZ16159571;     zJMmZiHGuvHjUEZ16159571 = zJMmZiHGuvHjUEZ32426916;     zJMmZiHGuvHjUEZ32426916 = zJMmZiHGuvHjUEZ8690043;     zJMmZiHGuvHjUEZ8690043 = zJMmZiHGuvHjUEZ1105960;     zJMmZiHGuvHjUEZ1105960 = zJMmZiHGuvHjUEZ83893906;     zJMmZiHGuvHjUEZ83893906 = zJMmZiHGuvHjUEZ85431849;     zJMmZiHGuvHjUEZ85431849 = zJMmZiHGuvHjUEZ72557489;     zJMmZiHGuvHjUEZ72557489 = zJMmZiHGuvHjUEZ75402459;     zJMmZiHGuvHjUEZ75402459 = zJMmZiHGuvHjUEZ90342543;     zJMmZiHGuvHjUEZ90342543 = zJMmZiHGuvHjUEZ16258624;     zJMmZiHGuvHjUEZ16258624 = zJMmZiHGuvHjUEZ5318661;     zJMmZiHGuvHjUEZ5318661 = zJMmZiHGuvHjUEZ3942639;     zJMmZiHGuvHjUEZ3942639 = zJMmZiHGuvHjUEZ67502429;     zJMmZiHGuvHjUEZ67502429 = zJMmZiHGuvHjUEZ68868470;     zJMmZiHGuvHjUEZ68868470 = zJMmZiHGuvHjUEZ19765897;     zJMmZiHGuvHjUEZ19765897 = zJMmZiHGuvHjUEZ80283740;     zJMmZiHGuvHjUEZ80283740 = zJMmZiHGuvHjUEZ88685375;     zJMmZiHGuvHjUEZ88685375 = zJMmZiHGuvHjUEZ61687548;     zJMmZiHGuvHjUEZ61687548 = zJMmZiHGuvHjUEZ76484756;     zJMmZiHGuvHjUEZ76484756 = zJMmZiHGuvHjUEZ17467524;     zJMmZiHGuvHjUEZ17467524 = zJMmZiHGuvHjUEZ12265003;     zJMmZiHGuvHjUEZ12265003 = zJMmZiHGuvHjUEZ81232018;     zJMmZiHGuvHjUEZ81232018 = zJMmZiHGuvHjUEZ25224377;     zJMmZiHGuvHjUEZ25224377 = zJMmZiHGuvHjUEZ70660265;     zJMmZiHGuvHjUEZ70660265 = zJMmZiHGuvHjUEZ44372655;     zJMmZiHGuvHjUEZ44372655 = zJMmZiHGuvHjUEZ78260051;     zJMmZiHGuvHjUEZ78260051 = zJMmZiHGuvHjUEZ42443053;     zJMmZiHGuvHjUEZ42443053 = zJMmZiHGuvHjUEZ64373723;     zJMmZiHGuvHjUEZ64373723 = zJMmZiHGuvHjUEZ11214047;     zJMmZiHGuvHjUEZ11214047 = zJMmZiHGuvHjUEZ58902015;     zJMmZiHGuvHjUEZ58902015 = zJMmZiHGuvHjUEZ92821687;     zJMmZiHGuvHjUEZ92821687 = zJMmZiHGuvHjUEZ5460247;     zJMmZiHGuvHjUEZ5460247 = zJMmZiHGuvHjUEZ1527934;     zJMmZiHGuvHjUEZ1527934 = zJMmZiHGuvHjUEZ64803983;     zJMmZiHGuvHjUEZ64803983 = zJMmZiHGuvHjUEZ59831672;     zJMmZiHGuvHjUEZ59831672 = zJMmZiHGuvHjUEZ32636320;     zJMmZiHGuvHjUEZ32636320 = zJMmZiHGuvHjUEZ82325240;     zJMmZiHGuvHjUEZ82325240 = zJMmZiHGuvHjUEZ89440155;     zJMmZiHGuvHjUEZ89440155 = zJMmZiHGuvHjUEZ83728600;     zJMmZiHGuvHjUEZ83728600 = zJMmZiHGuvHjUEZ80385667;     zJMmZiHGuvHjUEZ80385667 = zJMmZiHGuvHjUEZ69767160;     zJMmZiHGuvHjUEZ69767160 = zJMmZiHGuvHjUEZ96760523;     zJMmZiHGuvHjUEZ96760523 = zJMmZiHGuvHjUEZ15916020;     zJMmZiHGuvHjUEZ15916020 = zJMmZiHGuvHjUEZ91014631;     zJMmZiHGuvHjUEZ91014631 = zJMmZiHGuvHjUEZ58788980;     zJMmZiHGuvHjUEZ58788980 = zJMmZiHGuvHjUEZ20553349;     zJMmZiHGuvHjUEZ20553349 = zJMmZiHGuvHjUEZ81045559;     zJMmZiHGuvHjUEZ81045559 = zJMmZiHGuvHjUEZ98164640;     zJMmZiHGuvHjUEZ98164640 = zJMmZiHGuvHjUEZ31069113;     zJMmZiHGuvHjUEZ31069113 = zJMmZiHGuvHjUEZ58139347;     zJMmZiHGuvHjUEZ58139347 = zJMmZiHGuvHjUEZ16637467;     zJMmZiHGuvHjUEZ16637467 = zJMmZiHGuvHjUEZ76074153;     zJMmZiHGuvHjUEZ76074153 = zJMmZiHGuvHjUEZ10010980;     zJMmZiHGuvHjUEZ10010980 = zJMmZiHGuvHjUEZ49687070;     zJMmZiHGuvHjUEZ49687070 = zJMmZiHGuvHjUEZ9735682;     zJMmZiHGuvHjUEZ9735682 = zJMmZiHGuvHjUEZ34927552;     zJMmZiHGuvHjUEZ34927552 = zJMmZiHGuvHjUEZ7202539;     zJMmZiHGuvHjUEZ7202539 = zJMmZiHGuvHjUEZ38029778;     zJMmZiHGuvHjUEZ38029778 = zJMmZiHGuvHjUEZ56733304;     zJMmZiHGuvHjUEZ56733304 = zJMmZiHGuvHjUEZ5633856;     zJMmZiHGuvHjUEZ5633856 = zJMmZiHGuvHjUEZ42988797;     zJMmZiHGuvHjUEZ42988797 = zJMmZiHGuvHjUEZ8183766;     zJMmZiHGuvHjUEZ8183766 = zJMmZiHGuvHjUEZ64188412;     zJMmZiHGuvHjUEZ64188412 = zJMmZiHGuvHjUEZ31440528;     zJMmZiHGuvHjUEZ31440528 = zJMmZiHGuvHjUEZ23436937;     zJMmZiHGuvHjUEZ23436937 = zJMmZiHGuvHjUEZ99858414;     zJMmZiHGuvHjUEZ99858414 = zJMmZiHGuvHjUEZ2414705;     zJMmZiHGuvHjUEZ2414705 = zJMmZiHGuvHjUEZ2698446;     zJMmZiHGuvHjUEZ2698446 = zJMmZiHGuvHjUEZ9036798;     zJMmZiHGuvHjUEZ9036798 = zJMmZiHGuvHjUEZ87129577;     zJMmZiHGuvHjUEZ87129577 = zJMmZiHGuvHjUEZ97958500;     zJMmZiHGuvHjUEZ97958500 = zJMmZiHGuvHjUEZ99245220;     zJMmZiHGuvHjUEZ99245220 = zJMmZiHGuvHjUEZ77958947;     zJMmZiHGuvHjUEZ77958947 = zJMmZiHGuvHjUEZ49637134;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void oDUVqPLvHiLOgBTz21867477() {     float qRhpCETaVWowObY49630663 = -915434166;    float qRhpCETaVWowObY2797174 = -517891141;    float qRhpCETaVWowObY31896601 = -786579350;    float qRhpCETaVWowObY77035843 = -102064848;    float qRhpCETaVWowObY77522629 = 7951863;    float qRhpCETaVWowObY21610100 = -568848679;    float qRhpCETaVWowObY44538579 = -442524090;    float qRhpCETaVWowObY34277286 = -734817916;    float qRhpCETaVWowObY73755447 = -226583541;    float qRhpCETaVWowObY87185628 = -270456153;    float qRhpCETaVWowObY87943112 = -375650376;    float qRhpCETaVWowObY80109194 = -270568336;    float qRhpCETaVWowObY74177563 = -837435595;    float qRhpCETaVWowObY26007203 = -295211123;    float qRhpCETaVWowObY51430672 = -392463374;    float qRhpCETaVWowObY81837803 = -727044330;    float qRhpCETaVWowObY19960401 = -545669248;    float qRhpCETaVWowObY47496520 = -17300040;    float qRhpCETaVWowObY68976258 = -30732181;    float qRhpCETaVWowObY39106874 = -152249085;    float qRhpCETaVWowObY30504433 = -951424180;    float qRhpCETaVWowObY16719354 = 1373692;    float qRhpCETaVWowObY42201239 = -781540500;    float qRhpCETaVWowObY81411809 = -522627297;    float qRhpCETaVWowObY14938426 = -497463457;    float qRhpCETaVWowObY83728671 = -164851275;    float qRhpCETaVWowObY92222265 = -652563493;    float qRhpCETaVWowObY51563095 = -309441916;    float qRhpCETaVWowObY45105362 = -784252057;    float qRhpCETaVWowObY84779912 = -795025097;    float qRhpCETaVWowObY34518638 = -903758348;    float qRhpCETaVWowObY68617764 = -729832451;    float qRhpCETaVWowObY70884273 = 89370496;    float qRhpCETaVWowObY72840326 = -897414470;    float qRhpCETaVWowObY57760497 = -6283121;    float qRhpCETaVWowObY33170733 = 42088055;    float qRhpCETaVWowObY28199992 = -63110148;    float qRhpCETaVWowObY75236340 = -229535587;    float qRhpCETaVWowObY39256235 = -909760670;    float qRhpCETaVWowObY39375322 = -321688265;    float qRhpCETaVWowObY56724137 = -725889933;    float qRhpCETaVWowObY5463558 = -1179071;    float qRhpCETaVWowObY40567101 = -473316014;    float qRhpCETaVWowObY47917274 = -833293740;    float qRhpCETaVWowObY45705600 = -251008562;    float qRhpCETaVWowObY16912725 = -931108879;    float qRhpCETaVWowObY45165315 = -307697203;    float qRhpCETaVWowObY87648583 = -460260725;    float qRhpCETaVWowObY99890407 = -7451581;    float qRhpCETaVWowObY45269799 = -63093309;    float qRhpCETaVWowObY83919634 = -753095560;    float qRhpCETaVWowObY58919913 = -703491981;    float qRhpCETaVWowObY53708399 = -301760589;    float qRhpCETaVWowObY52693323 = -799997798;    float qRhpCETaVWowObY91363043 = -758393052;    float qRhpCETaVWowObY32911310 = -816807858;    float qRhpCETaVWowObY60595934 = -736350642;    float qRhpCETaVWowObY50484791 = -163952053;    float qRhpCETaVWowObY62097417 = -604601391;    float qRhpCETaVWowObY93793958 = -827196863;    float qRhpCETaVWowObY29387834 = -916285187;    float qRhpCETaVWowObY92975483 = -33082174;    float qRhpCETaVWowObY89171923 = -950565860;    float qRhpCETaVWowObY88975534 = -431558445;    float qRhpCETaVWowObY52666990 = -366697805;    float qRhpCETaVWowObY19325349 = -645817925;    float qRhpCETaVWowObY9224922 = -259938833;    float qRhpCETaVWowObY1337238 = -940021125;    float qRhpCETaVWowObY68246706 = -188928002;    float qRhpCETaVWowObY18259939 = -334551430;    float qRhpCETaVWowObY53637812 = -563934183;    float qRhpCETaVWowObY44724060 = -216133661;    float qRhpCETaVWowObY8240285 = -107539371;    float qRhpCETaVWowObY29600937 = -709043917;    float qRhpCETaVWowObY82382736 = -426359153;    float qRhpCETaVWowObY25040876 = -850245110;    float qRhpCETaVWowObY76152252 = -525310294;    float qRhpCETaVWowObY94283965 = -948246760;    float qRhpCETaVWowObY35706209 = -171618736;    float qRhpCETaVWowObY98025701 = -566354579;    float qRhpCETaVWowObY38563356 = -857154072;    float qRhpCETaVWowObY4573683 = -92302768;    float qRhpCETaVWowObY51672688 = -201990336;    float qRhpCETaVWowObY99835563 = -621158749;    float qRhpCETaVWowObY860278 = 58070463;    float qRhpCETaVWowObY75598725 = -100266368;    float qRhpCETaVWowObY14909365 = -328071862;    float qRhpCETaVWowObY18190950 = -110631706;    float qRhpCETaVWowObY81477282 = -39021419;    float qRhpCETaVWowObY24849188 = -189475263;    float qRhpCETaVWowObY72574799 = -221561303;    float qRhpCETaVWowObY77715200 = -899158095;    float qRhpCETaVWowObY13138923 = -624934197;    float qRhpCETaVWowObY45462277 = 17436193;    float qRhpCETaVWowObY9987488 = -405403079;    float qRhpCETaVWowObY63748654 = -592807760;    float qRhpCETaVWowObY16291635 = -50613211;    float qRhpCETaVWowObY51591566 = 58242430;    float qRhpCETaVWowObY95250283 = -366595936;    float qRhpCETaVWowObY26380252 = -915434166;     qRhpCETaVWowObY49630663 = qRhpCETaVWowObY2797174;     qRhpCETaVWowObY2797174 = qRhpCETaVWowObY31896601;     qRhpCETaVWowObY31896601 = qRhpCETaVWowObY77035843;     qRhpCETaVWowObY77035843 = qRhpCETaVWowObY77522629;     qRhpCETaVWowObY77522629 = qRhpCETaVWowObY21610100;     qRhpCETaVWowObY21610100 = qRhpCETaVWowObY44538579;     qRhpCETaVWowObY44538579 = qRhpCETaVWowObY34277286;     qRhpCETaVWowObY34277286 = qRhpCETaVWowObY73755447;     qRhpCETaVWowObY73755447 = qRhpCETaVWowObY87185628;     qRhpCETaVWowObY87185628 = qRhpCETaVWowObY87943112;     qRhpCETaVWowObY87943112 = qRhpCETaVWowObY80109194;     qRhpCETaVWowObY80109194 = qRhpCETaVWowObY74177563;     qRhpCETaVWowObY74177563 = qRhpCETaVWowObY26007203;     qRhpCETaVWowObY26007203 = qRhpCETaVWowObY51430672;     qRhpCETaVWowObY51430672 = qRhpCETaVWowObY81837803;     qRhpCETaVWowObY81837803 = qRhpCETaVWowObY19960401;     qRhpCETaVWowObY19960401 = qRhpCETaVWowObY47496520;     qRhpCETaVWowObY47496520 = qRhpCETaVWowObY68976258;     qRhpCETaVWowObY68976258 = qRhpCETaVWowObY39106874;     qRhpCETaVWowObY39106874 = qRhpCETaVWowObY30504433;     qRhpCETaVWowObY30504433 = qRhpCETaVWowObY16719354;     qRhpCETaVWowObY16719354 = qRhpCETaVWowObY42201239;     qRhpCETaVWowObY42201239 = qRhpCETaVWowObY81411809;     qRhpCETaVWowObY81411809 = qRhpCETaVWowObY14938426;     qRhpCETaVWowObY14938426 = qRhpCETaVWowObY83728671;     qRhpCETaVWowObY83728671 = qRhpCETaVWowObY92222265;     qRhpCETaVWowObY92222265 = qRhpCETaVWowObY51563095;     qRhpCETaVWowObY51563095 = qRhpCETaVWowObY45105362;     qRhpCETaVWowObY45105362 = qRhpCETaVWowObY84779912;     qRhpCETaVWowObY84779912 = qRhpCETaVWowObY34518638;     qRhpCETaVWowObY34518638 = qRhpCETaVWowObY68617764;     qRhpCETaVWowObY68617764 = qRhpCETaVWowObY70884273;     qRhpCETaVWowObY70884273 = qRhpCETaVWowObY72840326;     qRhpCETaVWowObY72840326 = qRhpCETaVWowObY57760497;     qRhpCETaVWowObY57760497 = qRhpCETaVWowObY33170733;     qRhpCETaVWowObY33170733 = qRhpCETaVWowObY28199992;     qRhpCETaVWowObY28199992 = qRhpCETaVWowObY75236340;     qRhpCETaVWowObY75236340 = qRhpCETaVWowObY39256235;     qRhpCETaVWowObY39256235 = qRhpCETaVWowObY39375322;     qRhpCETaVWowObY39375322 = qRhpCETaVWowObY56724137;     qRhpCETaVWowObY56724137 = qRhpCETaVWowObY5463558;     qRhpCETaVWowObY5463558 = qRhpCETaVWowObY40567101;     qRhpCETaVWowObY40567101 = qRhpCETaVWowObY47917274;     qRhpCETaVWowObY47917274 = qRhpCETaVWowObY45705600;     qRhpCETaVWowObY45705600 = qRhpCETaVWowObY16912725;     qRhpCETaVWowObY16912725 = qRhpCETaVWowObY45165315;     qRhpCETaVWowObY45165315 = qRhpCETaVWowObY87648583;     qRhpCETaVWowObY87648583 = qRhpCETaVWowObY99890407;     qRhpCETaVWowObY99890407 = qRhpCETaVWowObY45269799;     qRhpCETaVWowObY45269799 = qRhpCETaVWowObY83919634;     qRhpCETaVWowObY83919634 = qRhpCETaVWowObY58919913;     qRhpCETaVWowObY58919913 = qRhpCETaVWowObY53708399;     qRhpCETaVWowObY53708399 = qRhpCETaVWowObY52693323;     qRhpCETaVWowObY52693323 = qRhpCETaVWowObY91363043;     qRhpCETaVWowObY91363043 = qRhpCETaVWowObY32911310;     qRhpCETaVWowObY32911310 = qRhpCETaVWowObY60595934;     qRhpCETaVWowObY60595934 = qRhpCETaVWowObY50484791;     qRhpCETaVWowObY50484791 = qRhpCETaVWowObY62097417;     qRhpCETaVWowObY62097417 = qRhpCETaVWowObY93793958;     qRhpCETaVWowObY93793958 = qRhpCETaVWowObY29387834;     qRhpCETaVWowObY29387834 = qRhpCETaVWowObY92975483;     qRhpCETaVWowObY92975483 = qRhpCETaVWowObY89171923;     qRhpCETaVWowObY89171923 = qRhpCETaVWowObY88975534;     qRhpCETaVWowObY88975534 = qRhpCETaVWowObY52666990;     qRhpCETaVWowObY52666990 = qRhpCETaVWowObY19325349;     qRhpCETaVWowObY19325349 = qRhpCETaVWowObY9224922;     qRhpCETaVWowObY9224922 = qRhpCETaVWowObY1337238;     qRhpCETaVWowObY1337238 = qRhpCETaVWowObY68246706;     qRhpCETaVWowObY68246706 = qRhpCETaVWowObY18259939;     qRhpCETaVWowObY18259939 = qRhpCETaVWowObY53637812;     qRhpCETaVWowObY53637812 = qRhpCETaVWowObY44724060;     qRhpCETaVWowObY44724060 = qRhpCETaVWowObY8240285;     qRhpCETaVWowObY8240285 = qRhpCETaVWowObY29600937;     qRhpCETaVWowObY29600937 = qRhpCETaVWowObY82382736;     qRhpCETaVWowObY82382736 = qRhpCETaVWowObY25040876;     qRhpCETaVWowObY25040876 = qRhpCETaVWowObY76152252;     qRhpCETaVWowObY76152252 = qRhpCETaVWowObY94283965;     qRhpCETaVWowObY94283965 = qRhpCETaVWowObY35706209;     qRhpCETaVWowObY35706209 = qRhpCETaVWowObY98025701;     qRhpCETaVWowObY98025701 = qRhpCETaVWowObY38563356;     qRhpCETaVWowObY38563356 = qRhpCETaVWowObY4573683;     qRhpCETaVWowObY4573683 = qRhpCETaVWowObY51672688;     qRhpCETaVWowObY51672688 = qRhpCETaVWowObY99835563;     qRhpCETaVWowObY99835563 = qRhpCETaVWowObY860278;     qRhpCETaVWowObY860278 = qRhpCETaVWowObY75598725;     qRhpCETaVWowObY75598725 = qRhpCETaVWowObY14909365;     qRhpCETaVWowObY14909365 = qRhpCETaVWowObY18190950;     qRhpCETaVWowObY18190950 = qRhpCETaVWowObY81477282;     qRhpCETaVWowObY81477282 = qRhpCETaVWowObY24849188;     qRhpCETaVWowObY24849188 = qRhpCETaVWowObY72574799;     qRhpCETaVWowObY72574799 = qRhpCETaVWowObY77715200;     qRhpCETaVWowObY77715200 = qRhpCETaVWowObY13138923;     qRhpCETaVWowObY13138923 = qRhpCETaVWowObY45462277;     qRhpCETaVWowObY45462277 = qRhpCETaVWowObY9987488;     qRhpCETaVWowObY9987488 = qRhpCETaVWowObY63748654;     qRhpCETaVWowObY63748654 = qRhpCETaVWowObY16291635;     qRhpCETaVWowObY16291635 = qRhpCETaVWowObY51591566;     qRhpCETaVWowObY51591566 = qRhpCETaVWowObY95250283;     qRhpCETaVWowObY95250283 = qRhpCETaVWowObY26380252;     qRhpCETaVWowObY26380252 = qRhpCETaVWowObY49630663;}
// Junk Finished

// Junk Code By Peatreat & Thaisen's Gen
void DNKkIXGMiSzLzCwR10094589() {     float cWTShTfYPRuknLR10681380 = -934220025;    float cWTShTfYPRuknLR73894078 = -611325400;    float cWTShTfYPRuknLR849042 = -297579347;    float cWTShTfYPRuknLR9901268 = -109997982;    float cWTShTfYPRuknLR94309801 = -776213949;    float cWTShTfYPRuknLR40036816 = -186744766;    float cWTShTfYPRuknLR91969493 = -64984564;    float cWTShTfYPRuknLR42204920 = 44443382;    float cWTShTfYPRuknLR17862181 = -397492923;    float cWTShTfYPRuknLR24607210 = -632897803;    float cWTShTfYPRuknLR20137796 = -528613920;    float cWTShTfYPRuknLR1703322 = -842919652;    float cWTShTfYPRuknLR57140072 = -32037991;    float cWTShTfYPRuknLR16863563 = 99054226;    float cWTShTfYPRuknLR20721968 = -821721495;    float cWTShTfYPRuknLR98535599 = -466251763;    float cWTShTfYPRuknLR19741693 = -941349025;    float cWTShTfYPRuknLR45372083 = -209933848;    float cWTShTfYPRuknLR10563040 = -512287193;    float cWTShTfYPRuknLR22031796 = 58774924;    float cWTShTfYPRuknLR64931137 = -259932276;    float cWTShTfYPRuknLR43347719 = 4996352;    float cWTShTfYPRuknLR79612019 = -4306947;    float cWTShTfYPRuknLR33268689 = 88619642;    float cWTShTfYPRuknLR49416827 = -762250945;    float cWTShTfYPRuknLR7341903 = -609672695;    float cWTShTfYPRuknLR64809167 = -823861055;    float cWTShTfYPRuknLR20786796 = -99909335;    float cWTShTfYPRuknLR86972342 = -948570395;    float cWTShTfYPRuknLR67545944 = 36989545;    float cWTShTfYPRuknLR41735826 = -847292632;    float cWTShTfYPRuknLR58906858 = -384777938;    float cWTShTfYPRuknLR18836913 = -304102402;    float cWTShTfYPRuknLR55892986 = -41571523;    float cWTShTfYPRuknLR42615901 = -399505358;    float cWTShTfYPRuknLR46695238 = -610306220;    float cWTShTfYPRuknLR15315826 = -308664754;    float cWTShTfYPRuknLR32106751 = -65643315;    float cWTShTfYPRuknLR75256779 = -306293852;    float cWTShTfYPRuknLR40557635 = -647489444;    float cWTShTfYPRuknLR49471359 = -982205139;    float cWTShTfYPRuknLR53140613 = 89029217;    float cWTShTfYPRuknLR18867339 = -595972928;    float cWTShTfYPRuknLR99609992 = -149077306;    float cWTShTfYPRuknLR53684883 = -643281439;    float cWTShTfYPRuknLR88846407 = -580535378;    float cWTShTfYPRuknLR36581141 = -342871802;    float cWTShTfYPRuknLR38104085 = -211208809;    float cWTShTfYPRuknLR54936004 = -207100826;    float cWTShTfYPRuknLR3563961 = -815118391;    float cWTShTfYPRuknLR61567236 = -197475712;    float cWTShTfYPRuknLR55116707 = -713077759;    float cWTShTfYPRuknLR64954803 = -733650684;    float cWTShTfYPRuknLR27333808 = -774650228;    float cWTShTfYPRuknLR45163451 = -723074337;    float cWTShTfYPRuknLR67333661 = -839216377;    float cWTShTfYPRuknLR94282058 = -507018453;    float cWTShTfYPRuknLR67580352 = -286198989;    float cWTShTfYPRuknLR60484440 = -347747038;    float cWTShTfYPRuknLR86967898 = -66541254;    float cWTShTfYPRuknLR75227649 = -362883711;    float cWTShTfYPRuknLR71182698 = -965075229;    float cWTShTfYPRuknLR55232578 = -6986224;    float cWTShTfYPRuknLR50316236 = -334482468;    float cWTShTfYPRuknLR82871384 = -785605172;    float cWTShTfYPRuknLR61230938 = -43835982;    float cWTShTfYPRuknLR82866409 = -438817250;    float cWTShTfYPRuknLR1247086 = -990466468;    float cWTShTfYPRuknLR74247662 = -501440416;    float cWTShTfYPRuknLR74026730 = -111415275;    float cWTShTfYPRuknLR83219774 = -57587009;    float cWTShTfYPRuknLR87634942 = -775705711;    float cWTShTfYPRuknLR70115304 = -903639997;    float cWTShTfYPRuknLR70005405 = -864797750;    float cWTShTfYPRuknLR72560437 = 40980062;    float cWTShTfYPRuknLR11790524 = -248961493;    float cWTShTfYPRuknLR24480381 = -399030721;    float cWTShTfYPRuknLR80002027 = -855229642;    float cWTShTfYPRuknLR79583806 = -268098920;    float cWTShTfYPRuknLR60570420 = -81715567;    float cWTShTfYPRuknLR70760762 = -166800893;    float cWTShTfYPRuknLR26705082 = -512652247;    float cWTShTfYPRuknLR65850791 = -892808509;    float cWTShTfYPRuknLR83408382 = -33452004;    float cWTShTfYPRuknLR5978709 = -765534744;    float cWTShTfYPRuknLR86619118 = -34214874;    float cWTShTfYPRuknLR93952054 = -651127254;    float cWTShTfYPRuknLR91503104 = -529452175;    float cWTShTfYPRuknLR10729536 = -318497186;    float cWTShTfYPRuknLR75282240 = -560288982;    float cWTShTfYPRuknLR52413179 = -3287768;    float cWTShTfYPRuknLR47735474 = 77534235;    float cWTShTfYPRuknLR71622310 = -717896277;    float cWTShTfYPRuknLR88288880 = -139752598;    float cWTShTfYPRuknLR65329986 = -184605733;    float cWTShTfYPRuknLR78288660 = 82870090;    float cWTShTfYPRuknLR97908035 = -903984560;    float cWTShTfYPRuknLR68551102 = -161490460;    float cWTShTfYPRuknLR16738609 = -363472134;    float cWTShTfYPRuknLR92453944 = -934220025;     cWTShTfYPRuknLR10681380 = cWTShTfYPRuknLR73894078;     cWTShTfYPRuknLR73894078 = cWTShTfYPRuknLR849042;     cWTShTfYPRuknLR849042 = cWTShTfYPRuknLR9901268;     cWTShTfYPRuknLR9901268 = cWTShTfYPRuknLR94309801;     cWTShTfYPRuknLR94309801 = cWTShTfYPRuknLR40036816;     cWTShTfYPRuknLR40036816 = cWTShTfYPRuknLR91969493;     cWTShTfYPRuknLR91969493 = cWTShTfYPRuknLR42204920;     cWTShTfYPRuknLR42204920 = cWTShTfYPRuknLR17862181;     cWTShTfYPRuknLR17862181 = cWTShTfYPRuknLR24607210;     cWTShTfYPRuknLR24607210 = cWTShTfYPRuknLR20137796;     cWTShTfYPRuknLR20137796 = cWTShTfYPRuknLR1703322;     cWTShTfYPRuknLR1703322 = cWTShTfYPRuknLR57140072;     cWTShTfYPRuknLR57140072 = cWTShTfYPRuknLR16863563;     cWTShTfYPRuknLR16863563 = cWTShTfYPRuknLR20721968;     cWTShTfYPRuknLR20721968 = cWTShTfYPRuknLR98535599;     cWTShTfYPRuknLR98535599 = cWTShTfYPRuknLR19741693;     cWTShTfYPRuknLR19741693 = cWTShTfYPRuknLR45372083;     cWTShTfYPRuknLR45372083 = cWTShTfYPRuknLR10563040;     cWTShTfYPRuknLR10563040 = cWTShTfYPRuknLR22031796;     cWTShTfYPRuknLR22031796 = cWTShTfYPRuknLR64931137;     cWTShTfYPRuknLR64931137 = cWTShTfYPRuknLR43347719;     cWTShTfYPRuknLR43347719 = cWTShTfYPRuknLR79612019;     cWTShTfYPRuknLR79612019 = cWTShTfYPRuknLR33268689;     cWTShTfYPRuknLR33268689 = cWTShTfYPRuknLR49416827;     cWTShTfYPRuknLR49416827 = cWTShTfYPRuknLR7341903;     cWTShTfYPRuknLR7341903 = cWTShTfYPRuknLR64809167;     cWTShTfYPRuknLR64809167 = cWTShTfYPRuknLR20786796;     cWTShTfYPRuknLR20786796 = cWTShTfYPRuknLR86972342;     cWTShTfYPRuknLR86972342 = cWTShTfYPRuknLR67545944;     cWTShTfYPRuknLR67545944 = cWTShTfYPRuknLR41735826;     cWTShTfYPRuknLR41735826 = cWTShTfYPRuknLR58906858;     cWTShTfYPRuknLR58906858 = cWTShTfYPRuknLR18836913;     cWTShTfYPRuknLR18836913 = cWTShTfYPRuknLR55892986;     cWTShTfYPRuknLR55892986 = cWTShTfYPRuknLR42615901;     cWTShTfYPRuknLR42615901 = cWTShTfYPRuknLR46695238;     cWTShTfYPRuknLR46695238 = cWTShTfYPRuknLR15315826;     cWTShTfYPRuknLR15315826 = cWTShTfYPRuknLR32106751;     cWTShTfYPRuknLR32106751 = cWTShTfYPRuknLR75256779;     cWTShTfYPRuknLR75256779 = cWTShTfYPRuknLR40557635;     cWTShTfYPRuknLR40557635 = cWTShTfYPRuknLR49471359;     cWTShTfYPRuknLR49471359 = cWTShTfYPRuknLR53140613;     cWTShTfYPRuknLR53140613 = cWTShTfYPRuknLR18867339;     cWTShTfYPRuknLR18867339 = cWTShTfYPRuknLR99609992;     cWTShTfYPRuknLR99609992 = cWTShTfYPRuknLR53684883;     cWTShTfYPRuknLR53684883 = cWTShTfYPRuknLR88846407;     cWTShTfYPRuknLR88846407 = cWTShTfYPRuknLR36581141;     cWTShTfYPRuknLR36581141 = cWTShTfYPRuknLR38104085;     cWTShTfYPRuknLR38104085 = cWTShTfYPRuknLR54936004;     cWTShTfYPRuknLR54936004 = cWTShTfYPRuknLR3563961;     cWTShTfYPRuknLR3563961 = cWTShTfYPRuknLR61567236;     cWTShTfYPRuknLR61567236 = cWTShTfYPRuknLR55116707;     cWTShTfYPRuknLR55116707 = cWTShTfYPRuknLR64954803;     cWTShTfYPRuknLR64954803 = cWTShTfYPRuknLR27333808;     cWTShTfYPRuknLR27333808 = cWTShTfYPRuknLR45163451;     cWTShTfYPRuknLR45163451 = cWTShTfYPRuknLR67333661;     cWTShTfYPRuknLR67333661 = cWTShTfYPRuknLR94282058;     cWTShTfYPRuknLR94282058 = cWTShTfYPRuknLR67580352;     cWTShTfYPRuknLR67580352 = cWTShTfYPRuknLR60484440;     cWTShTfYPRuknLR60484440 = cWTShTfYPRuknLR86967898;     cWTShTfYPRuknLR86967898 = cWTShTfYPRuknLR75227649;     cWTShTfYPRuknLR75227649 = cWTShTfYPRuknLR71182698;     cWTShTfYPRuknLR71182698 = cWTShTfYPRuknLR55232578;     cWTShTfYPRuknLR55232578 = cWTShTfYPRuknLR50316236;     cWTShTfYPRuknLR50316236 = cWTShTfYPRuknLR82871384;     cWTShTfYPRuknLR82871384 = cWTShTfYPRuknLR61230938;     cWTShTfYPRuknLR61230938 = cWTShTfYPRuknLR82866409;     cWTShTfYPRuknLR82866409 = cWTShTfYPRuknLR1247086;     cWTShTfYPRuknLR1247086 = cWTShTfYPRuknLR74247662;     cWTShTfYPRuknLR74247662 = cWTShTfYPRuknLR74026730;     cWTShTfYPRuknLR74026730 = cWTShTfYPRuknLR83219774;     cWTShTfYPRuknLR83219774 = cWTShTfYPRuknLR87634942;     cWTShTfYPRuknLR87634942 = cWTShTfYPRuknLR70115304;     cWTShTfYPRuknLR70115304 = cWTShTfYPRuknLR70005405;     cWTShTfYPRuknLR70005405 = cWTShTfYPRuknLR72560437;     cWTShTfYPRuknLR72560437 = cWTShTfYPRuknLR11790524;     cWTShTfYPRuknLR11790524 = cWTShTfYPRuknLR24480381;     cWTShTfYPRuknLR24480381 = cWTShTfYPRuknLR80002027;     cWTShTfYPRuknLR80002027 = cWTShTfYPRuknLR79583806;     cWTShTfYPRuknLR79583806 = cWTShTfYPRuknLR60570420;     cWTShTfYPRuknLR60570420 = cWTShTfYPRuknLR70760762;     cWTShTfYPRuknLR70760762 = cWTShTfYPRuknLR26705082;     cWTShTfYPRuknLR26705082 = cWTShTfYPRuknLR65850791;     cWTShTfYPRuknLR65850791 = cWTShTfYPRuknLR83408382;     cWTShTfYPRuknLR83408382 = cWTShTfYPRuknLR5978709;     cWTShTfYPRuknLR5978709 = cWTShTfYPRuknLR86619118;     cWTShTfYPRuknLR86619118 = cWTShTfYPRuknLR93952054;     cWTShTfYPRuknLR93952054 = cWTShTfYPRuknLR91503104;     cWTShTfYPRuknLR91503104 = cWTShTfYPRuknLR10729536;     cWTShTfYPRuknLR10729536 = cWTShTfYPRuknLR75282240;     cWTShTfYPRuknLR75282240 = cWTShTfYPRuknLR52413179;     cWTShTfYPRuknLR52413179 = cWTShTfYPRuknLR47735474;     cWTShTfYPRuknLR47735474 = cWTShTfYPRuknLR71622310;     cWTShTfYPRuknLR71622310 = cWTShTfYPRuknLR88288880;     cWTShTfYPRuknLR88288880 = cWTShTfYPRuknLR65329986;     cWTShTfYPRuknLR65329986 = cWTShTfYPRuknLR78288660;     cWTShTfYPRuknLR78288660 = cWTShTfYPRuknLR97908035;     cWTShTfYPRuknLR97908035 = cWTShTfYPRuknLR68551102;     cWTShTfYPRuknLR68551102 = cWTShTfYPRuknLR16738609;     cWTShTfYPRuknLR16738609 = cWTShTfYPRuknLR92453944;     cWTShTfYPRuknLR92453944 = cWTShTfYPRuknLR10681380;}
// Junk Finished
