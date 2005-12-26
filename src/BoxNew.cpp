#include "wxDFast.h"

BEGIN_EVENT_TABLE(mBoxNew, wxDialog)
    EVT_BUTTON(XRCID("btnok"), mBoxNew::OnOk)
    EVT_BUTTON(XRCID("btncancel"), mBoxNew::OnCancel)
    EVT_BUTTON(XRCID("btndir"), mBoxNew::OnButtonDir)
END_EVENT_TABLE()

void mBoxNew::OnOk(wxCommandEvent& event)
{
    int j;
    if (XRCCTRL(*this, "edturl",wxTextCtrl)->IsEnabled())
    {
        wxString url;
        url = XRCCTRL(*this, "edturl",wxTextCtrl)->GetValue();
        mUrlName *urltmp;
        int i = 0, count = 0;
        for (i=0; i < (int)(url.Length()-1);i++)
        {
            if (url.Mid(i,1) == wxT("/"))
                count++;
        }
        if ((count < 1) || (url.Mid(url.Length()-1,1) == SEPARATOR_URL))
        {
            if (url.Mid(url.Length()-1,1) != SEPARATOR_URL)
                url = url + SEPARATOR_DIR;
            url = url + wxT("index.html");
        }
        urltmp = new mUrlName(url);
        if (!urltmp->UrlIsValid())
        {
            wxMessageBox(_("The URL is invalid!"),_("Error..."),wxOK | wxICON_ERROR,this);
            return;
        }
    }
    else
    {
        bool atleastoneitemischecked = FALSE;
        for (j = 0; j < XRCCTRL(*this, "lstdownloads",wxCheckListBox)->GetCount(); j++)
        {
            if (!XRCCTRL(*this, "lstdownloads",wxCheckListBox)->IsChecked(j))
                continue;
            atleastoneitemischecked = TRUE;
            wxString url;
            url = XRCCTRL(*this, "lstdownloads",wxCheckListBox)->GetString(j);
            mUrlName *urltmp;
            int i = 0, count = 0;
            for (i=0; i < (int)(url.Length()-1);i++)
            {
                if (url.Mid(i,1) == wxT("/"))
                    count++;
            }
            if ((count < 1) || (url.Mid(url.Length()-1,1) == SEPARATOR_URL))
            {
                if (url.Mid(url.Length()-1,1) != SEPARATOR_URL)
                    url = url + SEPARATOR_DIR;
                url = url + wxT("index.html");
            }
            urltmp = new mUrlName(url);
            if (!urltmp->UrlIsValid())
            {
                wxMessageBox(_("The follow URL is invalid:\n") + url,_("Error..."),wxOK | wxICON_ERROR,this);
                return;
            }
        }
        if (!atleastoneitemischecked)
        {
            wxMessageBox(_("Select at least one item of the address list!\n"), _("Information..."),wxOK | wxICON_INFORMATION,this);
            return;
        }
    }
    wxString destination;
    destination = XRCCTRL(*this, "edtdestination",wxTextCtrl)->GetValue();
    wxFileName *desttmp = new wxFileName(destination);
    if (!desttmp->DirExists())
    {
        wxMessageBox(_("The destination directory is invalid!"),_("Error..."),wxOK | wxICON_ERROR,this);
        return;
    }
    if (XRCCTRL(*this, "spinsplit",wxSpinCtrl)->GetValue() > MAX_NUM_PARTS)
    {
        wxMessageBox(_("I can't split the file in so many pieces!"),_("Error..."),wxOK | wxICON_ERROR,this);
        return;
    }
    EndModal(XRCID("btnok"));
}

void mBoxNew::OnCancel(wxCommandEvent& event)
{
     EndModal(XRCID("btncancel"));
}

void mBoxNew::OnButtonDir(wxCommandEvent& event)
{
     wxString dir;
     dir = wxDirSelector(_("Select the directory:"));
     if (dir != wxEmptyString)
         XRCCTRL(*this, "edtdestination",wxTextCtrl)->SetValue(dir);
}
