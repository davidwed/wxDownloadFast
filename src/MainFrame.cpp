//
// C++ Implementation: MainFrame
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
#include "../resources/small/stop.xpm"
#include "../resources/small/start.xpm"
#include "../resources/small/ok.xpm"
#include "../resources/small/error.xpm"
#include "../resources/small/queue.xpm"
#include "../resources/small/schedule.xpm"

BEGIN_EVENT_TABLE(mMainFrame,wxFrame)
    EVT_MENU(XRCID("menunew"),  mMainFrame::OnNew)
    EVT_MENU(XRCID("menuremove"),  mMainFrame::OnRemove)
    EVT_MENU(XRCID("menuschedule"),  mMainFrame::OnSchedule)
    EVT_MENU(XRCID("menustart"),  mMainFrame::OnStart)
    EVT_MENU(XRCID("menustop"),  mMainFrame::OnStop)
    EVT_MENU(XRCID("menustartall"),  mMainFrame::OnStartAll)
    EVT_MENU(XRCID("menustopall"),  mMainFrame::OnStopAll)
    EVT_MENU(XRCID("menuexit"),  mMainFrame::OnExit)
    EVT_MENU(XRCID("menuabout"), mMainFrame::OnAbout)
    EVT_MENU(XRCID("menulanguage"), mMainFrame::OnLanguages)
    EVT_MENU(XRCID("menudetails"), mMainFrame::OnDetails)
    EVT_MENU(XRCID("menupaste"), mMainFrame::OnPasteURL)
    EVT_MENU(XRCID("menuoptions"), mMainFrame::OnOptions)
    EVT_MENU(XRCID("menuproperties"), mMainFrame::OnProperties)
    EVT_MENU(XRCID("menufind"), mMainFrame::OnFind)
    EVT_MENU(XRCID("menuagain"), mMainFrame::OnDownloadAgain)
    EVT_MENU(XRCID("menumove"), mMainFrame::OnMove)
    EVT_MENU(XRCID("menumd5"), mMainFrame::OnCheckMD5)
    EVT_MENU(XRCID("menucopyurl"), mMainFrame::OnCopyURL)
    EVT_MENU(XRCID("menuexport"), mMainFrame::OnExportConf)
    EVT_MENU(XRCID("menuimport"), mMainFrame::OnImportConf)
    EVT_MENU(XRCID("menushutdown"), mMainFrame::OnShutdown)
    EVT_MENU(XRCID("menudisconnect"), mMainFrame::OnDisconnect)
    EVT_TOOL(-1, mMainFrame::OnToolLeftClick)
    EVT_TOOL_ENTER(-1, mMainFrame::OnToolMouseMove)
    EVT_ICONIZE(mMainFrame::OnIconize)
    EVT_TIMER(TIMER_ID, mMainFrame::OnTimer)
    EVT_CLOSE(mMainFrame::OnClose)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(mNotebook,wxNotebook)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("notebook01"),  mNotebook::OnChangePage)
END_EVENT_TABLE()

void mNotebook::OnChangePage(wxNotebookEvent& event)
{
    int oldselection = event.GetOldSelection();
    if (oldselection == 0)
        XRCCTRL(*(wxGetApp().mainframe), "inprogresslist",mInProgressList )->SelectUnselect(FALSE,-1,wxGetApp().mainframe);
    else if (oldselection == 1)
        XRCCTRL(*(wxGetApp().mainframe), "finishedlist",mFinishedList )->SelectUnselect(FALSE,-1,wxGetApp().mainframe);
    if (event.GetSelection() == 1)
    {
        XRCCTRL(*(wxGetApp().mainframe), "finishedlist",mFinishedList )->SortItems(CompareDates, 0l);
    }
    event.Skip();
};

mMainFrame::mMainFrame()
{
    imageslist = new wxImageList(16, 16, TRUE);
    wxBitmap image[6];
    int i,timerupdateinterval;
    image[0] = wxXPM(stop_xpm);
    image[1] = wxXPM(start_xpm);
    image[2] = wxXPM(ok_xpm);
    image[3] = wxXPM(error_xpm);
    image[4] = wxXPM(queue_xpm);
    image[5] = wxXPM(schedule_xpm);
    for (i=0;i<=5;i++)
       imageslist->Add(image[i]);
    wxXmlResource::Get()->LoadFrame(this,NULL, wxT("mainframe"));
    SetSize(-1,-1,600,400);
    SetTitle(PROGRAM_NAME);
    #ifdef __WXMSW__
    SetIcon(wxICON(wxdfast_ico));
    #else
    SetIcon(wxICON(wxdfast));
    #endif
    menubar = this->GetMenuBar();
    toolbar = this->GetToolBar();
    statusbar = this->GetStatusBar();

    //LOAD USER OPTIONS
    programoptions.closedialog = wxGetApp().Configurations(READ,OPT_DIALOG_CLOSE_REG,1);
    programoptions.rememberboxnewoptions = wxGetApp().Configurations(READ,OPT_REMEMBER_BOXNEW_OPTIONS_REG,1);
    programoptions.destination = wxGetApp().Configurations(READ,OPT_DESTINATION_REG,wxGetHomeDir());
    programoptions.attempts = wxGetApp().Configurations(READ,OPT_ATTEMPTS_REG,999);
    programoptions.attemptstime = wxGetApp().Configurations(READ,OPT_ATTEMPTS_TIME_REG,5);
    programoptions.simultaneous = wxGetApp().Configurations(READ,OPT_SIMULTANEOUS_REG,5);
    programoptions.alwaysshutdown = wxGetApp().Configurations(READ,OPT_SHUTDOWN_REG,0);
    programoptions.shutdown = programoptions.alwaysshutdown;
    programoptions.alwaysdisconnect = wxGetApp().Configurations(READ,OPT_DISCONNECT_REG,0);
    programoptions.timerupdateinterval = wxGetApp().Configurations(READ,OPT_TIMERINTERVAL_REG,500);
    programoptions.readbuffersize = wxGetApp().Configurations(READ,OPT_READBUFFERSIZE_REG,1024);
    programoptions.disconnect = programoptions.alwaysdisconnect;
    #ifdef __WXMSW__
    programoptions.shutdowncmd = wxGetApp().Configurations(READ,OPT_SHUTDOWN_CMD_REG,wxT("c:\\windows\\system32\\shutdown.exe -s -t 0"));
    programoptions.disconnectcmd = wxGetApp().Configurations(READ,OPT_DISCONNECT_CMD_REG,wxT("c:\\windows\\system32\\rasdial.exe /disconnect"));
    #else
    programoptions.shutdowncmd = wxGetApp().Configurations(READ,OPT_SHUTDOWN_CMD_REG,wxT("sudo /sbin/shutdown -h now"));
    programoptions.disconnectcmd = wxGetApp().Configurations(READ,OPT_DISCONNECT_CMD_REG,wxT("/usr/bin/poff"));
    #endif
    programoptions.graphshow = wxGetApp().Configurations(READ,OPT_GRAPH_SHOW_REG, 1);
    programoptions.graphhowmanyvalues = wxGetApp().Configurations(READ,OPT_GRAPH_HOWMANYVALUES_REG, 200);
    programoptions.graphrefreshtime = wxGetApp().Configurations(READ,OPT_GRAPH_REFRESHTIME_REG, 1000);
    programoptions.graphscale = wxGetApp().Configurations(READ,OPT_GRAPH_SCALE_REG, 40);
    programoptions.graphtextarea = wxGetApp().Configurations(READ,OPT_GRAPH_TEXTAREA_REG, 80);
    programoptions.graphspeedfontsize = wxGetApp().Configurations(READ,OPT_GRAPH_SPEEDFONTSIZE_REG, 18);
    programoptions.graphheight = wxGetApp().Configurations(READ,OPT_GRAPH_HEIGHT_REG, 48);
    programoptions.graphlinewidth = wxGetApp().Configurations(READ,OPT_GRAPH_LINEWIDTH_REG, 3);
    {
        wxString colour;
        long red,green,blue;
        colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORBACK_REG, wxT(""));
        if (!colour.Mid(1,3).ToLong(&red))            red = wxBLACK->Red();
        if (!colour.Mid(4,3).ToLong(&green))        green = wxBLACK->Green();
        if (!colour.Mid(7,3).ToLong(&blue))            blue = wxBLACK->Blue();
        programoptions.graphbackcolor.Set(red,green,blue);
    }
    {
        wxString colour;
        long red,green,blue;
        colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORGRID_REG, wxT(""));
        if (!colour.Mid(1,3).ToLong(&red))            red = wxGREEN->Red();
        if (!colour.Mid(4,3).ToLong(&green))        green = wxGREEN->Green();
        if (!colour.Mid(7,3).ToLong(&blue))            blue = wxGREEN->Blue();
        programoptions.graphgridcolor.Set(red,green,blue);
    }
    {
        wxString colour;
        long red,green,blue;
        colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORLINE_REG, wxT(""));
        if (!colour.Mid(1,3).ToLong(&red))            red = wxRED->Red();
        if (!colour.Mid(4,3).ToLong(&green))        green = wxRED->Green();
        if (!colour.Mid(7,3).ToLong(&blue))            blue = wxRED->Blue();
        programoptions.graphlinecolor.Set(red,green,blue);
    }
    {
        wxString colour;
        long red,green,blue;
        colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORFONT_REG, wxT(""));
        if (!colour.Mid(1,3).ToLong(&red))            red = wxBLUE->Red();
        if (!colour.Mid(4,3).ToLong(&green))        green = wxBLUE->Green();
        if (!colour.Mid(7,3).ToLong(&blue))            blue = wxBLUE->Blue();
        programoptions.graphfontcolor.Set(red,green,blue);
    }
    programoptions.activatescheduling = wxGetApp().Configurations(READ,OPT_SCHED_ACTIVATESCHEDULING_REG,0);
    programoptions.startdatetime.Set((time_t)wxGetApp().Configurations(READ,OPT_SCHED_STARTDATETIME_REG,0l));
    programoptions.finishdatetime.Set((time_t)wxGetApp().Configurations(READ,OPT_SCHED_FINISHDATETIME_REG,0l));
    programoptions.scheduleexceptionschanged = FALSE;
    for (i=0;i<MAX_SCHEDULE_EXCEPTIONS;i++)
    {
        programoptions.scheduleexceptions[i].isactive = wxGetApp().Configurations(READ,OPT_SCHED_SCHEDULEEXCEPTION_ISACTIVE_REG+int2wxstr(i),0);
        programoptions.scheduleexceptions[i].start = wxGetApp().Configurations(READ,OPT_SCHED_SCHEDULEEXCEPTION_START_REG+int2wxstr(i),wxT("00:00"));
        programoptions.scheduleexceptions[i].finish = wxGetApp().Configurations(READ,OPT_SCHED_SCHEDULEEXCEPTION_FINISH_REG+int2wxstr(i),wxT("00:00"));
        programoptions.scheduleexceptions[i].day = wxGetApp().Configurations(READ,OPT_SCHED_SCHEDULEEXCEPTION_DAY_REG+int2wxstr(i),0);
        programoptions.scheduleexceptions[i].newday = 0;
        programoptions.scheduleexceptions[i].newstart = wxEmptyString;
        programoptions.scheduleexceptions[i].newfinish = wxEmptyString;
    }
    programoptions.lastdestination = wxGetApp().Configurations(READ,OPT_LAST_DESTINATION_REG,programoptions.destination);
    programoptions.lastnumberofparts = wxGetApp().Configurations(READ,OPT_LAST_NUMBER_OF_PARTS_REG,DEFAULT_NUM_PARTS);
    programoptions.laststartoption = wxGetApp().Configurations(READ,OPT_LAST_START_OPTION_REG,DEFAULT_START_OPTION);

    menubar->GetMenu(3)->Check(XRCID("menushutdown"),programoptions.shutdown);
    menubar->GetMenu(3)->Check(XRCID("menudisconnect"),programoptions.disconnect);
    timerupdateinterval = programoptions.timerupdateinterval;

    XRCCTRL(*(this), "graphpanel",mGraph )->graph = &graph;
    XRCCTRL(*(this), "graphpanel",mGraph )->programoptions = &programoptions;
    XRCCTRL(*(this), "graphpanel",mGraph )->mainframe = this;

    //GENERATE THE LISTS
    GenerateInProgressList();
    GenerateFinishedList();

    //CREATE THE POPUPMENU FOR THE LISTS
    menupopup = new wxMenu;
    wxMenuItem *schedule = new wxMenuItem(menupopup,XRCID("menuschedule"), _("Schedule Download"));
    wxMenuItem *start = new wxMenuItem(menupopup,XRCID("menustart"), _("Start Download"));
    wxMenuItem *stop = new wxMenuItem(menupopup,XRCID("menustop"), _("Stop Download"));
    wxMenuItem *remove = new wxMenuItem(menupopup,XRCID("menuremove"), _("Remove Download"));
    wxMenuItem *properties = new wxMenuItem(menupopup,XRCID("menuproperties"), _("Properties of Download"));
    wxMenuItem *downloadagain = new wxMenuItem(menupopup,XRCID("menuagain"), _("Download File Again"));
    wxMenuItem *move = new wxMenuItem(menupopup,XRCID("menumove"), _("Move File"));
    wxMenuItem *checkmd5 = new wxMenuItem(menupopup,XRCID("menumd5"), _("Check Integrity"));
    wxMenuItem *copyurl = new wxMenuItem(menupopup,XRCID("menucopyurl"), _("Copy URL"));

    start->SetBitmap(image[1]);
    stop->SetBitmap(image[0]);
    schedule->SetBitmap(image[5]);
    remove->SetBitmap(image[3]);
    menupopup->Append(schedule);
    menupopup->Append(start);
    menupopup->Append(stop);
    menupopup->Append(remove);
    menupopup->Append(move);
    menupopup->AppendSeparator();
    menupopup->Append(copyurl);
    menupopup->Append(checkmd5);
    menupopup->Append(properties);
    menupopup->AppendSeparator();
    menupopup->Append(downloadagain);

    mutex_programoptions = new wxMutex();

    //CREATE TASKBARICON
    taskbaricon = new mTaskBarIcon();
    if (!(wxGetApp().parameters->Found(wxT("notray"))))
        #ifdef __WXMSW__
        taskbaricon->SetIcon(wxICON(wxdfast_ico),PROGRAM_NAME);
        #else
        taskbaricon->SetIcon(wxICON(wxdfast),PROGRAM_NAME);
        #endif
    taskbaricon->restoring = FALSE;

    //HIDE OR SHOW THE SPEED GRAPH
    if (!programoptions.graphshow)
        XRCCTRL(*(this), "graphpanel",mGraph )->Hide();
    else
        XRCCTRL(*(this), "graphpanel",mGraph )->Show();

    //DEFINE THE STATUSBAR DEFAULT TEXT
    statusbar->SetStatusText(_("Visit http://dfast.sf.net for updates"));

    mtimer = new wxTimer(this, TIMER_ID);
    timerinterval = 0;
    mtimer->Start(timerupdateinterval);
}

