// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of the CExRichText
// Author(s): Cherry!
//

#include "cbase.h"
#include "of_imagepanel.h"
#include "of_richtext.h"
#include "vgui_controls/scrollbar.h"
#include "vgui_controls/scrollbarslider.h"
#include "vgui_controls/button.h"
#include "of_controls.h"

DECLARE_BUILD_FACTORY(COFRichText)

COFRichText::COFRichText(Panel *parent, const char *panelName) : BaseClass(parent, panelName)
{
	m_bUseBorders = false;

	SetCursor(vgui::dc_arrow);

	m_pSidebarUpArrow = new COFImagePanel(this, "UpArrow"); // 0x6f0
	if (m_pSidebarUpArrow)
	{
		m_pSidebarUpArrow->SetImage("chalkboard_scroll_up");
		m_pSidebarUpArrow->SetFgColor(Color(255, 255, 255, 255));
		m_pSidebarUpArrow->SetAlpha(255);
		m_pSidebarUpArrow->SetPaintBackgroundEnabled(false);
		m_pSidebarUpArrow->SetVisible(false);
	}

	m_pSidebarDownArrow = new COFImagePanel(this, "DownArrow"); // 0x6f8
	if (m_pSidebarDownArrow)
	{
		m_pSidebarDownArrow->SetImage("chalkboard_scroll_down");
		m_pSidebarDownArrow->SetFgColor(Color(255, 255, 255));
		m_pSidebarDownArrow->SetAlpha(255);
		m_pSidebarDownArrow->SetPaintBackgroundEnabled(false);
		m_pSidebarDownArrow->SetVisible(false);
	}

	// merging CreateImagePanels here instead as it looks weird

	m_pSidebarBox = new COFImagePanel(this, "Box"); // 0x6fc
	if (m_pSidebarBox)
	{
		m_pSidebarBox->SetVisible(false);
	}

	m_pSidebarLine = new COFImagePanel(this, "Line"); // 0x6f4
	if (m_pSidebarLine)
	{
		m_pSidebarLine->SetVisible(false);
	}

	vgui::ivgui()->AddTickSignal(GetVPanel());
}

void COFRichText::OnTick()
{
	if (!IsVisible()) return;

	if (!m_pSidebarUpArrow || !m_pSidebarDownArrow || !m_pSidebarBox || !m_pSidebarLine) return;

	if (_vertScrollBar && _vertScrollBar->IsVisible())
	{
		SetScrollBarImagesVisible(true);

		int iMin, iMax;
		_vertScrollBar->GetRange(iMin, iMax);
		int iValue = _vertScrollBar->GetValue();
		int iRangeWindow = _vertScrollBar->GetRangeWindow();
		int iBottomMax = iMax - iRangeWindow;
		if (iBottomMax < 0) iBottomMax = 0;

		m_pSidebarUpArrow->SetAlpha((iMin <= iValue) ? 90 : 255);
		m_pSidebarDownArrow->SetAlpha((iBottomMax <= iValue) ? 90 : 255);

		vgui::ScrollBarSlider *pSlider = _vertScrollBar->GetSlider();
		if (pSlider && pSlider->GetRangeWindow() > 0)
		{
			int x, y, width, height, min, max;
			m_pSidebarLine->GetBounds(x, y, width, height);
			pSlider->GetNobPos(min, max);

			m_pSidebarBox->SetBounds(x, y + min, width, max - min);
		}

	}
	else
	{
		SetScrollBarImagesVisible(false);
	}
}

void COFRichText::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	SetCustomFont(inResourceData->GetString("font", "Default"), m_szFont);
	SetCustomFGColor(inResourceData->GetString("fgcolor", "RichText.TextColor"), m_szFGColor);
	//V_strcpy(m_szFont, inResourceData->GetString("font", "Default"), sizeof(m_szFont));
	//V_strcpy(m_szFGColor, inResourceData->GetString("fgcolor", "RichText.TextColor"), sizeof(m_szFGColor));

	SetCustomImage(m_pSidebarUpArrow, inResourceData->GetString("image_up_arrow", "chalkboard_scroll_up"), m_szUpArrowImage);
	SetCustomImage(m_pSidebarDownArrow, inResourceData->GetString("image_down_arrow", "chalkboard_scroll_down"), m_szDownArrowImage);
	SetCustomImage(m_pSidebarLine, inResourceData->GetString("image_line", "chalkboard_scroll_line"), m_szLineImage);
	SetCustomImage(m_pSidebarBox, inResourceData->GetString("image_box", "chalkboard_scroll_box"), m_szBoxImage);
}

void COFRichText::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetFont(g_pVGuiSchemeManager->GetIScheme(GetScheme())->GetFont(m_szFont));
	SetFgColor(g_pVGuiSchemeManager->GetIScheme(GetScheme())->GetColor(m_szFGColor, Color(255, 255, 255, 255)));
	SetBorder(pScheme->GetBorder("NoBorder"));
	SetBgColor(pScheme->GetColor("Blank", Color(255, 255, 255, 255)));
	SetPanelInteractive(false);
	SetUnusedScrollbarInvisible(true);

	if (m_pSidebarUpArrow) m_pSidebarUpArrow->SetFgColor(Color(255, 255, 255, 255));
	if (m_pSidebarDownArrow) m_pSidebarDownArrow->SetFgColor(Color(255, 255, 255, 255));

	SetScrollBarImagesVisible(false);
}

