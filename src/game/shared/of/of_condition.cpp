//====== Copyright © 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#ifdef GAME_DLL
#include "of_player.h"
#include "dt_utlvector_send.h"
#else
#include "c_of_player.h"
#include "dt_utlvector_recv.h"
#endif

BEGIN_NETWORK_TABLE_NOBASE( COFCondition, DT_Condition )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iID ) ),
	RecvPropFloat( RECVINFO( m_flDuration ) ),
	RecvPropEHandle( RECVINFO( m_hProvider ) ),
	RecvPropEHandle( RECVINFO( m_hOuter ) ),
	RecvPropInt( RECVINFO( m_iSeed ) ),
	RecvPropString( RECVINFO( m_iszVars ) )
#else
	SendPropInt( SENDINFO( m_iID ) ),
	SendPropFloat( SENDINFO( m_flDuration ) ),
	SendPropEHandle( SENDINFO( m_hProvider ) ),
	SendPropEHandle( SENDINFO( m_hOuter ) ),
	SendPropInt( SENDINFO( m_iSeed ) ),
	SendPropString( SENDINFO( m_iszVars ) )
#endif
END_NETWORK_TABLE()

COFCondition::COFCondition()
{
	m_pManager = NULL;
}

COFCondition::COFCondition( 
	ETFCond nCond, float flDuration, CCondPlayer *hOuter, int iSeed,
	EOFCondStackingMode nCanStack, CBaseEntity *hProvider, char *szVars )
{
	m_iID = nCond;
	m_flDuration = flDuration;
	m_iSeed = iSeed;
	m_nCanStack = nCanStack;
	m_hOuter = hOuter;
	m_hProvider = hProvider;

	m_pManager = NULL;

	if( szVars )
		Q_strncpy( m_iszVars.GetForModify(), szVars, OF_MAX_COND_LENGHT );
}

COFCondition::~COFCondition()
{
}

int COFCondition::GetInt( void )
{
	return Q_atoi(m_iszVars.Get());
}

bool COFCondition::GetBool( void )
{
	return !!Q_atoi(m_iszVars.Get());
}

float COFCondition::GetFloat( void )
{
	return Q_atof(m_iszVars.Get());
}

char *COFCondition::GetString( void )
{
	return m_iszVars.GetForModify();
}

void COFCondition::Think( void )
{
	if( g_fnCondThink[m_iID] )
		g_fnCondThink[m_iID]( this );

#ifdef GAME_DLL
	// Ignore permanent conditions
	if ( GetRemainingDuration() != PERMANENT_LENGHT )
	{
		float flReduction = gpGlobals->frametime;

		m_flDuration.Set( max( m_flDuration - flReduction, 0 ) );

		if ( m_flDuration == 0 )
		{
			m_pManager->RemoveCondSeed( GetSeed() );
		}
	}
#endif
}

BEGIN_NETWORK_TABLE_NOBASE( COFCondManager, DT_CondManager )
#ifdef CLIENT_DLL
RecvPropUtlVector( 
	RECVINFO_UTLVECTOR( m_hPlayerCond ), 
	32,
	RecvPropDataTable( NULL, 0, 0, &REFERENCE_RECV_TABLE( DT_Condition ) ) ),
RecvPropInt( RECVINFO( m_iCondSeed ) ),
#else
SendPropUtlVector( 
	SENDINFO_UTLVECTOR( m_hPlayerCond ),
	32, // max elements
	SendPropDataTable( NULL, 0, &REFERENCE_SEND_TABLE( DT_Condition ) ) ),
SendPropInt( SENDINFO( m_iCondSeed ), 32, SPROP_UNSIGNED | SPROP_CHANGES_OFTEN ),
#endif
END_NETWORK_TABLE()

