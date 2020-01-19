#ifndef _MY_WORK_HPP_
#define _MY_WORK_HPP_
#include <wx/thread.h>
#include <wx/event.h>
#include <vector>

class MyCandidate;
class MyCandidateDialog;
class MyEvtPayload{
public:
	int mTotalCandidate;
	std::vector<MyCandidate *> mCandidateList;
};

class MyFrame;
class MyThread : public wxThread
{
public:
	MyThread(MyFrame *pDst, MyCandidateDialog *pCandidateDialog)
		: wxThread(wxTHREAD_DETACHED)
	{
		mpDst = pDst;
		mbStart = false;
		mbQuit = false;
		mbFinish = false;
		mpCandidateDialog = pCandidateDialog;
	}
	
	~MyThread(){};
	bool IsStart() const {return mbStart;}
	void Start(){mbStart = true;}
	void Stop(){mbStart = false;}
	void Quit(){mbQuit = true;}
	bool HasFinish(){return mbFinish;}
	
protected:

	void LoadDataFromFile(const wxString &strFileName);
	void SaveDataToFile(const wxString &strFileName);
	void Work();
protected:
	bool mbStart;
	bool mbQuit;
	bool mbFinish;
	virtual ExitCode Entry();
	MyFrame *mpDst;
	MyCandidateDialog *mpCandidateDialog;
	std::vector<MyCandidate *> mCandidates;
};

#endif