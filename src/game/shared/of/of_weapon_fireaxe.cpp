// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Pyro's fire axe.
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_base.h"
#include "of_weapon_base_melee.h"
#include "of_weapon_fireaxe.h"

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFFireAxe, DT_OFWeaponFireAxe)

BEGIN_NETWORK_TABLE(COFFireAxe, DT_OFWeaponFireAxe)
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(COFFireAxe)
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
BEGIN_DATADESC(COFFireAxe)
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_fireaxe, COFFireAxe);

COFFireAxe::COFFireAxe()
{
}

COFFireAxe::~COFFireAxe()
{
}