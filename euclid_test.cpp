

#include "stdafx.h"
#include "math.h"
#include "tensors.h"
//#include "../rubidium/CameraView.h"
#include "CSignalView.h"
#include "intrinsics.h"

#define ONE_HERTZ
#include "euclid.h"

#define	TBASE	12.0
#define THEIGHT	10.0
#define TXBIAS	10.0
#define TYBIAS	0.0

int euclid::RenderProc (CDC *pdc,  CView *where)
{
	euclid e;
	euclidian master;	
	euclidian::bind (e,master);
	
	e.m_pdc = pdc;

#if 0
	
	CView *pre1 = static_cast<CView*>(view);
	CView *ptr2 = static_cast<CView*>(where);
	
	view->m_bIsPrinting = where->m_bIsPrinting;
	view->m_x_dpi = where->m_x_dpi;
	view->m_y_dpi = where->m_y_dpi;
	view->page_width = where->page_height;
	view->m_printer_x = where->m_printer_y;
	view->m_update = true;
#endif
	CView *view = new CSignalView;

	e.m_pView = static_cast<CSignalView*>(view);

	e.m_pView->GetClientRect (&master.m_host->m_bounds);
	e.init ();
	
	e.draw_border ();	
	bool printing;
	printing = pdc->IsPrinting();
	bool draw_grid = false;
	if (draw_grid==true)
		e.draw_grid ();

	SETCOLOR _(master.m_host->m_pdc,COLOR::red);
	master.m_host->m_scale = 0.25;
	master.draw_clock ();
	return 0;
}

#ifdef ONE_HERTZ
int euclid::RenderProc (CDC *pdc,  CSignalView *where)
#else
int euclid::RenderProc (CDC *pdc,  CCameraView *where)
#endif
{
	euclid e;
	euclidian master;	
	euclidian::bind (e,master);
	
	e.m_pdc = pdc;
	e.m_pView = where;
	e.m_pView->GetClientRect (&master.m_host->m_bounds);
	e.init ();

	bool fit_to_window = true;
	bool draw_grid = true;

	if (fit_to_window==true)
		master.m_host->m_scale = 0.50;

	e.draw_border ();	
	bool printing;
	printing = pdc->IsPrinting();
	
	if (draw_grid==true)
		e.draw_grid ();

	SETCOLOR _(master.m_host->m_pdc,COLOR::red);
	master.m_host->m_scale = 0.125;

//	triangle_test test;
//	euclidian::bind (e,test.T);
//	euclidian::bind (e,test.circle);
//	test.main();

//	test.circle.from_triangle (test.T);
//	fpoint radial = test.circle.m_center;
//	radial.x = radial.x+test.circle.m_radius;
//	e.draw_polygon (test.circle.m_center,radial);

//	pentagon pent = master.construct_pentagon (true);
//	decagon deca = master.construct_decagon (true);
//	hexagon hex = master.construct_hexagon (true);

	master.draw_clock ();
//	euclidian::bind (e,deca);
//	euclidian::bind(e,hex);
//	deca.on_draw ();
//	hex.on_draw ();
	
	return 0;
}


double euclidian::angle_test ()
{
	SETCOLOR _(m_host->m_pdc,COLOR::white);
	m_host->m_pView->GetClientRect (&m_host->m_bounds);
	bool draw_bisectors = false;
	COLORREF bcolor;

	DWORD t1 = GetTickCount();
	double phi1,phi2,tick;
	tick = (t1%1000)*0.001;
	double sec = ((t1/100)%600)*0.1;
	phi1 = 360.0-6.0*sec;

	box bounds;
	bind(*m_host,bounds);
	bounds.construct ();
	bounds.on_draw ();

	fpoint center = bounds.center();
	fline L1 = fline (center,TO_RADIANS(phi1));
	angle A = angle(L1);
	fline L2 = fline (center,-A.theta);
	fpoint pt6 = fpoint::project (center,A,3.0);
	{
		SETCOLOR _(m_host->m_pdc,COLOR::yellow);
		m_host->draw_line (center,pt6);
	}
	return phi1;
}

fpoint test [] = 
{
	fpoint (TXBIAS,TYBIAS),
	fpoint (TXBIAS+0.5*TBASE,TYBIAS+THEIGHT),
	fpoint (TXBIAS+TBASE,TYBIAS),
};

triangle_test::triangle_test ()
{
	create_points ();
	T.calculate_lines ();
	T.recompute_points ();
	T.generate_midpoints ();
	T.compute_centroid ();
}

int triangle_test::create_points ()
{
// create three points on the plane
	T.reference[0] = test[0]; 
	T.reference[1] = test[1]; 
	T.reference[2] = test[2]; 
	return 0;
}

int triangle_test::main ()
{
	bool b_draw_midpoints;
	bool b_draw_bisectors;
	b_draw_midpoints = true;
	b_draw_bisectors = true;
	T.draw_aux = false;

	DWORD t = GetTickCount();
	MATH_TYPE h = 31.0;
	
	MATH_TYPE theta = T.angle_test ();
	T.rotate_points (TO_RADIANS(theta));
	T.recompute_lines ();
	T.recompute_points ();
	T.generate_midpoints ();
	T.generate_bisectors (true);
	T.draw_triangle1 (T.m_host->m_scale);
	T.draw_incircle();

	if (b_draw_midpoints==true)
	{
		T.draw_midpoints (T.m_host->m_scale);
	}
	if (b_draw_bisectors==true)
	{	
		T.draw_bisectors (T.m_host->m_scale);
	}
	return 0;
}

solar_plot::solar_plot ()
{

}

int solar_plot::create_data ()
{

	return 0;
}

int solar_plot::main ()
{
	return 0;
}

