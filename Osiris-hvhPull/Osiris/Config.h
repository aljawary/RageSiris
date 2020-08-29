#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include "SDK/Vector.h"

#include "imgui/imgui.h"
#include "nSkinz/config_.hpp"
#include "ConfigStructs.h"

class Config {
public:
    explicit Config(const char*) noexcept;
    void load(size_t) noexcept;
    void save(size_t) const noexcept;
    void add(const char*) noexcept;
    void remove(size_t) noexcept;
    void rename(size_t, const char*) noexcept;
    void reset() noexcept;
    void listConfigs() noexcept;
    struct Record {
        Vector origin;
        float simulationTime;
        matrix3x4 matrix[128];
    };

    constexpr auto& getConfigs() noexcept
    {
        return configs;
    }

    struct Color {
        float color[3]{ 1.0f, 1.0f, 1.0f };
        bool rainbow{ false };
        float rainbowSpeed{ 0.6f };
    };

    struct ColorToggle : public Color {
        bool enabled{ false };
    };

    struct Aimbot {
        bool enabled{ false };
        bool onKey{ false };
        int key{ 0 };
        int keyMode{ 0 };
        bool aimlock{ false };
        bool silent{ false };
        bool friendlyFire{ false };
        bool visibleOnly{ true };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool autoScope{ false };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float recoilControlX{ 0.0f };
        float recoilControlY{ 0.0f };
        int hitchance{ 0 };
        int minDamage{ 1 };
        int firstShotDelay{ 0 };
        int killDelay{ 0 };
        int shotsFired{ 0 };
        int shotdelay{ 0 };
        bool killshot{ false };
        bool betweenShots{ true };
        bool standaloneRCS{ false };
    };
    std::array<Aimbot, 40> aimbot;

    struct Triggerbot {
        bool enabled = false;
        bool friendlyFire = false;
        bool scopedOnly = true;
        bool hitChance{ true };
        int hitChanceAmmount{ 0 };
        bool ignoreFlash = false;
        bool ignoreSmoke = false;
        bool killshot = false;
        bool onKey = false;
        int key = 0;
        int hitgroup = 0;
        int shotDelay = 0;
        int minDamage = 1;
        float burstTime = 0.0f;
    };
    std::array<Triggerbot, 40> triggerbot;

    struct {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
        int timeLimit{ 200 };
        bool fakeLatency{ false };
        int fakeLatencyAmmount{ 200 };
    } backtrack;

    struct {
        bool enabled{ false };
        int thirdpersonMode{ 0 };
        bool pitch{ false };
        bool yaw{ false };
        bool resolveall{ false };
        bool pitchResolver{ false };
        float pitchAngle{ 0.0f };
        float yawAngle{ 0.0f };
        int yawInverseAngleKey{ 0 };
        int yawInverseKeyMode{ 0 };
        bool yawInversed{ false };
        bool yawReal{ false };
        float bodyLean{ 0.0f };
        int mode{ 0 };
        float jitterMax{ 0.0f };
        float jitterMin{ 0.0f };
        bool LBYBreaker{ false };
        float LBYAngle{ 0.0f };
        struct {
            bool enabled{ false };
            int key{ 0 };
            int keyMode{ 0 };
            bool keyToggled{ 0 };
            int maxChoke{ 3 };
        } fakeWalk;
    } antiAim;

    struct Glow {
        bool enabled{ false };
        bool healthBased{ false };
        float thickness{ 1.0f };
        float alpha{ 1.0f };
        int style{ 0 };
        Color color;
    };
    std::array<Glow, 21> glow;

    struct Chams {
        struct Material : ColorA {
            bool enabled = false;
            bool healthBased = false;
            bool blinking = false;
            bool wireframe = false;
            int material = 0;
        };
        std::vector<Material> materials{ {}, {} };
    };

    std::array<Chams, 20> chams;

    struct Esp {
        struct Shared {
            bool enabled{ false };
            int font{ 0x1d };
            ColorToggle snaplines;
            ColorToggle box;
            int boxType{ 0 };
            ColorToggle name;
            ColorToggle ammo;
            ColorToggle outline{ 0.0f, 0.0f, 0.0f };
            ColorToggle distance;
            float maxDistance{ 0.0f };
        };

        struct Player : public Shared {
            ColorToggle eyeTraces;
            ColorToggle health;
            ColorToggle healthBar;
            ColorToggle armor;
            ColorToggle armorBar;
            ColorToggle money;
            ColorToggle headDot;
            ColorToggle activeWeapon;
            int hpside{ 0 };
            int armorside{ 0 };
            bool deadesp{ false };
        };

