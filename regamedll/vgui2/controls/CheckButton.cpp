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

#include <stdarg.h>
#include <stdio.h>

#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <KeyValues.h>

#include <vgui_controls/Image.h>
#include <vgui_controls/CheckButton.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

void CheckImage::Paint()
{
	DrawSetTextFont(GetFont());

	// draw background
	if (_CheckButton->IsEnabled() && _CheckButton->m_bCheckButtonCheckable)
	{
		DrawSetTextColor(_bgColor);
	}
	else
	{
		DrawSetTextColor(_CheckButton->GetBgColor());
	}
	DrawPrintChar(0, 1, 'g');

	// draw border box
	DrawSetTextColor(_borderColor1);
	DrawPrintChar(0, 1, 'e');
	DrawSetTextColor(_borderColor2);
	DrawPrintChar(0, 1, 'f');

	// draw selected check
	if (_CheckButton->IsSelected())
	{
		DrawSetTextColor(_checkColor);
		DrawPrintChar(0, 2, 'b');
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CheckButton::CheckButton(Panel *parent, const char *panelName, const char *text) : ToggleButton(parent, panelName, text)
{
 	SetContentAlignment(a_west);
	m_bCheckButtonCheckable = true;

	// create the image
	_checkBoxImage = new CheckImage(this);

	SetTextImageIndex(1);
	SetImageAtIndex(0, _checkBoxImage, CHECK_INSET);

	m_pSelected = scheme()->GetImage( "resource/icon_checked", false );
	m_pDefault = scheme()->GetImage( "resource/icon_emptybox", false );

	_selectedFgColor = Color( 196, 181, 80, 255 );
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CheckButton::~CheckButton()
{
	delete _checkBoxImage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CheckButton::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetFgColor(GetSchemeColor("FgColor", pScheme));
	_checkBoxImage->_bgColor = GetSchemeColor("CheckBgColor", Color(62, 70, 55, 255), pScheme);
	_checkBoxImage->_borderColor1 = GetSchemeColor("CheckButtonBorder1", Color(20, 20, 20, 255), pScheme);
	_checkBoxImage->_borderColor2 = GetSchemeColor("CheckButtonBorder2", Color(90, 90, 90, 255), pScheme);
	_checkBoxImage->_checkColor = GetSchemeColor("CheckButtonCheck", Color(20, 20, 20, 255), pScheme);
	_selectedFgColor = GetSchemeColor("BrightControlText", GetSchemeColor("ControlText",pScheme), pScheme);

	// don't draw a background
	SetPaintBackgroundEnabled(false);

	SetTextImageIndex( 1 );
	if ( IsSelected() && m_pSelected )
	{
		SetImageAtIndex( 0, m_pSelected, 0 );
	}
	else if ( m_pDefault )
	{
		SetImageAtIndex( 0, m_pDefault, 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
IBorder *CheckButton::GetBorder(bool depressed, bool armed, bool selected, bool keyfocus)
{
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Check the button
//-----------------------------------------------------------------------------
void CheckButton::SetSelected(bool state)
{
	if (m_bCheckButtonCheckable)
	{
		// send a message saying we've been checked
		KeyValues *msg = new KeyValues("CheckButtonChecked", "state", (int)state);
		PostActionSignal(msg);
		
		BaseClass::SetSelected(state);

		if ( m_pDefault && !state )
		{
			SetImageAtIndex( 0, m_pDefault, 0 );
		}

		if ( m_pSelected && state )
		{
			SetImageAtIndex( 0, m_pSelected, 0 );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: sets whether or not the state of the check can be changed
//-----------------------------------------------------------------------------
void CheckButton::SetCheckButtonCheckable(bool state)
{
	m_bCheckButtonCheckable = state;
	Repaint();
}

//-----------------------------------------------------------------------------
// Purpose: Gets a different foreground text color if we are selected
//-----------------------------------------------------------------------------
Color CheckButton::GetButtonFgColor()
{
	if (IsSelected())
	{
		return _selectedFgColor;
	}

	return BaseClass::GetButtonFgColor();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CheckButton::OnCheckButtonChecked(Panel *panel)
{
}
