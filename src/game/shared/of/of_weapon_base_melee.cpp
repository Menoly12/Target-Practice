// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFWeaponBaseMelee, bonk.
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_base.h"
#include "of_weapon_base_melee.h"

#ifdef CLIENT_DLL

#include "c_of_player.h"

#else

#include "of_player.h"
#include "ilagcompensationmanager.h"

#endif

ConVar tf_weapon_criticals_melee("tf_weapon_criticals_melee", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Controls random crits for melee weapons. 0 - Melee weapons do not randomly crit. 1 - Melee weapons can randomly crit only if tf_weapon_criticals is also enabled. 2 - Melee weapons can always randomly crit regardless of the tf_weapon_criticals setting.");
ConVar tf_meleeattackforcescale("tf_meleeattackforcescale", "80.0", FCVAR_NOTIFY | FCVAR_REPLICATED, "Bonk!");
extern ConVar tf_weapon_criticals;

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFWeaponBaseMelee, DT_OFWeaponBaseMelee)

BEGIN_NETWORK_TABLE(COFWeaponBaseMelee, DT_OFWeaponBaseMelee)
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(COFWeaponBaseMelee)
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
BEGIN_DATADESC(COFWeaponBaseMelee)
DEFINE_THINKFUNC(Smack)
END_DATADESC()
#endif

COFWeaponBaseMelee::COFWeaponBaseMelee()
{
	WeaponReset();
}

COFWeaponBaseMelee::~COFWeaponBaseMelee()
{
}

void COFWeaponBaseMelee::Spawn()
{
	Precache();

	FileWeaponInfo_t *pFileWeaponInfo = GetFileWeaponInfoFromHandle(LookupWeaponInfoSlot(CBaseEntity::GetClassname()));
	if (pFileWeaponInfo)
	{
		COFWeaponInfo *pWeaponInfo = dynamic_cast<COFWeaponInfo*>(pFileWeaponInfo);
		m_pWeaponInfo = pWeaponInfo;
	}

	m_iClip1 = -1;

	BaseClass::Spawn();
}

bool COFWeaponBaseMelee::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	m_flSmackTime = -1.0;

	COFPlayer *pPlayer = GetOFPlayerOwner();

	if (pPlayer)
	{
		pPlayer->m_flNextAttack = gpGlobals->curtime + 0.5;
	}

	// condition "self_mark_for_death"

	return BaseClass::Holster(pSwitchingTo);
}

void COFWeaponBaseMelee::WeaponReset()
{
	BaseClass::WeaponReset();

	m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;
	m_flSmackTime = -1.0;
	field_0x7e4 = false;
}

void COFWeaponBaseMelee::PrimaryAttack()
{
	if (!CanAttack()) return;

	COFPlayer *pPlayer = GetOFPlayerOwner();

	if (!pPlayer) return;

	m_iWeaponMode = OF_WEAPON_MODE_PRIMARY;
	field_0x7e4 = false;

	//pPlayer->EndClassSpecialSkill();

	Swing(pPlayer);

	//iVar1 = this_00->field_0x1d84;
	//this[0x6bc] = (CTFWeaponBaseMelee)(iVar1 != 0);
	//this[0x7e5] = (CTFWeaponBaseMelee)(iVar1 == 1);

	#ifdef GAME_DLL

	//pPlayer->SpeakWeaponFire(-1);

	//CTFGameStats::Event_PlayerFiredWeapon((CTFGameStats *)PTR__CTF_GameStats_00e340b8,this_00,(bool)this[0x6ba]);

	//if (pPlayer->m_Shared.IsStealthed() && ShouldRemoveInvisibilityOnPrimaryAttack())
	//{
		//pPlayer->RemoveInvisibility();
	//}

	#endif

	//pPlayer->m_Shared.OnAttack();
}

void COFWeaponBaseMelee::SecondaryAttack()
{
	if (!CanAttack()) return;

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	//pPlayer->DoClassSpecialSkill();

	m_bInAttack2 = true;

	m_flNextSecondaryAttack = gpGlobals->curtime + GetNextSecondaryAttackDelay();
}

