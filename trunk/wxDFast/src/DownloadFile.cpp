//
// C++ Implementation: DownloadFile
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

int ListCompareByIndex(const mDownloadFile** arg1, const mDownloadFile** arg2)
{
    if ((*arg1)->index > (*arg2)->index)
       return 1;
    else
       return -1;
}

int wxCALLBACK CompareDates(long item1, long item2, long WXUNUSED(sortData))
{
    if (item1 < item2)
        return 1;
    if (item1 > item2)
        return -1;

    return 0;
}

int mApplication::CreateDownloadRegister(mUrlName url,wxFileName destination, int parts, wxString user, wxString password, wxString comments,int scheduled)
{
    mDownloadFile *file = new mDownloadFile();
    file->index =  downloadlist.GetCount();
    file->scheduled = scheduled;
    file->status = STATUS_STOPED;
    file->restart = -1;
    file->parts = parts;
    file->name = url.GetFullName();
    file->destination = destination.GetFullPath();
    file->totalsize = 0;
    file->totalsizecompleted = 0;
    file->comments = comments;
    file->percentual = 0;
    file->timepassed = 0;
    file->timeremaining = 0;
    file->totalspeed = 0;
    file->urllist = url.GetFullPath();
    file->contenttype = wxEmptyString;
    file->MD5 = wxEmptyString;
    file->start = wxDateTime::Now();
    file->end = wxDateTime::Now();
    file->currentattempt = 0;
    for (int i =0;i<MAX_NUM_PARTS;i++)
    {
        file->messages[i].Clear();
        file->delta_size[i] = 0;
        file->sizecompleted[i] = 0;
        file->startpoint[i] = 0;
        file->size[i] = 0;
        file->finished[i] = FALSE;
    }
    if (user == wxEmptyString)
    {
        file->user = ANONYMOUS_USER;
        file->password = ANONYMOUS_PASS;
    }
    else
    {
        file->user = user;
        file->password = password;
    }
    file->free = TRUE;
    file->criticalerror = FALSE;
    file->split = WAIT;

    downloadlist.Append(file);
    RegisterListItemOnDisk(file);
    return (file->index);
}

void mApplication::RemoveListItemFromDisk(mDownloadFile *file)
{
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(FILES_REG);
    if (file->status == STATUS_FINISHED)
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
        config->DeleteGroup(file->name);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(FINISHED_REG);

    }
    else
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
    }
    config->DeleteGroup(file->name);
    delete config;
}

void mApplication::RegisterListItemOnDisk(mDownloadFile *file)
{
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(FILES_REG);
    if (file->status == STATUS_FINISHED)
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
        config->DeleteGroup(file->name);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(FINISHED_REG);

    }
    else
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
    }
    config->SetPath(file->name);
    config->Write(NAME_REG,file->name);
    config->Write(INDEX_REG,file->index);
    config->Write(STATUS_REG,file->status);
    config->Write(SCHEDULED_REG,file->scheduled);
    config->Write(RESTART_REG,file->restart);
    config->Write(PARTS_REG,file->parts);
    config->Write(DESTINATION_REG,file->destination);
    config->Write(SIZE_REG,file->totalsize.ToString());
    config->Write(SIZECOMPLETED_REG,file->totalsizecompleted.ToString());
    config->Write(TIMEPASSED_REG,file->timepassed.ToString());
    config->Write(SPEED_REG,file->totalspeed);
    config->Write(PERCENTUAL_REG,file->percentual);
    config->Write(MD5_REG,file->MD5);
    config->Write(START_REG,file->start.GetTicks());
    config->Write(END_REG,file->end.GetTicks());
    config->Write(COMMENTS_REG,file->comments);
    config->Write(CONTENTTYPE_REG,file->contenttype);
    config->Write(URL1_REG,file->urllist);

    config->Write(USER_REG,file->user);
    config->Write(PASSWORD_REG,file->password);

    delete config;
}

void mApplication::RecreateIndex()
{
    int i=0;
    for ( mDownloadList::Node *node = downloadlist.GetFirst(); node; node = node->GetNext() )
    {
        mDownloadFile *current = node->GetData();
        if (current->index != i)
        {
            current->index = i;
            RegisterListItemOnDisk(current);
        }
        i++;
    }
}

