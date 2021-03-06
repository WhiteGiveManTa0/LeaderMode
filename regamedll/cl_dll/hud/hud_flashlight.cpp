/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

int CHudFlashlight::Init(void)
{
	m_fFade = 0;
	m_fOn = 0;

	m_bitsFlags |= HUD_ACTIVE;

	gHUD::AddHudElem(this);

	return 1;
}

int CHudFlashlight::VidInit(void)
{
	int HUD_flash_empty = gHUD::GetSpriteIndex("flash_empty");
	int HUD_flash_full = gHUD::GetSpriteIndex("flash_full");
	int HUD_flash_beam = gHUD::GetSpriteIndex("flash_beam");

	m_hSprite1 = gHUD::GetSprite(HUD_flash_empty);
	m_hSprite2 = gHUD::GetSprite(HUD_flash_full);
	m_hBeam = gHUD::GetSprite(HUD_flash_beam);
	m_prc1 = *gHUD::GetSpriteRect(HUD_flash_empty);
	m_prc2 = *gHUD::GetSpriteRect(HUD_flash_full);
	m_prcBeam = *gHUD::GetSpriteRect(HUD_flash_beam);
	m_iWidth = m_prc2.right - m_prc2.left;

	return 1;
}

int CHudFlashlight::Draw(float flTime)
{
	if (gHUD::m_bitsHideHUDDisplay & (HIDEHUD_FLASHLIGHT | HIDEHUD_ALL))
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	if (!(gHUD::m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return 1;

	if (m_fOn)
		a = 225;
	else
		a = MIN_ALPHA;

	if (m_flBat < 0.20)
		UnpackRGB(r, g, b, RGB_REDISH);
	else
		UnpackRGB(r, g, b, RGB_YELLOWISH);

	ScaleColors(r, g, b, a);

	y = (m_prc1.bottom - m_prc2.top) / 2;
	x = ScreenWidth - m_iWidth - m_iWidth / 2;

	gEngfuncs.pfnSPR_Set(m_hSprite1, r, g, b);
	gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &m_prc1);

	if (m_fOn)
	{
		x = ScreenWidth - m_iWidth / 2;

		gEngfuncs.pfnSPR_Set(m_hBeam, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &m_prcBeam);
	}

	x = ScreenWidth - m_iWidth - m_iWidth / 2;

	int iOffset = m_iWidth * (1.0 - m_flBat);

	if (iOffset < m_iWidth)
	{
		rc = m_prc2;
		rc.left += iOffset;

		gEngfuncs.pfnSPR_Set(m_hSprite2, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x + iOffset, y, &rc);
	}

	return 1;
}

void CHudFlashlight::Reset(void)
{
	m_fFade = 0;
	m_fOn = 0;
}

void CHudFlashlight::MsgFunc_Flashlight(bool bOn, int iBattery)
{
	m_fOn = bOn;

	m_iBat = iBattery;
	m_flBat = ((float)iBattery) / 100.0f;
}

void CHudFlashlight::MsgFunc_FlashBat(int& iBattery)
{
	m_iBat = iBattery;
	m_flBat = ((float)iBattery) / 100.0f;
}
