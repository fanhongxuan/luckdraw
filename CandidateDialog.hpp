#ifndef _MY_CANDIDATE_DIALOG_HPP_
#define _MY_CANDIDATE_DIALOG_HPP_
#include <wx/dialog.h>

class MyFrame;
class wxGrid;
class wxGridEvent;
class MyResultDialog;
class MyCandidate{
public:
	wxString mId;
	wxString mName;
	wxString mDepart;
};

class MyCandidateDialog: public wxDialog
{
public:
	MyCandidateDialog(MyFrame *pFrame);
	~MyCandidateDialog();
	void SetResultDialog(MyResultDialog *pResult){mpResultDialog = pResult;}
	void OnCellRightClick(wxGridEvent &evt);
	void OnLabelRightClick(wxGridEvent &evt);
	void OnSelectCell(wxGridEvent &evt);
	void OnRightUp(wxMouseEvent &evt);
	void OnInsert(wxCommandEvent &evt);
	void OnAppend(wxCommandEvent &evt);
	void OnDelete(wxCommandEvent &evt);
	void OnDeleteAll(wxCommandEvent &evt);
	void OnApply(wxCommandEvent &evt);
	void OnImport(wxCommandEvent &evt);
	void OnExport(wxCommandEvent &evt);
	void OnSelect(wxCommandEvent &evt);
	int GetCandidateCount(const wxString &strPool, bool bIncludeAll);
	void GetCandidateList(const wxString &strPool, std::vector<MyCandidate *> &candidateList, bool bIncludeAll);
protected:
	void LoadFromFile(const wxString &strFileName);
	void SaveToFile(const wxString &strFileName);
	void UpdateRow(int iIndex);
	int GetCurrentLine();
private:
	MyResultDialog *mpResultDialog;
	wxGrid *mpGrid;
	MyFrame *mpFrame;
	int mCurrentLine;
	wxDECLARE_EVENT_TABLE();
};
#endif