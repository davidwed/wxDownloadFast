## Process this file with automake to produce Makefile.in

bin_PROGRAMS = wxdfast

INCLUDES = \
	$(WX_CXXFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

AM_CXXFLAGS =\
	 -Wall\
	 -g

wxdfast_SOURCES = \
	BoxFind.cpp\
	DownloadThread.cpp\
	InProgressList.cpp\
	UtilFunctions.cpp\
	BoxNew.cpp\
	FinishedList.cpp\
	MainFrame.cpp\
	wxDFast.cpp\
	BoxOptions.cpp\
	FTP.cpp\
	ProgressBar.cpp\
	CheckNewRelease.cpp\
	Graph.cpp\
	TaskBarIcon.cpp\
	DownloadFile.cpp\
	HTTP.cpp\
	UrlName.cpp\
	wxMD5/md5.cpp\
	wxMD5/wxMD5.cpp\
	wxDFast.rc

## Include the Resources.cpp file
## which is created using the XRC files
## See the Makefile.am inside resources/
wxdfast_SOURCES += ../resources/Resources.cpp

wxdfast_LDFLAGS = \
	$(WX_LIBS)

wxdfast_LDADD =  \
	$(WX_LIBS)

