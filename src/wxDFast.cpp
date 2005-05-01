#include "wxDFast.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(mDownloadList);
WX_DEFINE_LIST(mGraphPoints);
IMPLEMENT_DYNAMIC_CLASS(mInProgressList, wxListCtrl)
IMPLEMENT_DYNAMIC_CLASS(mFinishedList, wxListCtrl)
IMPLEMENT_DYNAMIC_CLASS(mNotebook, wxNotebook)
IMPLEMENT_DYNAMIC_CLASS(mGraph, wxPanel)
IMPLEMENT_DYNAMIC_CLASS(mBoxOptionsColorPanel, wxPanel)
IMPLEMENT_APP(mApplication)

////////////////////////XPM IMAGES////////////////////////////////
#include "../resources/wxdfast.xpm"
#include "../resources/small/stop.xpm"
#include "../resources/small/start.xpm"
#include "../resources/small/ok.xpm"
#include "../resources/small/error.xpm"
#include "../resources/small/schedule.xpm"

wxConnectionBase *mServer::OnAcceptConnection(const wxString& topic)
{
    if ( topic == IPC_TOPIC + wxGetUserId())
    {
        mConnection *connection = new mConnection();
        return connection;
    }
    return NULL;
}

bool mConnection::OnExecute(const wxString& topic, wxChar* data, int size, wxIPCFormat format)
{
    wxString url = data;
    url.Truncate(url.First(wxT("|-|")));
    if (url != wxEmptyString)
	    wxGetApp().mainframe->NewDownload(url,wxGetApp().mainframe->programoptions.destination,1,wxEmptyString,wxEmptyString,wxEmptyString,TRUE,TRUE);
	else if (!wxGetApp().mainframe->IsShown())
	{
		wxCommandEvent event;
	    wxGetApp().mainframe->taskbaricon->OnHide(event);
	}
    return TRUE;
}

bool mApplication::NewInstance()
{
    wxString server = IPC_SERVICE;
    wxString hostName;
    hostName = wxGetHostName();
    bool returntmp;

    mClient *client = new mClient;
    wxLogNull nolog;
    connection = (mConnection *)client->MakeConnection(hostName, server, IPC_TOPIC + wxGetUserId());
    if (!connection)
        returntmp = FALSE;
    else
    {
    	if (parameters->GetParamCount() > 0)
	        connection->Execute(parameters->GetParam(0)+wxT("|-|"),parameters->GetParam(0).Length()+3);
	    else
			connection->Execute(wxEmptyString);
        returntmp = TRUE;
    }

    delete connection;
    delete client;
    return returntmp;
}

mApplication::mApplication(): m_condAllDone(m_mutexAllDone)
{
    // the mutex associated with a condition must be initially locked, it will
    // only be unlocked when we call Wait()
    m_mutexAllDone.Lock();
    m_waitingUntilAllDone = FALSE;
}

mApplication::~mApplication()
{

}

bool mApplication::OnInit()
{
//	const wxString name = wxString::Format("MyApp-%s", wxGetUserId().c_str());
    parameters = new wxCmdLineParser(cmdlinedesc, argc, argv);
    if (parameters->Parse() != 0)
        exit(TRUE);

//	wxProgressDialog *caixa = new wxProgressDialog(_("Verifying..."),_("Verifying if some instance is already active..."));
//    m_checker = new wxSingleInstanceChecker(name);
//    if ( m_checker->IsAnotherRunning() )
//    {
   	    if (NewInstance())
	        return FALSE;
//    }
//    caixa->Update(100);
//    delete caixa;

    wxString service = IPC_SERVICE; //REGISTER THIS INSTANCE
    m_server = new mServer;
    m_server->Create(service);

    wxSocketBase *dummy;
    dummy = new wxSocketBase(); //Para o socket funcionar dentro de threads*/
    
    wxImage::AddHandler(new wxXPMHandler);
    wxXmlResource::Get()->InitAllHandlers();
    #ifdef RESOURCES_CPP
    InitXmlResource();
    #else
    wxXmlResource::Get()->Load(wxT("resources/mainwindow.xrc"));
    wxXmlResource::Get()->Load(wxT("resources/boxnew.xrc"));
    wxXmlResource::Get()->Load(wxT("resources/boxoptions.xrc"));
    #endif
	m_locale = new wxLocale();
    m_locale->Init(Configurations(READ,LANGUAGE_REG,0));
    m_locale->AddCatalogLookupPathPrefix(wxT("languages"));
    m_locale->AddCatalog(wxT("wxDFast"));
    LoadDownloadListFromDisk();
    mainframe = new mMainFrame();
    {
    	int x,y,width,height,maximized,separatorposition01,separatorposition02,details;
    	wxSplitterWindow *splitter01 = XRCCTRL(*mainframe, "splitter01",wxSplitterWindow);
    	wxSplitterWindow *splitter02 = XRCCTRL(*mainframe, "splitter02",wxSplitterWindow);
    	x = Configurations(READ,POS_X_REG,-1);
    	y = Configurations(READ,POS_Y_REG,-1);
    	width = Configurations(READ,SIZE_X_REG,-1);
    	height = Configurations(READ,SIZE_Y_REG,-1);
		maximized = Configurations(READ,MAXIMIZED_REG,0);
		separatorposition01 = Configurations(READ,SEPARATOR01_REG,-1);
		separatorposition02 = Configurations(READ,SEPARATOR02_REG,-1);
		details = Configurations(READ,DETAILS_REG,-1);
		mainframe->SetSize(x,y,width,height);
		if (maximized)		   	mainframe->Maximize(TRUE);
		if (!details)			splitter01->Unsplit();
		if (separatorposition01  < 20)	separatorposition01 = 100;
		if (separatorposition02  < 20)	separatorposition02 = 100;
		splitter01->SetSashPosition(separatorposition01);
		splitter02->SetSashPosition(separatorposition02);
    }

    if (!parameters->Found(wxT("hide")))
	    mainframe->Show(true);
    return true;
}

