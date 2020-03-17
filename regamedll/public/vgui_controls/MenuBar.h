//========= Copyright ?1996-2003, Valve LLC, All rights reserved. ============
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef MENUBAR_H
#define MENUBAR_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <utlvector.h>

namespace vgui
{

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class MenuBar : public Panel
{
	DECLARE_CLASS_SIMPLE( MenuBar, Panel );

public:
	MenuBar(Panel *parent, const char *panelName);
	~MenuBar();

	virtual void AddButton(MenuButton *button); // add button to end of menu list
	virtual void AddMenu( const char *pButtonName, Menu *pMenu );

protected:
	virtual void OnKeyCodeTyped(KeyCode code);
	virtual void OnKeyTyped(wchar_t unichar);
	virtual void ApplySchemeSettings(IScheme *pScheme);
	virtual void PerformLayout();
	virtual void Paint();
	MESSAGE_FUNC( OnMenuClose, "MenuClose" );
	MESSAGE_FUNC_INT( OnCursorEnteredMenuButton, "CursorEnteredMenuButton", VPanel);

private:
	CUtlVector<MenuButton *> m_pMenuButtons;
};

} // namespace vgui

#endif // MENUBAR_H