        struct Weapon : public Shared { } weapon;

        struct Projectile : public Shared { };
        std::array<Projectile, 9> projectiles;

        struct DangerZone : public Shared { };
        std::array<DangerZone, 18> dangerZone;

        std::array<Player, 6> players;
    } esp;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
        bool noFog{ false };
        bool no3dSky{ false };
        bool noAimPunch{ false };
        bool noViewPunch{ false };
        bool noHands{ false };
        bool noSleeves{ false };
        bool noWeapons{ false };
        bool noSmoke{ false };
        bool noBlur{ false };
        bool noBloom{ false };
        bool noScopeOverlay{ false };
        bool noGrass{ false };
        bool nightMode{ false };
        bool noShadows{ false };
        bool wireframeSmoke{ false };
        bool zoom{ false };
        int zoomKey{ 0 };
        bool thirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
        int viewmodelFov{ 0 };
        int fov{ 0 };
        int farZ{ 0 };
        int flashReduction{ 0 };
        float brightness{ 0.0f };
        int skybox{ 0 };
        std::string customSkybox;
        ColorToggle world;
        ColorToggle sky;
        ColorToggle showvelocity;
        bool showvelocityM{ false };
        int showvelocityPosX{ 0 };
        int showvelocityPosY{ 0 };
        int showvelocityResX;
        int showvelocityResY;
        bool deagleSpinner{ false };
        int screenEffect{ 0 };
        int hitEffect{ 0 };
        float hitEffectTime{ 0.6f };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
        ColorToggle hitMarkerDamageIndicator;
        bool hitMarkerDamageIndicatorCustomize{ false };
        int hitMarkerDamageIndicatorFont{ 31 };
        int hitMarkerDamageIndicatorAlpha{ 800 };
        int hitMarkerDamageIndicatorDist{ 150 };
        int hitMarkerDamageIndicatorTextX{ 60 };
        int hitMarkerDamageIndicatorTextY{ 150 };
        float hitMarkerDamageIndicatorRatio{ 0.0f };
        int playerModelT{ 0 };
        int playerModelCT{ 0 };

        struct {
            bool enabled = false;
            float blue = 0.0f;
            float red = 0.0f;
            float mono = 0.0f;
            float saturation = 0.0f;
            float ghost = 0.0f;
            float green = 0.0f;
            float yellow = 0.0f;
        } colorCorrection;

