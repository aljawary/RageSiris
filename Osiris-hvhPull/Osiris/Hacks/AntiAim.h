#pragma once
#include "Aimbot.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/UserCmd.h"
#include "../SDK/Vector.h"
#include "../SDK/WeaponId.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/PhysicsSurfaceProps.h"
#include "../SDK/WeaponData.h"

struct UserCmd;
struct Vector;

namespace AntiAim {
	float RandomFloat(float min, float max) noexcept;
	void setPitch(float pitch, UserCmd* cmd, bool sendPacket) noexcept;
	float getMaxDelta(AnimState* animstate);
	void Resolverp100();
	void indicators() noexcept;
	bool LbyUpdate();
	void setYaw(float yaw, float desyncYaw, UserCmd* cmd, bool sendPacket) noexcept;
	void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
	void run(UserCmd*, const Vector&, const Vector&, bool&) noexcept;
	void fakeWalk(UserCmd* cmd, bool& sendPacket) noexcept;
	void Resolver() noexcept;
}
