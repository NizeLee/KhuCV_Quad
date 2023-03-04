//  ClientView.cpp: implementation of CClientView (client view of child frame)
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

class CLabelDialog : public wxDialog
{
public:
    wxStaticText* m_pDisplayInfo;
    wxComboBox* m_pClassCombo;
    wxButton* m_pOkButton, * m_pCancelButton;

    wxBoxSizer* m_pVbox;
    wxBoxSizer* m_pHbox[3];

    CLabelDialog(wxWindow* parent, int nClass, const wxString &info) 
        : wxDialog(parent, wxID_ANY, wxString("Label Dialog"), wxDefaultPosition, wxSize(300, 150), wxDEFAULT_DIALOG_STYLE) {

        wxArrayString ClassString;
        for (int i = 0; i < g_classNum; ++i) {
            ClassString.Add(g_className[i]);
        }

        m_pDisplayInfo = new wxStaticText(this, IDC_DISPLAY_INFO, info, wxDefaultPosition, wxSize(250, 20), wxST_NO_AUTORESIZE);

        m_pClassCombo = new wxComboBox(this, IDC_CLASS_COMBO, wxEmptyString, wxDefaultPosition, wxSize(250, 20), ClassString, wxCB_DROPDOWN | wxCB_READONLY);
        m_pClassCombo->SetSelection(nClass);

        m_pOkButton = new wxButton(this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxSize(70, 20));
        m_pCancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize(70, 20));

        m_pVbox = new wxBoxSizer(wxVERTICAL);

        for (int i = 0; i < 3; ++i)
            m_pHbox[i] = new wxBoxSizer(wxHORIZONTAL);

        m_pHbox[0]->Add(m_pDisplayInfo, 1);

        m_pHbox[1]->Add(m_pClassCombo, 1);

        m_pHbox[2]->Add(m_pOkButton, 0);
        m_pHbox[2]->Add(m_pCancelButton, 0, wxLEFT, 5);

        m_pVbox->Add(m_pHbox[0], 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
        m_pVbox->Add(m_pHbox[1], 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
        m_pVbox->Add(m_pHbox[2], 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);

        SetSizer(m_pVbox);

        Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CLabelDialog::OnOk));
        Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CLabelDialog::OnCancel));
    }
    virtual ~CLabelDialog() {}

    void OnOk(wxCommandEvent& event) {
        EndModal(wxID_OK);
    }
    void OnCancel(wxCommandEvent& event) {
        EndModal(wxID_CANCEL);
    }
};

BEGIN_EVENT_TABLE(CClientView, wxScrolled<wxWindow>)
EVT_PAINT(CClientView::OnPaint)
EVT_LEFT_DOWN(CClientView::OnMouseLeftDown)
EVT_LEFT_UP(CClientView::OnMouseLeftUp)
EVT_LEFT_DCLICK(CClientView::OnMouseLeftDblClk)
EVT_MOTION(CClientView::OnMouseMotion)
EVT_CONTEXT_MENU(CClientView::OnContextMenu)

EVT_MENU(IDM_CONTEXT_COPY, CClientView::OnCopy)
EVT_MENU(IDM_CONTEXT_PASTE, CClientView::OnPaste)
EVT_MENU(IDM_CONTEXT_DUPLICATE, CClientView::OnDuplicate)
EVT_MENU(IDM_CONTEXT_ZOOM_IN, CClientView::OnZoomIn)
EVT_MENU(IDM_CONTEXT_ZOOM_OUT, CClientView::OnZoomOut)
EVT_MENU(IDM_CONTEXT_SEL_REGION, CClientView::OnSelRegion)
EVT_MENU(IDM_CONTEXT_SET_QUAD, CClientView::OnSetQuad)
EVT_MENU(IDM_CONTEXT_SET_LABEL, CClientView::OnSetLabel)
EVT_MENU(IDM_CONTEXT_SAVE_NEXT, CClientView::OnSaveNext)
EVT_MENU(IDM_CONTEXT_DELETE_QUAD, CClientView::OnDeleteQuad)
EVT_MENU(IDM_CONTEXT_BRING_FRONT, CClientView::OnBringFront)
EVT_MENU(IDM_CONTEXT_SEND_BACK, CClientView::OnSendBack)
END_EVENT_TABLE()

#ifndef _KHUCV_SDI
typedef CChildFrame CParentFrame;
#else
typedef CMainFrame CParentFrame;
#endif

bool IsInside(CKcPoint P, CKcPoint* V, int n)
{
    int cn = 0;    // the  crossing number counter

    // loop through all edges of the polygon
    for (int i = 0; i < n; i++) {    // edge from V[i]  to V[i+1]
        if (((V[i].y <= P.y) && (V[i + 1].y > P.y))     // an upward crossing
            || ((V[i].y > P.y) && (V[i + 1].y <= P.y))) { // a downward crossing
                // compute  the actual edge-ray intersect x-coordinate
            float vt = (float)(P.y - V[i].y) / (V[i + 1].y - V[i].y);
            if (P.x < V[i].x + vt * (V[i + 1].x - V[i].x)) // P.x < intersect
                ++cn;   // a valid crossing of y=P.y right of P.x
        }
    }
    return (cn & 1);    // 0 if even (out), and 1 if  odd (in)

}

