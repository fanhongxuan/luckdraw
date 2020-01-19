#include "Main.hpp"
#include "work.hpp"
#include <math.h>
#include "ResultDialog.hpp"
#include "CandidateDialog.hpp"
#include "ConfigDialog.hpp"

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MENU_TRIGGER_FULLSCREEN, MyFrame::OnFullScreen)
	EVT_MENU(MENU_SHOW_CONFIG, MyFrame::OnShowConfig)
	EVT_MENU(MENU_SHOW_CANDIDATE, MyFrame::OnShowCandidate)
	EVT_MENU(MENU_SHOW_RESULT, MyFrame::OnShowResult)
    EVT_LEFT_UP(MyFrame::OnLeftClick)
	EVT_RIGHT_UP(MyFrame::OnRightClick)
	EVT_MOTION(MyFrame::OnMouseMove)
	EVT_ERASE_BACKGROUND(MyFrame::OnErase)
	EVT_PAINT(MyFrame::OnPaint)
	EVT_CLOSE(MyFrame::OnClose)
	EVT_COMMAND(EVT_THREAD_MSG, wxEVT_NULL, MyFrame::OnThreadEvent)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#ifdef __WXMSW__
    if ( argc == 2 && !wxStricmp(argv[1],  wxT("/dx")) )
    {
        wxSystemOptions::SetOption(wxT("msw.display.directdraw"), 1);
    }
#endif // __WXMSW__

    // create the main application window
    MyFrame *frame = new MyFrame(_("�齱"),
                                 wxDefaultPosition, wxDefaultSize);

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show();

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style|wxFULL_REPAINT_ON_RESIZE)
{
	mpWork = NULL;
	mbMouseOnButton = false;
	mPercent = 100;
	mpConfigDialog = NULL;
	mpCandidateDialog = NULL;
	mpResultDialog = NULL;
	mpCurActivity = NULL;
	mpActivityBitmap = NULL;
    // set the frame icon
    SetIcon(wxICON(sample));
    EnableFullScreenView();

	SetMinSize(wxSize(800,600));
	SetSize(800,600);
	LoadMedia();
}

MyFrame::~MyFrame()
{
	if (NULL != mpCurActivity){
		delete mpCurActivity;
		mpCurActivity = NULL;
	}
	if (NULL != mpBackground){
		delete mpBackground;
		mpBackground = NULL;
	}
	if (NULL != mpActivityBitmap){
		delete mpActivityBitmap;
		mpActivityBitmap = NULL;
	}
}

void MyFrame::OnClose(wxCloseEvent &evt)
{
	Hide();
	if (NULL != mpWork){
		mpWork->Quit();
		while(!mpWork->HasFinish()){
			wxMilliSleep(100);
		}
	}
	Destroy();
}

void MyFrame::LoadMedia()
{
	wxInitAllImageHandlers();
	wxImage img(".\\media\\default.jpg");
	mpBackground = new wxBitmap(img);

	FILE *fp = fopen("./media/title.txt", "r");
	if (NULL == fp){
		mTitle = "���޸�./media/title.txt";
	}
	else{
		char line[10240];
		fgets(line, 10240, fp);
		mTitle = line;
		fclose(fp);
	}
	mButtonLabel = "��ϲ";
	
	if (NULL == mpConfigDialog){
		mpConfigDialog = new MyConfigDialog(this);
	}
	if (NULL == mpResultDialog){
		mpResultDialog = new MyResultDialog(this, mpConfigDialog);
	}
	if (NULL == mpCandidateDialog){
		mpCandidateDialog = new MyCandidateDialog(this);
	}
	mpCandidateDialog->SetResultDialog(mpResultDialog);
	if (NULL == mpWork){
		mpWork = new MyThread(this, mpCandidateDialog);
		mpWork->Run();
	}
}

