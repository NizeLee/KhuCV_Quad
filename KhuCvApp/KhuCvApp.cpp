//  KhuCvApp.cpp: implementation of CKhuCvApp
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "KhuCvApp.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif

#include <wx/display.h>

bool CKhuCvApp::OnInit()
{

    int language = wxLANGUAGE_KOREAN;
    m_locale.Init(language);

#ifndef _KHUCV_SDI
    m_pMainFrame = new CMainFrame(wxT("KhuCV App"));
#else
    m_pMainFrame = new CMainFrame(wxT("KhuCV App-Sdi"));
#endif

#ifndef  __APPLE__ 
    m_pMainFrame->Maximize(true);
#endif
    m_pMainFrame->Show();

    m_pMainFrame->DragAcceptFiles(true);
    
    wxSize MainFrameSize = m_pMainFrame->GetSize();
    
    
    wxPoint DlgPos;
#ifndef _KHUCV_SDI
    wxSize DlgSize(550, 350);
#else
    wxSize DlgSize(550, 500);
#endif
#ifdef __APPLE__
#ifndef _KHUCV_SDI
    DlgPos = {0, 100+480};//{5, wxDisplay().GetGeometry().GetHeight()-DlgSize.y};
#else
    DlgPos = { 5, MainFrameSize.y - 200 - DlgSize.y };
#endif
#else
    DlgPos = {5, MainFrameSize.y-200-DlgSize.y};
#endif
    
    m_pMainDialog = new CMainDialog(m_pMainFrame, wxID_ANY, wxT("Main Dialog"), DlgPos, DlgSize, wxDEFAULT_DIALOG_STYLE);
    m_pMainDialog->Show();

#ifndef _KHUCV_SDI
#else
    while (m_PrintVector.size() > 0) {
        for (auto msg : m_PrintVector) {
            int Num = m_pMainDialog->GetPrintListBox()->GetCount();
            m_pMainDialog->GetPrintListBox()->InsertItems(1, &msg, Num);
            m_pMainDialog->GetPrintListBox()->SetSelection(Num);
        }
        m_PrintVector.clear();
    }
#endif
   
    return true;
}

CKhuCvApp::~CKhuCvApp() {
    
}

IMPLEMENT_APP(CKhuCvApp)

#ifndef _KHUCV_SDI
void NewFileOpen(const char* fileName, cv::Mat cvImage, int nPosX, int nPosY) {
#ifndef _KHUCV_SDI
    CMainFrame* pMainFrame = wxGetApp().m_pMainFrame;
    CChildFrame* pParentFrame = new CChildFrame(pMainFrame, wxID_ANY, fileName);
#else
    CMainFrame* pParentFrame = wxGetApp().m_pMainFrame;
#endif

    CKcImage kcImage = CKcImage(cvImage, wxPoint(nPosX, nPosY));
    pParentFrame->m_ImageList.push_back(kcImage);
    pParentFrame->Show();
}
#endif

void DisplayImage(cv::Mat cvImage, int nPosX, int nPosY, bool bErase, bool bClearPos) {
#ifndef _KHUCV_SDI
    CMainFrame* pMainFrame = wxGetApp().m_pMainFrame;
    CChildFrame* pParentFrame = (CChildFrame*)(pMainFrame->GetActiveChild());
#else
    CMainFrame* pParentFrame = wxGetApp().m_pMainFrame;
#endif

    cv::Mat cvCloneImage = cvImage.clone();

    CKcImage kcImage;
    if (cvCloneImage.type() != CV_8UC3) {
        if (cvCloneImage.channels() == 1) {
            cv::Mat cvNewImage(cvCloneImage.rows, cvCloneImage.cols, CV_8UC3);
            cv::normalize(cvCloneImage, cvCloneImage, 0, 255, cv::NORM_MINMAX);
            cvCloneImage.convertTo(cvCloneImage, CV_8UC1);
            cv::cvtColor(cvCloneImage, cvNewImage, cv::COLOR_GRAY2BGR);
            kcImage = CKcImage(cvNewImage, wxPoint(nPosX, nPosY));
        }
        else if(cvCloneImage.channels() == 3) {
            cv::Mat cvNewImage;
            cv::Mat BGR[3];

            cv::split(cvCloneImage, BGR);
            for(int i = 0 ; i < 3 ; ++i) {
                //cv::normalize(BGR[i], BGR[i], 0, 255, cv::NORM_MINMAX);
                BGR[i].convertTo(BGR[i], CV_8UC1);
            }
            cv::merge(BGR, 3, cvNewImage);

            kcImage = CKcImage(cvNewImage, wxPoint(nPosX, nPosY));
        }
        else
            return;
    }
    else
        kcImage = CKcImage(cvCloneImage, wxPoint(nPosX, nPosY));

    if(bClearPos) {
        auto NewEnd = std::remove_if(pParentFrame->m_ImageList.begin(), pParentFrame->m_ImageList.end(), [=](CKcImage kcImage){return kcImage.pos == wxPoint(nPosX, nPosY);});
        pParentFrame->m_ImageList.erase(NewEnd, pParentFrame->m_ImageList.end());
    }

    pParentFrame->m_ImageList.push_back(kcImage);
    pParentFrame->m_pClientView->m_nLastSelImageNum = pParentFrame->m_ImageList.size()-1;
    pParentFrame->m_pClientView->Refresh(bErase);
    pParentFrame->m_pClientView->Update();
}

