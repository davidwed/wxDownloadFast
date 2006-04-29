#include "wxDFast.h"

mUrlName::mUrlName()
{
    m_url = wxEmptyString;
}

mUrlName::mUrlName(const wxString& fullpath)
{
    this->Assign(fullpath);
}

void mUrlName::Assign(const wxString& fullpath)
{
    bool hasport = FALSE;
    int position = 0;
    wxString name;
    m_url = fullpath;
    m_host = wxEmptyString;
    m_port = 0;
    m_dir = wxEmptyString;
    m_name = wxEmptyString;
    //COMPLETE THE ADDRESS
    if ((m_url.Mid(0,4).Lower()) == wxT("www."))
        m_url = wxT("http://") + fullpath;
    else if ((m_url.Mid(0,4).Lower()) == wxT("ftp."))
        m_url = wxT("ftp://") + fullpath;
    else if ((m_url.Mid(0,1).Lower()) == wxT("/"))
        m_url = wxT("file://") + fullpath;
    else if ((m_url.Mid(1,1).Lower()) == wxT(":"))
        m_url = wxT("file://") + fullpath;

    //VERIFY TYPE
    if ((m_url.Mid(0,7).Lower()) == wxT("http://"))
    {
        m_type = HTTP;
        m_port = 80;
    }
    else if ((m_url.Mid(0,6).Lower()) == wxT("ftp://"))
    {
        m_type = FTP;
        m_port = 21;
    }
    else if ((m_url.Mid(0,7).Lower()) == wxT("file://"))
        m_type = LOCAL_FILE;
    else
        m_type = -1;

    if ((m_type == HTTP) || (m_type == FTP))
    {
        if ((m_url.Mid(0,7).Lower()) == wxT("http://"))
            name = m_url.Mid(7);
        else if ((m_url.Mid(0,6).Lower()) == wxT("ftp://"))
            name = m_url.Mid(6);
        else if ((m_url.Mid(0,7).Lower()) == wxT("file://"))
            name = m_url.Mid(7);

        position = name.Find(SEPARATOR_URL);
        if (position > 0)
            m_host = name.Mid(0,position);

        position = m_host.Find(wxT(":"));
        if (position > 0)
        {
            m_host.Mid(position+1).ToLong((long *)&m_port);
            hasport = TRUE;
        }
        m_host = m_host.Mid(0,position);
    }

    //GET THE FILE NAME
    if ((m_url.Mid(0,7).Lower()) == wxT("http://"))
        name = m_url.Mid(7);
    else if ((m_url.Mid(0,6).Lower()) == wxT("ftp://"))
        name = m_url.Mid(6);
    else if ((m_url.Mid(0,7).Lower()) == wxT("file://"))
        name = m_url.Mid(7);
    else
        name = m_url;
    position = name.Find('/',TRUE);
    m_name = name.Mid(position+1);

    //GET THE DIRECTORY
    if ((m_type == HTTP) || (m_type == FTP))
    {
        if (hasport)
            name = name.Mid(m_host.Length() + int2wxstr(m_port).Length() + 1);
        else
            name = name.Mid(m_host.Length());
    }
    m_dir = name.Mid(0,name.Length()-m_name.Length());
}

bool mUrlName::UrlIsValid()
{
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
        return FALSE;
}

wxString mUrlName::GetHost()
{ 
    return m_host;
}

int mUrlName::GetPort()
{ 
    return m_port;
}

int mUrlName::Type()
{
    return m_type;
}

wxString mUrlName::GetDir()
{ 
    return m_dir;
}

wxString mUrlName::GetFullName()
{
    wxString tmpstr = wxEmptyString,str;
    //SEARCH FOR THE %20 CARACTER AND CHANGE FOR SPACE
    wxStringTokenizer tkz(m_name, wxT("0"));
    while ( tkz.HasMoreTokens() )
    {
        str = tkz.GetNextToken();
        if  (str.Mid(str.Length()-2,2) == wxT("%2"))
        {
            str = str.Mid(0,str.Length()-2);
            tmpstr += str + wxT(" ");
        }
        else
        {
            tmpstr += str + wxT("0");
        }
    }
    tmpstr.RemoveLast();

    //GET THE PART OF THE FILENAME WHICH REALLY MATTERS
    tmpstr = tmpstr.AfterLast('=');

    return tmpstr;
}

wxString mUrlName::GetFullRealName()
{
    return m_name;
}

wxString mUrlName::GetFullPath()
{
    return m_url;
}


