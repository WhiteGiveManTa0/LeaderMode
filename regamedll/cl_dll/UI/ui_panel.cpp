/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

CBasePanel::CBasePanel()
{
}

CBasePanel::~CBasePanel()
{
	// deal with own parent.
	if (m_pParent)
	{
		elements_t::iterator i = m_pParent->m_lstChildren.begin();

		while (i != m_pParent->m_lstChildren.end())
		{
			auto pPanel = (*i);

			if (pPanel == this)
			{
				m_pParent->m_lstChildren.erase(i++);  // alternatively, i = m_pParent->m_lstChildren.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	m_pParent = nullptr;

	// told all children that they are orphan now.
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->m_pParent = nullptr;
		}

		m_lstChildren.clear();
	}
}

bool CBasePanel::Initialize(void)
{
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->Initialize();
		}
	}

	return true;
}

bool CBasePanel::VidInit(void)
{
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->VidInit();
		}
	}

	return true;
}

void CBasePanel::InitHUDData(void)
{
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->InitHUDData();
		}
	}
}

void CBasePanel::OnNewRound(void)
{
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->OnNewRound();
		}
	}
}

bool CBasePanel::KeyEvent(bool bDown, int iKeyIndex, const char* pszCurrentBinding)
{
	bool bSkip = true;

	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			if (!pChild->KeyEvent(bDown, iKeyIndex, pszCurrentBinding))
				bSkip = false;
		}
	}

	return bSkip;
}

void CBasePanel::Think(void)
{
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->Think();
		}
	}
}

bool CBasePanel::Draw(float flTime)
{
	float x = GetX();
	float y = GetY();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(m_flScaledColour.r, m_flScaledColour.g, m_flScaledColour.b, m_flScaledColour.a);

	DrawUtils::Draw2DQuadNoTex(x, y, x + m_flWidth, y + m_flHeight);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			if (pChild->m_bitsFlags & HUD_ACTIVE)
				pChild->Draw(flTime);
		}
	}

	return true;
}

void CBasePanel::Shutdown(void)
{
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			pChild->Shutdown();
		}
	}
}
