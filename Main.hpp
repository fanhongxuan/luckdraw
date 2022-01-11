#ifndef _MY_MAIN_HPP_
#define _MY_MAIN_HPP_
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/artprov.h"
#include "wx/bookctrl.h"
#include "wx/sysopt.h"
#include <wx/dcclient.h>
#include <wx/graphics.h>

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

class MyThread;
class MyResultDialog;
class MyCandidateDialog;
class MyConfigDialog;
class MyActivity;
class MyCandidate;
// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);
	~MyFrame();

    // event handlers (these functions should _not_
	void OnPaint(wxPaintEvent &evt);
	void OnErase(wxEraseEvent &evt);
    void OnFullScreen(wxCommandEvent &evt);
	void OnShowConfig(wxCommandEvent &evt);
	void OnShowCandidate(wxCommandEvent &evt);
	void OnShowResult(wxCommandEvent &evt);
    void OnLeftClick(wxMouseEvent &evt);
	void OnRightClick(wxMouseEvent &evt);
	void OnMouseMove(wxMouseEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnThreadEvent(wxCommandEvent &evt);

	MyThread *GetThread(){return mpWork;}
	void SetCurActivity(MyActivity *pActivity);
	void ShowResult(const wxString &strTitle, std::vector<MyCandidate *> &candidateList);
	int GetCount(){ return mCount;}
	void GetCandidateList(std::vector<MyCandidate *> &candidateList);
private:
	void LoadMedia();
	void DrawTitle(wxDC &dc, const wxString &strTitle);
    void DrawContent(wxGraphicsContext &dc, wxDC &display, std::vector<MyCandidate *> &candidates, const wxString &strTitle);
	void DrawCandidateList(wxGraphicsContext &dc, wxDC &display, std::vector<MyCandidate *> &candidates, wxSize titleSize);
    void DrawMultiCandidateList(wxGraphicsContext &dc, wxDC &display, std::vector<MyCandidate *> &candidates, wxSize titleSize);
	void DrawButton(wxGraphicsContext &dc, const wxString &strLabel);
	wxString GetCandidateByIndex(std::vector<MyCandidate *> &candidates, int iIndex);
	void DoShow();
	void DoStart();
	void DoStop();
	void Prepare();
private:
	int mCount;
	int mPercent;
	wxString mTitle;
	wxString mButtonLabel;
	wxString mCandidateListTitle;
	std::vector<MyCandidate *> mCandidateList;
	wxRect mButtonRect;
	bool mbMouseOnButton;
	wxBitmap *mpBackground;
	std::vector<wxBitmap *> mActivityBitmap;
    std::vector<wxString> mActivityNames;

	MyActivity *mpCurActivity;

	MyThread *mpWork;

    // GUI controls
    MyResultDialog *mpResultDialog;
	MyCandidateDialog *mpCandidateDialog;
	MyConfigDialog *mpConfigDialog;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MENU_TRIGGER_FULLSCREEN= wxID_HIGHEST + 1,
	MENU_SHOW_CONFIG,
	MENU_SHOW_RESULT,
	MENU_SHOW_CANDIDATE,
	EVT_THREAD_MSG,
	MENU_CONFIG_DIALOG_BASE = wxID_HIGHEST + 100,
	MENU_CONFIG_DIALOG_INSERT,
	MENU_CONFIG_DIALOG_APPEND,
	MENU_CONFIG_DIALOG_DELETE,
	MENU_CONFIG_DIALOG_DELETE_ALL,
	MENU_CONFIG_DIALOG_APPLY,
	MENU_CONFIG_DIALOG_EXPORT,
	MENU_CONFIG_DIALOG_IMPORT,
	MENU_CONFIG_DIALOG_SELECT,
	MENU_CANDIDATE_DIALOG_BASE = wxID_HIGHEST + 200,
	MENU_CANDIDATE_DIALOG_INSERT,
	MENU_CANDIDATE_DIALOG_APPEND,
	MENU_CANDIDATE_DIALOG_DELETE,
	MENU_CANDIDATE_DIALOG_DELETE_ALL,
	MENU_CANDIDATE_DIALOG_EXPORT,
	MENU_CANDIDATE_DIALOG_IMPORT,


	MENU_RESULT_DIALOG_BASE = wxID_HIGHEST + 300, 
	MENU_RESULT_DIALOG_DELETE,
	MENU_RESULT_DIALOG_DELETE_GROUP,
	MENU_RESULT_DIALOG_SHOW_GROUP,
	MENU_RESULT_DIALOG_DELETE_ALL,
	MENU_RESULT_DIALOG_REFRESH,
	MENU_RESULT_DIALOG_EXPORT,

};


#endif
