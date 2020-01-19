#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/dynarray.h>
#include <wx/msgdlg.h>
#include "Main.hpp"
#include "work.hpp"
#include "ConfigDialog.hpp"

wxBEGIN_EVENT_TABLE(MyConfigDialog, wxDialog)
    EVT_GRID_CELL_RIGHT_CLICK(MyConfigDialog::OnCellRightClick)
	EVT_GRID_LABEL_RIGHT_CLICK(MyConfigDialog::OnLabelRightClick)
	EVT_GRID_CELL_LEFT_DCLICK(MyConfigDialog::OnLeftDClick)
	EVT_GRID_SELECT_CELL(MyConfigDialog::OnSelectCell)
	EVT_MENU(MENU_CONFIG_DIALOG_INSERT, MyConfigDialog::OnInsert)
	EVT_MENU(MENU_CONFIG_DIALOG_DELETE, MyConfigDialog::OnDelete)
	EVT_MENU(MENU_CONFIG_DIALOG_DELETE_ALL, MyConfigDialog::OnDeleteAll)
	EVT_MENU(MENU_CONFIG_DIALOG_APPEND, MyConfigDialog::OnAppend)
	EVT_MENU(MENU_CONFIG_DIALOG_EXPORT, MyConfigDialog::OnExport)
	EVT_MENU(MENU_CONFIG_DIALOG_IMPORT, MyConfigDialog::OnImport)
	EVT_MENU(MENU_CONFIG_DIALOG_SELECT, MyConfigDialog::OnSelect)
wxEND_EVENT_TABLE()

#define NAME_COL (0)
#define TARGET_COL  (1)
#define TARGET_DESC_COL (2)
#define TARGET_UNIT_COL (3)
#define TARGET_MAX_COL (4)
#define TARGET_CUR_COL (5)
#define INCLUDE_ALL_COL (6)
#define ENABLE_STEP_COL (7)
#define STATUS_COL  (8)
#define ACTIVITY_POOL_COL (9)
#define COL_COUNT (10)

MyConfigDialog::MyConfigDialog(MyFrame *pFrame)
	:wxDialog(pFrame, wxID_ANY, "奖项设置", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	mpFrame = pFrame;
	mpGrid = new wxGrid(this, wxID_ANY);
	// mpGrid->SetCellHighlightPenWidth(0);
	// mpGrid->EnableEditing(false);
	mpGrid->CreateGrid(0, COL_COUNT, wxGrid::wxGridSelectRows);
	mpGrid->SetColLabelValue(NAME_COL, "名称");
	mpGrid->SetColLabelValue(TARGET_COL, "奖品");
	mpGrid->SetColLabelValue(TARGET_DESC_COL, "奖品图片");

	mpGrid->SetColLabelValue(TARGET_MAX_COL, "总数量");
	mpGrid->SetColFormatNumber(TARGET_MAX_COL);

	mpGrid->SetColLabelValue(TARGET_UNIT_COL, "每次数量");
	mpGrid->SetColFormatNumber(TARGET_UNIT_COL);

	mpGrid->SetColLabelValue(TARGET_CUR_COL, "已抽数量");
	mpGrid->SetColFormatNumber(TARGET_CUR_COL);

	mpGrid->SetColLabelValue(INCLUDE_ALL_COL, "包含所有");
	mpGrid->SetColFormatBool(INCLUDE_ALL_COL);

	mpGrid->SetColLabelValue(ENABLE_STEP_COL, "分步显示结果");
	mpGrid->SetColFormatBool(ENABLE_STEP_COL);

	mpGrid->SetColLabelValue(ACTIVITY_POOL_COL, "奖池");

	mpGrid->SetColLabelValue(STATUS_COL, "状态"); // 已抽，未抽
	
	SetSize(900,300);

	mCurrentLine = -1;
	mActiveAct = -1;
	LoadFromFile("./media/activitylist.txt");
}

MyConfigDialog::~MyConfigDialog(){
	SaveToFile("./media/activitylist.txt");
}

