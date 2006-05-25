//
// C++ Implementation: DownloadThread
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

int mDownloadThread::GetType()
{
    mUrlName url(currenturl);
    return url.Type();
}

wxLongLong mDownloadThread::CurrentSize(wxString filepath,wxString filename)
{
    wxFile *outputfile;
    wxFileName file;
    file.Assign(filepath + wxT("/"));
    file.SetFullName(filename);
    wxLongLong size = 0;
    if (!file.FileExists())
       return size;
    outputfile = new wxFile(file.GetFullPath(),wxFile::read);
    if (!outputfile)
       return 0;
    size = outputfile->Length();
    outputfile->Close();
    delete outputfile;
    return size;
}

mDownloadThread::mDownloadThread(mDownloadFile *file, int index)
{
    downloadfile = file;
    downloadpartindex = index;
    programoptions = new mOptions(wxGetApp().mainframe->programoptions);
    downloadlist = &(wxGetApp().downloadlist);
}

void mDownloadThread::OnExit()
{
    wxGetApp().mainframe->SendSizeEvent();
    downloadfile->finished[downloadpartindex] = TRUE;
    if (downloadpartindex == 0)
    {
        delete downloadfile->mutex_speedcalc;
        downloadfile->mutex_speedcalc = NULL;

        if (downloadfile->GetStatus() == STATUS_STOPED)
        {
            if (downloadfile->IsScheduled())
                downloadfile->PutOnScheduleQueue();
            downloadfile->MarkWriteAsPending(TRUE);
        }

        downloadfile->SetFree();
    }
    delete programoptions;
}

