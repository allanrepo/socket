#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// app class
class CClient: public wxApp
{
public:
	// the 'main()'
	virtual bool OnInit();
};

// frame class
class CFrame: public wxFrame
{
public:
	CFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

};

// initialize the app here
bool CClient::OnInit()
{
	CFrame* pFrame = new CFrame("Server", wxPoint(50, 50), wxSize(450, 340));
	pFrame->Show(true);
	return true;
}

// construct the app's frame here
CFrame::CFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
}

wxIMPLEMENT_APP(CClient);
