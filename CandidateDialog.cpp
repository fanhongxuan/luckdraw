#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/dynarray.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include "Main.hpp"
#include "work.hpp"
#include "CandidateDialog.hpp"
#include "ResultDialog.hpp"

wxBEGIN_EVENT_TABLE(MyCandidateDialog, wxDialog)
    EVT_GRID_CELL_RIGHT_CLICK(MyCandidateDialog::OnCellRightClick)
	EVT_GRID_LABEL_RIGHT_CLICK(MyCandidateDialog::OnLabelRightClick)
	EVT_GRID_SELECT_CELL(MyCandidateDialog::OnSelectCell)
	EVT_MENU(MENU_CANDIDATE_DIALOG_INSERT, MyCandidateDialog::OnInsert)
	EVT_MENU(MENU_CANDIDATE_DIALOG_DELETE, MyCandidateDialog::OnDelete)
	EVT_MENU(MENU_CANDIDATE_DIALOG_DELETE_ALL, MyCandidateDialog::OnDeleteAll)
	EVT_MENU(MENU_CANDIDATE_DIALOG_APPEND, MyCandidateDialog::OnAppend)
	EVT_MENU(MENU_CANDIDATE_DIALOG_EXPORT, MyCandidateDialog::OnExport)
	EVT_MENU(MENU_CANDIDATE_DIALOG_IMPORT, MyCandidateDialog::OnImport)
	EVT_RIGHT_UP(MyCandidateDialog::OnRightUp)
wxEND_EVENT_TABLE()

#define ID_COL (0)
#define NAME_COL  (1)
#define DESC_COL (2)
#define STATUS_COL  (3)
#define COL_COUNT (4)
MyCandidateDialog::MyCandidateDialog(MyFrame *pFrame)
	:wxDialog(pFrame, wxID_ANY, "人员设置", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	mpFrame = pFrame;
	mpGrid = new wxGrid(this, wxID_ANY);
	// mpGrid->SetCellHighlightPenWidth(0);
	// mpGrid->EnableEditing(false);
	mpGrid->CreateGrid(0, COL_COUNT, wxGrid::wxGridSelectRows);
	mpGrid->SetColLabelValue(ID_COL, "工号");
	mpGrid->SetColFormatNumber(ID_COL);
	mpGrid->SetColLabelValue(NAME_COL, "姓名");
	mpGrid->SetColLabelValue(DESC_COL, "部门");
	mpGrid->SetColLabelValue(STATUS_COL, "中奖状态");
	SetSize(500,300);
	mCurrentLine = -1;
	LoadFromFile("./media/employeelist.txt");
}

MyCandidateDialog::~MyCandidateDialog(){
	SaveToFile("./media/employeelist.txt");
}

void MyCandidateDialog::UpdateRow(int iIndex){
	if (iIndex >= mpGrid->GetNumberRows()){
		return;
	}
	mpGrid->SetReadOnly(iIndex, STATUS_COL);
}

void MyCandidateDialog::OnCellRightClick(wxGridEvent &evt){
	wxMenu menu;
	menu.Append(MENU_CANDIDATE_DIALOG_INSERT, "插入");
	menu.Append(MENU_CANDIDATE_DIALOG_DELETE, "删除");
	menu.Append(MENU_CANDIDATE_DIALOG_DELETE_ALL, "清空列表");
	menu.Append(MENU_CANDIDATE_DIALOG_IMPORT, "导入...");
	menu.Append(MENU_CANDIDATE_DIALOG_EXPORT, "导出...");
	PopupMenu(&menu);
}

void MyCandidateDialog::OnRightUp(wxMouseEvent &evt){
	wxMenu menu;
	menu.Append(MENU_CANDIDATE_DIALOG_APPEND, "添加");
	menu.Append(MENU_CANDIDATE_DIALOG_DELETE_ALL, "清空列表");
	menu.Append(MENU_CANDIDATE_DIALOG_IMPORT, "导入...");
	menu.Append(MENU_CANDIDATE_DIALOG_EXPORT, "导出...");
	PopupMenu(&menu);
}

void MyCandidateDialog::OnLabelRightClick(wxGridEvent &evt){
	wxMenu menu;
	menu.Append(MENU_CANDIDATE_DIALOG_APPEND, "添加");
	menu.Append(MENU_CANDIDATE_DIALOG_DELETE_ALL, "清空列表");
	menu.Append(MENU_CANDIDATE_DIALOG_IMPORT, "导入...");
	menu.Append(MENU_CANDIDATE_DIALOG_EXPORT, "导出...");
	PopupMenu(&menu);
}

void MyCandidateDialog::OnSelectCell(wxGridEvent &evt)
{
	mCurrentLine = evt.GetRow();
}

int MyCandidateDialog::GetCurrentLine()
{
	return mCurrentLine;
}

