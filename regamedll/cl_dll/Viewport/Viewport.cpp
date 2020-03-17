/*

Created Date: Mar 16 2020

*/

#include "precompiled.h"

using namespace vgui;

Panel* g_lastPanel = NULL;
Button* g_lastButton = NULL;

CViewport* g_pViewPort = NULL;

CViewport::CViewport(void) : Panel(NULL, "NewClientViewport")
{
}

CViewport::~CViewport(void)
{
}

void CViewport::Start(void)
{
	SetVisible(false);
}

void CViewport::SetParent(VPANEL parent)
{
	Panel::SetParent(parent);
}

void CViewport::SetVisible(bool state)
{
	Panel::SetVisible(state);
}
