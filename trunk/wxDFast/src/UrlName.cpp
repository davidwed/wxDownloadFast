//
// C++ Implementation: UrlName
//
// Description: Implements a class with handle url's and permits to return
//              filenames, paths, host and verify if this is complete or not.
//
//
// Author: Max Magalhães Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

mUrlName::mUrlName() : wxURI()
{
}

mUrlName::mUrlName(wxString uri)
{
    wxString m_uri = uri;
    //COMPLETE THE ADDRESS
    if ((m_uri.Mid(0,4).Lower()) == wxT("www."))
        m_uri = wxT("http://") + m_uri;
    else if ((m_uri.Mid(0,4).Lower()) == wxT("ftp."))
        m_uri = wxT("ftp://") + m_uri;
    else if ((m_uri.Mid(0,1).Lower()) == wxT("/"))
        m_uri = wxT("file://") + m_uri;
    else if ((m_uri.Mid(1,1).Lower()) == wxT(":"))
        m_uri = wxT("file://") + m_uri;
    wxURI::Create(m_uri);
}

bool mUrlName::IsComplete()
{
    return (HasServer() && HasScheme() && HasPath());
}

wxString mUrlName::GetHost()
{ 
    return wxURI::GetServer();
}

wxString mUrlName::GetPort()
{
    wxString port = wxURI::GetPort();
    int type = Type();
    if (!HasPort())
    {
        if (type == HTTP)
            port = wxT("80");
        else if (type == FTP)
            port = wxT("21");
        else
            port = wxT("0");
    }
    return port;
}

int mUrlName::Type()
{
    wxString scheme = GetScheme();
    if (scheme == wxT("http"))
        return HTTP;
    else if (scheme == wxT("ftp"))
        return FTP;
    else if (scheme == wxT("file"))
        return LOCAL_FILE;
    else
        return -1;
}

wxString mUrlName::GetDir()
{ 
    return wxURI::GetPath().BeforeLast('/') + wxT("/");
}

wxString mUrlName::GetFullName()
{
    return wxURI::BuildUnescapedURI().AfterLast('/').AfterLast('=');;
}

wxString mUrlName::GetFullRealName()
{
    if (wxURI::HasQuery())
        return wxURI::GetPath().AfterLast('/') + wxT("?") + wxURI::GetQuery();
    else
        return wxURI::GetPath().AfterLast('/');
}

wxString mUrlName::GetFullPath()
{
    return wxURI::BuildURI();
}


