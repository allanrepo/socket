#include <guiClient.h>


/* ---------------------------------------------------------------------------------
initialize app here
--------------------------------------------------------------------------------- */
bool CClientApp::OnInit()
{
	CClientFrame* pFrame = new CClientFrame("Client", wxPoint(50,50), wxSize(800, 600));
	pFrame->Show(true);
	return true;
}

/* ---------------------------------------------------------------------------------

--------------------------------------------------------------------------------- */
void CClientFrame::OnKey( wxKeyEvent& event )
{
	if(event.GetKeyCode()== WXK_RETURN )
	{
		wxString s = m_pTextInput->GetLineText(0);
		if (s.length())
		{
			(*m_pTextOutput) << s << "\n";
			m_pTextInput->Clear();
			m_Client.send(s.ToStdString());
		}
	}
	event.Skip ();
}

void CClientFrame::OnTimer(wxTimerEvent& event)
{	
	std::string out;
	if (!m_Client.read(out, 1))
	{
		(*m_pTextOutput) << "disconnected.\n";
	}
	else
	{
		if (out.length()) (*m_pTextOutput) << out << "\n";
	}
} 

void CClientFrame::OnConnect( wxCommandEvent& event )
{
//	m_Client.connect( m_pAddrInput->GetLineText(0).ToStdString(), m_pPortInput->GetLineText(0).ToLong() );
	m_Client.connect( IP, PORT );
}

/* ---------------------------------------------------------------------------------

--------------------------------------------------------------------------------- */
CClientFrame::CClientFrame( const wxString& title, const wxPoint& pos, const wxSize& size )
	:wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxString port; 
	port << PORT;

	// create the widgets that will contain the top sizer
	m_pAddrInput = new wxTextCtrl(this, wxID_ANY, IP, wxDefaultPosition, wxDefaultSize, 0);
	m_pPortInput = new wxTextCtrl(this, wxID_ANY, port, wxDefaultPosition, wxDefaultSize, 0);
	m_pConnectBtn = new wxButton(this, wxID_ANY, "Connect");
	wxStaticText* pAddr = new wxStaticText(this, wxID_ANY, "IP:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* pPort = new wxStaticText(this, wxID_ANY, "Port:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

	m_pConnectBtn->Bind(wxEVT_BUTTON, &CClientFrame::OnConnect, this);

	// create the top sizer with horizontal config. this means all widgets added to it will be placed
	// in the layout in horizontal row.
	wxBoxSizer *pTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// now let's add the widgets we want to place on top row. note that all margins are set for each widget.
	pTopSizer->Add(pAddr, 1, wxLEFT | wxTOP | wxEXPAND, 5);
	pTopSizer->Add(m_pAddrInput, 0, wxLEFT | wxTOP | wxEXPAND, 5);
	pTopSizer->Add(pPort, 0, wxLEFT | wxTOP | wxEXPAND, 5);
	pTopSizer->Add(m_pPortInput, 0, wxLEFT | wxTOP | wxRIGHT, 5);
	pTopSizer->Add(m_pConnectBtn, 0, wxLEFT | wxTOP | wxRIGHT, 5);

	// create the middle row sizer
	wxBoxSizer *pMidSizer = new wxBoxSizer(wxHORIZONTAL);

	// create the text control which will auto fill width of middle row and the list view that 
	// will have fixed width in the middle row
	m_pTextOutput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_pListClient = new wxListView(this, wxID_ANY,  wxDefaultPosition, wxDefaultSize);	
	m_pListClient->AppendColumn("Clients");
	m_pListClient->SetColumnWidth( 0, 160);
		
	// add the widgets in the middle row sizer
	pMidSizer->Add(m_pTextOutput, 1, wxEXPAND | wxLEFT | wxBOTTOM | wxTOP, 5);
	pMidSizer->Add(m_pListClient, 0, wxEXPAND | wxALL, 5);

	// create bottom sizer
	wxBoxSizer *pBtmSizer = new wxBoxSizer(wxHORIZONTAL);

	// create text control that will accept text input from user and press return to send the message into the 
	// text control in the middle row. 
	m_pTextInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
	m_pTextInput->SetFocus();
	m_pTextInput->Bind(wxEVT_KEY_DOWN, &CClientFrame::OnKey, this);
	m_pClearBtn = new wxButton(this, wxID_ANY, "Send");

	// add the widgets in the bottom row sizer
	pBtmSizer->Add(m_pTextInput, 1, wxLEFT | wxBOTTOM | wxEXPAND, 5);
	pBtmSizer->Add(m_pClearBtn, 0, wxLEFT | wxBOTTOM | wxRIGHT, 5);

	// finally we create the frame sizer. we set its orientation as vertical because we'll be adding the 
	// top, middle, and bottom sizers in it.
	wxBoxSizer *pFrameSizer = new wxBoxSizer(wxVERTICAL);
	pFrameSizer->Add(pTopSizer, 0, wxEXPAND, 0);
	pFrameSizer->Add(pMidSizer, 1, wxEXPAND, 0);
	pFrameSizer->Add(pBtmSizer, 0, wxEXPAND, 0);
	SetSizer(pFrameSizer);

	m_pTimer = new wxTimer( this, wxID_ANY );
	Bind(wxEVT_TIMER, &CClientFrame::OnTimer, this, m_pTimer->GetId());
	m_pTimer->Start(5000);

	Layout();
}

wxIMPLEMENT_APP(CClientApp);

