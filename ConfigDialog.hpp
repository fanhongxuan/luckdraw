#ifndef _MY_CONFIG_DIALOG_HPP_
#define _MY_CONFIG_DIALOG_HPP_
#include <wx/dialog.h>

class MyFrame;
class wxGrid;
class wxGridEvent;
class MyCandidate;
class MyActivity
{
public:
	wxString mName;
	wxString mDesc;
	wxString mPool;
	wxString mBitmap;
};

class MyConfigDialog: public wxDialog
{
public:
	MyConfigDialog(MyFrame *pFrame);
	~MyConfigDialog();
	void OnCellRightClick(wxGridEvent &evt);
	void OnLabelRightClick(wxGridEvent &evt);
	void OnLeftDClick(wxGridEvent &evt);
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
	bool IsPool(const wxString &strName, const wxString &strDesc, const wxString &strPool);
	bool IsIncludeAll(const wxString &strName, const wxString &strDesc);
	int GetActivityCount(MyActivity *pAct);
	int GetDefaultStep(MyActivity *pAct);
	int GetLeftNumber(const wxString &strName, const wxString &strDesc);
	void UpdateAllActivity();
	bool UpdateActivity(const wxString &strName, const wxString &strDesc, int iNumber);
	bool UpdateActivity(MyActivity *pAct, std::vector<MyCandidate *> &candidates);
protected:
	void LoadFromFile(const wxString &strFileName);
	void SaveToFile(const wxString &strFileName);
	void UpdateRow(int iIndex);
	int GetCurrentLine();
private:
	wxGrid *mpGrid;
	MyFrame *mpFrame;
	int mCurrentLine;
	int mActiveAct;
	wxDECLARE_EVENT_TABLE();
};
#endif