void *mDownloadThread::Entry()
{
    wxSocketClient *connection = NULL;
    wxInputStream *filestream = NULL;
    int resp = -1;
    wxLongLong start,end;
    redirecting = FALSE;
    downloadfile->ResetAttempts();
    downloadfile->messages[downloadpartindex].Clear();
    downloadfile->finished[downloadpartindex] = FALSE;
    if (downloadpartindex == 0)
    {
        downloadfile->mutex_speedcalc = new wxMutex();
        downloadfile->criticalerror = FALSE;
        for (int i=0;i<downloadfile->GetNumberofParts();i++)
            downloadfile->startpoint[i] = 0;
    }
    else
    {
        while (downloadfile->WaitingForSplit())
        {
            if (downloadfile->GetStatus() == STATUS_STOPED)
                return NULL;
            Sleep(10);
        }
        if (!downloadfile->IsSplitted())
            return NULL;
    }
    do
    {
        downloadfile->SetAsActive();
        start = CurrentSize(downloadfile->GetDestination(),PREFIX + downloadfile->GetName() + EXT + MyUtilFunctions::int2wxstr(downloadpartindex));
        start += downloadfile->startpoint[downloadpartindex];
        currenturl = downloadfile->GetNextUrl();
        do
        {
                int type = GetType();
                if (type == HTTP)
                    connection = ConnectHTTP(&start);
                else if (type == FTP)
                    connection = ConnectFTP(&start);
                else if (type == LOCAL_FILE)
                    filestream = ConnectLOCAL_FILE(start);
                else
                {
                    PrintMessage( _("This protocol isn't supported.\n"),HTMLERROR);
                    downloadfile->criticalerror = TRUE;
                    break;
                }
                if ((downloadpartindex == 0) && (downloadfile->GetContentType().Lower().Contains(wxT("html"))))
                {
                    PrintMessage( _("This is a HTML file\nOpening in the default browser.\n"),HTMLERROR);
                    if ((!connection) && (!filestream))
                    {
                        resp = 0;
                        downloadfile->SetAsFinished();
                    }
                    wxCommandEvent openurl(wxEVT_OPEN_URL);
                    openurl.SetString(currenturl);
                    wxGetApp().mainframe->GetEventHandler()->AddPendingEvent(openurl);
                    break;
                }
        } while (redirecting);
        if ((connection) || (filestream))
        {

            if ((downloadpartindex == 0) && (downloadfile->WaitingForSplit()))
            {
                if ((downloadfile->RestartSupport() == YES) && (downloadfile->totalsize > MIN_SIZE_TO_SPLIT))
                {
                    wxLongLong tempsize = (downloadfile->totalsize / downloadfile->GetNumberofParts());
                    int i;
                    int nparts = downloadfile->GetNumberofParts();
                    for (i=0; i < ((downloadfile->GetNumberofParts())-1); i++)
                    {
                        downloadfile->startpoint[i] = wxLongLong(0l,(long)i)*tempsize;
                        downloadfile->size[i] = tempsize;
                    }
                    downloadfile->startpoint[(nparts)-1] = wxLongLong(0l,(long)((nparts)-1))*tempsize;
                    downloadfile->size[(nparts)-1] = downloadfile->totalsize - wxLongLong(0l,(long)(((nparts)-1)))*tempsize;
                    downloadfile->Split(TRUE);
                }
                else
                {
                    wxString partialfile = downloadfile->GetDestination();
                    if (partialfile.Mid(partialfile.Length()-1,1) != SEPARATOR_DIR)
                        partialfile += SEPARATOR_DIR;
                    partialfile += PREFIX + downloadfile->GetName() + EXT + MyUtilFunctions::int2wxstr(downloadpartindex);
                    ::wxRemoveFile(partialfile);
                    start = 0;
                    downloadfile->Split(FALSE);
                    downloadfile->size[downloadpartindex] = downloadfile->totalsize;
                }
            }
            end = downloadfile->startpoint[downloadpartindex] + downloadfile->size[downloadpartindex];
            resp = DownloadPart(connection,filestream,start,end);
        }
        else
        {
            if (downloadfile->GetStatus() == STATUS_STOPED)
                resp = 0;
            else if (downloadfile->GetStatus() != STATUS_FINISHED)
                resp = -1;
        }
        if (downloadpartindex == 0)
            downloadfile->IncrementAttempt();
        if ((downloadfile->GetCurrentAttempt() <= programoptions->attempts) && (resp == -1) && (!downloadfile->criticalerror))
        {
            PrintMessage( _("New attempt in ") + MyUtilFunctions::int2wxstr(programoptions->attemptstime) + _(" seconds\n"));
            wxStopWatch waittime;
            waittime.Start();
            while (waittime.Time() < ((programoptions->attemptstime)*1000))
            {
                if (downloadfile->GetStatus() == STATUS_STOPED)
                {
                    resp = 0;
                    break;
                }
                this->Sleep(10);
            }
            waittime.Pause();
            if ((downloadfile->GetStatus() == STATUS_ACTIVE) && (downloadpartindex == 0))
                PrintMessage( _("Attempt ") + MyUtilFunctions::int2wxstr(downloadfile->GetCurrentAttempt()) + _(" of ") + MyUtilFunctions::int2wxstr(programoptions->attempts) + wxT(" ...\n"));
        }
    }
    while ((downloadfile->GetCurrentAttempt() <= programoptions->attempts) && (resp == -1) && (!downloadfile->criticalerror));

    if ((resp == -1) || (downloadfile->criticalerror))
    //WILL BE TRUE IF A ERROR HAPPEN IN THIS THREAD(RESP == -1)
    //OR IN THE OTHERS(downloadfile->error == TRUE)
    {
        downloadfile->criticalerror = TRUE;
        downloadfile->SetAsStoped();
        if (downloadpartindex == 0)
        {
            WaitUntilAllFinished(FALSE);
            downloadfile->ErrorOccurred();
        }
    }
    if (downloadfile->GetStatus() == STATUS_STOPED)
    {
        if (downloadpartindex == 0)
            WaitUntilAllFinished(FALSE);
        PrintMessage( _("Canceled.\n"));
    }

    return NULL;
}