void MyFrame::SetCurActivity(MyActivity *pActivity, int iCurCount)
{
	if (NULL == pActivity){
		return;
	}
	if (NULL != mpWork && mpWork->IsStart()){
		return;
	}
	if (NULL != mpCurActivity){
		delete mpCurActivity;
		mpCurActivity = NULL;
	}
	mpCurActivity = pActivity;
	mCount = iCurCount;
	if (NULL != mpActivityBitmap){
		delete mpActivityBitmap;
		mpActivityBitmap = NULL;
	}
	if (!mpCurActivity->mBitmap.IsEmpty()){
		wxImage img(mpCurActivity->mBitmap);
		mpActivityBitmap = new wxBitmap(img);
	}
	Prepare();
	Refresh();
}

void MyFrame::GetCandidateList(std::vector<MyCandidate *> &candidateList)
{
	return mpCandidateDialog->GetCandidateList(mpCurActivity->mPool, candidateList, mpConfigDialog->IsIncludeAll(mpCurActivity->mName, mpCurActivity->mDesc));
}

void MyFrame::OnFullScreen(wxCommandEvent& WXUNUSED(event))
{
    ShowFullScreen(!IsFullScreen());
}

void MyFrame::OnShowConfig(wxCommandEvent &)
{
	if (NULL != mpConfigDialog){
		mpConfigDialog->Show();
		mpConfigDialog->Raise();
	}
}

void MyFrame::OnShowCandidate(wxCommandEvent &)
{
	if (NULL != mpCandidateDialog){
		mpCandidateDialog->Show();
		mpCandidateDialog->Raise();
	}
}

void MyFrame::OnShowResult(wxCommandEvent &)
{
	if (NULL != mpResultDialog){
		mpResultDialog->Show();
		mpResultDialog->Raise();
	}
}

void MyFrame::Prepare()
{
	// update the candidate count, title, and clear the preview candidateList
	int iCandidateCount = mpCandidateDialog->GetCandidateCount(mpCurActivity->mPool, mpConfigDialog->IsIncludeAll(mpCurActivity->mName, mpCurActivity->mDesc));
	int iLeftNumber = mpConfigDialog->GetLeftNumber(mpCurActivity->mName, mpCurActivity->mDesc);
	mCandidateListTitle = wxString::Format("%s %s (%d/%d)��%d��", mpCurActivity->mName, mpCurActivity->mDesc, mCount, iLeftNumber, iCandidateCount);
	// clear the result.
	if (!mCandidateList.empty()){
		for (int i = 0; i < mCandidateList.size(); i++){
			delete mCandidateList[i];
		}
		mCandidateList.clear();
	}
}

void MyFrame::ShowResult(const wxString &strTitle, std::vector<MyCandidate *> &candidateList)
{
	if (NULL != mpWork && mpWork->IsStart()){
		return;
	}
	mCandidateListTitle = strTitle;
	if (!mCandidateList.empty()){
		for (int i = 0; i < mCandidateList.size(); i++){
			delete mCandidateList[i];
		}
		mCandidateList.clear();
	}
	mCandidateList = candidateList;
	mButtonLabel = "��ϲ";
	Refresh();
}

void MyFrame::DoStart()
{
	if (NULL == mpCurActivity){
		return;
	}
	// update the real count
	mCount = mpConfigDialog->GetActivityCount(mpCurActivity);
	if (mCount <= 0){
		wxMessageBox("�ý����ѳ���");
		return;
	}
	Prepare();
	mButtonLabel = "ֹͣ";
	mpWork->Start();
	mPercent = 100;
	Refresh();
}

void MyFrame::DoStop()
{
	if (NULL == mpCurActivity){
		return;
	}
	mButtonLabel = "��ϲ";
	if (NULL != mpWork){
		mpWork->Stop();
	}
	mPercent = mpConfigDialog->GetDefaultStep(mpCurActivity);

	if (mpConfigDialog->UpdateActivity(mpCurActivity, mCandidateList)){
		mpResultDialog->AddResults(mpCurActivity, mCandidateList);
	}
	Refresh();
	// todo:fanhongxuan@gmail.com
	// 1, fix memory leak issue.
	// 2, show the activity image
	// 3, play audio when select.
}

