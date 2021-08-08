// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFHudPlayerStatus, displays the health status and class portrait
// Author(s): Cherry!
//

#include "cbase.h"
#include "hudelement.h"
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui/of_imagepanel.h"
#include "of_hud_playerhealth.h"
#include "of_hud_playerclass.h"
#include "of_hud_playerstatus.h"
#include "of_clientmode.h"

DECLARE_HUDELEMENT(COFHudPlayerStatus);

COFHudPlayerStatus::COFHudPlayerStatus(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudPlayerStatus")
{
	SetParent(g_pClientMode->GetViewport());

	m_pPlayerClassPanel = new COFHudPlayerClass(this, "HudPlayerClass"); // 0x1ac
	m_pPlayerHealthPanel = new COFHudPlayerHealth(this, "HudPlayerHealth"); // 0x1b0

	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD);
}

void COFHudPlayerStatus::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	ConVarRef cl_hud_minmode("cl_hud_minmode", true);
	if (cl_hud_minmode.IsValid() && cl_hud_minmode.GetBool())
	{
		m_pPlayerClassPanel->InvalidateLayout(false,true);
	}
}

void COFHudPlayerStatus::Reset()
{
	if (m_pPlayerClassPanel)
	{
		m_pPlayerClassPanel->Reset();
	}

	if (m_pPlayerHealthPanel)
	{
		m_pPlayerHealthPanel->Reset();
	}
}