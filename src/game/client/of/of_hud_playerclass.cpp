// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFHudPlayerClass
// Author(s): Cherry!
//

#include "cbase.h"
#include "hudelement.h"
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/ImagePanel.h"
#include "GameEventListener.h"
#include "vgui/of_imagepanel.h"
#include "of_hud_playerclass.h"
#include "of_clientmode.h"
#include <vgui_controls/AnimationController.h>
#include "c_of_player.h"

ConVar cl_hud_playerclass_use_playermodel("cl_hud_playerclass_use_playermodel", "0", FCVAR_ARCHIVE, "Use player model in player class HUD");

static const char *g_szRedClassImages[] =
{
	"",
	"../hud/class_scoutred",
	"../hud/class_sniperred",
	"../hud/class_soldierred",
	"../hud/class_demored",
	"../hud/class_medicred",
	"../hud/class_heavyred",
	"../hud/class_pyrored",
	"../hud/class_spyred",
	"../hud/class_engired",
	"../hud/class_scoutred",
};

static const char *g_szBlueClassImages[] =
{
	"",
	"../hud/class_scoutblue",
	"../hud/class_sniperblue",
	"../hud/class_soldierblue",
	"../hud/class_demoblue",
	"../hud/class_medicblue",
	"../hud/class_heavyblue",
	"../hud/class_pyroblue",
	"../hud/class_spyblue",
	"../hud/class_engiblue",
	"../hud/class_scoutblue",
};

DECLARE_BUILD_FACTORY(COFClassImage);

void COFClassImage::SetClass(int iTeam, int iClass, int iCloak)
{
	char szClassImage[128];

	switch (iTeam)
	{
	default:
	case OF_TEAM_RED:
		V_strncpy(szClassImage, g_szRedClassImages[iClass], sizeof(szClassImage));
		break;
	case OF_TEAM_BLUE:
		V_strncpy(szClassImage, g_szBlueClassImages[iClass], sizeof(szClassImage));
		break;
	}

	// OFTODO: figure these values out laterrr
	switch (iCloak)
	{
	case 1:
		V_strncat(szClassImage, "_halfcloak", sizeof(szClassImage));
		break;
	case 2:
		V_strncat(szClassImage, "_cloak", sizeof(szClassImage));
		break;
	}

	if (V_strlen(szClassImage) > 0)
	{
		SetImage(szClassImage);
	}
}

COFHudPlayerClass::COFHudPlayerClass(Panel *parent, const char *pElementName) : BaseClass(parent, pElementName)
{

	ListenForGameEvent("localplayer_changedisguise");

	for (int i = 0; i < OF_CLASS_COUNT; i++)
	{
		PrecacheMaterial(VarArgs("vgui/%s", g_szRedClassImages[i]));
		PrecacheMaterial(VarArgs("vgui/%s_cloak", g_szRedClassImages[i]));
		PrecacheMaterial(VarArgs("vgui/%s_halfcloak", g_szRedClassImages[i]));

		PrecacheMaterial(VarArgs("vgui/%s", g_szBlueClassImages[i]));
		PrecacheMaterial(VarArgs("vgui/%s_cloak", g_szBlueClassImages[i]));
		PrecacheMaterial(VarArgs("vgui/%s_halfcloak", g_szBlueClassImages[i]));
	}
}

void COFHudPlayerClass::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/HudPlayerClass.res");

	m_iClass = OF_CLASS_UNDEFINED; // field_0x1bc
	m_iTeam = TEAM_UNASSIGNED; // field_0x1b8
	//field_0x1c4 = 0;
	//field_0x1c0 = 0;
	//field_0x1c8 = -1;
	m_flNextThink = 0; // field_0x18c
	//field_0x1cc = 0;
	//field_0x1d0 = 0;

	m_pPlayerClassImage = FindControl<COFClassImage>("PlayerStatusClassImage"); // 0x190
	m_pPlayerClassImageBG = FindControl<COFImagePanel>("PlayerStatusClassImageBG"); // 0x194
	m_pPlayerSpyImage = FindControl<COFImagePanel>("PlayerStatusSpyImage"); // 0x198
	m_pPlayerSpyOutlineImage = FindControl<COFImagePanel>("PlayerStatusSpyOutlineImage"); // 0x19c

	// OFTODO: CTFPlayerModelPanel
	//m_pPlayerClassModel = FindControl<COFPlayerModelPanel>("classmodelpanel"); // 0x1a0
	m_pPlayerClassModelBG = FindControl<COFImagePanel>("classmodelpanelBG"); // 0x1a4
}

void COFHudPlayerClass::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.5;

	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HudSpyDisguiseHide");
}

void COFHudPlayerClass::FireGameEvent(IGameEvent *event)
{
	if (V_strcmp(event->GetName(), "localplayer_changedisguise"))
	{
		bool bDisguised = event->GetBool("disguised");

		m_pPlayerSpyImage->SetAlpha(bDisguised ? 255 : 0);
		m_pPlayerSpyOutlineImage->SetAlpha(0);

		m_pPlayerSpyImage->SetVisible(true);
		m_pPlayerSpyOutlineImage->SetVisible(true);

		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence(bDisguised ? "HudSpyDisguiseFadeIn" : "HudSpyDisguiseFadeOut");

		//UpdateModelPanel();
	}
}

// OFTODO: filled with econ that we can ignore but we need the spy disguise stuff, so do it later!
void COFHudPlayerClass::OnThink()
{
	if (gpGlobals->curtime < m_flNextThink) return;

	m_flNextThink = gpGlobals->curtime + 0.5;

	COFPlayer *pPlayer = COFPlayer::GetLocalOFPlayer();
	if (!pPlayer) return;

	bool bCorrectClass = (m_iClass == pPlayer->m_Class.GetClassIndex());
	bool bCorrectTeam = (m_iTeam == pPlayer->GetTeamNumber());

	if (!bCorrectClass || !bCorrectTeam)
	{
		m_iClass = pPlayer->m_Class.GetClassIndex();
		m_iTeam = pPlayer->GetTeamNumber();

		if (m_pPlayerClassImage) m_pPlayerClassImage->SetClass(m_iTeam, m_iClass, 0);
	}
}