#ifndef _KHUCV_SDI
CClientView::CClientView(wxMDIChildFrame* parent)  {
#else
CClientView::CClientView(wxWindow * parent) {
#endif
    m_nCurrentGrabImageNum = -1;
    m_nCurrentSelQuadNum = -1;

    m_nSelQuadIndx = -1;
    m_bPreviousRefresh = false;

    m_nCurrentClassIndex = 0;

    Create(parent, wxID_ANY);
    SetScrollRate(10, 10);
    SetVirtualSize(1024 * 20, 768 * 20);

    /*wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_CTRL, (int)'C', IDM_CONTEXT_COPY);
    entries[1].Set(wxACCEL_CTRL, (int)'V', IDM_CONTEXT_PASTE);
    entries[2].Set(wxACCEL_CTRL, (int)'D', IDM_CONTEXT_DUPLICATE);

    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);*/

    m_nSelRegionIndex = -1;
    m_bLeftIsDown = false;
    
    Bind(wxEVT_PAINT, &CClientView::OnPaint, this);

    m_Scale = 100;
};


void CClientView::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);

    wxPoint ScrolledPosition;
    CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

    for (CKcImage& kcImage : ((CParentFrame*)(GetParent()))->m_ImageList) {

        cv::Mat rgbImage;

        cv::cvtColor(kcImage.cvImage, rgbImage, cv::COLOR_BGR2RGB);
        cv::resize(rgbImage, rgbImage, cv::Size(m_Scale * kcImage.cvImage.cols / 100, m_Scale * kcImage.cvImage.rows / 100));


        wxImage wx_Image = wxImage(m_Scale * kcImage.cvImage.cols / 100, m_Scale * kcImage.cvImage.rows / 100, (unsigned char*)rgbImage.data, true);

        wxBitmap bmp = wx_Image;
        dc.DrawBitmap(bmp, ScrolledPosition.x + m_Scale * kcImage.pos.x / 100, ScrolledPosition.y + m_Scale * kcImage.pos.y / 100);
    }

    wxFont font(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");
    dc.SetFont(font);

    wxColour clr;
       
    CMainDialog* pMainDialog = wxGetApp().m_pMainDialog;

    bool bQuadView = true, bRectView = true;
    if (pMainDialog) {
        bQuadView = pMainDialog->m_pViewQuadCheck->GetValue();
        bRectView = pMainDialog->m_pViewRectCheck->GetValue();
    }

    for(int i = 0; bQuadView  && i < m_RegionVector.size() ; ++i) {
        clr = g_classColor[m_RegionVector[i].nClass];

        dc.SetPen(wxPen(clr, 2));
        dc.SetBrush(wxBrush(clr));
        dc.SetTextForeground(clr);

        std::stringstream ss;
        ss << i << ": " << g_className[m_RegionVector[i].nClass];
        dc.DrawText(ss.str(), m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition);

        dc.DrawLine(m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition, m_Scale * m_RegionVector[i].p1 / 100 + ScrolledPosition);
        dc.DrawLine(m_Scale * m_RegionVector[i].p1 / 100 + ScrolledPosition, m_Scale * m_RegionVector[i].p2 / 100 + ScrolledPosition);
        dc.DrawLine(m_Scale * m_RegionVector[i].p2 / 100 + ScrolledPosition, m_Scale * m_RegionVector[i].p3 / 100 + ScrolledPosition);
        dc.DrawLine(m_Scale * m_RegionVector[i].p3 / 100 + ScrolledPosition, m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition);

        dc.DrawCircle(m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition, 5);
        dc.DrawCircle(m_Scale * m_RegionVector[i].p1 / 100 + ScrolledPosition, 5);
        dc.DrawCircle(m_Scale * m_RegionVector[i].p2 / 100 + ScrolledPosition, 5);
        dc.DrawCircle(m_Scale * m_RegionVector[i].p3 / 100 + ScrolledPosition, 5);
    }
    
    for (int i = 0; bRectView && i < m_RectVector.size(); ++i) {
        clr = g_classColor[m_RegionVector[i].nClass];

        dc.SetPen(wxPen(clr, 2));
        dc.SetBrush(wxBrush(clr));
        dc.SetTextForeground(clr);

        std::stringstream ss;
        ss << i << ": " << g_className[m_RectVector[i].nClass];
        dc.DrawText(ss.str(), m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);

        dc.SetPen(wxPen(clr, 2, wxPENSTYLE_LONG_DASH));
        dc.DrawLine(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y, m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);
        dc.DrawLine(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y, m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y);
        dc.DrawLine(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y, m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y);
        dc.DrawLine(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y, m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);

        dc.SetPen(wxPen(clr, 2));
        dc.DrawCircle(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y, 5);
        dc.DrawCircle(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y, 5);
        dc.DrawCircle(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y, 5);
        dc.DrawCircle(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y, 5);
    }
    
#ifndef _MSC_VER
    if(m_nSelRegionIndex < 0) {
        if (m_nCurrentGrabImageNum < 0) return;

        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        dc.DrawRectangle(m_SavePrevRect);
    }
    else {
        if(m_bLeftIsDown) {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            
            dc.DrawRectangle(m_SavePrevRect);
            
        }
        else {
            wxSize sizeView = GetClientSize();
            
            dc.DrawLine(m_SaveMousePos.x, 0, m_SaveMousePos.x, sizeView.y);
            dc.DrawLine(0, m_SaveMousePos.y, sizeView.x, m_SaveMousePos.y);
        }
    }
#endif
}

int CClientView::GetPosImage(const wxPoint &pos) {
    for (int i = ((CParentFrame*)(GetParent()))->m_ImageList.size() - 1; i >= 0; --i) {
        CKcImage& kcImage = ((CParentFrame*)(GetParent()))->m_ImageList[i];
        wxRect rt(kcImage.pos, wxSize(kcImage.cvImage.cols, kcImage.cvImage.rows));
        if (rt.Contains(pos)) return i;
    }

    return -1;
}

int ccw(CKcPoint a, CKcPoint b, CKcPoint c) {
    int op = a.x * b.y + b.x * c.y + c.x * a.y;
    op -= (a.y * b.x + b.y * c.x + c.y * a.x);
    if (op > 0)return 1;
    else if (op == 0)return 0;
    else return -1;
}


int isIntersect(CKcLine x, CKcLine y) {
    int ab = ccw(x.p0, x.p1, y.p0) * ccw(x.p0, x.p1, y.p1);
    int cd = ccw(y.p0, y.p1, x.p0) * ccw(y.p0, y.p1, x.p1);
    if (ab == 0 && cd == 0) {
        if (x.p0 > x.p1)std::swap(x.p0, x.p1);
        if (y.p0 > y.p1)std::swap(y.p0, y.p1);
        return y.p0 <= x.p1 && x.p0 <= y.p1;
    }
    return ab <= 0 && cd <= 0;
}