CKcImage GetLastSelImage() {
#ifndef _KHUCV_SDI
    CMainFrame* pMainFrame = wxGetApp().m_pMainFrame;
    CChildFrame* pParentFrame = (CChildFrame*)(pMainFrame->GetActiveChild());
#else
    CMainFrame* pParentFrame = wxGetApp().m_pMainFrame;
#endif

    int nCurrentGrabImageNum = pParentFrame->m_pClientView->m_nLastSelImageNum;

    if (nCurrentGrabImageNum < 0) {

        if(pParentFrame->m_ImageList.size() > 0)
            return pParentFrame->m_ImageList[pParentFrame->m_ImageList.size()-1];

        return CKcImage();
    }
    else if (nCurrentGrabImageNum >= pParentFrame->m_ImageList.size()) {

        if (pParentFrame->m_ImageList.size() > 0)
            return pParentFrame->m_ImageList[pParentFrame->m_ImageList.size() - 1];

        return CKcImage();
    }

    return pParentFrame->m_ImageList[nCurrentGrabImageNum];
}

CMainDialog* GetMainDialog() {
    return wxGetApp().m_pMainDialog;
}

CClientView* GetActiveClientView() {
#ifndef _KHUCV_SDI
    CMainFrame* pMainFrame = wxGetApp().m_pMainFrame;
    CChildFrame* pParentFrame = (CChildFrame*)(pMainFrame->GetActiveChild());
#else
    CMainFrame* pParentFrame = wxGetApp().m_pMainFrame;
#endif
    return pParentFrame->m_pClientView;
}

void DlgPrintf(const char* ptr, ...) {
    unsigned int Num;

    char ach[1024];
    va_list args;
    va_start(args, ptr);
    vsnprintf(ach, 1024, ptr, args);

    wxString msg = ach;
#ifndef _KHUCV_SDI
    CMainFrame* pMainFrame = wxGetApp().m_pMainFrame;

    Num = pMainFrame->GetPrintListBox()->GetCount();
    pMainFrame->GetPrintListBox()->InsertItems(1, &msg, Num);
    pMainFrame->GetPrintListBox()->SetSelection(Num);
#else
    if (wxGetApp().m_pMainDialog) {
        CMainDialog* pMainDialog = wxGetApp().m_pMainDialog;

        while (wxGetApp().m_PrintVector.size() > 0) {
            for (auto msg : wxGetApp().m_PrintVector) {
                Num = pMainDialog->GetPrintListBox()->GetCount();
                pMainDialog->GetPrintListBox()->InsertItems(1, &msg, Num);
                pMainDialog->GetPrintListBox()->SetSelection(Num);
            }
            wxGetApp().m_PrintVector.clear();
        }
        Num = pMainDialog->GetPrintListBox()->GetCount();
        pMainDialog->GetPrintListBox()->InsertItems(1, &msg, Num);
        pMainDialog->GetPrintListBox()->SetSelection(Num);
    }
    else {
        wxGetApp().m_PrintVector.push_back(msg);
    }
#endif
}

void DrawTextOnImage(cv::Mat& cvImage, const std::string& str, int x, int y, unsigned char R, unsigned char G, unsigned char B, int pointSize) {
    cv::Mat rgbImage;
    int nW = cvImage.cols;
    int nH = cvImage.rows;

    wxColour color(R, G, B);

    cv::cvtColor(cvImage, rgbImage, cv::COLOR_BGR2RGB);

    wxImage wx_Image = wxImage(nW, nH, (unsigned char*)rgbImage.data, true);

    wxBitmap bmp = wx_Image;

    wxMemoryDC memdc;
    wxBitmap memBmp(bmp.GetWidth(), bmp.GetHeight());
    memBmp.UseAlpha();
    memdc.SelectObject(memBmp);
    memdc.DrawBitmap(bmp, 0, 0, true);

    wxFont font(pointSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");
    memdc.SetFont(font);

    wxString text = str;

    memdc.SetTextForeground(color);

    memdc.DrawText(text, x, y);
    memdc.SelectObject(wxNullBitmap);

    wx_Image = memBmp.ConvertToImage();

    rgbImage = cv::Mat(nH, nW, CV_8UC3, wx_Image.GetData(), nW * 3);
    cv::cvtColor(rgbImage, cvImage, cv::COLOR_RGB2BGR);
}