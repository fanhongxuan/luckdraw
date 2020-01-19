#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/dynarray.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include "Main.hpp"
#include "work.hpp"
#include "ResultDialog.hpp"
#include "ConfigDialog.hpp"
#include "CandidateDialog.hpp"

wxBEGIN_EVENT_TABLE(MyResultDialog, wxDialog)
    EVT_GRID_CELL_RIGHT_CLICK(MyResultDialog::OnCellRightClick)
	EVT_GRID_LABEL_RIGHT_CLICK(MyResultDialog::OnLabelRightClick)
	EVT_GRID_CELL_CHANGED(MyResultDialog::OnCellChanged)
	EVT_GRID_SELECT_CELL(MyResultDialog::OnSelectCell)
	EVT_MENU(MENU_RESULT_DIALOG_DELETE, MyResultDialog::OnDelete)
	EVT_MENU(MENU_RESULT_DIALOG_DELETE_GROUP, MyResultDialog::OnDeleteGroup)
	EVT_MENU(MENU_RESULT_DIALOG_SHOW_GROUP, MyResultDialog::OnShowGroup)
	EVT_MENU(MENU_RESULT_DIALOG_DELETE_ALL, MyResultDialog::OnDeleteAll)
	EVT_MENU(MENU_RESULT_DIALOG_EXPORT, MyResultDialog::OnExport)
	EVT_RIGHT_UP(MyResultDialog::OnRightUp)
wxEND_EVENT_TABLE()

#define ID_COL (0)
#define NAME_COL  (1)
#define DEPART_COL (2)
#define ACTIVITY_COL (3)
#define ACTIVITY_DETAIL_COL (4)
#define STATUS_COL  (5)
#define COL_COUNT (6)
MyResultDialog::MyResultDialog(MyFrame *pFrame, MyConfigDialog *pConfigDialog)
	:wxDialog(pFrame, wxID_ANY, "抽奖结果", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	mpFrame = pFrame;
	mpConfigDialog = pConfigDialog;
	mpGrid = new wxGrid(this, wxID_ANY);
	// mpGrid->SetCellHighlightPenWidth(0);
	// mpGrid->EnableEditing(false);
	mpGrid->CreateGrid(0, COL_COUNT, wxGrid::wxGridSelectRows);
	mpGrid->SetColLabelValue(ID_COL, "工号");
	mpGrid->SetColFormatNumber(ID_COL);
	mpGrid->SetColLabelValue(NAME_COL, "姓名");
	mpGrid->SetColLabelValue(DEPART_COL, "部门");
	mpGrid->SetColLabelValue(ACTIVITY_COL, "奖项");
	mpGrid->SetColLabelValue(ACTIVITY_DETAIL_COL, "奖品");
	mpGrid->SetColLabelValue(STATUS_COL, "是否领取");
	mpGrid->SetColFormatBool(STATUS_COL);
	
	SetSize(600,300);

	mCurrentLine = -1;
	LoadFromFile("./media/result.txt");
}

MyResultDialog::~MyResultDialog(){
	SaveToFile("./media/result.txt");
}

void MyResultDialog::UpdateRow(int iIndex){
	if (iIndex >= mpGrid->GetNumberRows()){
		return;
	}
	wxGridCellAttr *pAttr = new wxGridCellAttr;
	pAttr->SetReadOnly();
	pAttr->SetTextColour(*wxBLACK);
	if (mpGrid->GetCellValue(iIndex, STATUS_COL) == "1"){
		pAttr->SetTextColour(*wxLIGHT_GREY);
	}
	mpGrid->SetRowAttr(iIndex, pAttr);
	for (int i = 0; i < COL_COUNT; i++){
		mpGrid->SetReadOnly(iIndex, i);
	}
	if (mpGrid->GetCellValue(iIndex, NAME_COL).IsEmpty()){
		mpGrid->SetCellSize(iIndex, 0, 1, COL_COUNT);
		mpGrid->SetCellAlignment(iIndex, 0, wxALIGN_LEFT, wxALIGN_CENTER);
	}
	
	mpGrid->SetReadOnly(iIndex, STATUS_COL, false);
}