void MyFrame::DoShow()
{
	if (NULL == mpCurActivity){
		return;
	}
	if (mButtonLabel == "��ϲ"){
		if (mPercent < 100){
			mPercent += 10;
			if (mPercent >= 100){
				mPercent = 100;
			}
			Refresh();
		}
		else if (mPercent = 100){
			mButtonLabel = "��ʼ";
			Prepare();
			Refresh();
		}
	}
}


void MyFrame::OnThreadEvent(wxCommandEvent &evt)
{
	MyEvtPayload *pPayload = (MyEvtPayload*)evt.GetClientData();
	if (NULL != pPayload){
		// update the ui.
		if (NULL != mpWork && (!mpWork->IsStart())){
			// work has been stopped, ignore it and directly return.
			delete pPayload;
			evt.SetClientData(NULL);
			return;
		}
		if (!mCandidateList.empty()){
			for (int i = 0; i < mCandidateList.size(); i++){
				delete mCandidateList[i];
			}
			mCandidateList.clear();
		}
		mCandidateList = pPayload->mCandidateList;
		evt.SetClientData(NULL);
		delete pPayload;
		Refresh();
	}
}

void MyFrame::OnMouseMove(wxMouseEvent &evt){
	bool ret = false;
	if (mButtonRect.Contains(evt.GetX(), evt.GetY())){
		ret = true;
	}
	else{
		ret = false;
	}
	if (ret != mbMouseOnButton){
		mbMouseOnButton = ret;
		Refresh();
	}
}

void MyFrame::OnRightClick(wxMouseEvent &evt)
{
	if (mButtonRect.Contains(evt.GetX(), evt.GetY())){
		return;
	}
	wxMenu menu;
	if (IsFullScreen()){
		menu.Append(MENU_TRIGGER_FULLSCREEN, "�˳�ȫ��");
	}
	else{
		menu.Append(MENU_TRIGGER_FULLSCREEN, "ȫ����ʾ");
	}
	menu.Append(MENU_SHOW_CONFIG, "��������...");
	menu.Append(MENU_SHOW_CANDIDATE, "��Ա����...");
	menu.Append(MENU_SHOW_RESULT, "�齱���...");
	PopupMenu(&menu);
}

void MyFrame::OnLeftClick(wxMouseEvent &evt)
{
	int x = evt.GetX();
	int y = evt.GetY();
	if (mButtonRect.Contains(x, y)){
		if (mButtonLabel == "��ϲ"){
			DoShow();
		}
		else if(mButtonLabel == "��ʼ"){
			DoStart();
		}
		else{
			DoStop();
		}
		Refresh();
	}
	else{
		DoShow();
	}
}

void MyFrame::OnErase(wxEraseEvent &evt)
{
	// do nothing.
}

void MyFrame::DrawTitle(wxDC &dc, const wxString &strTitle)
{
	wxSize size = GetSize();
	// 1/10
	// select the right font according the size
	wxFont font;
	font.SetPixelSize(wxSize(0, size.GetHeight()/10));
	font.MakeBold();
	dc.SetFont(font);
	wxSize textSize = dc.GetTextExtent(strTitle);
	dc.SetTextBackground(*wxWHITE);
	dc.SetTextForeground(*wxWHITE);
	dc.DrawText(strTitle, (size.GetWidth() - textSize.GetWidth())/2, size.GetHeight()/20);
}