int mApplication::OnExit()
{
     // the mutex must be unlocked before being destroyed
    m_mutexAllDone.Unlock();    
    delete m_server;
//    delete m_checker;
    return 0;
}

wxString mApplication::Configurations(int operation, wxString option,wxString value)
{
	wxString tmpvalue = value;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(CONFIG_REG);
    if (operation == WRITE)
        config->Write(option,tmpvalue);
    else
        config->Read(option,&tmpvalue);
    delete config;
    return tmpvalue;
}

int mApplication::Configurations(int operation, wxString option,int value)
{
	int tmpvalue = value;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(CONFIG_REG);
    if (operation == WRITE)
        config->Write(option,tmpvalue);
    else
        config->Read(option,&tmpvalue);
    delete config;
    return tmpvalue;
}

BEGIN_EVENT_TABLE(mMainFrame,wxFrame)
    EVT_MENU(XRCID("menunew"),  mMainFrame::OnNew)
    EVT_MENU(XRCID("menuremove"),  mMainFrame::OnRemove)
    EVT_MENU(XRCID("menustart"),  mMainFrame::OnStart)
    EVT_MENU(XRCID("menustop"),  mMainFrame::OnStop)
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
    wxBitmap image[5];
    int i;
    image[0] = wxXPM(stop_xpm);
    image[1] = wxXPM(start_xpm);
    image[2] = wxXPM(ok_xpm);
    image[3] = wxXPM(error_xpm);
    image[4] = wxXPM(schedule_xpm);
    for (i=0;i<=4;i++)
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

	//LOAD USER OPTIONS
	programoptions.closedialog = wxGetApp().Configurations(READ,OPT_DIALOG_CLOSE_REG,1);
	programoptions.destination = wxGetApp().Configurations(READ,OPT_DESTINATION_REG,wxGetHomeDir());
	programoptions.attempts = wxGetApp().Configurations(READ,OPT_ATTEMPTS_REG,999);
	programoptions.attemptstime = wxGetApp().Configurations(READ,OPT_ATTEMPTS_TIME_REG,5);
	programoptions.simultaneous = wxGetApp().Configurations(READ,OPT_SIMULTANEOUS_REG,5);
	programoptions.alwaysshutdown = wxGetApp().Configurations(READ,OPT_SHUTDOWN_REG,0);
	programoptions.shutdown = programoptions.alwaysshutdown;
	programoptions.alwaysdisconnect = wxGetApp().Configurations(READ,OPT_DISCONNECT_REG,0);
	programoptions.timerupdateinterval = wxGetApp().Configurations(READ,OPT_TIMERINTERVAL_REG,500);
	programoptions.disconnect = programoptions.alwaysdisconnect;
	#ifdef __WXMSW__
	programoptions.shutdowncmd = wxGetApp().Configurations(READ,OPT_SHUTDOWN_CMD_REG,wxT("c:\\windows\\system32\\shutdown.exe -s -t 0"));
	programoptions.disconnectcmd = wxGetApp().Configurations(READ,OPT_DISCONNECT_CMD_REG,wxT("c:\\windows\\system32\\rasdial.exe /disconnect"));
	#else
	programoptions.shutdowncmd = wxGetApp().Configurations(READ,OPT_SHUTDOWN_CMD_REG,wxT("/sbin/shutdown -h now"));
	programoptions.disconnectcmd = wxGetApp().Configurations(READ,OPT_DISCONNECT_CMD_REG,wxT("/usr/bin/poff"));
	#endif
	programoptions.graphhowmanyvalues = wxGetApp().Configurations(READ,OPT_GRAPH_HOWMANYVALUES_REG, 200);
	programoptions.graphrefreshtime = wxGetApp().Configurations(READ,OPT_GRAPH_REFRESHTIME_REG, 1000);
	programoptions.graphscale = wxGetApp().Configurations(READ,OPT_GRAPH_SCALE_REG, 40);
	programoptions.graphtextarea = wxGetApp().Configurations(READ,OPT_GRAPH_TEXTAREA_REG, 80);
	programoptions.graphspeedfontsize = wxGetApp().Configurations(READ,OPT_GRAPH_SPEEDFONTSIZE_REG, 18);
	programoptions.graphlinewidth = wxGetApp().Configurations(READ,OPT_GRAPH_LINEWIDTH_REG, 3);
	{
		wxString colour;
		long red,green,blue;
		colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORBACK_REG, wxT(""));
		if (!colour.Mid(1,3).ToLong(&red))			red = wxBLACK->Red();
		if (!colour.Mid(4,3).ToLong(&green))		green = wxBLACK->Green();
		if (!colour.Mid(7,3).ToLong(&blue))			blue = wxBLACK->Blue();
		programoptions.graphbackcolor.Set(red,green,blue);
	}
	{
		wxString colour;
		long red,green,blue;
		colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORGRID_REG, wxT(""));
		if (!colour.Mid(1,3).ToLong(&red))			red = wxGREEN->Red();
		if (!colour.Mid(4,3).ToLong(&green))		green = wxGREEN->Green();
		if (!colour.Mid(7,3).ToLong(&blue))			blue = wxGREEN->Blue();
		programoptions.graphgridcolor.Set(red,green,blue);
	}
	{
		wxString colour;
		long red,green,blue;
		colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORLINE_REG, wxT(""));
		if (!colour.Mid(1,3).ToLong(&red))			red = wxRED->Red();
		if (!colour.Mid(4,3).ToLong(&green))		green = wxRED->Green();
		if (!colour.Mid(7,3).ToLong(&blue))			blue = wxRED->Blue();
		programoptions.graphlinecolor.Set(red,green,blue);
	}
	{
		wxString colour;
		long red,green,blue;
		colour = wxGetApp().Configurations(READ,OPT_GRAPH_COLORFONT_REG, wxT(""));
		if (!colour.Mid(1,3).ToLong(&red))			red = wxBLUE->Red();
		if (!colour.Mid(4,3).ToLong(&green))		green = wxBLUE->Green();
		if (!colour.Mid(7,3).ToLong(&blue))			blue = wxBLUE->Blue();
		programoptions.graphfontcolor.Set(red,green,blue);
	}
	XRCCTRL(*(this), "graphpanel",mGraph )->graph = &graph;
	XRCCTRL(*(this), "graphpanel",mGraph )->programoptions = &programoptions;
	
	menubar->GetMenu(3)->Check(XRCID("menushutdown"),programoptions.shutdown);
	menubar->GetMenu(3)->Check(XRCID("menudisconnect"),programoptions.disconnect);

	//GENERATE THE LISTS
    GenerateInProgressList();
    GenerateFinishedList();

	//CREATE THE POPUPMENU FOR THE LISTS
    menupopup = new wxMenu;
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
    remove->SetBitmap(image[3]);
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

	//CREATE TASKBARICON
	taskbaricon = new mTaskBarIcon();
	if (!(wxGetApp().parameters->Found(wxT("notray"))))
		#ifdef __WXMSW__
		taskbaricon->SetIcon(wxICON(wxdfast_ico),PROGRAM_NAME);
		#else
		taskbaricon->SetIcon(wxICON(wxdfast),PROGRAM_NAME);
		#endif
	taskbaricon->restoring = FALSE;

    //IF A URL WAS PASSED BY THE COMMAND LINE
    if (wxGetApp().parameters->GetParamCount() > 0)
    {
        wxString url = wxGetApp().parameters->GetParam(0);
        if (url != wxEmptyString)
            NewDownload(url,programoptions.destination,1,wxEmptyString,wxEmptyString,wxEmptyString,TRUE,TRUE);
    }
    mtimer = new wxTimer(this, TIMER_ID);
    timerinterval = 0;
    mtimer->Start(programoptions.timerupdateinterval);
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
	mInProgressList* list01 = XRCCTRL(*this, "inprogresslist",mInProgressList );
	mFinishedList* list02 = XRCCTRL(*this, "finishedlist",mFinishedList );
	int selection = list01->GetCurrentSelection();
	int simultaneous = programoptions.simultaneous;
	bool somedownloadfinishednow = FALSE;
	long currentspeed = 0;
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
		mDownloadFile *current = node->GetData();
		if ((current->status == STATUS_FINISHED) && (current->free))
		{
			int i = list02->GetItemCount();
			list02->InsertItem(i, wxEmptyString);
	        list02->SetItem(i, FINISHED_ICON01, wxEmptyString,current->status);
	        list02->SetItem(i, FINISHED_NAME, current->name);
	        list02->SetItem(i, FINISHED_SIZE, ByteString(current->totalsize));
	        list02->SetItem(i, FINISHED_END, current->end.Format());
	        list02->SetItemData(i,current->end.GetTicks());
            node = node->GetPrevious();  //GO TO THE PREVIOUS NODE BEFORE DELETE THE CURRENT ONE
			list01->RemoveItemListandFile(current->index);
			somedownloadfinishednow = TRUE;
            if (current->index == 0)  //IF THIS IS THE FIRST NODE, WE CAN GO TO THE PREVIOUS ONE
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
					current->status = STATUS_STOPED;
				if (current->status == STATUS_ACTIVE)
					simultaneous--;
				if (current->split != WAIT)
				{
					int i;
					current->totalsizecompleted = 0;
					
					for (i=0; i < parts; i++)
						current->totalsizecompleted += current->sizecompleted[i];
					if (current->totalspeed > 0)
						current->timeremaining = 1000l*((current->totalsize - current->totalsizecompleted)/current->totalspeed);
			        if (current->totalsize > 0)
			        	current->percentual = (int)(100*((double)current->totalsizecompleted) / ((double)current->totalsize));
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
		        	XRCCTRL(*(this), "messages",wxHtmlWindow)->SetPage(wxEmptyString);
		        	XRCCTRL(*(this), "messages",wxHtmlWindow)->AppendToPage(current->messages[treeindex]);
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

    if (timerinterval >= (programoptions.graphrefreshtime))
    {
    	float *value = new float();
	    *value = ((float)currentspeed) / 1024.0;
    	graph.Insert(value);
    		
    	if (graph.GetCount() > (unsigned int)programoptions.graphhowmanyvalues)
    	{
    		float *lastvalue = graph.GetLast()->GetData();
    		graph.DeleteNode(graph.GetLast());
    		delete lastvalue;
    	}
    	timerinterval = 0;
    	if (this->IsShown())
		    XRCCTRL(*this, "graphpanel",mGraph )->Refresh();
    }
    timerinterval += mtimer->GetInterval();
    if (simultaneous > 0)
    {
	    for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
	    {
			mDownloadFile *current = node->GetData();
			if (current->status == STATUS_QUEUE)
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
			mDownloadFile *current = node->GetData();
			if ((current->status != STATUS_STOPED) && (current->status != STATUS_ERROR) && (current->status != STATUS_FINISHED))
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
    list03->SetColumnWidth(0,100);
    list03->SetColumnWidth(1,400);
    list03->InsertItem(0, _("Name"));
    list03->InsertItem(1, _("Size"));
    list03->InsertItem(2, _("Speed"));
    list03->InsertItem(3, _("Time"));
    list03->InsertItem(4, _("Destination"));
    list03->InsertItem(5, _("Start"));
    list03->InsertItem(6, _("Finished"));
	list03->InsertItem(7, _("MD5"));
	list03->InsertItem(8, _("URLs"));
	list03->InsertItem(9, _("Comments"));

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

bool mMainFrame::NewDownload(wxString url, wxString destination,int parts,wxString user,wxString password,wxString comments,bool now, bool show)
{
    mBoxNew dlg;
    wxTextCtrl *edturl, *edtdestination, *edtuser ,*edtpassword, *edtcomments;
    wxSpinCtrl *spinsplit;
    wxRadioButton *optnow;
    int RETURN;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("boxnew"));

    edturl = XRCCTRL(dlg, "edturl",wxTextCtrl);
    edtdestination = XRCCTRL(dlg, "edtdestination",wxTextCtrl);
    edtuser = XRCCTRL(dlg, "edtuser",wxTextCtrl);
    edtpassword = XRCCTRL(dlg, "edtpassword",wxTextCtrl);
    edtcomments = XRCCTRL(dlg, "edtcomments",wxTextCtrl);
    optnow = XRCCTRL(dlg, "optnow",wxRadioButton);
    spinsplit = XRCCTRL(dlg, "spinsplit",wxSpinCtrl);

    edturl->SetValue(url);
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
    optnow->SetValue(now);
    spinsplit->SetValue(parts);
    if (show)
    	RETURN = dlg.ShowModal();
    else
    	RETURN = XRCID("btnok");

    if (RETURN == XRCID("btnok"))
    {
    	mUrlName urltmp;
    	wxFileName destinationtmp;
    	int index;
    	urltmp.Assign(edturl->GetValue());
    	destinationtmp.Assign(edtdestination->GetValue());
    	if (!wxGetApp().FindDownloadFile(urltmp.GetFullName()))
    	{
	    	index = wxGetApp().CreateDownloadRegister(urltmp,destinationtmp, spinsplit->GetValue(), edtuser->GetValue(), edtpassword->GetValue(), edtcomments->GetValue());
	    	XRCCTRL(*this, "inprogresslist",mInProgressList )->Insert(wxGetApp().downloadlist.Item(index)->GetData(),-1);
    	}
    	else
    	{
    		wxMessageBox(_("This file already is being downloaded!"),_("Error...") ,wxOK | wxICON_ERROR,this);
    		return FALSE;
    	}
    	XRCCTRL(*this, "inprogresslist",mInProgressList )->SetCurrentSelection(index);
    	if (optnow->GetValue())
			wxGetApp().downloadlist.Item(index)->GetData()->status = STATUS_QUEUE;
    }
    return TRUE;
}

void mMainFrame::OnNew(wxCommandEvent& event)
{
	//NewDownload(wxEmptyString,programoptions.destination,1,wxEmptyString,wxEmptyString,wxEmptyString,FALSE,TRUE);
	OnPasteURL(event);
}

void mMainFrame::OnRemove(wxCommandEvent& event)
{
	int currentselection;
	mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
	if ((currentselection = list->GetCurrentSelection()) >= 0)
	{
		mDownloadFile *currentfile = wxGetApp().downloadlist.Item(currentselection)->GetData();
		{
			wxProgressDialog *waitbox = new wxProgressDialog(_("Stopping the download..."),_("Stopping the current download before remove..."));
	    	OnStop(event);
	    	while (currentfile->free == FALSE)
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
					list->RemoveItemListandFile(currentselection);
		            if (destination.Mid(destination.Length()-1,1) != SEPARATOR_DIR)
        		        destination = destination + SEPARATOR_DIR;
				    for (int i = 0; i< MAX_NUM_PARTS;i++)
				    	::wxRemoveFile(destination + PREFIX + name + EXT + int2wxstr(i));
                }
                else if (resp == wxID_NO)
                {
           			wxGetApp().RemoveListItemFromDisk(currentfile);
					list->RemoveItemListandFile(currentselection);
                }
                dlg->Destroy();
            }
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
			file->status = STATUS_QUEUE;
	}
}	

bool mMainFrame::StartDownload(mDownloadFile *downloadfile)
{
	if (downloadfile->free)
	{
		downloadfile->free = FALSE;
		downloadfile->split = WAIT;
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
		mDownloadFile *file = wxGetApp().downloadlist.Item(currentselection)->GetData();
		if ((file->status =! STATUS_FINISHED) && (file->status != STATUS_ERROR))
			file->status = STATUS_STOPED;
	}
}

void mMainFrame::OnPasteURL(wxCommandEvent& event)
{
    wxString urltmp,destinationtmp;
    int now = TRUE;
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            urltmp = data.GetText().Strip(wxString::both);
            destinationtmp = programoptions.destination;
            NewDownload(urltmp,destinationtmp,5,wxEmptyString,wxEmptyString,wxEmptyString,now,TRUE);
            
        }
        else
            NewDownload(wxEmptyString,destinationtmp,5,wxEmptyString,wxEmptyString,wxEmptyString,now,TRUE);
        wxTheClipboard->Close();
    }
    else
        wxMessageBox(_("It was impossible to open the clipboard!"),_("Error...") ,wxOK | wxICON_ERROR,this);
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
	}
}

