//
// C++ Implementation: DownloadFile
//
// Description: Implements the classes DownloadList and DownloadFile. 
//              It's this classes which keep all the downloads informations,
//              like URL, size name, etc
//
//
// Author: Max Magalhães Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

void mDownloadList::RecreateIndex()
{
    int i=0;
    for ( mDownloadList::Node *node = this->GetFirst(); node; node = node->GetNext() )
    {
        mDownloadFile *current = node->GetData();
        if (current->index != i)
        {
            current->index = i;
            current->RegisterListItemOnDisk();
        }
        i++;
    }
}

void mDownloadList::ChangePosition(mDownloadFile *file01, mDownloadFile *file02)
{
    int index01,index02;
    index01 = file01->index;
    index02 = file02->index;
    file01->index = index02;
    file02->index = index01;

    Sort(mDownloadList::ListCompareByIndex);
}

void mDownloadList::ChangeName(mDownloadFile *file, wxString name, int value)
{
    wxString strname = name;
    if (file->name == strname)
        return ;
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(INPROGRESS_REG);
    config->DeleteGroup(file->name);
    delete config;
    if (value > 0)
        strname = MyUtilFunctions::int2wxstr(value+1) + wxT("-") + strname;
    if (this->FindDownloadFile(strname))
    {
        this->ChangeName(file,name,value+1);
    }
    else
    {
        file->name = strname;
        file->MarkWriteAsPending(TRUE);
    }
}

void mDownloadList::ChangeDownload(mDownloadFile *file, mUrlList *urllist,wxFileName destination, wxString user, wxString password, wxString comments)
{
    if (file->urllist)
        delete file->urllist;
    file->urllist->DeleteContents(TRUE);
    file->urllist = urllist;
    file->destination = destination.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
    file->user = user;
    file->password = password;
    file->comments = comments;
    file->MarkWriteAsPending(TRUE);
}

void mDownloadList::LoadDownloadListFromDisk()
{
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    wxString name;
    wxString tmp;
    int status;
    long index;
    config->SetPath(INPROGRESS_REG);
    if (config->GetFirstGroup(name, index))
    {

        do
        {
            mDownloadFile *file = new mDownloadFile();
            file->name = name;
            this->Append(file);
        }
        while(config->GetNextGroup(name, index));
    }
    for ( mDownloadList::Node *node = this->GetFirst(); node; node = node->GetNext() )
    {
        mDownloadFile *file = node->GetData();
        tmp = wxEmptyString;
        config->SetPath(file->GetName());
        config->Read(INDEX_REG,&(file->index));

        config->Read(STATUS_REG,&(status));
        config->Read(SCHEDULED_REG,&(file->scheduled));
        if (status == STATUS_STOPED)
            file->SetAsStoped();
        else if ((status == STATUS_ACTIVE) || (status == STATUS_QUEUE))
            file->PutOnQueue();
        else if (status == STATUS_FINISHED)
            file->SetAsFinished();
        else if (status == STATUS_ERROR)
            file->ErrorOccurred();
        else if ((status == STATUS_SCHEDULE_QUEUE) || (file->scheduled))
            file->PutOnScheduleQueue();
        else
            file->SetAsStoped();

        config->Read(RESTART_REG,&(file->restart));
        config->Read(PARTS_REG,&(file->parts));
        config->Read(DESTINATION_REG,&(file->destination));

        config->Read(SIZE_REG,&tmp);
        file->totalsize = MyUtilFunctions::wxstrtolonglong(tmp);

        config->Read(SIZECOMPLETED_REG,&tmp);
        file->totalsizecompleted = MyUtilFunctions::wxstrtolonglong(tmp);

        config->Read(TIMEPASSED_REG,&tmp);
        file->timepassed = MyUtilFunctions::wxstrtolonglong(tmp);

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

        file->urllist = new mUrlList();
        file->urllist->DeleteContents(TRUE);
        file->currenturl = 0;
        bool existurl = TRUE;
        int count = 1;
        wxString str;
        while (existurl)
        {
            str = wxEmptyString;
            config->Read(URL_REG + MyUtilFunctions::int2wxstr(count),&(str));
            if (str.IsEmpty())
                break;
            else
            {
                mUrlName *urltmp = new mUrlName(str);
                if (urltmp->IsComplete())
                    file->urllist->Append(urltmp);
            }
            count++;
        }

        if (file->percentual > 100)
            file->percentual = 0;
        file->timeremaining = 0;
        file->totalspeed = 0;
        file->currentattempt = 0;
        file->free = TRUE;
        file->criticalerror = FALSE;
        file->split = FALSE;
        file->waitbeforesplit = TRUE;
        file->MarkWriteAsPending(FALSE);
        file->MarkRemoveAsPending(FALSE);
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
    this->Sort(mDownloadList::ListCompareByIndex);
    delete config;
}

void mDownloadList::RemoveDownloadRegister(mDownloadFile *currentfile)
{
    unsigned int item = currentfile->GetIndex();
    if ((item + 1) < this->GetCount())
    {
        int count = item;
        for ( mDownloadList::Node *node = this->Item(item+1); node; node = node->GetNext() )
        {
            node->GetData()->index = count;
            node->GetData()->MarkWriteAsPending(TRUE);
            count++;
        }
    }
    mDownloadList::Node *node = this->Find(currentfile);
    this->DeleteNode(node);
}

mDownloadFile *mDownloadList::FindDownloadFile(wxString str)
{
    for ( mDownloadList::Node *node = this->GetFirst(); node; node = node->GetNext() )
        if (node->GetData()->GetName().Lower() == str.Lower())
            return (node->GetData());
    return (NULL);
}

int mDownloadList::ListCompareByIndex(const mDownloadFile** arg1, const mDownloadFile** arg2)
{
    if ((*arg1)->index > (*arg2)->index)
       return 1;
    else
       return -1;
}

mDownloadFile *mDownloadList::NewDownloadRegister(mUrlList *urllist,wxFileName destination, int parts, wxString user, wxString password, wxString comments,int scheduled)
{
    mDownloadFile *file = new mDownloadFile();
    file->index =  this->GetCount();
    file->scheduled = scheduled;
    file->status = STATUS_STOPED;
    file->restart = -1;
    file->parts = parts;
    file->urllist = urllist;
    file->urllist->DeleteContents(TRUE);
    file->currenturl = 0;
    file->name = file->GetFirstUrl().GetFullName();
    file->destination = destination.GetFullPath();
    file->totalsize = 0;
    file->totalsizecompleted = 0;
    file->comments = comments;
    file->percentual = 0;
    file->timepassed = 0;
    file->timeremaining = 0;
    file->totalspeed = 0;
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
    if (user.IsEmpty())
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
    file->split = FALSE;
    file->waitbeforesplit = TRUE;

    file->MarkWriteAsPending(TRUE);
    file->MarkRemoveAsPending(FALSE);
    this->Append(file);
    return file;
}

void mDownloadFile::RemoveListItemFromDisk()
{
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(FILES_REG);
    if (this->status == STATUS_FINISHED)
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
        config->DeleteGroup(this->name);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(FINISHED_REG);

    }
    else
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
    }
    config->DeleteGroup(this->name);
    delete config;
    this->MarkRemoveAsPending(FALSE);
}

