// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the team selection menu
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_teammenu.h"
#include "basemodelpanel.h"
#include "of_gamerules.h"
#include "c_of_player.h"
#include "hudelement.h"

COFTeamButton::COFTeamButton(Panel* parent, const char* panelName) : BaseClass(parent, panelName, "")
{
	m_szModel[0] = '\0'; // 0x29c
	m_iTeam = TEAM_UNASSIGNED; // 0x2dc
	m_flHoverTimeDelay = -1.0; // 0x2e0
	m_flHoverTime = 0.0; // 0x2e4
	m_bCursorEntered = false; // 0x2e8
	m_bDisabled = false; // 0x2e9

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

void COFTeamButton::ApplySettings(KeyValues *pKey)
{
	BaseClass::ApplySettings(pKey);

	V_strncpy(m_szModel, pKey->GetString("associated_model", ""), sizeof(m_szModel));
	m_iTeam = pKey->GetInt("team");
	m_flHoverTime = pKey->GetFloat("hover", -1.0);
}

void COFTeamButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetDefaultColor(GetFgColor(), Color(0, 0, 0, 0));
	SetArmedColor(GetButtonFgColor(), Color(0, 0, 0, 0));
	SetDepressedColor(GetButtonFgColor(), Color(0, 0, 0, 0));
}

void COFTeamButton::SendAnimation(const char *pszAnim)
{
	Panel *pParent = GetParent();
	if (!pParent) return;

	Panel *pModel = dynamic_cast<CModelPanel*>(pParent->FindChildByName(m_szModel));
	if (!pModel) return;

	KeyValues *kv = new KeyValues("SetAnimation");
	if (kv)
	{
		kv->SetString("animation", pszAnim);
		PostMessage(pModel, kv);
	}
}

bool COFTeamButton::IsTeamFull()
{
	if (tf_arena_use_queue.GetBool() && OFGameRules() && OFGameRules()->IsInArenaMode()) return false;

	if (m_iTeam > TEAM_UNASSIGNED)
	{
		COFTeamMenu *pMenu = dynamic_cast<COFTeamMenu*>(GetParent());
		if (pMenu)
		{
			switch (m_iTeam)
			{
			case OF_TEAM_RED:
				//return pMenu->0x37c;
				return pMenu->m_bRedDoorDisabled;
				break;
			case OF_TEAM_BLUE:
				//return pMenu->0x37d;
				return pMenu->m_bBlueDoorDisabled;
				break;
			}
		}
	}

	return false;
}

void COFTeamButton::OnCursorEntered()
{
	BaseClass::OnCursorEntered();

	m_bCursorEntered = true;

	m_flHoverTime = (m_flHoverTimeDelay > 0) ? gpGlobals->curtime + m_flHoverTimeDelay : -1.0;

	SendAnimation( (!m_bDisabled) ? "enter_enabled" : "enter_disabled" );
}

void COFTeamButton::OnCursorExited()
{
	BaseClass::OnCursorExited();
	
	m_bCursorEntered = false;

	m_flHoverTime = -1.0;

	SendAnimation((!m_bDisabled) ? "exit_enabled" : "exit_disabled");
}

void COFTeamButton::OnTick()
{
	Panel *pParent = GetParent();
	if (!pParent) return;

	bool bFullTeam = IsTeamFull();
	if (bFullTeam != m_bDisabled)
	{
		m_bDisabled = bFullTeam;

		if (m_bCursorEntered)
		{
			m_bCursorEntered = true;

			m_flHoverTime = (m_flHoverTimeDelay > 0) ? gpGlobals->curtime + m_flHoverTimeDelay : -1.0;

			SendAnimation( (!m_bDisabled) ? "enter_enabled" : "enter_disabled" );
		}
		else
		{
			SendAnimation((!m_bDisabled) ? "idle_enabled" : "idle_disabled");
		}
	}

	if (m_flHoverTime > 0 && gpGlobals->curtime > m_flHoverTime)
	{
		m_flHoverTime = -1.0;

		SendAnimation((!m_bDisabled) ? "hover_enabled" : "hover_disabled");
	}
}

