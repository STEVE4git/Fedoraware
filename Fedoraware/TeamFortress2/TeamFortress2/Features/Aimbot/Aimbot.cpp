#include "Aimbot.h"
#include "../Vars.h"

#include "AimbotHitscan/AimbotHitscan.h"
#include "AimbotProjectile/AimbotProjectile.h"
#include "AimbotMelee/AimbotMelee.h"

bool CAimbot::ShouldRun(CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	if (G::m_bFreecamActive)
		return false;

	if (!Vars::Aimbot::Global::Active.m_Var)
		return false;

	if (I::EngineVGui->IsGameUIVisible() || I::Surface->IsCursorVisible())
		return false;

	if (!pLocal->IsAlive()
		|| pLocal->IsTaunting()
		|| pLocal->IsBonked()
		|| pLocal->GetFeignDeathReady()
		|| pLocal->IsCloaked()
		|| pLocal->IsInBumperKart()
		|| pLocal->IsAGhost())
		return false;

	switch (G::m_nCurItemDefIndex)
	{
	case Soldier_m_RocketJumper:
	case Demoman_s_StickyJumper: return false;
	default: break;
	}

	switch (pWeapon->GetWeaponID())
	{
	case TF_WEAPON_PDA:
	case TF_WEAPON_PDA_ENGINEER_BUILD:
	case TF_WEAPON_PDA_ENGINEER_DESTROY:
	case TF_WEAPON_PDA_SPY:
	case TF_WEAPON_PDA_SPY_BUILD:
	case TF_WEAPON_BUILDER:
	case TF_WEAPON_INVIS:
	case TF_WEAPON_BUFF_ITEM:
	case TF_WEAPON_GRAPPLINGHOOK:
		{
			return false;
		}

	default: break;
	}

	return true;
}

void CAimbot::Run(CUserCmd* pCmd)
{
	G::m_nCurrentTargetIdx = 0;
	G::m_flCurAimFOV = 0.0f;
	G::m_vPredictedPos = Vec3();
	G::m_bHitscanRunning = false;
	G::m_bHitscanSilentActive = false;
	G::m_bProjectileSilentActive = false;
	G::m_vAimPos = Vec3();

	auto pLocal = I::EntityList->GetClientEntity(I::Engine->GetLocalPlayer());

	if (pLocal)
	{
		auto pWeapon = pLocal->GetActiveWeapon();

		if (!pWeapon)
		{
			return;
		}

		if (!ShouldRun(pLocal, pWeapon))
			return;

		SandvichAimbot::IsSandvich();
		if (SandvichAimbot::bIsSandvich) {
			G::m_WeaponType = EWeaponType::HITSCAN;
		}

		switch (G::m_WeaponType)
		{
		case EWeaponType::HITSCAN:
			{
				g_AimbotHitscan.Run(pLocal, pWeapon, pCmd);
				break;
			}

		case EWeaponType::PROJECTILE:
			{
				g_AimbotProjectile.Run(pLocal, pWeapon, pCmd);
				break;
			}

		case EWeaponType::MELEE:
			{
				g_AimbotMelee.Run(pLocal, pWeapon, pCmd);
				break;
			}

		default: break;
		}
	}
}
