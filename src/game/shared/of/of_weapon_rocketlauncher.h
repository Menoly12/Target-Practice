// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "I am not trapped in a facility full of robots. You are all trapped in here with me!"
// Author(s): Cherry!
//

#pragma once

#include "of_weapon_base_gun.h"

#ifdef CLIENT_DLL
	#define COFRocketLauncher C_OFRocketLauncher
#endif

class COFRocketLauncher : public COFWeaponBaseGun
{
public:
	DECLARE_CLASS(COFRocketLauncher, COFWeaponBaseGun);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFRocketLauncher();
	~COFRocketLauncher();

	#ifdef GAME_DLL

	virtual void Precache();

	#else

	virtual void CreateMuzzleFlashEffects(CBaseEntity *param_1, int param_2);

	#endif

	virtual OFWeaponID GetWeaponID() { return OF_WEAPON_ROCKETLAUNCHER; }
	// only for something about hints but ive never seen this ingame, so unused?
	//virtual CBaseEntity *FireProjectile(COFPlayer *pPlayer);
	//virtual void ItemPostFrame();
	// everything else is just econ related
};