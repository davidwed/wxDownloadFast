//
// C++ Implementation: FinishedList
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

BEGIN_EVENT_TABLE(mFinishedList, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(XRCID("finishedlist"), mFinishedList::OnSelected)
    EVT_LIST_ITEM_DESELECTED(XRCID("finishedlist"), mFinishedList::OnDeselected)
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("finishedlist"), mFinishedList::OnRClick)
    EVT_LIST_ITEM_ACTIVATED(XRCID("finishedlist"), mFinishedList::OnDoubleClick)
    EVT_ENTER_WINDOW(mFinishedList::OnEnterWindow)
    EVT_LEAVE_WINDOW(mFinishedList::OnLeaveWindow)
END_EVENT_TABLE()

int mFinishedList::CompareDates(long item1, long item2, long WXUNUSED(sortData))
{
    if (item1 < item2)
        return 1;
    if (item1 > item2)
        return -1;

    return 0;
}

void mFinishedList::OnEnterWindow(wxMouseEvent& event)
{
    mainframe->statusbar->SetStatusText(_("Double-click on the item opens the destination directory"));
}

void mFinishedList::OnLeaveWindow(wxMouseEvent& event)
{
    mainframe->statusbar->SetStatusText(TOOLBAR_DEFAULT_MSG);
}

void mFinishedList::OnRClick(wxListEvent& event)
{
    if (GetCurrentSelection() >= 0)
    {
        mainframe->menupopup->Enable(XRCID("menuschedule"),FALSE);
        mainframe->menupopup->Enable(XRCID("menustart"),FALSE);
        mainframe->menupopup->Enable(XRCID("menustop"),FALSE);
        mainframe->menupopup->Enable(XRCID("menuremove"),TRUE);
        mainframe->menupopup->Enable(XRCID("menumove"),TRUE);
        mainframe->menupopup->Enable(XRCID("menucopyurl"),TRUE);
        mainframe->menupopup->Enable(XRCID("menumd5"),TRUE);
        mainframe->menupopup->Enable(XRCID("menuopendestination"),TRUE);
        mainframe->menupopup->Enable(XRCID("menuproperties"),FALSE);
        mainframe->menupopup->Enable(XRCID("menuagain"),TRUE);
        mainframe->PopupMenu(mainframe->menupopup,event.GetPoint());
    }
}

void mFinishedList::OnDoubleClick(wxListEvent& event)
{
    mainframe->BrowserFile();
}

int mFinishedList::GetCurrentSelection()
{
    wxNotebook *notebook = XRCCTRL(*(mainframe), "notebook01",wxNotebook );
    int selection = -1;
    if (notebook->GetSelection() == 1)
    {
        int j;
        for (j = 0 ; j < this->GetItemCount();j++)
            if (this->GetItemState(j,wxLIST_STATE_SELECTED)&wxLIST_STATE_SELECTED )
            {
                selection = j;
                break;
            }
    }
    return selection;
}

void mFinishedList::SetCurrentSelection(int selection)
{
    int i;
    if (selection < 0)
    {
        for (i=0;i<GetItemCount();i++)
            SetItemState(i,0,wxLIST_STATE_SELECTED);
    }
    else
        SetItemState(selection,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
}

void mFinishedList::OnSelected(wxListEvent& event)
{
    SelectUnselect(TRUE,event.GetIndex());
}

void mFinishedList::OnDeselected(wxListEvent& event)
{
    SelectUnselect(FALSE,-1);
}

void mFinishedList::SelectUnselect(bool selected,int selection)
{
    wxListCtrl* infolist = XRCCTRL(*(mainframe), "infolist",wxListCtrl );
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuremove"),TRUE);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuschedule"),FALSE);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustart"),FALSE);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustop"),FALSE);
    mainframe->menubar->GetMenu(1)->Enable(XRCID("menucopyurl"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuproperties"),FALSE);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menumove"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menumd5"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuopendestination"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuagain"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolremove"),selected);
    if (selected)
    {
        wxFileConfig *config = new wxFileConfig(DFAST_REG);
        wxString str;
        long value;
        config->SetPath(FINISHED_REG);
        wxListItem item;
        item.SetId(selection);
        item.SetColumn(FINISHED_NAME);
        item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
        this->GetItem(item);
        config->SetPath(item.GetText());

        infolist->SetItem(0,1,item.GetText());

        str = wxEmptyString;
        config->Read(CONTENTTYPE_REG,&str);
        infolist->SetItem(1,1,str);

        str = wxEmptyString;
        config->Read(SIZE_REG,&str);
        infolist->SetItem(2,1,MyUtilFunctions::ByteString(MyUtilFunctions::wxstrtolonglong(str)) + wxT(" (") + str + wxT(" Bytes)"));

        //value = 0;
        //config->Read(SPEED_REG,&value);
        //infolist->SetItem(3,1,MyUtilFunctions::ByteString(value)+wxT("/s"));

        str = wxEmptyString;
        config->Read(TIMEPASSED_REG,&str);
        infolist->SetItem(3,1,MyUtilFunctions::TimeString(MyUtilFunctions::wxstrtolonglong(str)));

        str = wxEmptyString;
        config->Read(DESTINATION_REG,&str);
        infolist->SetItem(4,1,str);

        {
            wxDateTime date;
            value = 0;
            config->Read(START_REG,&value);
            date.Set(value);
            infolist->SetItem(5,1,date.Format());
        }

        {
            wxDateTime date;
            value = 0;
            config->Read(END_REG,&value);
            date.Set(value);
            infolist->SetItem(6,1,date.Format());
        }

        str = wxEmptyString;
        config->Read(MD5_REG,&str);
        infolist->SetItem(7,1,str);

        str = wxEmptyString;
        config->Read(URL1_REG,&str);
        infolist->SetItem(8,1,str);

        str = wxEmptyString;
        config->Read(COMMENTS_REG,&str);
        infolist->SetItem(9,1,str);

        delete config;
    }
    else
    {
        SetCurrentSelection(selection);
        infolist->SetItem(0, 1, wxEmptyString);
        infolist->SetItem(1, 1, wxEmptyString);
        infolist->SetItem(2, 1, wxEmptyString);
        infolist->SetItem(3, 1, wxEmptyString);
        infolist->SetItem(4, 1, wxEmptyString);
        infolist->SetItem(5, 1, wxEmptyString);
        infolist->SetItem(6, 1, wxEmptyString);
        infolist->SetItem(7, 1, wxEmptyString);
        infolist->SetItem(8, 1, wxEmptyString);
        infolist->SetItem(9, 1, wxEmptyString);
    }
}

