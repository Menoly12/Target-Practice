// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFHudPlayerStatus, displays the health status and class portrait
// Author(s): Cherry!
//

#pragma once

class COFHudPlayerStatus : public CHudElement, public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE(COFHudPlayerStatus, vgui::EditablePanel);

	COFHudPlayerStatus(const char *pElementName);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Reset();
	//virtual bool ShouldDraw(); // just for some minigun and condition stuff

private:

	COFHudPlayerClass *m_pPlayerClassPanel;
	COFHudPlayerHealth *m_pPlayerHealthPanel;

};