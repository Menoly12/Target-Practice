// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the team selection menu
// Author(s): Cherry!
//

#pragma once

#include "teammenu.h"
#include "of_controls.h"

class COFTeamButton : public COFButton
{
public:
	DECLARE_CLASS_SIMPLE(COFTeamButton, COFButton);

	COFTeamButton(Panel* parent, const char* panelName);

	virtual void ApplySettings(KeyValues *pKey);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void SendAnimation(const char *pszAnim);
	virtual bool IsTeamFull();
	virtual void OnCursorEntered();
	virtual void OnCursorExited();
	virtual void OnTick();

private:

	char m_szModel[64];
	int m_iTeam;
	float m_flHoverTimeDelay;
	float m_flHoverTime;
	bool m_bCursorEntered;
	bool m_bDisabled;
};

class COFTeamMenu : public CTeamMenu //, public CGameEventListener
{
public:
	DECLARE_CLASS_SIMPLE(COFTeamMenu, CTeamMenu);

	COFTeamMenu(IViewPort *pViewPort);
	~COFTeamMenu();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void ShowPanel(bool bShow);
	virtual void Update();
	virtual void LoadMapPage(const char *mapName) {}
	virtual void OnCommand(const char *cmd);

	bool m_bRedDoorDisabled;
	bool m_bBlueDoorDisabled;

private:

	COFTeamButton *m_pTeamBlueButton;
	COFTeamButton *m_pTeamRedButton;
	COFTeamButton *m_pTeamAutoButton;
	COFTeamButton *m_pTeamSpecButton;
	COFLabel *m_pTeamSpecLabel;
	COFLabel *m_pTeamFullLabel;
	COFLabel *m_pTeamFullLabelShadow;
	COFButton *m_pCancelButton;
};