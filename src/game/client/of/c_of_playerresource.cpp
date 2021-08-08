// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Entity that propagates general data needed by clients for every player
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_shareddefs.h"
#include "c_of_playerresource.h"

// Datatable
IMPLEMENT_CLIENTCLASS_DT(C_OFPlayerResource, DT_OFPlayerResource, COFPlayerResource)
	RecvPropArray3(RECVINFO_ARRAY(m_iTotalScore), RecvPropInt(RECVINFO(m_iTotalScore[0]))),
	RecvPropArray3(RECVINFO_ARRAY(m_iMaxHealth), RecvPropInt(RECVINFO(m_iMaxHealth[0]))),
	RecvPropArray3(RECVINFO_ARRAY(m_iMaxBuffedHealth), RecvPropInt(RECVINFO(m_iMaxBuffedHealth[0]))),
	RecvPropArray3(RECVINFO_ARRAY(m_iPlayerClass), RecvPropInt(RECVINFO(m_iPlayerClass[0]))),
	RecvPropArray3(RECVINFO_ARRAY(m_bArenaSpectator), RecvPropInt(RECVINFO(m_bArenaSpectator[0]))),
	RecvPropArray3(RECVINFO_ARRAY(m_iActiveDominations), RecvPropInt(RECVINFO(m_iActiveDominations[0]))),
END_RECV_TABLE()

C_OFPlayerResource *g_C_OFPlayerResource;

C_OFPlayerResource::C_OFPlayerResource()
{
	for (int i = 0; i < OF_TEAM_COUNT; i++)
	{
		m_Colors[i] = g_aTeamColors[i];
	}

	g_C_OFPlayerResource = this;
}

C_OFPlayerResource::~C_OFPlayerResource()
{
	g_C_OFPlayerResource = NULL;
}

int C_OFPlayerResource::GetArrayValue(int iPlayerIndex, int *pArray, int iDefault)
{
	if (!IsConnected(iPlayerIndex)) // || IsValid()) - doesnt exist
	{
		return iDefault;
	}

	return pArray[iPlayerIndex];
}

int C_OFPlayerResource::GetCountForPlayerClass(int iTeam, int iClass, bool bIgnoreLocalPlayer)
{
	int iLocalPlayerIndex = GetLocalPlayerIndex();
	int iClassCount = 0;

	for (int i = 0; i <= MAX_PLAYERS; i++)
	{
		if (!bIgnoreLocalPlayer || i != iLocalPlayerIndex)
		{
			if (GetTeam(i) == iTeam && GetPlayerClass(i) == iClass)
			{
				iClassCount++;
			}
		}
	}

	return iClassCount;
}