void CClientView::OnMouseLeftDown(wxMouseEvent& event) {
    m_bLeftIsDown = true;
    m_SaveGrabPoint = event.GetPosition();
    m_nCurrentSelQuadNum = -1;
    m_nCurrentSelQuadPointNum = -1;
    m_nCurrentSelRectPointNum = -1;

    wxPoint ScrolledPosition;
    CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

    if (m_nSelQuadIndx >= 0) {

        CKcImage KcImage = GetLastSelImage();

        m_QuadPt[m_nSelQuadIndx] = event.GetPosition() - ScrolledPosition;
        if (m_QuadPt[m_nSelQuadIndx].x < 0) m_QuadPt[m_nSelQuadIndx].x = 0;
        if (m_QuadPt[m_nSelQuadIndx].x >= m_Scale * KcImage.cvImage.cols / 100) m_QuadPt[m_nSelQuadIndx].x = m_Scale * KcImage.cvImage.cols / 100 - 1;

        if (m_QuadPt[m_nSelQuadIndx].y < 0) m_QuadPt[m_nSelQuadIndx].y = 0;
        if (m_QuadPt[m_nSelQuadIndx].y >= m_Scale * KcImage.cvImage.rows / 100) m_QuadPt[m_nSelQuadIndx].y = m_Scale * KcImage.cvImage.rows / 100 - 1;

        m_nSelQuadIndx++;
        if (m_nSelQuadIndx == 4) {
            m_nSelQuadIndx = -1;

            if (isIntersect(CKcLine(m_QuadPt[0], m_QuadPt[1]), CKcLine(m_QuadPt[2], m_QuadPt[3]))) {
                std::swap(m_QuadPt[0], m_QuadPt[3]);
                DlgPrintf("Cross type I");
            }
            if (isIntersect(CKcLine(m_QuadPt[1], m_QuadPt[2]), CKcLine(m_QuadPt[3], m_QuadPt[0]))) {
                std::swap(m_QuadPt[2], m_QuadPt[3]);
                DlgPrintf("Cross type II");
            }

            wxPoint NewQuadPt[4];

            NewQuadPt[0] = m_QuadPt[0] * 100 / m_Scale;
            NewQuadPt[1] = m_QuadPt[1] * 100 / m_Scale;
            NewQuadPt[2] = m_QuadPt[2] * 100 / m_Scale;
            NewQuadPt[3] = m_QuadPt[3] * 100 / m_Scale;

            CKcQuad KcQuad(NewQuadPt);
            KcQuad.nClass = m_nCurrentClassIndex;
            m_RegionVector.push_back(KcQuad);

            int left = std::min(m_QuadPt[0].x, m_QuadPt[1].x);
            left = std::min(left, m_QuadPt[2].x);
            left = std::min(left, m_QuadPt[3].x);

            int right = std::max(m_QuadPt[0].x, m_QuadPt[1].x);
            right = std::max(right, m_QuadPt[2].x);
            right = std::max(right, m_QuadPt[3].x);

            int top = std::min(m_QuadPt[0].y, m_QuadPt[1].y);
            top = std::min(top, m_QuadPt[2].y);
            top = std::min(top, m_QuadPt[3].y);

            int bottom = std::max(m_QuadPt[0].y, m_QuadPt[1].y);
            bottom = std::max(bottom, m_QuadPt[2].y);
            bottom = std::max(bottom, m_QuadPt[3].y);

            CKcRect KcRect(left * 100 / m_Scale, top * 100 / m_Scale, right * 100 / m_Scale, bottom * 100 / m_Scale);
            KcRect.nClass = m_nCurrentClassIndex;

            m_RectVector.push_back(KcRect);

            Refresh(true);
            Update();

            m_bModified = true;
        }
    }
    else {
        CMainDialog* pMainDialog = wxGetApp().m_pMainDialog;

        bool bQuadView = true, bRectView = true;
        if (pMainDialog) {
            bQuadView = pMainDialog->m_pViewQuadCheck->GetValue();
            bRectView = pMainDialog->m_pViewRectCheck->GetValue();
        }

        CKcPoint pt(event.GetPosition());

        for (int i = m_RegionVector.size() - 1; bQuadView && i >= 0 ; --i) {
            CKcPoint pts[5];
            pts[0] = m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition;
            pts[1] = m_Scale * m_RegionVector[i].p1 / 100 + ScrolledPosition;
            pts[2] = m_Scale * m_RegionVector[i].p2 / 100 + ScrolledPosition;
            pts[3] = m_Scale * m_RegionVector[i].p3 / 100 + ScrolledPosition;
            pts[4] = m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition;

            for (int k = 0; k < 4; ++k) {
                if (sqrt((pts[k].x - pt.x)* (pts[k].x - pt.x) + (pts[k].y - pt.y)* (pts[k].y - pt.y)) < 25) {
                    m_nCurrentSelQuadNum = i;
                    m_nCurrentSelQuadPointNum = k;

                    m_SaveDownPos = event.GetPosition();
                    m_SaveMousePos = m_SaveDownPos;


#ifdef _MSC_VER
                    wxColour clr = g_classColor[m_RegionVector[i].nClass];

                    wxClientDC dc(this);
                    dc.SetPen(wxPen(clr, 2));
                    dc.SetBrush(wxBrush(clr));
                    dc.DrawCircle(pts[k], 5);
#endif

                    DlgPrintf("%d point of %d region is selected", k, i);
                    break;
                }
            }

            if (m_nCurrentSelQuadNum >= 0) break;

            CKcPoint corner[5];
            corner[0] = CKcPoint(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);
            corner[1] = CKcPoint(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);
            corner[2] = CKcPoint(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y);
            corner[3] = CKcPoint(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y);

            for (int k = 0; bRectView && k < 4; ++k) {
                if (sqrt((corner[k].x - pt.x) * (corner[k].x - pt.x) + (corner[k].y - pt.y) * (corner[k].y - pt.y)) < 25) {
                    m_nCurrentSelQuadNum = i;
                    m_nCurrentSelRectPointNum = k;

                    m_SaveDownPos = event.GetPosition();
                    m_SaveMousePos = m_SaveDownPos;


#ifdef _MSC_VER
                    wxColour clr = g_classColor[m_RectVector[i].nClass];

                    wxClientDC dc(this);
                    dc.SetPen(wxPen(clr, 2));
                    dc.SetBrush(wxBrush(clr));
                    dc.DrawCircle(pts[k], 5);
#endif

                    DlgPrintf("%d point of %d rectangle is selected", k, i);
                    break;
                }
            }

            if (m_nCurrentSelQuadNum >= 0) break;

            if (IsInside(pt, pts, 4)) {
                m_nCurrentSelQuadNum = i;
                m_SaveDownPos = event.GetPosition();
                m_SaveMousePos = m_SaveDownPos;

                DlgPrintf("%d region is selected", i);
                break;
            }
        }
    }

    return;

    if(m_nSelRegionIndex < 0) {
        wxPoint ScrolledPosition;
        CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

        m_nCurrentGrabImageNum = GetPosImage(m_SaveGrabPoint - ScrolledPosition);
        m_SavePrevRect = wxRect(0, 0, 0, 0);

        if(m_nCurrentGrabImageNum >= 0){
            int nLastViewImageNum = ((CParentFrame*)(GetParent()))->m_ImageList.size()-1;

            std::swap(((CParentFrame*)(GetParent()))->m_ImageList[m_nCurrentGrabImageNum],
                ((CParentFrame*)(GetParent()))->m_ImageList[nLastViewImageNum]);

            m_nCurrentGrabImageNum = nLastViewImageNum;
            m_nLastSelImageNum = nLastViewImageNum;
        }
    }
    else {
        m_SavePrevRect = wxRect(event.GetPosition(), wxSize(0,0));
    }
}