void mApplication::LoadDownloadListFromDisk()
{
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    wxString name;
    wxString tmp;
    long index;
    config->SetPath(INPROGRESS_REG);
    if (config->GetFirstGroup(name, index))
    {

        do
        {
            mDownloadFile *file = new mDownloadFile();
            file->name = name;
            downloadlist.Append(file);
        }
        while(config->GetNextGroup(name, index));
    }
    for ( mDownloadList::Node *node = downloadlist.GetFirst(); node; node = node->GetNext() )
    {
        mDownloadFile *file = node->GetData();
        tmp = wxEmptyString;
        config->SetPath(file->name);
        config->Read(INDEX_REG,&(file->index));
        config->Read(STATUS_REG,&(file->status));
        config->Read(SCHEDULED_REG,&(file->scheduled));
        config->Read(RESTART_REG,&(file->restart));
        config->Read(PARTS_REG,&(file->parts));
        config->Read(DESTINATION_REG,&(file->destination));

        config->Read(SIZE_REG,&tmp);
        file->totalsize = wxstrtolonglong(tmp);

        config->Read(SIZECOMPLETED_REG,&tmp);
        file->totalsizecompleted = wxstrtolonglong(tmp);

        config->Read(TIMEPASSED_REG,&tmp);
        file->timepassed = wxstrtolonglong(tmp);

        config->Read(SPEED_REG,&(file->totalspeed));
        config->Read(PERCENTUAL_REG,&(file->percentual));
        config->Read(MD5_REG,&(file->MD5));
        config->Read(COMMENTS_REG,&(file->comments));
        config->Read(CONTENTTYPE_REG,&(file->contenttype));
        {
            time_t value = 0;
            config->Read(START_REG,&(value));
            file->start.Set(value);
            value = 0;
            config->Read(END_REG,&(value));
            file->end.Set(value);
        }
        config->Read(URL1_REG,&(file->urllist));
        if (file->status == STATUS_ACTIVE)
            file->status = STATUS_QUEUE;
        if (file->scheduled)
            file->status = STATUS_SCHEDULE;
        if (file->percentual > 100)
            file->percentual = 0;
        file->timeremaining = 0;
        file->totalspeed = 0;
        file->currentattempt = 0;
        file->free = TRUE;
        file->criticalerror = FALSE;
        file->split = WAIT;
        if ((file->parts < 1) || (file->parts > MAX_NUM_PARTS))
            file->parts = 1;

        for (int i =0;i<MAX_NUM_PARTS;i++)
        {

            file->messages[i].Clear();
            file->delta_size[i] = 0;
            file->sizecompleted[i] = 0;
            file->startpoint[i] = 0;
            file->size[i] = 0;
            file->finished[i] = FALSE;
        }

        config->Read(USER_REG,&(file->user));
        config->Read(PASSWORD_REG,&(file->password));

        config->SetPath(BACK_DIR_REG);
    }
    downloadlist.Sort(ListCompareByIndex);
    delete config;
}

void mApplication::ChangeName(mDownloadFile *currentfile, wxString name, int value)
{
    wxString strname = name;
    if (currentfile->name == strname)
        return ;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(INPROGRESS_REG);
    config->DeleteGroup(currentfile->name);
    delete config;
    if (value > 0)
        strname = int2wxstr(value+1) + wxT("-") + strname;
    if (FindDownloadFile(strname))
    {
        ChangeName(currentfile,name,value+1);
    }
    else
    {
        currentfile->name = strname;
        RegisterListItemOnDisk(currentfile);
    }
}

mDownloadFile *mApplication::FindDownloadFile(wxString str)
{
    for ( mDownloadList::Node *node = wxGetApp().downloadlist.GetFirst(); node; node = node->GetNext() )
        if (node->GetData()->name.Lower() == str.Lower())
            return (node->GetData());
    return (NULL);
}

wxString int2wxstr(long value,int format)
{
    wxString temp,string;
    string << wxT("%0") << format << wxT("ld");
    temp = temp.Format(string,value);
    return temp;
}

wxString TimeString(long value)
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

wxString TimeString(wxLongLong value)
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

wxString GetLine(wxString text, int line)
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

char *wxstr2str(wxString wxstr)
{
    int tamanho = wxstr.Length() + 1;
    char *data = new char[tamanho];
    int i;
    for (i = 0 ; i < (tamanho-1); i++)
        data[i] = wxstr.GetChar(i);
    data[i] = '\0';
    return data;
}

wxString str2wxstr(char *str)
{
    wxString retorno;
    unsigned int i;
    for (i = 0 ; i < strlen(str); i++)
        retorno.Append(str[i],1);
    return retorno;
}

wxString str2wxstr(char str)
{
    wxString retorno;
    retorno.Append(str,1);
    return retorno;
}

wxString ByteString(long size)
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

wxString ByteString(wxLongLong size)
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

wxLongLong wxstrtolonglong(wxString string)
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

double wxlonglongtodouble(wxLongLong value)
{
    double d = value.GetHi();
    d *= 1.0 + (double)ULONG_MAX;
    d += value.GetLo();
    return d;
}