void mMainFrame::OnIconize(wxIconizeEvent& event)
{
    if ((wxGetApp().parameters->Found(wxT("notray"))))
           event.Skip();
    else
    {
        if ((event.Iconized()) && (taskbaricon->restoring == FALSE))
        {
            wxCommandEvent eventtmp;
            taskbaricon->OnHide(eventtmp);
        }
    }
}

mMainFrame::~mMainFrame()
{
    wxSplitterWindow *splitter01 = XRCCTRL(*this, "splitter01",wxSplitterWindow);
    wxSplitterWindow *splitter02 = XRCCTRL(*this, "splitter02",wxSplitterWindow);
    if (splitter01->IsSplit())
    {
        wxGetApp().Configurations(WRITE,SEPARATOR01_REG,splitter01->GetSashPosition());
        if (splitter01->IsSplit())
            wxGetApp().Configurations(WRITE,SEPARATOR02_REG,splitter02->GetSashPosition());
        wxGetApp().Configurations(WRITE,DETAILS_REG,TRUE);
    }
    else
        wxGetApp().Configurations(WRITE,DETAILS_REG,FALSE);
    if (!IsIconized())
    {
        int x,y,width,height,maximized;
        GetSize(&width,&height);
        GetPosition(&x,&y);
        maximized = IsMaximized();
        if (!maximized)
        {
            wxGetApp().Configurations(WRITE,SIZE_X_REG,width);
            wxGetApp().Configurations(WRITE,SIZE_Y_REG,height);
            wxGetApp().Configurations(WRITE,POS_X_REG,x);
            wxGetApp().Configurations(WRITE,POS_Y_REG,y);
        }
        wxGetApp().Configurations(WRITE,MAXIMIZED_REG,maximized);
    }
    delete taskbaricon;
    delete mtimer;
}

