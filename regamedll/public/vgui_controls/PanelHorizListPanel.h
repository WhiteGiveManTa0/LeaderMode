//========= Copyright ?1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#if !defined( PANELHORIZLISTPANEL_H )
#define PANELHORIZLISTPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/Panel.h>

class KeyValues;

namespace vgui
{
class ScrollBar;


//-----------------------------------------------------------------------------
// Purpose: A list of variable height child panels
//-----------------------------------------------------------------------------
class PanelHorizListPanel : public vgui::Panel
{
	typedef vgui::Panel BaseClass;

public:
	typedef struct dataitem_s
	{
		// Always store a panel pointer
		vgui::Panel *panel;
	} DATAITEM;

	PanelHorizListPanel( vgui::Panel *parent, char const *panelName );
	~PanelHorizListPanel();

	// DATA & ROW HANDLING
	// The list now owns the panel
	virtual int	computeHPixelsNeeded( void );
	virtual int AddItem( vgui::Panel *panel );
	virtual int	GetItemCount( void );
	virtual vgui::Panel *GetItem(int itemIndex); // returns pointer to data the row holds
	virtual void RemoveItem(int itemIndex); // removes an item from the table (changing the indices of all following items)
	virtual void DeleteAllItems(); // clears and deletes all the memory used by the data items

	// PAINTING
	virtual vgui::Panel *GetCellRenderer( int row );

	virtual void OnSliderMoved( int position );

	void GetSliderRange( int &low, int &high );
	int GetSliderPosition();
	void SetSliderPosition( int position );

	DECLARE_PANELMAP();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	vgui::Panel *GetEmbedded() { return _embedded; }
	vgui::ScrollBar *GetScrollBar() { return _hbar; }

protected:

	DATAITEM	*GetDataItem( int itemIndex );

	virtual void PerformLayout();
	virtual void PaintBackground();
	virtual void OnMouseWheeled(int delta);

private:
	// list of the column headers
	vgui::Dar<DATAITEM *>	_dataItems;
	vgui::ScrollBar		*_hbar;
	vgui::Panel			*_embedded;
	int					m_itemPad;

	int					_tableStartX;
	int					_tableStartY;
};

}

#endif // PANELHORIZLISTPANEL_H