void MyCandidateDialog::SaveToFile(const wxString &strFileName){
	wxString ret;
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		for (int j = 0; j < mpGrid->GetNumberCols(); j++){
			ret += mpGrid->GetCellValue(i, j);
			ret += "\t";
		}
		ret += "\r\n";
	}
	FILE *fp = fopen(strFileName.c_str(), "w");
	if (NULL == fp){
		return;
	}
	fwrite(ret.c_str(), 1, strlen(ret.c_str()), fp);
	fclose(fp);
}

void MyCandidateDialog::LoadFromFile(const wxString &strFileName)
{
	FILE *fp = fopen(strFileName.c_str(), "r");
	if (NULL == fp){
		return;
	}

	char line[1024 * 10];
	int iRow = 0;
	while(fgets(line, 1024 * 10, fp)){
		int iLen = strlen(line);
		iRow = mpGrid->GetNumberRows();
		mpGrid->AppendRows();
		std::string text;
		int iCol = 0;
		for (int i = 0; i < iLen; i++){
			if (line[i] == '\t' || line[i] == '\r' || line[i] == '\n'){
				if (iCol >= COL_COUNT){
					break;
				}
				mpGrid->SetCellValue(iRow, iCol, text);
				text = "";
				iCol++;
			}
			else{
				text += line[i];
			}
		}
		UpdateRow(iRow);
		iRow++;
	}

	if (mpGrid->GetNumberRows() != 0){
		mpGrid->SetGridCursor(mpGrid->GetNumberRows() - 1, 0);
		mpGrid->SelectRow(mpGrid->GetNumberRows() - 1);
	}
	fclose(fp);
}

void MyCandidateDialog::OnImport(wxCommandEvent &evt)
{
	 wxFileDialog dlg(this, _("从文件导入"), "", "employeelist.txt",
						         "txt files (*.txt)|*.txt",
								  wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	 if (dlg.ShowModal() == wxID_CANCEL){
		 return;
	 }
	 LoadFromFile(dlg.GetPath());
}

void MyCandidateDialog::OnExport(wxCommandEvent &evt)
{
	 wxFileDialog dlg(this, _("导出到文件"), "", "employeelist.txt",
						         "txt files (*.txt)|*.txt",
								 wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	 if (dlg.ShowModal() == wxID_CANCEL){
		 return;
	 }
	 SaveToFile(dlg.GetPath());
}

void MyCandidateDialog::OnInsert(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur <= 0){
		mpGrid->InsertRows(0, 1);
	}
	else{
		mpGrid->InsertRows(iCur, 1);
	}

	UpdateRow(iCur);
	mpGrid->SetGridCursor(iCur, ID_COL);
	mpGrid->SelectRow(iCur);
}

void MyCandidateDialog::OnAppend(wxCommandEvent &evt)
{
	int iCount = mpGrid->GetNumberRows();
	mpGrid->AppendRows();
	UpdateRow(iCount);
	mpGrid->SetGridCursor(iCount, ID_COL);
	mpGrid->SelectRow(iCount);
}

void MyCandidateDialog::OnDelete(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur < 0){
		return;
	}
	mpGrid->DeleteRows(iCur, 1);
}

void MyCandidateDialog::OnDeleteAll(wxCommandEvent &evt)
{
	int answer = wxMessageBox("确认清空员工列表？","确认",wxYES_NO | wxCANCEL, this);
	if (answer != wxYES){
		return;
	}
	mpGrid->DeleteRows(0, mpGrid->GetNumberRows());
}

int MyCandidateDialog::GetCandidateCount(const wxString &strPool, bool bIncludeAll)
{
	int iRet = 0;
	if (bIncludeAll){
		return mpGrid->GetNumberRows();
	}
	std::set<wxString> records;
	mpResultDialog->GetResultSet(strPool, records);
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (records.find(mpGrid->GetCellValue(i, ID_COL)) != records.end()){
			continue;
		}
		iRet++;
	}
	return iRet;
}

void MyCandidateDialog::GetCandidateList(const wxString &strPool, std::vector<MyCandidate *> &candidateList, bool bIncludeAll)
{
	std::set<wxString> records;
	if (!bIncludeAll){
		mpResultDialog->GetResultSet(strPool, records);
	}
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (!bIncludeAll){
			if (records.find(mpGrid->GetCellValue(i, ID_COL)) != records.end()){
				continue;
			}
		}
		MyCandidate *pCandidate = new MyCandidate;
		pCandidate->mId = mpGrid->GetCellValue(i, ID_COL);
		pCandidate->mName  = mpGrid->GetCellValue(i, NAME_COL);
		pCandidate->mDepart = mpGrid->GetCellValue(i, DESC_COL);
		candidateList.push_back(pCandidate);
	}
}