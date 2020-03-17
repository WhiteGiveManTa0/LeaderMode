//========= Copyright ?1996-2005, Valve LLC, All rights reserved. =============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef _CHROMEHTMLWINDOW_H_
#define _CHROMEHTMLWINDOW_H_
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui/IHTML.h>
#include <vgui/IImage.h>

#if defined ( GAMEUI_EXPORTS )
#include <vgui/IPanel.h>
#include <vgui_controls/Controls.h>
#else
#include "VPanel.h"
#endif

#if defined ( GAMEUI_EXPORTS )
#include "../vgui2/src/Memorybitmap.h"
#else
#include "Memorybitmap.h"
#endif

#include <html/ihtmlchrome.h>
#include <utlstring.h>
#include "Color.h"

class IHTMLSerializer;
struct IHTMLElement;

class HtmlWindow : public vgui::IHTML,  vgui::IHTMLEvents, IHTMLResponses
{
public:
	HtmlWindow(vgui::IHTMLEvents *events, vgui::VPANEL c, HWND parent, bool AllowJavaScript, bool DirectToHWND);
	virtual ~HtmlWindow();

	virtual void OpenURL(const char *);
	virtual bool StopLoading();
	virtual bool Refresh();
	virtual void SetVisible(bool state);
	virtual bool Show( bool state );

	void CreateBrowser( bool AllowJavaScript );

	HWND GetHWND() { return m_parent; }
	HWND GetIEHWND() { return m_ieHWND;}
	HWND GetObjectHWND() { return m_oleObjectHWND;}
	vgui::IHTMLEvents *GetEvents() {return m_events;}

	virtual void OnPaint(HDC hDC);
	virtual vgui::IImage *GetBitmap() { return m_Bitmap; }

	virtual void OnSize(int x,int y, int w,int h);
	virtual void ScrollHTML(int x,int y);

	virtual void OnMouse(vgui::MouseCode code,MOUSE_STATE s,int x,int y);
	virtual void OnChar(wchar_t unichar);
	virtual void OnKeyDown(vgui::KeyCode code);
	virtual void AddText(const char *text);
	virtual void Clear();

	virtual bool OnMouseOver();

	void GetSize(int &wide,int &tall) {wide=w ; tall=h;}
	virtual void GetHTMLSize(int &wide,int &tall) {wide=html_w ; tall=html_h;}

	// events
	virtual bool OnStartURL(const char *url, const char *target, bool first);
	virtual void OnFinishURL(const char *url);
	virtual void OnProgressURL(long current, long maximum);
	virtual void OnSetStatusText(const char *text);
	virtual void OnUpdate();
	virtual void OnLink();
	virtual void OffLink();

	char *GetOpenedPage() { return m_currentUrl; }

private:
	char m_currentUrl[512]; // the currently opened URL

	long w,h; // viewport size
	long window_x, window_y;
	long html_w,html_h; // html page size
	long html_x,html_y; // html page offset
	
	HWND m_parent;  // the HWND of the vgui parent
	HWND m_oleObjectHWND; // the oleObjects window (which is inside the main window)
	HWND m_ieHWND; // the handle to the IE browser itself, which is inside oleObject
	vgui::VPANEL m_vcontext; // the vpanel of our frame, used to find out the abs pos of the panel

	
	vgui::IHTMLEvents *m_events; // place to send events to

	vgui::MemoryBitmap *m_Bitmap; // the vgui image that the page is rendered into

	bool m_bDirectToHWND;
	bool m_bOnLink;

private:
	// IHTMLResponses callbacks from the browser engine
	virtual void BrowserReady( void );
	virtual void BrowserNeedsPaint( int textureid, int wide, int tall, const unsigned char *rgba, int updatex, int updatey, int updatewide, int updatetall, int combobox_wide, int combobox_tall, const unsigned char *combobox_rgba );
	virtual void BrowserStartRequest( const char *url, const char *target, const char *postdata, bool isredirect );
	virtual void BrowserURLChanged( const char *url, const char *postdata, bool isredirect );
	virtual void BrowserFinishedRequest( const char *url, const char *pagetitle );
	virtual void BrowserShowPopup( );
	virtual void BrowserHidePopup( );
	virtual void BrowserPopupHTMLWindow( const char *url, int wide, int tall, int x, int y );
	virtual void BrowserSetHTMLTitle( const char *title );
	virtual void BrowserLoadingResource( void );
	virtual void BrowserStatusText( const char *text );
	virtual void BrowserSetCursor( int in_cursor );
	virtual void BrowserFileLoadDialog( );
	virtual void BrowserShowToolTip( const char *text );
	virtual void BrowserUpdateToolTip( const char *text );
	virtual void BrowserHideToolTip( void );
	virtual void BrowserClose( void );
	virtual void BrowserHorizontalScrollBarSizeResponse( int x, int y, int wide, int tall, int scroll, int scroll_max, float zoom );
	virtual void BrowserVerticalScrollBarSizeResponse( int x, int y, int wide, int tall, int scroll, int scroll_max, float zoom );
	virtual void BrowserGetZoomResponse( float flZoom );
	virtual void BrowserLinkAtPositionResponse( const char *url, int x, int y );
	virtual void BrowserJSAlert( const char *message );
	virtual void BrowserJSConfirm( const char *messag );
	virtual void BrowserCanGoBackandForward( bool bgoback, bool bgoforward );
	virtual void BrowserSizePopup( int x, int y, int wide, int tall );

private:
	IHTMLSerializer *m_pSerializer;

	int m_iHTMLTextureID; // vgui texture id
	// Track the texture width and height requested so we can tell
	// when the size has changed and reallocate the texture.
	int m_allocedTextureWidth;
	int m_allocedTextureHeight;

	struct ScrollData_t
	{
		ScrollData_t() 
		{
			m_bVisible = false;
			m_nX = m_nY = m_nWide = m_nTall = m_nMax = m_nScroll = 0;
		}

		bool operator==( ScrollData_t const &src ) const
		{
			return m_bVisible == src.m_bVisible && 
				m_nX == src.m_nX &&
				m_nY == src.m_nY &&
				m_nWide == src.m_nWide &&
				m_nTall == src.m_nTall &&
				m_nMax == src.m_nMax &&
				m_nScroll == src.m_nScroll;
		}

		bool operator!=( ScrollData_t const &src ) const
		{	
			return !operator==(src);
		}


		bool m_bVisible; // is the scroll bar visible
		int m_nX; /// where cef put the scroll bar
		int m_nY;
		int m_nWide;
		int m_nTall;  // how many pixels of scroll in the current scroll knob
		int m_nMax; // most amount of pixels we can scroll
		int m_nScroll; // currently scrolled amount of pixels
		float m_flZoom; // zoom level this scroll bar is for
	};

	ScrollData_t m_scrollHorizontal; // details of horizontal scroll bar
	ScrollData_t m_scrollVertical; // details of vertical scroll bar

	CUtlString m_sPendingURLLoad; // cache of url to load if we get a PostURL before the cef object is mage
	CUtlString m_sPendingPostData; // cache of the post data for above
};


#endif // _HTMLWINDOW_H_ 