void mDownloadFile::RegisterListItemOnDisk()
{
    wxFileConfig *config = new wxFileConfig(DFAST_REG);
    config->SetPath(FILES_REG);
    if (this->status == STATUS_FINISHED)
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
        config->DeleteGroup(this->name);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(BACK_DIR_REG);
        config->SetPath(FINISHED_REG);

    }
    else
    {
        config->SetPath(BACK_DIR_REG);
        config->SetPath(INPROGRESS_REG);
    }
    config->SetPath(this->name);
    config->Write(NAME_REG,this->name);
    config->Write(INDEX_REG,this->index);
    config->Write(STATUS_REG,this->status);
    config->Write(SCHEDULED_REG,this->scheduled);
    config->Write(RESTART_REG,this->restart);
    config->Write(PARTS_REG,this->parts);
    config->Write(DESTINATION_REG,this->destination);
    config->Write(SIZE_REG,this->totalsize.ToString());
    config->Write(SIZECOMPLETED_REG,this->totalsizecompleted.ToString());
    config->Write(TIMEPASSED_REG,this->timepassed.ToString());
    config->Write(SPEED_REG,this->totalspeed);
    config->Write(PERCENTUAL_REG,this->percentual);
    config->Write(MD5_REG,this->MD5);
    config->Write(START_REG,this->start.GetTicks());
    config->Write(END_REG,this->end.GetTicks());
    config->Write(COMMENTS_REG,this->comments);
    config->Write(CONTENTTYPE_REG,this->contenttype);

    unsigned int count = 1;
    bool deleteoldurls = TRUE;
    wxString str;
    for ( mUrlList::Node *node = urllist->GetFirst(); node; node = node->GetNext() )
    {
        config->Write(URL_REG + MyUtilFunctions::int2wxstr(count),node->GetData()->GetFullPath());
        count++;
    }
    while (deleteoldurls) //THIS ERASE URLS THAT WAS WRITE BEFORE, BUT THAT DOESN'T WAS REWRITE NOW
    {
        str = wxEmptyString;
        config->Read(URL_REG + MyUtilFunctions::int2wxstr(count),&(str));
        if (str.IsEmpty())
            break;
        else
            config->DeleteEntry(URL_REG + MyUtilFunctions::int2wxstr(count));
    }

    config->Write(USER_REG,this->user);
    config->Write(PASSWORD_REG,this->password);

    delete config;
    this->MarkWriteAsPending(FALSE);
}

int mDownloadFile::GetIndex()
{
    return this->index;
}

wxString mDownloadFile::GetName()
{
    return this->name;
}

wxString mDownloadFile::GetExposedName()
{
    if (this->exposedname != wxEmptyString)
        return this->exposedname;
    else
        return this->name;
}

void mDownloadFile::SetExposedName(wxString name)
{
    this->exposedname = name;
}

void mDownloadFile::UnSetExposedName()
{
    this->exposedname = wxEmptyString;
}

int mDownloadFile::GetStatus()
{
    return this->status;
}

