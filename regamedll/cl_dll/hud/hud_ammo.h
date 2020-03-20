/*

Created Date: Mar 11 2020

*/

#pragma once

typedef struct
{
	unsigned char r, g, b, a;
}
RGBA;

class CHudAmmo : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Think(void);
	void Reset(void);

public:
	hSprite m_rghAmmoSprite[AMMO_MAXTYPE];
	wrect_t	m_rgrcAmmoSprite[AMMO_MAXTYPE];
	float m_flAlpha;
	int m_iLastDrawnClip;
	int m_iLastDrawnBpAmmo;
};