void CClientView::OnMouseMotion(wxMouseEvent& event) {
    wxPoint ScrolledPosition;
    CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

    m_nRightSelQuadNum = -1;
    if (m_nSelQuadIndx >= 0) {
#ifdef _MSC_VER
        wxClientDC dc(this);
        //dc.SetBrush(*wxTRANSPARENT_BRUSH);

        wxColour clr = g_classColor[m_nCurrentClassIndex];

        dc.SetPen(wxPen(clr, 2));
        if (m_nSelQuadIndx > 1)
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, m_QuadPt[1] + ScrolledPosition);

        if (m_nSelQuadIndx > 2)
            dc.DrawLine(m_QuadPt[1] + ScrolledPosition, m_QuadPt[2] + ScrolledPosition);

        dc.SetPen(wxPen(*wxBLACK, 2));
        dc.SetLogicalFunction(wxINVERT);
        if (m_nSelQuadIndx == 1) {
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, m_SaveMousePos);
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, event.GetPosition());
        }

        if (m_nSelQuadIndx == 2) {
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, m_SaveMousePos);
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, event.GetPosition());

            dc.DrawLine(m_QuadPt[1] + ScrolledPosition, m_SaveMousePos);
            dc.DrawLine(m_QuadPt[1] + ScrolledPosition, event.GetPosition());
        }

        if (m_nSelQuadIndx == 3) {
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, m_SaveMousePos);
            dc.DrawLine(m_QuadPt[0] + ScrolledPosition, event.GetPosition());

            dc.DrawLine(m_QuadPt[2] + ScrolledPosition, m_SaveMousePos);
            dc.DrawLine(m_QuadPt[2] + ScrolledPosition, event.GetPosition());
        }
#endif
        m_SaveMousePos = event.GetPosition();
#ifndef _MSC_VER
        Refresh(false);
#endif
    }
    else if (m_nCurrentSelQuadNum >= 0) {
        if (event.LeftIsDown()) {
#ifdef _MSC_VER
            wxClientDC dc(this);
            dc.SetLogicalFunction(wxINVERT);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);

            if (m_nCurrentSelQuadPointNum < 0 && m_nCurrentSelRectPointNum < 0) {
                if (m_SaveMousePos != m_SaveDownPos) {
                    wxPoint MoveOffset = m_SaveMousePos - m_SaveDownPos;

                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition);
                }

                wxPoint MoveOffset = event.GetPosition() - m_SaveDownPos;
                dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition);
                dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition);
                dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition);
                dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition);
            }
            else if(m_nCurrentSelQuadPointNum >= 0) {
                if (m_SaveMousePos != m_SaveDownPos) {
                    wxPoint MoveOffset = m_SaveMousePos - m_SaveDownPos;

                    switch (m_nCurrentSelQuadPointNum) {
                    case 0:
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + ScrolledPosition);
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + ScrolledPosition);
                        break;
                    case 1:
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + ScrolledPosition);
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + ScrolledPosition);
                        break;
                    case 2:
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + ScrolledPosition);
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + ScrolledPosition);
                        break;
                    case 3:
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + ScrolledPosition);
                        dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + ScrolledPosition);
                        break;
                    }                    
                }
                wxPoint MoveOffset = event.GetPosition() - m_SaveDownPos;

                switch (m_nCurrentSelQuadPointNum) {
                case 0:
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + ScrolledPosition);
                    break;
                case 1:
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + ScrolledPosition);
                    break;
                case 2:
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p1 / 100 + ScrolledPosition);
                    break;
                case 3:
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p2 / 100 + ScrolledPosition);
                    dc.DrawLine(m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p3 / 100 + MoveOffset + ScrolledPosition, m_Scale * m_RegionVector[m_nCurrentSelQuadNum].p0 / 100 + ScrolledPosition);
                    break;
                }
            }
            else if (m_nCurrentSelRectPointNum >= 0) {
                if (m_SaveMousePos != m_SaveDownPos) {
                    wxPoint MoveOffset = m_SaveMousePos - m_SaveDownPos;
                    CKcRect NewRect = CKcRect(m_Scale * m_RectVector[m_nCurrentSelQuadNum].left / 100, m_Scale * m_RectVector[m_nCurrentSelQuadNum].top / 100,
                        m_Scale * m_RectVector[m_nCurrentSelQuadNum].right / 100, m_Scale * m_RectVector[m_nCurrentSelQuadNum].bottom / 100);

                    switch (m_nCurrentSelRectPointNum) {
                    case 0:
                        NewRect.left += MoveOffset.x;
                        NewRect.top += MoveOffset.y;
                        break;
                    case 1:
                        NewRect.right += MoveOffset.x;
                        NewRect.top += MoveOffset.y;
                        break;
                    case 2:
                        NewRect.right += MoveOffset.x;
                        NewRect.bottom += MoveOffset.y;
                        break;
                    case 3:
                        NewRect.left += MoveOffset.x;
                        NewRect.bottom += MoveOffset.y;
                        break;
                    }
                    dc.DrawLine(NewRect.left + ScrolledPosition.x, NewRect.top + ScrolledPosition.y, NewRect.right + ScrolledPosition.x, NewRect.top + ScrolledPosition.y);
                    dc.DrawLine(NewRect.right + ScrolledPosition.x, NewRect.top + ScrolledPosition.y, NewRect.right + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y);
                    dc.DrawLine(NewRect.right + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y, NewRect.left + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y);
                    dc.DrawLine(NewRect.left + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y, NewRect.left + ScrolledPosition.x, NewRect.top + ScrolledPosition.y);
                }
                wxPoint MoveOffset = event.GetPosition() - m_SaveDownPos;
                CKcRect NewRect = CKcRect(m_Scale * m_RectVector[m_nCurrentSelQuadNum].left / 100, m_Scale * m_RectVector[m_nCurrentSelQuadNum].top / 100,
                    m_Scale * m_RectVector[m_nCurrentSelQuadNum].right / 100, m_Scale * m_RectVector[m_nCurrentSelQuadNum].bottom / 100);

                switch (m_nCurrentSelRectPointNum) {
                case 0:
                    NewRect.left += MoveOffset.x;
                    NewRect.top += MoveOffset.y;
                    break;
                case 1:
                    NewRect.right += MoveOffset.x;
                    NewRect.top += MoveOffset.y;
                    break;
                case 2:
                    NewRect.right += MoveOffset.x;
                    NewRect.bottom += MoveOffset.y;
                    break;
                case 3:
                    NewRect.left += MoveOffset.x;
                    NewRect.bottom += MoveOffset.y;
                    break;
                }
                dc.DrawLine(NewRect.left + ScrolledPosition.x, NewRect.top + ScrolledPosition.y, NewRect.right + ScrolledPosition.x, NewRect.top + ScrolledPosition.y);
                dc.DrawLine(NewRect.right + ScrolledPosition.x, NewRect.top + ScrolledPosition.y, NewRect.right + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y);
                dc.DrawLine(NewRect.right + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y, NewRect.left + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y);
                dc.DrawLine(NewRect.left + ScrolledPosition.x, NewRect.bottom + ScrolledPosition.y, NewRect.left + ScrolledPosition.x, NewRect.top + ScrolledPosition.y);
            }