void mDownloadFile::SetAsActive()
{
    this->status = STATUS_ACTIVE;
}

void mDownloadFile::SetAsStoped(bool stopschedule)
{
    this->status = STATUS_STOPED;
    if (stopschedule)
        this->scheduled = FALSE;
}

void mDownloadFile::SetAsFinished()
{
    this->status = STATUS_FINISHED;
}

void mDownloadFile::ErrorOccurred()
{
    this->status = STATUS_ERROR;
}

void mDownloadFile::PutOnScheduleQueue()
{
    this->scheduled = TRUE;
    this->status = STATUS_SCHEDULE_QUEUE;
}

void mDownloadFile::PutOnQueue()
{
    this->scheduled = FALSE;
    this->status = STATUS_QUEUE;
}

wxString mDownloadFile::GetDestination()
{
    return this->destination;
}

wxString mDownloadFile::GetComments()
{
    return this->comments;
}

wxString mDownloadFile::GetUser()
{
    return this->user;
}

wxString mDownloadFile::GetPassword()
{
    return this->password;
}

int mDownloadFile::RestartSupport()
{
    return this->restart;
}

void mDownloadFile::SetRestartSupport(bool support)
{
    if (support)
        this->restart = YES;
    else
        this->restart = NO;
}

bool mDownloadFile::IsScheduled()
{
    return this->scheduled;
}

wxString mDownloadFile::GetContentType()
{
    return this->contenttype;
}

void mDownloadFile::SetContentType(wxString contenttype)
{
    this->contenttype = contenttype;
}

bool mDownloadFile::IsHtml()
{
    return this->GetContentType().Lower().Contains(wxT("html"));
}

int mDownloadFile::GetNumberofParts()
{
    return this->parts;
}

int mDownloadFile::GetCurrentAttempt()
{
    return this->currentattempt;
}

void mDownloadFile::ResetAttempts()
{
    this->currentattempt = 1;
}

void mDownloadFile::IncrementAttempt()
{
    this->currentattempt++;
}

mUrlName mDownloadFile::GetNextUrl()
{
    mUrlName urltmp;
    if (!this->urllist)
        return urltmp;
    if (this->urllist->GetCount() > this->currenturl)
    {
        mUrlList::Node *node = this->urllist->Item(this->currenturl);
        if (node)
        {
            this->currenturl++;
            return *(node->GetData());
        }
        else
        {
            currenturl = 1;
            return this->GetFirstUrl();
        }
    }
    else
    {
        currenturl = 1;
        return this->GetFirstUrl();
    }
}

mUrlName mDownloadFile::GetFirstUrl()
{
    mUrlName urltmp;
    if (!urllist)
        return urltmp;
    mUrlList::Node *node = this->urllist->GetFirst();
    if (node)
    {
        return *(node->GetData());
    }
    else
    {
        return urltmp;
    }
}

bool mDownloadFile::AppendUrl(mUrlName *url)
{
    if (url->IsComplete())
    {
        this->urllist->Append(url);
        this->MarkWriteAsPending(TRUE);
        return TRUE;
    }
    else
        return FALSE;
}

bool mDownloadFile::FindUrl(mUrlName url)
{
    for ( mUrlList::Node *node = this->urllist->GetFirst(); node; node = node->GetNext() )
        if (node->GetData()->GetFullPath().Lower() == url.GetFullPath().Lower())
            return TRUE;
    return FALSE;
}

int mDownloadFile::GetUrlCount()
{
    return this->urllist->GetCount();
}

wxArrayString mDownloadFile::GetUrlArray()
{
    wxArrayString urlarray;
    for ( mUrlList::Node *node = this->urllist->GetFirst(); node; node = node->GetNext() )
        urlarray.Add(node->GetData()->GetFullPath());
    return urlarray;
}

void mDownloadFile::SetFinishedDateTime(wxDateTime time)
{
    this->end = time;
}

wxDateTime mDownloadFile::GetFinishedDateTime()
{
    return this->end;
}

void mDownloadFile::SetMD5(wxString md5)
{
    this->MD5 = md5;
}

int mDownloadFile::GetProgress()
{
    return this->percentual;
}

void mDownloadFile::SetProgress(int percentual)
{
    this->percentual = percentual;
}

bool mDownloadFile::IsSplitted()
{
    return this->split;
}

void mDownloadFile::Split(bool split)
{
    this->split = split;
    this->waitbeforesplit = FALSE;
}

bool mDownloadFile::WaitingForSplit()
{
    return this->waitbeforesplit;
}

void mDownloadFile::WaitSplit()
{
    this->waitbeforesplit = TRUE;
}

bool mDownloadFile::WriteIsPending()
{
    return this->writependig;
}

void mDownloadFile::MarkWriteAsPending(bool pending)
{
    this->writependig = pending;
}

void mDownloadFile::SetFree(bool free)
{
    this->free = free;
}

bool mDownloadFile::IsFree()
{
    return this->free;
}

bool mDownloadFile::RemoveIsPending()
{
    return this->removepending;
}

void mDownloadFile::MarkRemoveAsPending(bool pending)
{
    this->removepending = pending;
}
