// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: May I borrow your earpiece? This is Scout! Rainbows make me cry! Over!
// Author(s): Cherry!
//

#pragma once

#ifdef CLIENT_DLL
	#define COFKnife C_OFKnife
#endif

class COFKnife : public COFWeaponBaseMelee
{
public:
	DECLARE_CLASS(COFKnife, COFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFKnife();
	~COFKnife();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual void ItemPostFrame();

	virtual OFWeaponID GetWeaponID() { return OF_WEAPON_KNIFE; }
	virtual bool CalcIsAttackCriticalHelper();
	virtual float GetMeleeDamage(CBaseEntity *pTarget, int *iDamageType, int *iDamageCustom);

	virtual void BackstabVMThink();
	virtual void SendPlayerAnimEvent(COFPlayer *pPlayer);
	virtual bool SendWeaponAnim(int iActivity);

	virtual bool CanPerformBackstabAgainstTarget(COFPlayer *pTarget);
	virtual bool IsBehindAndFacingTarget(COFPlayer *pTarget);
	//virtual void ResetVars(); // just for spycicle

private:

	CNetworkVar(bool, m_bReadyToBackstab);
	COFPlayer *m_pBackstabTarget; // 0x804
};