#endif
            m_SaveMousePos = event.GetPosition();
#ifndef _MSC_VER
            Refresh(false);
#endif
        }
    }
    else {
        CKcPoint pt(event.GetPosition());
        bool bSel = false;

        CMainDialog* pMainDialog = wxGetApp().m_pMainDialog;

        bool bQuadView = true, bRectView = true;
        if (pMainDialog) {
            bQuadView = pMainDialog->m_pViewQuadCheck->GetValue();
            bRectView = pMainDialog->m_pViewRectCheck->GetValue();
        }

        for (int i = m_RegionVector.size() - 1; i >= 0; --i) {

            CKcPoint pts[5];
            if (bQuadView) {
                pts[0] = m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition;
                pts[1] = m_Scale * m_RegionVector[i].p1 / 100 + ScrolledPosition;
                pts[2] = m_Scale * m_RegionVector[i].p2 / 100 + ScrolledPosition;
                pts[3] = m_Scale * m_RegionVector[i].p3 / 100 + ScrolledPosition;
                pts[4] = m_Scale * m_RegionVector[i].p0 / 100 + ScrolledPosition;

                for (int k = 0; k < 4; ++k) {
                    if (sqrt((pts[k].x - pt.x) * (pts[k].x - pt.x) + (pts[k].y - pt.y) * (pts[k].y - pt.y)) < 25) {
#ifdef _MSC_VER
                        wxColour clr = g_classColor[m_RegionVector[i].nClass];

                        wxClientDC dc(this);
                        dc.SetPen(wxPen(clr, 2));
                        dc.SetBrush(wxBrush(clr));
                        dc.DrawCircle(pts[k], 10);
#endif
                        bSel = true;
                        break;
                    }
                }

                if (bSel) break;
            }

            if (bRectView) {
                CKcPoint corner[5];
                corner[0] = CKcPoint(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);
                corner[1] = CKcPoint(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].top / 100 + ScrolledPosition.y);
                corner[2] = CKcPoint(m_Scale * m_RectVector[i].right / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y);
                corner[3] = CKcPoint(m_Scale * m_RectVector[i].left / 100 + ScrolledPosition.x, m_Scale * m_RectVector[i].bottom / 100 + ScrolledPosition.y);

                for (int k = 0; k < 4; ++k) {
                    if (sqrt((corner[k].x - pt.x) * (corner[k].x - pt.x) + (corner[k].y - pt.y) * (corner[k].y - pt.y)) < 25) {
#ifdef _MSC_VER
                        wxColour clr = g_classColor[m_RectVector[i].nClass];

                        wxClientDC dc(this);
                        dc.SetPen(wxPen(clr, 2));
                        dc.SetBrush(wxBrush(clr));
                        dc.DrawCircle(corner[k], 10);
#endif
                        bSel = true;
                        break;
                    }
                }

                if (bSel) break;
            }

            if (bQuadView && IsInside(pt, pts, 4)) {
#ifdef _MSC_VER
                wxClientDC dc(this);
                wxPoint wxPoints[4];

                for (int j = 0; j < 4; ++j)
                    wxPoints[j] = pts[j];

                wxColour clr = g_classColor[m_RegionVector[i].nClass];

                dc.SetPen(wxPen(clr, 2));
                dc.SetBrush(wxBrush(clr, wxBRUSHSTYLE_FDIAGONAL_HATCH));
                dc.DrawPolygon(4, wxPoints);
#endif

                m_nRightSelQuadNum = i;

                if (m_nPrevRightSelQuadNum != m_nRightSelQuadNum)
                    Refresh(true);

                m_nPrevRightSelQuadNum = m_nRightSelQuadNum;

                bSel = true;
                break;
            }
        }

        if (!bSel && !m_bPreviousRefresh) {
            Refresh(true);
            m_bPreviousRefresh = true;
        }
        else if (!bSel && m_bPreviousRefresh) {
            m_bPreviousRefresh = true;
        }
        else
            m_bPreviousRefresh = false;
    }

    return;

    if(m_nSelRegionIndex < 0) {
        if (m_nCurrentGrabImageNum < 0) return;

#ifdef _MSC_VER
        wxClientDC dc(this);
        dc.SetLogicalFunction(wxINVERT);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
#endif

        CKcImage& kcImge = ((CParentFrame*)(GetParent()))->m_ImageList[m_nCurrentGrabImageNum];

        wxRect rt(kcImge.pos, wxSize(kcImge.cvImage.cols, kcImge.cvImage.rows));

        wxPoint ScrolledPosition;
        CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));
        rt.Offset(event.GetPosition() - m_SaveGrabPoint + ScrolledPosition);

#ifdef _MSC_VER
        dc.DrawRectangle(m_SavePrevRect);
        dc.DrawRectangle(rt);  
#endif

        m_SavePrevRect = rt;

#ifndef _MSC_VER
        Refresh(false);
#endif
    }
    else {
#ifdef _MSC_VER
        wxClientDC dc(this);
        dc.SetLogicalFunction(wxINVERT);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
#endif

        if(event.LeftIsDown()) {
            wxRect rt;
            rt.x = m_SavePrevRect.x;
            rt.y = m_SavePrevRect.y;
            rt.width = event.GetPosition().x - m_SavePrevRect.x;
            rt.height = event.GetPosition().y - m_SavePrevRect.y;

#ifdef _MSC_VER
            dc.DrawRectangle(m_SavePrevRect);
            dc.DrawRectangle(rt);  
#endif

            m_SavePrevRect = rt;

            if(m_nSelRegionIndex == 1) {
                wxSize sizeView = GetClientSize();

#ifdef _MSC_VER
                dc.DrawLine(m_SaveMousePos.x, 0, m_SaveMousePos.x, sizeView.y);
                dc.DrawLine(0, m_SaveMousePos.y, sizeView.x, m_SaveMousePos.y);
#endif
                m_nSelRegionIndex++;
            }
        }
        else {
            wxSize sizeView = GetClientSize();

#ifdef _MSC_VER
            dc.DrawLine(m_SaveMousePos.x, 0, m_SaveMousePos.x, sizeView.y);
            dc.DrawLine(0, m_SaveMousePos.y, sizeView.x, m_SaveMousePos.y);

            dc.DrawLine(event.GetPosition().x, 0, event.GetPosition().x, sizeView.y);
            dc.DrawLine(0, event.GetPosition().y, sizeView.x, event.GetPosition().y);
#endif

            m_SaveMousePos = event.GetPosition();
        }
#ifndef _MSC_VER
        Refresh(false);
#endif
    }
}

