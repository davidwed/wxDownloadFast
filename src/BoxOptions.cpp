#include "wxDFast.h"
#include "wx/colordlg.h"

BEGIN_EVENT_TABLE(mBoxOptions, wxDialog)
    EVT_BUTTON(XRCID("btnoptionsave"), mBoxOptions::OnOk)
    EVT_BUTTON(XRCID("btnoptioncancel"), mBoxOptions::OnCancel)
    EVT_BUTTON(XRCID("btnoptiondestination"), mBoxOptions::OnButtonDir)
    EVT_BUTTON(XRCID("graphbtnback"), mBoxOptions::OnGraphBackgroundColour)
    EVT_BUTTON(XRCID("graphbtngrid"), mBoxOptions::OnGraphGridColour)
    EVT_BUTTON(XRCID("graphbtnline"), mBoxOptions::OnGraphLineColour)
    EVT_BUTTON(XRCID("graphbtnfont"), mBoxOptions::OnGraphFontColour)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mBoxOptionsColorPanel, wxPanel)
    EVT_PAINT(mBoxOptionsColorPanel::OnPaint)
END_EVENT_TABLE()

void mBoxOptions::OnOk(wxCommandEvent& event)
{
	EndModal(XRCID("btnoptionsave"));
}

void mBoxOptions::OnCancel(wxCommandEvent& event)
{
     EndModal(XRCID("btnoptioncancel"));
}

void mBoxOptions::OnButtonDir(wxCommandEvent& event)
{
     wxString dir;
     dir = wxDirSelector(_("Select the directory:"));
     if (dir != wxEmptyString)
         XRCCTRL(*this, "edtdestination",wxTextCtrl)->SetValue(dir);
}

void mBoxOptions::OnGraphBackgroundColour(wxCommandEvent& event)
{
	wxColour colour = wxGetColourFromUser(this);
	if (colour.Ok())
	{
		XRCCTRL(*(this), "graphpanelback",mBoxOptionsColorPanel)->colour = colour;
		XRCCTRL(*(this), "graphpanelback",mBoxOptionsColorPanel)->Refresh();
	}
}

void mBoxOptions::OnGraphGridColour(wxCommandEvent& event)
{
	wxColour colour = wxGetColourFromUser(this);
	if (colour.Ok())
	{
		XRCCTRL(*(this), "graphpanelgrid",mBoxOptionsColorPanel)->colour = colour;
		XRCCTRL(*(this), "graphpanelgrid",mBoxOptionsColorPanel)->Refresh();
	}
}

void mBoxOptions::OnGraphLineColour(wxCommandEvent& event)
{
	wxColour colour = wxGetColourFromUser(this);
	if (colour.Ok())
	{
		XRCCTRL(*(this), "graphpanelline",mBoxOptionsColorPanel)->colour = colour;
		XRCCTRL(*(this), "graphpanelline",mBoxOptionsColorPanel)->Refresh();
	}
}

void mBoxOptions::OnGraphFontColour(wxCommandEvent& event)
{
	wxColour colour = wxGetColourFromUser(this);
	if (colour.Ok())
	{
		XRCCTRL(*(this), "graphpanelfont",mBoxOptionsColorPanel)->colour = colour;
		XRCCTRL(*(this), "graphpanelfont",mBoxOptionsColorPanel)->Refresh();
	}
}

void mBoxOptionsColorPanel::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	if (colour.Ok())
	{
	    wxBrush b(colour, wxSOLID);
	    dc.SetBackground(b);
	    dc.Clear();
	}
}
