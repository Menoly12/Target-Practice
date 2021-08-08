// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Implementation of CTFHudPlayerHealth
// Author(s): Cherry!
//

#include "cbase.h"
#include "hudelement.h"
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/ImagePanel.h"
#include "of_hud_playerhealth.h"
#include "vgui/ISurface.h"
#include "of_shareddefs.h"
#include "c_of_player.h"

COFHealthPanel::COFHealthPanel(Panel *parent, const char *pElementName) : BaseClass(parent, pElementName)
{
	m_flHealth = 1.0; // 0x154

	m_iHealthMaterialIndex = g_pVGuiSurface->DrawGetTextureId("hud/health_color"); // 0x158
	if (m_iHealthMaterialIndex == -1)
	{
		m_iHealthMaterialIndex = g_pVGuiSurface->CreateNewTextureID();
		g_pVGuiSurface->DrawSetTextureFile(m_iHealthMaterialIndex, "hud/health_color", true, false);
	}

	m_iDeadMaterialIndex = g_pVGuiSurface->DrawGetTextureId("hud/health_dead"); // 0x15c
	if (m_iDeadMaterialIndex == -1)
	{
		m_iDeadMaterialIndex = g_pVGuiSurface->CreateNewTextureID();
		g_pVGuiSurface->DrawSetTextureFile(m_iDeadMaterialIndex, "hud/health_dead", true, false);
	}
}

void COFHealthPanel::Paint()
{
	BaseClass::Paint();

	int x, y, width, height;
	GetBounds(x,y,width,height);

	vgui::Vertex_t pVertice[4];
	float uv1 = 0.0f;
	float uv2 = 1.0f;
	float flXPos = 0.0;
	float flYPos = 0.0;

	// draw health
	if (m_flHealth > 0.0)
	{
		g_pVGuiSurface->DrawSetTexture(m_iHealthMaterialIndex);

		float flYPosHealth = (1.0 - m_flHealth) * height;

		Vector2D uv11(uv1, (uv2 - m_flHealth));
		Vector2D uv21(uv2, (uv2 - m_flHealth));
		Vector2D uv22(uv2, uv2);
		Vector2D uv12(uv1, uv2);

		pVertice[0].Init(Vector2D(flXPos, flYPosHealth), uv11);
		pVertice[1].Init(Vector2D(flXPos + width, flYPosHealth), uv21);
		pVertice[2].Init(Vector2D(flXPos + width, flYPos + height), uv22);
		pVertice[3].Init(Vector2D(flXPos, flYPos + height), uv12);

		g_pVGuiSurface->DrawSetColor(GetFgColor());
	}
	else // yer dead!
	{
		g_pVGuiSurface->DrawSetTexture(m_iDeadMaterialIndex);

		Vector2D uv11(uv1, uv1);
		Vector2D uv21(uv2, uv1);
		Vector2D uv22(uv2, uv2);
		Vector2D uv12(uv1, uv2);

		pVertice[0].Init(Vector2D(flXPos, flYPos), uv11);
		pVertice[1].Init(Vector2D(flXPos + width, flYPos), uv21);
		pVertice[2].Init(Vector2D(flXPos + width, flYPos + height), uv22);
		pVertice[3].Init(Vector2D(flXPos, flYPos + height), uv12);

		g_pVGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
	}

	g_pVGuiSurface->DrawTexturedPolygon(4, pVertice);
}

COFHudPlayerHealth::COFHudPlayerHealth(Panel *parent, const char *pElementName) : BaseClass(parent, pElementName)
{
	m_pHealthPanel = new COFHealthPanel(this, "PlayerStatusHealthImage"); // 0x188
	m_pHealthImageBG = new vgui::ImagePanel(this, "PlayerStatusHealthImageBG"); // 400 = 0x190
	m_pHealthBonusImage = new vgui::ImagePanel(this, "PlayerStatusHealthBonusImage"); // 0x18c
	m_pHealthBuildingImage = new vgui::ImagePanel(this, "BuildingStatusHealthImageBG"); // 0x194

	m_flNextThink = 0;

	// beyond here is bleeding, hook bleeding, marked for death, just stuff that wont appear without econ items
}

// conditions cut
void COFHudPlayerHealth::OnThink()
{
	if (m_flNextThink > gpGlobals->curtime) return;

	COFPlayer *pPlayer = ToOFPlayer(CBasePlayer::GetLocalPlayer());
	if (pPlayer)
	{
		SetHealth(pPlayer->GetHealth(), pPlayer->GetMaxHealth(), 1000); // OFTODO: pPlayer->m_Shared.GetMaxBuffedHealth();
	}

	m_flNextThink = gpGlobals->curtime + 0.05;
}

void COFHudPlayerHealth::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	LoadControlSettings(GetResFilename());

	if (m_pHealthBonusImage)
	{
		m_pHealthBonusImage->GetBounds(m_iBonusImageX, m_iBonusImageY, m_iBonusImageWidth, m_iBonusImageHeight);
	}

	m_flNextThink = 0;

	BaseClass::ApplySchemeSettings(pScheme);

	m_pHealthBuildingImage->SetVisible(m_bShowBuildingHealthBG); // 0x1e8

	//iVar3 = vgui::Panel::FindChildByName((Panel *)this,"PlayerStatusPlayerLevel",false);
}

void COFHudPlayerHealth::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05;
	m_iHealth = -1;
	m_bShowBuildingHealthBG = false;
	//field_0x1ec = 0;
}

void COFHudPlayerHealth::SetHealth(int iHealth, int iMaxHealth, int iMaxBuffedHealth)
{
	m_iHealth = iHealth; // field_0x1d0
	m_iMaxHealth = iMaxHealth; // field_0x1d4

	m_pHealthPanel->SetHealth(clamp(float(iHealth) / float(iMaxHealth), 0, 1.0));

	if (m_pHealthPanel)
	{
		m_pHealthPanel->SetFgColor(Color(255, 255, 255, 255));
		//param_1 = *(int *)(this + 0x1d0); - this is unnecessary :v
	}

	if (m_iHealth <= 0)
	{
		if (m_pHealthImageBG->IsVisible()) m_pHealthImageBG->SetVisible(false);
		if (m_pHealthBuildingImage->IsVisible()) m_pHealthBuildingImage->SetVisible(false);
	}
	else
	{
		if (!m_pHealthImageBG->IsVisible()) m_pHealthImageBG->SetVisible(true);
		if (m_pHealthBuildingImage->IsVisible() != m_bShowBuildingHealthBG) m_pHealthBuildingImage->SetVisible(m_bShowBuildingHealthBG);

		// over heal
		if (m_iMaxHealth < m_iHealth)
		{

		}
		// low health warning
		else if (m_iHealth < (m_iMaxHealth * m_flHealthDeathWarning))
		{

		}
		else
		{
			//HideHealthBonusImage();
		}
	}

	if (m_iHealth > 0)
	{
		SetDialogVariable("Health", m_iHealth);

		if (m_iMaxHealthDamageToShow <= m_iMaxHealth - m_iHealth)
		{
			SetDialogVariable("MaxHealth", m_iMaxHealth);
		}
		else
		{
			SetDialogVariable("MaxHealth", "");
		}
	}
	else
	{
		SetDialogVariable("Health", "");
		SetDialogVariable("MaxHealth", "");
	}
}