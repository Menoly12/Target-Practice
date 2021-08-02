// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Entity to trigger a round win
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_gameroundwin.h"
#include "of_shareddefs.h"
#include "of_gamerules.h"

// ----------------------------------------------------------------------------- //
// Data tables
// ----------------------------------------------------------------------------- //

BEGIN_DATADESC(COFTeamplayRoundWin)

	DEFINE_KEYFIELD(m_bForceMapReset, FIELD_BOOLEAN, "force_map_reset"),
	DEFINE_KEYFIELD(m_bSwitchTeamsOnWin, FIELD_BOOLEAN, "switch_teams"),
	DEFINE_KEYFIELD(m_iWinReason, FIELD_INTEGER, "win_reason"),

	// Input
	DEFINE_INPUTFUNC(FIELD_INTEGER, "SetTeam", InputSetTeam),
	DEFINE_INPUTFUNC(FIELD_VOID, "RoundWin", InputRoundWin),

	// Output
	DEFINE_OUTPUT(m_outputOnRoundWin, "OnRoundWin"),

END_DATADESC()

LINK_ENTITY_TO_CLASS(game_round_win, COFTeamplayRoundWin);

COFTeamplayRoundWin::COFTeamplayRoundWin()
{
	m_bForceMapReset = false;
	m_bSwitchTeamsOnWin = false;
	m_iWinReason = WINREASON_DEFEND_UNTIL_TIME_LIMIT;
}

COFTeamplayRoundWin::~COFTeamplayRoundWin()
{
}

void COFTeamplayRoundWin::InputRoundWin(inputdata_t &inputdata)
{
	RoundWin(inputdata);
}

void COFTeamplayRoundWin::RoundWin(inputdata_t &inputdata)
{
	if (OFGameRules())
	{
		int iTeam = GetTeamNumber();

		if (iTeam > LAST_SHARED_TEAM)
		{
			if (m_bForceMapReset)
			{
				OFGameRules()->SetWinningTeam(iTeam, m_iWinReason, true, m_bSwitchTeamsOnWin);
			}
			else
			{
				OFGameRules()->SetWinningTeam(iTeam, m_iWinReason, false);
			}
		}
		else
		{
			OFGameRules()->SetStalemate(m_iWinReason, m_bForceMapReset, m_bSwitchTeamsOnWin);
		}
	}

	m_outputOnRoundWin.FireOutput(this, this);
}