void MyResultDialog::OnCellRightClick(wxGridEvent &evt){
	wxMenu menu;
	if (mpGrid->GetCellValue(evt.GetRow(), NAME_COL).IsEmpty()){
		bool bTake = false;
		int i = 0;
		for (i = evt.GetRow()+1; i < mpGrid->GetNumberRows(); i++){
			if (mpGrid->GetCellValue(i, NAME_COL).IsEmpty()){
				break;
			}
			if (mpGrid->GetCellValue(i, STATUS_COL) == "1"){
				bTake = true;
				break;
			}
		}
		menu.Append(MENU_RESULT_DIALOG_SHOW_GROUP, "显示结果");
		if (!bTake){
			menu.Append(MENU_RESULT_DIALOG_DELETE_GROUP, "本组弃奖");
		}
	}
	else{
		if (mpGrid->GetCellValue(evt.GetRow(), STATUS_COL) != "1"){
			menu.Append(MENU_RESULT_DIALOG_DELETE, "弃奖");
		}
	}
	menu.Append(MENU_RESULT_DIALOG_EXPORT, "导出");
	menu.Append(MENU_RESULT_DIALOG_DELETE_ALL, "清空抽奖结果");
	PopupMenu(&menu);
}

void MyResultDialog::OnRightUp(wxMouseEvent &evt){
}

void MyResultDialog::OnLabelRightClick(wxGridEvent &evt){
	wxMenu menu;
	menu.Append(MENU_RESULT_DIALOG_EXPORT, "导出");
	menu.Append(MENU_RESULT_DIALOG_DELETE_ALL, "清空抽奖结果");
	PopupMenu(&menu);
}

void MyResultDialog::OnSelectCell(wxGridEvent &evt)
{
	mCurrentLine = evt.GetRow();
	UpdateRow(evt.GetRow());
}

void MyResultDialog::OnCellChanged(wxGridEvent &evt)
{
	UpdateRow(evt.GetRow());
	mpGrid->Refresh();
}

int MyResultDialog::GetCurrentLine()
{
	return mCurrentLine;
}

void MyResultDialog::SaveToFile(const wxString &strFileName){
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

void MyResultDialog::LoadFromFile(const wxString &strFileName)
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

void MyResultDialog::OnExport(wxCommandEvent &evt)
{
	 wxFileDialog dlg(this, _("导出到文件"), "", "result.txt",
						         "txt files (*.txt)|*.txt",
								 wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	 if (dlg.ShowModal() == wxID_CANCEL){
		 return;
	 }
	 SaveToFile(dlg.GetPath());
}

void MyResultDialog::OnDelete(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur < 1){
		return;
	}
	mpConfigDialog->UpdateActivity(mpGrid->GetCellValue(iCur, ACTIVITY_COL), mpGrid->GetCellValue(iCur, ACTIVITY_DETAIL_COL), -1);
	mpGrid->DeleteRows(iCur, 1);
	for (int i = iCur-1; i >= 0; i--){
		if (mpGrid->GetCellValue(i, NAME_COL).IsEmpty()){
			// this is the title line.
			wxString strCount = mpGrid->GetCellValue(i, DEPART_COL);
			long iCount = 0;
			strCount.ToLong(&iCount);
			iCount--;
			if (iCount <= 0){
				mpGrid->DeleteRows(i, 1);
				return;
			}
			wxString group = wxString::Format("%s (%s)共%d人", mpGrid->GetCellValue(i, ACTIVITY_COL), mpGrid->GetCellValue(i, ACTIVITY_DETAIL_COL), iCount);
			mpGrid->SetCellValue(i, ID_COL, group);
			mpGrid->SetCellValue(i, DEPART_COL, wxString::Format("%d", iCount));
			break;
		}
	}
}

void MyResultDialog::OnDeleteGroup(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur < 0){
		return;
	}
	int answer = wxMessageBox("确认本组弃奖？","确认",wxYES_NO | wxCANCEL, this);
	if (answer != wxYES){
		return;
	}
	if (mpGrid->GetCellValue(iCur, NAME_COL).IsEmpty()){
		wxString strCount = mpGrid->GetCellValue(iCur, DEPART_COL);
		long iCount = 0;
		strCount.ToLong(&iCount);
		mpConfigDialog->UpdateActivity(mpGrid->GetCellValue(iCur, ACTIVITY_COL), mpGrid->GetCellValue(iCur, ACTIVITY_DETAIL_COL), -iCount);
		mpGrid->DeleteRows(iCur, iCount+1);
	}
}