void COFWeaponBaseMelee::Swing(COFPlayer *pPlayer)
{
	CalcIsAttackCritical();

	//CTFGameStats::Event_PlayerFiredWeapon((CTFGameStats *)PTR__CTF_GameStats_00e340b8, param_1, (bool)this[0x6ba]);
	//C_CTFGameStats::Event_PlayerFiredWeapon((C_CTFGameStats *)PTR__C_CTF_GameStats_00f8a6ec, param_1, (bool)this[0xb16]);

	SendPlayerAnimEvent(pPlayer);
	DoViewModelAnimation();

	//ApplyFireDelay()

	float flDelay = gpGlobals->curtime + GetOFWpnData().m_WeaponModeInfo[m_iWeaponMode].m_flTimeFireDelay;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = flDelay;

	// leads to mShared...
	//if ((float)param_1->field_0x1c7c != m_flNextPrimaryAttack) {
	//	(**(code **)(param_1->field_0x19b8 + 0x10))();
	//	param_1->field_0x1c7c = m_flNextPrimaryAttack;
	//}

	SetWeaponIdleTime(flDelay + GetOFWpnData().m_WeaponModeInfo[m_iWeaponMode].m_flTimeIdleEmpty);

	// For the big CollectPlayers chunk
	// its just for the boston basher!

	PlaySwingSound();

	m_flSmackTime = gpGlobals->curtime + GetSmackTime(m_iWeaponMode);
}

void COFWeaponBaseMelee::Smack()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	trace_t trace;

	#ifdef GAME_DLL

	lagcompensation->StartLagCompensation(pPlayer, pPlayer->GetCurrentCommand());

	#endif

	if (DoSwingTraceInternal(trace))
	{
		OnSwingHit(trace);
	}

	#ifdef GAME_DLL

	lagcompensation->FinishLagCompensation(pPlayer);

	#endif
}

// FILLED with junk, inlined, and optimized to hell, its the full RE package
bool COFWeaponBaseMelee::DoSwingTraceInternal(trace_t &trace)
{
	static Vector vecSwingMinsBase(-18, -18, -18);
	static Vector vecSwingMaxsBase(18, 18, 18);

	// "melee_bounds_multiplier"

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return false;

	int iSwingRange = GetSwingRange();
	QAngle angle = pPlayer->EyeAngles();

	Vector vForward;
	AngleVectors(angle, &vForward);
	Vector vBeginPos = pPlayer->Weapon_ShootPosition();
	Vector vEndPos = vBeginPos + vForward * iSwingRange;

	UTIL_TraceLine(vBeginPos, vEndPos, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &trace);
	if (trace.fraction >= 1.0)
	{
		UTIL_TraceHull(vBeginPos, vEndPos, vecSwingMinsBase, vecSwingMaxsBase, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &trace);
		if (trace.fraction < 1.0)
		{
			//CBaseEntity *pTarget = trace.m_pEnt;
			//if (!pTarget || pTarget->IsBSPModel())
			//{
			//	FindHullIntersection() // OFTODO: aaaaaaa
			//}

			vEndPos = trace.endpos;
		}
	}

	return (trace.fraction < 1.0);
}

void COFWeaponBaseMelee::OnSwingHit(trace_t &trace)
{
	PlayHitSound(trace);
	DoMeleeDamage(trace.m_pEnt, trace);
}

