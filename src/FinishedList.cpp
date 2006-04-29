#include "wxDFast.h"

BEGIN_EVENT_TABLE(mFinishedList, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(XRCID("finishedlist"), mFinishedList::OnSelected)
    EVT_LIST_ITEM_DESELECTED(XRCID("finishedlist"), mFinishedList::OnDeselected)
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("finishedlist"), mFinishedList::OnRClick)
END_EVENT_TABLE()

void mFinishedList::OnRClick(wxListEvent& event)
{
    if (GetCurrentSelection() >= 0)
    {
        wxGetApp().mainframe->menupopup->Enable(XRCID("menuschedule"),FALSE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menustart"),FALSE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menustop"),FALSE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menuremove"),FALSE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menumove"),TRUE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menucopyurl"),TRUE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menumd5"),TRUE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menuproperties"),FALSE);
        wxGetApp().mainframe->menupopup->Enable(XRCID("menuagain"),TRUE);
        wxGetApp().mainframe->PopupMenu(wxGetApp().mainframe->menupopup,event.GetPoint());
    }
}


int mFinishedList::GetCurrentSelection()
{
    wxNotebook *notebook = XRCCTRL(*(wxGetApp().mainframe), "notebook01",wxNotebook );
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
    SelectUnselect(TRUE,event.GetIndex(),wxGetApp().mainframe);
}

void mFinishedList::OnDeselected(wxListEvent& event)
{
    SelectUnselect(FALSE,-1,wxGetApp().mainframe);
}

void mFinishedList::SelectUnselect(bool selected,int selection,mMainFrame *mainframe)
{
    wxListCtrl* infolist = XRCCTRL(*(wxGetApp().mainframe), "infolist",wxListCtrl );
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuremove"),FALSE);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuschedule"),FALSE);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustart"),FALSE);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustop"),FALSE);
       mainframe->menubar->GetMenu(1)->Enable(XRCID("menucopyurl"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuproperties"),FALSE);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menumove"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menumd5"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuagain"),selected);
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

        value = 0;
        config->Read(SIZE_REG,&value);
        infolist->SetItem(1,1,ByteString(value));

        value = 0;
        config->Read(SPEED_REG,&value);
        infolist->SetItem(2,1,ByteString(value)+wxT("/s"));

        str = wxEmptyString;
        config->Read(TIMEPASSED_REG,&str);
        infolist->SetItem(3,1,TimeString(wxstrtolonglong(str)));

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