// Loops through all our conditions and triggers their think function
// By default that only checks if their time is up
// But conds can add their own think function
void COFCondManager::RunThink( void )
{
	FOR_EACH_VEC_BACK( m_hPlayerCond, i )
	{
		m_hPlayerCond[i].Think();
	}
}
//-----------------------------------------------------------------------------
// Purpose: Add a condition
// flDuration - How long the cond will be applied for, PERMANENT_LENGHT(-1) for infinite
// nCanStack - If you can have multiple instances of this condition, and under which circumstances
// pProvider - Who gave us this condition? Default is NULL
// szVars - Additional vars that the cond may use in its think function or otherwise, usually empty
//-----------------------------------------------------------------------------
void COFCondManager::AddCond( ETFCond nCond, float flDuration /* = PERMANENT_LENGHT */, EOFCondStackingMode nCanStack /* = OF_COND_STACK_NONE */,
	CBaseEntity *pProvider /* = NULL */, char *szVars /* = NULL */ )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	switch( nCanStack )
	{
		// Override the cond
		case OF_COND_STACK_NONE:
			{
				COFCondition *pPrevious = GetCond(nCond);
				if( pPrevious )
				{
					pPrevious->SetProvider( pProvider );
					pPrevious->SetDuration( flDuration );
					pPrevious->SetVars( szVars );
					return;
				}
			}
			break;
		// Only one of the same condition per provider
		case OF_COND_STACK_ONE_PER_PROVIDER:
			{
				COFCondition *pPreviousProvider = GetCond(nCond, pProvider);
				if( pPreviousProvider )
				{
					pPreviousProvider->SetProvider( pProvider );
					pPreviousProvider->SetDuration( flDuration );
					pPreviousProvider->SetVars( szVars );
					return;
				}
			}
			break;
		case OF_COND_STACK_LONGER_TIME:
			// TODO: Check if we have the cond, if we do, override it if our new cond has a longer time
			break;
		// Allow stacking
		case OF_COND_STACK_ALL:
		default:
			break;
	}

	COFCondition hNewCond(nCond, flDuration, m_pOuter, m_iCondSeed, nCanStack, pProvider, szVars);
	hNewCond.m_pManager = this;
	m_hPlayerCond.AddToTail(hNewCond);

	m_pOuter->m_Shared.OnConditionAdded(nCond);

	m_iCondSeed++;
}

//-----------------------------------------------------------------------------
// Purpose: Remove all instances of a cond
// optionally, all instances of a cond given by a specific provider
//-----------------------------------------------------------------------------
void COFCondManager::RemoveCond( ETFCond nCond, CBaseEntity *pProvider )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	if( pProvider )
	{
		FOR_EACH_VEC_BACK( m_hPlayerCond, i )
		{
			if( m_hPlayerCond[i].GetID() == nCond && m_hPlayerCond[i].GetProvider() == pProvider )
				m_hPlayerCond.FastRemove(i);
		}
	}
	else
	{
		FOR_EACH_VEC_BACK( m_hPlayerCond, i )
		{
			if( m_hPlayerCond[i].GetID() == nCond )
				m_hPlayerCond.FastRemove(i);
		}
	}

	m_pOuter->m_Shared.OnConditionRemoved(nCond);
}

//-----------------------------------------------------------------------------
// Purpose: Remove one specific condition
//-----------------------------------------------------------------------------
void COFCondManager::RemoveCondSeed( int iSeed )
{
	ETFCond nCond = TF_COND_NONE;
	FOR_EACH_VEC_BACK( m_hPlayerCond, i )
	{
		if( m_hPlayerCond[i].GetSeed() == iSeed )
		{
			nCond = m_hPlayerCond[i].GetID();
			m_pOuter->m_Shared.OnConditionRemoved( nCond );
			m_hPlayerCond.FastRemove(i);
			break;
		}
	}
}