void CClientView::OnMouseLeftUp(wxMouseEvent& event) {
    wxPoint ScrolledPosition;
    CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

    m_bLeftIsDown = false;

    if (m_nCurrentSelQuadNum >= 0) {

        if (m_nCurrentSelQuadPointNum < 0 && m_nCurrentSelRectPointNum < 0) {
            wxPoint MoveOffset = event.GetPosition() - m_SaveDownPos;
            m_RegionVector[m_nCurrentSelQuadNum].p0 = m_RegionVector[m_nCurrentSelQuadNum].p0 + MoveOffset * 100 / m_Scale;
            m_RegionVector[m_nCurrentSelQuadNum].p1 = m_RegionVector[m_nCurrentSelQuadNum].p1 + MoveOffset * 100 / m_Scale;
            m_RegionVector[m_nCurrentSelQuadNum].p2 = m_RegionVector[m_nCurrentSelQuadNum].p2 + MoveOffset * 100 / m_Scale;
            m_RegionVector[m_nCurrentSelQuadNum].p3 = m_RegionVector[m_nCurrentSelQuadNum].p3 + MoveOffset * 100 / m_Scale;

            m_RectVector[m_nCurrentSelQuadNum].left = m_RectVector[m_nCurrentSelQuadNum].left + MoveOffset.x * 100 / m_Scale;
            m_RectVector[m_nCurrentSelQuadNum].top = m_RectVector[m_nCurrentSelQuadNum].top + MoveOffset.y * 100 / m_Scale;
            m_RectVector[m_nCurrentSelQuadNum].right = m_RectVector[m_nCurrentSelQuadNum].right + MoveOffset.x * 100 / m_Scale;
            m_RectVector[m_nCurrentSelQuadNum].bottom = m_RectVector[m_nCurrentSelQuadNum].bottom + MoveOffset.y * 100 / m_Scale;

            m_bModified = true;
        }
        else if(m_nCurrentSelQuadPointNum >= 0) {
            wxPoint MoveOffset = event.GetPosition() - m_SaveDownPos;
            switch (m_nCurrentSelQuadPointNum)
            {
            case 0:
                m_RegionVector[m_nCurrentSelQuadNum].p0 = m_RegionVector[m_nCurrentSelQuadNum].p0 + MoveOffset * 100 / m_Scale ;
                break;
            case 1:
                m_RegionVector[m_nCurrentSelQuadNum].p1 = m_RegionVector[m_nCurrentSelQuadNum].p1 + MoveOffset * 100 / m_Scale;
                break;
            case 2:
                m_RegionVector[m_nCurrentSelQuadNum].p2 = m_RegionVector[m_nCurrentSelQuadNum].p2 + MoveOffset * 100 / m_Scale;
                break;
            case 3:
                m_RegionVector[m_nCurrentSelQuadNum].p3 = m_RegionVector[m_nCurrentSelQuadNum].p3 + MoveOffset * 100 / m_Scale;
                break;
            }

            if (isIntersect(CKcLine(m_RegionVector[m_nCurrentSelQuadNum].p0, m_RegionVector[m_nCurrentSelQuadNum].p1), CKcLine(m_RegionVector[m_nCurrentSelQuadNum].p2, m_RegionVector[m_nCurrentSelQuadNum].p3))) {
                std::swap(m_RegionVector[m_nCurrentSelQuadNum].p0, m_RegionVector[m_nCurrentSelQuadNum].p3);
                DlgPrintf("Cross type I");
            }
            if (isIntersect(CKcLine(m_RegionVector[m_nCurrentSelQuadNum].p1, m_RegionVector[m_nCurrentSelQuadNum].p2), CKcLine(m_RegionVector[m_nCurrentSelQuadNum].p3, m_RegionVector[m_nCurrentSelQuadNum].p0))) {
                std::swap(m_RegionVector[m_nCurrentSelQuadNum].p2, m_RegionVector[m_nCurrentSelQuadNum].p3);
                DlgPrintf("Cross type II");
            }

            m_bModified = true;
        }
        else if (m_nCurrentSelRectPointNum >= 0) {
            wxPoint MoveOffset = event.GetPosition() - m_SaveDownPos;
            switch (m_nCurrentSelRectPointNum)
            {
            case 0:
                m_RectVector[m_nCurrentSelQuadNum].left += MoveOffset.x * 100 / m_Scale;
                m_RectVector[m_nCurrentSelQuadNum].top += MoveOffset.y * 100 / m_Scale;
                break;
            case 1:
                m_RectVector[m_nCurrentSelQuadNum].right += MoveOffset.x * 100 / m_Scale;
                m_RectVector[m_nCurrentSelQuadNum].top += MoveOffset.y * 100 / m_Scale;
                break;
            case 2:
                m_RectVector[m_nCurrentSelQuadNum].right += MoveOffset.x * 100 / m_Scale;
                m_RectVector[m_nCurrentSelQuadNum].bottom += MoveOffset.y * 100 / m_Scale;
                break;
            case 3:
                m_RectVector[m_nCurrentSelQuadNum].left += MoveOffset.x * 100 / m_Scale;
                m_RectVector[m_nCurrentSelQuadNum].bottom += MoveOffset.y * 100 / m_Scale;
                break;
            }

            if (m_RectVector[m_nCurrentSelQuadNum].left > m_RectVector[m_nCurrentSelQuadNum].right)
                std::swap(m_RectVector[m_nCurrentSelQuadNum].left, m_RectVector[m_nCurrentSelQuadNum].right);

            if (m_RectVector[m_nCurrentSelQuadNum].top > m_RectVector[m_nCurrentSelQuadNum].bottom)
                std::swap(m_RectVector[m_nCurrentSelQuadNum].top, m_RectVector[m_nCurrentSelQuadNum].bottom);

            m_bModified = true;
        }

        m_nCurrentSelQuadNum = -1;
        Refresh(true);
    }

    return;

    if(m_nSelRegionIndex < 0) {
        if (m_nCurrentGrabImageNum < 0) return;

        ((CParentFrame*)(GetParent()))->m_ImageList[m_nCurrentGrabImageNum].pos += event.GetPosition() - m_SaveGrabPoint;

        wxSize size = GetClientSize();
        wxRect rectUpdate(wxPoint(0, 0), size);

        RefreshRect(rectUpdate);
        Update();

        m_nCurrentGrabImageNum = -1;
    }
    else {
        wxRect rt;
        rt.x = m_SavePrevRect.x;
        rt.y = m_SavePrevRect.y;
        rt.width = event.GetPosition().x - m_SavePrevRect.x;
        rt.height = event.GetPosition().y - m_SavePrevRect.y;

        if(rt.width < 0) {
            rt.x += rt.width;
            rt.width *= -1;
        }

        if(rt.height < 0) {
            rt.y += rt.height;
            rt.height *= -1;
        }

        if(rt.height > 0 && rt.width > 0) {
            cv::Mat cvImage(rt.height, rt.width, CV_8UC3);

            wxPoint ScrolledPosition;
            CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

            for(int y = 0 ; y < rt.height ; ++y)
                for(int x = 0 ; x < rt.width ; ++x) {

                    cvImage.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);

                    for (CKcImage& kcImage : ((CParentFrame*)(GetParent()))->m_ImageList) {
                        wxPoint pos = wxPoint(rt.x, rt.y)+wxPoint(x, y)-ScrolledPosition;
                        wxRect rtImage(kcImage.pos, wxSize(kcImage.cvImage.cols, kcImage.cvImage.rows));

                        if(rtImage.Contains(pos)) 
                            cvImage.at<cv::Vec3b>(y,x) = kcImage.cvImage.at<cv::Vec3b>(
                                y+rt.y-(kcImage.pos.y+ScrolledPosition.y), x+rt.x-(kcImage.pos.x+ScrolledPosition.x));
                    }
                }

            DisplayImage(cvImage, rt.x+rt.width-ScrolledPosition.x, rt.y-ScrolledPosition.y, true, false);
        }

        m_nSelRegionIndex = -1;
    }
}

