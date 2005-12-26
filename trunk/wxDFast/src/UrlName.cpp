#include "wxDFast.h"

mUrlName::mUrlName()
{
    m_url = wxEmptyString;
}

mUrlName::mUrlName(const wxString& fullpath)
{
    m_url = fullpath;
    if ((m_url.Mid(0,4).Lower()) == wxT("www."))
        m_url = wxT("http://") + fullpath;
    else if ((m_url.Mid(0,4).Lower()) == wxT("ftp."))
        m_url = wxT("ftp://") + fullpath;
    else if ((m_url.Mid(0,1).Lower()) == wxT("/"))
        m_url = wxT("file://") + fullpath;
}

void mUrlName::Assign(const wxString& fullpath)
{
    m_url = fullpath;
    if ((m_url.Mid(0,4).Lower()) == wxT("www."))
        m_url = wxT("http://") + fullpath;
    else if ((m_url.Mid(0,4).Lower()) == wxT("ftp."))
        m_url = wxT("ftp://") + fullpath;
    else if ((m_url.Mid(0,1).Lower()) == wxT("/"))
        m_url = wxT("file://") + fullpath;
}

bool mUrlName::UrlIsValid()
{
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
    else if ((m_url.Mid(0,8).Lower()) == wxT("file:///"))
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
    int count = 0;
    wxString nome;
    if ((m_url.Mid(0,7).Lower()) == wxT("http://"))
        nome = m_url.Mid(7);
    else if ((m_url.Mid(0,6).Lower()) == wxT("ftp://"))
        nome = m_url.Mid(6);
    else if ((m_url.Mid(0,7).Lower()) == wxT("file://"))
        nome = m_url.Mid(7);

    count = nome.Find(SEPARATOR_URL);
    if (count > 0)
        return(nome.Mid(0,count--));
    else
        return wxEmptyString;
}

int mUrlName::Type()
{
    if ((m_url.Mid(0,7).Lower()) == wxT("http://"))
        return HTTP;
    else if ((m_url.Mid(0,6).Lower()) == wxT("ftp://"))
        return FTP;
    else if ((m_url.Mid(0,7).Lower()) == wxT("file://"))
        return LOCAL_FILE;
    else
        return -1;
}

wxString mUrlName::GetDir()
{ 
    wxString nome;
    if ((m_url.Mid(0,7).Lower()) == wxT("http://"))
        nome = m_url.Mid(7);
    else if ((m_url.Mid(0,6).Lower()) == wxT("ftp://"))
        nome = m_url.Mid(6);
    else if ((m_url.Mid(0,7).Lower()) == wxT("file://"))
        nome = m_url.Mid(7);
    else
        return(m_url);
    wxString tmp01 = this->GetHost();
    wxString tmp02 = GetFullName();
    nome = nome.Mid(tmp01.Length());
    nome = nome.Mid(0,nome.Length()-tmp02.Length());
    return(nome);
}

wxString mUrlName::GetFullName()
{
    unsigned int pos, count;
    pos = count = 0;
    wxString nome = m_url;
    while (count < nome.Length())
    {
        count++;
        if (nome.Mid(count,1) == SEPARATOR_URL)
           pos = count;
    }
    nome = nome.Mid(pos+1);
    return(nome);
}

wxString mUrlName::GetFullPath()
{
    return m_url;
}


