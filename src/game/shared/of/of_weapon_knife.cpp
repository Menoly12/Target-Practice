// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "May I borrow your earpiece? This is Scout! Rainbows make me cry! Over!"
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_base.h"
#include "of_weapon_base_melee.h"
#include "of_weapon_knife.h"

#ifdef CLIENT_DLL
	#include "c_of_player.h"
#else
	#include "of_player.h"
	#include "ilagcompensationmanager.h"
#endif

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFKnife, DT_OFWeaponKnife)

BEGIN_NETWORK_TABLE(COFKnife, DT_OFWeaponKnife)
#ifdef CLIENT_DLL
	RecvPropBool(RECVINFO(m_bReadyToBackstab)),
#else
	SendPropBool(SENDINFO(m_bReadyToBackstab)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(COFKnife)
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
BEGIN_DATADESC(COFKnife)
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_knife, COFKnife);

COFKnife::COFKnife()
{
	m_bReadyToBackstab = false;
}

COFKnife::~COFKnife()
{
}

void COFKnife::PrimaryAttack()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	if (!CanAttack()) return;

	m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;
	m_pBackstabTarget = NULL; //field_0x804 = 0xffffffff;

	#ifdef GAME_DLL
	lagcompensation->StartLagCompensation(pPlayer, pPlayer->GetCurrentCommand());
	#endif

	trace_t trace;
	bool bHit = DoSwingTraceInternal(trace);

	COFPlayer *pEnemy = ToOFPlayer(trace.m_pEnt);
	if (bHit && pEnemy)
	{
		if (pEnemy->GetTeamNumber() != pPlayer->GetTeamNumber())
		{
			int iTargetDamage = 0;
			if (CanPerformBackstabAgainstTarget(pEnemy))
			{
				m_pBackstabTarget = pEnemy;
				iTargetDamage = Max(pEnemy->GetHealth(), 75);

				// mannpower beyond here
			}
		}
	}

	//pPlayer->RemoveInvisibility();

	#ifdef GAME_DLL
	lagcompensation->FinishLagCompensation(pPlayer);
	#endif

	Swing(pPlayer);
	Smack();

	m_flSmackTime = -1.0;
	m_bReadyToBackstab = false;

	//pPlayer->SpeakWeaponFire(-1.0);
	//CTFGameStats::Event_PlayerFiredWeapon((CTFGameStats *)PTR__CTF_GameStats_00e340b8, (CTFPlayer *)this_01, (bool)this[0x6ba]);

	// beyond here is just stuff for the eternal reward
}

void COFKnife::SecondaryAttack()
{
	if (m_bInAttack2) return;

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	//pPlayer->DoClassSpecialSkill();
	m_bInAttack2 = true;

	m_flNextSecondaryAttack = gpGlobals->curtime + GetNextSecondaryAttackDelay();
}

void COFKnife::ItemPostFrame()
{
	BackstabVMThink();
	BaseClass::ItemPostFrame();
}

bool COFKnife::CalcIsAttackCriticalHelper()
{
	return (m_pBackstabTarget ? true : false);
}

float COFKnife::GetMeleeDamage(CBaseEntity *pTarget, int *iDamageType, int *iDamageCustom)
{
	float flDamage = BaseClass::GetMeleeDamage(pTarget, iDamageType, iDamageCustom);

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return 0.0;

	if (pTarget->IsPlayer())
	{
		// removed mvm checks
		if (m_pBackstabTarget)
		{
			COFPlayer *pOFTarget = ToOFPlayer(pTarget);
			if (pOFTarget) flDamage = float(pTarget->GetHealth() * 2);

			*iDamageCustom = OF_CUSTOMDAMAGE_BACKSTAB;
		}
		//else if (pPlayer->IsCritBoosted()) { m_bAttackCritical = true; }
		else
		{
			m_bAttackCritical = false;
		}
	}

	return flDamage;
}

void COFKnife::SendPlayerAnimEvent(COFPlayer *pPlayer)
{
	if (m_pBackstabTarget)
	{
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_CUSTOM_GESTURE, ACT_MP_ATTACK_STAND_SECONDARYFIRE);
	}
	else
	{ 
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
	}
}

