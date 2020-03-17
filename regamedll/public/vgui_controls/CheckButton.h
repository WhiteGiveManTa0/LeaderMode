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

#ifndef CHECKBUTTON_H
#define CHECKBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/TextImage.h>

namespace vgui
{

class TextImage;

//-----------------------------------------------------------------------------
// Purpose: Check box image
//-----------------------------------------------------------------------------
class CheckImage : public TextImage
{
public:
	CheckImage(CheckButton *CheckButton) : TextImage( "g" )
	{
		_CheckButton = CheckButton;

		SetSize(20, 13);
	}

	virtual void Paint();

	Color _borderColor1;
	Color _borderColor2;
	Color _checkColor;

	Color _bgColor;

private:
	CheckButton *_CheckButton;
};

//-----------------------------------------------------------------------------
// Purpose: Tick-box button
//-----------------------------------------------------------------------------
class CheckButton : public ToggleButton
{
	DECLARE_CLASS_SIMPLE( CheckButton, ToggleButton );

public:
	CheckButton(Panel *parent, const char *panelName, const char *text);
	~CheckButton();

	// Check the button
	virtual void SetSelected(bool state);

	// sets whether or not the state of the check can be changed
	// if this is set to false, then no input in the code or by the user can change it's state
	virtual void SetCheckButtonCheckable(bool state);

protected:
	virtual void ApplySchemeSettings(IScheme *pScheme);
	MESSAGE_FUNC_PTR( OnCheckButtonChecked, "CheckButtonChecked", panel );
	virtual Color GetButtonFgColor();

	virtual IBorder *GetBorder(bool depressed, bool armed, bool selected, bool keyfocus);

	/* MESSAGES SENT
		"CheckButtonChecked" - sent when the check button state is changed
			"state"	- button state: 1 is checked, 0 is unchecked
	*/

private:
	enum { CHECK_INSET = 6 };
	CheckImage *_checkBoxImage;
	bool m_bCheckButtonCheckable;
	Color _selectedFgColor;

	IImage *m_pDefault;
	IImage *m_pSelected;
	friend class CheckImage;
};

} // namespace vgui

#endif // CHECKBUTTON_H
