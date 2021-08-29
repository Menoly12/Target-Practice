// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the Map Info Panel
// Author(s): Ms, Cherry!
//

#include "cbase.h"
#include "of_mapinfomenu.h"
#include "vgui/ILocalize.h"
#include "of_gamerules.h"
#include "filesystem.h"

COFMapInfoMenu::COFMapInfoMenu( IViewPort* pViewPort ) : vgui::Frame( NULL, PANEL_MAPINFO )
{
	SetScheme( "ClientScheme" );
	SetTitleBarVisible( false );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( false );
	SetProportional( true );
	SetVisible( false );
	SetKeyBoardInputEnabled( true );

	m_pMapInfoTitle = new COFLabel(this, "MapInfoTitle", ""); // 0x220
	m_pMapInfoContinue = new COFButton(this, "MapInfoContinue", "#TF_Continue"); // 0x228
	m_pMapInfoBack = new COFButton(this, "MapInfoBack", "#TF_Back"); // 0x22c
	m_pMapInfoWatchIntro = new COFButton(this, "MapInfoWatchIntro", "#TF_WatchIntro"); // 0x230
	m_pMapInfoText = new COFRichText(this, "MapInfoText"); // 0x224
	m_pMapInfoImage = new COFImagePanel(this, "MapImage"); // 0x240
}

void COFMapInfoMenu::SetData( KeyValues* data )
{
}

//OFSTATUS: Complete
void COFMapInfoMenu::Reset()
{
	Update();
}

//OFSTATUS: Complete
void COFMapInfoMenu::Update()
{
	InvalidateLayout( false, true );
}

//OFSTATUS: Incomplete
void COFMapInfoMenu::ShowPanel( bool bShow )
{
	if ( IsVisible() != bShow )
	{
		if ( bShow )
		{
			InvalidateLayout( true, true );
			Activate();
			CheckIntroState();
		}
		SetVisible( bShow );
	}
}

void COFMapInfoMenu::CheckIntroState()
{
	bool bFileExist = (OFGameRules() && g_pFullFileSystem->FileExists(OFGameRules()->GetVideoFileForMap()));
	if (bFileExist && UTIL_GetMapKeyCount("viewed") > 0)
	{
		if (m_pMapInfoWatchIntro && !m_pMapInfoWatchIntro->IsVisible())
		{
			m_pMapInfoWatchIntro->SetVisible(true);
		}
	}
	else
	{
		if (m_pMapInfoWatchIntro && m_pMapInfoWatchIntro->IsVisible())
		{
			m_pMapInfoWatchIntro->SetVisible(false);
		}
	}
}

// OFSTATUS: INCOMPLETE, this is supposed to load the mission briefing and pictures of the map
void COFMapInfoMenu::LoadMapPage()
{
}


// OFSTATUS: INCOMPLETE
void COFMapInfoMenu::ApplySchemeSettings( vgui::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	LoadControlSettings( "Resource/UI/MapInfoMenu.res" );
	SetMouseInputEnabled( true );
	CheckIntroState();
	//CheckBackContinueButtons();

	char MapName[MAX_MAP_NAME];
	V_FileBase( engine->GetLevelName(), MapName, sizeof( MapName ) );
	V_strncpy( m_szMapName, MapName, sizeof( m_szMapName ) );
	V_strupr( m_szMapName );
	LoadMapPage();
	SetDialogVariable( "mapname", GetMapDisplayName( m_szMapName ) );
	SetDialogVariable( "gamemode", g_pVGuiLocalize->Find( GetMapType( m_szMapName ) ) );
}

// OFSTATUS: INCOMPLETE, this is supossed to do some arena checks and something related to the video intro
void COFMapInfoMenu::OnCommand( const char* command )
{
	if ( V_strcmp( command, "continue" ) == 0 )
	{
		engine->ClientCmd( "team_ui_setup" );
		Close();
	}
}