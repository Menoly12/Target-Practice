// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of C_TFPlayer
// Author(s): Fenteale, Nopey
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "c_of_player.h"
#include "c_basetempentity.h"
#include "effect_dispatch_data.h"
#include "c_te_effect_dispatch.h"
#include "engine/ivdebugoverlay.h"
#include "of_weapon_base.h"
#include "functionproxy.h"
#include "voice_status.h"
#include "prediction.h"
#include "tier0/vprof.h"
#include "c_of_playerresource.h"

// Don't alias here
// Why not? -Nopey
#if defined( COFPlayer )
#undef COFPlayer
#endif

IMaterial *g_pHeadLabelMaterial[2] = { NULL, NULL };

const char *pszHeadLabelNames[] =
{
	"effects/speech_voice_red",
	"effects/speech_voice_blue",
};

//-----------------------------------------------------------------------------
// Purpose: dummy to stop live tf2 materials from going black
//-----------------------------------------------------------------------------

class CProxyBurnLevel : public CResultProxy
{
public:
	void OnBind( void *pC_BaseEntity )
	{
		Assert( m_pResult );

		m_pResult->SetFloatValue( 0.0f );
	}
};

EXPOSE_INTERFACE( CProxyBurnLevel, IMaterialProxy, "BurnLevel" IMATERIAL_PROXY_INTERFACE_VERSION );


//-----------------------------------------------------------------------------
// Purpose: dummy to stop live tf2 materials from going black
//-----------------------------------------------------------------------------
class CProxyYellowLevel : public CResultProxy
{
public:
	void OnBind( void *pC_BaseEntity )
	{
		Assert( m_pResult );

		// stop stuff from going black
		m_pResult->SetVecValue( 1.0f, 1.0f, 1.0f );
	}
};

EXPOSE_INTERFACE(CProxyYellowLevel, IMaterialProxy, "YellowLevel" IMATERIAL_PROXY_INTERFACE_VERSION);


// -------------------------------------------------------------------------------- //
// Player animation event reciever. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate( DataUpdateType_t updateType )
	{
		VPROF("C_TEPlayerAnimEvent::PostDataUpdate");

		// Create the effect.
		C_OFPlayer *pPlayer = dynamic_cast< C_OFPlayer* >( m_hPlayer.Get() );
		if ( pPlayer && !pPlayer->IsDormant() )
		{
			pPlayer->DoAnimationEvent( (PlayerAnimEvent_t)m_iEvent.Get(), m_nData );
		}	
	}

	CNetworkHandle( C_BasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_iEvent ) ),
	RecvPropInt( RECVINFO( m_nData ) )
END_RECV_TABLE()

// Local Exclusive
BEGIN_RECV_TABLE_NOBASE(C_OFPlayer, DT_OFLocalPlayerExclusive)
	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin) ),
	//SendPropArray2(
	//
	//
	RecvPropFloat( RECVINFO(m_angEyeAngles[0]) ),
	RecvPropFloat( RECVINFO(m_angEyeAngles[1]) ),
END_RECV_TABLE()