void mMainFrame::OnTimer(wxTimerEvent& event)
{
    if (mutex_programoptions->TryLock() != wxMUTEX_NO_ERROR)
        return;
    mInProgressList* list01 = XRCCTRL(*this, "inprogresslist",mInProgressList );
    mFinishedList* list02 = XRCCTRL(*this, "finishedlist",mFinishedList );
    int selection = list01->GetCurrentSelection();
    int simultaneous = programoptions.simultaneous;
    bool somedownloadfinishednow = FALSE;
    long currentspeed = 0;
    mDownloadFile *current;
    #ifdef USE_HTML_MESSAGES
    if ((selection < 0) && (*(XRCCTRL(*(this), "messages",wxHtmlWindow)->GetParser()->GetSource()) != wxEmptyString))
        XRCCTRL(*(this), "messages",wxHtmlWindow)->SetPage(wxEmptyString);
    #else
    if ((selection < 0) && (XRCCTRL(*(this), "messages",wxTextCtrl)->GetValue() != wxEmptyString))
        XRCCTRL(*(this), "messages",wxTextCtrl)->Clear();
    #endif
    if ((selection < 0) &&  (((int)XRCCTRL(*(this), "treemessages",wxTreeCtrl)->GetCount()) > 0))
        XRCCTRL(*(this), "treemessages",wxTreeCtrl)->DeleteAllItems();
    for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
    {
        current = node->GetData();
        if ((current->status == STATUS_FINISHED) && (current->free))
        {
            int i = list02->GetItemCount();
            int index = current->index;
            mUrlName url(current->urllist);
            list02->InsertItem(i, wxEmptyString);
            list02->SetItem(i, FINISHED_ICON01, wxEmptyString,current->status);
            list02->SetItem(i, FINISHED_NAME, current->name);
            list02->SetItem(i, FINISHED_SIZE, ByteString(current->totalsize));
            list02->SetItem(i, FINISHED_END, current->end.Format());
            list02->SetItemData(i,current->end.GetTicks());
            wxGetApp().RegisterListItemOnDisk(current);
            if (url.GetFullName().AfterLast('.').Lower() == wxT("html"))
                wxLaunchDefaultBrowser(url.GetFullPath());
            if (index > 0)
                node = node->GetPrevious();  //GO TO THE PREVIOUS NODE BEFORE DELETE THE CURRENT ONE
            list01->RemoveItemListandFile(index);
            somedownloadfinishednow = TRUE;
            if (index == 0)  //IF THIS IS THE FIRST NODE, WE CANN'T GO TO THE PREVIOUS ONE
               break;
        }
        else
        {
            int parts;
            if (current->split == YES)
                parts = current->parts;
            else
                parts = 1;
            if ((!current->free) || (current->status == STATUS_ACTIVE))
            {
                if ((current->free) && (current->status == STATUS_ACTIVE))
                {
                    current->status = STATUS_STOPED;
                    wxGetApp().RegisterListItemOnDisk(current);
                }
                if ((current->free) && (current->scheduled) && (current->status == STATUS_STOPED))
                {
                    current->status = STATUS_SCHEDULE;
                    wxGetApp().RegisterListItemOnDisk(current);
                }
                if (current->status == STATUS_ACTIVE)
                    simultaneous--;
                if (current->split != WAIT)
                {
                    int i;
                    current->totalsizecompleted = 0;

                    for (i=0; i < parts; i++)
                        current->totalsizecompleted += current->sizecompleted[i];
                    if (current->totalspeed > 0)
                        current->timeremaining = wxLongLong(0l,1000l)*(current->totalsize - current->totalsizecompleted)/wxLongLong(0l,current->totalspeed);
                    if (current->totalsize > 0)
                        current->percentual = (int)(100*( wxlonglongtodouble(current->totalsizecompleted)) / ( wxlonglongtodouble(current->totalsize)));
                }
                list01->Insert(current,current->index);
                currentspeed += current->totalspeed;
            }
            else //VERIFY IF THE IMAGE OF THE ITEM HAS CHANGED
            {
                wxListItem listitem;
                   listitem.SetId(current->index);
                   listitem.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
                   listitem.SetColumn(INPROGRESS_ICON01);
                   list01->GetItem(listitem);
                   if (current->status != listitem.GetImage())
                       list01->Insert(current,current->index);

            }
            if (selection == current->index)
            {
                long treeindex = 0;
                if  (((int)XRCCTRL(*(this), "treemessages",wxTreeCtrl)->GetCount()) != parts)
                {
                    wxTreeCtrl *tree = XRCCTRL(*(this), "treemessages",wxTreeCtrl);
                    wxTreeItemId root;
                    tree->DeleteAllItems();
                    root = tree->AddRoot(wxEmptyString);
                    for (int i=0;i<parts;i++)
                        tree->AppendItem(root,_("Piece ") + int2wxstr(i+1));
                    treeindex = 0;
                    tree->Refresh();
                }
                else
                {
                    wxTreeCtrl *tree = XRCCTRL(*(this), "treemessages",wxTreeCtrl);
                    wxString text = tree->GetItemText(tree->GetSelection());
                    text.Mid(text.Length()-1,1).ToLong(&treeindex);
                    treeindex--;
                    if ((treeindex < 0) || (treeindex >= parts))
                    {
                        treeindex = 0;
                    }
                }
                #ifdef USE_HTML_MESSAGES
                if  (*(XRCCTRL(*(this), "messages",wxHtmlWindow)->GetParser()->GetSource()) != current->messages[treeindex])
                {
                    int x,y,xx,yy;
                    XRCCTRL(*(this), "messages",wxHtmlWindow)->SetPage(wxEmptyString);
                    XRCCTRL(*(this), "messages",wxHtmlWindow)->AppendToPage(current->messages[treeindex]);
                    XRCCTRL(*(this), "messages",wxHtmlWindow)->GetScrollPixelsPerUnit(&x,&y);
                    XRCCTRL(*(this), "messages",wxHtmlWindow)->GetVirtualSize(&xx,&yy);
                    xx /= x;
                    yy /= y;
                    XRCCTRL(*(this), "messages",wxHtmlWindow)->Scroll(-1,yy);
                }

                #else
                if  (XRCCTRL(*(this), "messages",wxTextCtrl)->GetValue() != current->messages[treeindex])
                {
                    XRCCTRL(*(this), "messages",wxTextCtrl)->Clear();
                    XRCCTRL(*(this), "messages",wxTextCtrl)->SetValue(current->messages[treeindex]);
                    XRCCTRL(*(this), "messages",wxTextCtrl)->ShowPosition(current->messages[treeindex].Length()-200);

                }
                   #endif
            }
        }
    }

    if (programoptions.scheduleexceptionschanged)
    {
        programoptions.scheduleexceptionschanged = FALSE;
        for (int i=0;i<MAX_SCHEDULE_EXCEPTIONS;i++)
        {
            if (programoptions.scheduleexceptions[i].newstart != wxEmptyString)
            {
                programoptions.scheduleexceptions[i].start = programoptions.scheduleexceptions[i].newstart;
                programoptions.scheduleexceptions[i].finish = programoptions.scheduleexceptions[i].newfinish;
                programoptions.scheduleexceptions[i].day = programoptions.scheduleexceptions[i].newday;
                programoptions.scheduleexceptions[i].isactive = TRUE;
            }
            else
                programoptions.scheduleexceptions[i].isactive = FALSE;
        }
    }

    if (programoptions.activatescheduling)
    {
        wxDateTime now = wxDateTime::Now();
        wxDateTime exceptionstart,exceptionfinish;
        long hour,minute;
        bool stopscheduled = FALSE;
        bool exceptionhappened = FALSE;

        if ((now > programoptions.startdatetime) && (now < programoptions.finishdatetime))
        {
            for (int i = 0;i < MAX_SCHEDULE_EXCEPTIONS;i++)
            {
                if ((programoptions.scheduleexceptions[i].day == now.GetWeekDay()) && (programoptions.scheduleexceptions[i].isactive))
                {
                    programoptions.scheduleexceptions[i].start.Mid(0,2).ToLong(&hour);
                    programoptions.scheduleexceptions[i].start.Mid(3,2).ToLong(&minute);
                    exceptionstart.Set(hour,minute);
                    programoptions.scheduleexceptions[i].finish.Mid(0,2).ToLong(&hour);
                    programoptions.scheduleexceptions[i].finish.Mid(3,2).ToLong(&minute);
                    exceptionfinish.Set(hour,minute);
                    if ((now > exceptionstart) && (now < exceptionfinish))
                    {
                        stopscheduled = TRUE;
                        exceptionhappened = TRUE;
                        break;
                    }
                }
            }

            if ((simultaneous > 0) && (!stopscheduled))
            {
                for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
                {
                    current = node->GetData();
                    if (current->status == STATUS_SCHEDULE)
                    {
                        if (StartDownload(current))
                        {
                            simultaneous--;
                            if (simultaneous <= 0)
                                break;
                        }
                    }
                }
            }
        }
        else
            stopscheduled = TRUE;
        if (stopscheduled)
        {
            mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
            if (now > programoptions.finishdatetime)
                programoptions.activatescheduling = FALSE;
            for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
            {
                current = node->GetData();
                if ((current->status == STATUS_ACTIVE) && (current->scheduled))
                {
                    list->SetCurrentSelection(current->index);
                    StopDownload(current);
                    current->scheduled = TRUE;
                    if (!exceptionhappened)
                        somedownloadfinishednow = TRUE;
                }
            }
        }
    }
    if (simultaneous > 0)
    {
        for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
        {
            current = node->GetData();
            if ((current->status == STATUS_QUEUE) && (!current->scheduled))
            {
                if (StartDownload(current))
                {
                    simultaneous--;
                    if (simultaneous <= 0)
                        break;
                }
            }
        }
    }
    if (somedownloadfinishednow)
    {
        bool somedownloadactive = FALSE;
        for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
        {
            current = node->GetData();
            if ((current->status == STATUS_ACTIVE) || ((current->status == STATUS_SCHEDULE) && (programoptions.activatescheduling)))
                somedownloadactive = TRUE;
        }
        if (!somedownloadactive)
        {
            if (programoptions.shutdown)
            {
                wxStopWatch waittime;
                waittime.Start();
                wxProgressDialog *waitbox = new wxProgressDialog(_("Shutdown the computer..."),_("The computer will be shutdown in seconds..."),30000,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
                while ((waittime.Time() < 30000) && (waitbox->Update(waittime.Time())))
                    wxMilliSleep(200);
                delete waitbox;
                if (waittime.Time() >= 30000)
                    ::wxExecute(programoptions.shutdowncmd);
            }
            else if (programoptions.disconnect)
            {
                wxStopWatch waittime;
                waittime.Start();
                wxProgressDialog *waitbox = new wxProgressDialog(_("Disconnecting from the internet..."),_("The internet connection will be closed in seconds..."),30000,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
                while ((waittime.Time() < 30000) && (waitbox->Update(waittime.Time())))
                    wxMilliSleep(200);
                delete waitbox;
                if (waittime.Time() >= 30000)
                    ::wxExecute(programoptions.disconnectcmd);
            }
        }
    }
    if (timerinterval >= (programoptions.graphrefreshtime))
    {
        float *value = new float();
        wxString temp;
        *value = ((float)currentspeed) / 1024.0;
        graph.Append(value);

        temp.Printf(_("Total Speed: %0.1f kb/s"), *value);
        temp.Replace(wxT(","),wxT("."));
        statusbar->SetStatusText(temp,1);

        if (graph.GetCount() > (unsigned int)programoptions.graphhowmanyvalues)
        {
            float *firstvalue = graph.GetFirst()->GetData();
            graph.DeleteNode(graph.GetFirst());
            delete firstvalue;
        }
        timerinterval = 0;
        if (this->IsShown())
        {
            mutex_programoptions->Unlock();
            XRCCTRL(*this, "graphpanel",mGraph )->Refresh();
        }
    }
    timerinterval += mtimer->GetInterval();
    mutex_programoptions->Unlock();
}

void mMainFrame::GenerateInProgressList()
{
    mInProgressList* list01 = XRCCTRL(*this, "inprogresslist",mInProgressList );
    wxListItem itemCol;

    list01->SetImageList(imageslist, wxIMAGE_LIST_SMALL);
    itemCol.m_mask = wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE;
    itemCol.m_text = wxEmptyString;
    itemCol.m_image = -1;
    list01->ClearAll();
    list01->InsertColumn(INPROGRESS_ICON01, itemCol);

    itemCol.m_text = _("Restart");
    list01->InsertColumn(INPROGRESS_ICON02, itemCol);

    itemCol.m_text = _("Filename");
    list01->InsertColumn(INPROGRESS_NAME, itemCol);

    itemCol.m_text = _("Size");
    list01->InsertColumn(INPROGRESS_SIZE, itemCol);

    itemCol.m_text = _("Completed");
    list01->InsertColumn(INPROGRESS_COMPLETED, itemCol);

    itemCol.m_text = _("Percentual");
    list01->InsertColumn(INPROGRESS_PERCENTUAL, itemCol);

    itemCol.m_text = _("Time Passed");
    list01->InsertColumn(INPROGRESS_TIMEPASSED, itemCol);

    itemCol.m_text = _("Remaining");
    list01->InsertColumn(INPROGRESS_TIMEREMAINING, itemCol);

    itemCol.m_text = _("Speed");
    list01->InsertColumn(INPROGRESS_SPEED, itemCol);

    itemCol.m_text = _("Attempts");
    list01->InsertColumn(INPROGRESS_ATTEMPTS, itemCol);

    itemCol.m_text = _("URL");
    list01->InsertColumn(INPROGRESS_URL, itemCol);

    list01->Hide();
    {
        list01->SetColumnWidth(INPROGRESS_ICON01,20);
        list01->SetColumnWidth(INPROGRESS_ICON02,80);
        list01->SetColumnWidth(INPROGRESS_NAME,160);
        list01->SetColumnWidth(INPROGRESS_SIZE,100);
        list01->SetColumnWidth(INPROGRESS_COMPLETED,100);
        list01->SetColumnWidth(INPROGRESS_PERCENTUAL,100);
        list01->SetColumnWidth(INPROGRESS_TIMEPASSED,100);
        list01->SetColumnWidth(INPROGRESS_TIMEREMAINING,100);
        list01->SetColumnWidth(INPROGRESS_SPEED,100);
        list01->SetColumnWidth(INPROGRESS_ATTEMPTS,100);
        list01->SetColumnWidth(INPROGRESS_URL,300);
    }
    int i=0;
    for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
    {
        mDownloadFile *current = node->GetData();
        if (current->index != i)
        {
            current->index = i;
            if ((current->status != STATUS_ERROR) && ((current->status != STATUS_FINISHED)))
                current->status = STATUS_STOPED;
            wxGetApp().RegisterListItemOnDisk(current);
        }
        list01->Insert(current,-1);
        i++;
    }

    list01->SelectUnselect(FALSE,-1,this);
    list01->Show();
}

void mMainFrame::GenerateFinishedList()
{
    mFinishedList* list02 = XRCCTRL(*this, "finishedlist",mFinishedList );
    wxListCtrl* list03 = XRCCTRL(*this, "infolist",wxListCtrl );
    wxListItem itemCol;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    wxString name;
    long size;
    time_t enddate = 0;
    wxDateTime date;
    long index,i=0;
    int status;
    list02->SetImageList(imageslist, wxIMAGE_LIST_SMALL);
    itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
    itemCol.m_text = wxEmptyString;
    itemCol.m_image = -1;
    list02->ClearAll();
    list02->InsertColumn(FINISHED_ICON01, itemCol);
    itemCol.m_text = _("Filename");
    list02->InsertColumn(FINISHED_NAME, itemCol);
    itemCol.m_text = _("Size");
    list02->InsertColumn(FINISHED_SIZE, itemCol);
    itemCol.m_text = _("Finished");
    list02->InsertColumn(FINISHED_END, itemCol);

    list03->ClearAll();
//    list03->SetBackgroundColour(BLUE);
//    list03->SetTextColour(*wxWHITE);
    list03->InsertColumn(0, wxEmptyString);
    list03->InsertColumn(1, wxEmptyString);
    list03->SetColumnWidth(0,120);
    list03->SetColumnWidth(1,400);
    list03->InsertItem(0, _("Name"));
    list03->InsertItem(1, _("File type"));
    list03->InsertItem(2, _("Size"));
    list03->InsertItem(3, _("Speed"));
    list03->InsertItem(4, _("Time"));
    list03->InsertItem(5, _("Destination"));
    list03->InsertItem(6, _("Start"));
    list03->InsertItem(7, _("Finished"));
    list03->InsertItem(8, _("MD5"));
    list03->InsertItem(9, _("URLs"));
    list03->InsertItem(10, _("Comments"));

    list02->Hide();
    {
        list02->SetColumnWidth(FINISHED_ICON01,20);
        list02->SetColumnWidth(FINISHED_NAME,200);
        list02->SetColumnWidth(FINISHED_SIZE,100);
        list02->SetColumnWidth(FINISHED_END,120);
    }

    config->SetPath(FINISHED_REG);
    if (config->GetFirstGroup(name, index))
        do
        {
            list02->InsertItem(i, name);
            i++;
        }
        while(config->GetNextGroup(name, index));

    for (i=0; i < list02->GetItemCount();i++)
    {
        name = list02->GetItemText(i);
        config->SetPath(name);
        config->Read(STATUS_REG,&status);
        config->Read(SIZE_REG,&size);
        config->Read(END_REG,&enddate);
        date.Set(enddate);
        list02->SetItem(i, FINISHED_ICON01, wxEmptyString,status);
        list02->SetItem(i, FINISHED_NAME, name);
        list02->SetItem(i, FINISHED_SIZE, ByteString(size));
        list02->SetItem(i, FINISHED_END, date.Format());
        list02->SetItemData(i,enddate);

        config->SetPath(BACK_DIR_REG);
    }
    delete config;
    list02->SortItems(CompareDates, 0l);
    list02->Show();
}

bool mMainFrame::NewDownload(wxArrayString url, wxString destination,int parts,wxString user,wxString password,wxString comments,int startoption, bool show)
{
    mBoxNew dlg;
    wxTextCtrl *edturl, *edtdestination, *edtuser ,*edtpassword, *edtcomments;
    wxCheckListBox *lstaddresslist;
    wxSpinCtrl *spinsplit;
    wxRadioButton *optnow,*optschedule;
    int RETURN;
    unsigned int i,nparams;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("boxnew"));

    edturl = XRCCTRL(dlg, "edturl",wxTextCtrl);
    edtdestination = XRCCTRL(dlg, "edtdestination",wxTextCtrl);
    edtuser = XRCCTRL(dlg, "edtuser",wxTextCtrl);
    edtpassword = XRCCTRL(dlg, "edtpassword",wxTextCtrl);
    edtcomments = XRCCTRL(dlg, "edtcomments",wxTextCtrl);
    lstaddresslist = XRCCTRL(dlg, "lstaddresslist",wxCheckListBox);
    optnow = XRCCTRL(dlg, "optnow",wxRadioButton);
    optschedule = XRCCTRL(dlg, "optschedule",wxRadioButton);
    spinsplit = XRCCTRL(dlg, "spinsplit",wxSpinCtrl);

    if (url.GetCount() <= 0)
    {
        edturl->SetValue(wxEmptyString);
        lstaddresslist->Hide();
        XRCCTRL(dlg,"lbladdresslist",wxStaticText)->Hide();
    }
    else if (url.GetCount() == 1)
    {
        edturl->SetValue(url.Item(0));
        lstaddresslist->Hide();
        XRCCTRL(dlg,"lbladdresslist",wxStaticText)->Hide();
    }
    else
    {
        edturl->Enable(FALSE);
        lstaddresslist->Clear();
        lstaddresslist->InsertItems(url,0);
        nparams = lstaddresslist->GetCount();
        for (i = 0; i < nparams ;i++)
            lstaddresslist->Check(i);
    }
    dlg.SetBestFittingSize();
    edtdestination->SetValue(destination);
    if (user == ANONYMOUS_USER)
    {
        edtuser->SetValue(wxEmptyString);
        edtpassword->SetValue(wxEmptyString);
    }
    else
    {
        edtuser->SetValue(user);
        edtpassword->SetValue(password);
    }
    edtcomments->SetValue(comments);
    if (startoption == NOW)
        optnow->SetValue(TRUE);
    else
        optnow->SetValue(FALSE);
    spinsplit->SetValue(parts);
    if (show)
        RETURN = dlg.ShowModal();
    else
        RETURN = XRCID("btnok");

    if (RETURN == XRCID("btnok"))
    {
        if (edturl->IsEnabled())
        {
            wxArrayString urltmp;
            urltmp.Add(edturl->GetValue());
            lstaddresslist->InsertItems(urltmp,0);
            lstaddresslist->Check(0);
        }
        nparams = lstaddresslist->GetCount();
        wxArrayString fileswitherror;
        wxFileName destinationtmp;
        int index;
        int scheduled, now;
        scheduled = optschedule->GetValue();
        now = optnow->GetValue();

        destinationtmp.Assign(edtdestination->GetValue());

        for (i = 0; i < nparams; i++)
        {
            if (!lstaddresslist->IsChecked(i))
                continue;

            mUrlName urltmp(lstaddresslist->GetString(i));
            if (!wxGetApp().FindDownloadFile(urltmp.GetFullName()))
            {
                index = wxGetApp().CreateDownloadRegister(urltmp,destinationtmp, spinsplit->GetValue(),
                        edtuser->GetValue(), edtpassword->GetValue(), edtcomments->GetValue(),scheduled);
                XRCCTRL(*this, "inprogresslist",mInProgressList )->Insert(wxGetApp().downloadlist.Item(index)->GetData(),-1);
                //XRCCTRL(*this, "inprogresslist",mInProgressList )->SetCurrentSelection(index); //CAUSES SEGMENTATION FAULT
            }
            else
            {
                fileswitherror.Add(lstaddresslist->GetString(i));
                continue;
            }

            if (now)
                wxGetApp().downloadlist.Item(index)->GetData()->status = STATUS_QUEUE;
            if (scheduled)
                wxGetApp().downloadlist.Item(index)->GetData()->status = STATUS_SCHEDULE;
        }
        if (fileswitherror.GetCount() > 0)
        {
            wxString message = _("The follow file(s) already are being downloaded:");
            for (i = 0; i < fileswitherror.GetCount(); i++)
                message += wxT("\n") + fileswitherror.Item(i);
            wxMessageBox(message,_("Error...") ,wxOK | wxICON_ERROR,this);
            return FALSE;
        }
        programoptions.lastnumberofparts = spinsplit->GetValue();
        if (scheduled)
            programoptions.laststartoption = SCHEDULE;
        else if (now)
            programoptions.laststartoption = NOW;
        else
            programoptions.laststartoption = MANUAL;
        programoptions.lastdestination = edtdestination->GetValue();

        wxGetApp().Configurations(WRITE,OPT_LAST_DESTINATION_REG, programoptions.lastdestination);
        wxGetApp().Configurations(WRITE,OPT_LAST_NUMBER_OF_PARTS_REG, programoptions.lastnumberofparts);
        wxGetApp().Configurations(WRITE,OPT_LAST_START_OPTION_REG, programoptions.laststartoption);
    }
    return TRUE;
}

void mMainFrame::OnNew(wxCommandEvent& event)
{
    OnPasteURL(event);
}

void mMainFrame::OnRemove(wxCommandEvent& event)
{
    int currentselection;
    mInProgressList *inprogresslist = XRCCTRL(*this, "inprogresslist",mInProgressList );
    if ((currentselection = inprogresslist->GetCurrentSelection()) >= 0)
    {
        mDownloadFile *currentfile = wxGetApp().downloadlist.Item(currentselection)->GetData();
        {
            wxProgressDialog *waitbox = new wxProgressDialog(_("Stopping the download..."),_("Stopping the current download before remove..."));
            StopDownload(currentfile);
            while ((currentfile->free == FALSE) || (currentfile->status == STATUS_ACTIVE))
                wxYield();
            waitbox->Update(100);
            delete waitbox;
        }
        if (((currentfile->status == STATUS_STOPED) || (currentfile->status == STATUS_ERROR)) && (currentfile->free))
        {
            wxMessageDialog *dlg = new wxMessageDialog(this, _("Do you want remove the item from the list and the file from the disk?"),_("Remove..."),
                        wxYES_NO | wxCANCEL | wxICON_QUESTION);
            if (dlg)
            {
                int resp;
                dlg->Centre(wxBOTH);
                resp = dlg->ShowModal();
                if (resp == wxID_YES)
                {
                    wxString destination = currentfile->destination,name = currentfile->name;
                    wxGetApp().RemoveListItemFromDisk(currentfile);
                    inprogresslist->RemoveItemListandFile(currentselection);
                    if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
                        destination = destination + SEPARATOR_DIR;
                    for (int i = 0; i< MAX_NUM_PARTS;i++)
                        ::wxRemoveFile(destination + PREFIX + name + EXT + int2wxstr(i));
                }
                else if (resp == wxID_NO)
                {
                    wxGetApp().RemoveListItemFromDisk(currentfile);
                    inprogresslist->RemoveItemListandFile(currentselection);
                }
                dlg->Destroy();
            }
        }
        return ;
    }
    mFinishedList *finishedlist = XRCCTRL(*this, "finishedlist",mFinishedList );
    if ((currentselection = finishedlist->GetCurrentSelection()) >= 0)
    {
        wxMessageDialog *dlg = new wxMessageDialog(this, _("Do you want remove the item from the list and the file from the disk?"),_("Remove..."),
                    wxYES_NO | wxCANCEL | wxICON_QUESTION);
        if (dlg)
        {
            int resp;
            dlg->Centre(wxBOTH);
            resp = dlg->ShowModal();
            if (resp != wxID_CANCEL)
            {
                wxString destination,name;
                if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
                    destination = destination + SEPARATOR_DIR;

                wxFileConfig *config = new wxFileConfig(DFAST_REG);
                wxListItem item;
                item.SetId(currentselection);
                item.SetColumn(FINISHED_NAME);
                item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
                finishedlist->GetItem(item);
                config->SetPath(FINISHED_REG);
                config->SetPath(item.GetText());

                destination = wxEmptyString;
                config->Read(DESTINATION_REG,&destination);
                if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
                    destination = destination + SEPARATOR_DIR;

                name = wxEmptyString;
                config->Read(NAME_REG,&name);

                config->SetPath(BACK_DIR_REG);
                config->DeleteGroup(item.GetText());
                finishedlist->DeleteItem(currentselection);
                delete config;
                if (resp == wxID_YES)
                    ::wxRemoveFile(destination + name);
            }
        }
    }
}

void mMainFrame::OnSchedule(wxCommandEvent& event)
{
    int currentselection;
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
        mDownloadFile *file = wxGetApp().downloadlist.Item(currentselection)->GetData();
        if (((file->status == STATUS_STOPED) || (file->status == STATUS_QUEUE) || (file->status == STATUS_ERROR)) && (file->free))
        {
            file->scheduled = TRUE;
            file->status = STATUS_SCHEDULE;
            wxGetApp().RegisterListItemOnDisk(file);
        }
    }
}

