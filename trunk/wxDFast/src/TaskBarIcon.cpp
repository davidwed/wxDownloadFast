//
// C++ Implementation: TaskBarIcon
//
// Description: Implements the class taskbaricon, with include a icon on the tray.
//
//
// Author: Max Magalhães Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

////////////////////////XPM IMAGES////////////////////////////////
#ifndef __WXMSW__
#include "../resources/wxdfast.xpm"
#endif


BEGIN_EVENT_TABLE(mTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(HIDE,  mTaskBarIcon::OnHide)
    EVT_MENU(NEW,  mTaskBarIcon::OnNew)
    EVT_MENU(CLOSE,  mTaskBarIcon::OnClose)
    EVT_TASKBAR_LEFT_DOWN(mTaskBarIcon::OnLButtonClick)
    EVT_TASKBAR_MOVE(mTaskBarIcon::OnMouseMove)
END_EVENT_TABLE()

mTaskBarIcon::mTaskBarIcon(mMainFrame *frame)
{
    mainframe = frame;
};

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
            if  (current->GetStatus() == STATUS_ACTIVE)
            {
                totalpercentual += current->GetProgress();
                totalspeed += current->totalspeed;
                count++;
            }
        }
        if (count > 0)
        {
            taskTip.Clear();
            taskTip.Printf(_("Total Speed: %.2fKb/s\nTotal Complete: %d%%\nDownloads in progress: %d"), totalspeed/1024.0, totalpercentual/count,count);
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
    if (mainframe)
    {
        if (mainframe->IsShown())
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
    if (mainframe)
        mainframe->Close();
}

void mTaskBarIcon::OnNew(wxCommandEvent& event)
{
    if (mainframe)
        mainframe->OnNew(event);
}

void mTaskBarIcon::OnHide(wxCommandEvent& event)
{
    if (mainframe)
    {
        if (mainframe->IsShown())
        {
            if (mainframe->active)
            {
                restoring = TRUE;
                mainframe->Hide();
            }
        }
        else
        {
            restoring = TRUE;
            if (mainframe->IsIconized())
                mainframe->Iconize(FALSE);
            mainframe->Show();
            restoring = FALSE;
        }
    }
}
