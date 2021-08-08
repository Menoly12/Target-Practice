// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFHudPlayerClass
// Author(s): Cherry!
//

#pragma once

class COFClassImage : public vgui::ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE(COFClassImage, vgui::ImagePanel);

	COFClassImage(Panel *parent, const char *pElementName) : BaseClass(parent, pElementName) {}

	virtual void SetClass(int iTeam, int iClass, int iCloak);
};

class COFHudPlayerClass : public vgui::EditablePanel, public CGameEventListener
{
public:
	DECLARE_CLASS_SIMPLE(COFHudPlayerClass, vgui::EditablePanel);

	COFHudPlayerClass(Panel *parent, const char *pElementName);

	virtual void OnThink();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Reset();
	virtual void FireGameEvent(IGameEvent *event);

private:

	int m_iClass;
	int m_iTeam;
	float m_flNextThink;

	COFClassImage *m_pPlayerClassImage;
	COFImagePanel *m_pPlayerClassImageBG;

	//COFPlayerModelPanel *m_pPlayerClassModel;
	COFImagePanel *m_pPlayerClassModelBG;
	COFImagePanel *m_pPlayerSpyImage;
	COFImagePanel *m_pPlayerSpyOutlineImage;
};