## Process this file with automake to produce Makefile.in
## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

## SUBDIRS = src

bin_PROGRAMS = wxdfast

EXTRA_DIST = \
	README\
	README.br\
	COPYING\
	AUTHORS\
	ChangeLog\
	ChangeLog.br\
	INSTALL\
	NEWS\
	TODO

INCLUDES = \
	$(WX_CXXFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

AM_CXXFLAGS =\
	 -Wall\
	 -g

wxdfast_SOURCES = \
	src/BoxFind.cpp\
	src/DownloadThread.cpp\
	src/InProgressList.cpp\
	src/UtilFunctions.cpp\
	src/BoxNew.cpp\
	src/FinishedList.cpp\
	src/MainFrame.cpp\
	src/wxDFast.cpp\
	src/BoxOptions.cpp\
	src/FTP.cpp\
	src/ProgressBar.cpp\
	src/CheckNewRelease.cpp\
	src/Graph.cpp\
	src/TaskBarIcon.cpp\
	src/DownloadFile.cpp\
	src/HTTP.cpp\
	src/UrlName.cpp\
	src/wxMD5/md5.cpp\
	src/wxMD5/wxMD5.cpp

wxdfast_LDFLAGS = \
	$(WX_LIBS)

wxdfast_LDADD =  \
	$(WX_LIBS)
