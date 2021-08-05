// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFWeaponBaseMelee, bonk.
// Author(s): Cherry!
//

#pragma once

#ifdef CLIENT_DLL
	#define COFWeaponBaseMelee C_OFWeaponBaseMelee
#endif

class COFWeaponBaseMelee : public COFWeaponBase
{
public:
	DECLARE_CLASS(COFWeaponBaseMelee, COFWeaponBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFWeaponBaseMelee();
	~COFWeaponBaseMelee();

	virtual void Spawn();
	virtual bool Holster(CBaseCombatWeapon *pSwitchingTo);
	virtual void WeaponReset();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual void Swing(COFPlayer *pPlayer);
	virtual void Smack();

	virtual void ItemPostFrame();
	virtual bool CalcIsAttackCriticalHelper();

	virtual bool DoSwingTraceInternal(trace_t &trace);
	virtual void OnSwingHit(trace_t &trace);
	virtual void DoMeleeDamage(CBaseEntity *pEntity, trace_t &trace);

	virtual void PlayHitSound(trace_t &trace) { WeaponSound(trace.m_pEnt->IsPlayer() ? MELEE_HIT : MELEE_HIT_WORLD); }
	virtual void PlaySwingSound() { WeaponSound(m_bAttackCritical ? BURST : MELEE_MISS); }
	virtual void DoViewModelAnimation();
	virtual bool CanBeSelected() { return true; }
	virtual OFWeaponID GetWeaponID() { return WEAPON_NONE; }
	virtual float GetMeleeDamage(CBaseEntity *param_1, int *param_2, int *param3); // are these really pointers?
	virtual float GetSmackTime(int iWeaponMode) { return GetOFWpnData().m_WeaponModeInfo[iWeaponMode].m_flSmackDelay; }
	virtual int GetSwingRange() { return 48; } // removed sword checks
	virtual int GetDamageCustom() { return 0; } // OFTODO: figure out the values
	virtual float GetForceScale();
	virtual bool CanCritFromBehind() { return false; } // OFTODO: when attributes get added, add a check here
	virtual bool HasPrimaryAmmo() { return true; }
	virtual void OnEntityHit(CBaseEntity *pEntity, CTakeDamageInfo *damageInfo) {}
	virtual void SendPlayerAnimEvent(COFPlayer *pPlayer);

private:

	float m_flSmackTime;
	bool field_0x7e4;
};