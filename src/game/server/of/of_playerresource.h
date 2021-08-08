// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Entity that propagates general data needed by clients for every player
// Author(s): Cherry!
//

#pragma once

#include "player_resource.h"

class COFPlayerResource : public CPlayerResource
{
public:
	DECLARE_CLASS(COFPlayerResource, CPlayerResource);
	DECLARE_SERVERCLASS();

	COFPlayerResource();

	virtual void Init(int iPlayerIndex);

	// FILLED with matchmaking stuff we just dont need
	// gonna do this my own way
	// UpdateConnectedPlayer, UpdateDisconnectedPlayer, UpdatePlayerData
	virtual void UpdatePlayerData();

	virtual int GetTotalScore(int iPlayerIndex);
	virtual void SetPlayerClassWhenKilled(int iPlayerIndex, int iClass) { m_iPlayerClass.Set(iPlayerIndex, iClass); }

private:

	CNetworkArray(int, m_iTotalScore, MAX_PLAYERS + 1); // 2248 = 0x8c8
	CNetworkArray(int, m_iMaxHealth, MAX_PLAYERS + 1); // 2520 = 0x9d8
	CNetworkArray(int, m_iMaxBuffedHealth, MAX_PLAYERS + 1); // 2656 = 0xa60
	CNetworkArray(int, m_iPlayerClass, MAX_PLAYERS + 1); // 2384 = 0x950
	CNetworkArray(int, m_bArenaSpectator, MAX_PLAYERS + 1);
	CNetworkArray(int, m_iActiveDominations, MAX_PLAYERS + 1); 
};