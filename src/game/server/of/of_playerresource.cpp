// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Entity that propagates general data needed by clients for every player
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_playerresource.h"
#include "of_shareddefs.h"
#include "of_player.h"
#include "of_gamerules.h"

// Datatable
IMPLEMENT_SERVERCLASS_ST(COFPlayerResource, DT_OFPlayerResource)
	SendPropArray3(SENDINFO_ARRAY3(m_iTotalScore), SendPropInt(SENDINFO_ARRAY(m_iTotalScore), -1, SPROP_VARINT | SPROP_UNSIGNED)),
	SendPropArray3(SENDINFO_ARRAY3(m_iMaxHealth), SendPropInt(SENDINFO_ARRAY(m_iMaxHealth), -1, SPROP_VARINT | SPROP_UNSIGNED)),
	SendPropArray3(SENDINFO_ARRAY3(m_iMaxBuffedHealth), SendPropInt(SENDINFO_ARRAY(m_iMaxBuffedHealth), -1, SPROP_VARINT | SPROP_UNSIGNED)),
	SendPropArray3(SENDINFO_ARRAY3(m_iPlayerClass), SendPropInt(SENDINFO_ARRAY(m_iPlayerClass), Q_log2(OF_CLASS_COUNT) + 1, SPROP_UNSIGNED)),
	SendPropArray3(SENDINFO_ARRAY3(m_bArenaSpectator), SendPropInt(SENDINFO_ARRAY(m_bArenaSpectator), 1, SPROP_UNSIGNED)),
	SendPropArray3(SENDINFO_ARRAY3(m_iActiveDominations), SendPropInt(SENDINFO_ARRAY(m_iActiveDominations), 6, SPROP_UNSIGNED)),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(tf_player_manager, COFPlayerResource);

COFPlayerResource::COFPlayerResource()
{
}

void COFPlayerResource::Init(int iPlayerIndex)
{
	BaseClass::Init(iPlayerIndex);

	m_iTotalScore.Set(iPlayerIndex, 0);
	m_iMaxHealth.Set(iPlayerIndex, 1);
	m_iMaxBuffedHealth.Set(iPlayerIndex, 1);
	m_iPlayerClass.Set(iPlayerIndex, 0);
	m_bArenaSpectator.Set(iPlayerIndex, false);
	m_iActiveDominations.Set(iPlayerIndex, 0);
}

void COFPlayerResource::UpdatePlayerData()
{
	BaseClass::UpdatePlayerData();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		COFPlayer *pPlayer = ToOFPlayer(UTIL_PlayerByIndex(i));

		if (pPlayer && pPlayer->IsConnected() && OFGameRules())
		{
			// OFTODO: we need CTFGameStats and more game rules work for this,
			// and CTFGameStats is a big one, good luck to whomever tackles that
			// m_iTotalScore

			m_iMaxHealth.Set(i, pPlayer->GetMaxHealth());
			m_iMaxBuffedHealth.Set(i, pPlayer->GetMaxHealthForBuffing());
			m_iPlayerClass.Set(i, pPlayer->m_Class.GetClassIndex());

			//m_bArenaSpectator
			//m_iActiveDominations
		}
	}
}

int COFPlayerResource::GetTotalScore(int iPlayerIndex)
{
	CBasePlayer *pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
	if (pPlayer && pPlayer->IsConnected())
	{
		return m_iTotalScore[iPlayerIndex];
	}

	return 0;
}