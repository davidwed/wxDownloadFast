#include "wxDFast.h"
#include <iostream>

int mDownloadThread::GetType()
{
	mUrlName url;
	url.Assign(currenturl);
	return url.Type();
}

long mDownloadThread::CurrentSize(wxString filepath,wxString filename)
{
    wxFile *outputfile;
    wxFileName file;
    file.Assign(filepath + wxT("/"));
  	file.SetFullName(filename);
    long size = 0;
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
}

void mDownloadThread::OnExit()
{
	wxGetApp().mainframe->SendSizeEvent();
	downloadfile->finished[downloadpartindex] = TRUE;
	if (downloadpartindex == 0)
		downloadfile->free = TRUE;
}

void *mDownloadThread::Entry()
{   
	wxSocketClient *connection = NULL;
	int resp = -1,start,end;
	mOptions *programoptions = &(wxGetApp().mainframe->programoptions);
	redirecting = FALSE;
	downloadfile->currentattempt = 1;
	downloadfile->messages[downloadpartindex].Clear();
	downloadfile->finished[downloadpartindex] = FALSE;
	if (downloadpartindex == 0)
	{
		downloadfile->criticalerror = FALSE;
		for (int i=0;i<downloadfile->parts;i++)
			downloadfile->startpoint[i] = 0;
	}
	else
	{
		while (downloadfile->split == WAIT)
			Sleep(10);
		if (downloadfile->split == NO)
			return NULL;
	}
	do
	{
		downloadfile->status = STATUS_ACTIVE;
		start = CurrentSize(downloadfile->destination,PREFIX + downloadfile->name + EXT + int2wxstr(downloadpartindex));
		start += downloadfile->startpoint[downloadpartindex];
		currenturl = downloadfile->urllist;
		do
		{
				int type = GetType();
				if (type == HTTP)
					connection = ConnectHTTP(start);
				else if (type == FTP)
					connection = ConnectFTP(start);
				else
				{
					PrintMessage( _("This protocol isn't supported.\n"),HTMLERROR);
					downloadfile->criticalerror = TRUE;
					break;
				}
		}
		while (redirecting);
		if (connection)
		{
			
			if ((downloadpartindex == 0) && (downloadfile->split == WAIT))
			{
				if ((downloadfile->restart == YES) && (downloadfile->totalsize > MIN_SIZE_TO_SPLIT))
				{
					long tempsize = (long) (downloadfile->totalsize / downloadfile->parts);
					int i;
					for (i=0; i < ((downloadfile->parts)-1); i++)
					{
						downloadfile->startpoint[i] = i*tempsize;
						downloadfile->size[i] = tempsize;
					}
					downloadfile->startpoint[(downloadfile->parts)-1] = ((downloadfile->parts)-1)*tempsize;
					downloadfile->size[(downloadfile->parts)-1] = downloadfile->totalsize - (((downloadfile->parts)-1)*tempsize);
					downloadfile->split = YES;
				}
				else
				{
					downloadfile->split = NO;
					downloadfile->size[downloadpartindex] = downloadfile->totalsize;
				}
			}
			end = downloadfile->startpoint[downloadpartindex] + downloadfile->size[downloadpartindex];
			resp = DownloadPart(connection,start,end);
		}
		else
		{
			if (downloadfile->status == STATUS_STOPED)
				resp = 0;
			else
				resp = -1;
		}
		if (downloadpartindex == 0)
			downloadfile->currentattempt++;
		if ((downloadfile->currentattempt <= programoptions->attempts) && (resp == -1) && (!downloadfile->criticalerror))
		{
			PrintMessage( _("New attempt in ") + int2wxstr(programoptions->attemptstime) + _(" seconds\n"));
			wxStopWatch waittime;
			waittime.Start();
			while (waittime.Time() < ((programoptions->attemptstime)*1000)) 
			{
				if (downloadfile->status == STATUS_STOPED)
				{
		            resp = 0;
		            break;
				}
				this->Sleep(10);
			}
			waittime.Pause();
			if ((downloadfile->status == STATUS_ACTIVE) && (downloadpartindex == 0))
				PrintMessage( _("Attempt ") + int2wxstr(downloadfile->currentattempt) + _(" of ") + int2wxstr(wxGetApp().mainframe->programoptions.attempts) + _(" ...\n"));
		}
	}
	while ((downloadfile->currentattempt <= programoptions->attempts) && (resp == -1) && (!downloadfile->criticalerror));
	if ((resp == -1) || (downloadfile->criticalerror))
	//WILL BE TRUE IF A ERROR HAPPEN IN THIS THREAD(RESP == -1)
	//OR IN THE OTHERS(downloadfile->error == TRUE)
	{
		downloadfile->criticalerror = TRUE;
		downloadfile->status = STATUS_STOPED;
		if (downloadpartindex == 0)
		{
			WaitUntilAllFinished(FALSE);
			downloadfile->status = STATUS_ERROR;
		}	
	}
	if (downloadfile->status == STATUS_STOPED)
		PrintMessage( _("Canceled.\n"));
	if (downloadpartindex == 0)
	{
		wxGetApp().RegisterListItemOnDisk(downloadfile);
		if (downloadfile->status == STATUS_STOPED)
			downloadfile->status = STATUS_ACTIVE; //THIS IS FOR THE LIST BE UPDATED FOR THA LAST TIME
	}
	return NULL;
}

