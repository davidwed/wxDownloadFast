//
// C++ Implementation: UtilFunctions
//
// Description: This file contain the implementation of some usefull conversion
//              functions.
//
//
// Author: Max Magalh�s Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

wxString MyUtilFunctions::int2wxstr(long value,int format)
{
    wxString temp,string;
    string << wxT("%0") << format << wxT("ld");
    temp = temp.Format(string,value);
    return temp;
}

wxString MyUtilFunctions::TimeString(long value)
{
    wxString tmp;
    int hour,min,sec;
    long time;
    time = (long) (value / 1000);
    hour = (int) (time/3600);
    time = time - (hour * 3600);
    min = (int) (time/60);
    sec = (time%60);
    if (hour == 0)
       tmp.Printf(wxT("%01dm %01ds"), min, sec);
    else
       tmp.Printf(wxT("%01dh %01dm %01ds"),hour, min, sec);
    return(tmp);
}

wxString MyUtilFunctions::TimeString(wxLongLong value)
{
    wxString tmp;
    int hour,min,sec;
    long time;
    time = (value / 1000).ToLong();
    hour = (int) (time/3600);
    time = time - (hour * 3600);
    min = (int) (time/60);
    sec = (time%60);
    if (hour == 0)
       tmp.Printf(wxT("%01dm %01ds"), min, sec);
    else
       tmp.Printf(wxT("%01dh %01dm %01ds"),hour, min, sec);
    return(tmp);
}

wxString MyUtilFunctions::GetLine(wxString text, int line)
{
    wxString str = text;
    int pos = str.Find(wxT("\n"));
    int i;
    if (pos == 0)
    {
        str=str.Mid(1);
        pos = str.Find(wxT("\n"));
    }
    for (i=1; i<line;i++)
    {
        str = str.Mid(pos+1);
        pos = str.Find(wxT("\n"));
    }
    return str.Mid(0,pos-1);
}

wxString MyUtilFunctions::str2wxstr(char *str)
{
    wxString retorno;
    unsigned int i;
    for (i = 0 ; i < strlen(str); i++)
        retorno.Append(str[i],1);
    return retorno;
}

wxString MyUtilFunctions::str2wxstr(char str)
{
    wxString retorno;
    retorno.Append(str,1);
    return retorno;
}

wxString MyUtilFunctions::ByteString(long size)
{
    wxString str;
    if ( size < 1024)
//        str.Printf(wxT("%0.1f Bytes"),(double) size);
        str.Printf(wxT("0.0 KB"));
    if (( size >= 1024) && (size < 1048576))
        str.Printf(wxT("%0.1f KB"),((double) size) /((double)1024));
    if ( size >= 1048576)
        str.Printf(wxT("%0.1f MB"),((double) size)/((double)1048576));
    return(str);
}

wxString MyUtilFunctions::ByteString(wxLongLong size)
{
    wxString str;
    if ( size < 1024)
//        str.Printf(wxT("%0.1f Bytes"),(double) size);
        str.Printf(wxT("0.0 KB"));
    if (( size >= 1024) && (size < 1048576))
        str.Printf(wxT("%0.1f KB"),( wxlonglongtodouble(size)) /((double)1024));
    if ( size >= 1048576)
        str.Printf(wxT("%0.1f MB"),( wxlonglongtodouble(size))/((double)1048576));
    return(str);
}

wxLongLong MyUtilFunctions::wxstrtolonglong(wxString string)
{
    wxString tmp = string.Trim().Trim(FALSE);
    char carac;
    wxLongLong result = 0;
    int sign=1;
    if (tmp.GetChar(0) == '-')
        sign = -1;
    for (unsigned int i = 0; i < tmp.Length(); i++)
    {
        carac = tmp.GetChar(i);
        if ((carac >= '0') && (carac <= '9'))
            result = result * 10LL + carac-'0';
        else
            continue;
    }
    return result*sign;
}

double MyUtilFunctions::wxlonglongtodouble(wxLongLong value)
{
    double d = value.GetHi();
    d *= 1.0 + (double)ULONG_MAX;
    d += value.GetLo();
    return d;
}

#ifdef __WXMSW__
wxString MyUtilFunctions::GetProgramFilesDir()
{
    wxString result = wxEmptyString;
    wxRegKey regKey;
    wxString idName(wxT("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion"));

    regKey.SetName(idName);
    regKey.QueryValue(wxT("ProgramFilesDir"),result);
    regKey.Close();
	
    return result;
}

wxString MyUtilFunctions::GetMyDocumentsDir()
{
    wxString result = wxEmptyString;
    wxRegKey regKey;
    wxString idName(wxT("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"));

    regKey.SetName(idName);
    regKey.QueryValue(wxT("Personal"),result);
    regKey.Close();
	
    return result;
}

#endif