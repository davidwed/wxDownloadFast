#ifndef __WXPROG_H__
    #define __WXPROG_H__
    #include "wx/wxprec.h"
    #include "wx/wx.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/listctrl.h"
    #include "wx/treectrl.h"
    #include "wx/html/htmlwin.h"
    #include "wx/frame.h"
    #include "wx/fileconf.h"
    #include "wx/splitter.h"
    #include "wxMD5/wxMD5.h"
    #include "wx/imaglist.h"
    #include "wx/protocol/ftp.h"
    #include "wx/file.h"
    #include "wx/thread.h"
    #include "wx/toolbar.h"
    #include "wx/bitmap.h"
    #include "wx/filename.h"
    #include "wx/menu.h"
    #include "wx/clipbrd.h"
    #include "wx/sckstrm.h"
    #include "wx/fdrepdlg.h"
    #include "wx/cmdline.h"
    #include "wx/progdlg.h"
    #include "wx/ipc.h"
    #include "wx/process.h"
    #include "wx/socket.h"
    #include "wx/spinctrl.h"
    #include "wx/taskbar.h"
    #include "wx/xrc/xmlres.h"
    #include "wx/image.h"
    #include "wx/snglinst.h"
    #include "wx/pen.h"
    #include "wx/calctrl.h"
    #include "wx/tokenzr.h"
    #include "wx/wfstream.h"
    #include "wx/longlong.h"
    #ifdef RESOURCES_CPP
    extern void InitXmlResource();
    #endif

    const wxCmdLineEntryDesc cmdlinedesc[] =
    {
        { wxCMD_LINE_SWITCH, wxT("hide"), wxT("hide"), wxT("Start with the the main frame hide.")},
        { wxCMD_LINE_SWITCH, wxT("notray"), wxT("notray"),  wxT("Don't show the icon on system tray")},
        { wxCMD_LINE_OPTION, wxT("list"), wxT("list"),  wxT("Parse a text file with the list of files to download")},
        { wxCMD_LINE_OPTION, wxT("destination"), wxT("destination"),  wxT("Destination directory")},
        { wxCMD_LINE_OPTION, wxT("comments"), wxT("comments"),  wxT("Add comments to download")},
        { wxCMD_LINE_PARAM, NULL, NULL,  wxT("URL of the file(s) to be downloaded"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        { wxCMD_LINE_NONE }
    };

    #ifdef __WXMSW__
        #define wxXPM(x)  (wxBitmap((const char **)x))
    #else
        #define wxXPM(x)  (wxBitmap(x))
    #endif

    const wxString PROGRAM_NAME = wxT("wxDownload Fast");
    const wxString VERSION = wxT("0.3.2");
    const wxString SEPARATOR_URL = wxT("/");
    #ifdef __WXMSW__
       const wxString SEPARATOR_DIR = wxT("\\");
    #else
       const wxString SEPARATOR_DIR = wxT("/");
    #endif
    #define MAX_NUM_PARTS              9
    #define DEFAULT_NUM_PARTS          5
    #define MIN_SIZE_TO_SPLIT          500000l
    #define USE_HTML_MESSAGES          1 
                                        //CHANGE THIS AND THE FILE MAIN_WINDOW.XRC
    #define MAX_SCHEDULE_EXCEPTIONS    9

    #define IPC_SERVICE                wxT("24242")
    #define IPC_TOPIC                  wxT("IPC_WXDOWNLAD_FAST_")
    #define IPC_END_CONNECTION         wxT("IPC_WXDOWNLAD_FAST_END_CONECTION")

    const wxString DFAST_REG = wxT("wxDownloadFast");
    const wxString FILES_REG = wxT("files");
    const wxString INPROGRESS_REG = wxT("files/inprogress");
    const wxString FINISHED_REG = wxT("files/finished");
    const wxString BACK_DIR_REG = wxT("..");
    const wxString NAME_REG = wxT("name");
    const wxString INDEX_REG = wxT("index");
    const wxString STATUS_REG = wxT("status");
    const wxString SCHEDULED_REG = wxT("scheduled");
    const wxString RESTART_REG = wxT("restart");
    const wxString PARTS_REG = wxT("parts");
    const wxString DESTINATION_REG = wxT("destination");
    const wxString SIZE_REG = wxT("size");
    const wxString SIZECOMPLETED_REG = wxT("sizecompleted");
    const wxString TIMEPASSED_REG = wxT("timepassed");
    const wxString PERCENTUAL_REG = wxT("percentual");
    const wxString SPEED_REG = wxT("speed");
    const wxString URL1_REG = wxT("url1");
    const wxString MD5_REG = wxT("md5");
    const wxString COMMENTS_REG = wxT("comments");
    const wxString START_REG = wxT("date_start");
    const wxString END_REG = wxT("date_end");
    const wxString USER_REG = wxT("user");
    const wxString PASSWORD_REG = wxT("password");

    const wxString CONFIG_REG = wxT("config");
    const wxString LANGUAGE_REG = wxT("language");
    const wxString DETAILS_REG = wxT("details");
    const wxString SIZE_X_REG = wxT("width");
    const wxString SIZE_Y_REG = wxT("height");
    const wxString POS_X_REG = wxT("posx");
    const wxString POS_Y_REG = wxT("posy");
    const wxString MAXIMIZED_REG = wxT("maximized");
    const wxString SEPARATOR01_REG = wxT("separator01position");
    const wxString SEPARATOR02_REG = wxT("separator02position");

    const wxString OPT_DIALOG_CLOSE_REG = wxT("closewindow");
    const wxString OPT_ATTEMPTS_REG = wxT("attempts");
    const wxString OPT_ATTEMPTS_TIME_REG = wxT("attemptstime");
    const wxString OPT_SIMULTANEOUS_REG = wxT("simultaneous");
    const wxString OPT_DESTINATION_REG = wxT("dirdestination");
    const wxString OPT_SHUTDOWN_REG = wxT("shutdown");
    const wxString OPT_SHUTDOWN_CMD_REG = wxT("shutdowncmd");
    const wxString OPT_DISCONNECT_REG = wxT("disconnect");
    const wxString OPT_DISCONNECT_CMD_REG = wxT("disconnectcmd");
    const wxString OPT_TIMERINTERVAL_REG = wxT("timerinterval");
    const wxString OPT_READBUFFERSIZE_REG = wxT("readbuffersize");
    const wxString OPT_GRAPH_HOWMANYVALUES_REG = wxT("graphhowmanyvalues");
    const wxString OPT_GRAPH_REFRESHTIME_REG = wxT("graphrefreshtime");
    const wxString OPT_GRAPH_SCALE_REG = wxT("graphscale");
    const wxString OPT_GRAPH_TEXTAREA_REG = wxT("graphtextarea");
    const wxString OPT_GRAPH_SPEEDFONTSIZE_REG = wxT("graphspeedfontsize");
    const wxString OPT_GRAPH_LINEWIDTH_REG = wxT("graphlinewidth");
    const wxString OPT_GRAPH_COLORBACK_REG = wxT("graphcolorbackground");
    const wxString OPT_GRAPH_COLORGRID_REG = wxT("graphcolorgrid");
    const wxString OPT_GRAPH_COLORLINE_REG = wxT("graphcolorline");
    const wxString OPT_GRAPH_COLORFONT_REG = wxT("graphcolorfont");
    const wxString OPT_SCHED_ACTIVATESCHEDULING_REG = wxT("activatescheduling");
    const wxString OPT_SCHED_STARTDATETIME_REG = wxT("schedstartdatetime");
    const wxString OPT_SCHED_FINISHDATETIME_REG = wxT("schedfinishdatetime");
    const wxString OPT_SCHED_SCHEDULEEXCEPTION_START_REG = wxT("scheduleexceptionstart");
    const wxString OPT_SCHED_SCHEDULEEXCEPTION_FINISH_REG = wxT("scheduleexceptionfinish");
    const wxString OPT_SCHED_SCHEDULEEXCEPTION_DAY_REG = wxT("scheduleexceptionday");
    const wxString OPT_SCHED_SCHEDULEEXCEPTION_ISACTIVE_REG = wxT("scheduleexceptionisactive");


    const wxString EXT = wxT(".dfast");
    #ifdef __WXMSW__
    const wxString PREFIX = wxT("");
    #else
    const wxString PREFIX = wxT(".");
    #endif
    const wxString LINE = wxT("-");
    const wxString CRLF = wxT("\r\n");
    const wxString ANONYMOUS_USER = wxT("anonymous");
    const wxString ANONYMOUS_PASS = wxT("anonymous@anonymous.com");

    const wxColor RED = wxColor(255,0,30);
    const wxColor BLUE = wxColor(0,47,94);
    const wxColor GREEN = wxColor(0,98,65);
    const wxColor YELLOW = wxColor(170,170,0);

    const wxString HTMLERROR = wxT("#FF0030");         //RED
    const wxString HTMLSERVER = wxT("#009865");        //GREEN
    const wxString HTMLNORMAL = wxT("#002f5e");        //BLUE
    const wxString HTMLBLACK = wxT("#000000");        //BLACK

    const wxString days[7] = {_("Sunday"),_("Mondays"),_("Tuesdays"),
                              _("Wednesdays"),_("Thursdays"),_("Fridays"),
                              _("Saturdays")};

    const int INPROGRESS_ICON01 = 0;
    const int INPROGRESS_ICON02 = 1;
    const int INPROGRESS_NAME = 2;
    const int INPROGRESS_SIZE = 3;
    const int INPROGRESS_COMPLETED = 4;
    const int INPROGRESS_PERCENTUAL = 5;
    const int INPROGRESS_TIMEPASSED = 6;
    const int INPROGRESS_TIMEREMAINING = 7;
    const int INPROGRESS_SPEED = 8;
    const int INPROGRESS_ATTEMPTS = 9;
    const int INPROGRESS_URL = 10;

    const int FINISHED_ICON01 = 0;
    const int FINISHED_NAME = 1;
    const int FINISHED_SIZE = 2;
    const int FINISHED_END = 3;

    #define HTTP                1
    #define FTP                 2
    #define LOCAL_FILE          3
    #define READ                4
    #define WRITE               5

    #define HIDE                1000
    #define NEW                 1001
    #define CLOSE               1002
    #define TIMER_ID            1003

    //mDownloadFile STATUS
    #define STATUS_STOPED       0
    #define STATUS_ACTIVE       1
    #define STATUS_FINISHED     2
    #define STATUS_ERROR        3
    #define STATUS_QUEUE        4
    #define STATUS_SCHEDULE     5


    //mDownloadFile RESTART, SPLIT
    #define WAIT               -1
    #define YES                 0
    #define NO                  1

    class mDownloadFile
    {
    public:
        bool free;
        int index;
        int status;
        int restart;
        int scheduled;
        wxString name;            //DON'T CHANGE THIS VAR DIRECTLY - USE THE ChangeName() function
        wxString destination;
        wxString comments;
        wxString user;
        wxString password;
        int percentual;
        wxLongLong timepassed;
        wxLongLong timeremaining;
        wxString MD5;
        wxDateTime start;
        wxDateTime end;
        wxString urllist;
        int parts;
        int currentattempt;
        wxLongLong totalsize;
        wxLongLong totalsizecompleted;
        int totalspeed;
        bool criticalerror;
        int split;
        bool speedpoint;
        int speedpointowner;
        wxMutex *mutex_speedcalc;
        bool finished[MAX_NUM_PARTS];
        wxLongLong startpoint[MAX_NUM_PARTS];
        wxLongLong size[MAX_NUM_PARTS];
        wxLongLong sizecompleted[MAX_NUM_PARTS];
        long delta_size[MAX_NUM_PARTS];
        wxString messages[MAX_NUM_PARTS];
    };

    class mScheduleException
    {
    public:
        wxString start,finish;
        wxString newstart,newfinish;
        int day;    //0 - Sunday
        int newday; //1 - Monday ...
        int isactive;
    };

    class mOptions
    {
    public:
        int attempts;             //number of attempts
        int closedialog;         //show the close dialog
        int simultaneous;         //number of simultaneous downloads
        int attemptstime;         //time between the attempts in seconds
        int shutdown;
        int disconnect;
        int alwaysshutdown;
        int alwaysdisconnect;
        int timerupdateinterval; //time between the timer refreshs in milliseconds
        long readbuffersize;
        int graphhowmanyvalues;
        int graphrefreshtime;    //time between the graph refreshs in milliseconds
        int graphscale;          //max value showed in the graph
        int graphtextarea;       //size the area reserved for the speed value
        int graphspeedfontsize;
        wxColour graphbackcolor;
        wxColour graphgridcolor;
        wxColour graphlinecolor;
        int graphlinewidth;
        wxColour graphfontcolor;
        wxString shutdowncmd;
        wxString disconnectcmd;
        wxString destination;
        int activatescheduling;
        wxDateTime startdatetime;
        wxDateTime finishdatetime;
        mScheduleException scheduleexceptions[MAX_SCHEDULE_EXCEPTIONS];
        int scheduleexceptionschanged;
    };

    WX_DECLARE_LIST(mDownloadFile, mDownloadList);
    WX_DECLARE_LIST(float, mGraphPoints);

    class mUrlName;
    class mMainFrame;
    class mBoxNew;
    class mBoxOptions;
    class mInProgressList;
    class mFinishedList;
    class mTaskBarIcon;
    class mConnection;
    class mClient;
    class mServer;
    class mDownloadThread;

    WX_DEFINE_ARRAY(mDownloadThread *, mDownloadThreadArray);

    class mApplication : public wxApp
    {
    public:
        mApplication();
        virtual ~mApplication();
        virtual bool OnInit();
        virtual int OnExit();
        bool NewInstance();
        mDownloadList downloadlist;
        mDownloadThreadArray *downloadthreads;
        int CreateDownloadRegister(mUrlName url,wxFileName destination, int parts, wxString user, wxString password, wxString comments,int scheduled);
        void RegisterListItemOnDisk(mDownloadFile *file);
        void RecreateIndex();
        void LoadDownloadListFromDisk();
        void RemoveListItemFromDisk(mDownloadFile *file);
        mDownloadFile *FindDownloadFile(wxString str);
        void ChangeName(mDownloadFile *currentfile, wxString name, int value = 0);
        wxString Configurations(int operation, wxString option, wxString value);
        int Configurations(int operation, wxString option, int value);
        long Configurations(int operation, wxString option,long value);
        mMainFrame *mainframe;
        wxLocale *m_locale;
        wxCmdLineParser *parameters;
        mServer *m_server;
        mConnection *connection;
        wxSingleInstanceChecker *m_checker;
        wxCriticalSection m_critsect;
        wxMutex m_mutexAllDone;
        wxCondition m_condAllDone;
        bool m_waitingUntilAllDone;
    };

    DECLARE_APP(mApplication)

    class mMainFrame : public wxFrame
    {
    public:
        mMainFrame();
        ~mMainFrame();
        void OnTimer(wxTimerEvent& event);
        bool NewDownload(wxArrayString url, wxString destination,int parts,wxString user,wxString password,wxString comments,bool now, bool show);
        bool StartDownload(mDownloadFile *downloadfile);
        void StopDownload(mDownloadFile *downloadfile);
        void OnNew(wxCommandEvent& event);
        void OnRemove(wxCommandEvent& event);
        void OnSchedule(wxCommandEvent& event);
        void OnStart(wxCommandEvent& event);
        void OnStop(wxCommandEvent& event);
        void OnPasteURL(wxCommandEvent& event);
        void OnCopyURL(wxCommandEvent& event);
        void OnFind(wxCommandEvent& event);
        void OnDetails(wxCommandEvent& event);
        void OnLanguages(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
        void OnMove(wxCommandEvent& event);
        void OnDownloadAgain(wxCommandEvent& event);
        void OnCheckMD5(wxCommandEvent& event);
        void OnExportConf(wxCommandEvent& event);
        void OnImportConf(wxCommandEvent& event);
        void OnShutdown(wxCommandEvent& event);
        void OnDisconnect(wxCommandEvent& event);
        void OnOptions(wxCommandEvent& event);
        void OnIconize(wxIconizeEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnAbout(wxCommandEvent& WXUNUSED(event));
        void GenerateInProgressList();
        void GenerateFinishedList();
        void OnToolLeftClick(wxCommandEvent& event);
        void OnUpDown(bool up);
        mTaskBarIcon *taskbaricon;
        wxMenuBar *menubar;
        wxToolBar *toolbar;
        wxMenu *menupopup;
        mOptions programoptions;
        wxMutex *mutex_programoptions;
        mGraphPoints graph;
        int timerinterval;
    private:
        wxTimer *mtimer;
        wxImageList *imageslist;
        DECLARE_EVENT_TABLE()
    };

    class mGraph : public wxPanel
    {
    public:
        void OnPaint(wxPaintEvent &event);
        mOptions *programoptions;
        mGraphPoints *graph;
        DECLARE_DYNAMIC_CLASS(mGraph)
    private:
        DECLARE_EVENT_TABLE()
    };

    class mTaskBarIcon: public wxTaskBarIcon
    {
    public:
       mTaskBarIcon(){};
       void OnLButtonClick(wxTaskBarIconEvent&);
       void OnMouseMove(wxTaskBarIconEvent&);
       void OnClose(wxCommandEvent& event);
       void OnHide(wxCommandEvent& event);
       void OnNew(wxCommandEvent& event);
       virtual wxMenu *CreatePopupMenu();
       bool restoring;
    private:
       DECLARE_EVENT_TABLE()
    };

    class mBoxNew: public wxDialog
    {
    public:
        void OnOk(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnButtonDir(wxCommandEvent& event);
    private:
        DECLARE_EVENT_TABLE()
    };

    class mBoxOptions: public wxDialog
    {
    public:
        void OnOk(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnButtonDir(wxCommandEvent& event);
        void OnGraphBackgroundColour(wxCommandEvent& event);
        void OnGraphGridColour(wxCommandEvent& event);
        void OnGraphLineColour(wxCommandEvent& event);
        void OnGraphFontColour(wxCommandEvent& event);
        void OnButtonStartDate(wxCommandEvent& event);
        void OnButtonFinishDate(wxCommandEvent& event);
        void OnAdd(wxCommandEvent& event);
        void OnRemove(wxCommandEvent& event);
    private:
        DECLARE_EVENT_TABLE()
    };

    class wxDatePicker: public wxDialog
    {
    public:
        wxDatePicker::wxDatePicker(wxWindow* parent, wxWindowID id, const wxString& title, wxString date):
                wxDialog(parent, id, title)
        {
            int wdate,hdate,wbtn,hbtn;
            wxDateTime tmpdate;
            tmpdate.ParseDate(date);
            m_datepicker = new wxCalendarCtrl(this,0,tmpdate,wxPoint(0,0),wxDefaultSize);
            m_datepicker->GetClientSize(&wdate,&hdate);
            m_btnok = new wxButton(this,wxID_OK,_("Ok"));
            m_btnok->GetClientSize(&wbtn,&hbtn);
            m_btnok->SetSize(wdate-wbtn,hdate,-1,-1);
            this->SetSize(-1,-1,wdate,hdate+hbtn);
            this->CentreOnParent();
        };
        void wxDatePicker::OnOk(wxCommandEvent& event)
        {
            m_selecteddate = m_datepicker->GetDate().Format(wxT("%Y/%m/%d"));
            EndModal(wxID_OK);
        };
        wxString wxDatePicker::GetSelectedDate()
        {
            return m_selecteddate;
        };
    private:
        wxCalendarCtrl *m_datepicker;
        wxButton *m_btnok;
        wxString m_selecteddate;
        DECLARE_EVENT_TABLE()
    };

    class mBoxOptionsColorPanel: public wxPanel
    {
    public:
        void OnPaint(wxPaintEvent &event);
        wxColor colour;
        DECLARE_DYNAMIC_CLASS(mBoxOptionsColorPanel)
    private:
        DECLARE_EVENT_TABLE()
    };

    class mBoxFind: public wxFindReplaceDialog {
    public:
        mBoxFind(wxWindow * parent, wxFindReplaceData* data, const wxString& title, int style = 0):
        wxFindReplaceDialog(parent, data, title, style){}

        void OnFind(wxFindDialogEvent& event);
        void OnClose(wxFindDialogEvent& event);
    private:
        DECLARE_EVENT_TABLE()
    };

    class mInProgressList : public wxListCtrl
    {
    public:
        void OnRClick(wxListEvent& event);
        void OnSelected(wxListEvent& event);
        void OnDeselected(wxListEvent& event);
        void SelectUnselect(bool selected,int selection,mMainFrame *mainframe);
        int Insert(mDownloadFile *current, int item);
        int GetCurrentSelection();
        void SetCurrentSelection(int selection);
        void RemoveItemListandFile(int item);
         DECLARE_DYNAMIC_CLASS(mInProgressList)
    private:
        DECLARE_EVENT_TABLE()
    };

    class mFinishedList : public wxListCtrl
    {
    public:
        void OnRClick(wxListEvent& event);
        void OnSelected(wxListEvent& event);
        void OnDeselected(wxListEvent& event);
        void SelectUnselect(bool selected,int selection,mMainFrame *mainframe);
        int GetCurrentSelection();
        void SetCurrentSelection(int selection);

        DECLARE_DYNAMIC_CLASS(mFinishedList)
    private:
        DECLARE_EVENT_TABLE()
    };

    class mNotebook : public wxNotebook
    {
    public:
        void OnChangePage(wxNotebookEvent& event);
        DECLARE_DYNAMIC_CLASS(mNotebook)
    private:
        DECLARE_EVENT_TABLE()
    };

    class mUrlName
    {
    public:
       mUrlName();
       mUrlName(const wxString& fullpath);
       void Assign(const wxString& fullpath);
       bool UrlIsValid();
       wxString GetHost();
       int GetPort();
       wxString GetDir();
       wxString GetFullName();
       wxString GetFullRealName();
       wxString GetFullPath();
       int Type();
    private:
       wxString m_url;
       wxString m_host;
       wxString m_dir;
       wxString m_name;
       int m_port;
       int m_type;
    };
    
    class mFTP: public wxFTP
    {
    public:
        wxLongLong GetFileSize(const wxString& fileName);
        wxInputStream *GetInputStream(const wxString& path);
    };

    class mConnection: public wxConnection
    {
    public:
        bool OnExecute(const wxString& topic, wxChar* data, int size, wxIPCFormat format);
    };

    class mClient: public wxClient
    {
    public:
        wxConnectionBase *OnMakeConnection(void) { return new mConnection; }
    };

    class mServer: public wxServer
    {
    public:
        wxConnectionBase *OnAcceptConnection(const wxString& topic);
    };

    class mDownloadThread : public wxThread
    {
    public:
        mDownloadThread(mDownloadFile *file, int index);
        // thread execution starts here
        virtual void *Entry();

        // called when the thread exits - whether it terminates normally or is
        // stopped with Delete() (but not when it is Kill()ed!)
        virtual void OnExit();
        int GetType();
        wxLongLong CurrentSize(wxString filepath,wxString filename);
        wxSocketClient *ConnectHTTP(wxLongLong start);
        wxSocketClient *ConnectFTP(wxLongLong start);
        wxInputStream *ConnectLOCAL_FILE(wxLongLong start);
        int  DownloadPart(wxSocketClient *connection, wxInputStream *filestream, wxLongLong start, wxLongLong end);
                        //the parameter wxInputStream *filestream is used just for LOCAL_FILE
        void PrintMessage(wxString str,wxString color=HTMLNORMAL);
        void WaitUntilAllFinished(bool canstop = TRUE);
        bool JoinFiles();
        void SpeedCalculation(long delta_t);
        mDownloadFile *downloadfile;
        mOptions *programoptions;
        int downloadpartindex;
        wxString currenturl;
        bool redirecting;
        bool restart;
    };

    wxString int2wxstr(long value,int format = 0);
    wxString TimeString(long value);
    wxString TimeString(wxLongLong value);
    wxString GetLine(wxString text, int line);
    char *wxstr2str(wxString wxstr);
    wxString str2wxstr(char *str);
    wxString str2wxstr(char str);
    wxString ByteString(long size);
    wxString ByteString(wxLongLong size);
    int ListCompareByIndex(const mDownloadFile** arg1, const mDownloadFile** arg2);
    wxLongLong wxstrtolonglong(wxString string);
    double wxlonglongtodouble(wxLongLong value);
    int wxCALLBACK CompareDates(long item1, long item2, long WXUNUSED(sortData));
#endif
