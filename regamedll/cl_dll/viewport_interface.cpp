/*

Created Date: Mar 12 2020

*/

#include "cl_base.h"
#include "VGUI/IEngineVGui.h"
#include "Interface/IClientVGUI.h"

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

IClientVGUI* g_pClientVGUI;

EXPOSE_SINGLE_INTERFACE(CClientVGUI, IClientVGUI, CLIENTVGUI_INTERFACE_VERSION);

// LUNA: all these vars are temperory. the true one is on the way, VGUI/Controls.cpp.
vgui::ISurface* g_pVGuiSurface;
vgui::IPanel* g_pVGuiPanel;
vgui::IInput* g_pVGuiInput;
vgui::IVGui* g_pVGui;
vgui::ISystem* g_pVGuiSystem;
vgui::ISchemeManager* g_pVGuiSchemeManager;
vgui::ILocalize* g_pVGuiLocalize;

void CClientVGUI::Initialize(CreateInterfaceFn* factoryList, int count)
{
	//CreateInterfaceFn vguiFactory = factories[1];

	g_pVGuiSurface = (vgui::ISurface*)factoryList[0](VGUI_SURFACE_INTERFACE_VERSION, NULL);
	g_pVGuiPanel = (vgui::IPanel*)factoryList[1](VGUI_PANEL_INTERFACE_VERSION, NULL);
	g_pVGuiInput = (vgui::IInput*)factoryList[1](VGUI_INPUT_INTERFACE_VERSION, NULL);
	g_pVGui = (vgui::IVGui*)factoryList[1](VGUI_IVGUI_INTERFACE_VERSION, NULL);
	g_pVGuiSystem = (vgui::ISystem*)factoryList[1](VGUI_SYSTEM_INTERFACE_VERSION, NULL);
	g_pVGuiSchemeManager = (vgui::ISchemeManager*)factoryList[1](VGUI_SCHEME_INTERFACE_VERSION, NULL);
	g_pVGuiLocalize = (vgui::ILocalize*)factoryList[1](VGUI_LOCALIZE_INTERFACE_VERSION, NULL);

	/* UNDONE
	if (!vgui::VGui_InitInterfacesList("ClientUI", factories, count))
		return;

	enginevgui = (IEngineVGui*)factories[0](VENGINE_VGUI_VERSION, NULL);
	gameuifuncs = (IGameUIFuncs*)factories[0](VENGINE_GAMEUIFUNCS_VERSION, NULL);

	g_pClientVGUI = (IClientVGUI*)((CreateInterfaceFn)gExportfuncs.ClientFactory())(CLIENTVGUI_INTERFACE_VERSION, NULL);
	g_pClientVGUI->Initialize(factories, count);
	*/
}

void CClientVGUI::Start(void)
{
	/* UNDONE
	g_pClientVGUI->Start();
	g_pViewPort = new CViewport();
	g_pViewPort->Start();
	*/
}

void CClientVGUI::SetParent(vgui::VPANEL parent)
{
/* UNDONE
	g_pClientVGUI->SetParent(parent);
	g_pViewPort->SetParent(parent);
	*/
}

bool CClientVGUI::UseVGUI1(void)
{
/* UNDONE
	return g_pClientVGUI->UseVGUI1();
	*/
	return false;
}

void CClientVGUI::HideScoreBoard(void)
{
/* UNDONE
	g_pViewPort->HideScoreBoard();
	g_pClientVGUI->HideScoreBoard();
	*/
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
}

void CClientVGUI::HideClientUI(void)
{
/* UNDONE
	g_pViewPort->HideClientUI();
	g_pClientVGUI->HideClientUI();
	*/
}
