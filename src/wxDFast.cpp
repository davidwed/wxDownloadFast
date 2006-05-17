//
// C++ Implementation: wxDFast
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
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(mDownloadList);
WX_DEFINE_LIST(mGraphPoints);
IMPLEMENT_DYNAMIC_CLASS(mInProgressList, wxListCtrl)
IMPLEMENT_DYNAMIC_CLASS(mFinishedList, wxListCtrl)
IMPLEMENT_DYNAMIC_CLASS(mNotebook, wxNotebook)
IMPLEMENT_DYNAMIC_CLASS(mGraph, wxPanel)
IMPLEMENT_DYNAMIC_CLASS(mBoxOptionsColorPanel, wxPanel)
IMPLEMENT_APP(mApplication)

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
    wxString urlsparameter = data;
    wxArrayString urls;
    wxStringTokenizer *tkz01, *tkz02;
    int numberofparts, startoption;
    wxString textfile,destination,comments,strurls,strtemp;
    wxTextFile file;

    tkz01 = new wxStringTokenizer(urlsparameter, wxT("$"));
    textfile = tkz01->GetNextToken();
    destination = tkz01->GetNextToken();
    comments = tkz01->GetNextToken();
    strurls = tkz01->GetNextToken();
    tkz02 = new wxStringTokenizer(strurls, wxT("#"));
    while (tkz02->HasMoreTokens())
    {
        strtemp = tkz02->GetNextToken();
        if (strtemp != wxEmptyString)
            urls.Add(strtemp);
    }
    wxLogNull nolog;
    if ((textfile != wxT("NONE")) && (file.Open(textfile)))
    {
        for ( strtemp = file.GetFirstLine(); !file.Eof(); strtemp = file.GetNextLine() )
            if (strtemp != wxEmptyString)
                urls.Add(strtemp);
        if (strtemp != wxEmptyString)
            urls.Add(strtemp);
    }
    if (wxGetApp().mainframe->programoptions.rememberboxnewoptions)
    {
        startoption = wxGetApp().mainframe->programoptions.laststartoption;
        numberofparts = wxGetApp().mainframe->programoptions.lastnumberofparts;
        if (destination == wxT("NONE"))
            destination = wxGetApp().mainframe->programoptions.lastdestination;
    }
    else
    {
        startoption = DEFAULT_START_OPTION;
        numberofparts = DEFAULT_NUM_PARTS;
        if (destination == wxT("NONE"))
            destination = wxGetApp().mainframe->programoptions.destination;
    }
    if (comments == wxT("NONE"))
        comments = wxEmptyString;

    if (urls.GetCount() > 0)
        wxGetApp().mainframe->NewDownload(urls,destination,numberofparts,wxEmptyString,wxEmptyString,comments,startoption,TRUE);
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
    wxString textfile, destination, comments, urlparameter = wxEmptyString,stringtosend;
    hostName = wxGetHostName();
    bool returntmp;

    mClient *client = new mClient;
    wxLogNull nolog;
    connection = (mConnection *)client->MakeConnection(hostName, server, IPC_TOPIC + wxGetUserId());
    if (!connection)
        returntmp = FALSE;
    else
    {
        if (!wxGetApp().parameters->Found(wxT("list"),&textfile))
            textfile = wxT("NONE");
        if (!wxGetApp().parameters->Found(wxT("destination"),&destination))
            destination = wxT("NONE");
        if (!wxGetApp().parameters->Found(wxT("comments"),&comments))
            comments = wxT("NONE");

        for (unsigned int i = 0; i < parameters->GetParamCount(); i++)
            urlparameter += parameters->GetParam(i) + wxT("#");
        stringtosend = textfile + wxT("$") + destination + wxT("$") + comments + wxT("$") + urlparameter + wxT("$");
        connection->Execute((wxChar *)stringtosend.c_str(),(stringtosend.Length() + 1) * sizeof(wxChar));
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
//    const wxString name = wxString::Format("MyApp-%s", wxGetUserId().c_str());
    parameters = new wxCmdLineParser(cmdlinedesc, argc, argv);
    if (parameters->Parse() != 0)
        exit(TRUE);

//    wxProgressDialog *caixa = new wxProgressDialog(_("Verifying..."),_("Verifying if some instance is already active..."));
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
    dummy->SetTimeout(0);

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
    mainframe = NULL;
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
        if (maximized)               mainframe->Maximize(TRUE);
        if (!details)            splitter01->Unsplit();
        if (separatorposition01  < 20)    separatorposition01 = 100;
        if (separatorposition02  < 20)    separatorposition02 = 100;
        splitter01->SetSashPosition(separatorposition01);
        splitter02->SetSashPosition(separatorposition02);
    }

    //IF A URL OR A TEXT FILE WAS PASSED BY THE COMMAND LINE 
    int startoption, numberofparts;
    wxString listtextfile,comments,destination;
    wxArrayString url; 
    if (parameters->GetParamCount() > 0)
    {
        int i, nparams = parameters->GetParamCount();
        for (i = 0;i<nparams; i++)
            if (parameters->GetParam(i) != wxEmptyString)
                url.Add(parameters->GetParam(i));
    }
    if (parameters->Found(wxT("list"),&listtextfile))
    {
        wxLogNull nolog;
        wxTextFile file(listtextfile);
        if (file.Open())
        {
            wxString str;
            for ( str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
                if (str != wxEmptyString)
                    url.Add(str);
            if (str != wxEmptyString)
                url.Add(str);
        }
    }
    if (mainframe->programoptions.rememberboxnewoptions)
    {
        numberofparts = mainframe->programoptions.lastnumberofparts;
        startoption = mainframe->programoptions.laststartoption;
        if (!parameters->Found(wxT("destination"),&destination))
            destination = mainframe->programoptions.lastdestination;
    }
    else
    {
        numberofparts = DEFAULT_NUM_PARTS;
        startoption = DEFAULT_START_OPTION;
        if (!parameters->Found(wxT("destination"),&destination))
            destination = mainframe->programoptions.destination;
    }
    if (!parameters->Found(wxT("comments"),&comments))
        comments = wxEmptyString;
    if (url.GetCount() > 0)
        mainframe->NewDownload(url,destination,numberofparts,wxEmptyString,wxEmptyString,comments,startoption,TRUE);


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

long mApplication::Configurations(int operation, wxString option,long value)
{
    wxString tmpvalue;
    long returnvalue;
    tmpvalue << value;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(CONFIG_REG);
    if (operation == WRITE)
        config->Write(option,tmpvalue);
    else
        config->Read(option,&tmpvalue);
    delete config;
    tmpvalue.ToLong(&returnvalue);
    return returnvalue;
}

