#include <wx/utils.h>
#include "Main.hpp"
#include "work.hpp"
#include "CandidateDialog.hpp"

wxThread::ExitCode MyThread::Entry()
{
	while(mbQuit == false){
		if (mbStart){
			Work();
		}
		else{
			wxMilliSleep(100);
			for (int i = 0; i < mCandidates.size(); i++){
				delete mCandidates[i];
			}
			mCandidates.clear();
		}
	}
	wxMilliSleep(100);
	for (int i = 0; i < mCandidates.size(); i++){
		delete mCandidates[i];
	}
	mCandidates.clear();
	mbFinish = true;
	return (ExitCode) 0;
}

void MyThread::Work(){
	//
	if (NULL == mpDst){
		return;
	}
	wxCommandEvent *pEvt = new wxCommandEvent;
	pEvt->SetId(EVT_THREAD_MSG);
	MyEvtPayload *pPayload = new MyEvtPayload();

	if (mCandidates.empty()){
		mpDst->GetCandidateList(mCandidates);
	}
	std::vector<MyCandidate *> tempList = mCandidates;
	if (mCandidates.empty()){
		wxMilliSleep(10);
		return;
	}
	int iCount = mpDst->GetCount();
	if (iCount == 0){
		wxMilliSleep(10);
		return;
	}

	srand(GetTickCount());
	int iCandidateCount = tempList.size();

	while(pPayload->mCandidateList.size() != iCount){
		if (iCandidateCount<= 0){
			break;
		}
		int iOffset = rand()%iCandidateCount;
		pPayload->mCandidateList.push_back(new MyCandidate(*tempList[iOffset]));
		tempList[iOffset] = tempList[tempList.size() -1];
		tempList.resize(tempList.size()-1);
		iCandidateCount = tempList.size();
	}
	pPayload->mTotalCandidate = mCandidates.size();
	pEvt->SetClientData(pPayload);
	wxQueueEvent(mpDst, pEvt);
	wxMilliSleep(10);
}
