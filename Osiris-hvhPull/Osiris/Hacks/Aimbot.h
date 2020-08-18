#pragma once

struct UserCmd;
struct Vector;
class GameEvent;

namespace Aimbot {
    Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
	void handleKill(GameEvent& event) noexcept;
	void autoZeus(UserCmd* cmd) noexcept;
	void run(UserCmd*) noexcept;
    static float lastKillTime{ 0 };

    struct Enemies
    {
        int id;
        int health;
        float distance;

        bool operator<(const Enemies& enemy) const noexcept
        {
            if (health != enemy.health)
                return health < enemy.health;
            return distance < enemy.distance;
        }
        Enemies(int id, int health, float distance) noexcept : id(id), health(health), distance(distance) { }
    };
}