wxString MyFrame::GetCandidateByIndex(std::vector<MyCandidate *> &candidates, int iIndex)
{
	wxString ret;
	if (iIndex >= candidates.size()){
		return ret;
	}
	MyCandidate* pRet = candidates[iIndex];
	if (NULL == pRet){
		return ret;
	}
	ret = pRet->mId + " " + pRet->mName;
	if (mPercent >= 100){
		return ret;
	}
	// replace the char by *;
	int iCount = ret.Length() * (100 - mPercent) / 100;
	int iLen = ret.Length();
	for (int i = 1; i <= iCount; i++){
		if ((iLen - i) >= 0){
			ret[iLen - i] = '*';
		}
		else{
			break;
		}
	}
	return ret;
}

void MyFrame::DrawCandidateList(wxGraphicsContext &dc, wxDC &display, std::vector<MyCandidate *> &candidates, const wxString &strTitle){
	// draw the total candidates
	// 8/10
	wxSize size = GetSize();
	// common settings
	double dXStart = 0.1, dYStart = 0.2, dWidth = 0.8, dHeight = 0.6, dTitleHeight = 0.1;
	wxColour titleBackgroundColour = wxColour(243,96,20, 160);
	wxColour backgroundColour = wxColour(255,255,255,80);
	wxColour textColour = *wxBLACK;

	// 1, draw the total background
	dc.SetBrush(wxBrush(backgroundColour));
	dc.SetPen(wxPen(backgroundColour));
	dc.DrawRoundedRectangle(size.GetWidth() * dXStart, size.GetHeight() * dYStart, size.GetWidth() * dWidth, size.GetHeight() * dHeight, 5);

	// 2, draw title background
	wxSize titleSize(size.GetWidth() * dWidth, size.GetHeight() * dTitleHeight);
	dc.SetPen(wxPen(titleBackgroundColour));
	dc.SetBrush(wxBrush(titleBackgroundColour));
	dc.DrawRoundedRectangle(size.GetWidth() * dXStart, size.GetHeight() * dYStart, titleSize.GetWidth(), titleSize.GetHeight(), 5);

	// 3, draw the title
	wxFont font;
	font.SetPixelSize(wxSize(0, titleSize.GetHeight() * 0.6));
	font.MakeBold();
	dc.SetFont(dc.CreateFont(font, textColour));
	double textWidth = 0;
	dc.GetTextExtent(strTitle, &textWidth, NULL);
	dc.DrawText(strTitle, (size.GetWidth() - textWidth)/2, size.GetHeight() * dYStart + titleSize.GetHeight() * 0.2);

	// draw the canidate list.
	int iCount = candidates.size();
	if (iCount <= 0){
		if (NULL != mpActivityBitmap && NULL != mpWork && (!mpWork->IsStart())){
			wxMemoryDC mem(*mpActivityBitmap);
			int height = size.GetHeight() * 0.4, width = size.GetWidth() * 0.7;
			double ratio = (double)mpActivityBitmap->GetWidth()/(double)mpActivityBitmap->GetHeight();
			if ((height * ratio) > width){
				height = width * (double)mpActivityBitmap->GetWidth()/(double)mpActivityBitmap->GetHeight();
			}
			else{
				width = height * ratio;
			}
			display.StretchBlit(size.GetWidth() * 0.5 - width/2, size.GetHeight() * 0.31, width, height, &mem, 
		                         0, 0, mpActivityBitmap->GetWidth(), mpActivityBitmap->GetHeight());
		}
		return;
	}
	// max has 13 row, 4 column
	if (iCount > (13*4)){
		// too much
		return;
	}

	// int iColumn = iCount/8+1, iRow = iCount/iColumn;
	int iColumn = 1, iRow = iCount;
	if (iCount <= 2){
		iColumn = 1;
	}
	else if (iCount <= 12){
		iColumn = 2;
	}
	else if (iCount <= 24){
		iColumn = 3;
	}
	else{
		iColumn = 4;
	}
	iRow = iCount /iColumn;
	if (iCount%iColumn){
		iRow++;
	}

	// draw multi lines here.
	int startX = 0, startY = 0, xStep = 0, yStep = 0;
	int iIndex = 0;
	startX = size.GetWidth() * 0.15;
	startY = size.GetHeight() * 0.3;
	yStep = size.GetHeight() * 0.4 / iRow;
	xStep = size.GetWidth() * 0.7 / iColumn;

	wxFont candidateFont;
	if (yStep > titleSize.GetHeight()){
		candidateFont.SetPixelSize(wxSize(0, titleSize.GetHeight() * 0.6));
	}
	else{
		candidateFont.SetPixelSize(wxSize(0, yStep * 0.6));
	}
	candidateFont.MakeBold();
	dc.SetFont(dc.CreateFont(candidateFont, textColour));
	for (int i = 0; i < iRow; i++){
		for (int j = 0; j < iColumn; j++){
			wxString text = GetCandidateByIndex(candidates, iIndex);
			double textWidth = 0;
			dc.GetTextExtent(text, &textWidth, NULL);
			if (text.Length() > 0){
				dc.DrawText(text, (startX + xStep * j) + (xStep - textWidth)/2, startY + yStep * i);
			}
			iIndex++;
			if (iIndex >= iCount){
				break;
			}
		}
	}
}