void mMainFrame::OnLanguages(wxCommandEvent& event)
{
    wxString langs[] =
    {
        _("(Default)"),
        _("English"),
        _("Portuguese(Brazil)"),
        _("Italian"),
        _("Spanish")
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
            case 3 : langvalue = wxLANGUAGE_ITALIAN; break;
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
		if (dlg.ShowModal() == XRCID("btnok"))
		{
            mUrlName urltmp;
			currentfile->urllist = XRCCTRL(dlg, "edturl",wxTextCtrl)->GetValue();
			currentfile->destination = XRCCTRL(dlg, "edtdestination",wxTextCtrl)->GetValue();
			currentfile->user = XRCCTRL(dlg, "edtuser",wxTextCtrl)->GetValue();
			currentfile->password = XRCCTRL(dlg, "edtpassword",wxTextCtrl)->GetValue();
			currentfile->comments = XRCCTRL(dlg, "edtcomments",wxTextCtrl)->GetValue();
            urltmp.Assign(currentfile->urllist);
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
        wxString url,destination,user,password,comments;
        int parts;
        item.SetId(currentselection);
        item.SetColumn(FINISHED_NAME);
        item.SetMask(wxLIST_MASK_DATA|wxLIST_MASK_STATE|wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE);
        list->GetItem(item);
        
        config->SetPath(FINISHED_REG);
        config->SetPath(item.GetText());

        url = wxEmptyString;
        config->Read(URL1_REG,&url);
        
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
        
        if (NewDownload(url, destination, parts, user, password, comments, TRUE, FALSE))
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
	
	XRCCTRL(dlg, "spinattempts", wxSpinCtrl)->SetValue(programoptions.attempts);
	XRCCTRL(dlg, "spinattemptstime", wxSpinCtrl)->SetValue(programoptions.attemptstime);
	XRCCTRL(dlg, "spinsimultaneous", wxSpinCtrl)->SetValue(programoptions.simultaneous);
	XRCCTRL(dlg, "edtshutdown",wxTextCtrl)->SetValue(programoptions.shutdowncmd);
	XRCCTRL(dlg, "edtdisconnect",wxTextCtrl)->SetValue(programoptions.disconnectcmd);
	XRCCTRL(dlg, "chkclosedialog",wxCheckBox)->SetValue(programoptions.closedialog);
	XRCCTRL(dlg, "chkshutdown",wxCheckBox)->SetValue(programoptions.alwaysshutdown);
	XRCCTRL(dlg, "chkdisconnect",wxCheckBox)->SetValue(programoptions.alwaysdisconnect);
	XRCCTRL(dlg, "edtdestination",wxTextCtrl)->SetValue(programoptions.destination);
	XRCCTRL(dlg, "spintimerinterval",wxSpinCtrl)->SetValue(programoptions.timerupdateinterval);
	XRCCTRL(dlg, "spingraphpoints",wxSpinCtrl)->SetValue(programoptions.graphhowmanyvalues);
	XRCCTRL(dlg, "spingraphrefreshrate",wxSpinCtrl)->SetValue(programoptions.graphrefreshtime);
	XRCCTRL(dlg, "spingraphmaxvalue",wxSpinCtrl)->SetValue(programoptions.graphscale);
	XRCCTRL(dlg, "spingraphtextarea",wxSpinCtrl)->SetValue(programoptions.graphtextarea);
	XRCCTRL(dlg, "spingraphfontsize",wxSpinCtrl)->SetValue(programoptions.graphspeedfontsize);
	XRCCTRL(dlg, "spingraphlinewidth",wxSpinCtrl)->SetValue(programoptions.graphlinewidth);
	XRCCTRL(dlg, "graphpanelback", mBoxOptionsColorPanel)->colour = programoptions.graphbackcolor;
	XRCCTRL(dlg, "graphpanelgrid", mBoxOptionsColorPanel)->colour = programoptions.graphgridcolor;
	XRCCTRL(dlg, "graphpanelline", mBoxOptionsColorPanel)->colour = programoptions.graphlinecolor;
	XRCCTRL(dlg, "graphpanelfont", mBoxOptionsColorPanel)->colour = programoptions.graphfontcolor;
	
	if (dlg.ShowModal() == XRCID("btnoptionsave"))
	{
		programoptions.attempts = XRCCTRL(dlg, "spinattempts", wxSpinCtrl)->GetValue();
		programoptions.attemptstime = XRCCTRL(dlg, "spinattemptstime", wxSpinCtrl)->GetValue();
		programoptions.simultaneous = XRCCTRL(dlg, "spinsimultaneous", wxSpinCtrl)->GetValue();
		programoptions.shutdowncmd = XRCCTRL(dlg, "edtshutdown",wxTextCtrl)->GetValue();
		programoptions.disconnectcmd = XRCCTRL(dlg, "edtdisconnect",wxTextCtrl)->GetValue();
		programoptions.closedialog = XRCCTRL(dlg, "chkclosedialog",wxCheckBox)->GetValue();
		programoptions.alwaysshutdown = XRCCTRL(dlg, "chkshutdown",wxCheckBox)->GetValue();
		programoptions.alwaysdisconnect = XRCCTRL(dlg, "chkdisconnect",wxCheckBox)->GetValue();
		programoptions.destination = XRCCTRL(dlg, "edtdestination",wxTextCtrl)->GetValue();
		programoptions.timerupdateinterval = XRCCTRL(dlg, "spintimerinterval",wxSpinCtrl)->GetValue();
		programoptions.graphhowmanyvalues = XRCCTRL(dlg, "spingraphpoints",wxSpinCtrl)->GetValue();
		programoptions.graphrefreshtime = XRCCTRL(dlg, "spingraphrefreshrate",wxSpinCtrl)->GetValue();
		programoptions.graphscale = XRCCTRL(dlg, "spingraphmaxvalue",wxSpinCtrl)->GetValue();
		programoptions.graphtextarea = XRCCTRL(dlg, "spingraphtextarea",wxSpinCtrl)->GetValue();
		programoptions.graphspeedfontsize = XRCCTRL(dlg, "spingraphfontsize",wxSpinCtrl)->GetValue();
		programoptions.graphlinewidth = XRCCTRL(dlg, "spingraphlinewidth",wxSpinCtrl)->GetValue();
	 	programoptions.graphbackcolor = XRCCTRL(dlg, "graphpanelback", mBoxOptionsColorPanel)->colour;
		programoptions.graphgridcolor = XRCCTRL(dlg, "graphpanelgrid", mBoxOptionsColorPanel)->colour;
		programoptions.graphlinecolor = XRCCTRL(dlg, "graphpanelline", mBoxOptionsColorPanel)->colour;
		programoptions.graphfontcolor = XRCCTRL(dlg, "graphpanelfont", mBoxOptionsColorPanel)->colour;
	
		wxGetApp().Configurations(WRITE,OPT_DIALOG_CLOSE_REG,programoptions.closedialog);
		wxGetApp().Configurations(WRITE,OPT_DESTINATION_REG,programoptions.destination);
		wxGetApp().Configurations(WRITE,OPT_ATTEMPTS_REG,programoptions.attempts);
		wxGetApp().Configurations(WRITE,OPT_ATTEMPTS_TIME_REG,programoptions.attemptstime);
		wxGetApp().Configurations(WRITE,OPT_SIMULTANEOUS_REG,programoptions.simultaneous);
		wxGetApp().Configurations(WRITE,OPT_SHUTDOWN_REG,programoptions.alwaysshutdown);
		wxGetApp().Configurations(WRITE,OPT_DISCONNECT_REG,programoptions.alwaysdisconnect);
		wxGetApp().Configurations(WRITE,OPT_SHUTDOWN_CMD_REG,programoptions.shutdowncmd);
		wxGetApp().Configurations(WRITE,OPT_DISCONNECT_CMD_REG,programoptions.disconnectcmd);
		wxGetApp().Configurations(WRITE,OPT_TIMERINTERVAL_REG,programoptions.timerupdateinterval);
		wxGetApp().Configurations(WRITE,OPT_GRAPH_HOWMANYVALUES_REG, programoptions.graphhowmanyvalues);
		wxGetApp().Configurations(WRITE,OPT_GRAPH_REFRESHTIME_REG, programoptions.graphrefreshtime);
		wxGetApp().Configurations(WRITE,OPT_GRAPH_SCALE_REG, programoptions.graphscale);
		wxGetApp().Configurations(WRITE,OPT_GRAPH_TEXTAREA_REG, programoptions.graphtextarea);
		wxGetApp().Configurations(WRITE,OPT_GRAPH_SPEEDFONTSIZE_REG, programoptions.graphspeedfontsize);
		wxGetApp().Configurations(WRITE,OPT_GRAPH_LINEWIDTH_REG, programoptions.graphlinewidth);
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
		XRCCTRL(*(this), "graphpanel",mGraph )->Refresh();
	}	
}

void mMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString message;
    message.Printf( PROGRAM_NAME + wxT(" ")  + VERSION + _("\n\nCreated by Max Velasques"));

    wxMessageBox(message, _("About..."), wxOK | wxICON_INFORMATION, this);
}

void mMainFrame::OnExit(wxCommandEvent& event)
{
   Close();
}

void mMainFrame::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto())
    {
        if ((programoptions.closedialog) && (this->IsShown()))
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
		wxCommandEvent dummieevent;
		mDownloadFile *currentfile;
		mInProgressList *list = XRCCTRL(*this, "inprogresslist",mInProgressList );
		wxProgressDialog *waitbox = new wxProgressDialog(_("Stopping the downloads..."),_("Stopping the downloads before exit..."),100,NULL,wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
		int total = list->GetItemCount();
		for (int i=0 ; i < total;i++)
		{
			list->SetCurrentSelection(i);
			OnStop(dummieevent);
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
    
    if (event.GetId() == XRCID("toolstart"))
    {
        OnStart(event);
    }

    if (event.GetId() == XRCID("toolstop"))
    {
        OnStop(event);
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

BEGIN_EVENT_TABLE(mGraph, wxPanel)
    EVT_PAINT  (mGraph::OnPaint)
END_EVENT_TABLE()

void mGraph::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	if (!programoptions)
		return;
	int x, y, lastx, lasty, count;
	float dx, dy;

	//DEFINE DE PARAMETERS
	int scale = programoptions->graphscale;
	int textarea = programoptions->graphtextarea;
	wxFont smallfont,bigfont;
    bigfont.SetPointSize(programoptions->graphspeedfontsize);
    smallfont.SetPointSize(10);
    
	//SET THE BACKGROUND COLOR
    wxBrush b(programoptions->graphbackcolor, wxSOLID);
    dc.SetBackground(b);
    dc.Clear();

	//GET THE LIMITS OF THE WINDOW
    this->GetSize(&x,&y);
    dx = 3.0;
    dy = ((float) y)/((float) scale);
    
    //DRAW A GRID FOR THE GRAPH
    wxPen gridpen(programoptions->graphgridcolor);
    dc.SetPen(gridpen);
    int grid = y/5;
    count = 1;
    while (y >= (grid*count))
    {
    	dc.DrawLine(textarea,y-grid*count ,x-((int)dx),y-grid*count);
		count++;
    }
    dc.DrawLine(textarea,0 ,textarea,y);


	//DRAW THE GRAPH
    count = 1;
    wxPen linepen(programoptions->graphlinecolor);
    linepen.SetWidth(programoptions->graphlinewidth);
    dc.SetPen(linepen);    
    mGraphPoints::Node *node = graph->GetFirst();
    if (node)
    {
    	float *current = node->GetData();
    	
    	//WRITE THE CURRENT SPEED
    	wxString temp;
    	temp.Printf(wxT("%0.1f"), *current);
    	temp.Replace(wxT(","),wxT("."));
    	dc.SetTextForeground(programoptions->graphfontcolor);
    	dc.SetFont(bigfont);
    	dc.DrawText(temp,5,5);
    	dc.SetFont(smallfont);
    	dc.DrawText(wxT("kb/s"),5,programoptions->graphspeedfontsize + 10);
    	
	    lastx = x-((int)dx);
	    lasty = y-((int)(dy*(*current)));
	    for ( node = node->GetNext(); node; node = node->GetNext() )
	    {
	        float *current = node->GetData();
	        count++;
	        //                 X1        ,Y1,          X2     ,           Y2
	        dc.DrawLine(lastx,lasty ,x-((int)dx)*count,y-((int)(dy*(*current))));
		    lastx = x-((int)dx*count);
		    lasty = y-((int)(dy*(*current)));
		    
		    //DON'T DRAW THE GRAPH INSIDE THE TEXT AREA
		    if ((lastx <= textarea+3) || (programoptions->graphhowmanyvalues <= count+1))
		        break;
	    }
    }
    else
    {
    	//WRITE THE DEFAULT SPEED
    	dc.SetTextForeground(*wxBLUE);
    	dc.SetFont(bigfont);
    	dc.DrawText(wxT("0.0"),5,5);
    	dc.SetFont(smallfont);
    	dc.DrawText(wxT("kb/s"),5,programoptions->graphspeedfontsize + 10);
    }
}

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
	wxString taskTip,temp;
    if(!wxGetApp().downloadlist.IsEmpty())
	{for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
	{
		mDownloadFile *current = node->GetData();
		temp.Printf(_("%s  Completed: %d%%  Speed: %.2f Kb/s\n"),current->name.c_str(),current->percentual,float(current->totalspeed)/1000.0);
		taskTip+=temp;
	}
	taskTip.RemoveLast();
      #ifdef __WXMSW__
	SetIcon(wxICON(wxdfast_ico),taskTip);
	#else
	SetIcon(wxICON(wxdfast),taskTip);
	#endif
	}
    else
	SetIcon(wxICON(wxdfast),wxT("wxDfast"));
}

wxMenu *mTaskBarIcon::CreatePopupMenu()
{
    wxMenu *traymenu = new wxMenu;
	wxMenuItem *mnuhide;
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
    return traymenu;
}

void mTaskBarIcon::OnClose(wxCommandEvent& event)
{
    wxGetApp().mainframe->Close();
}

void mTaskBarIcon::OnNew(wxCommandEvent& event)
{
    wxGetApp().mainframe->OnNew(event);
}

void mTaskBarIcon::OnHide(wxCommandEvent& event)
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
