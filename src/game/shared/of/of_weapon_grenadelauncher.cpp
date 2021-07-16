// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFGrenadeLauncher, the Demoman's grenade launcher.
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_grenadelauncher.h"
#include "of_weapon_grenadeproj_pipebomb.h"

#ifdef CLIENT_DLL
	#include "c_of_player.h"
#else
	#include "of_player.h"
#endif

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFGrenadeLauncher, DT_WeaponGrenadeLauncher)

BEGIN_NETWORK_TABLE(COFGrenadeLauncher, DT_WeaponGrenadeLauncher)
#ifdef CLIENT_DLL
	//RecvPropFloat(RECVINFO(m_flDetonateTime)),
	//RecvPropInt(RECVINFO(m_iCurrentTube)),
	//RecvPropInt(RECVINFO(m_iGoalTube)),
#else
	//SendPropFloat(SENDINFO(m_flDetonateTime)),
	//SendPropInt(SENDINFO(m_iCurrentTube)),
	//SendPropInt(SENDINFO(m_iGoalTube)),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(COFGrenadeLauncher)
END_PREDICTION_DATA()

#ifdef GAME_DLL
	BEGIN_DATADESC(COFGrenadeLauncher)
	END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_grenadelauncher, COFGrenadeLauncher);

COFGrenadeLauncher::COFGrenadeLauncher()
{
	m_bReloadsSingly = true;
}

COFGrenadeLauncher::~COFGrenadeLauncher()
{
}

void COFGrenadeLauncher::Spawn()
{
	//field_0x6cc = 21;
	BaseClass::Spawn();
	//ResetDetonateTime();
}

bool COFGrenadeLauncher::Deploy()
{
	//ResetDetonateTime();
	return BaseClass::Deploy();
}

bool COFGrenadeLauncher::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	//ResetDetonateTime();
	return BaseClass::Holster(pSwitchingTo);
}

void COFGrenadeLauncher::Misfire()
{
	BaseClass::Misfire();
	LaunchGrenade();
}

void COFGrenadeLauncher::PrimaryAttack()
{
	if (m_iClip1 <= 0 && m_iClip1 != -1) return;

	if (gpGlobals->curtime < m_flNextPrimaryAttack) return;

	if (!CanAttack()) return; //ResetDetonateTime();

	m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;

	LaunchGrenade();

	// loose cannon stuff
}

void COFGrenadeLauncher::SecondaryAttack()
{
#ifdef GAME_DLL
	if (CanAttack())
	{
		COFPlayer *pPlayer = GetOFPlayerOwner();
		if (pPlayer)
		{
			//pPlayer->DoClassSpecialSkill();
		}
	}
#endif
}

void COFGrenadeLauncher::WeaponReset()
{
	BaseClass::WeaponReset();
	//ResetDetonateTime();

	//m_iCurrentTube = 0;
	//m_iGoalTube = 0;
	// 0x7fc = true;
}

void COFGrenadeLauncher::FireFullClipAtOnce()
{
	m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;
	LaunchGrenade();
}

void COFGrenadeLauncher::LaunchGrenade()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	CalcIsAttackCritical();

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	pPlayer->SetAnimation(PLAYER_ATTACK1);
	pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);

	//if (AutoFiresFullClipAtOnce()) // what would this be for..
	//	uVar2 = *(uint *)PTR_m_nPredictionRandomSeed_00f8a1d8;
	//	puVar4 = (undefined8 *)C_BasePlayer::GetPunchAngle((C_BasePlayer *)this_00);

	FireProjectile(pPlayer);

	#ifdef GAME_DLL

	//pPlayer->SpeakWeaponFire(-1);

	#endif

	PostFire();
}

void COFGrenadeLauncher::PostFire()
{
	// ApplyFireDelay is just a bunch of mannpower, attribute, and econ weapon stuff - cherry
	m_flNextPrimaryAttack = gpGlobals->curtime + GetOFWpnData().m_WeaponModeInfo[m_iWeaponMode].m_flTimeFireDelay;

	SetWeaponIdleTime(gpGlobals->curtime + SequenceDuration());

	if (m_bReloadsSingly)
	{
		m_iReloadStage.Set(OF_RELOAD_STAGE_NONE);
	}

	// loose canon stuff

	//ResetDetonateTime();
}

/* gonna take a gamble and say this is all for econ weapons as it's filled with attributes
#ifdef GAME_DLL
void COFGrenadeLauncher::FireProjectileInternal(COFPlayer *pPlayer)
{
	COFWeaponBaseGrenadeProj *pProjectile = dynamic_cast<COFWeaponBaseGrenadeProj*>(FireProjectile(pPlayer));
	if (pProjectile)
	{
		//detonate mode? econ likely
	}
}
#endif
*/