#ifndef __GUICLIENT__
#define __GUICLIENT__

#include <socket.h>
#include <wx/wxprec.h>
#include <wx/listctrl.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif 

// app class
class CClientApp: public wxApp
{
public:
	virtual bool OnInit();
};


// frame class
class CClientFrame: public wxFrame
{
private:
	wxTextCtrl* m_pTextOutput;
	wxTextCtrl* m_pTextInput;
	wxButton* m_pClearBtn;
	wxTextCtrl* m_pAddrInput;
	wxTextCtrl* m_pPortInput;
	wxButton* m_pConnectBtn;
	wxListView* m_pListClient;
	wxTimer* m_pTimer;

	CClient m_Client;

public:
	CClientFrame( const wxString& title, const wxPoint& pos, const wxSize& size );
	virtual~ CClientFrame(){}

	void OnKey( wxKeyEvent& event );
	void OnTimer(wxTimerEvent& event);
	void OnConnect(wxCommandEvent& event);
};

#endif
