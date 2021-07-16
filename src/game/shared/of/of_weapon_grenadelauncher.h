// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFGrenadeLauncher, the Demoman's grenade launcher.
// Author(s): Cherry!
//

#pragma once

#include "of_weapon_base_gun.h"
#include "of_weapon_base_grenadeproj.h"

#ifdef CLIENT_DLL
#define COFGrenadeLauncher C_OFGrenadeLauncher
#endif

class COFGrenadeLauncher : public COFWeaponBaseGun
{
public:
	DECLARE_CLASS(COFGrenadeLauncher,COFWeaponBaseGun);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFGrenadeLauncher();
	~COFGrenadeLauncher();

	virtual void Spawn();
	virtual bool Deploy();
	virtual bool Holster(CBaseCombatWeapon* pSwitchingTo);
	virtual void Misfire();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void WeaponReset();

	virtual void FireFullClipAtOnce();
	//bool CanCharge(); - econ
	//float GetChargeBeginTime(); - defintly for the loose cannon
	//float GetChargeMaxTime(); - defintly for the loose cannon
	//int GetDefaultClip1(); - just the base class
	//int GetMaxClip1(); - just the base class
	//bool IsBlastImpactWeapon(); - econ?
	//bool IsDoubleDonk(); - econ mayhaps :)
	virtual float GetProjectileSpeed() { return 1200.0; } // removed mannpower up stuff
	virtual OFWeaponID GetWeaponID() const { return OF_WEAPON_GRENADELAUNCHER; }
	virtual void LaunchGrenade();
	virtual void PostFire();

private:

	//CNetworkVar(float, m_flDetonateTime);
	//CNetworkVar(int, m_iCurrentTube);
	//CNetworkVar(int, m_iGoalTube);

};