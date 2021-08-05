// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "Cry some more!"
// Author(s): Cherry!
//

#pragma once

#include "of_weapon_base_gun.h"

#ifdef CLIENT_DLL
	#define COFMinigun C_OFMinigun
#endif

class COFMinigun : public COFWeaponBaseGun
{
public:
	DECLARE_CLASS(COFMinigun, COFWeaponBaseGun);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFMinigun();
	~COFMinigun();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual bool Lower();
	virtual bool CanHolster() const;
	virtual bool CanReload() { return false; }
	virtual void FireGameEvent(IGameEvent *event) { return; }
	virtual int GetCustomDamageType() const { return 5; }
	// GetDamageAccumulator()
	// GetProjectileDamage();
	virtual OFWeaponID GetWeaponID() const { return OF_WEAPON_MINIGUN; }
	virtual bool ShouldDoMuzzleFlash() { return false; }
	virtual void PlayWeaponShootSound() {}
	// virtual float GetWeaponSpread();
	virtual void HandleFireOnEmpty();
	virtual void SetWeaponState(MinigunState_t iState);
	virtual bool SendWeaponAnim(int iActivity);
	virtual void SharedAttack();

	#ifdef GAME_DLL
	virtual int UpdateTransmitState() { return SetTransmitState(FL_EDICT_ALWAYS); }
	#else

	virtual void StartBrassEffect();
	virtual void StopBrassEffect();

	virtual void StartMuzzleEffect();
	virtual void StopMuzzleEffect();

	virtual void OnDataChanged(DataUpdateType_t updateType);
	virtual void ItemPreFrame();
	virtual void ItemPostFrame();
	virtual void SetDormant(bool bDormant);
	virtual void WeaponSoundUpdate();

	virtual CStudioHdr *OnNewModel();
	virtual void StandardBlendingRules(CStudioHdr *hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask);

	virtual void UpdateBarrelSpin();

	#endif

	virtual void SpinUp();
	virtual void SpinDown();
	virtual void WeaponIdle();
	virtual void WeaponReset();
	

private:

	CNetworkVar(int, m_iWeaponState);
	CNetworkVar(bool, m_bCritShot);
	float field_0x874;
	float m_flLastStartSpinupTime;
	float field_0x870;

	#ifdef CLIENT_DLL

	float m_flBarrelSpinSpeed;
	float m_flBarrelSpinTarget; // field_0x880
	float m_flBarrelAngle; // also 0xc44 in client

	int field_0xc3c;
	int m_iCurrentSound;
	CSoundPatch *m_pCurrentSound;

	int field_0xc40;

	CNewParticleEffect *m_pBrassEffect;
	EHANDLE m_hBrassWeapon;
	int m_iBrassAttachPoint;

	CNewParticleEffect *m_pMuzzleEffect;
	EHANDLE m_hMuzzleWeapon; // field_0xc70
	int m_iMuzzleAttachPoint; // field_0xc78

	#endif

};