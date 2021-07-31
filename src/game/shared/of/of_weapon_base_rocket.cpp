// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFBaseRocket
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "of_weapon_base.h"
#include "of_weapon_base_rocket.h"
#include "basegrenade_shared.h"

#ifdef GAME_DLL

#include "iscorer.h"
#include "Sprite.h"
#include "of_fx.h"
#include "soundent.h"
#include "of_player.h"

extern void SendProxy_Origin(const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID);
extern void SendProxy_Angles(const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID);

#endif

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

IMPLEMENT_NETWORKCLASS_ALIASED(OFBaseRocket, DT_OFBaseRocket)

BEGIN_NETWORK_TABLE(COFBaseRocket, DT_OFBaseRocket)
#ifdef CLIENT_DLL
	RecvPropVector(RECVINFO(m_vInitialVelocity)),
	RecvPropVector(RECVINFO_NAME(m_vecNetworkOrigin, m_vecOrigin)),
	RecvPropQAngles(RECVINFO_NAME(m_angNetworkAngles, m_angRotation)),
	RecvPropInt(RECVINFO(m_iDeflected)),
	RecvPropEHandle(RECVINFO(m_hLauncher)),
#else
	SendPropVector(SENDINFO(m_vInitialVelocity), 12, 0, -3000.0, 3000),
	SendPropExclude("DT_BaseEntity", "m_vecOrigin"),
	SendPropExclude("DT_BaseEntity", "m_angRotation"),
	SendPropVector(SENDINFO(m_vecOrigin), -1, SPROP_CHANGES_OFTEN | SPROP_COORD_MP_INTEGRAL, 0.0, HIGH_DEFAULT, SendProxy_Origin),
	SendPropQAngles(SENDINFO(m_angRotation), 6, SPROP_CHANGES_OFTEN, SendProxy_Angles),
	SendPropInt(SENDINFO(m_iDeflected), 4),
	SendPropEHandle(SENDINFO(m_hLauncher)),
#endif
END_NETWORK_TABLE()

COFBaseRocket::COFBaseRocket()
{
	m_vInitialVelocity.Init();
	m_iDeflected = 0;
	m_hLauncher = NULL;

	#ifdef GAME_DLL

	m_flDamage = 0.0;

	#else

	m_flSpawnTime = 0.0;

	#endif
}

COFBaseRocket::~COFBaseRocket()
{
}

void COFBaseRocket::Precache()
{
	BaseClass::Precache();
}

void COFBaseRocket::Spawn()
{
	BaseClass::Spawn();
	Precache();
	UseClientSideAnimation();

	// econ
	//if (GetLauncher())
	//{ }

	#ifdef GAME_DLL

	SetSolid(SOLID_BBOX);
	SetMoveType(MOVETYPE_FLY, MOVECOLLIDE_FLY_CUSTOM);
	// OFTODO: figure out what happen here
	//AddEFlags(EFL_DIRTY_SHADOWUPDATE); // 0x20 is EFL_DIRTY_SHADOWUPDATE but thats client only??
	AddEffects(EF_NOSHADOW); // and then theirs no shadow???????
	SetCollisionGroup(OF_COLLISION_GROUP_ROCKETS);

	// dont think the min has to be a negative :v
	UTIL_SetSize(this, Vector(0, 0, 0), Vector(0, 0, 0));
	ResetSequence(LookupSequence("idle"));

	SetTouch(&COFBaseRocket::RocketTouch);
	SetNextThink(gpGlobals->curtime);
	AddFlag(FL_GRENADE);

	//field_0x4e4 = gpGlobals->curtime + 0.25
	//field_0x4e8 = false;

	#else

	m_flSpawnTime = gpGlobals->curtime;

	#endif
}

#ifdef GAME_DLL

COFBaseRocket *COFBaseRocket::Create(CBaseEntity *pLauncher, const char *szName, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner)
{
	COFBaseRocket *pRocket = dynamic_cast<COFBaseRocket*>(BaseClass::Create(szName, vecOrigin, vecAngles, pOwner));

	if (!pRocket) return NULL;

	pRocket->SetLauncher(pLauncher);
	pRocket->SetOwnerEntity(pOwner);
	pRocket->Spawn();

	Vector vForward, vRight, vUp;
	AngleVectors(vecAngles, &vForward, &vRight, &vUp); // local_1c, local_2c, local_3c

	// attribute "mult_projectile_speed"
	float flProjectileSpeed = 1100.0;

	// for some attributes we dont care about
	//if (pOwner)
	//{
	//}

	// mann power
	//if (uVar14 != 0xffffffff) {}

	Vector velocity = vForward * flProjectileSpeed;
	pRocket->SetAbsVelocity(velocity);
	pRocket->SetInitialVelocity(velocity); //pRocket->0x4cc = vecVelocity;

	QAngle angle;
	VectorAngles(velocity, angle);
	pRocket->SetAbsAngles(angle);

	pRocket->ChangeTeam(pOwner->GetTeamNumber());

	return pRocket;
}

