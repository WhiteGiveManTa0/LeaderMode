/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020

*/

#include "precompiled.h"

std::array<unsigned, 4U> g_rgiManpower = { 0U, 0U, 0U, 0U };
std::array<TacticalSchemes, SCHEMES_COUNT> g_rgiTeamSchemes;

int CHudScenarioStatus::Init(void)
{
	m_bitsFlags = HUD_ACTIVE;

	int iHeight = 0, iWidth = 0;

	m_iIdSpeaker = LoadDDS("texture/HUD/Items/Speaker.dds");
	m_iIdManpower = LoadDDS("texture/HUD/Items/Manpower.dds", &iWidth, &iHeight); m_flManpowerTextureRatio = float(iWidth) / float(iHeight);

	m_rgiIdSchemeTexture[Doctrine_GrandBattleplan]		= LoadDDS("texture/HUD/Schemes/Doctrine_GrandBattleplan.dds");
	m_rgiIdSchemeTexture[Doctrine_MassAssault]			= LoadDDS("texture/HUD/Schemes/Doctrine_MassAssault.dds");
	m_rgiIdSchemeTexture[Doctrine_MobileWarfare]		= LoadDDS("texture/HUD/Schemes/Doctrine_MobileWarfare.dds");
	m_rgiIdSchemeTexture[Doctrine_SuperiorFirepower]	= LoadDDS("texture/HUD/Schemes/Doctrine_SuperiorFirepower.dds");

	gHUD::AddHudElem(this);
	return 1;
}

int CHudScenarioStatus::VidInit(void)
{
	return 1;
}

void CHudScenarioStatus::Reset(void)
{
	m_rgflTimeSpeakerIconHide.fill(0.0f);
	g_rgiManpower.fill(0);
	g_rgiTeamSchemes.fill(Scheme_UNASSIGNED);
}

int CHudScenarioStatus::Draw(float fTime)
{
	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return 1;

	if (gEngfuncs.IsSpectateOnly())
		return 1;

	// Start from right next to radar.
	int x = CHudRadar::BORDER_GAP * 2 + CHudRadar::HUD_SIZE;
	int y = CHudRadar::BORDER_GAP;

	//
	Vector color = gHUD::GetColor(gHUD::m_iPlayerNum);

	const wchar_t* pwcsName = nullptr;
	int iTall = 0, iWidth = 0;

	for (size_t i = 0; i < MAX_CLIENTS; i++)
	{
		if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam != g_iTeam)
			continue;

		// hide unimportant players.
		if (g_PlayerExtraInfo[i].m_iRoleType <= Role_UNASSIGNED || g_PlayerExtraInfo[i].m_iRoleType >= ROLE_COUNT)
			continue;

		x = CHudRadar::BORDER_GAP * 2 + CHudRadar::HUD_SIZE;	// Reset X pos on each loop.

		pwcsName = UTF8ToUnicode(g_PlayerInfoList[i].name);
		gFontFuncs.GetTextSize(gHUD::m_Scoreboard.m_hPlayerNameFont, pwcsName, &iWidth, &iTall);

		// Draw class icon.
		gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
		gEngfuncs.pTriAPI->Brightness(1.0);

		// in order to make transparent fx on dds texture...
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		gEngfuncs.pTriAPI->CullFace(TRI_NONE);

		color = gHUD::GetColor(i);
		glColor4f(color.r, color.g, color.b, 1);

		glBindTexture(GL_TEXTURE_2D, gHUD::m_Radar.m_rgiRadarIcons[g_PlayerExtraInfo[i].m_iRoleType]);
		DrawUtils::Draw2DQuad(x, y, x + iTall, y + iTall);	// yeah, it's a square.

		x += iTall + GAP_PLAYERNAME_ICON;	// move to the right side of that icon.

		// Draw the name text follow.
		gFontFuncs.DrawSetTextFont(gHUD::m_Scoreboard.m_hPlayerNameFont);
		gFontFuncs.DrawSetTextPos(x, y);
		gFontFuncs.DrawSetTextColor(235, 235, 235, 255);	// have to keep the text white.
		gFontFuncs.DrawPrintText(pwcsName);

		x += iWidth + GAP_PLAYERNAME_ICON;	// move to the right side of that text.

		if (m_rgflTimeSpeakerIconHide[i] > g_flClientTime)
		{
			gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
			gEngfuncs.pTriAPI->Brightness(1.0);

			// in order to make transparent fx on dds texture...
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			glColor4f(1, 1, 1, 1);
			glBindTexture(GL_TEXTURE_2D, m_iIdSpeaker);
			DrawUtils::Draw2DQuad(x, y, x + iTall, y + iTall);	// yeah, it's a square.
		}

		y += iTall + GAP_ROWS;	// To the next row.
	}

	// Manpower Indicator
	x = CHudRadar::BORDER_GAP * 2 + CHudRadar::HUD_SIZE;	// Reset X pos on manpower indicator.
	y += GAP_PLAYERNAME_MANPOWER;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);

	// in order to make transparent fx on dds texture...
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glColor4f(1, 1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, m_iIdManpower);

	iTall = MANPOWER_ICON_SIZE;
	iWidth = round(m_flManpowerTextureRatio * float(iTall));

	// no more than 10 manpower icon. use text and numbers for the rests.
	for (int i = 0; i < Q_min(g_rgiManpower[g_iTeam], 10U); i++)
	{
		glColor4f(1, 1, 1, MANPOWER_ALPHAS[i]);
		DrawUtils::Draw2DQuad(x, y, x + iWidth, y + iTall);

		x += iWidth + GAP_MANPOWER_INTERICON;	// right shift.
	}

	if (g_rgiManpower[g_iTeam] > 10)
	{
		gFontFuncs.DrawSetTextPos(x, y);
		gFontFuncs.DrawPrintText(m_rgwcsManpowerTexts[g_iTeam].c_str());
	}

	// Scheme Indicator

	// completely move x and y to another location.
	x = CHudRadar::BORDER_GAP;
	y = CHudRadar::BORDER_GAP + CHudRadar::HUD_SIZE + 10;

	// only show this indicator when it is not disputing.
	if (g_rgiTeamSchemes[g_iTeam] != Scheme_UNASSIGNED)
	{
		glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, m_rgiIdSchemeTexture[g_rgiTeamSchemes[g_iTeam]]);
		DrawUtils::Draw2DQuad(x, y, x + 60, y + 60);
	}

	return 1;
}
