//
// C++ Implementation: InProgressList
//
// Description: Implements the list of downloads in progress.
//
//
// Author: Max Magalhães Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

BEGIN_EVENT_TABLE(mInProgressList, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(XRCID("inprogresslist"), mInProgressList::OnSelected)
    EVT_LIST_ITEM_DESELECTED(XRCID("inprogresslist"), mInProgressList::OnDeselected)
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("inprogresslist"), mInProgressList::OnRClick)
    EVT_LIST_ITEM_ACTIVATED(XRCID("inprogresslist"), mInProgressList::OnDoubleClick)
    EVT_ENTER_WINDOW(mInProgressList::OnEnterWindow)
    EVT_LEAVE_WINDOW(mInProgressList::OnLeaveWindow)
END_EVENT_TABLE()

mInProgressList::mInProgressList()
{
    this->handleselectdeselectevents = TRUE;
    this->lastselection = -1;
}

void mInProgressList::OnEnterWindow(wxMouseEvent& event)
{
    mainframe->statusbar->SetStatusText(_("Double-click on the item Start/Stop the download"));
}

void mInProgressList::OnLeaveWindow(wxMouseEvent& event)
{
    mainframe->statusbar->SetStatusText(TOOLBAR_DEFAULT_MSG);
}

void mInProgressList::OnRClick(wxListEvent& event)
{
    if (this->GetCurrentSelection().GetCount() >= 0)
    {
        mainframe->menupopup->Enable(XRCID("menuschedule"),TRUE);
        mainframe->menupopup->Enable(XRCID("menustart"),TRUE);
        mainframe->menupopup->Enable(XRCID("menustop"),TRUE);
        mainframe->menupopup->Enable(XRCID("menuremove"),TRUE);
        mainframe->menupopup->Enable(XRCID("menumove"),FALSE);
        mainframe->menupopup->Enable(XRCID("menucopyurl"),TRUE);
        mainframe->menupopup->Enable(XRCID("menucopydownloaddata"),TRUE);
        mainframe->menupopup->Enable(XRCID("menumd5"),FALSE);
        mainframe->menupopup->Enable(XRCID("menuopendestination"),FALSE);
        mainframe->menupopup->Enable(XRCID("menuproperties"),TRUE);
        mainframe->menupopup->Enable(XRCID("menuagain"),FALSE);
        mainframe->PopupMenu(mainframe->menupopup,event.GetPoint());
    }
}

void mInProgressList::OnDoubleClick(wxListEvent& event)
{
    if (this->GetCurrentSelection().GetCount() > 0)
    {
        mDownloadList::Node *node = wxGetApp().downloadlist.Item(this->GetCurrentLastSelection());
        if (node)
        {
            mDownloadFile *file = node->GetData();
            wxCommandEvent event;
            int status = file->GetStatus();
            if (status == STATUS_STOPED)
                mainframe->OnStart(event);
            else if ((status == STATUS_ACTIVE) || (status == STATUS_QUEUE) || (status == STATUS_SCHEDULE_QUEUE))
                mainframe->OnStop(event);
        }
    }
}

int mInProgressList::GetCurrentLastSelection()
{
    if (this->lastselection >= this->GetItemCount())
        this->lastselection = -1;
    return this->lastselection;
}

mListSelection mInProgressList::GetCurrentSelection()
{
    wxNotebook *notebook = XRCCTRL(*(mainframe), "notebook01",wxNotebook );
    mListSelection selection;
    if (notebook->GetSelection() == 0)
    {
        int j;
        for (j = 0 ; j < this->GetItemCount();j++)
            if (this->GetItemState(j,wxLIST_STATE_SELECTED)&wxLIST_STATE_SELECTED )
                selection.Add(j);
    }
    return selection;
}

