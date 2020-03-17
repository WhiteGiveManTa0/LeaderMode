/*

Created Date: Mar 16 2020

*/

#pragma once

class CViewport : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CViewport, vgui::Panel);

public:
	CViewport(void);
	~CViewport(void);

public:
	void Start(void);
	void SetParent(vgui::VPANEL parent);
	void SetVisible(bool state);
};

extern vgui::Panel* g_lastPanel;
extern vgui::Button* g_lastButton;
extern CViewport* g_pViewPort;