        bool indicatorsEnabled{ false };
        const char* indicators[4] = {
            "Desync",
            "LBY",
            "Fakelag",
            "Fakeduck"
        };
        bool selectedIndicators[4] = {
            false,
            false,
            false,
            false
        };
        ColorToggle bulletTracers;
    } visuals;

    std::array<item_setting, 36> skinChanger;

    struct {
        int chickenVolume{ 100 };

        struct Player {
            int masterVolume{ 100 };
            int headshotVolume{ 100 };
            int weaponVolume{ 100 };
            int footstepVolume{ 100 };
        };

        std::array<Player, 3> players;
    } sound;

    struct {
        int menuStyle{ 0 };
        int menuColors{ 0 };
    } style;

    struct {
        int menuKey{ 0x2D }; // VK_INSERT
        bool antiAfkKick{ false };
        //bool resolver{ false };
        //bool resolveAll{ false };
        bool autoStrafe{ false };
        int autoStrafeKey{ 0 };
        int autoStrafeStyle{ 0 };
        bool bunnyHop{ false };
        bool usespam{ false };
        bool CustomViewModel{ false };
        float viewmodel_x{ 0 };
        float viewmodel_y{ 0 };
        float viewmodel_z{ 0 };
        bool customClanTag{ false };
        bool clocktag{ false };
        char clanTag[16];
        int tagAnimType{ 0 };
        int playerBlockerKey{ 0 };
        bool playerBlocker{ false };
        bool animatedClanTag{ false };
        bool fastDuck{ false };
        bool fastStop{ false };
        bool moonwalk{ 0 };
        bool edgejump{ false };
        bool jumpbug{ false };
        int jumpbugkey{ 0 };
        bool bombDmg{ false };
        int edgejumpkey{ 0 };
        bool slowwalk{ false };
        int slowwalkKey{ 0 };
        bool sniperCrosshair{ false };
        bool recoilCrosshair{ false };
        bool autoPistol{ false };
        bool autoReload{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool revealRanks{ false };
        bool revealMoney{ false };
        bool revealSuspect{ false };
        bool spectatorList{ false };
        ColorToggle watermark;
        bool fixAnimationLOD{ false };      
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        float aspectratio{ 0 };
        bool killMessage{ false };
        std::string killMessageString{ "Gotcha!" };
        bool nameStealer{ false };
        bool disablePanoramablur{ false };
        int banColor{ 6 };
        std::string banText{ "Cheater has been permanently banned from official CS:GO servers." };
        bool fastPlant{ false };
        ColorToggle bombTimer{ 1.0f, 0.55f, 0.0f };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
        int fakeLagMode{ 0 };
        int fakeLagTicks{ 0 };
        int fakeLagKey{ 0 };
        const char* fakeLagFlags[4] = {
            "While Shooting",
            "While Standing",
            "While Moving",
            "In Air"
        };
        bool fakeLagSelectedFlags[4] = {
            false,
            false,
            false,
            false
        };
        int quickHealthshotKey{ 0 };
        bool nadePredict{ false };
        bool fixTabletSignal{ false };
        float maxAngleDelta{ 255.0f };
        bool fakePrime{ false };
        bool svpurebypass{ false };
        bool drawAimbotFov{ false };
        float actualFov{ 0.0f };
        bool autoZeus{ false };
        bool autoZeusBaimOnly{ false };
        int autoZeusMaxPenDist{ 0 };
        bool fakeDuck{ false };
        int fakeDuckKey{ 0 };
        bool fakeDucking{ false };
        int fakeDuckShotState{ 0 };
        int killSound{ 0 };
        int chat_spammer{ false };
        std::string customKillSound;
        std::string customHitSound;
        PurchaseList purchaseList;
        float wposAimbotX{ 250.0f }; // wpos config .h //
        float wposAimbotY{ 250.0f };
        float wposAntiAimX{ 250.0f };
        float wposAntiAimY{ 250.0f };
        float wposTriggerBotX{ 250.0f };
        float wposTriggerBotY{ 250.0f };
        float wposBacktrackX{ 250.0f };
        float wposBacktrackY{ 250.0f };
        float wposGlowX{ 250.0f };
        float wposGlowY{ 250.0f };
        float wposChamsX{ 250.0f };
        float wposChamsY{ 250.0f };
        float wposEspX{ 250.0f };
        float wposEspY{ 250.0f };
        float wposVisualsX{ 250.0f };
        float wposVisualsY{ 250.0f };
        float wposSkinchangerX{ 250.0f };
        float wposSkinchangerY{ 250.0f };
        float wposSoundX{ 250.0f };
        float wposSoundY{ 250.0f };
        float wposStyleX{ 250.0f };
        float wposStyleY{ 250.0f };
        float wposMiscX{ 250.0f };
        float wposMiscY{ 250.0f };
        float wposReportbotX{ 250.0f };
        float wposReportbotY{ 250.0f };
        float wposConfigX{ 250.0f };
        float wposConfigY{ 250.0f };
        float wposStyle2X{ 250.0f };
        float wposStyle2Y{ 250.0f };
        float wposPurchaseListX{ 250.0f };
        float wposPurchaseListY{ 250.0f };
        float wposSpectatorListX{ 250.0f };
        float wposSpectatorListY{ 250.0f };
        float wposPurchaseListScaleX{ 200.0f };
        float wposPurchaseListScaleY{ 200.0f };
        float wposSpectatorListScaleX{ 200.0f };
        float wposSpectatorListScaleY{ 200.0f };
        const char* wposLockFlags[17] = { // "wpos Locks" //
            "Aimbot", "Anti Aim", "Triggerbot", "Backtrack",
            "Glow", "Chams", "Esp", "Visuals", "Skinchanger",
            "Sound", "Style", "Misc", "Reportbot", "Config",
            "Style2", "PurchaseList","SpectatorList"
        };
        bool wposLockSelectedFlags[17] = {
            false, false, false, false,
            false, false, false, false,
            false, false, false, false,
            false, false, false, false,
            false
        };
    } misc;

    struct {
        bool enabled{ false };
        bool textAbuse{ false };
        bool griefing{ false };
        bool wallhack{ true };
        bool aimbot{ true };
        bool other{ true };
        int target{ 0 };
        int delay{ 1 };
        int rounds{ 1 };
    } reportbot;

    struct {
        bool thirdPersonAnglesSet{ false };
        Vector fakeAngle;
        Vector realAngle;
        Vector cmdAngle;
        Record serverPos;
        float serverTime{ 0.f };
        int tickRate{ 0 };
        float nextLBY{ 0.f };
        float lastLBY{ 0.f };
        int chokedPackets{ 0 };
        bool sendPacket{ false };
    } globals;
private:
    std::filesystem::path path;
    std::vector<std::string> configs;
};

inline std::unique_ptr<Config> config;