void mInProgressList::SetCurrentSelection(int selection)
{
    int i;
    for (i=0;i<GetItemCount();i++)
        this->SetItemState(i,0,wxLIST_STATE_SELECTED);
    if (selection >= 0)
        this->SetItemState(selection,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
    this->lastselection = selection;
}

void mInProgressList::OnSelected(wxListEvent& event)
{
    if (this->handleselectdeselectevents)
        this->SelectUnselect(TRUE,event.GetIndex());
}

void mInProgressList::OnDeselected(wxListEvent& event)
{
    if (this->handleselectdeselectevents)
        if (this->GetCurrentSelection().GetCount()==0)
            this->SelectUnselect(FALSE,-1);
}

void mInProgressList::SelectUnselect(bool selected,int selection)
{
    if (!selected)
        this->SetCurrentSelection(selection);
    this->lastselection = selection;
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuremove"),selected);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuschedule"),selected);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustart"),selected);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustop"),selected);
    mainframe->menubar->GetMenu(1)->Enable(XRCID("menucopyurl"),selected);
    mainframe->menubar->GetMenu(1)->Enable(XRCID("menucopydownloaddata"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuproperties"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menumove"),FALSE);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menumd5"),FALSE);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuopendestination"),FALSE);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuagain"),FALSE);
    mainframe->toolbar-> EnableTool(XRCID("toolremove"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolschedule"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolstart"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolstop"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolup"),selected);
    mainframe->toolbar-> EnableTool(XRCID("tooldown"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolproperties"),selected);
}

int mInProgressList::Insert(mDownloadFile *current, int item)
{
    long tmp;
    int currentstatus;
    if (current != NULL)
    {
        currentstatus = current->GetStatus();
        if (item == -1)
        {
           item = current->GetIndex();
           tmp = this->InsertItem(item, wxEmptyString,currentstatus);
           this->SetItemData(tmp, item);
           this->SetItem(item, INPROGRESS_ICON01, wxEmptyString,currentstatus);
        }
        else
        {
            wxListItem listitem;
            listitem.SetId(item);
            listitem.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
            listitem.SetColumn(INPROGRESS_ICON01);
            this->GetItem(listitem);
            if (currentstatus != listitem.GetImage())
            {
                listitem.SetImage(currentstatus);
                this->SetItem(listitem);
            }
        }
        if (current->RestartSupport() == YES)
            this->SetItem(item, INPROGRESS_ICON02, _("   [ Yes ]"));
        else if (current->RestartSupport() == NO)
            this->SetItem(item, INPROGRESS_ICON02, _("   [ No  ]"));
        else
            this->SetItem(item, INPROGRESS_ICON02, wxT("   [     ]"));
        this->SetItem(item, INPROGRESS_NAME, current->GetExposedName());
        this->SetItem(item, INPROGRESS_SIZE, MyUtilFunctions::ByteString(current->totalsize));
        this->SetItem(item, INPROGRESS_COMPLETED, MyUtilFunctions::ByteString(current->totalsizecompleted));
        this->SetItem(item, INPROGRESS_PERCENTUAL, MyUtilFunctions::int2wxstr(current->GetProgress()) + wxT(" %"));
        this->SetItem(item, INPROGRESS_TIMEPASSED, MyUtilFunctions::TimeString(current->timepassed));
        this->SetItem(item, INPROGRESS_TIMEREMAINING, MyUtilFunctions::TimeString(current->timeremaining));
        this->SetItem(item, INPROGRESS_SPEED, MyUtilFunctions::ByteString(current->totalspeed)+wxT("/s"));
        this->SetItem(item, INPROGRESS_ATTEMPTS, MyUtilFunctions::int2wxstr(current->GetCurrentAttempt()));
        this->SetItem(item, INPROGRESS_URL, current->GetFirstUrl().GetFullPath());
    }
    return item;
}

void mInProgressList::HandleSelectDeselectEvents(bool value)
{
    this->handleselectdeselectevents = value;
}

void mInProgressList::GenerateList(wxImageList *imageslist)
{
    wxListItem itemCol;

    this->SetImageList(imageslist, wxIMAGE_LIST_SMALL);
    itemCol.m_mask = wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE;
    itemCol.m_text = wxEmptyString;
    itemCol.m_image = -1;
    this->ClearAll();
    this->InsertColumn(INPROGRESS_ICON01, itemCol);

    itemCol.m_text = _("Restart");
    this->InsertColumn(INPROGRESS_ICON02, itemCol);

    itemCol.m_text = _("Filename");
    this->InsertColumn(INPROGRESS_NAME, itemCol);

    itemCol.m_text = _("Size");
    this->InsertColumn(INPROGRESS_SIZE, itemCol);

    itemCol.m_text = _("Completed");
    this->InsertColumn(INPROGRESS_COMPLETED, itemCol);

    itemCol.m_text = _("Percentual");
    this->InsertColumn(INPROGRESS_PERCENTUAL, itemCol);

    itemCol.m_text = _("Time Passed");
    this->InsertColumn(INPROGRESS_TIMEPASSED, itemCol);

    itemCol.m_text = _("Remaining");
    this->InsertColumn(INPROGRESS_TIMEREMAINING, itemCol);

    itemCol.m_text = _("Speed");
    this->InsertColumn(INPROGRESS_SPEED, itemCol);

    itemCol.m_text = _("Attempts");
    this->InsertColumn(INPROGRESS_ATTEMPTS, itemCol);

    itemCol.m_text = _("URL");
    this->InsertColumn(INPROGRESS_URL, itemCol);

    this->Hide();
    {
        this->SetColumnWidth(INPROGRESS_ICON01,20);
        this->SetColumnWidth(INPROGRESS_ICON02,80);
        this->SetColumnWidth(INPROGRESS_NAME,160);
        this->SetColumnWidth(INPROGRESS_SIZE,100);
        this->SetColumnWidth(INPROGRESS_COMPLETED,100);
        this->SetColumnWidth(INPROGRESS_PERCENTUAL,100);
        this->SetColumnWidth(INPROGRESS_TIMEPASSED,100);
        this->SetColumnWidth(INPROGRESS_TIMEREMAINING,100);
        this->SetColumnWidth(INPROGRESS_SPEED,100);
        this->SetColumnWidth(INPROGRESS_ATTEMPTS,100);
        this->SetColumnWidth(INPROGRESS_URL,300);
    }
    int i=0;
    bool problemwithindex = FALSE;
    for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
    {
        mDownloadFile *current = node->GetData();
        if (current->GetIndex() != i)
            problemwithindex = TRUE;
        this->Insert(current,-1);
        i++;
    }
    if (problemwithindex)
        wxGetApp().downloadlist.RecreateIndex();

    this->SelectUnselect(FALSE,-1);
    this->Show();
    this->lastselection = -1;
}
