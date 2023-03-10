//  KhuCvApp.h: interface of CKhuCvApp
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/clipbrd.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/filehistory.h>
#include <wx/config.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <sstream>
#include <fstream>

#include "ClientView.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "MainDialog.h"

extern int g_classNum;
extern wxColour g_classColor[2000];
extern wxString g_className[2000];

class CKhuCvApp : public wxApp
{
public:
    CKhuCvApp() : wxApp() {
        m_pMainFrame = nullptr;
        m_pMainDialog = nullptr;
    }

    CMainFrame* m_pMainFrame;
    CMainDialog* m_pMainDialog;
    wxLocale m_locale;

    std::vector<wxString> m_PrintVector;

    ~CKhuCvApp();
    virtual bool OnInit();
};

DECLARE_APP(CKhuCvApp)

#ifndef _KHUCV_SDI
void NewFileOpen(const char* fileName, cv::Mat cvImage, int nPosX=0, int nPosY=0);
#endif
void DisplayImage(cv::Mat cvImage, int nPosX, int nPosY, bool bErase, bool bClearPos);
CKcImage GetLastSelImage();
CMainDialog* GetMainDialog();
CClientView* GetActiveClientView();
void DlgPrintf(const char* ptr, ...);
void DrawTextOnImage(cv::Mat& cvImage, const std::string& str, int x, int y, unsigned char R = 255, unsigned char G = 0, unsigned char B = 0, int pointSize = 15);