void mMainFrame::OnStart(wxCommandEvent& event)
{
    int currentselection;
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
        mDownloadFile *file = wxGetApp().downloadlist.Item(currentselection)->GetData();
        if (((file->status == STATUS_STOPED) || (file->status == STATUS_ERROR)) && (file->free))
        {
            file->scheduled = FALSE;
            file->status = STATUS_QUEUE;
            wxGetApp().RegisterListItemOnDisk(file);
        }
    }
}

bool mMainFrame::StartDownload(mDownloadFile *downloadfile)
{
    if (downloadfile->free)
    {
        downloadfile->free = FALSE;
        downloadfile->split = WAIT;
        downloadfile->speedpointowner = -1;
        for (int i=0; i < downloadfile->parts;i++)
        {
            mDownloadThread *thread = new mDownloadThread(downloadfile,i);
            if ( thread->Create() != wxTHREAD_NO_ERROR )
            {
                wxMessageBox(_("Error creating thread!"));
                return FALSE;
            }
            else
            {
                wxCriticalSectionLocker enter(wxGetApp().m_critsect);
                if ( thread->Run() != wxTHREAD_NO_ERROR )
                {
                    wxMessageBox(_("Error starting thread!"));
                    return FALSE;
                }
            }
        }
        Refresh();
        return TRUE;
    }
    return FALSE;
}

