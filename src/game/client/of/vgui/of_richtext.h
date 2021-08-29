// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the CExRichText
// Author(s): Cherry!
//

#pragma once

#include "vgui_controls/RichText.h"

class COFRichText : public vgui::RichText
{
public:
	DECLARE_CLASS_SIMPLE(COFRichText, vgui::RichText);

	COFRichText(Panel *parent, const char *panelName);

	virtual void OnTick();

	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PerformLayout();

	virtual void SetScrollBarImagesVisible(bool bVisible);
	virtual void SetCustomImage(Panel *pImage, const char *pszImageName, char *pszImageStored);
	virtual void SetCustomFont(const char *pszFontName, char *pszFontStored);
	virtual void SetCustomFGColor(const char *pszColorName, char *pszColorStored);
	virtual void SetText(const char *text);
	virtual void SetText(const wchar_t *text);

private:

	bool m_bUseBorders;

	COFImagePanel *m_pSidebarUpArrow;
	COFImagePanel *m_pSidebarDownArrow;
	COFImagePanel *m_pSidebarBox;
	COFImagePanel *m_pSidebarLine;

	char m_szFont[128];
	char m_szFGColor[128];
	char m_szUpArrowImage[128];
	char m_szDownArrowImage[128];
	char m_szLineImage[128];
	char m_szBoxImage[128];
};