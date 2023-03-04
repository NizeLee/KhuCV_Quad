//  MainDialog.h: interface of CMainDialog (main dialog of KhuCvApp)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once

#include "Project.h"

#ifndef _KHUCV_SDI
#define H_BOX_SIZER_NUM	6
#else
#define H_BOX_SIZER_NUM	7
#endif

class CMainDialog : public wxDialog
{
public:
	CMainDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style);
	virtual ~CMainDialog();
	
	wxBoxSizer* m_pVbox;
	wxBoxSizer* m_pHbox[H_BOX_SIZER_NUM];

	wxButton* m_pSrcFolderSelButton;
	wxCheckBox* m_pVideoFileCheck;
	wxStaticText* m_pDisplaySrcPathText;

	wxButton* m_pDesFolderSelButton;
	wxCheckBox* m_pSaveFrameCheck;
	wxStaticText* m_pDisplayDesPathText;

	wxListCtrl* m_pListCtrl;

	wxTextCtrl* m_pStartNum;
	wxTextCtrl* m_pEndNum;
	wxCheckBox* m_pCopyCheck;
	wxComboBox* m_pSelZoom;

	wxComboBox* m_pSelCam;
	wxButton* m_pRunButton;
	wxButton* m_pSaveButton, * m_pNextButton, * m_pSaveNextButton;
	wxCheckBox* m_pStepCheck;
	wxCheckBox* m_pVerboseCheck;
	
	wxButton* m_pExampleButton;
	wxCheckBox* m_pViewRectCheck, * m_pViewQuadCheck;
#ifndef _KHUCV_SDI
#else
	wxListBox* m_pPrintListBox;
#endif

	wxTimer m_SequenceRunTimer;
	int m_nProcessingNum;
	bool m_bRunTimer, m_bRunPause;

	friend class CClientView;

	wxTimer m_VideoRunTimer;
	cv::VideoCapture m_VideoProcessingVc;
	cv::VideoCapture m_CamProcessingVc;
	char m_VideoFileName[256];
	int m_nVideoFileFrameCnt;

	wxTimer m_CamRunTimer;

	bool m_bFirstRun;

	CProject m_Project;
#ifndef _KHUCV_SDI
#else
	wxListBox* GetPrintListBox() const;
#endif
	
	void OnSelSrcFileFolder(wxCommandEvent& event);
	void OnSelDesFileFolder(wxCommandEvent& event);
	void OnActivatedFileListCtrl(wxListEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnRun(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnNext(wxCommandEvent& event);
	void OnSaveNext(wxCommandEvent& event);
	void OnSaveNextCopy(wxCommandEvent& event);
	void OnExample(wxCommandEvent& event);
	void OnCloseup(wxCommandEvent& event);
	void OnCloseupSelZoom(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();
};