void mMainFrame::OnStop(wxCommandEvent& event)
{
    int currentselection;
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
        int status;
        mDownloadFile *file = wxGetApp().downloadlist.Item(currentselection)->GetData();
        status = file->status;
        StopDownload(file);
        if ((status == STATUS_QUEUE) || (status == STATUS_SCHEDULE)) //WHEN THE DOWNLOAD IS ALREADY ACTIVE THE WRITE ON DISK WILL BE MADE IN THE TIMER
            wxGetApp().RegisterListItemOnDisk(file);
    }
}

void mMainFrame::StopDownload(mDownloadFile *downloadfile)
{
    if ((downloadfile->status != STATUS_FINISHED) && (downloadfile->status != STATUS_ERROR))
    {
        downloadfile->scheduled = FALSE;
        downloadfile->status = STATUS_STOPED;
    }
}

void mMainFrame::OnStartAll(wxCommandEvent& event)
{
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    wxProgressDialog *waitbox = new wxProgressDialog(_("Starting the downloads..."),_("Starting the downloads..."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
    int total = list->GetItemCount();
    for (int i=0 ; i < total;i++)
    {
        list->SetCurrentSelection(i);
        OnStart(event);
        waitbox->Update((int)(i*100/total));
    }
    waitbox->Update(100);
    delete waitbox;
    wxMilliSleep(200);
}

void mMainFrame::OnStopAll(wxCommandEvent& event)
{
    mDownloadFile *currentfile;
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    wxProgressDialog *waitbox = new wxProgressDialog(_("Stopping the downloads..."),_("Stopping the downloads..."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
    int total = list->GetItemCount();
    for (int i=0 ; i < total;i++)
    {
        list->SetCurrentSelection(i);
        OnStop(event);
    }
    for (int i=0 ; i < total;i++)
    {
        currentfile = wxGetApp().downloadlist.Item(i)->GetData();
        while ((currentfile->free == FALSE) && (waitbox->Update((int)(i*100/total))))
        {
            wxYield();
            wxMilliSleep(200);
        }
    }
    waitbox->Update(100);
    delete waitbox;
    wxMilliSleep(200);
}

void mMainFrame::OnPasteURL(wxCommandEvent& event)
{
    int numberofparts, startoption;
    wxString destinationtmp = wxEmptyString;
    wxArrayString urltmp;
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            if (data.GetText().Strip(wxString::both) != wxEmptyString)
                urltmp.Add(data.GetText().Strip(wxString::both));
        }
        wxTheClipboard->Close();
    }

    if (programoptions.rememberboxnewoptions)
    {
        numberofparts = programoptions.lastnumberofparts;
        startoption = programoptions.laststartoption;
        destinationtmp = programoptions.lastdestination;
    }
    else
    {
        numberofparts = DEFAULT_NUM_PARTS;
        startoption = DEFAULT_START_OPTION;
        destinationtmp = programoptions.destination;
    }
    NewDownload(urltmp,destinationtmp,numberofparts,wxEmptyString,wxEmptyString,wxEmptyString,startoption,TRUE);
}

void mMainFrame::OnCopyURL(wxCommandEvent& event)
{
    wxString urltmp,destinationtmp;
    if (wxTheClipboard->Open())
    {

            wxFileConfig *config = new wxFileConfig(DFAST_REG);
            wxString str;
            wxTextDataObject data;
            int currentselection,column;
            wxListCtrl *list;
            mInProgressList *list01 = XRCCTRL(*(wxGetApp().mainframe), "inprogresslist",mInProgressList );
            mFinishedList *list02 = XRCCTRL(*(wxGetApp().mainframe), "finishedlist",mFinishedList);
            if ((currentselection = list01->GetCurrentSelection()) >= 0)
            {
                config->SetPath(INPROGRESS_REG);
                column = INPROGRESS_NAME;
                list = (wxListCtrl*)list01;
            }
            else if ((currentselection = list02->GetCurrentSelection()) >= 0)
            {
                config->SetPath(FINISHED_REG);
                column = FINISHED_NAME;
                list = (wxListCtrl*)list02;
            }
            else
               return;
            wxListItem item;
            item.SetId(currentselection);
              item.SetColumn(column);
            item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
            list->GetItem(item);
            config->SetPath(item.GetText());
            str = wxEmptyString;
            config->Read(URL1_REG,&str);
            wxTheClipboard->SetData( new wxTextDataObject(str));
            wxTheClipboard->Close();
    }
    else
        wxMessageBox(_("It was impossible to open the clipboard!"),_("Error...") ,wxOK | wxICON_ERROR,this);
}

void mMainFrame::OnFind(wxCommandEvent& event)
{
    wxFindReplaceData *data = new wxFindReplaceData();
    data->SetFlags(wxFR_DOWN);
    mBoxFind *dlg = new mBoxFind(this,data,_("Find file..."));
    if (dlg)
        dlg->Show();
}

void mMainFrame::OnDetails(wxCommandEvent& event)
{
    wxSplitterWindow *splitter = XRCCTRL(*this, "splitter01",wxSplitterWindow);
    if (splitter->IsSplit())
        splitter->Unsplit();
    else
    {
        splitter->SplitHorizontally(XRCCTRL(*this, "inprogresslist",mInProgressList ),XRCCTRL(*this, "panelmessages01",wxPanel));
        XRCCTRL(*this, "panelmessages01",wxPanel)->Show();
    }

}

void mMainFrame::OnUpDown(bool up)
{
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    int newselection = -1,currentselection;
    currentselection = list->GetCurrentSelection();
    if ((currentselection > 0) && (up == TRUE))
        newselection = currentselection -1;
    if ((currentselection >= 0) && (up == FALSE) && (currentselection+1 < list->GetItemCount()))
        newselection = currentselection +1;
    if (newselection >= 0)
    {
        mDownloadFile *currentfile = wxGetApp().downloadlist.Item(currentselection)->GetData();
        mDownloadFile *newfile = wxGetApp().downloadlist.Item(newselection)->GetData();
        currentfile->index = newselection;
        newfile->index = currentselection;
        wxGetApp().downloadlist.Sort(ListCompareByIndex);
        wxGetApp().RegisterListItemOnDisk(currentfile);
        wxGetApp().RegisterListItemOnDisk(newfile);

        list->Insert(currentfile,newselection);
        list->Insert(newfile,currentselection);
        list->SetCurrentSelection(newselection);
        toolbar->Realize();
    }
}

void mMainFrame::OnLanguages(wxCommandEvent& event)
{
    wxString langs[] =
    {
        _("(Default)"),
        _("English"),
        _("Portuguese(Brazil)"),
        _("German"),
        _("Spanish"),
    };

    int lng = wxGetSingleChoiceIndex(_("Please select the language:"), _("Language"),
                                   WXSIZEOF(langs), langs);
    if (lng >= 0)
    {
        int langvalue=0;
        switch (lng)
        {
            case 0 : langvalue = wxLANGUAGE_DEFAULT; break;
            case 1 : langvalue = wxLANGUAGE_ENGLISH; break;
            case 2 : langvalue = wxLANGUAGE_PORTUGUESE_BRAZILIAN; break;
            case 3 : langvalue = wxLANGUAGE_GERMAN; break;
            case 4 : langvalue = wxLANGUAGE_SPANISH; break;
        }
        wxGetApp().Configurations(WRITE,LANGUAGE_REG,langvalue); //WRITE OPTION
        wxMessageBox(_("You need restart the program to use the new language!"),
                 _("Information..."), wxOK | wxICON_INFORMATION, this);
    }
}

void mMainFrame::OnProperties(wxCommandEvent& event)
{
    int currentselection;
    mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
        mBoxNew dlg;
        wxString newname, oldname;
        mDownloadFile *currentfile = wxGetApp().downloadlist.Item(currentselection)->GetData();
        wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("boxnew"));
        dlg.Centre(wxBOTH);
        dlg.SetTitle(_("Download Properties"));
        XRCCTRL(dlg, "edturl",wxTextCtrl)->SetValue(currentfile->urllist);
        XRCCTRL(dlg, "edtdestination",wxTextCtrl)->SetValue(currentfile->destination);

        if (currentfile->user == ANONYMOUS_USER)
        {
            XRCCTRL(dlg, "edtuser",wxTextCtrl)->SetValue(wxEmptyString);
            XRCCTRL(dlg, "edtpassword",wxTextCtrl)->SetValue(wxEmptyString);
        }
        else
        {
            XRCCTRL(dlg, "edtuser",wxTextCtrl)->SetValue(currentfile->user);
            XRCCTRL(dlg, "edtpassword",wxTextCtrl)->SetValue(currentfile->password);
        }
        XRCCTRL(dlg, "spinsplit",wxSpinCtrl)->SetValue(currentfile->parts);
        XRCCTRL(dlg, "edtcomments",wxTextCtrl)->SetValue(currentfile->comments);
        XRCCTRL(dlg, "optmanual",wxRadioButton)->Enable(FALSE);
        XRCCTRL(dlg, "optnow",wxRadioButton)->Enable(FALSE);
        XRCCTRL(dlg, "optschedule",wxRadioButton)->Enable(FALSE);

        mUrlName urltmp(currentfile->urllist);
        oldname = urltmp.GetFullName();
        if ((currentfile->status != STATUS_STOPED) && (currentfile->status != STATUS_FINISHED) && (currentfile->status != STATUS_ERROR))
        {
            XRCCTRL(dlg, "edturl",wxTextCtrl)->SetEditable(FALSE);
            XRCCTRL(dlg, "edtdestination",wxTextCtrl)->SetEditable(FALSE);
            XRCCTRL(dlg, "edtuser",wxTextCtrl)->SetEditable(FALSE);
            XRCCTRL(dlg, "edtpassword",wxTextCtrl)->SetEditable(FALSE);
            XRCCTRL(dlg, "edtcomments",wxTextCtrl)->SetEditable(FALSE);
        }
        XRCCTRL(dlg, "spinsplit",wxSpinCtrl)->Enable(FALSE);

        XRCCTRL(dlg,"lstaddresslist",wxCheckListBox)->Hide();
        XRCCTRL(dlg,"lbladdresslist",wxStaticText)->Hide();
        dlg.SetBestFittingSize();

        if (dlg.ShowModal() == XRCID("btnok"))
        {
            currentfile->urllist = XRCCTRL(dlg, "edturl",wxTextCtrl)->GetValue();
            currentfile->destination = XRCCTRL(dlg, "edtdestination",wxTextCtrl)->GetValue();
            currentfile->user = XRCCTRL(dlg, "edtuser",wxTextCtrl)->GetValue();
            currentfile->password = XRCCTRL(dlg, "edtpassword",wxTextCtrl)->GetValue();
            currentfile->comments = XRCCTRL(dlg, "edtcomments",wxTextCtrl)->GetValue();
            mUrlName urltmp(currentfile->urllist);
            newname = urltmp.GetFullName();
            if (currentfile->destination.Mid(currentfile->destination.Length()-1,1) != SEPARATOR_DIR)
                currentfile->destination = currentfile->destination + SEPARATOR_DIR;
            if (newname != oldname)
            {
                wxGetApp().ChangeName(currentfile,newname,0);
                XRCCTRL(*this, "inprogresslist",mInProgressList )->SetItem(currentfile->index,INPROGRESS_NAME,currentfile->name);
            }
            list->SetItem(currentfile->index,INPROGRESS_URL,currentfile->urllist);
            wxGetApp().RegisterListItemOnDisk(currentfile);
        }
        dlg.Destroy();
    }
}

