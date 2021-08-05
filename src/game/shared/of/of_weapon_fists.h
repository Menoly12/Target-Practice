// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "I will kill you with bare hands!"
// Author(s): Cherry!
//

#pragma once

#ifdef CLIENT_DLL
	#define COFFists C_OFFists
#endif

class COFFists : public COFWeaponBaseMelee
{
public:
	DECLARE_CLASS(COFFists, COFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	#ifdef GAME_DLL
	DECLARE_DATADESC();
	#endif

	COFFists();
	~COFFists();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Punch();

	virtual OFWeaponID GetWeaponID() { return OF_WEAPON_FISTS; }
	virtual void SendPlayerAnimEvent(COFPlayer *pPlayer);
	virtual void DoViewModelAnimation();

};