bool COFKnife::SendWeaponAnim(int iActivity)
{
	if (GetOFPlayerOwner())
	{
		if (m_bReadyToBackstab &&
			(iActivity == ACT_VM_IDLE || iActivity == ACT_ITEM1_VM_IDLE || iActivity == ACT_ITEM2_VM_IDLE))
		{
			iActivity = ACT_BACKSTAB_VM_IDLE;
		}
	}

	return BaseClass::SendWeaponAnim(iActivity);
}

void COFKnife::BackstabVMThink()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	// ??
	//COFWeaponBase *pWeapon = pPlayer->GetActiveOFWeapon();
	//if (pWeapon != this) return;

	Activity act = GetActivity();
	// these values are obviously broken so here's my shot at it
	//ACT_ITEM1_BACKSTAB_VM_UP;
	//ACT_VM_IDLE;
	ACT_MELEE_ALLCLASS_VM_IDLE_LOWERED; //1719
	//ACT_ITEM3_VM_RELOAD; // 1630
	if (act != ACT_VM_IDLE && act != ACT_BACKSTAB_VM_IDLE &&
		act != ACT_ITEM1_VM_IDLE && act != ACT_ITEM1_BACKSTAB_VM_IDLE &&
		act != ACT_ITEM2_VM_IDLE && act != ACT_ITEM2_BACKSTAB_VM_IDLE) return;

	trace_t trace;
	if (DoSwingTraceInternal(trace) && CanAttack())
	{
		// removed a double IsPlayer check
		COFPlayer *pTarget = ToOFPlayer(trace.m_pEnt);
		if (!pTarget) return;

		if (pPlayer->GetTeamNumber() == pTarget->GetTeamNumber()) return;

		bool bCanStab = CanPerformBackstabAgainstTarget(pTarget);

		// may i stab this idiot? yes!
		if (bCanStab)
		{
			// we havent told the vm to raise the knife, do it!
			if (!m_bReadyToBackstab)
			{
				SendWeaponAnim(ACT_BACKSTAB_VM_UP); // ACT_MELEE_ALLCLASS_VM_IDLE_TO_LOWERED probably not right??
				m_bReadyToBackstab = true;
			}
		}
		// i may not stab this idiot.. so return to normal vm idle
		else if (m_bReadyToBackstab)
		{
			SendWeaponAnim(ACT_BACKSTAB_VM_DOWN);
			m_bReadyToBackstab = false;
		}
	}
	else if (m_bReadyToBackstab)
	{
		SendWeaponAnim(ACT_BACKSTAB_VM_DOWN);
		m_bReadyToBackstab = false;
	}
}

bool COFKnife::CanPerformBackstabAgainstTarget(COFPlayer *pTarget)
{
	// attribute "cannot_be_backstabbed"

	if (!pTarget) return false;

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return false;

	// 2401 = 0x961 - gamerules mann power check

	return IsBehindAndFacingTarget(pTarget);

	// mvm checks
}

bool COFKnife::IsBehindAndFacingTarget(COFPlayer *pTarget)
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return false;

	Vector vecTarget = pTarget->WorldSpaceCenter();
	Vector vecPlayer = pPlayer->WorldSpaceCenter();

	Vector vecDistance = vecTarget - vecPlayer;
	vecDistance.Normalized();
	vecDistance.z = 0;

	//QAngle angleEye = pPlayer->EyeAngles();
	Vector vecPlayerEyeAngles;
	AngleVectors(pPlayer->EyeAngles(), &vecPlayerEyeAngles, NULL, NULL);
	vecPlayerEyeAngles.Normalized();
	vecPlayerEyeAngles.z = 0;

	Vector vecTargetEyeAngles;
	AngleVectors(pTarget->EyeAngles(), &vecTargetEyeAngles, NULL, NULL);
	vecTargetEyeAngles.Normalized();
	vecTargetEyeAngles.z = 0;

	return (0.0 < DotProduct(vecDistance, vecTargetEyeAngles) && 0.5 < DotProduct(vecPlayerEyeAngles, vecDistance) && -0.3 < DotProduct(vecPlayerEyeAngles, vecTargetEyeAngles));
}