int mDownloadThread::DownloadPart(wxSocketClient *connection, long start,long end)
{
	int resp = 0;
	wxInputStream *in;
	wxFileName destination;
	destination.Assign(downloadfile->destination + wxT("/"));
	destination.SetFullName(downloadfile->name);
	int type = GetType();
	
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
    	//connection->SetFlags(wxSOCKET_WAITALL);
    	if (type == FTP)
	    {
	    	wxFTP *tempconnection = (wxFTP*)connection;
	    	mUrlName url(currenturl);
			in = tempconnection->GetInputStream(url.GetFullName());
			PrintMessage( tempconnection->GetLastResult() + wxT("\n"),HTMLSERVER);
	    }
    	else
	    	in = new wxSocketInputStream(*connection);
	    if (!in)
	    {
	        PrintMessage( _("Error establishing input stream.\n"),HTMLERROR);
	        connection->Close();
	        delete connection;
	        connection = NULL;
	        in = NULL;
	        return resp = -1;
	    }
	
		if (downloadfile->status == STATUS_STOPED)
		{
			connection->Close();
			delete connection;
			connection = NULL;
			delete in;
			in = NULL;
			return resp=1;
		}
		PrintMessage( _("Copying file...\n"));
	
	    wxFile *outputfile = new wxFile(destination.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + PREFIX + destination.GetFullName() + EXT + int2wxstr(downloadpartindex),wxFile::write_append);
	    char *data = new char[READ_BUFFER];
	    wxStopWatch time;
	    int read_buffer;
	    long lasttime;
	    downloadfile->delta_size[downloadpartindex] = 0;
	    downloadfile->speedpoint = FALSE;
	    time.Pause();
	    if (downloadpartindex == 0)
	    	time.Start(downloadfile->timepassed); //INICIAR APARTIR DO VALOR ATUAL
	    else
	    	time.Start();
	    lasttime = time.Time();
	    while (start < end)
	    {
	    	read_buffer = READ_BUFFER;
	    	if (READ_BUFFER > (end - start))
	    		read_buffer = end - start;
			
			if (downloadfile->status == STATUS_STOPED){resp = 1; break;}
			
			while ((type != FTP) && (connection->WaitForRead(0,10) == FALSE))
			{
				if (downloadfile->status == STATUS_STOPED){break;}
				this->Sleep(30);
			}
			if (downloadfile->status == STATUS_STOPED){resp = 1; break;}
	        this->Sleep(30);
	        if (!in->Read(data, read_buffer))
	        {
	            PrintMessage( _("Error copying file.\n"),HTMLERROR);
	            resp = -1;
	            break;
	        }
	        else
	        {
				if (downloadfile->status == STATUS_STOPED){resp = 1; break;}
				
	            if (outputfile->Write(data,in->LastRead()) != in->LastRead())
	            {
	                PrintMessage( _("Error writing file.\n"),HTMLERROR);
	                resp = -1;
	                break;
	            }
	            else
	               start += in->LastRead();
				
				if (downloadfile->status == STATUS_STOPED){resp = 1; break;}
	        }
	        downloadfile->delta_size[downloadpartindex] += in->LastRead();
	        downloadfile->sizecompleted[downloadpartindex] = start - downloadfile->startpoint[downloadpartindex];
	               
		    if (!downloadfile->speedpoint)
		    	downloadfile->delta_size[downloadpartindex] = 0;
	        if (downloadpartindex == 0)
	        {
	        	if (!downloadfile->speedpoint)
	        	{
	        		lasttime = time.Time();
			    	downloadfile->speedpoint = TRUE;
	        	}
		        downloadfile->timepassed = time.Time();
		        if (downloadfile->delta_size[downloadpartindex] > 20*READ_BUFFER)
		        {
		        	if ((time.Time() - lasttime) > 0)
						SpeedCalculation(time.Time() - lasttime);
		        	downloadfile->speedpoint = FALSE;
		        }
	        }
	        
	        if (downloadfile->status == STATUS_STOPED){resp = 1; break;}
	    }
	    outputfile->Close();
	    delete outputfile;
	    delete [] data;
	    delete in;
		connection->Close();
		delete connection;
		connection = NULL;
		in = NULL;
    }
	if (resp == 0)
		PrintMessage( _("Finished this piece\n"));
	if (downloadpartindex == 0)
	{
		if (resp == 0)
		{
			if (downloadfile->split == YES)
			{
				PrintMessage( _("Waiting for the others pieces...\n"));
				WaitUntilAllFinished(downloadfile);
			}
			if (downloadfile->status == STATUS_STOPED)
				resp = 1;
			else
			{
				if (downloadfile->split == YES)
					PrintMessage( _("Putting all the pieces together...\n"));
				downloadfile->end = wxDateTime::Now();
				if (!JoinFiles())
				{
					PrintMessage( _("Error joining the pieces.\nTry restart the download.\n"),HTMLERROR);
					resp = -1;
					downloadfile->criticalerror = TRUE;
				}
				else
				{
				    PrintMessage( _("Verifying MD5 of the file...\n"));
				    destination.SetFullName(downloadfile->name);
				    wxFileName filemd5 = wxFileName(destination.GetFullPath());
				    wxMD5 md5(filemd5);
				    PrintMessage( _("MD5 = ") + md5.GetDigest() + wxT("\n"));
				    downloadfile->MD5 = md5.GetDigest();
				    PrintMessage( _("Finished\n"));
				}
				downloadfile->status = STATUS_FINISHED;
			}
		}
	}
	if (resp == 1)
		downloadfile->status = STATUS_STOPED;
	return resp;
}