void COFRichText::PerformLayout()
{
	BaseClass::PerformLayout();

	if (_vertScrollBar)
	{
		_vertScrollBar->SetZPos(500);
		m_pSidebarUpArrow->SetZPos(501);
		m_pSidebarDownArrow->SetZPos(501);

		_vertScrollBar->SetPaintBackgroundEnabled(false);
		_vertScrollBar->SetPaintBorderEnabled(false);
		_vertScrollBar->SetPaintEnabled(false);
		_vertScrollBar->SetScrollbarButtonsVisible(false);

		_vertScrollBar->GetButton(0)->SetKeyBoardInputEnabled(false);
		_vertScrollBar->GetButton(1)->SetKeyBoardInputEnabled(false);

		if (_vertScrollBar->IsVisible())
		{
			int iMin, iMax;
			_vertScrollBar->GetRange(iMin, iMax);
			_vertScrollBar->SetValue(iMax);

			int iWide, iTall;
			int iScrollBarWide = _vertScrollBar->GetWide();
			GetSize(iWide, iTall);

			if (m_pSidebarUpArrow)
			{
				m_pSidebarUpArrow->SetBounds(iWide - iScrollBarWide, 0, iScrollBarWide, iScrollBarWide);
				m_pSidebarUpArrow->SetSize(iScrollBarWide, iScrollBarWide);
			}

			if (m_pSidebarDownArrow)
			{
				m_pSidebarDownArrow->SetBounds(iWide - iScrollBarWide, iTall - iScrollBarWide, iScrollBarWide, iScrollBarWide);
				m_pSidebarDownArrow->SetSize(iScrollBarWide, iScrollBarWide);
			}

			if (m_pSidebarLine)
			{
				m_pSidebarLine->SetBounds(iWide - iScrollBarWide, iScrollBarWide, iScrollBarWide, iTall - (iScrollBarWide * 2));
			}

			if (m_pSidebarBox)
			{
				m_pSidebarBox->SetBounds(iWide - iScrollBarWide, iScrollBarWide, iScrollBarWide, iScrollBarWide);
			}

			SetScrollBarImagesVisible(false);
		}
	}
}

void COFRichText::SetScrollBarImagesVisible(bool bVisible)
{
	if (m_pSidebarUpArrow && m_pSidebarUpArrow->IsVisible() != bVisible)
	{
		m_pSidebarUpArrow->SetVisible(bVisible);
		m_pSidebarUpArrow->SetEnabled(bVisible);
	}

	if (m_pSidebarDownArrow && m_pSidebarDownArrow->IsVisible() != bVisible)
	{
		m_pSidebarDownArrow->SetVisible(bVisible);
		m_pSidebarDownArrow->SetEnabled(bVisible);
	}

	if (m_pSidebarBox && m_pSidebarBox->IsVisible() != bVisible)
	{
		m_pSidebarBox->SetVisible(bVisible);
	}

	if (m_pSidebarLine && m_pSidebarLine->IsVisible() != bVisible)
	{
		m_pSidebarLine->SetVisible(bVisible);
	}
}

void COFRichText::SetCustomFont(const char *pszFontName, char *pszFontStored)
{
	V_strcpy(pszFontStored, pszFontName);
	SetFont(g_pVGuiSchemeManager->GetIScheme(GetScheme())->GetFont(pszFontName));
}

void COFRichText::SetCustomFGColor(const char *pszColorName, char *pszColorStored)
{
	V_strcpy(pszColorStored, pszColorName);
	SetFgColor(g_pVGuiSchemeManager->GetIScheme(GetScheme())->GetColor(pszColorName, Color(255, 255, 255, 255)));
}

void COFRichText::SetCustomImage(Panel *pImage, const char *pszImageName, char *pszImageStored)
{
	if (pszImageStored) V_strcpy(pszImageStored, pszImageName);

	if (pImage)
	{
		COFImagePanel *pOFImage = dynamic_cast<COFImagePanel*>(pImage);

		if (m_bUseBorders) // field_0x6ef
		{
			vgui::IBorder *pBorder = g_pVGuiSchemeManager->GetIScheme(GetScheme())->GetBorder(pszImageName);
			if (pBorder)
			{
				pImage->SetBorder(pBorder);
				return;
			}
		}

		pOFImage->SetImage(pszImageName);
	}
}

void COFRichText::SetText(const char *text)
{
	char newtext[2048];
	V_strncpy(newtext, text, sizeof(newtext));

	for (char *character = newtext; *character != 0; character++)
	{
		if (*character == '\r') *character = ' ';
	}

	BaseClass::SetText(newtext);
}

void COFRichText::SetText(const wchar_t *text)
{
	wchar_t newtext[2048];
	V_wcsncpy(newtext, text, sizeof(newtext));

	for (wchar_t *character = newtext; *character != 0; character++)
	{
		if (*character == '\r') *character = ' ';
	}

	BaseClass::SetText(newtext);
}