void mMainFrame::OnDownloadAgain(wxCommandEvent& event)
{
    int currentselection;
    mFinishedList *list = XRCCTRL(*this, "finishedlist",mFinishedList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
        wxFileConfig *config = new wxFileConfig(DFAST_REG);
        wxListItem item;
        int startoption;
        wxString url,destination,user,password,comments;
        wxArrayString urlarray;
        int parts;
        item.SetId(currentselection);
        item.SetColumn(FINISHED_NAME);
        item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
        list->GetItem(item);

        config->SetPath(FINISHED_REG);
        config->SetPath(item.GetText());

        url = wxEmptyString;
        config->Read(URL1_REG,&url);
        if (url != wxEmptyString)
            urlarray.Add(url);

        destination = wxEmptyString;
        config->Read(DESTINATION_REG,&destination);

        parts = 1;
        config->Read(PARTS_REG,&parts);

        user = wxEmptyString;
        config->Read(USER_REG,&user);

        password = wxEmptyString;
        config->Read(PASSWORD_REG,&password);

        comments = wxEmptyString;
        config->Read(COMMENTS_REG,&comments);
        config->SetPath(BACK_DIR_REG);
        
        if (programoptions.rememberboxnewoptions)
            startoption = programoptions.laststartoption;
        else
            startoption = DEFAULT_START_OPTION;

        if (NewDownload(urlarray, destination, parts, user, password, comments, startoption, FALSE))
        {
            config->DeleteGroup(item.GetText());
            list->DeleteItem(currentselection);
            list->SelectUnselect(FALSE,-1,this);
        }
        delete config;
    }
}

void mMainFrame::OnMove(wxCommandEvent& event)
{
    int currentselection;
    mFinishedList *list = XRCCTRL(*this, "finishedlist",mFinishedList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
         wxString dir;
        dir = wxDirSelector(_("Select the directory:"));
        if (dir != wxEmptyString)
        {
            if (dir.Mid(dir.Length()-1,1) != SEPARATOR_DIR)
                dir = dir + SEPARATOR_DIR;
            wxFileConfig *config = new wxFileConfig(DFAST_REG);
            wxListItem item;
            wxString name,destination;
            item.SetId(currentselection);
            item.SetColumn(FINISHED_NAME);
            item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
            list->GetItem(item);

            config->SetPath(FINISHED_REG);
            config->SetPath(item.GetText());

            name = wxEmptyString;
            config->Read(NAME_REG,&name);

            destination = wxEmptyString;
            config->Read(DESTINATION_REG,&destination);
            if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
                destination = destination + SEPARATOR_DIR;

            if (destination != dir)
            {
                if (::wxFileExists(destination+name))
                {
                    wxProgressDialog *dlg = new wxProgressDialog(_("Moving..."),_("Moving file..."));
                    wxLogNull logNo;
                    if (::wxCopyFile(destination+name,dir+name,TRUE))
                    {
                        dlg->Update(50);
                        config->Write(DESTINATION_REG,dir);
                        ::wxRemoveFile(destination+name);
                    }
                    else
                        wxMessageBox(_("Error moving file."),_("Error..."),wxOK|wxICON_ERROR,this);
                    delete dlg;
                }
                else
                {
                    if (wxMessageBox(_("The file isn't in the path that it was saved initially.\nDo you want to change only the old path for the current one?"),
                            _("Continue..."),wxYES | wxNO | wxICON_QUESTION, this) == wxYES)
                        config->Write(DESTINATION_REG,dir);
                }
            }
            delete config;
            list->SelectUnselect(TRUE,currentselection,this);
        }
    }
}

void mMainFrame::OnCheckMD5(wxCommandEvent& event)
{
    int currentselection;
    mFinishedList *list = XRCCTRL(*this, "finishedlist",mFinishedList );
    if ((currentselection = list->GetCurrentSelection()) >= 0)
    {
        wxFileConfig *config = new wxFileConfig(DFAST_REG);
        wxListItem item;
        wxString name,destination,md5old, md5new;
        item.SetId(currentselection);
        item.SetColumn(FINISHED_NAME);
        item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
        list->GetItem(item);

        config->SetPath(FINISHED_REG);
        config->SetPath(item.GetText());

        name = wxEmptyString;
        config->Read(NAME_REG,&name);

        destination = wxEmptyString;
        config->Read(DESTINATION_REG,&destination);
        if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
            destination = destination + SEPARATOR_DIR;

        md5old = wxEmptyString;
        config->Read(MD5_REG,&md5old);

        if (::wxFileExists(destination + name))
        {
            wxFileName filemd5 = wxFileName(destination + name);
            wxMD5 md5(filemd5);
            md5new = md5.GetDigest();
            if (md5new == md5old)
                wxMessageBox(_("The file was verified successfully."), _("Success..."), wxOK | wxICON_INFORMATION,this);
            else
            {
                wxString msg;
                msg = _("The MD5 calculated previously is different of the current one.");
                msg = msg + _("\nOld MD5 =\t\t") + md5old;
                msg = msg + _("\nCurrent MD5 =\t") + md5new;
                wxMessageBox(msg, _("Error..."), wxOK | wxICON_ERROR,this);
            }

        }
        else
            wxMessageBox(_("File not found."), _("Error..."), wxOK | wxICON_ERROR,this);
        delete config;
    }
}

void mMainFrame::OnExportConf(wxCommandEvent& event)
{
     wxString dir;
    dir = wxDirSelector(_("Select the directory:"));
    if (dir != wxEmptyString)
    {
        if (dir.Mid(dir.Length()-1,1) != SEPARATOR_DIR)
            dir = dir + SEPARATOR_DIR;
        wxString source, destination;
        source = wxGetHomeDir();
        if (source.Mid(source.Length()-1,1) != SEPARATOR_DIR)
            source = source + SEPARATOR_DIR;
        #ifdef __WXMSW__
        source = source + DFAST_REG + wxT(".ini");
        #else
        source = source + wxT(".") + DFAST_REG;
        #endif
        destination = dir + DFAST_REG + wxT(".conf");
        wxLogNull noLog;
        if (::wxCopyFile(source,destination,TRUE))
            wxMessageBox(_("The configurations were exported successfully."), _("Success..."),wxOK|wxICON_INFORMATION,this);
        else
            wxMessageBox(_("Error exporting configurations."), _("Error..."),wxOK|wxICON_ERROR,this);
    }
}

void mMainFrame::OnImportConf(wxCommandEvent& event)
{
     wxFileDialog *dlg;
    dlg = new wxFileDialog(this, _("Select the file..."), wxEmptyString, wxEmptyString, wxT("*.conf"));
    if (dlg->ShowModal() == wxID_OK)
    {
        wxString source, destination;
        source = dlg->GetPath();
        destination = wxGetHomeDir();
        if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
            destination = destination + SEPARATOR_DIR;
        #ifdef __WXMSW__
        destination = destination + DFAST_REG + wxT(".ini");
        #else
        destination = destination + wxT(".") + DFAST_REG;
        #endif
        wxLogNull noLog;
        if (::wxCopyFile(source,destination,TRUE))
        {
            wxMessageBox(_("The configurations were imported successfully.\nThe program will be restated now to the alterations have effect."), _("Success..."),wxOK|wxICON_INFORMATION,this);
            Iconize(TRUE);
            Close(TRUE);
        }
        else
            wxMessageBox(_("Error importing configurations."), _("Error..."),wxOK|wxICON_ERROR,this);
    }
}

void mMainFrame::OnShutdown(wxCommandEvent& event)
{
    if (menubar->GetMenu(3)->IsChecked(XRCID("menushutdown")))
    {
        programoptions.shutdown = TRUE;
        wxMessageBox(_("Command :\t" ) + programoptions.shutdowncmd,_("Shutdown Command..."),wxOK|wxICON_INFORMATION,this);
    }
    else
        programoptions.shutdown = FALSE;
}

