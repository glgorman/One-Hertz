// CSignalView.cpp : implementation of the CSignalView class
// Copyright 2025 Gerold Lee Gorman.  MIT License.

#include "stdafx.h"
#include "math.h"
#include "intrinsics.h"
#include "tensors.h"
#include "euclid.h"
#include "One Hertz.h"
#include "CSignalView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDT_RENDER 1024
#define nullptr NULL

const COLORREF color [] = 
{
	COLOR::black,
	COLOR::blue,
	COLOR::red,
	COLOR::magenta,
	COLOR::green,
	COLOR::cyan,
	COLOR::yellow,
	COLOR::gray,
	COLOR::white,
	COLOR::dark_red,
};

// CSignalView

CSignalView::CSignalView()
{
}

CSignalView::~CSignalView()
{
}


BEGIN_MESSAGE_MAP(CSignalView, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER ()
END_MESSAGE_MAP()

void CSignalView::calculate_page_info (CDC *dc)
{
	UINT h_res, v_res, h_size_mm, v_size_mm;
	float physical_width, physical_height;
	float pixel_offset_x, pixel_offset_y;

	physical_width = dc->GetDeviceCaps(PHYSICALWIDTH);
	physical_height = dc->GetDeviceCaps(PHYSICALHEIGHT);
	pixel_offset_x = dc->GetDeviceCaps(PHYSICALOFFSETX);
	pixel_offset_y = dc->GetDeviceCaps(PHYSICALOFFSETY);

	h_res = dc->GetDeviceCaps(HORZRES);
	v_res = dc->GetDeviceCaps(VERTRES);
	h_size_mm = dc->GetDeviceCaps(HORZSIZE);
	v_size_mm = dc->GetDeviceCaps(VERTSIZE);

	if (m_bIsPrinting==true)
	{
		m_x_dpi =	dc->GetDeviceCaps(LOGPIXELSY);
		m_y_dpi =	dc->GetDeviceCaps(LOGPIXELSY);
		m_page_width = physical_width/m_x_dpi;
		m_page_height = physical_height/m_y_dpi;
	}
	else {
	// this allows us to fill the screen
	// with a "to scale" grid pattern when
	// using a reasonable sized monitor
		m_x_dpi = (h_res*25.4)/h_size_mm+0.5;
		m_y_dpi = (v_res*25.4)/v_size_mm+0.5;
		m_page_width = 17.0;
		m_page_height = 11.0;
	}
	m_printer_x = float(pixel_offset_x)/m_x_dpi;
	m_printer_y = float(pixel_offset_y)/m_y_dpi;
}

BOOL CSignalView::OnEraseBkgnd(CDC* pDC) 
{
	m_update = true;
	return TRUE;
}

void CSignalView::OnSize(UINT nType, int cx, int cy) 
{
	bool m_bOpenGL=false;
	writeln (output,"CSignalView::OnSize: ",cx,", ",cy);
	CView::OnSize(nType, cx, cy);
	Invalidate(TRUE);
	if (m_bOpenGL==true)
	{
		if ((cx<=0)||(cy<=0)) 
			return;
	}
}

// CSignalView message handlers

BOOL CSignalView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

int CSignalView::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
	writeln (output,"CSignalView::OnCreate\r\n") ;
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    CClientDC dc(this);
	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);   
    pfd.nVersion = 1 ;                           // Version number
	pfd.dwFlags =  PFD_DOUBLEBUFFER |        // Use double buffer
	               PFD_SUPPORT_OPENGL |          // Use OpenGL
	               PFD_DRAW_TO_WINDOW ;          // Pixel format is for a window.
		pfd.iPixelType = PFD_TYPE_RGBA ;
    pfd.cColorBits = 24;                         // 8-bit color
	pfd.cDepthBits = 32 ;					 // 32-bit depth buffer
    pfd.iLayerType = PFD_MAIN_PLANE ;            // Layer type

    int nPixelFormat = ChoosePixelFormat(dc.m_hDC, &pfd);
		if (nPixelFormat == 0)
		{
		TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	TRACE("Pixel Format %d\r\n", nPixelFormat) ;
    BOOL bResult = SetPixelFormat(dc.m_hDC, nPixelFormat, &pfd);
	if (!bResult)
	{
		TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
#ifdef HAVE_OPEN_GL
	m_hrc = wglCreateContext(dc.m_hDC);
	if (!m_hrc)
	{
		TRACE("wglCreateContext Failed %x\r\n", GetLastError()) ;
		return -1;
	}
	const GLubyte *glu_version = gluGetString (GLU_VERSION);
	TRACE ("GLU version %s\n",glu_version);
	CreateRGBPalette(dc.m_hDC) 
#endif
	m_idtRender = SetTimer(IDT_RENDER,250,NULL);
	return 0;
}

void CSignalView::OnTimer (UINT timer)
{	 
	int tick = GetTickCount();
//	writeln (output,"Timer Message: Tick = ",tick);
	switch (timer)
	{	
	case IDT_RENDER:
		Invalidate (TRUE);
		break;

	default:
		CWnd::OnTimer (timer);
		break;
	}
}

void CSignalView::OnInitialUpdate()
{
//	m_dbgDoc.create ((CWnd*)this);
	Invalidate(TRUE);
	m_update = true;
}

void CSignalView::OnPaint() 
{
	PAINTSTRUCT ps;
	CDC* pDC = BeginPaint(&ps);
	calculate_page_info (pDC);
	SelectObject (pDC->m_hDC,GetStockObject(DC_PEN));
	SelectObject (pDC->m_hDC,GetStockObject(DC_BRUSH));
	OnDraw (pDC);
	EndPaint(&ps);	
}

void CSignalView::OnDrawGDI (CDC *dc)
{
	CSignalView *ptr;
	ptr = reinterpret_cast<CSignalView*> (this);
	euclid::RenderProc (dc, ptr);
}

void CSignalView::OnDraw(CDC *dc) 
{
	HENHMETAFILE m_hm=NULL;
	ENHMETAHEADER	mheader;
	CDC offscreen;
	CDC	*render = NULL;
	char *fname = NULL;
	int w,h,x,y;
	CBitmap displayBmp;
	DWORD dBlitMode;

	bool m_bInverse=false;
	bool bIsPrinting=false;
	bool buffer=true;
	bool m_use_metafile=false;
	
	if (dc->m_bPrinting==TRUE)
	{
		int hres, vres;
		hres = dc->GetDeviceCaps(HORZRES);
		vres = dc->GetDeviceCaps(VERTRES);	
		m_use_metafile=false;
		w = hres;
		h = vres;
		m_update_region = CRect (0,0,w,h);
		m_update=true;
		m_bIsPrinting = true;
		render = dc;
	}
	else
	{
		GetClientRect (&m_update_region);
		w = m_update_region.Width();
		h = m_update_region.Height();
		bIsPrinting = false;
//		CBrush blkBrush (COLOR::black);
//		dc->FillRect (&m_update_region,&blkBrush);
	}
//	The idea is that we might want to update a drawing
//	once, and then save it as a metafile, which can
//	then be rendered over and over again, for certain
//	types of objects, without having to redo any
//	complicated geometry, most of the time, that is.
	if ((m_use_metafile==true)&&(m_update==true))
	{
		if (m_hm!=NULL)
		{
			DeleteEnhMetaFile (m_hm);
			m_hm = NULL;
		}	 
		GetEnhMetaFileHeader(m_hm,sizeof(mheader),&mheader);
		m_update = false;
	}
	if ((buffer==true)&&(bIsPrinting==FALSE))
	{	
//  		writeln (output,"Selecting Offscreen Bitmap for rendering");
		displayBmp.CreateCompatibleBitmap (dc,w,h);
		offscreen.CreateCompatibleDC (dc);
		offscreen.SelectObject (&displayBmp);
		render = &offscreen;
		render->SetBkColor (COLOR::black);
		render->SetTextColor (COLOR::yellow);
		CBrush *pBrush = dc->GetCurrentBrush ();
		CBrush bkgrnd (COLOR::dark_red);
		render->FillRect (&m_update_region,&bkgrnd);
		if (m_use_metafile==true)
			PlayEnhMetaFile (render->m_hDC,m_hm,&m_update_region);
		else
			OnDrawGDI (render);

		CRect &r = m_update_region;
		x = r.left;
		y = r.top;
		w = r.Width ();
		h = r.Height ();
		dBlitMode = (m_bInverse==false?SRCCOPY:NOTSRCCOPY);		
		dc->BitBlt (x,y,w,h,render,x,y,dBlitMode);
	}
	m_update = false;
}

LRESULT CSignalView::OnRxString (WPARAM w, LPARAM l)
{
	bool m_debug_trace = false;
	int tick = GetTickCount();
	double dtick = ((double) tick)/1000.0;
	LRESULT res = 0;
	char *rx_buffer = (char*)w;
	CString msg(rx_buffer);
	CString tickStr;
	tickStr.Format(_T("%6.3lf: "),dtick);
	delete rx_buffer;
	if (m_debug_trace==false)
		return res;

	CString m_text;
	m_text.Append(tickStr);
	m_text.Append (msg);
	m_text.Append (_T("\r\n"));
	int sz = m_text.GetLength(); 
	if (sz>2048)
		m_text = m_text.Right(8200);
	m_update = true;
	return res;
}

CRect CSignalView::compute_metafile_size (CDC *dc)
{
	int x_dpi, y_dpi, w, h, x_offset, y_offset; 
	int h_size_mm, v_size_mm;
	int h_res, v_res;
	bool b_set_offset = true;

	h_res = dc->GetDeviceCaps(HORZRES);
	v_res = dc->GetDeviceCaps(VERTRES);
	h_size_mm = dc->GetDeviceCaps(HORZSIZE);
	v_size_mm = dc->GetDeviceCaps(VERTSIZE);
	if (b_set_offset==true)
	{
		x_offset = dc->GetDeviceCaps(PHYSICALOFFSETX);
		y_offset = dc->GetDeviceCaps(PHYSICALOFFSETY);
	}
	else
	{
		x_offset = 0;
		y_offset = 0;
	}
	if (dc->m_bPrinting==TRUE) 
	{
		w = h_res+x_offset; //dc->GetDeviceCaps (PHYSICALWIDTH);
		h = v_res+y_offset; //dc->GetDeviceCaps (PHYSICALHEIGHT);
		x_dpi = dc->GetDeviceCaps(LOGPIXELSX);
		y_dpi = dc->GetDeviceCaps(LOGPIXELSY);
	}
	else {
		w =  m_update_region.Width();
		h = m_update_region.Height();
		x_dpi = (h_res*25.4)/h_size_mm+0.5;
		y_dpi = (v_res*25.4)/v_size_mm+0.5;
	}
	CRect sz;
	sz.top = (2540.0*y_offset)/y_dpi;
	sz.left = (2540.0*x_offset)/x_dpi;
	sz.bottom = (2540.0*h)/y_dpi;
	sz.right = (2540.0*w)/x_dpi;
	return sz;
}

HENHMETAFILE CSignalView::render_scene_to_metafile (CDC *dc,char *fname)
{
	CMetaFileDC mf;
	CRect sz;
	BOOL res;
	CDC *dc2 = CWnd::GetDC();
	sz = compute_metafile_size (dc);
	try {
		if (fname!=NULL)
			res = mf.CreateEnhanced (dc,(const CString)fname,&sz,NULL);
		else
			res = mf.CreateEnhanced (dc,NULL,&sz,NULL);
		
		mf.SetAttribDC (dc2->m_hDC);
	}
	catch (...)
	{
		writeln (output,"Metafile error: DC exception!");
		ASSERT(false);
		return NULL;
	}
	mf.SetGraphicsMode (GM_ADVANCED);
	// todo - add additional stuff for drawing border
	// or not drawing border.
	
	OnDrawGDI (&mf);
	m_hm = mf.CloseEnhanced ();
	if (m_bIsPrinting==true)
	{
		if( OpenClipboard() )
            {
                EmptyClipboard();
                SetClipboardData(CF_ENHMETAFILE, m_hm);
                CloseClipboard();
            }
	}
	ReleaseDC (dc2);
	return m_hm;
}

