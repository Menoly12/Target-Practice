// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFProjectile_Rocket
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_base.h"
#include "of_projectile_rocket.h"

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFProjectile_Rocket, DT_OFProjectile_Rocket)

BEGIN_NETWORK_TABLE(COFProjectile_Rocket, DT_OFProjectile_Rocket)
#ifdef CLIENT_DLL
	RecvPropBool(RECVINFO(m_bCritical)),
#else
	SendPropBool(SENDINFO(m_bCritical)),
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
LINK_ENTITY_TO_CLASS(tf_projectile_rocket, COFProjectile_Rocket);
PRECACHE_REGISTER(tf_projectile_rocket);
#endif

#ifdef CLIENT_DLL

COFProjectile_Rocket::COFProjectile_Rocket()
{
	pEffect = NULL;
}

COFProjectile_Rocket::~COFProjectile_Rocket()
{
	if (pEffect)
	{
		ParticleProp()->StopEmission(pEffect);
	}
}

#endif

#ifdef GAME_DLL

void COFProjectile_Rocket::Precache()
{
	PrecacheModel("models/weapons/w_models/w_rocket.mdl");
	//PrecacheGibsForModel(iVar1); // dont remember rockets being able to break :v
	PrecacheParticleSystem("critical_rocket_blue");
	PrecacheParticleSystem("critical_rocket_red");
	//PrecacheParticleSystem("eyeboss_projectile");
	PrecacheParticleSystem("rockettrail");
	//PrecacheParticleSystem("rockettrail_RocketJumper");
	BaseClass::Precache();
}

void COFProjectile_Rocket::Spawn()
{
	SetModel("models/weapons/w_models/w_rocket.mdl");
	BaseClass::Spawn();
}

COFProjectile_Rocket *COFProjectile_Rocket::Create(CBaseEntity *pLauncher, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pEntity)
{
	COFProjectile_Rocket *pRocket = dynamic_cast<COFProjectile_Rocket*>(COFBaseRocket::Create(pLauncher, "tf_projectile_rocket", vecOrigin, vecAngles, pOwner));

	if (pRocket)
	{
		pRocket->SetScorer(pEntity);

		//field_0x4fe = 0;

		// leads to econ stuff after here
	}

	return pRocket;
}

int COFProjectile_Rocket::GetDamageType()
{
	int iDamageType = BaseClass::GetDamageType();

	if (m_bCritical)
	{
		iDamageType |= DMG_CRIT;
	}

	return iDamageType;
}

#else

void COFProjectile_Rocket::CreateTrails()
{
	if (IsDormant()) return;

	if (pEffect)
	{
		ParticleProp()->StopEmission(pEffect);
		pEffect = NULL;
	}

	if (LookupAttachment("trail") == INVALID_PARTICLE_ATTACHMENT) return;

	if (enginetrace->GetPointContents(GetAbsOrigin()) & MASK_WATER)
	{
		ParticleProp()->Create("rockettrail_underwater", PATTACH_POINT_FOLLOW, "trail");
	}
	else
	{
		ParticleProp()->Create(GetTrailParticleName(), PATTACH_POINT_FOLLOW, "trail");
	}
	//else if (GetTeamNumber() == TEAM_UNASSIGNED)
	//{
		// why?
		//ParticleProp()->Create("rockettrail_underwater", PATTACH_POINT_FOLLOW, "trail");
	//}
	// holiday stuff
	//else {}

	if (m_bCritical)
	{
		switch (GetTeamNumber())
		{
		default:
		case OF_TEAM_RED:
			ParticleProp()->Create("critical_rocket_red", PATTACH_POINT_FOLLOW);
			break;
		case OF_TEAM_BLUE:
			ParticleProp()->Create("critical_rocket_blue", PATTACH_POINT_FOLLOW);
			break;
		}
	}
}

#endif