void MyConfigDialog::UpdateRow(int iIndex)
{
	if (iIndex >= mpGrid->GetNumberRows()){
		return;
	}
	wxString text = mpGrid->GetCellValue(iIndex, TARGET_CUR_COL);
	bool bReadOnly = true;
	if (text.IsEmpty()){
		bReadOnly = false;
		mpGrid->SetCellValue(iIndex, TARGET_CUR_COL, "0");
	}
	else{
		long iLong = 0;
		text.ToLong(&iLong);
		if (iLong == 0){
			bReadOnly = false;
		}
	}
	if (bReadOnly){
		mpGrid->SetCellValue(iIndex, STATUS_COL, "已抽");
	}
	else{
		mpGrid->SetCellValue(iIndex, STATUS_COL, "未抽");
	}

	for (int i = 0; i < mpGrid->GetNumberCols(); i++){
		if (bReadOnly || i == TARGET_CUR_COL || i == STATUS_COL|| i == TARGET_DESC_COL){
			mpGrid->SetReadOnly(iIndex, i);
		}
		else{
			mpGrid->SetReadOnly(iIndex, i, false);
		}
		if (iIndex == mActiveAct){
			mpGrid->SetCellTextColour(iIndex, i, *wxRED);
			mpGrid->SetCellBackgroundColour(iIndex, i, *wxGREEN);
		}
		else{
			mpGrid->SetCellTextColour(iIndex, i, *wxBLACK);
			mpGrid->SetCellBackgroundColour(iIndex, i, *wxWHITE);
		}
	}
}

void MyConfigDialog::OnCellRightClick(wxGridEvent &evt){
	wxMenu menu;
	menu.Append(MENU_CONFIG_DIALOG_SELECT, "使用");
	menu.Append(MENU_CONFIG_DIALOG_INSERT, "插入");
	menu.Append(MENU_CONFIG_DIALOG_APPEND, "添加");
	menu.Append(MENU_CONFIG_DIALOG_DELETE, "删除");
	menu.Append(MENU_CONFIG_DIALOG_DELETE_ALL, "清空奖项");
	menu.Append(MENU_CONFIG_DIALOG_IMPORT, "导入...");
	menu.Append(MENU_CONFIG_DIALOG_EXPORT, "导出...");
	PopupMenu(&menu);
}

void MyConfigDialog::OnLabelRightClick(wxGridEvent &evt){
	wxMenu menu;
	menu.Append(MENU_CONFIG_DIALOG_APPEND, "添加");
	menu.Append(MENU_CONFIG_DIALOG_IMPORT, "导入...");
	menu.Append(MENU_CONFIG_DIALOG_EXPORT, "导出...");
	menu.Append(MENU_CONFIG_DIALOG_DELETE_ALL, "清空奖项");
	PopupMenu(&menu);
}

