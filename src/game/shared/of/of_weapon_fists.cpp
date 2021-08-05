// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "I will kill you with bare hands!"
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_base.h"
#include "of_weapon_base_melee.h"
#include "of_weapon_fists.h"

#ifdef CLIENT_DLL
	#include "c_of_player.h"
#else
	#include "of_player.h"
#endif

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFFists, DT_OFWeaponFists)

BEGIN_NETWORK_TABLE(COFFists, DT_OFWeaponFists)
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(COFFists)
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
BEGIN_DATADESC(COFFists)
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_fists, COFFists);

COFFists::COFFists()
{
}

COFFists::~COFFists()
{
}

void COFFists::PrimaryAttack()
{
	if (!CanAttack()) return;

	m_iWeaponMode = IsViewModelFlipped() ? OF_WEAPON_MODE_SECONDARY : OF_WEAPON_MODE_PRIMARY;

	Punch();
}

// same thing just has a function we dont need
void COFFists::SecondaryAttack()
{
	if (!CanAttack()) return;

	m_iWeaponMode = IsViewModelFlipped() ? OF_WEAPON_MODE_PRIMARY : OF_WEAPON_MODE_SECONDARY;

	Punch();
}

void COFFists::Punch()
{
	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	Swing(pPlayer);

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;

	//pPlayer->SpeakWeaponFire(-1);

	//CTFGameStats::Event_PlayerFiredWeapon((CTFGameStats *)PTR__CTF_GameStats_00e340b8, this_01, (bool)this[0x6ba]);

	// why is this here..?
	//cVar5 = CTFPlayerShared::IsStealthed((CTFPlayerShared *)&this_01->field_0x19b8);
	//if (cVar5 != '\0') {
	//CTFPlayer::RemoveInvisibility(this_01);
	//}
}

void COFFists::SendPlayerAnimEvent(COFPlayer *pPlayer)
{
	pPlayer->DoAnimationEvent(!IsAttackCritical() ? PLAYERANIMEVENT_ATTACK_PRIMARY : PLAYERANIMEVENT_ATTACK_SECONDARY);
}

void COFFists::DoViewModelAnimation()
{
	int iAnim = (m_iWeaponMode == OF_WEAPON_MODE_PRIMARY ? ACT_VM_HITLEFT : ACT_VM_HITRIGHT);

	if (IsAttackCritical()) iAnim = ACT_VM_SWINGHARD;

	SendWeaponAnim(iAnim);
}