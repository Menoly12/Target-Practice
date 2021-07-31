// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: "I am not trapped in a facility full of robots. You are all trapped in here with me!"
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_weapon_rocketlauncher.h"

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFRocketLauncher, DT_WeaponRocketLauncher)

BEGIN_NETWORK_TABLE(COFRocketLauncher, DT_WeaponRocketLauncher)
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(COFRocketLauncher)
END_PREDICTION_DATA()

#ifdef GAME_DLL
BEGIN_DATADESC(COFRocketLauncher)
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS(tf_weapon_rocketlauncher, COFRocketLauncher);

COFRocketLauncher::COFRocketLauncher()
{
	m_bReloadsSingly = true;
}

COFRocketLauncher::~COFRocketLauncher()
{
}

#ifdef GAME_DLL

void COFRocketLauncher::Precache()
{
	BaseClass::Precache();
	PrecacheParticleSystem("rocketbackblast");
}

#else

void COFRocketLauncher::CreateMuzzleFlashEffects(CBaseEntity *param_1, int param_2)
{
	BaseClass::CreateMuzzleFlashEffects(param_1, param_2);

	COFPlayer *pPlayer = GetOFPlayerOwner();
	if (!pPlayer) return;

	ParticleProp()->Init(this);
	ParticleProp()->Create("rocketbackblast", PATTACH_POINT_FOLLOW, "backblast");
}

#endif