/*

Created Date: Mar 12 2020

*/

#include "precompiled.h"
#include "../public/vgui/IEngineVGui.h"
#include "../public/Interface/IClientVGUI.h"

IEngineVGui* enginevgui;

class CClientVGUI : public IClientVGUI
{
public:
	virtual void Initialize(CreateInterfaceFn* factories, int count);
	virtual void Start(void);
	virtual void SetParent(vgui::VPANEL parent);
	virtual bool UseVGUI1(void);
	virtual void HideScoreBoard(void);
	virtual void HideAllVGUIMenu(void);
	virtual void ActivateClientUI(void);
	virtual void HideClientUI(void);
};

EXPOSE_SINGLE_INTERFACE(CClientVGUI, IClientVGUI, CLIENTVGUI_INTERFACE_VERSION);

void CClientVGUI::Initialize(CreateInterfaceFn* factoryList, int count)
{
	//CreateInterfaceFn vguiFactory = factories[1];

	if (!vgui::VGuiControls_Init("ClientUI", factoryList, count))
		return;

	// UNDONE, LUNA: crsky says it's not the time.
	//enginevgui = (IEngineVGui*)factoryList[0](VENGINE_VGUI_VERSION, NULL);
	//gameuifuncs = (IGameUIFuncs*)factoryList[0](VENGINE_GAMEUIFUNCS_VERSION, NULL);
}

void CClientVGUI::Start(void)
{
	g_pViewPort = new CViewport();
	g_pViewPort->Start();
}

void CClientVGUI::SetParent(vgui::VPANEL parent)
{
	g_pViewPort->SetParent(parent);
}

bool CClientVGUI::UseVGUI1(void)
{
	return false;
}

void CClientVGUI::HideScoreBoard(void)
{
	// UNDONE
	//g_pViewPort->HideScoreBoard();
}

void CClientVGUI::HideAllVGUIMenu(void)
{
/* UNDONE
	g_pViewPort->HideAllVGUIMenu();
	g_pClientVGUI->HideAllVGUIMenu();
	*/
}

void CClientVGUI::ActivateClientUI(void)
{
/* UNDONE
	g_pViewPort->ActivateClientUI();
	g_pClientVGUI->HideClientUI();
	*/
	IN_ActivateMouse();
}

void CClientVGUI::HideClientUI(void)
{
/* UNDONE
	g_pViewPort->HideClientUI();
	g_pClientVGUI->HideClientUI();
	*/
	IN_DeactivateMouse();
}