void MyFrame::DrawButton(wxGraphicsContext &dc, const wxString &strTitle)
{
	wxSize size = GetSize();
	// common settings
	double dXStart = 0.4, dYStart = 0.82, dWidth = 0.2, dHeight = 0.08, dLabelHeight = 0.6;
	wxColour labelBackgroundColour;
	if (mbMouseOnButton){
		labelBackgroundColour = wxColour(199,220,42, 160);
	}
	else{
		labelBackgroundColour = wxColour(255,124,0, 160);
	}
	wxColour textColour = *wxBLACK;

	// 1, draw the background
	dc.SetBrush(wxBrush(labelBackgroundColour));
	dc.SetPen(wxPen(textColour));
	mButtonRect.SetX(size.GetWidth() * dXStart);
	mButtonRect.SetY(size.GetHeight() * dYStart);
	mButtonRect.SetWidth(size.GetWidth() * dWidth);
	mButtonRect.SetHeight(size.GetHeight() * dHeight);
	dc.DrawRoundedRectangle(mButtonRect.GetX(), mButtonRect.GetY(), mButtonRect.GetWidth(), mButtonRect.GetHeight(), 5);

	// 3, draw the label
	wxFont font;
	font.SetPixelSize(wxSize(0, size.GetHeight() * dHeight* dLabelHeight ));
	font.MakeBold();
	dc.SetFont(dc.CreateFont(font, textColour));
	double textWidth = 0, textHeight = 0;
	dc.GetTextExtent(strTitle, &textWidth, &textHeight);
	dc.DrawText(strTitle, (size.GetWidth() - textWidth)/2, mButtonRect.GetY() + (mButtonRect.GetHeight() - textHeight)/2);
}

void MyFrame::OnPaint(wxPaintEvent &evt)
{
	wxPaintDC display(this);
	if (NULL == mpBackground){
		return;
	}
	wxSize size = GetSize();
	wxBitmap buff(size.GetWidth(), size.GetHeight());
	wxMemoryDC dc(buff);
	
	// draw background
	wxMemoryDC background(*mpBackground);
	dc.StretchBlit(0, 0, size.GetWidth(), size.GetHeight(), &background, 
		                   0, 0, mpBackground->GetWidth(), mpBackground->GetHeight());

	DrawTitle(dc, mTitle);
	wxGraphicsContext *pCtx = wxGraphicsContext::Create(dc);
	if (NULL != pCtx){
		DrawCandidateList(*pCtx, dc,mCandidateList, mCandidateListTitle);
		DrawButton(*pCtx, mButtonLabel);
		delete pCtx;
	}
	// draw the entile display in one time
	display.Blit(0, 0, size.GetWidth(), size.GetHeight(), &dc,0, 0);
	// mem.DrawBitmap(*mpBackground, 0, 0);
}