// Non Local Exclusive
BEGIN_RECV_TABLE_NOBASE(C_OFPlayer, DT_OFNonLocalPlayerExclusive)
	RecvPropVector( RECVINFO_NAME(m_vecNetworkOrigin, m_vecOrigin) ),
	RecvPropFloat( RECVINFO(m_angEyeAngles[0]) ),
	RecvPropFloat( RECVINFO(m_angEyeAngles[1]) ),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT(C_OFPlayer, DT_OF_Player, COFPlayer)
	RecvPropDataTable(RECVINFO_DT(m_Shared), 0, &REFERENCE_RECV_TABLE(DT_OFPlayerShared)),
	RecvPropDataTable(RECVINFO_DT(m_Class), 0, &REFERENCE_RECV_TABLE(DT_OFPlayerClassShared)),
	RecvPropDataTable("oflocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_OFLocalPlayerExclusive)),
	RecvPropDataTable("ofnonlocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_OFNonLocalPlayerExclusive)),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_OFPlayer )
	DEFINE_PRED_TYPEDESCRIPTION(m_Shared, COFPlayerShared),
	DEFINE_PRED_FIELD(m_nSkin, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_nBody, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_nSequence, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_flPlaybackRate, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_flPlaybackRate, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_ARRAY_TOL(m_flEncodedController, FIELD_FLOAT, MAXSTUDIOBONECTRLS, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK, 0.02f),
	DEFINE_PRED_FIELD(m_nNewSequenceParity, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_nResetEventsParity, FIELD_INTEGER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
	DEFINE_PRED_FIELD(m_nMuzzleFlashParity, FIELD_CHARACTER, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK),
END_PREDICTION_DATA()

C_OFPlayer::C_OFPlayer() : m_iv_angEyeAngles("C_OFPlayer::m_iv_angEyeAngles")
{
	m_PlayerAnimState = CreatePlayerAnimState( this );
	AddVar(&m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR);

	m_Shared.m_pOuter = this;
	m_Shared.m_Conds.m_pOuter = this;
}

void C_OFPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if (IsLocalPlayer() && !prediction->IsFirstTimePredicted() && cl_predict->GetBool()) return;

	MDLCACHE_CRITICAL_SECTION();
	m_PlayerAnimState->DoAnimationEvent(event, nData);
}

C_OFPlayer* C_OFPlayer::GetLocalOFPlayer()
{
	return ToOFPlayer(BaseClass::GetLocalPlayer());
}

const QAngle &C_OFPlayer::EyeAngles()
{
	if (IsLocalPlayer() && g_nKillCamMode == OBS_MODE_NONE)
	{
		return BaseClass::EyeAngles();
	}

	return m_angEyeAngles;
}

extern ConVar friendlyfire;
extern ConVar sv_showimpacts;

// OFTODO: Move this to Shared Player
void C_OFPlayer::FireBullet( 
						   Vector vecSrc,	// shooting postion
						   const QAngle &shootAngles,  //shooting angle
						   float vecSpread, // spread vector
						   int iDamage, // base damage
						   int iBulletType, // ammo type
						   CBaseEntity *pevAttacker, // shooter
						   bool bDoEffects,	// create impact effect ?
						   float x,	// spread x factor
						   float y	// spread y factor
						   )
{
	float fCurrentDamage = iDamage;   // damage of the bullet at it's current trajectory
	float flCurrentDistance = 0.0;  //distance that the bullet has traveled so far

	Vector vecDirShooting, vecRight, vecUp;
	AngleVectors( shootAngles, &vecDirShooting, &vecRight, &vecUp );

	if ( !pevAttacker )
		pevAttacker = this;  // the default attacker is ourselves

	// add the spray 
	Vector vecDir = vecDirShooting +
		x * vecSpread * vecRight +
		y * vecSpread * vecUp;

	VectorNormalize( vecDir );

	float flMaxRange = 8000;

	Vector vecEnd = vecSrc + vecDir * flMaxRange; // max bullet range is 10000 units

	trace_t tr; // main enter bullet trace

	UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID|CONTENTS_DEBRIS|CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &tr );

		if ( tr.fraction == 1.0f )
			return; // we didn't hit anything, stop tracing shoot

	if ( sv_showimpacts.GetBool() )
	{
#ifdef CLIENT_DLL
		// draw red client impact markers
		debugoverlay->AddBoxOverlay( tr.endpos, Vector(-2,-2,-2), Vector(2,2,2), QAngle( 0, 0, 0), 255,0,0,127, 4 );

		if ( tr.m_pEnt && tr.m_pEnt->IsPlayer() )
		{
			C_BasePlayer *player = ToBasePlayer( tr.m_pEnt );
			player->DrawClientHitboxes( 4, true );
		}
#else
		// draw blue server impact markers
		NDebugOverlay::Box( tr.endpos, Vector(-2,-2,-2), Vector(2,2,2), 0,0,255,127, 4 );

		if ( tr.m_pEnt && tr.m_pEnt->IsPlayer() )
		{
			CBasePlayer *player = ToBasePlayer( tr.m_pEnt );
			player->DrawServerHitboxes( 4, true );
		}
#endif
	}

		//calculate the damage based on the distance the bullet travelled.
		flCurrentDistance += tr.fraction * flMaxRange;

		// damage get weaker of distance
		fCurrentDamage *= pow ( 0.85f, (flCurrentDistance / 500));

		int iDamageType = DMG_BULLET | DMG_NEVERGIB;

		if( bDoEffects )
		{
			// See if the bullet ended up underwater + started out of the water
			if ( enginetrace->GetPointContents( tr.endpos ) & (CONTENTS_WATER|CONTENTS_SLIME) )
			{	
				trace_t waterTrace;
				UTIL_TraceLine( vecSrc, tr.endpos, (MASK_SHOT|CONTENTS_WATER|CONTENTS_SLIME), this, COLLISION_GROUP_NONE, &waterTrace );

				if( waterTrace.allsolid != 1 )
				{
					CEffectData	data;
					data.m_vOrigin = waterTrace.endpos;
					data.m_vNormal = waterTrace.plane.normal;
					data.m_flScale = random->RandomFloat( 8, 12 );

					if ( waterTrace.contents & CONTENTS_SLIME )
					{
						data.m_fFlags |= FX_WATER_IN_SLIME;
					}

					DispatchEffect( "gunshotsplash", data );
				}
			}
			else
			{
				//Do Regular hit effects

				// Don't decal nodraw surfaces
				if ( !( tr.surface.flags & (SURF_SKY|SURF_NODRAW|SURF_HINT|SURF_SKIP) ) )
				{
					CBaseEntity *pEntity = tr.m_pEnt;
					if ( !( !friendlyfire.GetBool() && pEntity && pEntity->IsPlayer() && pEntity->GetTeamNumber() == GetTeamNumber() ) )
					{
						UTIL_ImpactTrace( &tr, iDamageType );
					}
				}
			}
		} // bDoEffects

		// add damage to entity that we hit

#ifdef GAME_DLL
		ClearMultiDamage();

		CTakeDamageInfo info( pevAttacker, pevAttacker, fCurrentDamage, iDamageType );
		CalculateBulletDamageForce( &info, iBulletType, vecDir, tr.endpos );
		tr.m_pEnt->DispatchTraceAttack( info, vecDir, &tr );

		TraceAttackToTriggers( info, tr.startpos, tr.endpos, vecDir );

		ApplyMultiDamage();
#endif
}

void SetupHeadLabelMaterials()
{
	for (int i = 0; i < ARRAYSIZE(g_pHeadLabelMaterial); i++)
	{
		if (g_pHeadLabelMaterial[i])
		{
			g_pHeadLabelMaterial[i]->DecrementReferenceCount(); // 0x32
			g_pHeadLabelMaterial[i] = NULL;
		}

		g_pHeadLabelMaterial[i] = materials->FindMaterial(pszHeadLabelNames[i], TEXTURE_GROUP_VGUI); // 0x124
		g_pHeadLabelMaterial[i]->IncrementReferenceCount(); // 0x30
	}
}

// OFSTATUS: INCOMPLETE
void C_OFPlayer::OnPreDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnPreDataChanged(updateType);

	m_iPreDataChangeClass = m_Class.m_iClass;

	m_iPreDataChangeTeam = GetTeamNumber(); // 0x2068

	m_Shared.OnPreDataChanged();
}

// OFSTATUS: VERY INCOMPLETE
void C_OFPlayer::OnDataChanged(DataUpdateType_t updateType)
{
	SetNetworkAngles(GetLocalAngles());
	BaseClass::OnDataChanged(updateType);

	if (updateType == DATA_UPDATE_CREATED)
	{
		SetNextClientThink(CLIENT_THINK_ALWAYS);
		//InitInvulnerableMaterial();
	}
	else
	{
		if ((m_iPreDataChangeTeam != GetTeamNumber()))// || field_0x2070 != m_Shared.GetDisguiseTeam()) // field_0x2070 = m_iPreDataChangeDisguise?
		{
			//InitInvulnerableMaterial();
		}

		/* This is for that unused dispenser disguise condition
		iVar21 = this->field_0x2074;
		iVar11 = 9;
		cVar6 = CTFPlayerShared::InCond((CTFPlayerShared *)&this->field_0x17cc, 0x31);
		if (cVar6 == '\0') {
			iVar11 = this->field_0x18d0;
		}
		if (iVar21 != iVar11) {
			C_BaseEntity::RemoveAllDecals((C_BaseEntity *)this);
		}
		*/
	}

	// line 188
	if (m_iPreDataChangeClass != m_Class.m_iClass)
	{
		OnPlayerClassChange();
	}

	// line 340
	if (IsLocalPlayer())
	{
		if (updateType == DATA_UPDATE_CREATED)
		{
			SetupHeadLabelMaterials();
			GetClientVoiceMgr()->SetHeadLabelOffset(50.0);
		}

		// there's some vr stuff in here but.. i dont think anyone wants to play tf2 in vr
		if (m_iPreDataChangeTeam != GetTeamNumber())
		{
			IGameEvent *pEvent = gameeventmanager->CreateEvent("localplayer_changeteam");
			if (pEvent)
			{
				gameeventmanager->FireEventClientSide(pEvent);
			}
		}
	}

	m_Shared.OnDataChanged();
}

void C_OFPlayer::ValidateModelIndex()
{
	// that dispenser diguise again..

	// OFTODO: CONDITIONSSS
	//if (m_Shared.InCond(TF_COND_DISGUISED) && IsEnemyPlayer())
	//{
	//	m_nModelIndex = modelinfo->GetModelIndex(GetPlayerClassData(m_Shared.m_nDisguiseClass)->GetModelName()); //field_0x18d0 = m_nDisguiseClass
	//}
	//else
	//{
		m_nModelIndex = modelinfo->GetModelIndex(GetPlayerClassData(m_Class.m_iClass)->GetModelName());
	//}
	/*
	if (field_0x2088 > -1 && GetModelPtr())
	{
		if (m_Shared.InCond(TF_COND_DISGUISED) && !IsEnemyPlayer())
		{
			SetBodygroup(field_0x2088, true);
		}
		else
		{
			SetBodygroup(field_0x2088, false);
		}
	}
	*/

	BaseClass::ValidateModelIndex();
}

void C_OFPlayer::OnPlayerClassChange()
{
	m_PlayerAnimState->SetRunSpeed(GetPlayerClassData(m_Class.m_iClass)->m_flMaxSpeed);
	m_PlayerAnimState->SetWalkSpeed(GetPlayerClassData(m_Class.m_iClass)->m_flMaxSpeed * 0.5);

	//RemoveOverheadEffect("particle_nemesis_red", true);
	//RemoveOverheadEffect("particle_nemesis_blue", true);
	//*(undefined *)&this->field_0x1c24 = 0;
	//RemoveOverheadEffect("duel_red", true);
	//RemoveOverheadEffect("duel_blue", true);
	//*(undefined *)((int)&this->field_0x1c24 + 1) = 0;

	//SetAppropriateCamera();
}

void C_OFPlayer::UpdateClientSideAnimation()
{
	if (GetLocalOFPlayer() == this)
	{
		QAngle angle = EyeAngles();
		m_PlayerAnimState->Update(angle[YAW], angle[PITCH]);
	}
	else
	{
		m_PlayerAnimState->Update(m_angEyeAngles[YAW], m_angEyeAngles[PITCH]);
	}

	BaseClass::UpdateClientSideAnimation();
}

const QAngle &C_OFPlayer::GetRenderAngles()
{
	if (IsRagdoll())
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState->GetRenderAngles();
	}
}

int C_OFPlayer::GetMaxHealth() const
{
	if (g_C_OFPlayerResource)
	{
		return g_C_OFPlayerResource->GetMaxHealth(entindex());
	}
	
	return 1;
}