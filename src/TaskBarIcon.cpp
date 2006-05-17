//
// C++ Implementation: TaskBarIcon
//
// Description: 
//
//
// Author: Max Magalhães Velasques <max@debiancomp1>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

////////////////////////XPM IMAGES////////////////////////////////
#include "../resources/wxdfast.xpm"


BEGIN_EVENT_TABLE(mTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(HIDE,  mTaskBarIcon::OnHide)
    EVT_MENU(NEW,  mTaskBarIcon::OnNew)
    EVT_MENU(CLOSE,  mTaskBarIcon::OnClose)
    EVT_TASKBAR_LEFT_DOWN(mTaskBarIcon::OnLButtonClick)
    EVT_TASKBAR_MOVE(mTaskBarIcon::OnMouseMove)
END_EVENT_TABLE()

void mTaskBarIcon::OnLButtonClick(wxTaskBarIconEvent&)
{
    wxCommandEvent event;
    OnHide(event);
}

void mTaskBarIcon::OnMouseMove(wxTaskBarIconEvent&)
{
    wxString taskTip = PROGRAM_NAME;
    int totalpercentual = 0,count = 0;
    float totalspeed = 0.0;
    if(!wxGetApp().downloadlist.IsEmpty())
    {
        for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
        {
            mDownloadFile *current = node->GetData();
            if  (current->status == STATUS_ACTIVE)
            {
                totalpercentual += current->percentual;
                totalspeed += current->totalspeed;
                count++;
            }
        }
        if (count > 0)
        {
            taskTip.Clear();
            taskTip.Printf(_("Total Speed = %.2fKb/s\nTotal Complete = %d%%\nDownloads in progress = %d"), totalspeed/1024.0, totalpercentual/count,count);
        }
    }
    #ifdef __WXMSW__
    SetIcon(wxICON(wxdfast_ico),taskTip);
    #else
    SetIcon(wxICON(wxdfast),taskTip);
    #endif
}

wxMenu *mTaskBarIcon::CreatePopupMenu()
{
    wxMenu *traymenu = new wxMenu;
    wxMenuItem *mnuhide;
    if (wxGetApp().mainframe)
    {
        if (wxGetApp().mainframe->IsShown())
            mnuhide = new wxMenuItem(traymenu,HIDE, _("Hide the main window"));
        else
            mnuhide = new wxMenuItem(traymenu,HIDE, _("Show the main window"));
        wxMenuItem *mnunew = new wxMenuItem(traymenu,NEW, _("New download"));
        wxMenuItem *mnuclose = new wxMenuItem(traymenu,CLOSE, _("Close the program"));
        traymenu->Append(mnuhide);
        traymenu->AppendSeparator();
        traymenu->Append(mnunew);
        traymenu->AppendSeparator();
        traymenu->Append(mnuclose);
    }
    return traymenu;
}

void mTaskBarIcon::OnClose(wxCommandEvent& event)
{
    if (wxGetApp().mainframe)
        wxGetApp().mainframe->Close();
}

void mTaskBarIcon::OnNew(wxCommandEvent& event)
{
    if (wxGetApp().mainframe)
        wxGetApp().mainframe->OnNew(event);
}

void mTaskBarIcon::OnHide(wxCommandEvent& event)
{
    if (wxGetApp().mainframe)
    {
        if (wxGetApp().mainframe->IsShown())
        {
                restoring = TRUE;
            wxGetApp().mainframe->Hide();
        }
        else
        {
                restoring = TRUE;
                    if (wxGetApp().mainframe->IsIconized())
                        wxGetApp().mainframe->Iconize(FALSE);
                wxGetApp().mainframe->Show();
                restoring = FALSE;
        }
    }
}