int mDownloadThread::DownloadPart(wxSocketClient *connection, wxInputStream *filestream, wxLongLong start,wxLongLong end)
{
    int resp = 0;
    wxInputStream *in = NULL;
    wxFileName destination;
    int type = GetType();
    long readbuffersize = programoptions->readbuffersize;

    destination.Assign(downloadfile->GetDestination() + wxT("/"));
    destination.SetFullName(downloadfile->GetName());

    if (start > end)
    {
        downloadfile->criticalerror = TRUE;
        PrintMessage( _("Critical error!! The start point is bigger that the end point!!\n"),HTMLERROR);
        resp = -1;
    }
    else if (start == end)
    {
        resp = 0;
        downloadfile->sizecompleted[downloadpartindex] = start - downloadfile->startpoint[downloadpartindex];
    }
    else
    {
        if (type == FTP)
        {
            mFTP *tempconnection = (mFTP*)connection;
            mUrlName url(currenturl);
            in = tempconnection->GetInputStream(url.GetFullRealName());
            PrintMessage( tempconnection->GetLastResult() + wxT("\n"),HTMLSERVER);
        }
        else if (type == HTTP)
            in = new wxSocketInputStream(*connection);
        else if (type == LOCAL_FILE)
            in = filestream;
        if (!in)
        {
            PrintMessage( _("Error establishing input stream.\n"),HTMLERROR);
            if (connection) {connection->Close(); delete connection;}
            connection = NULL;
            in = NULL;
            return resp = -1;
        }

        if (downloadfile->GetStatus() == STATUS_STOPED)
        {
            if (connection) {connection->Close(); delete connection;}
            connection = NULL;
            delete in;
            in = NULL;
            return resp = 1;
        }
        PrintMessage( _("Copying file...\n"));

        wxString filepath = destination.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        filepath = filepath + PREFIX + destination.GetFullName() + EXT + MyUtilFunctions::int2wxstr(downloadpartindex);
        wxFile *outputfile = new wxFile(filepath,wxFile::write_append);
        if (!outputfile)
        {
            PrintMessage( _("Error opening file ") + filepath + wxT(".\n"),HTMLERROR);
            if (connection) {connection->Close(); delete connection;}
            connection = NULL;
            delete in;
            in = NULL;
            return resp = -1;
        }
        char *data = new char[readbuffersize];
        long read_buffer;
        wxLongLong readbuffersizelonglong(0l,readbuffersize);
        wxLongLong lasttime;
        wxLongLong endminustart;
        int counttimes=0;
        wxStopWatch time;
        wxLongLong timepassed = downloadfile->timepassed;

        downloadfile->delta_size[downloadpartindex] = 0;
        downloadfile->speedpoint = FALSE;
        time.Pause();
        time.Start();
        lasttime = timepassed + time.Time();

        while (start < end)
        {
            read_buffer = readbuffersize;
            endminustart = end - start;

            if (readbuffersizelonglong > endminustart)
                read_buffer = endminustart.ToLong();

            if (downloadfile->GetStatus() == STATUS_STOPED){resp = 1; break;}
            this->Sleep(1);
            //this->Yield();
            in->Read(data, read_buffer);
            if (in->LastRead() == 0)
            {
                counttimes++;
                if (counttimes == 3000)
                {
                    PrintMessage( _("Error copying file.\n"),HTMLERROR);
                    this->Sleep(1);
                    resp = -1;
                    if (downloadpartindex == downloadfile->speedpointowner)
                    {
                        downloadfile->totalspeed = 0;
                        downloadfile->speedpointowner = -1;
                        downloadfile->speedpoint = FALSE;
                    }
                    downloadfile->delta_size[downloadpartindex] = 0;
                    break;
                }
            }
            else
            {
                counttimes = 0;
                if (downloadfile->GetStatus() == STATUS_STOPED){resp = 1; break;}

                if (outputfile->Write(data,in->LastRead()) != in->LastRead())
                {
                    PrintMessage( _("Error writing file.\n"),HTMLERROR);
                    resp = -1;
                    if (downloadpartindex == downloadfile->speedpointowner)
                    {
                        downloadfile->totalspeed = 0;
                        downloadfile->speedpointowner = -1;
                        downloadfile->speedpoint = FALSE;
                    }
                    downloadfile->delta_size[downloadpartindex] = 0;
                    break;
                }
                else
                   start += in->LastRead();
                if (downloadfile->GetStatus() == STATUS_STOPED){resp = 1; break;}
            }
            downloadfile->delta_size[downloadpartindex] += in->LastRead();
            downloadfile->sizecompleted[downloadpartindex] = start - downloadfile->startpoint[downloadpartindex];

            if (!downloadfile->speedpoint)
                downloadfile->delta_size[downloadpartindex] = 0;

            //if (downloadpartindex == 0)
            if (downloadfile->mutex_speedcalc->TryLock() == wxMUTEX_NO_ERROR)
            {
                if (downloadfile->speedpointowner == -1)
                    downloadfile->speedpointowner = downloadpartindex;
                if (downloadfile->speedpointowner == downloadpartindex)
                {
                    if (!downloadfile->speedpoint)
                    {
                        //lasttime = time.Time();
                        lasttime = timepassed + time.Time();
                        downloadfile->speedpoint = TRUE;
                    }
                    downloadfile->timepassed = timepassed + time.Time();
                    if (downloadfile->delta_size[downloadpartindex] > 20*readbuffersize)
                    {
                        //if ((time.Time() - lasttime) > 0)
                        //    SpeedCalculation(time.Time() - lasttime);
                        if ((timepassed + time.Time() - lasttime).ToLong() > 0)
                            SpeedCalculation((timepassed + time.Time() - lasttime).ToLong());
                        downloadfile->speedpoint = FALSE;
                        if (time.Time() > 2000000000l) //I DID THIS BECAUSE Time() RETURN A LONG AND NOD LONGLONG;
                        {
                            timepassed += time.Time();
                            time.Pause();
                            time.Start();
                        }
                    }
                }
                downloadfile->mutex_speedcalc->Unlock();
            }

            if (downloadfile->GetStatus() == STATUS_STOPED){resp = 1; break;}

        }
        if (downloadfile->speedpointowner == downloadpartindex)
        {
            downloadfile->totalspeed = 0;
            downloadfile->speedpointowner = -1;
            downloadfile->speedpoint = FALSE;
        }
        downloadfile->delta_size[downloadpartindex] = 0;
        outputfile->Close();
        delete outputfile;
        delete [] data;
        delete in;
        if (connection) {connection->Close(); delete connection;}
        connection = NULL;
        in = NULL;
    }
    if (resp == 0)
        PrintMessage( _("Finished this piece\n"));
    if (downloadpartindex == downloadfile->speedpointowner)
        downloadfile->speedpointowner = -1;
    if (downloadpartindex == 0)
    {
        if (resp == 0)
        {
            if (downloadfile->IsSplitted())
            {
                PrintMessage( _("Waiting for the others pieces...\n"));
                WaitUntilAllFinished(TRUE);
            }
            if (downloadfile->GetStatus() == STATUS_STOPED)
                resp = 1;
            else
            {
                if (downloadfile->IsSplitted())
                    PrintMessage( _("Putting all the pieces together...\n"));
                downloadfile->SetFinishedDateTime(wxDateTime::Now());
                if (!JoinFiles())
                {
                    PrintMessage( _("Error joining the pieces.\nTry restart the download.\n"),HTMLERROR);
                    resp = -1;
                    downloadfile->criticalerror = TRUE;
                }
                else
                {
                    PrintMessage( _("Checking MD5 of the file...\n"));
                    downloadfile->SetExposedName(_("Checking MD5 ... (") + downloadfile->GetName() + wxT(")"));
                    wxFileName filemd5 = wxFileName(destination.GetFullPath());
                    wxMD5 md5(filemd5);
                    PrintMessage( wxT("MD5 = ") + md5.GetDigest() + wxT("\n"));
                    downloadfile->SetMD5(md5.GetDigest());
                    downloadfile->UnSetExposedName();
                    PrintMessage( _("Finished\n"));
                }
                downloadfile->SetAsFinished();
            }
        }
    }
    //if (resp == 1)
    //    downloadfile->SetAsStoped();
    return resp;
}

