//  ClientView.h: interface of CClientView (client view of child frame)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#pragma once

struct CKcPoint {
	int x, y;

	CKcPoint() : x(0), y(0) {}
	CKcPoint(int x, int y) : x(x), y(y) {};
	CKcPoint(const wxPoint &wxP) : x(wxP.x), y(wxP.y) {}

	operator wxPoint() const {
		return wxPoint(x, y);
	}
	bool operator > (const CKcPoint p) {
		if (x == p.x) {
			return y > p.y;
		}
		return x > p.x;
	}
	bool operator <= (const CKcPoint p) {
		return x <= p.x;
	}
};
struct CKcLine {
	CKcPoint p0, p1;

	CKcLine() {}
	CKcLine(const CKcPoint& p0, const CKcPoint& p1) : p0(p0), p1(p1) {}
	CKcLine(const wxPoint& p0, const wxPoint& p1) : p0(p0), p1(p1) {}
};
struct CKcQuad {
	CKcPoint p0, p1, p2, p3;
	int nClass;

	CKcQuad() : nClass(0)  {}
	CKcQuad(const CKcPoint* KcP) : p0(KcP[0]), p1(KcP[1]), p2(KcP[2]), p3(KcP[3]), nClass(0) {}
	CKcQuad(const wxPoint* wxP) : p0(wxP[0]), p1(wxP[1]), p2(wxP[2]), p3(wxP[3]), nClass(0) {}
};
struct CKcRect {
	int left, top, right, bottom;
	int nClass;

	CKcRect() : left(0), top(0), right(0), bottom(0), nClass(0) {}
	CKcRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b), nClass(0) {}
};

class CClientView : public wxScrolled<wxWindow>
{
public:
#ifndef _KHUCV_SDI
	CClientView(wxMDIChildFrame* parent);
#else
	CClientView(wxWindow* parent);
#endif
	friend class CMainFrame;

private:
	void OnPaint(wxPaintEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnMouseLeftDblClk(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);

	int GetPosImage(const wxPoint &pos);

	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	void OnDuplicate(wxCommandEvent& event);
	void OnZoomIn(wxCommandEvent& event);
	void OnZoomOut(wxCommandEvent& event);
	void OnSelRegion(wxCommandEvent& event);
	void OnSetQuad(wxCommandEvent& event);
	void OnSetLabel(wxCommandEvent& event);
	void OnSaveNext(wxCommandEvent& event);
	void OnDeleteQuad(wxCommandEvent& event);
	void OnBringFront(wxCommandEvent& event);
	void OnSendBack(wxCommandEvent& event);

	int m_nCurrentGrabImageNum;

	wxPoint m_SaveGrabPoint, m_ContextPoint, m_SaveMousePos;
	wxRect m_SavePrevRect;

	int m_nSelRegionIndex;
    bool m_bLeftIsDown;

	int m_nSelQuadIndx;
	int m_nCurrentSelQuadNum;
	int m_nCurrentSelQuadPointNum;
	int m_nCurrentSelRectPointNum;
	int m_nRightSelQuadNum, m_nPrevRightSelQuadNum;
	wxPoint m_SaveDownPos;
	std::vector<CKcQuad> m_RegionVector;
	std::vector<CKcRect> m_RectVector;
	wxPoint m_QuadPt[4];
	bool m_bPreviousRefresh;
	
public:
	int m_nCurrentClassIndex;
	std::string m_LabelPath;
	int m_Scale;

	int m_nLastSelImageNum;
	bool m_bModified;

	void SaveLabel(const char *filename);
	void ReadLabel(const char* filename);
	void ClearLabels();

	DECLARE_EVENT_TABLE();
};