void CClientView::OnMouseLeftDblClk(wxMouseEvent& event) {
    return;

    wxPoint ScrolledPosition;
    CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

    m_nCurrentGrabImageNum = GetPosImage(m_SaveGrabPoint-ScrolledPosition);

    if (m_nCurrentGrabImageNum >= 0) {
        CKcImage kcImage;
        kcImage.cvImage = ((CParentFrame*)(GetParent()))->m_ImageList[m_nCurrentGrabImageNum].cvImage.clone();
        kcImage.pos = ((CParentFrame*)(GetParent()))->m_ImageList[m_nCurrentGrabImageNum].pos;

        ((CParentFrame*)(GetParent()))->ClearAllImages();
        ((CParentFrame*)(GetParent()))->m_ImageList.push_back(kcImage);

        Refresh();
        Update();

        m_nCurrentGrabImageNum = -1;
        m_nLastSelImageNum = 0;
    }
}

void CClientView::OnContextMenu(wxContextMenuEvent& event) {
    m_ContextPoint = event.GetPosition();

    if ((m_ContextPoint.x == -1) && (m_ContextPoint.y == -1))
    {
        wxSize size = GetSize();
        m_ContextPoint.x = size.x / 2;
        m_ContextPoint.y = size.y / 2;
    }
    else
    {
        m_ContextPoint = ScreenToClient(m_ContextPoint);
    }

    wxMenu* popupMenu = new wxMenu;

    popupMenu->Append(IDM_CONTEXT_COPY, "&Copy\tCtrl+C");
 //   popupMenu->Append(IDM_CONTEXT_PASTE, "&Paste\tCtrl+V");
 //   popupMenu->Append(IDM_CONTEXT_DUPLICATE, "&Duplicate\tCtrl+D");
 //   popupMenu->Append(IDM_CONTEXT_ZOOM_IN, "Zoom &In\tCtrl+UP");
 //   popupMenu->Append(IDM_CONTEXT_ZOOM_OUT, "Zoom &Out\tCtrl+DOWN");
 //   popupMenu->Append(wxID_SEPARATOR);
 //   popupMenu->Append(IDM_CONTEXT_SEL_REGION, "Sel Region");
    popupMenu->Append(wxID_SEPARATOR);
    popupMenu->Append(IDM_CONTEXT_SET_QUAD, "Set Quad Region\tCtrl+Q");
    popupMenu->Append(IDM_CONTEXT_SET_LABEL, "Set Label\tCtrl+L");
    popupMenu->Append(IDM_CONTEXT_SAVE_NEXT, "Save Next\tCtrl+A");
    popupMenu->Append(IDM_CONTEXT_DELETE_QUAD, "Delete Region\tCtrl+W");
    popupMenu->Append(wxID_SEPARATOR);
    popupMenu->Append(IDM_CONTEXT_BRING_FRONT, "Bring to Front");
    popupMenu->Append(IDM_CONTEXT_SEND_BACK, "Send to Back");

    PopupMenu(popupMenu, m_ContextPoint);

    wxPoint ScrolledPosition;
    CalcScrolledPosition(0, 0, &(ScrolledPosition.x), &(ScrolledPosition.y));

    m_nLastSelImageNum = GetPosImage(m_ContextPoint-ScrolledPosition);
}

void CClientView::OnCopy(wxCommandEvent& event) {
    if (m_nLastSelImageNum < 0 || ((CParentFrame*)GetParent())->m_ImageList.size() <= m_nLastSelImageNum) return;

    cv::Mat cvImage;
    cvImage = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum].cvImage.clone();

    wxImage::AddHandler(new wxPNGHandler); 
    cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
    wxImage image(cvImage.cols, cvImage.rows, cvImage.data, true);
    wxBitmap bitmap = image;

    if (wxTheClipboard->Open()) {  
        wxBitmapDataObject bitmapDataObject(bitmap);
        wxTheClipboard->SetData(new wxBitmapDataObject(bitmap)); 
        wxTheClipboard->Close();
    } 
}

void CClientView::OnPaste(wxCommandEvent& event) {
    if (wxTheClipboard->Open()) {
        if (wxTheClipboard->IsSupported(wxDF_BITMAP)) {
            wxBitmapDataObject data;
            wxTheClipboard->GetData(data);
            wxBitmap bitmap = data.GetBitmap();

            wxImage image = bitmap.ConvertToImage();

            cv::Mat cvImage(image.GetHeight(), image.GetWidth(), CV_8UC3);

            memcpy(cvImage.data, image.GetData(), image.GetWidth()* image.GetHeight() * 3);
            cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
            //DisplayImage(cvImage, m_ContextPoint.x, m_ContextPoint.y, true, false);

            wxRect rectUpdate = GetUpdateRegion().GetBox();
            CalcUnscrolledPosition(rectUpdate.x, rectUpdate.y,
                &rectUpdate.x, &rectUpdate.y);
            DisplayImage(cvImage, rectUpdate.x, rectUpdate.y, true, false);

        }
        wxTheClipboard->Close();
    }
}

void CClientView::OnDuplicate(wxCommandEvent& event) {
    if (m_nLastSelImageNum < 0 || ((CParentFrame*)GetParent())->m_ImageList.size() <= m_nLastSelImageNum) return;

    CKcImage kcImage = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum];
    cv::Mat cvImage = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum].cvImage.clone();

    DisplayImage(cvImage, kcImage.pos.x+kcImage.cvImage.cols, kcImage.pos.y, true, false);
}

