#ifndef _MY_RESULT_DIALOG_HPP_
#define _MY_RESULT_DIALOG_HPP_
#include <wx/dialog.h>
#include <set>
class MyFrame;
class MyCandidate;
class wxGrid;
class wxGridEvent;
class MyConfigDialog;
class MyResultDialog: public wxDialog
{
public:
	MyResultDialog(MyFrame *pFrame, MyConfigDialog *pConfig);
	~MyResultDialog();
	void OnCellRightClick(wxGridEvent &evt);
	void OnLabelRightClick(wxGridEvent &evt);
	void OnCellChanged(wxGridEvent &evt);
	void OnSelectCell(wxGridEvent &evt);
	void OnRightUp(wxMouseEvent &evt);
	void OnDelete(wxCommandEvent &evt);
	void OnDeleteGroup(wxCommandEvent &evt);
	void OnShowGroup(wxCommandEvent &evt);
	void OnDeleteAll(wxCommandEvent &evt);
	void OnExport(wxCommandEvent &evt);
	void OnRefresh(wxCommandEvent &evt);
	void AddResults(MyActivity *pAct, std::vector<MyCandidate *> &resultList);
	void GetResultSet(const wxString &strPool, std::set<wxString > &rets);
	bool HasRecord(const wxString &strId, const wxString &strPool);
protected:
	void LoadFromFile(const wxString &strFileName);
	void SaveToFile(const wxString &strFileName);
	void UpdateRow(int iIndex);
	int GetCurrentLine();
private:
	wxGrid *mpGrid;
	MyFrame *mpFrame;
	MyConfigDialog *mpConfigDialog;
	int mCurrentLine;
	wxDECLARE_EVENT_TABLE();
};
#endif