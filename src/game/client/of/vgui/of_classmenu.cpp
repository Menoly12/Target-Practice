// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the class selection menu
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_classmenu.h"

COFClassMenu::COFClassMenu(IViewPort *pViewPort) : BaseClass(pViewPort)
{
	char szPanelName[256];
	for (int i = 0; i < IMAGECLASS_COUNT; i++)
	{
		V_snprintf(szPanelName, sizeof(szPanelName), "countImage%d", i);

		m_pClassCountImage[i] = new COFImagePanel(this, szPanelName);
	}

	m_pCountLabel = NULL;

	m_pClassInfoPanel = new COFClassInfoPanel(this, "ClassInfoPanel");
}

COFClassMenu::~COFClassMenu()
{
}

void COFClassMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("Resource/UI/ClassMenu.res");
}

void COFClassMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	COFLabel *pCountLabel = dynamic_cast<COFLabel*>(FindChildByName("CountLabel"));

	if (pCountLabel)
	{
		m_pCountLabel = pCountLabel;
		pCountLabel->SizeToContents();
	}
}

void COFClassMenu::ShowPanel(bool bShow)
{
	if (bShow)
	{
		engine->CheckPoint("ClassMenu");

		InvalidateLayout(true, true);
		Activate();
	}
	else
	{
		SetVisible(false);
	}
}