void MyConfigDialog::OnLeftDClick(wxGridEvent &evt)
{
	if (evt.GetCol() == TARGET_DESC_COL){
			 wxFileDialog dlg(this, _("选择图片"), "", mpGrid->GetCellValue(evt.GetRow(), evt.GetCol()),
						         "图像文件 (*.jpeg;*.jpg;*.png;*.bmp;*.gif)",
								  wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	 if (dlg.ShowModal() == wxID_CANCEL){
		 return;
	 }
	 mpGrid->SetCellValue(evt.GetRow(), evt.GetCol(), dlg.GetPath());
	 //LoadFromFile(dlg.GetPath());
	}
}


void MyConfigDialog::OnSelectCell(wxGridEvent &evt)
{
	mCurrentLine = evt.GetRow();
}

int MyConfigDialog::GetCurrentLine()
{
	return mCurrentLine;
}

void MyConfigDialog::SaveToFile(const wxString &strFileName){
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

void MyConfigDialog::LoadFromFile(const wxString &strFileName)
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

void MyConfigDialog::OnImport(wxCommandEvent &evt)
{
	 wxFileDialog dlg(this, _("从文件导入"), "", "activitylist.txt",
						         "txt files (*.txt)|*.txt",
								  wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	 if (dlg.ShowModal() == wxID_CANCEL){
		 return;
	 }
	 LoadFromFile(dlg.GetPath());
}

void MyConfigDialog::OnExport(wxCommandEvent &evt)
{
	wxFileDialog dlg(this, _("导出到文件"), "", "activitylist.txt",
						         "txt files (*.txt)|*.txt",
								 wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	 if (dlg.ShowModal() == wxID_CANCEL){
		 return;
	 }
	 SaveToFile(dlg.GetPath());
}

void MyConfigDialog::OnInsert(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur <= 0){
		mpGrid->InsertRows(0, 1);
	}
	else{
		mpGrid->InsertRows(iCur, 1);
	}
	mpGrid->SetCellValue(iCur, TARGET_MAX_COL, "1");
	mpGrid->SetCellValue(iCur, TARGET_UNIT_COL, "1");
	mpGrid->SetCellValue(iCur, TARGET_CUR_COL, "0");

	UpdateRow(iCur);
	mpGrid->SetGridCursor(iCur, NAME_COL);
	mpGrid->SelectRow(iCur);
}

void MyConfigDialog::OnAppend(wxCommandEvent &evt)
{
	int iCount = mpGrid->GetNumberRows();
	mpGrid->AppendRows();
	mpGrid->SetCellValue(iCount, TARGET_MAX_COL, "1");
	mpGrid->SetCellValue(iCount, TARGET_UNIT_COL, "1");
	mpGrid->SetCellValue(iCount, TARGET_CUR_COL, "0");
	UpdateRow(iCount);
	mpGrid->SetGridCursor(iCount, NAME_COL);
	mpGrid->SelectRow(iCount);
}

int MyConfigDialog::GetDefaultStep(MyActivity *pAct)
{
	int ret = 10;
	if (NULL == pAct){
		return ret;
	}
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (mpGrid->GetCellValue(i, NAME_COL) == pAct->mName && mpGrid->GetCellValue(i, TARGET_COL) == pAct->mDesc){
			if (mpGrid->GetCellValue(i, ENABLE_STEP_COL) != "1"){
				ret = 100;
				break;
			}
		}
	}
	return ret;
}

int MyConfigDialog::GetActivityCount(MyActivity *pAct)
{
	int ret = 0;
	if (NULL == pAct){
		return ret;
	}
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (mpGrid->GetCellValue(i, NAME_COL) == pAct->mName && mpGrid->GetCellValue(i, TARGET_COL) == pAct->mDesc){
			long iCount = 0, iTotalCount = 0, iUsed = 0;
			mpGrid->GetCellValue(i, TARGET_MAX_COL).ToLong(&iTotalCount);
			mpGrid->GetCellValue(i, TARGET_UNIT_COL).ToLong(&iCount);
			mpGrid->GetCellValue(i, TARGET_CUR_COL).ToLong(&iUsed);
			if ((iTotalCount - iUsed) < iCount){
				iCount = iTotalCount - iUsed;
			}
			ret = iCount;
			break;
		}
	}
	return ret;
}

void MyConfigDialog::OnSelect(wxCommandEvent &evt)
{
	MyThread *pThread = mpFrame->GetThread();
	if (NULL != pThread && pThread->IsStart()){
		wxMessageBox("请先结束当前抽奖");
		return;
	}
	int iIndex = GetCurrentLine();
	if (iIndex < 0){
		return;
	}
	// mpGrid->SetCellValue(iIndex, STATUS_COL, "正在抽奖");
	// Update mpFrame according current selected item.
	long iCount = 0, iTotalCount = 0, iUsed = 0;
	mpGrid->GetCellValue(iIndex, TARGET_MAX_COL).ToLong(&iTotalCount);
	mpGrid->GetCellValue(iIndex, TARGET_UNIT_COL).ToLong(&iCount);
	mpGrid->GetCellValue(iIndex, TARGET_CUR_COL).ToLong(&iUsed);
	if ((iTotalCount - iUsed) < iCount){
		iCount = iTotalCount - iUsed;
	}
	if (iCount == 0){
		wxMessageBox("该奖项已抽完");
		return;
	}
	MyActivity *pAct = new MyActivity;
	pAct->mName = mpGrid->GetCellValue(iIndex, NAME_COL);
	pAct->mDesc = mpGrid->GetCellValue(iIndex, TARGET_COL);
	pAct->mPool = mpGrid->GetCellValue(iIndex, ACTIVITY_POOL_COL);
	pAct->mBitmap = mpGrid->GetCellValue(iIndex, TARGET_DESC_COL);
	//pAct->mTotalNumber = iTotalCount;
	//pAct->mLeftNumber = iTotalCount - iUsed;
	mpFrame->SetCurActivity(pAct, iCount);
	if (mActiveAct >= 0 && mActiveAct <= mpGrid->GetNumberRows()){
		for (int i = 0; i < mpGrid->GetNumberCols(); i++){
			mpGrid->SetCellTextColour(mActiveAct, i, *wxBLACK);
			mpGrid->SetCellBackgroundColour(mActiveAct, i, *wxWHITE);
		}
	}
	mActiveAct = iIndex;
	UpdateRow(iIndex);
	mpGrid->Refresh();
}

void MyConfigDialog::OnDelete(wxCommandEvent &evt)
{
	int iCur = GetCurrentLine();
	if (iCur < 0){
		return;
	}
	if (mActiveAct == iCur){
		mActiveAct = -1;
	}
	wxString text = mpGrid->GetCellValue(iCur, TARGET_CUR_COL);
	long iLong = 0;
	text.ToLong(&iLong);
	if (iLong != 0){
		wxMessageBox("该奖项已抽取，无法删除，请先做弃奖操作或者清空抽奖记录");
		return;
	}
	mpGrid->DeleteRows(iCur, 1);
}

void MyConfigDialog::OnDeleteAll(wxCommandEvent &evt)
{
	mActiveAct = -1;
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		wxString text = mpGrid->GetCellValue(i, TARGET_CUR_COL);
		long iLong = 0;
		text.ToLong(&iLong);
		if (iLong != 0){
			wxMessageBox("已有奖项被抽取，请先做弃奖操作或者清空抽奖记录");
			return;
		}
	}
	int answer = wxMessageBox("确认清空奖项列表？","确认",wxYES_NO | wxCANCEL, this);
	if (answer != wxYES){
		return;
	}
	mpGrid->DeleteRows(0, mpGrid->GetNumberRows());
}

