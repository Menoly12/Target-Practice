// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFBaseRocket
// Author(s): Cherry!
//

#pragma once

#include "baseprojectile.h"

#ifdef CLIENT_DLL
	#define COFBaseRocket C_OFBaseRocket
#endif

#define OF_ROCKET_RADIUS 146.0;

class COFBaseRocket : public CBaseProjectile
{
public:
	DECLARE_CLASS(COFBaseRocket, CBaseProjectile);
	DECLARE_NETWORKCLASS();

	COFBaseRocket();
	~COFBaseRocket();

	virtual void Precache();
	virtual void Spawn();

	virtual int GetDeflected() { return m_iDeflected; }
	virtual CBaseEntity *GetLauncher() { return m_hLauncher; }

#ifdef GAME_DLL

	static COFBaseRocket *Create(CBaseEntity *pLauncher, const char *szName, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner);
	virtual void Destroy(bool bBlinkOut = true, bool bBreakRocket = false) override;
	virtual void Explode(trace_t *pTrace, CBaseEntity *pEntity);
	virtual void RocketTouch(CBaseEntity *pEntity);

	virtual void SetInitialVelocity(Vector vecVelocity) { m_vInitialVelocity = vecVelocity; }
	virtual OFWeaponID GetWeaponID() { return OF_WEAPON_ROCKETLAUNCHER; }
	virtual float GetRadius() { return OF_ROCKET_RADIUS; } // attributes cut
	virtual int GetDamageCustom() { return 0; }
	virtual float GetDamage() { return m_flDamage; }
	virtual void SetDamage(float flDamage) { m_flDamage = flDamage; }
	virtual int GetDamageType() { return g_aWeaponDamageTypes[GetWeaponID()]; }
	virtual CBaseEntity *GetEnemy() { return m_hEnemy; }
	virtual void IncrementDeflected() { m_iDeflected++; }
	virtual void SetLauncher(CBaseEntity *pLauncher);
	virtual unsigned int PhysicsSolidMaskForEntity() const;

	#else

	virtual void CreateTrails() {}
	virtual int DrawModel(int flags);
	virtual void OnDataChanged(DataUpdateType_t updateType);
	virtual void PostDataUpdate(DataUpdateType_t updateType);

	#endif

private:

	CNetworkVector(m_vInitialVelocity);
	CNetworkVar(int, m_iDeflected);
	CNetworkHandle(CBaseEntity, m_hLauncher);

	#ifdef GAME_DLL

	CHandle<CBaseEntity> m_hEnemy;

	float m_flDamage;

	#else

	int m_iClientDeflect;
	float m_flSpawnTime; // field_0x8a8

	#endif
};