void COFBaseRocket::Destroy(bool bBlinkOut, bool bBreakRocket)
{
	if (bBreakRocket)
	{
		CPVSFilter filter(GetAbsOrigin());
		UserMessageBegin(filter, "BreakModelRocketDud");
		MessageWriteShort(GetModelIndex());
		MessageWriteVec3Coord(GetAbsOrigin());
		MessageWriteAngles(GetAbsAngles());
		MessageEnd();
	}

	SetThink(&BaseClass::SUB_Remove);
	SetNextThink(gpGlobals->curtime);
	SetTouch(NULL);
	AddEffects(EF_NODRAW);

	if (bBlinkOut)
	{
		CSprite *pSprite = CSprite::SpriteCreate("sprites/light_glow02_noz.vmt", GetAbsOrigin(), false);
		if (pSprite)
		{
			pSprite->SetTransparency(kRenderGlow, 255, 255, 255, 255, kRenderFxFadeFast);
			pSprite->SetThink(&BaseClass::SUB_Remove);
			pSprite->SetNextThink(gpGlobals->curtime + 1.0);
		}
	}
}

void COFBaseRocket::Explode(trace_t *pTrace, CBaseEntity *pEnemy)
{
	// ShouldNotDetonate() ??? heads to InNoGrenadeZone()
	//{ Destroy(); return; }

	m_hEnemy = pEnemy;

	//SetModelName(NULL_STRING); // this is.. a weird way to make the rocket disapper
	AddEffects(EF_NODRAW); // test this out later instead
	AddSolidFlags(SOLID_OBB_YAW);
	m_takedamage = DAMAGE_NO;

	if (pTrace->fraction != 1.0)
	{
		SetAbsOrigin(pTrace->endpos + pTrace->plane.normal);
	}

	Vector vecOrigin = GetAbsOrigin();
	CPVSFilter filter(vecOrigin);

	// attribute, holiday, and mvm stuff here

	TE_OFExplosion(filter, 0.0, vecOrigin, pTrace->plane.normal, GetWeaponID(), OFFX_NULL);
	CSoundEnt::InsertSound(SOUND_COMBAT, vecOrigin, BASEGRENADE_EXPLOSION_VOLUME, 3.0);

	CBaseEntity *pOwner = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>(pOwner);

	if (pScorerInterface)
	{
		pOwner = pScorerInterface->GetScorer();
	}
	else if (pOwner && pOwner->GetOwnerEntity())
	{
		pOwner = pOwner->GetOwnerEntity();
	}

	float flRadius = GetRadius();

	if (pOwner)
	{
		//COFPlayer *pPlayer = ToOFPlayer(GetEnemy());
		//if (pPlayer)
		//{
			// CheckForStunOnImpact()
			// RecordEnemyPlayerHit() - doesnt exist!
		//}

		CTakeDamageInfo info(this, pOwner, GetOriginalLauncher(), vec3_origin, vecOrigin, GetDamage(), GetDamageType(), GetDamageCustom());
		RadiusDamage(info, vecOrigin, flRadius, OF_CLASS_UNDEFINED, NULL);
	}

	// ignore whatever local11c is, leads to that halloween stuff
	if (!pEnemy->IsPlayer())
	{
		UTIL_DecalTrace(pTrace, "Scorch");
	}

	UTIL_Remove(this);
}

void COFBaseRocket::RocketTouch(CBaseEntity *pEntity)
{
	bool bCheck = pEntity->IsCombatItem() && !InSameTeam(pEntity);
	if (!bCheck && (pEntity->GetSolidFlags() & (FSOLID_VOLUME_CONTENTS | FSOLID_TRIGGER))) return;


	const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
	if (pTrace->surface.flags & SURF_SKY)
	{
		UTIL_Remove(this);
		return;
	}

	trace_t trace;
	memcpy(&trace, pTrace, sizeof(trace_t));
	Explode(&trace, pEntity);
}

void COFBaseRocket::SetLauncher(CBaseEntity *pLauncher)
{
	m_hLauncher = pLauncher;
	BaseClass::SetLauncher(pLauncher);
}

unsigned int COFBaseRocket::PhysicsSolidMaskForEntity() const
{
	unsigned int iContents;

	if (CanCollideWithTeammates())
	{
		iContents = CONTENTS_TEAMRED | CONTENTS_TEAMBLUE;
	}
	else
	{
		iContents = (GetTeamNumber() == OF_TEAM_RED) ? CONTENTS_TEAMBLUE : CONTENTS_TEAMRED;
	}

	return BaseClass::PhysicsSolidMaskForEntity() | iContents;
}

#else

int COFBaseRocket::DrawModel(int flags)
{
	if (gpGlobals->curtime - m_flSpawnTime < 0.2) return 0;

	return BaseClass::DrawModel(flags);
}

void COFBaseRocket::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);

	if (updateType == DATA_UPDATE_CREATED || m_iClientDeflect != GetDeflected())
	{
		CreateTrails();
	}

	m_iClientDeflect = GetDeflected();
}

void COFBaseRocket::PostDataUpdate(DataUpdateType_t updateType)
{
	BaseClass::PostDataUpdate(updateType);

	if (updateType == DATA_UPDATE_CREATED)
	{
		CInterpolatedVar<Vector> &vecInterpolator = GetOriginInterpolator();
		vecInterpolator.ClearHistory();

		CInterpolatedVar<QAngle> &angleInterpolator = GetRotationInterpolator();
		angleInterpolator.ClearHistory();

		float flTime = GetLastChangeTime(LATCH_SIMULATION_VAR);

		Vector vecOrigin = GetLocalOrigin() - m_vInitialVelocity;
		vecInterpolator.AddToHead(flTime - 1.0, &vecOrigin, false);

		QAngle angles = GetLocalAngles();
		angleInterpolator.AddToHead(flTime - 1.0, &angles, false);

		vecOrigin = GetLocalOrigin();
		vecInterpolator.AddToHead(flTime, &vecOrigin, false);

		angleInterpolator.AddToHead(flTime - 1.0, &angles, false);
	}
}

#endif