void CClientView::OnZoomIn(wxCommandEvent& event) {
    if (m_nLastSelImageNum < 0 || ((CParentFrame*)GetParent())->m_ImageList.size() <= m_nLastSelImageNum) return;

    CKcImage kcImage = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum];
    cv::Mat cvImage;
    cv::Size cvSize = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum].cvImage.size();

    cv::resize(((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum].cvImage, cvImage,
        cvSize*2, 0, 0, cv::INTER_CUBIC);

    DisplayImage(cvImage, kcImage.pos.x+kcImage.cvImage.cols, kcImage.pos.y, true, false);
}

void CClientView::OnZoomOut(wxCommandEvent& event) {
    if (m_nLastSelImageNum < 0 || ((CParentFrame*)GetParent())->m_ImageList.size() <= m_nLastSelImageNum) return;

    CKcImage kcImage = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum];
    cv::Mat cvImage;
    cv::Size cvSize = ((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum].cvImage.size();

    cv::resize(((CParentFrame*)GetParent())->m_ImageList[m_nLastSelImageNum].cvImage, cvImage,
        cvSize/2, 0, 0, cv::INTER_CUBIC);

    DisplayImage(cvImage, kcImage.pos.x+kcImage.cvImage.cols, kcImage.pos.y, true, false);
}

void CClientView::OnSelRegion(wxCommandEvent& event) {
    m_nSelRegionIndex = 1;

    m_SaveMousePos = wxPoint(-10, -10);
}

void CClientView::OnSetQuad(wxCommandEvent& event) {
    m_nSelQuadIndx = 0;
}

void CClientView::OnSetLabel(wxCommandEvent& event) {
    if (m_nRightSelQuadNum < 0) return;

    std::stringstream ss;
    ss << "Label #: " << m_nRightSelQuadNum << " (" << g_className[m_RegionVector[m_nRightSelQuadNum].nClass] << ")";
    CLabelDialog dlg(this, m_RegionVector[m_nRightSelQuadNum].nClass, ss.str());
    if (dlg.ShowModal() == wxID_OK) {
        int nClass = dlg.m_pClassCombo->GetSelection();   

        if (m_RegionVector[m_nRightSelQuadNum].nClass != nClass) {
            DlgPrintf("%s -> %s", (std::string)g_className[m_RegionVector[m_nRightSelQuadNum].nClass], (std::string)g_className[nClass]);
            m_RegionVector[m_nRightSelQuadNum].nClass = nClass;
            m_RectVector[m_nRightSelQuadNum].nClass = nClass;

            m_bModified = true;
        }
    }
}

void CClientView::OnSaveNext(wxCommandEvent& event) {
    CMainDialog* pMainDialog = wxGetApp().m_pMainDialog;

    wxCommandEvent newEvent(wxEVT_COMMAND_BUTTON_CLICKED, IDC_SAVE_NEXT);
    pMainDialog->ProcessEvent(newEvent);
}

void CClientView::OnDeleteQuad(wxCommandEvent& event) {
    if (m_nRightSelQuadNum < 0) return;

    int nLastIndex = m_RegionVector.size() - 1;
    std::swap(m_RegionVector[m_nRightSelQuadNum], m_RegionVector[nLastIndex]);
    std::swap(m_RectVector[m_nRightSelQuadNum], m_RectVector[nLastIndex]);

    m_RegionVector.erase(m_RegionVector.end() - 1);
    m_RectVector.erase(m_RectVector.end() - 1);

    Refresh(true);
    m_bModified = true;
}


void CClientView::OnBringFront(wxCommandEvent& event) {
    if (m_nRightSelQuadNum < 0) return;

    int nLastIndex = m_RegionVector.size() - 1;
    std::swap(m_RegionVector[m_nRightSelQuadNum], m_RegionVector[nLastIndex]);
    std::swap(m_RectVector[m_nRightSelQuadNum], m_RectVector[nLastIndex]);

    Refresh(true);
    m_bModified = true;
}

void CClientView::OnSendBack(wxCommandEvent& event) {
    if (m_nRightSelQuadNum < 0) return;

    std::swap(m_RegionVector[m_nRightSelQuadNum], m_RegionVector[0]);
    std::swap(m_RectVector[m_nRightSelQuadNum], m_RectVector[0]);

    Refresh(true);
    m_bModified = true;
}

void CClientView::SaveLabel(const char* filename) {
    if (m_RegionVector.size() <= 0) return;

    std::string filepath1 = m_LabelPath + "/" + filename, filepath;
    int pos = filepath1.rfind(".");
    filepath = filepath1.substr(0, pos);
    std::ofstream ofs(filepath + ".txt");

    if (ofs) {
        for (int i = 0; i < m_RegionVector.size(); ++i) {
            ofs << i << ", " << m_RegionVector[i].p0.x << ", " << m_RegionVector[i].p0.y
                << ", " << m_RegionVector[i].p1.x << ", " << m_RegionVector[i].p1.y
                << ", " << m_RegionVector[i].p2.x << ", " << m_RegionVector[i].p2.y
                << ", " << m_RegionVector[i].p3.x << ", " << m_RegionVector[i].p3.y
                << ", " << g_className[m_RegionVector[i].nClass] << '\n';

            ofs << i << ", " << m_RectVector[i].left << ", " << m_RectVector[i].top
                << ", " << m_RectVector[i].right << ", " << m_RectVector[i].bottom
                << ", " << g_className[m_RegionVector[i].nClass] << '\n';
        }

        ofs.close();
    }
}

void CClientView::ReadLabel(const char* filename) {
    std::string filepath1 = m_LabelPath + "/" + filename, filepath;
    int pos = filepath1.rfind(".");
    filepath = filepath1.substr(0, pos);
    std::ifstream ifs(filepath + ".txt");

    if (ifs) {
        DlgPrintf("%s file was opened.", filename);
        while (true) {
            int i;
            CKcQuad KcQuad;
            CKcRect KcRect;
            std::string className, Comma;

            ifs >> i >> Comma >> KcQuad.p0.x >> Comma >> KcQuad.p0.y
                >> Comma >> KcQuad.p1.x >> Comma >> KcQuad.p1.y
                >> Comma >> KcQuad.p2.x >> Comma >> KcQuad.p2.y
                >> Comma >> KcQuad.p3.x >> Comma >> KcQuad.p3.y
                >> Comma >> className;

            if (!ifs.good()) break;

            ifs >> i >> Comma >> KcRect.left >> Comma >> KcRect.top
                >> Comma >> KcRect.right >> Comma >> KcRect.bottom
                >> Comma >> className;

            if (!ifs.good()) break;

            for (int i = 0; i < g_classNum; ++i) {
                if (className == g_className[i]) {
                    KcQuad.nClass = i;
                    KcRect.nClass = i;

                    break;
                }
            }

            m_RegionVector.push_back(KcQuad);
            m_RectVector.push_back(KcRect);
        }

        ifs.close();
    }
    
    Refresh(true);
}

void CClientView::ClearLabels() {
    m_RegionVector.clear();
    m_RectVector.clear();

    Refresh(true);
}