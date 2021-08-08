// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Entity that propagates general data needed by clients for every player
// Author(s): Cherry!
//

#pragma once

#include "c_playerresource.h"

class C_OFPlayerResource : public C_PlayerResource
{
public:
	DECLARE_CLASS(C_OFPlayerResource, C_PlayerResource);
	DECLARE_CLIENTCLASS();

	C_OFPlayerResource();
	~C_OFPlayerResource();

	int GetArrayValue(int iPlayerIndex, int *pArray, int iDefault);
	int GetCountForPlayerClass(int iTeam, int iClass, bool bIgnoreLocalPlayer);

	int GetMaxHealth(int iPlayerIndex) { return GetArrayValue(iPlayerIndex, m_iMaxHealth, 1); }
	int GetPlayerClass(int iPlayerIndex) { return GetArrayValue(iPlayerIndex, m_iPlayerClass, 0); }

private:

	int m_iTotalScore[MAX_PLAYERS + 1];
	int m_iMaxHealth[MAX_PLAYERS + 1];
	int m_iMaxBuffedHealth[MAX_PLAYERS + 1];
	int m_iPlayerClass[MAX_PLAYERS + 1];
	int m_bArenaSpectator[MAX_PLAYERS + 1];
	int m_iActiveDominations[MAX_PLAYERS + 1];

};

extern C_OFPlayerResource *g_C_OFPlayerResource;