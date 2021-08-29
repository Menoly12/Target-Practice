// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the class selection menu
// Author(s): Cherry!
//

#pragma once

#include "classmenu.h"
#include "of_controls.h"
#include "basemodelpanel.h"
#include "imagemouseoverbutton.h"

#define IMAGECLASS_COUNT 11

// so in 2008 tf2 thanks to the res files, the class model was shown with
// CTFClassInfoPanel but in live its now CTFPlayerModelPanel, problem with that is
// ITS FILLED WITH ECON COMPONENTS TO THE TOP, so im doing this my way - cherry
class COFClassInfoPanel : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE(COFClassInfoPanel, EditablePanel);

	COFClassInfoPanel(Panel *parent, const char *panelName) : BaseClass(parent, panelName)
	{
		LoadControlSettings("Resource/UI/ClassInfoPanel.res");
	}

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);

		LoadControlSettings("Resource/UI/ClassInfoPanel.res");
	}
};

// Base Class Menu
class COFClassMenu : public CClassMenu
{
public:
	DECLARE_CLASS_SIMPLE(COFClassMenu, CClassMenu);

	COFClassMenu(IViewPort *pViewPort);
	~COFClassMenu();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PerformLayout();
	virtual void ShowPanel(bool bShow);

	CImageMouseOverButton<COFClassInfoPanel> *m_pClassButtons[IMAGECLASS_COUNT];
	COFClassInfoPanel *m_pClassInfoPanel;

private:

	COFLabel *m_pCountLabel;
	COFImagePanel *m_pClassCountImage[IMAGECLASS_COUNT];
};

// Red Class Menu
class COFClassMenuRed : public COFClassMenu
{
public:
	DECLARE_CLASS_SIMPLE(COFClassMenuRed, COFClassMenu);

	COFClassMenuRed(IViewPort *pViewPort) : BaseClass(pViewPort)
	{
		m_pClassButtons[OF_CLASS_RANDOM] = new CImageMouseOverButton<COFClassInfoPanel>(this, "randompc_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SCOUT] = new CImageMouseOverButton<COFClassInfoPanel>(this, "scout_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SOLDIER] = new CImageMouseOverButton<COFClassInfoPanel>(this, "soldier_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_PYRO] = new CImageMouseOverButton<COFClassInfoPanel>(this, "pyro_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_DEMO] = new CImageMouseOverButton<COFClassInfoPanel>(this, "demoman_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_HEAVY] = new CImageMouseOverButton<COFClassInfoPanel>(this, "heavyweapons_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_ENGINEER] = new CImageMouseOverButton<COFClassInfoPanel>(this, "engineer_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_MEDIC] = new CImageMouseOverButton<COFClassInfoPanel>(this, "medic_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SNIPER] = new CImageMouseOverButton<COFClassInfoPanel>(this, "sniper_red", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SPY] = new CImageMouseOverButton<COFClassInfoPanel>(this, "spy_red", m_pClassInfoPanel);
	}

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);
		LoadControlSettings("Resource/UI/classmenu_red.res");
	}

	virtual const char *GetName() { return PANEL_CLASS_RED; }
};

// Blue Class Menu
class COFClassMenuBlue : public COFClassMenu
{
public:
	DECLARE_CLASS_SIMPLE(COFClassMenuBlue, COFClassMenu);

	COFClassMenuBlue(IViewPort *pViewPort) : BaseClass(pViewPort)
	{
		m_pClassButtons[OF_CLASS_RANDOM] = new CImageMouseOverButton<COFClassInfoPanel>(this, "randompc_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SCOUT] = new CImageMouseOverButton<COFClassInfoPanel>(this, "scout_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SOLDIER] = new CImageMouseOverButton<COFClassInfoPanel>(this, "soldier_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_PYRO] = new CImageMouseOverButton<COFClassInfoPanel>(this, "pyro_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_DEMO] = new CImageMouseOverButton<COFClassInfoPanel>(this, "demoman_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_HEAVY] = new CImageMouseOverButton<COFClassInfoPanel>(this, "heavyweapons_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_ENGINEER] = new CImageMouseOverButton<COFClassInfoPanel>(this, "engineer_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_MEDIC] = new CImageMouseOverButton<COFClassInfoPanel>(this, "medic_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SNIPER] = new CImageMouseOverButton<COFClassInfoPanel>(this, "sniper_blue", m_pClassInfoPanel);
		m_pClassButtons[OF_CLASS_SPY] = new CImageMouseOverButton<COFClassInfoPanel>(this, "spy_blue", m_pClassInfoPanel);
	}

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);
		LoadControlSettings("Resource/UI/classmenu_blue.res");
	}

	virtual const char *GetName() { return PANEL_CLASS_BLUE; }
};