void mFinishedList::GenerateList(wxListCtrl* list,wxImageList *imageslist)
{
    wxListItem itemCol;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    wxString name;
    wxString size;
    time_t enddate = 0;
    wxDateTime date;
    long index,i=0;
    int status;
    this->SetImageList(imageslist, wxIMAGE_LIST_SMALL);
    itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
    itemCol.m_text = wxEmptyString;
    itemCol.m_image = -1;
    this->ClearAll();
    this->InsertColumn(FINISHED_ICON01, itemCol);
    itemCol.m_text = _("Filename");
    this->InsertColumn(FINISHED_NAME, itemCol);
    itemCol.m_text = _("Size");
    this->InsertColumn(FINISHED_SIZE, itemCol);
    itemCol.m_text = _("Finished");
    this->InsertColumn(FINISHED_END, itemCol);

    list->ClearAll();
//    list->SetBackgroundColour(BLUE);
//    list->SetTextColour(*wxWHITE);
    list->InsertColumn(0, wxEmptyString);
    list->InsertColumn(1, wxEmptyString);
    list->SetColumnWidth(0,120);
    list->SetColumnWidth(1,400);
    list->InsertItem(0, _("Name"));
    list->InsertItem(1, _("File type"));
    list->InsertItem(2, _("Size"));
    //list->InsertItem(3, _("Speed"));
    list->InsertItem(3, _("Time"));
    list->InsertItem(4, _("Destination"));
    list->InsertItem(5, _("Start"));
    list->InsertItem(6, _("Finished"));
    list->InsertItem(7, _("MD5"));
    list->InsertItem(8, _("URLs"));
    list->InsertItem(9, _("Comments"));

    this->Hide();
    {
        this->SetColumnWidth(FINISHED_ICON01,20);
        this->SetColumnWidth(FINISHED_NAME,200);
        this->SetColumnWidth(FINISHED_SIZE,100);
        this->SetColumnWidth(FINISHED_END,120);
    }

    config->SetPath(FINISHED_REG);
    if (config->GetFirstGroup(name, index))
        do
        {
            this->InsertItem(i, name);
            i++;
        }
        while(config->GetNextGroup(name, index));

    for (i=0; i < this->GetItemCount();i++)
    {
        name = this->GetItemText(i);
        config->SetPath(name);
        config->Read(STATUS_REG,&status);
        config->Read(SIZE_REG,&size);
        config->Read(END_REG,&enddate);
        date.Set(enddate);
        this->SetItem(i, FINISHED_ICON01, wxEmptyString,status);
        this->SetItem(i, FINISHED_NAME, name);
        this->SetItem(i, FINISHED_SIZE, MyUtilFunctions::ByteString(MyUtilFunctions::wxstrtolonglong(size)));
        this->SetItem(i, FINISHED_END, date.Format());
        this->SetItemData(i,enddate);

        config->SetPath(BACK_DIR_REG);
    }
    delete config;
    this->SortItems(mFinishedList::CompareDates, 0l);
    this->Show();
}
