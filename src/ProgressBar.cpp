//
// C++ Implementation: ProgressBar
//
// Description: Implements the segmented progress bar
//
//
// Author: Max Magalhães Velasques <maxvelasques@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "wxDFast.h"

IMPLEMENT_DYNAMIC_CLASS(mProgressBar, wxPanel)

BEGIN_EVENT_TABLE(mProgressBar, wxPanel)
    EVT_PAINT  (mProgressBar::OnPaint)
END_EVENT_TABLE()

mProgressBar::mProgressBar()
{
    SetParams(0,NULL);
}

void mProgressBar::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    int realwidth, realheight;
    int width, height,x,y;

    //GET THE LIMITS OF THE WINDOW
    this->GetSize(&realwidth,&realheight);
    height = realheight-2;
    width = realwidth-2;
    x = 1;
    y = 1;

    //CLEAR THE PROGRESS BAR AREA
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(x,y,width,height);

    if (parts > 0)
    {
        float partssize = ((float)width)/((float)parts);
        float scale = ((float)width)/((float)100);
        int tmpwidth;
        for (int i = 0 ; i < parts ; i++)
        {
            tmpwidth = (int)(completed[i]*scale);
            dc.SetBrush(wxBrush(LIGHT_BLUE));
            dc.SetPen(wxPen(LIGHT_BLUE));
            dc.DrawRectangle(x+((int)(partssize))*i,y,tmpwidth,height);
        }
    }

    //DRAW THE PROGRESS BAR BORDER
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(x,y,width,height);

}

bool mProgressBar::Hide()
{
    return Show(false);
}

bool mProgressBar::Show(bool show)
{
    wxSplitterWindow *splitter = XRCCTRL(*mainframe, "splitter01",wxSplitterWindow);
    int width,height;
    bool value = FALSE;
    if (show)
    {
        this->SetBestFittingSize(wxSize(200,30));
        splitter->GetSize(&width,&height);
        this->SetSize(wxSize(width,30));
        if (!IsShown())
        {
            splitter->SetSize(wxSize(width,height-30-5));
            value = wxPanel::Show(TRUE);
            //this->Refresh();
        }
    }
    else if (IsShown())
    {
        value = wxPanel::Show(FALSE);
        this->SetBestFittingSize(wxSize(200,0));
        splitter->GetSize(&width,&height);
        splitter->SetSize(wxSize(width,height+30+5));
    }
    return value;
}

void mProgressBar::SetParams(int parts,int *completed)
{
    this->parts = parts;
    for (int i=0; i < parts; i++)
        this->completed[i] = completed[i];
}

void mProgressBar::SetMainFrame(mMainFrame *mainframe)
{
    this->mainframe = mainframe;
}