wxSocketClient* mDownloadThread::ConnectPROXY(wxString proxytype,wxString server,wxString port)
{
	wxSocketClient *client = new wxSocketClient();
    wxIPV4address address;
	
	address.Service(port);
	client->Notify(FALSE);
	
		
    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

	if (address.Hostname(server)==FALSE)
    {
        PrintMessage( _("\nProxy server not found.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    else
        PrintMessage( _(" OK\n"),HTMLSERVER);
	 
	
	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
	
	PrintMessage( _("Trying to connect in '") + server + _("' ...\n"));
    client->Connect(address,TRUE);
	
    if (client->IsConnected() == FALSE )
    {
        PrintMessage( _("Connection denied.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
	else
		PrintMessage( _("Connection proxy server success.\n"),HTMLSERVER);
	    
	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

	
		
	return client;


}

wxSocketClient *mDownloadThread::ConnectHTTP(long start)
{
	wxSocketClient *client=NULL;
    mUrlName url;
    wxFileName destination;
    wxString buffer = wxEmptyString;
    
    
	
	url.Assign(currenturl);
	destination.Assign(downloadfile->destination);
	destination.SetFullName(PREFIX + downloadfile->name + EXT + int2wxstr(downloadpartindex));
	redirecting = FALSE;
	restart = NO;

    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
	
	if(downloadfile->server!=wxEmptyString)
	{	
		
		client = ConnectPROXY(downloadfile->proxytype,downloadfile->server,downloadfile->port);
	
	}
	else
	{
		wxIPV4address address;
		address.Service(80);
		client = new wxSocketClient();
		client->Notify(FALSE);
    PrintMessage( _("Resolving host '") + url.GetHost() + _("' ..."));
    if (address.Hostname(url.GetHost())==FALSE)
		{
			PrintMessage( _("\nHost not found.\n"),HTMLERROR);
			client->Close();
			delete client;
			return NULL;
		}
    else
        PrintMessage( _(" OK\n"),HTMLSERVER);

	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Trying to connect in '") + url.GetHost() + _("' ...\n"));
    client->Connect(address,TRUE);
    if (client->IsConnected() == FALSE )
		{
			PrintMessage( _("Connection denied.\n"),HTMLERROR);
			client->Close();
			delete client;
			return NULL;
		}
	}
    if (client)
    {
    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
    
		client->Notify(FALSE);
		PrintMessage( _("Accessing server...\n\n"));
		{
			wxString msgserver;
			char *dados;
			msgserver << downloadfile->proxyaction<<wxT(" http://") <<url.GetHost()<< url.GetDir() << url.GetFullName() << wxT(" HTTP/1.1\r\n");
			PrintMessage( msgserver,HTMLSERVER);
			dados = wxstr2str(msgserver);
			client->Write(dados, strlen(dados));
			delete dados;
		}
    
    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
    
    {
        char *dados;
        wxString msgserver;
        msgserver << wxT("HOST: ") << url.GetHost() << wxT("\r\n");
        msgserver << wxT("ACCEPT: */*\r\n");
        msgserver << wxT("User-Agent: wxDownload Fast\r\n");
        msgserver << wxT("Range: bytes=") << start << wxT("-\r\n");
        PrintMessage( msgserver,HTMLSERVER);
        dados = wxstr2str(msgserver);
        client->Write(dados, strlen(dados));
        delete dados;
    }
    
    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
    
    {
        char *dados;
        wxString msgserver;
        msgserver << wxT("Pragma: no-cache\r\n");
        msgserver << wxT("Connection: close\r\n\r\n");
		PrintMessage( msgserver,HTMLSERVER);
        dados = wxstr2str(msgserver);
        client->Write(dados, strlen(dados));
        delete dados;
    }
    
    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
    
    
    if (client->WaitForRead(30,0))
    {
    	wxString line;
    	wxString message;
        char buf[1024];
        long sizetmp;
        unsigned int count = 0;
        client->Read(buf,1024);
        do
    	{
    		line.Clear();
    		while (buf[count] != '\n')
    		{
	    		if ((buf[count] != '\n') && (buf[count] != '\r'))
		    		line.Append(buf[count],1);
                count++;
    		}
            count++;
    		PrintMessage( line + wxT("\n"),HTMLSERVER);
    		message += line + wxT("\n");
            if (line.Mid(0,14).Lower() == wxT("content-length"))
                line.Mid(16).ToLong(&sizetmp);
            if (line.Mid(0,13).Lower() == wxT("accept-ranges")||line.Mid(0,13).Lower() == wxT("content-range"))
                restart = YES;
            if (line.Mid(0,8).Lower() == wxT("location"))
            {
                currenturl = line.Mid(10);
                redirecting = TRUE;
            }

			if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    	}while (line != wxEmptyString);

        {//PUT IN THE QUEUE THE DATA THAT ISN'T PART OF THE MESSAGE
            unsigned int i,lastcount;
            lastcount = client->LastCount();
            char unreadbuf[1024];
            for (i = 0 ; i < lastcount-count;i++)
                unreadbuf[i] = buf[i+count];
            client->Unread(unreadbuf,i);
        }
    	
        if ((message.GetChar(9) == '4') && (message.GetChar(10) == '1') && (message.GetChar(11) == '6'))
        {	// HTTP/1.1 416 Requested Range Not Satisfiable
        	//IF THIS HAPPEN, IS BECAUSE THE THE START POINT IS BIGGER THAT THE END POINT
        	PrintMessage( _("The file already was downloaded.\n"));
        }
        else if (message.GetChar(9) == '3')
        {
            PrintMessage( _("Redirecting to ") + currenturl + wxT("\n\n"),HTMLSERVER);
            wxGetApp().ChangeName(downloadfile, currenturl.Mid(currenturl.Find('/',true)+1),0);
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
	        	if (downloadpartindex == 0)
	        	{
	        		downloadfile->totalsize =  sizetmp + start;
	        		downloadfile->restart = restart;
	        	}
	        	downloadfile->sizecompleted[downloadpartindex] = start - downloadfile->startpoint[downloadpartindex];
	        }
	        else
	        {
	        	restart = NO;
	        	if (downloadpartindex == 0)
		        	downloadfile->restart = restart;
	        	/*if (downloadpartindex == 0)
	        	{
	        		downloadfile->restart = restart;
		            if (destination.FileExists())
	    	            wxRemoveFile(destination.GetFullPath());
	        	    downloadfile->sizecompleted[downloadpartindex] = 0;
	        	}
	        	else*/
	        	{
	        		PrintMessage( _("This server don't support restart.\n"),HTMLERROR);
	            	client->Close(); delete client;
	            	return NULL;
	        	}
	            
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
	    
    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    return client;
}

wxSocketClient *mDownloadThread::ConnectFTP(long start)
{
	wxFTP *client = new wxFTP();
    wxIPV4address address;
    mUrlName url;
    wxFileName destination;
    wxString buffer = wxEmptyString;
    long sizetmp;
    address.Service(21);
    client->Notify(FALSE);
	
	url.Assign(currenturl);
	destination.Assign(downloadfile->destination);
	destination.SetFullName(PREFIX + downloadfile->name + EXT + int2wxstr(downloadpartindex));
	redirecting = FALSE;
	restart = NO;

    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
	
    PrintMessage( _("Resolving host '") + url.GetHost() + _("' ..."));
    if (address.Hostname(url.GetHost())==FALSE)
    {
        PrintMessage( _("\nHost not found.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    else
        PrintMessage( _(" OK\n"),HTMLSERVER);

	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Trying to connect in '") + url.GetHost() + _("' ..."));
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

    if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Waiting for welcome message.\n"));
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);

	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Verifying if the server supports restarts..."));
    if (client->SendCommand(wxT("REST ") + int2wxstr(start))!= '3')
    {
    	restart = NO;
    	if (downloadpartindex == 0)
        	downloadfile->restart = restart;
  		PrintMessage( _("\nThis server don't support restart.\n"),HTMLERROR);
       	client->Close();
       	delete client;
       	return NULL;
    }
    else
    {
    	PrintMessage( _(" OK\n"),HTMLSERVER);
    	restart = YES;
    	if (downloadpartindex == 0)
    		downloadfile->restart = restart;
    }
    
	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
    
    PrintMessage( _("Changing to directory ") + url.GetDir() + _(" ...\n"));
    if (!client->ChDir(url.GetDir()))
    {
        PrintMessage( _("Invalid directory.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);

	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}

    PrintMessage( _("Changing to binary mode...\n"));
    if (client->SendCommand(wxT("TYPE I"))!='2')
    {
        PrintMessage( _("Impossible to change to binary mode.\n"),HTMLERROR);
        client->Close();
        delete client;
        return NULL;
    }
    PrintMessage( client->GetLastResult() + wxT("\n"),HTMLSERVER);

	if (downloadfile->status == STATUS_STOPED){client->Close(); delete client; return NULL;}
    
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
    	PrintMessage( _("File size: ") + int2wxstr(sizetmp) + wxT("\n"),HTMLSERVER);
       	if (downloadpartindex == 0)
    		downloadfile->totalsize =  sizetmp;
    	downloadfile->sizecompleted[downloadpartindex] = start - downloadfile->startpoint[downloadpartindex];
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
	if (downloadfile->split == YES)
		parts = downloadfile->parts;
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
	wxStopWatch time;
	time.Pause();
	time.Start(downloadfile->timepassed);
	long lasttime = time.Time();
	downloadfile->speedpoint = FALSE;
	if (downloadfile->parts > 1)
	{
		bool waitall = TRUE;
		while (waitall)
		{
			int i;
			waitall = FALSE;
			for (i=1; i < downloadfile->parts; i++)
				if (downloadfile->finished[i] == FALSE)
					waitall = TRUE;
			if ((downloadfile->status == STATUS_STOPED) && (canstop))	{return ;}
			Sleep(500);
			if ((downloadfile->status == STATUS_STOPED) && (canstop))	{return ;}
			if (downloadpartindex == 0)
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
			}
		}
	}
}

bool mDownloadThread::JoinFiles()
{
	wxFile *outputfile;
	wxFileName destination;
	wxString str;
	bool result;
	int data_size = 60*READ_BUFFER;
	char *data = new char[data_size];
	long lastread = 0;
	destination.Assign(downloadfile->destination + wxT("/"));
	destination.SetFullName(downloadfile->name);
	str = destination.GetFullPath();
	destination.SetFullName(PREFIX + downloadfile->name);
	if (destination.FileExists())
	{
		wxFileName temp(str);
		wxGetApp().ChangeName(downloadfile,int2wxstr(downloadfile->end.GetTicks()) + wxT(" - ") + downloadfile->name);
		temp.SetFullName(downloadfile->name);
		str = temp.GetFullPath();
	}
	result = wxRenameFile(destination.GetFullPath() + EXT + int2wxstr(0),str);
	if ((downloadfile->split == YES) && (result == TRUE))
	{
		outputfile = new wxFile(str,wxFile::write_append);
		int i;
		for (i=1;i < downloadfile->parts;i++)
		{
			str = destination.GetFullPath() + EXT + int2wxstr(i);
			if (::wxFileExists(str))
			{
				wxFile *piece = new wxFile(destination.GetFullPath() + EXT + int2wxstr(i),wxFile::read);
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