void MyResultDialog::OnDeleteAll(wxCommandEvent &evt)
{
	int answer = wxMessageBox("确认清空抽奖结果？","确认",wxYES_NO | wxCANCEL, this);
	if (answer != wxYES){
		return;
	}
	mpConfigDialog->UpdateAllActivity();
	mpGrid->DeleteRows(0, mpGrid->GetNumberRows());
}

void MyResultDialog::OnShowGroup(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur < 0){
		return;
	}
	if (!mpGrid->GetCellValue(iCur, NAME_COL).IsEmpty()){
		return;
	}
	// call ShowResult
	wxString strCount = mpGrid->GetCellValue(iCur, DEPART_COL);
	long iCount = 0;
	strCount.ToLong(&iCount);
	wxString strTitle = wxString::Format("%s %s 共%d人", 
		mpGrid->GetCellValue(iCur, ACTIVITY_COL), 
		mpGrid->GetCellValue(iCur, ACTIVITY_DETAIL_COL), iCount);
	std::vector<MyCandidate *> candidateList;
	for (int i = 0; i < iCount; i++){
		MyCandidate *pRet = new MyCandidate;
		pRet->mId = mpGrid->GetCellValue(iCur + 1 + i, ID_COL);
		pRet->mName = mpGrid->GetCellValue(iCur+1 +i, NAME_COL);
		pRet->mDepart = mpGrid->GetCellValue(iCur + 1 + i, DEPART_COL);
		candidateList.push_back(pRet);
	}
	mpFrame->ShowResult(strTitle, candidateList);
}


void MyResultDialog::AddResults(MyActivity *pAct, std::vector<MyCandidate *> &resultList)
{
	if (resultList.empty() || NULL == pAct){
		return;
	}
	// todo:fanhongxuan@gmail.com
	// find the right position to add the new result
	int iIndex = mpGrid->GetNumberRows();
	mpGrid->AppendRows(1);
	wxString group = wxString::Format("%s (%s)共%d人", pAct->mName, pAct->mDesc, resultList.size());
	mpGrid->SetCellValue(iIndex, ID_COL, group);
	mpGrid->SetCellValue(iIndex, DEPART_COL, wxString::Format("%d", resultList.size()));
	mpGrid->SetCellValue(iIndex, ACTIVITY_COL, pAct->mName);
	mpGrid->SetCellValue(iIndex, ACTIVITY_DETAIL_COL, pAct->mDesc);
	UpdateRow(iIndex);

	for (int i = 0; i < resultList.size(); i++){
		MyCandidate *pCan = resultList[i];
		if (NULL == pCan){
			continue;
		}
		iIndex = mpGrid->GetNumberRows();
		mpGrid->AppendRows();
		mpGrid->SetCellValue(iIndex, ID_COL,pCan->mId);
		mpGrid->SetCellValue(iIndex, NAME_COL, pCan->mName);
		mpGrid->SetCellValue(iIndex, DEPART_COL, pCan->mDepart);
		mpGrid->SetCellValue(iIndex, ACTIVITY_COL, pAct->mName);
		mpGrid->SetCellValue(iIndex, ACTIVITY_DETAIL_COL, pAct->mDesc);
		mpGrid->SetCellValue(iIndex, STATUS_COL, "0");
		UpdateRow(iIndex+i);
	}
	if (IsShown()){
		Refresh();
	}
	SaveToFile("./media/result.txt");
}

void MyResultDialog::GetResultSet(const wxString &strPool, std::set<wxString> &rets)
{
	rets.empty();
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		// this is a group title and not in current strPool
		if (mpGrid->GetCellValue(i, NAME_COL).IsEmpty() && (!mpConfigDialog->IsPool(mpGrid->GetCellValue(i, ACTIVITY_COL), mpGrid->GetCellValue(i, ACTIVITY_DETAIL_COL), strPool))){
			wxString strCount = mpGrid->GetCellValue(i, DEPART_COL);
			long iCount = 0;
			strCount.ToLong(&iCount);
			i += iCount;
			continue;
		}
		rets.insert(mpGrid->GetCellValue(i, ID_COL));
	}
}