void mMainFrame::OnDisconnect(wxCommandEvent& event)
{
    if (menubar->GetMenu(3)->IsChecked(XRCID("menudisconnect")))
    {
        programoptions.disconnect = TRUE;
        wxMessageBox(_("Command :\t" ) + programoptions.disconnectcmd,_("Disconnect Command..."),wxOK|wxICON_INFORMATION,this);
    }
    else
        programoptions.disconnect = FALSE;
}

void mMainFrame::OnOptions(wxCommandEvent& event)
{

    mBoxOptions dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("boxoptions"));
    int i,j;
    int oldgraphheight = programoptions.graphheight;

    XRCCTRL(dlg, "spinattempts", wxSpinCtrl)->SetValue(programoptions.attempts);
    XRCCTRL(dlg, "spinattemptstime", wxSpinCtrl)->SetValue(programoptions.attemptstime);
    XRCCTRL(dlg, "spinsimultaneous", wxSpinCtrl)->SetValue(programoptions.simultaneous);
    XRCCTRL(dlg, "edtshutdown",wxTextCtrl)->SetValue(programoptions.shutdowncmd);
    XRCCTRL(dlg, "edtdisconnect",wxTextCtrl)->SetValue(programoptions.disconnectcmd);
    XRCCTRL(dlg, "chkclosedialog",wxCheckBox)->SetValue(programoptions.closedialog);
    XRCCTRL(dlg, "chkshutdown",wxCheckBox)->SetValue(programoptions.alwaysshutdown);
    XRCCTRL(dlg, "chkdisconnect",wxCheckBox)->SetValue(programoptions.alwaysdisconnect);
    XRCCTRL(dlg, "chkrememberboxnewoptions",wxCheckBox)->SetValue(programoptions.rememberboxnewoptions);
    XRCCTRL(dlg, "edtdestination",wxTextCtrl)->SetValue(programoptions.destination);
    XRCCTRL(dlg, "spintimerinterval",wxSpinCtrl)->SetValue(programoptions.timerupdateinterval);
    XRCCTRL(dlg, "spinreadbuffersize",wxSpinCtrl)->SetValue(programoptions.readbuffersize);
    XRCCTRL(dlg, "spingraphpoints",wxSpinCtrl)->SetValue(programoptions.graphhowmanyvalues);
    XRCCTRL(dlg, "spingraphrefreshrate",wxSpinCtrl)->SetValue(programoptions.graphrefreshtime);
    XRCCTRL(dlg, "spingraphmaxvalue",wxSpinCtrl)->SetValue(programoptions.graphscale);
    XRCCTRL(dlg, "spingraphtextarea",wxSpinCtrl)->SetValue(programoptions.graphtextarea);
    XRCCTRL(dlg, "spingraphheight",wxSpinCtrl)->SetValue(programoptions.graphheight);
    XRCCTRL(dlg, "spingraphfontsize",wxSpinCtrl)->SetValue(programoptions.graphspeedfontsize);
    XRCCTRL(dlg, "spingraphlinewidth",wxSpinCtrl)->SetValue(programoptions.graphlinewidth);
    XRCCTRL(dlg, "chkgraphshow",wxCheckBox)->SetValue(programoptions.graphshow);
    XRCCTRL(dlg, "graphpanelback", mBoxOptionsColorPanel)->colour = programoptions.graphbackcolor;
    XRCCTRL(dlg, "graphpanelgrid", mBoxOptionsColorPanel)->colour = programoptions.graphgridcolor;
    XRCCTRL(dlg, "graphpanelline", mBoxOptionsColorPanel)->colour = programoptions.graphlinecolor;
    XRCCTRL(dlg, "graphpanelfont", mBoxOptionsColorPanel)->colour = programoptions.graphfontcolor;
    XRCCTRL(dlg, "chkactivatescheduling",wxCheckBox)->SetValue(programoptions.activatescheduling);
    XRCCTRL(dlg, "edtstartdate",wxTextCtrl)->SetValue(programoptions.startdatetime.Format(wxT("%Y/%m/%d")));
    XRCCTRL(dlg, "edtfinishdate",wxTextCtrl)->SetValue(programoptions.finishdatetime.Format(wxT("%Y/%m/%d")));
    XRCCTRL(dlg, "spinstarthour",wxSpinCtrl)->SetValue(programoptions.startdatetime.GetHour());
    XRCCTRL(dlg, "spinstartminute",wxSpinCtrl)->SetValue(programoptions.startdatetime.GetMinute());
    XRCCTRL(dlg, "spinfinishhour",wxSpinCtrl)->SetValue(programoptions.finishdatetime.GetHour());
    XRCCTRL(dlg, "spinfinishminute",wxSpinCtrl)->SetValue(programoptions.finishdatetime.GetMinute());
    for (i=0; i<7;i++)
        XRCCTRL(dlg, "comboweekdays",wxComboBox)->Append(
wxGetTranslation(days[i]));
    XRCCTRL(dlg, "comboweekdays",wxComboBox)->SetValue(wxGetTranslation(days[0]));
    for (i=0;i<MAX_SCHEDULE_EXCEPTIONS;i++)
    {
        if (programoptions.scheduleexceptions[i].isactive)
        {
            wxString temp = programoptions.scheduleexceptions[i].start + wxT(" | ") +
                            programoptions.scheduleexceptions[i].finish + wxT(" | ") +
                            XRCCTRL(dlg, "comboweekdays",wxComboBox)->GetString(programoptions.scheduleexceptions[i].day);
            XRCCTRL(dlg, "lstexceptionlist",wxListBox)->InsertItems(1,&temp,0);
        }
    }

    if (dlg.ShowModal() == XRCID("btnoptionsave"))
    {
        wxProgressDialog *waitbox = new wxProgressDialog(_("Updating the configurations..."),_("Updating and saving the configurations..."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
        waitbox->Update(0);
        mutex_programoptions->Lock();

        programoptions.attempts = XRCCTRL(dlg, "spinattempts", wxSpinCtrl)->GetValue();
        programoptions.attemptstime = XRCCTRL(dlg, "spinattemptstime", wxSpinCtrl)->GetValue();
        programoptions.simultaneous = XRCCTRL(dlg, "spinsimultaneous", wxSpinCtrl)->GetValue();
        programoptions.shutdowncmd = XRCCTRL(dlg, "edtshutdown",wxTextCtrl)->GetValue();
        programoptions.disconnectcmd = XRCCTRL(dlg, "edtdisconnect",wxTextCtrl)->GetValue();
        programoptions.closedialog = XRCCTRL(dlg, "chkclosedialog",wxCheckBox)->GetValue();
        programoptions.alwaysshutdown = XRCCTRL(dlg, "chkshutdown",wxCheckBox)->GetValue();
        programoptions.alwaysdisconnect = XRCCTRL(dlg, "chkdisconnect",wxCheckBox)->GetValue();
        programoptions.rememberboxnewoptions = XRCCTRL(dlg, "chkrememberboxnewoptions",wxCheckBox)->GetValue();
        programoptions.destination = XRCCTRL(dlg, "edtdestination",wxTextCtrl)->GetValue();
        programoptions.timerupdateinterval = XRCCTRL(dlg, "spintimerinterval",wxSpinCtrl)->GetValue();
        programoptions.readbuffersize = XRCCTRL(dlg, "spinreadbuffersize",wxSpinCtrl)->GetValue();
        programoptions.graphshow = XRCCTRL(dlg, "chkgraphshow",wxCheckBox)->GetValue();
        programoptions.graphhowmanyvalues = XRCCTRL(dlg, "spingraphpoints",wxSpinCtrl)->GetValue();
        programoptions.graphrefreshtime = XRCCTRL(dlg, "spingraphrefreshrate",wxSpinCtrl)->GetValue();
        programoptions.graphscale = XRCCTRL(dlg, "spingraphmaxvalue",wxSpinCtrl)->GetValue();
        programoptions.graphtextarea = XRCCTRL(dlg, "spingraphtextarea",wxSpinCtrl)->GetValue();
        programoptions.graphheight = XRCCTRL(dlg, "spingraphheight",wxSpinCtrl)->GetValue();
        programoptions.graphspeedfontsize = XRCCTRL(dlg, "spingraphfontsize",wxSpinCtrl)->GetValue();
        programoptions.graphlinewidth = XRCCTRL(dlg, "spingraphlinewidth",wxSpinCtrl)->GetValue();
        programoptions.graphbackcolor = XRCCTRL(dlg, "graphpanelback", mBoxOptionsColorPanel)->colour;
        programoptions.graphgridcolor = XRCCTRL(dlg, "graphpanelgrid", mBoxOptionsColorPanel)->colour;
        programoptions.graphlinecolor = XRCCTRL(dlg, "graphpanelline", mBoxOptionsColorPanel)->colour;
        programoptions.graphfontcolor = XRCCTRL(dlg, "graphpanelfont", mBoxOptionsColorPanel)->colour;
        programoptions.activatescheduling = FALSE;
        programoptions.startdatetime.ParseDate(XRCCTRL(dlg, "edtstartdate",wxTextCtrl)->GetValue());
        programoptions.startdatetime.SetHour(XRCCTRL(dlg, "spinstarthour",wxSpinCtrl)->GetValue());
        programoptions.startdatetime.SetMinute(XRCCTRL(dlg, "spinstartminute",wxSpinCtrl)->GetValue());
        programoptions.finishdatetime.ParseDate(XRCCTRL(dlg, "edtfinishdate",wxTextCtrl)->GetValue());
        programoptions.finishdatetime.SetHour(XRCCTRL(dlg, "spinfinishhour",wxSpinCtrl)->GetValue());
        programoptions.finishdatetime.SetMinute(XRCCTRL(dlg, "spinfinishminute",wxSpinCtrl)->GetValue());
        waitbox->Update(20);
        for (i=0;i<MAX_SCHEDULE_EXCEPTIONS;i++)
        {
            if (i < XRCCTRL(dlg, "lstexceptionlist",wxListBox)->GetCount())
            {
                wxString temp = XRCCTRL(dlg, "lstexceptionlist",wxListBox)->GetString(i);
                programoptions.scheduleexceptions[i].newstart = temp.Mid(0,5);
                programoptions.scheduleexceptions[i].newfinish = temp.Mid(8,5);
                for (j = 0;j <7;j++)
                    if (temp.Mid(16) == wxGetTranslation(days[j]))
                    {
                        programoptions.scheduleexceptions[i].newday = j;
                        break;
                    }
            }
            else
            {
                programoptions.scheduleexceptions[i].newstart = wxEmptyString;
                programoptions.scheduleexceptions[i].newfinish = wxEmptyString;
                programoptions.scheduleexceptions[i].newday = 0;
            }
            wxGetApp().Configurations(WRITE,OPT_SCHED_SCHEDULEEXCEPTION_ISACTIVE_REG+int2wxstr(i), programoptions.scheduleexceptions[i].isactive);
            wxGetApp().Configurations(WRITE,OPT_SCHED_SCHEDULEEXCEPTION_START_REG+int2wxstr(i), programoptions.scheduleexceptions[i].start);
            wxGetApp().Configurations(WRITE,OPT_SCHED_SCHEDULEEXCEPTION_FINISH_REG+int2wxstr(i), programoptions.scheduleexceptions[i].finish);
            wxGetApp().Configurations(WRITE,OPT_SCHED_SCHEDULEEXCEPTION_DAY_REG+int2wxstr(i), programoptions.scheduleexceptions[i].day);
        }
        programoptions.scheduleexceptionschanged = TRUE;
        if ((XRCCTRL(dlg, "chkactivatescheduling",wxCheckBox)->GetValue()) && (programoptions.startdatetime < programoptions.finishdatetime))
            programoptions.activatescheduling = TRUE;
        else
            programoptions.activatescheduling = FALSE;
        waitbox->Update(50);
        wxGetApp().Configurations(WRITE,OPT_DIALOG_CLOSE_REG,programoptions.closedialog);
        wxGetApp().Configurations(WRITE,OPT_REMEMBER_BOXNEW_OPTIONS_REG,programoptions.rememberboxnewoptions);
        wxGetApp().Configurations(WRITE,OPT_DESTINATION_REG,programoptions.destination);
        wxGetApp().Configurations(WRITE,OPT_ATTEMPTS_REG,programoptions.attempts);
        wxGetApp().Configurations(WRITE,OPT_ATTEMPTS_TIME_REG,programoptions.attemptstime);
        wxGetApp().Configurations(WRITE,OPT_SIMULTANEOUS_REG,programoptions.simultaneous);
        wxGetApp().Configurations(WRITE,OPT_SHUTDOWN_REG,programoptions.alwaysshutdown);
        wxGetApp().Configurations(WRITE,OPT_DISCONNECT_REG,programoptions.alwaysdisconnect);
        wxGetApp().Configurations(WRITE,OPT_SHUTDOWN_CMD_REG,programoptions.shutdowncmd);
        wxGetApp().Configurations(WRITE,OPT_DISCONNECT_CMD_REG,programoptions.disconnectcmd);
        wxGetApp().Configurations(WRITE,OPT_TIMERINTERVAL_REG,programoptions.timerupdateinterval);
        wxGetApp().Configurations(WRITE,OPT_READBUFFERSIZE_REG,programoptions.readbuffersize);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_SHOW_REG,programoptions.graphshow);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_HOWMANYVALUES_REG, programoptions.graphhowmanyvalues);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_REFRESHTIME_REG, programoptions.graphrefreshtime);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_SCALE_REG, programoptions.graphscale);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_TEXTAREA_REG, programoptions.graphtextarea);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_HEIGHT_REG, programoptions.graphheight);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_SPEEDFONTSIZE_REG, programoptions.graphspeedfontsize);
        wxGetApp().Configurations(WRITE,OPT_GRAPH_LINEWIDTH_REG, programoptions.graphlinewidth);
        waitbox->Update(70);
        {
            wxString red,green,blue;
            red.Printf(wxT("%.3d"), programoptions.graphbackcolor.Red());
            green.Printf(wxT("%.3d"), programoptions.graphbackcolor.Green());
            blue.Printf(wxT("%.3d"), programoptions.graphbackcolor.Blue());
            wxGetApp().Configurations(WRITE,OPT_GRAPH_COLORBACK_REG, red + wxT("-") + green + wxT("-") + blue);
        }
        {
            wxString red,green,blue;
            red.Printf(wxT("%.3d"), programoptions.graphgridcolor.Red());
            green.Printf(wxT("%.3d"), programoptions.graphgridcolor.Green());
            blue.Printf(wxT("%.3d"), programoptions.graphgridcolor.Blue());
            wxGetApp().Configurations(WRITE,OPT_GRAPH_COLORGRID_REG, red + wxT("-") + green + wxT("-") + blue);
        }
        {
            wxString red,green,blue;
            red.Printf(wxT("%.3d"), programoptions.graphlinecolor.Red());
            green.Printf(wxT("%.3d"), programoptions.graphlinecolor.Green());
            blue.Printf(wxT("%.3d"), programoptions.graphlinecolor.Blue());
            wxGetApp().Configurations(WRITE,OPT_GRAPH_COLORLINE_REG, red + wxT("-") + green + wxT("-") + blue);
        }
        {
            wxString red,green,blue;
            red.Printf(wxT("%.3d"), programoptions.graphfontcolor.Red());
            green.Printf(wxT("%.3d"), programoptions.graphfontcolor.Green());
            blue.Printf(wxT("%.3d"), programoptions.graphfontcolor.Blue());
            wxGetApp().Configurations(WRITE,OPT_GRAPH_COLORFONT_REG, red + wxT("-") + green + wxT("-") + blue);
        }
        waitbox->Update(90);
        wxGetApp().Configurations(WRITE,OPT_SCHED_ACTIVATESCHEDULING_REG, programoptions.activatescheduling);
        wxGetApp().Configurations(WRITE,OPT_SCHED_STARTDATETIME_REG, (long)programoptions.startdatetime.GetTicks());
        wxGetApp().Configurations(WRITE,OPT_SCHED_FINISHDATETIME_REG, (long)programoptions.finishdatetime.GetTicks());

        if (programoptions.graphheight != oldgraphheight) //SHOW/HIDE/CHANGE THE HEIGHT OF THE GRAPH
        {
            int currentgraphheight = programoptions.graphheight;
            programoptions.graphheight = oldgraphheight;
            XRCCTRL(*(this), "graphpanel",mGraph )->Hide();
            programoptions.graphheight = currentgraphheight;
            if (programoptions.graphshow)
                XRCCTRL(*(this), "graphpanel",mGraph )->Show();
        }
        else
        {
            if (!programoptions.graphshow)
                XRCCTRL(*(this), "graphpanel",mGraph )->Hide();
            else
                XRCCTRL(*(this), "graphpanel",mGraph )->Show();
        }
        waitbox->Update(100);
        delete waitbox;
        mutex_programoptions->Unlock();
    }
}

void mMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString message;
    message.Printf( PROGRAM_NAME + wxT(" ")  + VERSION + wxT("\n\nhttp://dfast.sf.net") + _("\nCreated by Max Velasques"));

    wxMessageBox(message, _("About..."), wxOK | wxICON_INFORMATION, this);
}

void mMainFrame::OnExit(wxCommandEvent& event)
{
   Close();
}

void mMainFrame::OnClose(wxCloseEvent& event)
{
    int closedialog = programoptions.closedialog;
    if (event.CanVeto())
    {
        if ((closedialog) && (this->IsShown()))
        {
            wxMessageDialog *dlg = new wxMessageDialog(this, _("Do you want to close the program?"),_("Close..."),
                        wxYES_NO | wxICON_QUESTION);
            if (dlg)
            {
                dlg->Centre(wxBOTH);
                if (dlg->ShowModal() == wxID_YES)
                {
                    event.Skip();
                    dlg->Destroy();
                }
                else
                {
                    event.Veto();
                    dlg->Destroy();
                    return ;
                }
            }
        }
        else
        {
            event.Skip();
        }
    }
    else
        event.Skip();
    mtimer->Stop();
    {
        mDownloadFile *currentfile;
        mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
        wxProgressDialog *waitbox = new wxProgressDialog(_("Stopping the downloads..."),_("Stopping the downloads before exit..."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
        int total = list->GetItemCount();
        for (int i=0 ; i < total;i++)
        {
            list->SetCurrentSelection(i);
            StopDownload(wxGetApp().downloadlist.Item(i)->GetData());
        }
        for (int i=0 ; i < total;i++)
        {
            currentfile = wxGetApp().downloadlist.Item(i)->GetData();
            while ((currentfile->free == FALSE) && (waitbox->Update((int)(i*100/total))))
            {
                wxYield();
                wxMilliSleep(200);
            }
        }
        waitbox->Update(100);
        delete waitbox;
        wxMilliSleep(200);
    }


}

void mMainFrame::OnToolLeftClick(wxCommandEvent& event)
{
    if (event.GetId() == XRCID("toolnew"))
    {
        OnNew(event);
    }

    if (event.GetId() == XRCID("toolremove"))
    {
        OnRemove(event);
    }

    if (event.GetId() == XRCID("toolschedule"))
    {
        OnSchedule(event);
    }

    if (event.GetId() == XRCID("toolstart"))
    {
        OnStart(event);
    }

    if (event.GetId() == XRCID("toolstop"))
    {
        OnStop(event);
    }

    if (event.GetId() == XRCID("toolstartall"))
    {
        OnStartAll(event);
    }

    if (event.GetId() == XRCID("toolstopall"))
    {
        OnStopAll(event);
    }

    if (event.GetId() == XRCID("toolup"))
    {
        OnUpDown(TRUE);
    }

    if (event.GetId() == XRCID("tooldown"))
    {
        OnUpDown(FALSE);
    }

    if (event.GetId() == XRCID("toolproperties"))
    {
        OnProperties(event);
    }

    if (event.GetId() == XRCID("toolclose"))
    {
        OnExit(event);
    }
}

void mMainFrame::OnToolMouseMove(wxCommandEvent& event)
{
    if (event.GetSelection() == XRCID("toolnew"))
    {
        statusbar->SetStatusText(_("Add new download"));
    }
    else if (event.GetSelection() == XRCID("toolremove"))
    {
        statusbar->SetStatusText(_("Remove the selected download"));
    }
    else if (event.GetSelection() == XRCID("toolschedule"))
    {
        statusbar->SetStatusText(_("Mark the selected download as scheduled"));
    }
    else if (event.GetSelection() == XRCID("toolstart"))
    {
        statusbar->SetStatusText(_("Start the selected download"));
    }
    else if (event.GetSelection() == XRCID("toolstop"))
    {
        statusbar->SetStatusText(_("Stop the selected download"));
    }
    else if (event.GetSelection() == XRCID("toolstartall"))
    {
        statusbar->SetStatusText(_("Start all downloads"));
    }
    else if (event.GetSelection() == XRCID("toolstopall"))
    {
        statusbar->SetStatusText(_("Stop all downloads"));
    }
    else if (event.GetSelection() == XRCID("toolup"))
    {
        statusbar->SetStatusText(_("Raise in one level the selected download in the queue"));
    }
    else if (event.GetSelection() == XRCID("tooldown"))
    {
        statusbar->SetStatusText(_("Lower in one level the selected download in the queue"));
    }
    else if (event.GetSelection() == XRCID("toolproperties"))
    {
        statusbar->SetStatusText(_("Show the properties of the selected download"));
    }
    else if (event.GetSelection() == XRCID("toolclose"))
    {
        statusbar->SetStatusText(_("Close the program"));
    }
    else
        statusbar->SetStatusText(_("Visit http://dfast.sf.net for updates"));
}



