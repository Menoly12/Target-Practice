// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Pyro's fire axe.
// Author(s): Cherry!
//

#pragma once

#ifdef CLIENT_DLL
	#define COFFireAxe C_OFFireAxe
#endif

class COFFireAxe : public COFWeaponBaseMelee
{
public:
	DECLARE_CLASS(COFFireAxe, COFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFFireAxe();
	~COFFireAxe();

	virtual OFWeaponID GetWeaponID() { return OF_WEAPON_FIREAXE; }
};