wxSocketClient *mDownloadThread::ConnectHTTP(wxLongLong *start)
{
    wxSocketClient *client=NULL;
    wxFileName destination;
    wxString buffer = wxEmptyString;
    mUrlName url(currenturl);
    destination.Assign(downloadfile->GetDestination());
    destination.SetFullName(PREFIX + downloadfile->GetName() + EXT + MyUtilFunctions::int2wxstr(downloadpartindex));
    redirecting = FALSE;
    int restart = NO;

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    wxIPV4address address;
    address.Service(url.GetPort());
    client = new wxSocketClient();
    client->Notify(FALSE);
    client ->SetFlags(wxSOCKET_NOWAIT);
    PrintMessage( _("Resolving host '") + url.GetHost() + wxT("' ..."));
    if (address.Hostname(url.GetHost())==FALSE)
        {
        PrintMessage( _("\nHost not found.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    else
        PrintMessage( _(" OK\n"),HTMLSERVER);

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Trying to connect in '") + url.GetHost() + wxT(" ") + url.GetPort() + wxT("' ...\n"));
    this->Sleep(1);
    client->Connect(address,TRUE);
    if (client->IsConnected() == FALSE )
    {
        PrintMessage( _("Connection denied.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }

    if (client)
    {
        if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

        client->Notify(FALSE);
        PrintMessage( _("Accessing server...\n\n"));
        {
            wxString msgserver;
            char *dados;
            msgserver << wxT("GET ") /*<< wxT(" http://") <<url.GetHost()*/ << url.GetDir() << url.GetFullRealName() << wxT(" HTTP/1.1\r\n");
            PrintMessage( msgserver,HTMLSERVER);
            dados = MyUtilFunctions::wxstr2str(msgserver);
            client->Write(dados, strlen(dados));
            delete dados;
        }

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    {
        char *dados;
        wxString msgserver;
        msgserver << wxT("HOST: ") << url.GetHost() << wxT("\r\n");
        msgserver << wxT("ACCEPT: */*\r\n");
        msgserver << wxT("User-Agent: wxDownload Fast\r\n");
        msgserver << wxT("Range: bytes=") << *start << wxT("-\r\n");
        PrintMessage( msgserver,HTMLSERVER);
        dados = MyUtilFunctions::wxstr2str(msgserver);
        client->Write(dados, strlen(dados));
        delete dados;
    }

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    {
        char *dados;
        wxString msgserver;
        msgserver << wxT("Pragma: no-cache\r\n");
        msgserver << wxT("Connection: close\r\n\r\n");
        PrintMessage( msgserver,HTMLSERVER);
        dados = MyUtilFunctions::wxstr2str(msgserver);
        client->Write(dados, strlen(dados));
        delete dados;
    }

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}


    if (client->WaitForRead(30,0))
    {
        wxString line;
        wxString message;
        char buf[2048];
        wxLongLong sizetmp = 0;
        wxString newfilename = wxEmptyString;
        unsigned int count = 0;
        client->Read(buf,2048);
        do
        {
            line.Clear();
            while ((buf[count] != '\n') && (count <2048))
            {
                if ((buf[count] != '\n') && (buf[count] != '\r'))
                    line.Append(buf[count],1);
                count++;
            }
            if (count < 2048)
                count++;
            PrintMessage( line + wxT("\n"),HTMLSERVER);
            message += line + wxT("\n");
            if (line.Mid(0,14).Lower() == wxT("content-length"))
            {
                sizetmp = MyUtilFunctions::wxstrtolonglong(line.AfterFirst(':').Trim(TRUE).Trim(FALSE));
                //this->Sleep(1);
            }
            else if (line.Mid(0,12).Lower() == wxT("content-type"))
            {
                if (downloadpartindex == 0)
                    downloadfile->SetContentType(line.AfterFirst(':').Trim(TRUE).Trim(FALSE));
                //this->Sleep(1);
            }
            else if (line.Mid(0,13).Lower() == wxT("accept-ranges")||line.Mid(0,13).Lower() == wxT("content-range"))
            {
                restart = YES;
                //this->Sleep(1);
            }
            else if (line.Mid(0,8).Lower() == wxT("location"))
            {
                currenturl = line.AfterFirst(':').Trim(TRUE).Trim(FALSE);
                mUrlName urltmp(currenturl);
                urltmp.Resolve(url);
                currenturl = urltmp.BuildURI();
                //this->Sleep(1);
            }
            else if (line.Mid(0,19).Lower() == wxT("Content-Disposition"))
            {
                wxStringTokenizer tkz(line,wxT(" "));
                while ( tkz.HasMoreTokens() )
                {
                    wxString token = tkz.GetNextToken();
                    if (token.Mid(0,9).Lower() == wxT("filename="))
                    {
                        newfilename = token.Mid(9);
                        if (newfilename.Mid(0,1) == wxT("\""))
                            newfilename = newfilename.AfterFirst('\"').BeforeLast('\"');
                        break;
                    }
                }
            }

            if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}
        }while (line != wxEmptyString);

        {//PUT IN THE QUEUE THE DATA THAT ISN'T PART OF THE MESSAGE
            unsigned int i,lastcount;
            lastcount = client->LastCount();
            char unreadbuf[2048];
            for (i = 0 ; i < (lastcount-count);i++)
                unreadbuf[i] = buf[i+count];
            client->Unread(unreadbuf,i);
        }
        if (newfilename == wxEmptyString)
            newfilename = downloadfile->GetName();
        {
            wxString forbiddenchars;
            int pos;
            forbiddenchars = wxFileName::GetForbiddenChars();
            for (unsigned int i=0;i<forbiddenchars.Length();i++)
            {
                if ((pos = newfilename.Find(forbiddenchars.GetChar(i))) >= 0)
                    newfilename.SetChar(pos,'_');
            }
        }
        if (newfilename != downloadfile->GetName())
            downloadlist->ChangeName(downloadfile,newfilename);

        if ((message.GetChar(9) == '4') && (message.GetChar(10) == '1') && (message.GetChar(11) == '6'))
        {    // HTTP/1.1 416 Requested Range Not Satisfiable
            //IF THIS HAPPEN, IS BECAUSE THE THE START POINT IS BIGGER THAT THE END POINT
            PrintMessage( _("The file already was downloaded.\n"));
        }
        else if (message.GetChar(9) == '3')
        {
            redirecting = TRUE;
            PrintMessage( _("Redirecting to ") + currenturl + wxT("\n\n"),HTMLSERVER);
            downloadlist->ChangeName(downloadfile,currenturl.Mid(currenturl.Find('/',true)+1));
            client->Close(); delete client;
            return NULL;
        }
        else if ((message.GetChar(9) != '1') && (message.GetChar(9) != '2'))
        {
            PrintMessage( _("Error requesting file.\n"),HTMLERROR);
            client->Close(); delete client;
            return NULL;
        }
        else
        {
            if (restart == YES)
            {
                downloadfile->sizecompleted[downloadpartindex] = *start - downloadfile->startpoint[downloadpartindex];
                if (downloadpartindex == 0)
                {
                    if ((downloadfile->totalsize > 0) && (downloadfile->totalsize != sizetmp + *start) && (downloadfile->GetCurrentAttempt() == 1))
                    {
                        wxString partialfile = downloadfile->GetDestination();
                        if (partialfile.Mid(partialfile.Length()-1,1) != SEPARATOR_DIR)
                            partialfile += SEPARATOR_DIR;
                        partialfile += PREFIX + downloadfile->GetName() + EXT;
                        PrintMessage( _("The file size has changed since the last time.\nRemoving the partial downloaded files...\n"),HTMLERROR);
                        for (int i = 0;i < downloadfile->GetNumberofParts();i++)
                        {
                            ::wxRemoveFile(partialfile + MyUtilFunctions::int2wxstr(i));
                            downloadfile->sizecompleted[i] = 0;
                        }
                        downloadfile->totalsize =  sizetmp + *start;
                        *start = 0;

                        redirecting = TRUE;
                        client->Close(); delete client;
                        return NULL;
                    }
                    else
                        downloadfile->totalsize =  sizetmp + *start;
                    downloadfile->SetRestartSupport();
                }
            }
            else
            {
                restart = NO;
                if (sizetmp > 0)
                {
                    downloadfile->totalsize =  sizetmp;
                    downloadfile->sizecompleted[downloadpartindex] = 0;
                    PrintMessage( _("This server don't support restart.\n"),HTMLERROR);
                    if (downloadpartindex == 0)
                        downloadfile->SetRestartSupport(FALSE);
                    else
                    {
                        client->Close(); delete client;
                        return NULL;
                    }
                }
                else
                {
                    downloadfile->totalsize =  0;
                    downloadfile->sizecompleted[downloadpartindex] = 0;
                    PrintMessage( _("Impossible to return the file size.\n"),HTMLERROR);
                    client->Close(); delete client;
                    return NULL;
                }
                /*if (downloadpartindex == 0)
                {
                    downloadfile->restart = restart;
                    if (destination.FileExists())
                        wxRemoveFile(destination.GetFullPath());
                    downloadfile->sizecompleted[downloadpartindex] = 0;
                }*/
            }
        }
    }
    else
    {
        PrintMessage( _("The Server stayed a lot of time without answering.\n"),HTMLERROR);
        client->Close(); delete client;
        return NULL;
    }
  }
  else
  {
    delete client;
      return NULL;
  }

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    return client;
}

wxSocketClient *mDownloadThread::ConnectFTP(wxLongLong *start)
{
    mFTP *client = new mFTP();
    wxIPV4address address;
    wxFileName destination;
    wxString buffer = wxEmptyString;
    wxLongLong sizetmp;
    mUrlName url(currenturl);
    client->Notify(FALSE);

    address.Service(url.GetPort());
    destination.Assign(downloadfile->GetDestination());
    destination.SetFullName(PREFIX + downloadfile->GetName() + EXT + MyUtilFunctions::int2wxstr(downloadpartindex));
    redirecting = FALSE;

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Resolving host '") + url.GetHost() + wxT("' ..."));
    if (address.Hostname(url.GetHost())==FALSE)
    {
        PrintMessage( _("\nHost not found.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    else
        PrintMessage( _(" OK\n"),HTMLSERVER);

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Trying to connect in '") + url.GetHost() + wxT("' ..."));
    this->Sleep(1);
    client->Connect(address,TRUE);
    if (client->IsConnected() == FALSE )
    {
        PrintMessage( _("\nConnection denied.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    else
        PrintMessage( _(" OK\n"),HTMLSERVER);

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Waiting for welcome message.\n"));
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Verifying if the server supports restarts..."));
    if (client->SendCommand(wxT("REST ") + start->ToString())!= '3')
    {
        PrintMessage( _("\nThis server don't support restart.\n"),HTMLERROR);
        if (downloadpartindex == 0)
            downloadfile->SetRestartSupport(FALSE);
        else
        {
            client->Close();
            delete client;
            return NULL;
        }
    }
    else
    {
        PrintMessage( _(" OK\n"),HTMLSERVER);
        if (downloadpartindex == 0)
            downloadfile->SetRestartSupport();
    }

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Changing to directory ") + url.GetDir() + wxT(" ...\n"));
    if (!client->ChDir(url.GetDir()))
    {
        PrintMessage( _("Invalid directory.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Changing to binary mode...\n"));
    if (client->SendCommand(wxT("TYPE I"))!='2')
    {
        PrintMessage( _("Impossible to change to binary mode.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);

    if (downloadfile->GetStatus() == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Verifying the size of the ") + url.GetFullName() + wxT("...\n"));
    if ((sizetmp = client->GetFileSize(url.GetFullName()))== -1) 
    {
        PrintMessage( _("Impossible to return the file size.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    else
    {
        PrintMessage( _("File size: ") + sizetmp.ToString() + wxT("\n"),HTMLSERVER);
        if (downloadfile->RestartSupport() == NO)
            downloadfile->sizecompleted[downloadpartindex] = 0;
        else
            downloadfile->sizecompleted[downloadpartindex] = *start - downloadfile->startpoint[downloadpartindex];
        if (downloadpartindex == 0)
        {
            if ((downloadfile->totalsize > 0) && (downloadfile->totalsize != sizetmp))
            {
                wxString partialfile = downloadfile->GetDestination();
                if (partialfile.Mid(partialfile.Length()-1,1) != SEPARATOR_DIR)
                    partialfile += SEPARATOR_DIR;
                partialfile += PREFIX + downloadfile->GetName() + EXT;
                PrintMessage( _("The file size has changed since the last time.\nRemoving the partial downloaded files...\n"),HTMLERROR);
                for (int i = 0;i < downloadfile->GetNumberofParts();i++)
                {
                    ::wxRemoveFile(partialfile + MyUtilFunctions::int2wxstr(i));
                    downloadfile->sizecompleted[i] = 0;
                }
                *start = 0;
                downloadfile->totalsize =  sizetmp;
                redirecting = TRUE;
                client->Close(); delete client;
                return NULL;
            }
            else
                downloadfile->totalsize =  sizetmp;
        }
    }

    PrintMessage( _("Requesting file...\n"));
/*    if (client->SendCommand(wxT("RETR ") + wxURI::Unescape(currenturl)) != '1')
    {
        PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);
        PrintMessage( _("Error requesting file.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);*/

    return client;
}

wxInputStream *mDownloadThread::ConnectLOCAL_FILE(wxLongLong start)
{
    wxFileInputStream *filestream;
    wxFile *file;
    wxFileName source, destination;
    mUrlName url(currenturl);
    wxString buffer = wxEmptyString;
    wxLongLong sizetmp;

    source.Assign(url.GetDir() + url.GetFullRealName());
    destination.Assign(downloadfile->GetDestination());
    destination.SetFullName(PREFIX + downloadfile->GetName() + EXT + MyUtilFunctions::int2wxstr(downloadpartindex));
    redirecting = FALSE;
    if (downloadpartindex == 0)
        downloadfile->SetRestartSupport(); //YES

    if (downloadfile->GetStatus() == STATUS_STOPED){return NULL;}

    PrintMessage( _("Looking for file ") + url.GetFullRealName() + wxT("...\n"));
    if (!source.FileExists())
    {
        PrintMessage( _("File not found.\n"),HTMLERROR);
        return NULL;
    }

    file = new wxFile(source.GetFullPath());
    PrintMessage( _("Verifying the size of the ") + url.GetFullRealName() + wxT("...\n"));
    if (!file)
    {
        PrintMessage( _("Impossible to return the file size.\n"),HTMLERROR);
        return NULL;
    }
    else
    {
        sizetmp = file->Length();
        PrintMessage( _("File size: ") + sizetmp.ToString() + wxT("\n"),HTMLSERVER);
        if (downloadpartindex == 0)
            downloadfile->totalsize =  sizetmp;
        downloadfile->sizecompleted[downloadpartindex] = start - downloadfile->startpoint[downloadpartindex];
        file->Close();
        delete file;
    }

    PrintMessage( _("Requesting file...\n"));
    filestream = new wxFileInputStream(source.GetFullPath());
    filestream->SeekI(start.GetValue());

    return filestream;
}

void mDownloadThread::PrintMessage(wxString str,wxString color)
{
    #ifdef USE_HTML_MESSAGES
    wxString newstr(str);
    newstr.Replace(wxT("\r"),wxT(""));
    newstr.Replace(wxT("\n"),wxT("<BR>"));
    downloadfile->messages[downloadpartindex].Append(wxT("<font size=1 color=") + color + wxT(">") + newstr + wxT("</font>"));
    #else
    wxString newstr(str);
    newstr.Replace(wxT("\r"),wxT(""));
    downloadfile->messages[downloadpartindex].Append(newstr);
    #endif
}

void mDownloadThread::SpeedCalculation(long delta_t)
{
    int parts;
    if (downloadfile->IsSplitted())
        parts = downloadfile->GetNumberofParts();
    else
        parts = 1;
    downloadfile->totalspeed = 0;
    for (int i = 0; i < parts; i++)
    {
        if (!downloadfile->finished[downloadpartindex])
            downloadfile->totalspeed += downloadfile->delta_size[i]*1000l / (delta_t);
        downloadfile->delta_size[i] = 0;
    }
}

void mDownloadThread::WaitUntilAllFinished(bool canstop)
{
    //wxStopWatch time;
    //time.Pause();
    //time.Start(downloadfile->timepassed);
    //long lasttime = time.Time();
    //downloadfile->speedpoint = FALSE;
    if (downloadfile->GetNumberofParts() > 1)
    {
        bool waitall = TRUE;
        while (waitall)
        {
            int i;
            waitall = FALSE;
            for (i=1; i < downloadfile->GetNumberofParts(); i++)
                if (downloadfile->finished[i] == FALSE)
                    waitall = TRUE;
            if ((downloadfile->GetStatus() == STATUS_STOPED) && (canstop))    {return ;}
            Sleep(500);
            if ((downloadfile->GetStatus() == STATUS_STOPED) && (canstop))    {return ;}
            /*if (downloadpartindex == 0)
            {
                downloadfile->timepassed = time.Time();
                if (!downloadfile->speedpoint)
                {
                    lasttime = time.Time();
                    downloadfile->speedpoint = TRUE;
                }
                if ((time.Time()-lasttime) > 1000)
                {
                    downloadfile->delta_size[downloadpartindex] = 0;
                    SpeedCalculation(time.Time()-lasttime);
                    downloadfile->speedpoint = FALSE;
                }
            }*/
        }
    }
}

bool mDownloadThread::JoinFiles()
{
    wxFile *outputfile;
    wxFileName destination;
    wxString str;
    long readbuffersize = programoptions->readbuffersize;
    bool result;
    int data_size = 60*readbuffersize;
    char *data = new char[data_size];
    long lastread = 0;
    destination.Assign(downloadfile->GetDestination() + wxT("/"));
    destination.SetFullName(downloadfile->GetName());
    str = destination.GetFullPath();
    destination.SetFullName(PREFIX + downloadfile->GetName());
    if (destination.FileExists())
    {
        wxFileName temp(str);
        downloadlist->ChangeName(downloadfile,MyUtilFunctions::int2wxstr(downloadfile->GetFinishedDateTime().GetTicks()) + wxT(" - ") + downloadfile->GetName());
        temp.SetFullName(downloadfile->GetName());
        str = temp.GetFullPath();
    }
    result = wxRenameFile(destination.GetFullPath() + EXT + MyUtilFunctions::int2wxstr(0),str);
    if ((downloadfile->IsSplitted()) && (result == TRUE))
    {
        outputfile = new wxFile(str,wxFile::write_append);
        int i;
        for (i=1;i < downloadfile->GetNumberofParts();i++)
        {
            str = destination.GetFullPath() + EXT + MyUtilFunctions::int2wxstr(i);
            if (::wxFileExists(str))
            {
                wxFile *piece = new wxFile(destination.GetFullPath() + EXT + MyUtilFunctions::int2wxstr(i),wxFile::read);
                while (!piece->Eof())
                {
                    lastread = piece->Read(data,data_size);
                    outputfile->Write(data,lastread);
                    lastread = 0;
                }
                piece->Close();
                delete piece;
                wxRemoveFile(str);
            }
            else
            {
                result = FALSE;
                break;
            }
        }
        outputfile->Close();
        delete outputfile;
    }
    delete [] data;
    return result;
}
