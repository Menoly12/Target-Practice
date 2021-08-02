// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Entity to trigger a round win
// Info: Something weird about this is it's called CTeamplayRoundWin which id imagine
//		 would be in the base sdk but it's not? doesnt even have a TF prefix
//		 so for some consistency ill just add an OF prefix
// Author(s): Cherry!
//

#pragma once

class COFTeamplayRoundWin : public CBaseEntity
{
public:
	DECLARE_CLASS(COFTeamplayRoundWin, CBaseEntity);
	DECLARE_DATADESC();

	COFTeamplayRoundWin();
	~COFTeamplayRoundWin();

	void InputRoundWin(inputdata_t &inputdata);
	void RoundWin(inputdata_t &inputdata);

private:

	bool m_bForceMapReset;
	bool m_bSwitchTeamsOnWin;
	int m_iWinReason;

	COutputEvent m_outputOnRoundWin;

};