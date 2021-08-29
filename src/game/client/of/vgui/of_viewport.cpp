// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Clientside viewport
// Author(s): ficool2
//

#include "cbase.h"

#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui/Cursor.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <vgui/VGUI.h>

#include "hud.h"

#include <baseviewport.h>
#include "of_viewport.h"
#include "of_textwindow.h"
#include "of_mapinfomenu.h"
#include "of_teammenu.h"
#include "of_classmenu.h"

void OFViewport::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	gHUD.InitColors( pScheme );

	SetPaintBackgroundEnabled( false );
}

void OFViewport::CreateDefaultPanels( void )
{
	AddNewPanel(CreatePanelByName(PANEL_MAPINFO), "PANEL_MAPINFO");
	AddNewPanel(CreatePanelByName(PANEL_TEAM), "PANEL_TEAM");
	AddNewPanel(CreatePanelByName(PANEL_CLASS_RED), "PANEL_CLASS_RED");
	AddNewPanel(CreatePanelByName(PANEL_CLASS_BLUE), "PANEL_CLASS_BLUE");
	AddNewPanel(CreatePanelByName(PANEL_INTRO), "PANEL_INTRO");
	AddNewPanel(CreatePanelByName(PANEL_ROUNDINFO), "PANEL_ROUNDINFO");
	BaseClass::CreateDefaultPanels();
}

IViewPortPanel* OFViewport::CreatePanelByName( const char *szPanelName )
{
	IViewPortPanel* newpanel = NULL;

	if (V_strcmp(PANEL_INFO, szPanelName) == 0)
	{
		newpanel = new COFTextWindow(this);
	}
	else if (V_strcmp(PANEL_MAPINFO, szPanelName) == 0)
	{
		newpanel = new COFMapInfoMenu(this);
	}
	else if (V_strcmp(PANEL_TEAM, szPanelName) == 0)
	{
		newpanel = new COFTeamMenu(this);
	}
	else if (V_strcmp(PANEL_CLASS_RED, szPanelName) == 0)
	{
		newpanel = new COFClassMenuRed(this);
	}
	else if (V_strcmp(PANEL_CLASS_BLUE, szPanelName) == 0)
	{
		newpanel = new COFClassMenuBlue(this);
	}
	else
	{
		newpanel = BaseClass::CreatePanelByName( szPanelName );
	}

	return newpanel; 
}