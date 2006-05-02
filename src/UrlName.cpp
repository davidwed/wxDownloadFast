#include "wxDFast.h"

mUrlName::mUrlName() : wxURI()
{
}

mUrlName::~mUrlName()
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

bool mUrlName::UrlIsValid()
{
    return (HasServer() && HasScheme() && HasPath());

/*    wxString m_url = wxURI::BuildURI();
    if (m_url.Length() <=7 )
        return FALSE;
    if (((m_url.Mid(0,7).Lower()) == wxT("http://")) || ((m_url.Mid(0,6).Lower()) == wxT("ftp://")))
    {
        int i;
        for (i=6; i < (int)(m_url.Length() - 1); i++)
        {
            if (m_url.Mid(i,2) == wxT("//")    )
                return FALSE;
            if (m_url.Mid(i,1) == wxT("\\")    )
                return FALSE;
        }
        if (m_url.Mid(i,1) == wxT("\\")    )
            return FALSE;
        return TRUE;
    }
    else if (((m_url.Mid(0,7).Lower()) == wxT("file://")) && 
            (((m_url.Mid(7,1).Lower()) == wxT("/")) || ((m_url.Mid(8,2).Lower()) == wxT(":/"))))
    {
        int i;
        for (i=7; i < (int)(m_url.Length() - 1); i++)
        {
            if (m_url.Mid(i,2) == wxT("//")    )
                return FALSE;
            if (m_url.Mid(i,1) == wxT("\\")    )
                return FALSE;
        }
        if (m_url.Mid(i,1) == wxT("\\")    )
            return FALSE;
        return TRUE;
    }
    else
        return FALSE;*/
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
    return wxURI::GetPath().AfterLast('/');
}

wxString mUrlName::GetFullPath()
{
    return wxURI::BuildURI();
}


