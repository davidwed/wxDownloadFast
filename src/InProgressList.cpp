#include "wxDFast.h"

BEGIN_EVENT_TABLE(mInProgressList, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(XRCID("inprogresslist"), mInProgressList::OnSelected)
    EVT_LIST_ITEM_DESELECTED(XRCID("inprogresslist"), mInProgressList::OnDeselected)
    EVT_LIST_ITEM_RIGHT_CLICK(XRCID("inprogresslist"), mInProgressList::OnRClick)
END_EVENT_TABLE()

void mInProgressList::OnRClick(wxListEvent& event)
{	
	if (GetCurrentSelection() >= 0)
	{
		wxGetApp().mainframe->menupopup->Enable(XRCID("menustart"),TRUE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menustop"),TRUE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menuremove"),TRUE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menumove"),FALSE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menucopyurl"),TRUE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menumd5"),FALSE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menuproperties"),TRUE);
		wxGetApp().mainframe->menupopup->Enable(XRCID("menuagain"),FALSE);
		wxGetApp().mainframe->PopupMenu(wxGetApp().mainframe->menupopup,event.GetPoint());
	}
}

int mInProgressList::GetCurrentSelection()
{
    wxNotebook *notebook = XRCCTRL(*(wxGetApp().mainframe), "notebook01",wxNotebook );
    int selection = -1;
	if (notebook->GetSelection() == 0)
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

void mInProgressList::SetCurrentSelection(int selection)
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

void mInProgressList::OnSelected(wxListEvent& event)
{
	SelectUnselect(TRUE,event.GetIndex(),wxGetApp().mainframe);
}

void mInProgressList::OnDeselected(wxListEvent& event)
{
	SelectUnselect(FALSE,-1,wxGetApp().mainframe);
}

void mInProgressList::SelectUnselect(bool selected,int selection,mMainFrame *mainframe)
{
	if (!selected)
		SetCurrentSelection(selection);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menuremove"),selected);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustart"),selected);
    mainframe->menubar->GetMenu(0)->Enable(XRCID("menustop"),selected);
   	mainframe->menubar->GetMenu(1)->Enable(XRCID("menucopyurl"),selected);
    mainframe->menubar->GetMenu(3)->Enable(XRCID("menuproperties"),selected);
	mainframe->menubar->GetMenu(3)->Enable(XRCID("menumove"),FALSE);
	mainframe->menubar->GetMenu(3)->Enable(XRCID("menumd5"),FALSE);
	mainframe->menubar->GetMenu(3)->Enable(XRCID("menuagain"),FALSE);
    mainframe->toolbar-> EnableTool(XRCID("toolremove"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolstart"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolstop"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolup"),selected);
    mainframe->toolbar-> EnableTool(XRCID("tooldown"),selected);
    mainframe->toolbar-> EnableTool(XRCID("toolproperties"),selected);
}

int mInProgressList::Insert(mDownloadFile *current, int item)
{
    long tmp;
    if (current != NULL)
    {
        if (item == -1)
        {
           item = (*current).index;
           tmp = this->InsertItem(item, wxEmptyString,(*current).status);
           this->SetItemData(tmp, item);
           this->SetItem(item, INPROGRESS_ICON01, wxEmptyString,(*current).status);
        }
        else
        {
           wxListItem listitem;
           listitem.SetId(item);
           listitem.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
           listitem.SetColumn(INPROGRESS_ICON01);
           this->GetItem(listitem);
           listitem.SetImage((*current).status);
           this->SetItem(listitem);
        }
        if ((*current).restart == YES)
            this->SetItem(item, INPROGRESS_ICON02, _("   [ Yes ]"));
        else if ((*current).restart == NO)
            this->SetItem(item, INPROGRESS_ICON02, _("   [ No  ]"));
        else
            this->SetItem(item, INPROGRESS_ICON02, wxT("   [     ]"));
        this->SetItem(item, INPROGRESS_NAME, (*current).name);
        this->SetItem(item, INPROGRESS_SIZE, ByteString((*current).totalsize));
        this->SetItem(item, INPROGRESS_COMPLETED, ByteString((*current).totalsizecompleted));
        this->SetItem(item, INPROGRESS_PERCENTUAL, int2wxstr((*current).percentual) + wxT(" %"));
        this->SetItem(item, INPROGRESS_TIMEPASSED, TimeString((*current).timepassed));
        this->SetItem(item, INPROGRESS_TIMEREMAINING, TimeString((*current).timeremaining));
        this->SetItem(item, INPROGRESS_SPEED, ByteString((*current).totalspeed)+wxT("/s"));
        this->SetItem(item, INPROGRESS_ATTEMPTS, int2wxstr((*current).currentattempt));
        this->SetItem(item, INPROGRESS_URL, (*current).urllist);
    }
    return item;
}

void mInProgressList::RemoveItemListandFile(int item)
{
	if ((item >=0) && (item < GetItemCount()))
	{
		mDownloadFile *currentfile = wxGetApp().downloadlist.Item(item)->GetData();
		if ((item + 1) < GetItemCount())
		{
			int count = item;
			for ( mDownloadList::Node *node = wxGetApp().downloadlist.Item(item+1); node; node = node->GetNext() )
		    {
		    	node->GetData()->index = count;
		    	wxGetApp().RegisterListItemOnDisk(node->GetData());
		    	Insert(node->GetData(),count);
		    	count++;
			}
		}
		wxGetApp().downloadlist.DeleteObject(currentfile);
		DeleteItem(GetItemCount()-1);
		SelectUnselect(FALSE,-1,wxGetApp().mainframe);
	}
}
