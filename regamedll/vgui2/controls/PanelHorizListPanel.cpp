//========= Copyright ?1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include <assert.h>
#include <vgui_controls/ScrollBar.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>

#include <KeyValues.h>
#include <vgui/MouseCode.h>
#include <vgui/KeyCode.h>
#include <vgui/IInput.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/PanelHorizListPanel.h>
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : x - 
//			y - 
//			wide - 
//			tall - 
// Output : 
//-----------------------------------------------------------------------------
PanelHorizListPanel::PanelHorizListPanel( vgui::Panel *parent, char const *panelName ) : Panel( parent, panelName )
{
	SetBounds( 0, 0, 100, 100 );

	_hbar = new ScrollBar(this, "CPanelListPanelHScroll", false );
	_hbar->SetBounds( 0, 0, 20, 20 );
	_hbar->SetVisible(false);
	_hbar->AddActionSignalTarget( this );

	_embedded = new Panel( this, "PanelListEmbedded" );
	_embedded->SetBounds( 0, 0, 20, 20 );
	_embedded->SetPaintBackgroundEnabled( false );
	_embedded->SetPaintBorderEnabled( false );

	m_itemPad = 5;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
PanelHorizListPanel::~PanelHorizListPanel()
{
	// free data from table
	DeleteAllItems();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	PanelHorizListPanel::computeHPixelsNeeded( void )
{
	int pixels =0;
	DATAITEM *item;
	Panel *panel;
	for ( int i = 0; i < _dataItems.GetCount(); i++ )
	{
		item = _dataItems[ i ];
		if ( !item )
			continue;

		panel = item->panel;
		if ( !panel )
			continue;

		if ( !panel->IsVisible() )
			continue;

		int w = panel->GetWide();
		pixels += w + m_itemPad;
	}

	return pixels;

}

//-----------------------------------------------------------------------------
// Purpose: Returns the panel to use to render a cell
// Input  : column - 
//			row - 
// Output : Panel
//-----------------------------------------------------------------------------
Panel *PanelHorizListPanel::GetCellRenderer( int row )
{
	DATAITEM *item = _dataItems[ row ];
	if ( item )
	{
		Panel *panel = item->panel;
		return panel;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: adds an item to the view
//			data->GetName() is used to uniquely identify an item
//			data sub items are matched against column header name to be used in the table
// Input  : *item - 
//-----------------------------------------------------------------------------
int PanelHorizListPanel::AddItem( Panel *panel )
{
	InvalidateLayout();

	DATAITEM *newitem = new DATAITEM;
	newitem->panel = panel;
	panel->SetParent( _embedded );
	return _dataItems.PutElement( newitem );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : 
//-----------------------------------------------------------------------------
int	PanelHorizListPanel::GetItemCount( void )
{
	return _dataItems.GetCount();
}

//-----------------------------------------------------------------------------
// Purpose: returns pointer to data the row holds
// Input  : itemIndex - 
// Output : KeyValues
//-----------------------------------------------------------------------------
Panel *PanelHorizListPanel::GetItem(int itemIndex)
{
	if ( itemIndex < 0 || itemIndex >= _dataItems.GetCount() )
		return NULL;

	return _dataItems[itemIndex]->panel;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : itemIndex - 
// Output : DATAITEM
//-----------------------------------------------------------------------------
PanelHorizListPanel::DATAITEM *PanelHorizListPanel::GetDataItem( int itemIndex )
{
	if ( itemIndex < 0 || itemIndex >= _dataItems.GetCount() )
		return NULL;

	return _dataItems[ itemIndex ];
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : index - 
//-----------------------------------------------------------------------------
void PanelHorizListPanel::RemoveItem(int itemIndex)
{
	DATAITEM *item = _dataItems[ itemIndex ];
	delete item->panel;
	delete item;
	_dataItems.RemoveElementAt(itemIndex);

	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: clears and deletes all the memory used by the data items
//-----------------------------------------------------------------------------
void PanelHorizListPanel::DeleteAllItems()
{
	for (int i = 0; i < _dataItems.GetCount(); i++)
	{
		if ( _dataItems[i] )
		{
			delete _dataItems[i]->panel;
		}
		delete _dataItems[i];
	}
	_dataItems.RemoveAll();

	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void PanelHorizListPanel::OnMouseWheeled(int delta)
{
	int val = _hbar->GetValue();
	val -= (delta * 3 * 5);
	_hbar->SetValue(val);
}

//-----------------------------------------------------------------------------
// Purpose: relayouts out the panel after any internal changes
//-----------------------------------------------------------------------------
void PanelHorizListPanel::PerformLayout()
{
	int wide, tall;
	GetSize( wide, tall );

	int hpixels = computeHPixelsNeeded();
	int range = ( 3 * wide ) / 4;

	//!! need to make it recalculate scroll positions
	int oldPos = _hbar->GetValue();
	_hbar->SetVisible(true);
	_hbar->SetEnabled(false);
	_hbar->SetRange( 0, hpixels + range - wide );
	_hbar->SetRangeWindow( range );
	_hbar->SetButtonPressedScrollValue( 24 );
	_hbar->SetPos(0, tall - 20);
	_hbar->SetSize(wide - 2, 18);
	_hbar->InvalidateLayout();
	_hbar->SetValue( oldPos );

	int left = _hbar->GetValue();

	_embedded->SetPos( -left, 0 );
	_embedded->SetSize( hpixels, tall-20 );

	// Now lay out the controls on the embedded panel
	int x = m_itemPad;
	int w = 0;
	for ( int i = 0; i < _dataItems.GetCount(); i++ )
	{
		DATAITEM *item = _dataItems[ i ];
		if ( !item || !item->panel )
			continue;

		if ( !item->panel->IsVisible() )
			continue;

		w = item->panel->GetWide();
		item->panel->SetBounds( x, m_itemPad, w, tall - ( 2 * m_itemPad + 20 ) );
		x += w + m_itemPad;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void PanelHorizListPanel::PaintBackground()
{
	Panel::PaintBackground();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *inResourceData - 
//-----------------------------------------------------------------------------
void PanelHorizListPanel::ApplySchemeSettings(IScheme *pScheme)
{
	Panel::ApplySchemeSettings(pScheme);

	SetBorder(pScheme->GetBorder("ButtonDepressedBorder"));
	SetBgColor(GetSchemeColor("LabelBgColor", GetSchemeColor("WindowDisabledBgColor", pScheme), pScheme));

	_hbar->GetButton( 0 )->SetArmedSound( "sound/UI/buttonrollover.wav" );
	_hbar->GetButton( 0 )->SetDepressedSound( "sound/UI/buttonclick.wav" );
	_hbar->GetButton( 0 )->SetReleasedSound( "sound/UI/buttonclickrelease.wav" );
	_hbar->GetButton( 1 )->SetArmedSound( "sound/UI/buttonrollover.wav" );
	_hbar->GetButton( 1 )->SetDepressedSound( "sound/UI/buttonclick.wav" );
	_hbar->GetButton( 1 )->SetReleasedSound( "sound/UI/buttonclickrelease.wav" );
	_hbar->SetBorder( pScheme->GetBorder( "BrowserBorder" ) );

//	_labelFgColor = GetSchemeColor("WindowFgColor");
//	_selectionFgColor = GetSchemeColor("ListSelectionFgColor", _labelFgColor);
}

void PanelHorizListPanel::OnSliderMoved( int position )
{
	InvalidateLayout();
	Repaint();
}

void PanelHorizListPanel::GetSliderRange( int &low, int &high )
{
	_hbar->GetRange( low, high );
}

int PanelHorizListPanel::GetSliderPosition()
{
	return _hbar->GetValue();
}

void PanelHorizListPanel::SetSliderPosition( int position )
{
	_hbar->SetValue( position );
}

//-----------------------------------------------------------------------------
// Purpose: Message mapping 
//-----------------------------------------------------------------------------
MessageMapItem_t PanelHorizListPanel::m_MessageMap[] =
{
	MAP_MESSAGE_INT( PanelHorizListPanel, "ScrollBarSliderMoved", OnSliderMoved, "position" ),		// custom message
};

IMPLEMENT_PANELMAP(PanelHorizListPanel, BaseClass);