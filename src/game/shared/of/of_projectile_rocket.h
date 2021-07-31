// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFProjectile_Rocket
// Author(s): Cherry!
//

#pragma once

#include "of_weapon_base_rocket.h"

#ifdef CLIENT_DLL
	#define COFProjectile_Rocket C_OFProjectile_Rocket
#endif

class COFProjectile_Rocket : public COFBaseRocket
{
public:

	DECLARE_CLASS(COFProjectile_Rocket, COFBaseRocket);
	DECLARE_NETWORKCLASS();

	#ifdef CLIENT_DLL

	COFProjectile_Rocket();
	~COFProjectile_Rocket();

	#endif

	#ifdef GAME_DLL

	virtual void Precache();
	virtual void Spawn();

	static COFProjectile_Rocket *Create(CBaseEntity *pLauncher, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pEntity);

	virtual CBaseEntity *GetAssistant() { return NULL; }
	virtual void SetScorer(CBaseEntity *pScorer) { m_hScorer = pScorer; }
	virtual CBaseEntity *GetScorer() { return dynamic_cast<CBaseEntity*>(m_hScorer.Get()); }
	virtual OFWeaponID GetWeaponID() { return OF_WEAPON_ROCKETLAUNCHER; }
	virtual bool IsDeflectable() { return true; }
	virtual int GetDamageType();
	virtual void SetCritical(bool bCritical) { m_bCritical = bCritical; }

	#else

	virtual void OnDataChanged(DataUpdateType_t updateType) { BaseClass::OnDataChanged(updateType); }
	virtual void CreateTrails();
	virtual const char *GetTrailParticleName() { return "rockettrail"; } // econ cut

	#endif

	virtual bool GetCritical() { return m_bCritical; }

private:

	CNetworkVar(bool, m_bCritical);

	#ifdef GAME_DLL

	CBaseHandle m_hScorer;

	#else

	CNewParticleEffect *pEffect;

	#endif
};