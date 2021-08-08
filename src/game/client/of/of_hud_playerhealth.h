// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFHudPlayerHealth
// Author(s): Cherry!
//

#pragma once

class COFHealthPanel : public vgui::Panel
{
public:
	DECLARE_CLASS_SIMPLE(COFHealthPanel, vgui::Panel);

	COFHealthPanel(Panel *parent, const char *pElementName);
	virtual void Paint();
	virtual void SetHealth(float flHealth) { m_flHealth = flHealth; }

private:

	float m_flHealth;
	int m_iHealthMaterialIndex;
	int m_iDeadMaterialIndex;
};

class COFHudPlayerHealth : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE(COFHudPlayerHealth, vgui::EditablePanel);

	COFHudPlayerHealth(Panel *parent, const char *pElementName);

	virtual void OnThink();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Reset();

	virtual void SetHealth(int iHealth, int iMaxHealth, int iMaxBuffedHealth);
	virtual const char *GetResFilename() { return "resource/UI/HudPlayerHealth.res"; }

private:

	COFHealthPanel *m_pHealthPanel;
	vgui::ImagePanel *m_pHealthImageBG;
	vgui::ImagePanel *m_pHealthBonusImage;
	vgui::ImagePanel *m_pHealthBuildingImage;

	float m_flNextThink; // field_0x184
	int m_iHealth; // 0x1d0
	int m_iMaxHealth; // 0x1d4

	int m_iBonusImageX;
	int m_iBonusImageY;
	int m_iBonusImageWidth;
	int m_iBonusImageHeight;

	bool m_bShowBuildingHealthBG; // 0x1e8

	CPanelAnimationVar(int, m_nHealthBonusPosAdj, "HealthBonusPosAdj", "25");
	CPanelAnimationVar(float, m_flHealthDeathWarning, "HealthDeathWarning", "0.49"); // field_0x1fc
	CPanelAnimationVar(Color, m_clrHealthDeathWarningColor, "HealthDeathWarningColor", "HUDDeathWarning");
	CPanelAnimationVar(int, m_iMaxHealthDamageToShow, "MaxHealthDamageToShow", "4");
};