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

    #ifdef RESOURCES_CPP
    extern void InitXmlResource(); 
    #endif
    
    const wxCmdLineEntryDesc cmdlinedesc[] =
    {
        { wxCMD_LINE_SWITCH, wxT("hide"), wxT("hide"), wxT("Start with the the main frame hide.")},
        { wxCMD_LINE_SWITCH, wxT("notray"), wxT("notray"),  wxT("Don't show the icon on system tray")},
        { wxCMD_LINE_PARAM, NULL, NULL,  wxT("URL of the file to be downloaded"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL},
        { wxCMD_LINE_NONE }
    };
    
    #ifdef __WXMSW__
        #define wxXPM(x)  (wxBitmap((const char **)x))
    #else
        #define wxXPM(x)  (wxBitmap(x))
    #endif

    const wxString PROGRAM_NAME = wxT("wxDownload Fast");
    const wxString VERSION = wxT("0.2.1");
    const wxString SEPARATOR_URL = wxT("/");
    #ifdef __WXMSW__
       const wxString SEPARATOR_DIR = wxT("\\");
    #else
       const wxString SEPARATOR_DIR = wxT("/");
	#endif
	#define READ_BUFFER			1024
	#define MAX_NUM_PARTS 		9
	#define MIN_SIZE_TO_SPLIT	500000l
	#define USE_HTML_MESSAGES	1 //CHANGE THIS AND THE FILE MAIN_WINDOW.XRC
	
    #define IPC_SERVICE wxT("24242")
    #define IPC_TOPIC wxT("IPC_WXDOWNLAD_FAST_")

    const wxString DFAST_REG = wxT("wxDownloadFast");
    const wxString FILES_REG = wxT("files");
    const wxString INPROGRESS_REG = wxT("files/inprogress");
    const wxString FINISHED_REG = wxT("files/finished");
    const wxString BACK_DIR_REG = wxT("..");
    const wxString NAME_REG = wxT("name");
    const wxString INDEX_REG = wxT("index");
    const wxString STATUS_REG = wxT("status");
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
    
    const wxString HTMLERROR = wxT("#FF0030"); 		//RED
    const wxString HTMLSERVER = wxT("#009865");		//GREEN
    const wxString HTMLNORMAL = wxT("#002f5e");		//BLUE
    const wxString HTMLBLACK = wxT("#000000");		//BLACK
    
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

    #define HTTP              	1
    #define FTP               	2 
    #define READ				3
    #define WRITE				4
    
    #define HIDE				1000
    #define NEW					1001
    #define CLOSE				1002
    #define TIMER_ID			1003
    
    //mDownloadFile STATUS
    #define STATUS_STOPED 		0
    #define STATUS_ACTIVE		1
    #define STATUS_FINISHED 	2
    #define STATUS_ERROR		3
    #define STATUS_QUEUE		4

    //mDownloadFile RESTART, SPLIT
    #define WAIT		-1
    #define YES			0
    #define NO			1
    
    class mDownloadFile
    {
    public:
    	bool free;
        int index;
        int status;
        int restart;
        wxString name;
        wxString destination;
        wxString comments;
        wxString user;
        wxString password;
        int percentual;
        long timepassed;
        long timeremaining;
        wxString MD5;
        wxDateTime start;
        wxDateTime end;
		wxString urllist;
        int parts;
        int currentattempt;
        long totalsize;
        long totalsizecompleted;
        int totalspeed;
        bool criticalerror;
        int split;

		bool speedpoint;
        bool finished[MAX_NUM_PARTS];
        long startpoint[MAX_NUM_PARTS];
        long size[MAX_NUM_PARTS];
        long sizecompleted[MAX_NUM_PARTS];        
        long delta_size[MAX_NUM_PARTS];
        wxString messages[MAX_NUM_PARTS];
    };  
    
    class mOptions
	{
	public:
		int attempts;		     //number of attempts
		int closedialog;	     //show the close dialog
		int simultaneous;	     //number of simultaneous downloads
		int attemptstime;	     //time between the attempts in seconds
		int shutdown;
		int disconnect;
		int alwaysshutdown;
		int alwaysdisconnect;
		int timerupdateinterval; //time between the timer refreshs in milliseconds
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
    	int CreateDownloadRegister(mUrlName url,wxFileName destination, int parts, wxString user, wxString password, wxString comments);
        void RegisterListItemOnDisk(mDownloadFile *file);
        void RecreateIndex();
		void LoadDownloadListFromDisk();
		void RemoveListItemFromDisk(mDownloadFile *file);
		mDownloadFile *FindDownloadFile(wxString str);
		void ChangeName(mDownloadFile *currentfile, wxString name, int value = 0);
		wxString Configurations(int operation, wxString option, wxString value);
		int Configurations(int operation, wxString option, int value);
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
        bool NewDownload(wxString url, wxString destination,int parts,wxString user,wxString password,wxString comments,bool now, bool show);
        bool StartDownload(mDownloadFile *downloadfile);
        void OnNew(wxCommandEvent& event);
        void OnRemove(wxCommandEvent& event);
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
       void OnLButtonDClick(wxTaskBarIconEvent&);
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
	private:
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
       wxString GetDir();
       wxString GetFullName();
       wxString GetFullPath();
       wxString SetFullPath(wxString url);
       int Type();
    private:
       wxString m_url;

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
        long CurrentSize(wxString filepath,wxString filename);
		wxSocketClient *ConnectHTTP(long start);
		wxSocketClient *ConnectFTP(long start);
		int  DownloadPart(wxSocketClient *connection,long start,long end);
		void PrintMessage(wxString str,wxString color=HTMLNORMAL);
		void WaitUntilAllFinished(bool canstop = TRUE);
		bool JoinFiles();
		void SpeedCalculation(long delta_t);
		mDownloadFile *downloadfile;
		int downloadpartindex;
		wxString currenturl;
        bool redirecting;
        bool restart;
    };

	wxString int2wxstr(long value);
	wxString TimeString(long value);
	wxString GetLine(wxString text, int line);
    char *wxstr2str(wxString wxstr);
    wxString str2wxstr(char *str);
    wxString str2wxstr(char str);
    wxString ByteString(long size);
	int ListCompareByIndex(const mDownloadFile** arg1, const mDownloadFile** arg2);
	int wxCALLBACK CompareDates(long item1, long item2, long WXUNUSED(sortData));
#endif