bool MyConfigDialog::UpdateActivity(const wxString &strName, const wxString &strDesc, int iNumber)
{
	int i = 0;
	bool bFound = false;
	for (i = 0; i < mpGrid->GetNumberRows(); i++){
		if (mpGrid->GetCellValue(i, NAME_COL) == strName && mpGrid->GetCellValue(i, TARGET_COL) == strDesc){
			bFound = true;
			break;
		}
	}
	if (!bFound){
		return false;
	}
	wxString text = mpGrid->GetCellValue(i, TARGET_CUR_COL);
	long iCount = 0;
	text.ToLong(&iCount);
	iCount += iNumber;
	text = mpGrid->GetCellValue(i, TARGET_MAX_COL);
	long iTotal = 0;
	text.ToLong(&iTotal);
	if (iCount >= iTotal){
		iCount = iTotal;
	}
	text = wxString::Format("%d", iCount);
	mpGrid->SetCellValue(i, TARGET_CUR_COL, text);
	UpdateRow(i);
	return true;
}

bool MyConfigDialog::UpdateActivity(MyActivity *pActivity, std::vector<MyCandidate *> &candidates)
{
	if (NULL == pActivity){
		return false;
	}
	long iCount = 0;
	for (int j = 0; j < candidates.size(); j++){
		if (candidates[j] != NULL){
			iCount++;
		}
	}
	return UpdateActivity(pActivity->mName, pActivity->mDesc, iCount);
}

void MyConfigDialog::UpdateAllActivity()
{
	for(int i = 0; i < mpGrid->GetNumberRows(); i++){
		mpGrid->SetCellValue(i, TARGET_CUR_COL, "0");
		UpdateRow(i);
	}
}

bool MyConfigDialog::IsPool(const wxString &strName, const wxString &strDesc, const wxString &strPool){
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (mpGrid->GetCellValue(i, NAME_COL) == strName && mpGrid->GetCellValue(i, TARGET_COL) == strDesc){
			if (mpGrid->GetCellValue(i, ACTIVITY_POOL_COL) == strPool){
				return true;
			}
			return false;
		}
	}
	return true;
}

bool MyConfigDialog::IsIncludeAll(const wxString &strName, const wxString &strDesc)
{	
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (mpGrid->GetCellValue(i, NAME_COL) == strName && mpGrid->GetCellValue(i, TARGET_COL) == strDesc){
			if (mpGrid->GetCellValue(i, INCLUDE_ALL_COL) == "1"){
				return true;
			}
			return false;
		}
	}
	return false;
}

int MyConfigDialog::GetLeftNumber(const wxString &strName, const wxString &strDesc)
{
	for (int i = 0; i < mpGrid->GetNumberRows(); i++){
		if (mpGrid->GetCellValue(i, NAME_COL) == strName && mpGrid->GetCellValue(i, TARGET_COL) == strDesc){
			long iCount = 0, iTotalCount = 0, iUsed = 0;
			mpGrid->GetCellValue(i, TARGET_MAX_COL).ToLong(&iTotalCount);
			mpGrid->GetCellValue(i, TARGET_CUR_COL).ToLong(&iUsed);
			iCount = iTotalCount - iUsed;
			return iCount;
		}
	}
	return 0;
}