void COFWeaponBaseMelee::DoMeleeDamage(CBaseEntity *pEntity, trace_t &trace)
{
	COFPlayer *pPlayer = GetOFPlayerOwner(); // this_00
	if (!pPlayer) return;

	pPlayer->Weapon_ShootPosition();

	Vector vForward;
	AngleVectors(pPlayer->EyeAngles(), &vForward);
	Vector vBeginPos = pPlayer->Weapon_ShootPosition();
	Vector vEndPos = vBeginPos + vForward * 48; // wouldnt we wanna call GetSwingRange()?

	#ifdef GAME_DLL

	int iDamageType = DMG_CLUB | DMG_NEVERGIB; // local_44
	int iDamageCustom = GetDamageCustom(); // local_40

	// attribute "crit_from_behind"

	if (CanCritFromBehind())
	{
		Vector entityForward;
		AngleVectors(pEntity->EyeAngles(), &entityForward);

		Vector entityPos = pEntity->GetAbsOrigin() - pPlayer->GetAbsOrigin();

		if (DotProduct(entityPos, entityForward) > 0.7071)
		{
			iDamageType |= DMG_CRIT;
		}
	}

	float flDamage = GetMeleeDamage(pEntity, &iDamageType, &iDamageCustom); // fVar14

	if (IsAttackCritical())
	{
		iDamageType |= DMG_CRIT;
	}
	//else {
	//	local_44 = local_44 | 0x100000;
	//}

	CTakeDamageInfo info(pPlayer, pPlayer, this, flDamage, iDamageType, iDamageCustom); // local_bc

	if (flDamage >= 1.0)
	{
		CalculateMeleeDamageForce(&info, vForward, vEndPos, (1.0 / flDamage) * GetForceScale());
	}
	else
	{
		info.SetDamageForce(vec3_origin);
	}

	pEntity->DispatchTraceAttack(info, vForward, &trace);
	ApplyMultiDamage();

	OnEntityHit(pEntity, &info);

	// econ and mann up beyond here

	#endif

	if (pEntity && pEntity->GetTeamNumber() != pPlayer->GetTeamNumber())
	{
		#ifdef CLIENT_DLL
		UTIL_ImpactTrace(&trace, DMG_CLUB);
		#endif

		field_0x7e4 = true;
	}
}

bool COFWeaponBaseMelee::CalcIsAttackCriticalHelper()
{
	if (tf_weapon_criticals_melee.GetInt() == 0) return false; // CalcIsAttackCriticalHelperNoCrits doesnt exist for us soooo yeah :)

	if (tf_weapon_criticals_melee.GetInt() == 1 && !tf_weapon_criticals.GetBool()) return false;

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return false;

	if (!CanFireCriticalShot()) return false;

	//if (pPlayer->m_Shared.IsCritBoosted()) return true;

	// cutting attributes

	// just to keep things randomized
	// note: huh, player index gets a bonus 16 bit shift for melee weapons
	int iSeed = (pPlayer->entindex() << 16 | entindex() << 8) ^ GetPredictionRandomSeed();
	if (iSeed != m_iCritSeed)
	{
		m_iCritSeed = iSeed;
		RandomSeed(iSeed);
	}

	float flCritMult = pPlayer->GetCritMult();
	int iRandom = 0;

	iRandom = RandomInt(0, OF_WEAPON_RANDOM_RANGE - 1);
	return iRandom < (OF_WEAPON_CRIT_CHANCE_MELEE * flCritMult) * OF_WEAPON_RANDOM_RANGE;
}

void COFWeaponBaseMelee::ItemPostFrame()
{
	if (m_flSmackTime > 0.0 && m_flSmackTime < gpGlobals->curtime)
	{
		m_flSmackTime = -1.0;
		Smack();

		/*
		if ((iVar1 != 0) && (*(int *)(&DAT_00001d84 + iVar1) != 0)) {
		(**(code **)(*(int *)(iVar1 + 0x19b8) + 0x10))(iVar1 + 0x19b8,&DAT_00001d84 + iVar1);
		*(undefined4 *)(&DAT_00001d84 + iVar1) = 0;
		}
		*/
	}

	BaseClass::ItemPostFrame();
}

void COFWeaponBaseMelee::DoViewModelAnimation()
{
	if (IsAttackCritical())
	{
		if (SendWeaponAnim(ACT_VM_SWINGHARD)) return;
	}

	SendWeaponAnim(m_iWeaponMode == OF_WEAPON_MODE_PRIMARY ? ACT_VM_HITCENTER : ACT_VM_SWINGHARD);
}

void COFWeaponBaseMelee::SendPlayerAnimEvent(COFPlayer *pPlayer)
{
	pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
}

// attributes! all cut!
float COFWeaponBaseMelee::GetMeleeDamage(CBaseEntity *pTarget, int *iDamageType, int *iDamageCustom)
{
	return GetOFWpnData().m_WeaponModeInfo[m_iWeaponMode].m_iDamage;
}

float COFWeaponBaseMelee::GetForceScale()
{
	return tf_meleeattackforcescale.GetFloat();
}