COFTeamMenu::COFTeamMenu(IViewPort *pViewPort) : BaseClass(pViewPort)
{
	SetMenuButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetVisible(false);
	SetKeyBoardInputEnabled(true);

	m_pTeamBlueButton = new COFTeamButton(this, "teambuttonblue"); // 0x33c
	m_pTeamRedButton = new COFTeamButton(this, "teambuttonred"); // 0x344
	m_pTeamAutoButton = new COFTeamButton(this, "teambuttonauto"); //0x348
	m_pTeamSpecButton = new COFTeamButton(this, "teambuttonspec"); // 0x1ec
	m_pTeamSpecLabel = new COFLabel(this, "TeamMenuSpectate", ""); // 0x34c
	m_pCancelButton = new COFButton(this, "CancelButton", "#TF_Cancel"); // 0x350
	//m_pTeamFullLabel = new COFLabel(this, "TeamsFullLabel", "");
	//m_pTeamFullLabelShadow = new COFLabel(this, "TeamsFullLabelShadow", "");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	LoadControlSettings("Resource/UI/Teammenu.res");
	SetMouseInputEnabled(true);

	m_bRedDoorDisabled = false;
	m_bBlueDoorDisabled = false;

	//ListenForGameEvent("server_spawn");
}

COFTeamMenu::~COFTeamMenu()
{
}

void COFTeamMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("Resource/UI/Teammenu.res");
	SetMouseInputEnabled(true);
	Update();
}

void COFTeamMenu::ShowPanel(bool bShow)
{
	if (IsVisible() == bShow) return;

	if (!gameuifuncs || !gViewPortInterface) return; // || !engine) - when would this ever fail >:T

	if (bShow)
	{
		COFPlayer *pPlayer = COFPlayer::GetLocalOFPlayer();
		if (!pPlayer) return;

		if (OFGameRules()->State_Get() == GR_STATE_TEAM_WIN && pPlayer->GetTeamNumber() != OFGameRules()->GetWinningTeam() &&
			pPlayer->GetTeamNumber() != TEAM_SPECTATOR && pPlayer->GetTeamNumber() != TEAM_UNASSIGNED)
		{
			SetVisible(false);

			//CHudNotificationPanel *pPanel = gHUD.FindElement("CHudNotificationPanel");
			//if (pPanel)
			//{
			//	pPanel->SetupNotifyCustom("#TF_CantChangeTeamNow", "ico_notify_flag_moving", pPlayer->GetTeamNumber());
			//}

			return;
		}

		gViewPortInterface->ShowPanel(PANEL_CLASS_RED, false);
		gViewPortInterface->ShowPanel(PANEL_CLASS_BLUE, false);

		engine->CheckPoint("TeamMenu");

		InvalidateLayout(true, true);
		Activate();
	}
	else
	{
		SetVisible(false);
	}
}

void COFTeamMenu::Update()
{
	BaseClass::Update();

	COFPlayer *pPlayer = COFPlayer::GetLocalOFPlayer();
	if (pPlayer && pPlayer->GetTeamNumber() != TEAM_UNASSIGNED)
	{
		if (m_pCancelButton) m_pCancelButton->SetVisible(true);
	}
	else
	{
		if (m_pCancelButton && m_pCancelButton->IsVisible()) m_pCancelButton->SetVisible(false);
	}
}

void COFTeamMenu::OnCommand(const char *cmd)
{
	COFPlayer *pPlayer = COFPlayer::GetLocalOFPlayer();

	if (pPlayer && V_stricmp(cmd, "vguicancel"))
	{
		if (V_strstr(cmd, "jointeam "))
		{
			const char *pszTeam = cmd + V_strlen("jointeam ");
			int iTeam = TEAM_UNASSIGNED;

			if (V_stricmp(pszTeam, "spectate") == 0)
			{
				iTeam = TEAM_SPECTATOR;
			}
			else if (V_stricmp(pszTeam, "red") == 0)
			{
				iTeam = OF_TEAM_RED;
			}
			else if (V_stricmp(pszTeam, "blue") == 0)
			{
				iTeam = OF_TEAM_BLUE;
			}

			if (m_bRedDoorDisabled && iTeam == OF_TEAM_RED) return;
			if (m_bBlueDoorDisabled && iTeam == OF_TEAM_BLUE) return;

			if (pPlayer->GetTeamNumber() != iTeam)
			{
				engine->ClientCmd(cmd);
			}
		}
		else if (V_strstr(cmd, "jointeam_nomenus "))
		{
			engine->ClientCmd(cmd);
		}
	}

	BaseClass::OnCommand(cmd);
	ShowPanel(false);
	OnClose();
}