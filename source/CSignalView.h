// CSignalView.h : interface of the CSignalView class
//
#pragma once
#include "afx.h"
#include "cstring"

namespace COLOR
{
const COLORREF black = RGB (0,0,0);
const COLORREF red = RGB (255,0,0);
const COLORREF green = RGB (0,255,0);
const COLORREF blue = RGB (0,0,255);
const COLORREF yellow = RGB (255,255,0);
const COLORREF cyan = RGB (0,255,255);
const COLORREF magenta = RGB (255,0,255);
const COLORREF light_green = RGB (0,64,0);
const COLORREF dark_red = RGB (48,0,0);

const COLORREF brown0 = RGB (224,192,64);
const COLORREF brown1 = RGB (224,192,96);
const COLORREF brown2 = RGB (224,192,128);

const COLORREF gray = RGB (128,128,128);
const COLORREF mirror_gray = RGB (192,192,192);
const COLORREF off_white = RGB (240,240,240);
const COLORREF white = RGB (255,255,255);
extern COLORREF randColor ();
}

class SETCOLOR 
{
#define THICKNESS 2

private:
	CPen	*m_pOldPen, m_pen;
	CBrush	*m_pOldBrush, m_brush;
//	camera	*m_pCamera;
	CDC		*m_pCDC;

public:
#if 0
	SETCOLOR (camera &a, COLORREF c1, COLORREF c2)
	{
		m_pCamera = &a;
		m_pCDC = m_pCamera->m_graph;
		m_pen.CreatePen (PS_SOLID,THICKNESS,c1);
		m_brush.CreateSolidBrush (c2);
		m_pOldPen = m_pCDC->SelectObject (&m_pen);
		m_pOldBrush = m_pCDC->SelectObject (&m_brush);
	}
#endif
	SETCOLOR (CDC *pCDC, COLORREF c, int thickness=THICKNESS)
	{
		m_pCDC = pCDC;
		m_pen.CreatePen (PS_SOLID,thickness,c);
		m_brush.CreateSolidBrush (c);
		m_pOldPen = m_pCDC->SelectObject (&m_pen);
		m_pOldBrush = m_pCDC->SelectObject (&m_brush);
	}
#if 0
	SETCOLOR (camera &a, COLORREF c)
	{
		m_pCamera = &a;
		m_pCDC = m_pCamera->m_graph;
		m_pen.CreatePen (PS_SOLID,THICKNESS,c);
		m_brush.CreateSolidBrush (c);
		m_pOldPen = m_pCDC->SelectObject (&m_pen);
		m_pOldBrush = m_pCDC->SelectObject (&m_brush);
	};
#endif
	~SETCOLOR ()
	{
		m_pCDC->SelectObject (m_pOldPen);
		m_pCDC->SelectObject (m_pOldBrush);
	}
};

// CSignalView window

class CSignalView : public CView
{
friend class CMainFrame;

public:
	CSignalView();

// Attributes
public:
	HENHMETAFILE m_hm;
	HGLRC	m_hrc; 			//OpenGL Rendering Context
	bool m_bIsPrinting;
	bool m_update;
	double m_page_width;
	double m_page_height;
	int m_printer_x;
	int m_printer_y;
	int	m_x_dpi;
	int	m_y_dpi;
	CRect m_update_region;
//	CString m_description;
//	CString m_text;update;

protected:
	UINT m_idtRender;

// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void calculate_page_info (CDC *dc);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnDrawGDI (CDC *dc);

// Implementation
public:
	virtual ~CSignalView();

	// Generated message map functions
protected:
	LRESULT OnRxString (WPARAM w, LPARAM l);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer (UINT timer);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitialUpdate();
	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
	CRect compute_metafile_size (CDC *dc);
	HENHMETAFILE render_scene_to_metafile (CDC *dc,char *fname);
	
	DECLARE_MESSAGE_MAP()
};