COFCondition *COFCondManager::GetCond( ETFCond nCond, CBaseEntity *pProvider /* = NULL*/, COFCondition *pPrevious /*= NULL*/ )
{
	for( int i = pPrevious ? max(m_hPlayerCond.Find(*pPrevious), 0) : 0; i < m_hPlayerCond.Count(); i++ )
	{
		if( m_hPlayerCond[i].GetID() == nCond && ( !pProvider || m_hPlayerCond[i].GetProvider() == pProvider ) )
			return &m_hPlayerCond[i];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool COFCondManager::InCond( ETFCond nCond )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	FOR_EACH_VEC_BACK( m_hPlayerCond, i )
	{
		if( m_hPlayerCond[i].GetID() == nCond )
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float COFCondManager::GetConditionDuration( ETFCond nCond )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	FOR_EACH_VEC_BACK( m_hPlayerCond, i )
	{
		if( m_hPlayerCond[i].GetID() == nCond )
		{
			return m_hPlayerCond[i].GetRemainingDuration();
		}
	}

	return 0.0f;
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COFCondManager::OnPreDataChanged( void )
{
	m_hOldPlayerCond.CopyArray( m_hPlayerCond.Base(), m_hPlayerCond.Size() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COFCondManager::OnDataChanged( void )
{
	// Update conditions from last network change

	// We remove all conditions that are the same between old and new
	// The ones left in the old one are the ones that just ran out, aka OnCondRemoved()
	// The ones left in the new one are the ones that were just added, aka OnCondAdded()
	CUtlVector<COFCondition> hAddPlayerCond; hAddPlayerCond.CopyArray( m_hPlayerCond.Base(), m_hPlayerCond.Size() );
	CUtlVector<COFCondition> hRemovePlayerCond; hRemovePlayerCond.CopyArray( m_hOldPlayerCond.Base(), m_hOldPlayerCond.Size() );

	FOR_EACH_VEC_BACK( m_hPlayerCond, i )
	{
		m_hPlayerCond[i].m_pManager = this;
		int y = m_hOldPlayerCond.Find(m_hPlayerCond[i]);

		if( y != -1 )
		{
			hRemovePlayerCond.Remove(y);
			hAddPlayerCond.Remove(i);
		}
	}

	FOR_EACH_VEC( hRemovePlayerCond, i )
		m_pOuter->m_Shared.OnConditionRemoved( hRemovePlayerCond[i].GetID() );

	FOR_EACH_VEC( hAddPlayerCond, i )
		m_pOuter->m_Shared.OnConditionAdded( hAddPlayerCond[i].GetID() );

	m_hOldPlayerCond.CopyArray( m_hPlayerCond.Base(), m_hPlayerCond.Size() );
}
#endif

// example of the formatting
void CondBasicThink( COFCondition *pCond )
{
}

// ===================================================================
// Purpose: Think function for afterburn
// However it also serves as an example on how a condition think could be used!
// ===================================================================
void CondBurningThink( COFCondition *pCond )
{
#ifdef GAME_DLL
	CCommand args;
	args.Tokenize(pCond->GetString());

	int iDamage = args.ArgC() ? atoi(args[0]) : 3;
	float flFlameFrequency = args.ArgC() > 1 ? atof(args[1]) : 0.5f;

	// Newly created afterburn should only take effect once you stop firing at someone
	// For consistency in damage output
	float flFlameBurnTime = args.ArgC() > 2 ? atof(args[2]) : gpGlobals->curtime + flFlameFrequency;

	CCondPlayer *pOwner = pCond->GetOuter();
	
	// Above waist water extinguishes us
	if( pOwner->GetWaterLevel() >= WL_Waist )
	{
		// Gets removed right after this function
		pCond->SetDuration(0);
		return;
	}
	else if( flFlameBurnTime <= gpGlobals->curtime && OF_CLASS_PYRO != pOwner->m_Class.GetClassIndex() )
	{
		// Take damage unless we're a pyro
		CBaseEntity *pAttacker = pCond->GetProvider() ? pCond->GetProvider() : pCond->GetOuter();
		CTakeDamageInfo info( pAttacker, pAttacker, iDamage, DMG_BURN | DMG_PREVENT_PHYSICS_FORCE );
		pOwner->TakeDamage(info);
		flFlameBurnTime = gpGlobals->curtime + flFlameFrequency;
	}

	// Update our vars
	pCond->SetVars( UTIL_VarArgs( "%d %f %f", iDamage, flFlameFrequency, flFlameBurnTime ) );
#endif
}

void (*g_fnCondThink[TF_COND_LAST])( COFCondition *pCond ) =
{
	NULL, // TF_COND_AIMING
	NULL, // TF_COND_ZOOMED
	NULL, // TF_COND_DISGUISING
	NULL, // TF_COND_DISGUISED
	NULL, // TF_COND_STEALTHED
	NULL, // TF_COND_INVULNERABLE
	NULL, // TF_COND_TELEPORTED
	NULL, // TF_COND_TAUNTING
	NULL, // TF_COND_INVULNERABLE_WEARINGOFF
	NULL, // TF_COND_STEALTHED_BLINK
	NULL, // TF_COND_SELECTED_TO_TELEPORT
	NULL, // TF_COND_CRITBOOSTED
	NULL, // TF_COND_TMPDAMAGEBONUS
	NULL, // TF_COND_FEIGN_DEATH
	NULL, // TF_COND_PHASE
	NULL, // TF_COND_STUNNED
	NULL, // TF_COND_OFFENSEBUFF
	NULL, // TF_COND_SHIELD_CHARGE
	NULL, // TF_COND_DEMO_BUFF
	NULL, // TF_COND_ENERGY_BUFF
	NULL, // TF_COND_RADIUSHEAL
	NULL, // TF_COND_HEALTH_BUFF
	&CondBurningThink, // TF_COND_BURNING
	NULL, // TF_COND_HEALTH_OVERHEALED
	NULL, // TF_COND_URINE
	NULL, // TF_COND_BLEEDING
	NULL, // TF_COND_DEFENSEBUFF
	NULL, // TF_COND_MAD_MILK
	NULL, // TF_COND_MEGAHEAL
	NULL, // TF_COND_REGENONDAMAGEBUFF
	NULL, // TF_COND_MARKEDFORDEATH
	NULL, // TF_COND_NOHEALINGDAMAGEBUFF
	NULL, // TF_COND_SPEED_BOOST
	NULL, // TF_COND_CRITBOOSTED_PUMPKIN
	NULL, // TF_COND_CRITBOOSTED_USER_BUFF
	NULL, // TF_COND_CRITBOOSTED_DEMO_CHARGE
	NULL, // TF_COND_SODAPOPPER_HYPE
	NULL, // TF_COND_CRITBOOSTED_FIRST_BLOOD
	NULL, // TF_COND_CRITBOOSTED_BONUS_TIME
	NULL, // TF_COND_CRITBOOSTED_CTF_CAPTURE
	NULL, // TF_COND_CRITBOOSTED_ON_KILL
	NULL, // TF_COND_CANNOT_SWITCH_FROM_MELEE
	NULL, // TF_COND_DEFENSEBUFF_NO_CRIT_BLOCK
	NULL, // TF_COND_REPROGRAMMED
	NULL, // TF_COND_CRITBOOSTED_RAGE_BUFF
	NULL, // TF_COND_DEFENSEBUFF_HIGH
	NULL, // TF_COND_SNIPERCHARGE_RAGE_BUFF
	NULL, // TF_COND_DISGUISE_WEARINGOFF
	NULL, // TF_COND_MARKEDFORDEATH_SILENT
	NULL, // TF_COND_DISGUISED_AS_DISPENSER
	NULL, // TF_COND_SAPPED
	NULL, // TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGE
	NULL, // TF_COND_INVULNERABLE_USER_BUFF
	NULL, // TF_COND_HALLOWEEN_BOMB_HEAD
	NULL, // TF_COND_HALLOWEEN_THRILLER
	NULL, // TF_COND_RADIUSHEAL_ON_DAMAGE
	NULL, // TF_COND_CRITBOOSTED_CARD_EFFECT
	NULL, // TF_COND_INVULNERABLE_CARD_EFFECT
	NULL, // TF_COND_MEDIGUN_UBER_BULLET_RESIST
	NULL, // TF_COND_MEDIGUN_UBER_BLAST_RESIST
	NULL, // TF_COND_MEDIGUN_UBER_FIRE_RESIST
	NULL, // TF_COND_MEDIGUN_SMALL_BULLET_RESIST
	NULL, // TF_COND_MEDIGUN_SMALL_BLAST_RESIST
	NULL, // TF_COND_MEDIGUN_SMALL_FIRE_RESIST
	NULL, // TF_COND_STEALTHED_USER_BUFF
	NULL, // TF_COND_MEDIGUN_DEBUFF
	NULL, // TF_COND_STEALTHED_USER_BUFF_FADING
	NULL, // TF_COND_BULLET_IMMUNE
	NULL, // TF_COND_BLAST_IMMUNE
	NULL, // TF_COND_FIRE_IMMUNE
	NULL, // TF_COND_PREVENT_DEATH
	NULL, // TF_COND_MVM_BOT_STUN_RADIOWAVE
	NULL, // TF_COND_HALLOWEEN_SPEED_BOOST
	NULL, // TF_COND_HALLOWEEN_QUICK_HEAL
	NULL, // TF_COND_HALLOWEEN_GIANT
	NULL, // TF_COND_HALLOWEEN_TINY
	NULL, // TF_COND_HALLOWEEN_IN_HELL
	NULL, // TF_COND_HALLOWEEN_GHOST_MODE
	NULL, // TF_COND_MINICRITBOOSTED_ON_KILL
	NULL, // TF_COND_OBSCURED_SMOKE
	NULL, // TF_COND_PARACHUTE_ACTIVE
	NULL, // TF_COND_BLASTJUMPING
	NULL, // TF_COND_HALLOWEEN_KART
	NULL, // TF_COND_HALLOWEEN_KART_DASH
	NULL, // TF_COND_BALLOON_HEAD
	NULL, // TF_COND_MELEE_ONLY
	NULL, // TF_COND_SWIMMING_CURSE
	NULL, // TF_COND_FREEZE_INPUT
	NULL, // TF_COND_HALLOWEEN_KART_CAGE
	NULL, // TF_COND_DONOTUSE_0
	NULL, // TF_COND_RUNE_STRENGTH
	NULL, // TF_COND_RUNE_HASTE
	NULL, // TF_COND_RUNE_REGEN
	NULL, // TF_COND_RUNE_RESIST
	NULL, // TF_COND_RUNE_VAMPIRE
	NULL, // TF_COND_RUNE_REFLECT
	NULL, // TF_COND_RUNE_PRECISION
	NULL, // TF_COND_RUNE_AGILITY
	NULL, // TF_COND_GRAPPLINGHOOK
	NULL, // TF_COND_GRAPPLINGHOOK_SAFEFALL
	NULL, // TF_COND_GRAPPLINGHOOK_LATCHED
	NULL, // TF_COND_GRAPPLINGHOOK_BLEEDING
	NULL, // TF_COND_AFTERBURN_IMMUNE
	NULL, // TF_COND_RUNE_KNOCKOUT
	NULL, // TF_COND_RUNE_IMBALANCE
	NULL, // TF_COND_CRITBOOSTED_RUNE_TEMP
	NULL, // TF_COND_PASSTIME_INTERCEPTION
	NULL, // TF_COND_SWIMMING_NO_EFFECTS
	NULL, // TF_COND_PURGATORY
	NULL, // TF_COND_RUNE_KING
	NULL, // TF_COND_RUNE_PLAGUE
	NULL, // TF_COND_RUNE_SUPERNOVA
	NULL, // TF_COND_PLAGUE
	NULL, // TF_COND_KING_BUFFED
	NULL, // TF_COND_TEAM_GLOWS
	NULL, // TF_COND_KNOCKED_INTO_AIR
	NULL, // TF_COND_COMPETITIVE_WINNER
	NULL, // TF_COND_COMPETITIVE_LOSER
	NULL, // TF_COND_HEALING_DEBUFF
	NULL, // TF_COND_PASSTIME_PENALTY_DEBUFF
	NULL, // TF_COND_GRAPPLED_TO_PLAYER
	NULL, // TF_COND_GRAPPLED_BY_PLAYER
	NULL, // TF_COND_PARACHUTE_DEPLOYED
	NULL, // TF_COND_GAS
	NULL, // TF_COND_BURNING_PYRO
	NULL, // TF_COND_ROCKETPACK
	NULL, // TF_COND_LOST_FOOTING
	NULL, // TF_COND_AIR_CURRENT
};