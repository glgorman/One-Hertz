
#include "stdafx.h"
#define ONE_HERTZ 1

//#include "../rubidium/CameraView.h"
#include "math.h"
#include "CSignalView.h"
#include "tensors.h"
#include "intrinsics.h"
#include "euclid.h"

// line weights for different pencils

#define	_4H (0.1)
#define	_2H	(0.2)
#define	_H	(0.3)
#define	_F	(0.25)
#define	_HB	(0.4)

void page_info::set_margins (double i)
{
	top_margin = 0.5;
	left_margin = 0.5;
	right_margin = page_width-0.5;
	bottom_margin = page_height-0.5;
}

//	rotate the points in a specified point cloud by
//	tje specified specified point and through the
//	specfied angle.

void angle::rotate_points (vector<fpoint> &points, const fpoint &G1)
{
	double c,s;
	fpoint P1,P2;
	c = m_c;
	s = m_s;	
	int i,j;
	j = points.size();
	for (i=0;i<j;i++)
	{
		P1 = points[i]-G1;
		P2.x = P1.x*c-P1.y*s;
		P2.y = P1.x*s+P1.y*c;
		points[i] = P2+G1;
	}
}

euclid::euclid(void)
{
	m_objects.reserve (8);
	m_objects.resize (0);
	m_debug = false;
	m_offset = false;

}

euclid::~euclid(void)
{

}

bool euclidian::bind (euclid &p1, euclidian &p2)
{
	p2.m_host = &p1;
	size_t sz = p1.m_objects.size();
	p1.m_objects.resize (sz+1);
	p1.m_objects [sz] = &p2;
	return true;
}

void euclid::init ()
{	
	page.page_width = m_pView->m_page_width;
	page.page_height = m_pView->m_page_height;
	page.x_offset = m_pView->m_printer_x;
	page.y_offset = m_pView->m_printer_y;

	x_dpi = m_pView->m_x_dpi;
	y_dpi = m_pView->m_y_dpi;
	page.set_margins (0.5);
	m_scale = 1.00;
	
	if (m_pView->m_bIsPrinting==true)
	{
		m_c=COLOR::black;
	}
	else {
		m_c=COLOR::red;
	}	
	line_width = x_dpi/25.4;
	m_origin = fpoint (4.5,10.0);
	m_offset = true;
}

void euclid::draw_border ()
{
	SETCOLOR _(m_pdc,COLOR::gray,1+line_width*_HB);
#if 0
	writeln (output,"euclid::draw_border:");
#endif
	m_debug = false;
	bool temp = m_offset;
	m_offset = false;
	draw_line (page.left_margin,page.top_margin,page.right_margin,page.top_margin);
	draw_line (page.left_margin,page.bottom_margin,page.right_margin,page.bottom_margin);
	draw_line (page.left_margin,page.top_margin,page.left_margin,page.bottom_margin);
	draw_line (page.right_margin,page.top_margin,page.right_margin,page.bottom_margin);
	draw_line (page.left_margin,page.bottom_margin-page.top_margin,page.right_margin,page.bottom_margin-page.top_margin);
	draw_line (2*page.left_margin,page.bottom_margin-page.top_margin,2*page.left_margin,page.bottom_margin);
	m_debug = false;
	m_offset = temp;
}

void euclid::draw_grid ()
{
	SETCOLOR _(m_pdc,COLOR::gray);
#if 0
	writeln (output,"euclid::draw_grid:");
#endif
	double x_pos, y_pos;
//	int grid_spacing = GRID_SPACING*map_scale;
//	int grid_spacing = 1.0*m_scale;
	double grid_spacing1,view_offset;
	m_pView->GetClientRect (&m_bounds);
	m_bounds.left = page.left_margin;
	m_bounds.right = page.right_margin;
	
	grid_spacing1 = 0.25;
	double title_offset = 0.5;

	bool temp = m_offset;
	m_offset = false;
	for (x_pos=page.left_margin;x_pos<=page.right_margin;x_pos+=grid_spacing1)
	{
		draw_line (x_pos,page.top_margin,x_pos,page.bottom_margin-title_offset);
	}
	for (y_pos=page.top_margin;y_pos<=page.bottom_margin-0.5;y_pos+=grid_spacing1)
	{
		draw_line (page.left_margin,y_pos,page.right_margin,y_pos);
	}
	m_offset = true;
}

void euclid::draw_line (float x1, float y1, float x2, float y2)
{
	fpoint p1, p2;
	p1.x = x1;
	p1.y = y1;
	p2.x = x2;
	p2.y = y2;
	draw_line (p1,p2);
}

void euclid::draw_line (const fpoint &p1, const fpoint &p2)
{
	double x1,y1,x2,y2;
	double height = m_bounds.Height();
	double x_scale = x_dpi*m_scale;
	double y_scale = y_dpi*m_scale;
	double z = 0.0;

#ifndef ONE_HERTZ
	m_mode = m_pdc->IsPrinting();
	camera &a = m_pView->m_camera;
#endif

	if (m_mode==false)
		x_scale = -x_scale;

	if (m_debug==true)
	{
		writeln (output,"draw_line: height = ",height,", x_scale = ",x_scale);
		write (output,"draw_line: p1 = {",p1.x,", ",p1.y,"}");
		writeln (output,", p2 = {",p2.x,", ",p2.y,"}");
	}
	
	fpoint p3,p4;
	if (m_offset==true)
	{
		p3 = p1+m_origin;
		p4 = p2+m_origin;
	}
	else
	{
		p3 = p1;
		p4 = p2;
	}
	bool m_flipy;

#ifdef ONE_HERTZ
	m_flipy = true;
#else
	m_flipy = false;
#endif

	x1 = (p3.x-page.x_offset)*x_scale;
	x2 = (p4.x-page.x_offset)*x_scale;
	if (m_flipy==false) {
		y1 = height-(p3.y-page.y_offset)*y_scale;
		y2 = height-(p4.y-page.y_offset)*y_scale;
	}
	else
	{
		y1 = (p3.y-page.y_offset)*y_scale;
		y2 = (p4.y-page.y_offset)*y_scale;
	}
	if (m_debug==true)
	{
		write (output,"draw_line: x1 = ",x1,", y1 = ",y1);
		writeln (output,", x2 = ",x2,", y2 = ",y2);
	}
	if (m_mode==true)
	{
		m_pdc->MoveTo (x1,y1);
		m_pdc->LineTo (x2,y2);
	}
	else
	{
//		a.move_to_ex (_vector(-x1,height-y1,z));
//		a.line_to_ex (_vector(-x2,height-y2,z));
#ifndef ONE_HERTZ
		a.move_to_ex (_vector(x1,height-y1,z));
		a.line_to_ex (_vector(x2,height-y2,z));
#else
		m_pdc->MoveTo (x1,y1);
		m_pdc->LineTo (x2,y2);
#endif
	}
}

////////////////////////////////////////////////////////////////////
// 
//	PROPOSITION ???
//
//	Given a point on the plane representing the center of a circle
//	and a point on the plane representing a point on the
//	circumference of a circle, plot the indicated circle.
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

void euclid::draw_polygon (const fpoint &p1, const fpoint &p2)
{
	SETCOLOR _(m_pdc,COLOR::green,NULL);
//	writeln (output,"euclid::polygon:");

	double x1,y1,x2,y2;
	double dx, dy, r, r2;
	double x_scale, y_scale;
	int m_sides = 120;
	CRect rect;

	x_scale = x_dpi*m_scale;
	y_scale = y_dpi*m_scale;
	double height;
	height = m_bounds.Height();
	
	// find radius of circle
	dx = (p1.x-p2.x);
	dy = (p1.y-p2.y);
	r = sqrt (dx*dx+dy*dy);

	x1 = (p1.x-page.x_offset)*x_scale;
	y1 = height-(p1.y-page.y_offset)*y_scale;
	r2 = r*x_scale;

	rect.bottom = y1+r2;
	rect.top = y1-r2;
	rect.right = x1+r2;
	rect.left = x1-r2;
	
//	todo - use a custom drawing method here
//	instead of Windoze - especially when we
//	get to the point where we are going to
//	be drawing the shadow of a cone, for example
//	on another plane - in 3d of course!

#if 0
	write (output,"euclid::draw_circle p1.x = ",p1.x,", p1.y = ",p1.y);
	write (output,", p2.x = ",p2.x,", p2.y = ",p2.y);
	writeln (output,", computed radius = ",r);
#endif

	fpoint p3;
	if (m_offset==true)
		p3 = p1+m_origin;
	else
		p3 = p1;
	
#ifndef ONE_HERTZ
	polygon_2d poly;
	poly.construct_regular_polygon (m_sides,r2);  
	orientable_object ref;
	ref.set_orientation (0,0,0);
	ref.set_position (_vector(-x_scale*p3.x,y_scale*p3.y,0.0));

	camera &at = m_pView->m_camera;
	at.push_object_transformation (ref);
	poly.on_render (at);
	at.pop_object_transformation ();
#else
//	m_pdc->Ellipse (rect);
#endif
}

void box::construct ()
{
// create the default test box
	x_axis = fline (fpoint(1.0,1.0),fpoint(10.0,1.0));
	y_axis = fline (fpoint(1.0,1.0),fpoint(1.0,8.0));
	x_limit = fline (fpoint(10.0,1.0),fpoint(10.0,8.0));
	y_limit = fline (fpoint(1.0,8.0),fpoint(10.0,8.0));
	
	m_pt[0] = fpoint::intersect (x_axis,y_axis);
	m_pt[1] = fpoint::intersect (x_axis,x_limit);
	m_pt[2] = fpoint::intersect (x_limit,y_limit);
	m_pt[3] = fpoint::intersect (y_axis,y_limit);
}

void box::on_draw () const
{
	m_host->draw_line (m_pt[0],m_pt[1]);
	m_host->draw_line (m_pt[1],m_pt[2]);
	m_host->draw_line (m_pt[2],m_pt[3]);
	m_host->draw_line (m_pt[0],m_pt[3]);
}

fpoint box::center ()
{
	fpoint result;
	result.x = (0.25)*(m_pt[0].x+m_pt[1].x+m_pt[2].x+m_pt[3].x);
	result.y = (0.25)*(m_pt[0].y+m_pt[1].y+m_pt[2].y+m_pt[3].y);
	return result;
}
  
decagon euclidian::construct_decagon (bool draw)
{
	double d;
	fpoint p1,p2,p3,p4;
	decagon deca;
	pentagon temp;
	euclidian::bind (*m_host,temp);
	temp = construct_pentagon (draw);
	deca.m_center = temp.m_center;
	int i;
	for (i=0;i<5;i++)
		deca.m_points [2*i] = temp.m_points [i];

	d = fpoint::length (deca.m_points[0],deca.m_center);
	for (i=0;i<5;i++)
	{
		p1 = deca.m_points [2*i];
		p2 = deca.m_points [2*(i+1)%10];
		p3 = fpoint::midpoint (p1,p2);
		p4 = fpoint::extend1 (segment (p3,deca.m_center),-d);
		deca.m_points [2*i+1] = p4;
	}
	return deca;
}

void decagon::on_draw () const
{
	int i;
	fpoint pt1, pt2;
	ASSERT (m_host!=NULL);
	SETCOLOR _(m_host->m_pdc,COLOR::white);
	for (i=0;i<10;i++) {
		pt1 = m_points [i];
		pt2 = m_points [(i+1)%10];
		m_host->draw_line (pt1,pt2);
	}
}

double fpoint::length (const fpoint &p1, const fpoint &p2)
{
	double result, dx, dy;
	dx = p1.x-p2.x;
	dy = p1.y-p2.y;
	result = sqrt (dx*dx+dy*dy);
	return result;
}

////////////////////////////////////////////////////////////////////
// 
//	PROP. IX.—Problem.
//
//	Using a compass and strait edge, if necessary.
//	Identify a point along a line segment AB which is
//	a particular distace 'd' fromm point A and which
//	exists along the line segment AB.
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

fpoint fpoint::extend0 (const segment &s, double d)
{
//	First find the length of the line segment and then
//	calculate the sine and cosine of the angle relative
//	to the standard complex plane
	fpoint result;
	const fpoint &A = s.p[0];
	const fpoint &B = s.p[1];
	double c1,s1,length,dx,dy,recip_l;
	dx = B.x-A.x;
	dy = B.y-A.y;
	length = sqrt((dx*dx)+(dy*dy));
	recip_l = 1.0/length;
	c1 = dx*recip_l;
	s1 = dy*recip_l;

//	now add the length d in the direction of A->B
//	to the value of A.

	result.x = A.x + d*c1;
	result.y = A.y + d*s1;
	return result;
}

fpoint fpoint::extend1 (const segment &s, double d)
{
//	First find the length of the line segment and then
//	calculate the sine and cosine of the angle relative
//	to the standard complex plane
	fpoint result;
	const fpoint &A = s.p[0];
	const fpoint &B = s.p[1];
	double c1,s1,length,dx,dy,recip_l;
	dx = B.x-A.x;
	dy = B.y-A.y;
	length = sqrt((dx*dx)+(dy*dy));
	recip_l = 1.0/length;
	c1 = dx*recip_l;
	s1 = dy*recip_l;

//	now add the length d in the direction of A->B
//	to the value of B.

	result.x = B.x + d*c1;
	result.y = B.y + d*s1;
	return result;
}

//////////////////////////////////////////////////////////////////
//
// Assuming that the angle object A already contains the
// comptued values of the directioal cosines for a line,
// line_segment, ray or else for a simple angle, project
// a point p2 in the direction indicated by A, through
// a distance d, and return the computed value
//
////////////////////////////////////////////////////////////////

fpoint fpoint::project (const fpoint &p1, const angle &A, double d)
{
	fpoint result;
	double dx = A.m_c*d;
	double dy = A.m_s*d;
	result.x = p1.x+dx;
	result.y = p1.y+dy;
	return result;
}

///////////////////////////////////////////////////////////////////
//
//	To reflect a point about a given line, find the equation
//	of the line that passes through point P and whcih is
//	perpendicular to the given line L.  Then find the point
//	where the given line and it's perpendicular interseect.
//	Finally extend this line segment by an amount equal to
//	the distance from the point of intersection, along the
//	normal.
//
//	Optimization??
//
///////////////////////////////////////////////////////////////////

#if 0
fpoint fpoint::reflect (const fline &L, const fpoint &P)
{
	fpoint result;
	fline normal = fline::perpendicular (L,P);
	fpoint I = fpoint::intersect (L,normal);
	double d = fpoint::length (I,P);
	result = fpoint::extend1 (segment(P,I),d);
	return result;
}
#endif

//////////////////////////////////////////////////////////////////
//
// Reflecting a point about another point is dead simple
// in one dimension.  Given two points A and B, the distance
// from A to B is just (A-B).  So if we want to reflect
// A about the point B, we would subtract (A-B) from B
// obtaining B-(A-B) = 2*B-A.  So why not try this in
// two dimeension?
//
/////////////////////////////////////////////////////////////////

fpoint fpoint::reflect (const fline &L, const fpoint &P)
{
	fpoint result;
	fline normal = fline::perpendicular (L,P);
	fpoint I = fpoint::intersect (L,normal);
	result.y = 2*I.y-P.y;
	result.x = 2*I.x-P.x;
	return result;
}

// given two points p1 and p2 in two dimensional
// space, find the euqation of a line in the form
// a*x + b*y + c = 0

fline::fline (const fpoint &p1, const fpoint &p2)
{
	double dx,dy;
	if (p1==p2) {
		m_bvalid=false;
		return;
	}
	dy = p2.y-p1.y;
	dx = p2.x-p1.x;
	a = -dy;
	b = dx;
	c = -(a*p1.x+b*p1.y);
}

fline::fline (const fpoint &p1, double theta)
{
	a = cos(theta);
	b = -sin(theta);
	c = -(a*p1.x+b*p1.y);
	m_bvalid = true;
}

fline &fline::operator = (const fline &L)
{
	a = L.a;
	b = L.b;
	c = L.c;
	return *this;
}

////////////////////////////////////////////////////////////////////
//
//	PROP. XXXI.—Problem.
//	Through a given point (C) to draw a right line parallel
//	to a given right line.
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

fline fline::parallel (const fline &L, const fpoint &pt)
{
//
//	A parallel line will have the same slope as the line that
//	we want to make a copy from, so we can simply copy the
//	values of a and b over to the result, and then solve for
//	c in the equation a*x+b*y+c=0.
//
//	The choices made in the construction however have
//	epistimololical implications with respect to the
//	consideration of the parallel postulate, like when we
//	want to construct longitude lines, lattitude lines
//	as well as the arcs of great circles that is on the
//	surface of a given sphere or other surface where
//	the concept of local curvature has meaning.
//	
//	12-10-2023 GLG
//
//////////////////////////////////////////////////////////////////
	
	fline result;
	result.a = L.a;
	result.b = L.b;
	result.c = -(L.a*pt.x+L.b*pt.y);
	return result;
}

////////////////////////////////////////////////////////////////////
//
//	"PROP. XI.—Problem. From a given point (C) in
//	a given right line (AB) to draw a right line 
//	perpendicular to the given line. (Book 1: page 16)"
//
//  "PROP. XII.—Problem. To draw a perpendicular to a given 
//	indefinite right line (AB) from a given point (C)
//	without it.  (Book 1: page 17)"
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

fline fline::perpendicular (const fline &L, const fpoint &pt)
{
// a perpendicular line can be found if we simply
// rotate the original line by 90 degrees, and
// then solve for c based on our given point
// like we did for parallels.

	fline result;
	result.a = -L.b;
	result.b = L.a;
	result.c = -(result.a*pt.x+result.b*pt.y);
	return result;
}

fpoint fpoint::intersect (const fline &L1, const fline &L2)
{
////////////////////////////////////////////////////////////////////
//
//	Determine if l1 and L2 intersect, and if they do
//	caclulate the x,y values of the point of intersection
//	and return true or else leave x and y unchanged and 
//	return false.
//
///////////////////////////////////////////////////////////////////
	
	fpoint result;
	double det,recip_det;
	det = L1.a*L2.b-L1.b*L2.a;
	if (det!=0)
	{
		recip_det = 1.0/det;
		result.x = (L1.b*L2.c-L1.c*L2.b)*recip_det;
		result.y = (L1.c*L2.a-L1.a*L2.c)*recip_det;
		result.m_bvalid = true;
	}
	else
	{
		result.m_bvalid = false;
	}
	return result;
}

void fpoint::plot_2d (const euclid &realm)
{
	double x_pos, y_pos;
	CDC *dc = realm.m_pdc;
	SETCOLOR _(dc,COLOR::green);
	dc->SetTextColor (COLOR::green);
	CRect text_box;
	// todo - compute point relative to current
	// drawing area.
	x_pos = x;
	y_pos = y;

	text_box.top = y_pos;
	text_box.bottom = y_pos;
	text_box.left = x_pos;
	text_box.right = x_pos;		
	
//	Need to cast to CString IF the project settings
//	allow for the use of Unicode, or else this will
//	result in an error

	dc->Ellipse (x_pos-2,y_pos-2,x_pos+2,y_pos+2);
	dc->DrawText (CString(m_label),&text_box,DT_CALCRECT);
	dc->DrawText (CString(m_label),&text_box,DT_LEFT);
}

fpoint fpoint::midpoint (const fpoint &p1, const fpoint &p2)
{
	fpoint result;
	result.x = 0.5*(p1.x+p2.x);
	result.y = 0.5*(p1.y+p2.y);
	return result;
}

double *fpoint::law_of_cosines (const fpoint &p1, 
	const fpoint &p2, const fpoint &p3)
{
	double a,b,c,ac,bc,ab,a2,b2,c2;
	double dx1,dy1,dx2,dy2,dx3,dy3;
	static double result[3];
	dx1 = (p2.x-p3.x);
	dy1 = (p2.y-p3.y);
	a2 = (dx1*dx1)+(dy1*dy1);
	a = sqrt(a2);
	dx2 = p1.x-p3.x;
	dy2 = p1.y-p3.y;
	b2 = (dx2*dx2)+(dy2*dy2);
	b = sqrt(b2);
	dx3 = p1.x-p2.x;	
	dy3 = p1.y-p2.y;
	c2 = (dx3*dx3)+(dy3*dy3);
	c = sqrt(c2);

	ab = a*b;
	ac = a*c;
	bc = b*c;

	bool debug_trace=false;
	if (debug_trace==true)
	{
		write (
		output,"Sides: a = ",a,
		", b = ",b,
		", c = ",c
		);
	}
	result[0] = (b2+c2-a2)/(2*bc);
	result[1] = (a2+c2-b2)/(2*ac);
	result[2] = (a2+b2-c2)/(2*ab);

	if (debug_trace==true)
	{
		writeln(
		output," Angles: A = ",TO_DEGREES(arccos(result[0])),
		", B = ",TO_DEGREES(arccos(result[1])),
		", C = ",TO_DEGREES(arccos(result[2]))
		);
	}
	return result;
}

void analog_clock::construct ()
{
	vector<fpoint> hrot;
	dodecagon hour_marks;
	pentagon penta;

	hour_marks = construct_dodecagon (false);
	penta = construct_pentagon (false);
	euclidian::bind(*m_host,hour_marks);
	hour_marks.on_draw (false,true);
	int i,j;
	angle a;
	hrot.resize (12);
	for (i=0;i<5;i++) {
		a = angle(fline(penta.m_points[i],penta.m_center));
		for (j=0;j<12;j++)
			hrot[j] = hour_marks.m_points [j];
		a.angle::rotate_points (hrot,hour_marks.m_center);
		for (j=0;j<12;j++)
			m_minutes [12*i+j]=hrot[j];
	}
	m_center = hour_marks.m_center;
}

void analog_clock::on_draw () const
{
	int i;
	angle b;
	SETCOLOR _(m_host->m_pdc,COLOR::green);
#if 0
	for (i=0;i<60;i++)
	{
		m_host->draw_line (m_center,m_minutes [i]);
		b = angle (fline(m_minutes[i],m_center));
		write(output,"Minute: ",i," Angle: ",TO_DEGREES(b.theta), ", ");
		if (i%5==4)
			writeln(output);
	}
#endif
	fpoint pt1,pt2;
	for (i=0;i<60;i++)
	{
		pt1 = m_minutes[i];
		// line segement
		pt2 = fpoint::midpoint (pt1,m_center);
		// half of the previous line segment!
		pt2 = fpoint::midpoint (pt1,pt2);
		// one quarter of the first line segment!
		pt2 = fpoint::midpoint (pt1,pt2);
		// one eigth of the first line segment!
		pt2 = fpoint::midpoint (pt1,pt2);
		m_host->draw_line (pt1,pt2);
	}
	m_host->draw_polygon(m_center,m_minutes[0]);
	m_host->draw_polygon(m_center,pt2);
}

#define HOUR_LENGTH (0.55)
#define MINUTE_LENGTH (0.75)
#define SECONDS_LENGTH (0.85)

void analog_clock::plot_time () const
{
	fpoint pt0 = m_minutes[0];
	fpoint pt1,pt2,pt3;

	double l1 = fpoint::length (pt0,m_center);
	double h1 = -90.0+m_hour*30.0+m_min*0.50;
	double m1 = -90.0+m_min*6.0+m_sec*0.10;
	double s1 = -90.0+m_sec*6.0;
	
	angle a1 = angle(TO_RADIANS(h1));
	angle a2 = angle(TO_RADIANS(m1));
	angle a3 = angle(TO_RADIANS(s1));
	
	SETCOLOR _(m_host->m_pdc,COLOR::yellow);
	pt1 = fpoint::project (m_center,a1,l1*HOUR_LENGTH);
	pt2 = fpoint::project (m_center,a2,l1*MINUTE_LENGTH);
	pt3 = fpoint::project (m_center,a3,l1*SECONDS_LENGTH);
	
	m_host->draw_line (m_center,pt1);
	m_host->draw_line (m_center,pt2);
	m_host->draw_line (m_center,pt3);
}

void analog_clock::set_time (int h, int m, int s)
{
	m_hour = h;
	m_min = m;
	m_sec = s;
	m_tick = 0;
}

void euclidian::draw_clock ()
{
	COleDateTime _dt = COleDateTime::GetCurrentTime();

	int h,m,s;
	h = _dt.GetHour();
	m = _dt.GetMinute();
	s = _dt.GetSecond();
	analog_clock clock;
	euclidian::bind(*m_host,clock);
//	DWORD t1 = GetTickCount();
//	for (int i=0;i<100;i++)
		clock.construct ();

//	DWORD t2 = GetTickCount();
//	int dt = t2-t1;
//	writeln (output,"clock.construct took ",dt," msec.");
	clock.on_draw ();
	clock.set_time (h,m,s); 
	clock.plot_time ();
}

void dodecagon::on_draw (bool b1, bool b2) const
{
	fpoint p1,p2,p3,p4;
	int i;
	SETCOLOR _(m_host->m_pdc,COLOR::green);

	if (b1==true)
	{
		m_host->draw_line(m_points[11],m_points[0]);
		for (i=0;i<12;i++)
			m_host->draw_line(m_points[i],m_points[i+1]);
	}
	if (b2==true)
	for (i=0;i<12;i++) {
		p1 = m_points[i];
		p2 = fpoint::midpoint(p1,m_center);
		p3 = fpoint::midpoint(p1,p2);
		p4 = fpoint::midpoint(p1,p3);
		m_host->draw_line(p1,p4);
	}
	else
	for (i=0;i<12;i++) {
		p1 = m_points[i];
		m_host->draw_line(p1,m_center);
	}
}

fpoint euclidian::bisect (const fpoint &A, const fpoint &B, const fpoint &C)
{
	triangle T;
	euclidian::bind (*m_host,T);
	T.reference [0] = A;
	T.reference [1] = B;
	T.reference [2] = C;

	T.current[0] = T.reference[0];
	T.current[1] = T.reference[1];
	T.current[2] = T.reference[2];
	T.calculate_lines ();
	
	fpoint result;
	T.generate_bisectors (false);
	result  = T.J;
	return result;
}

dodecagon euclidian::construct_dodecagon (bool draw)
{
	fpoint pt0,pt1,pt2,pt3,pt4;
	dodecagon decca;
	hexagon hex = construct_hexagon (false);
	int i;
	double len1;
//	first copy the even points over
	decca.m_center = hex.m_center;
	for (i=0;i<6;i++)
		decca.m_points [2*i] = hex.m_points [i];
	
	pt0 = decca.m_center;
	for (i=0;i<6;i++)
	{
		pt1 = decca.m_points [2*i];
		pt2 = decca.m_points [(2*(i+1))%12];
		pt3 = bisect (pt0,pt1,pt2);
		len1 = fpoint::length (pt0,pt1);
		pt4 = fpoint::extend0 (segment (pt0,pt3),len1);
		decca.m_points [2*i+1] = pt4;
	}
	return decca;
}

void hexagon::on_draw () const
{
	int i;
	SETCOLOR _(m_host->m_pdc,COLOR::yellow);
	for (i=0;i<5;) {
		m_host->draw_line(m_points[i],m_points[i++]);
	}
	m_host->draw_line(m_points[5],m_points[0]);
}

hexagon euclidian::construct_hexagon (bool draw)
{
	bool draw_rect = draw;
	
// First construct a rectangle with a diagonal
// that is also the hypotenuse of a right triangle

	double diagonal,width, height;

//	For demonstration purposes.
//	these need to be passed in as parameters
//	or else we could simply consruct a reference
//	hexagon inside a standard unit circle at
//	the origin, and then shift, rotate and scale
//	as desired.

	fpoint origin (35.0,10.0);
	diagonal = 12.0;
	height = 0.5*diagonal;
	width = sqrt(diagonal*diagonal-height*height);
	rectangle test_rect;
	test_rect.construct(width,height);
	euclidian::bind(*m_host,test_rect);
	test_rect.move_to (origin);

	if (draw==true)
		test_rect.on_draw ();

//	The four points of the rectangle.
	fpoint A,B,C,D;
	A = test_rect.m_pt[0];
	B = test_rect.m_pt[1];
	C = test_rect.m_pt[2];
	D = test_rect.m_pt[3];

//	compute and draw local x_axis.
	fline LL1 = fline::fline(C,D);
	fline LL2 = fline::fline(A,D);

	if (draw==true)
	{
		SETCOLOR _(m_host->m_pdc,COLOR::green);
		m_host->draw_line (C,D);
		m_host->draw_line (A,D);
	}

//	A good optimizing compiler will probably
//	elimiate these variables, but they are
//	here anyway for clarity.

	fpoint H1,H2,H3,H4,H5,H6;
	H1 = B;
	H2 = fpoint::reflect (LL1,H1);
	H6 = fpoint::extend0 (segment(D,A),diagonal);
	H4 = fpoint::reflect (LL2,H2);
	H5 = fpoint::reflect (LL2,H1);
	H3 = fpoint::reflect (LL1,H6);

	hexagon hex;
	euclidian::bind(*m_host,hex);
	hex.m_center = D;
	hex.m_points [0] = H1;
	hex.m_points [1] = H2;
	hex.m_points [2] = H3;
	hex.m_points [3] = H4;
	hex.m_points [4] = H5;
	hex.m_points [5] = H6;
	return hex;
}

pentagon::pentagon ()
{
	m_center = fpoint (0.0,0.0);
}

/////////////////////////////////////////////////////////////////
//
//	Problem:  Using methods compatable with compass and
//	straight edge, construct a regular pentagon and a
//	regular hegagon.  Store the points generated in a 
//	suitable analytic format, i.e., as a vector or fixed
//	array.
//
////////////////////////////////////////////////////////////////

pentagon euclidian::construct_pentagon (bool draw)
{
	bool draw_rect = draw;
	double width, height;
	width = 12.0;
	height = 0.5*width;
	fpoint origin (35.0,10.0);
	rectangle test_rect;
	test_rect.construct(width,height);
	euclidian::bind(*m_host,test_rect);
	test_rect.move_to (origin);
	fpoint A = test_rect.m_pt[1];
	fpoint B = test_rect.m_pt[2];
	fpoint C = test_rect.m_pt[3];
	fpoint D = test_rect.m_pt[0];
	segment AC = segment::segment (C,A);
	fline diagonal (A,C);
	fline base_line (B,C);	
	double d1 = fpoint::length (A,C);
	fpoint E = fpoint::extend1 (AC,height);
	
	fpoint F = fpoint::midpoint (C,E);
	double c36 = fpoint::length (F,C)/width;
	double s36 = sqrt (1-c36*c36);

	if (draw==true) {
		m_host->draw_polygon (A,B);
		m_host->draw_line (A,E);
	}
	if (draw_rect==true) {
		test_rect.on_draw();
		m_host->draw_line (A,C);
		writeln (output,"diagonal length of rectangle: ",d1);
		writeln (output,"measured cosine value: ",c36);
		writeln (output,"measured sine value:   ",s36);
	}
//	draw a fake circle for now, then calculate and draw
//	the 36 degree vector
	m_host->draw_polygon (fpoint(origin.x,origin.y+height),fpoint(origin.x+width,origin.y+height));
	fpoint H = fpoint::extend1 (segment(B,C),-c36*width);
	fline HJ = fline::perpendicular (base_line,H);
	fpoint J = fpoint::intersect (HJ,diagonal);
	fpoint K = fpoint::extend1 (segment(J,H),-s36*width);

//	H is now the point on the baseline ~cos(36)*width
//	J is a hack to get a line segment perpendicular to the base_line at H.
//	so that K will be the 36 degree point on the main circle
	if (draw==true)
	{
		m_host->draw_line (C,H);
		m_host->draw_line (H,K);
		m_host->draw_line (C,K);
	}
//  If we bisect the 36 degree angle, we should get an 18
//  degree angle, which we can then use to find a perpendicular
//  which will add up to 108 degrees!
	
	triangle T;
	euclidian::bind (*m_host,T);
	T.reference [0] = H;
	T.reference [1] = C;
	T.reference [2] = K;

	T.current[0] = T.reference[0];
	T.current[1] = T.reference[1];
	T.current[2] = T.reference[2];

	T.calculate_lines ();
	fpoint bisectors[3];
	T.generate_bisectors (false);
	bisectors [0] = T.J;
	bisectors [1] = T.K;
	bisectors [2] = T.L;

	fpoint L1,L2,L3,L4,P18;	
	P18 = bisectors [1];
//	the 180 degree mark is easy, so is the reflecton
//	of K about the baseline, thus yeilding L2, and L4
	L2 = fpoint::reflect (fline(C,D),B);
	L4 = fpoint::reflect (base_line,K);

//	L3 is going to be a reflection of L1 about the
//	base_line, so how to calculate the 108 degree line?
	fline LL1 = fline::perpendicular (fline(C,P18),C);
	fline LL2 = fline (K,L2);
	fpoint M = fpoint::intersect (LL1,LL2);
	L1 = fpoint::extend0 (segment(C,M),width);
	L3 = fpoint::reflect (base_line,L1);
	
	if (draw==true)
	{
		m_host->draw_line (C,L4);
		m_host->draw_line (H,L4);
	}
//	finally we can construct a pentagon with our
//	set of points.
	pentagon penta;
	euclidian::bind (*m_host,penta);
	penta.m_center = C;
	penta.m_points [0] = K;
	penta.m_points [1] = L1;
	penta.m_points [2] = L2;
	penta.m_points [3] = L3;
	penta.m_points [4] = L4;
//	O.K. lets draw it insinde the circle that we drew earlier.

	if (draw==true)
	{
		SETCOLOR _(m_host->m_pdc,COLOR::magenta);
		m_host->draw_line (C,penta.m_points[1]);
		m_host->draw_line (C,penta.m_points[2]);
		m_host->draw_line (C,penta.m_points[3]);
		m_host->draw_line (C,penta.m_points[4]);
	}
	if (draw==true)
	{
		SETCOLOR _(m_host->m_pdc,COLOR::cyan);
		m_host->draw_line (penta.m_points[0],penta.m_points[1]);
		m_host->draw_line (penta.m_points[1],penta.m_points[2]);
		m_host->draw_line (penta.m_points[2],penta.m_points[3]);
		m_host->draw_line (penta.m_points[3],penta.m_points[4]);
		m_host->draw_line (penta.m_points[4],penta.m_points[0]);
	}
	return penta;
}

rectangle::rectangle ()
{
	m_host = NULL;
}

rectangle &rectangle::construct (float width, float height)
{
	m_pt[0] = fpoint(0.0,0.0);
	m_pt[1] = fpoint(width,0);
	m_pt[2] = fpoint(width,height);
	m_pt[3] = fpoint(0.0,height);
	return (*this);
}

void rectangle::move_to (const fpoint &dest)
{
	fpoint delta = dest-m_pt[0];
	m_pt[0] = dest;
	m_pt[1] = m_pt[1]+delta;
	m_pt[2] = m_pt[2]+delta;
	m_pt[3] = m_pt[3]+delta;
}

void rectangle::on_draw () const
{
	SETCOLOR _(m_host->m_pdc,COLOR::red);
	m_host->draw_line (m_pt[0],m_pt[1]);
	m_host->draw_line (m_pt[1],m_pt[2]);
	m_host->draw_line (m_pt[2],m_pt[3]);
	m_host->draw_line (m_pt[3],m_pt[0]);
}

int triangle::calculate_lines ()
{
// now calculate some lines
// in the form a*x+b*y+c=0
	AB = fline (reference[0],reference[1]);
	AC = fline (reference[0],reference[2]);	
	BC = fline (reference[1],reference[2]);
	return 0;
}

int triangle::recompute_lines ()
{
// now calculate some lines
// in the form a*x+b*y+c=0
	AB = fline (current[0],current[1]);
	AC = fline (current[0],current[2]);	
	BC = fline (current[1],current[2]);
	return 0;
}

int triangle::recompute_points ()
{
	fpoint A1,B1,C1;

	A1 = fpoint::intersect (AB,AC);
	B1 = fpoint::intersect (AB,BC);
	C1 = fpoint::intersect (AC,BC);
	
	current[0] = A1;
	current[1] = B1;
	current[2] = C1;

	return 0;
}

int triangle::generate_midpoints ()
{
	bcolor = COLOR::cyan;
	fpoint A1,B1,C1;

	A1 = current[0];
	B1 = current[1];
	C1 = current[2];

	D = fpoint::midpoint (B1,C1);
	E = fpoint::midpoint (A1,C1);
	F = fpoint::midpoint (A1,B1);
	return 0;
}

int triangle::compute_centroid ()
{
	fline AD,BE;
	fpoint A1,B1;

	A1 = current[0];
	B1 = current[1];

	AD = fline(A1,D);
	BE = fline(B1,E);
	G1 = fpoint::intersect(AD,BE);
	return 0;
}

int triangle::rotate_points (double theta)
{
	vector<fpoint> pts;
	pts.resize(3);

	pts[0] = reference[0];
	pts[1] = reference[1];
	pts[2] = reference[2];

	angle a = angle(theta);
	a.angle::rotate_points (pts,G1);
	current[0] = pts[0];
	current[1] = pts[1];
	current[2] = pts[2];
	return 0;
}

int triangle::draw_triangle0 (double scale)
{
	bcolor = COLOR::red;
	SETCOLOR _(m_host->m_pdc,bcolor);
//	writeln (output,"euclid::draw_triangle0:");
	fpoint A1,B1,C1;

	A1 = current[0];
	B1 = current[1];
	C1 = current[2];

	m_host->draw_line (A1,B1);
	m_host->draw_line (B1,C1);
	m_host->draw_line (C1,A1);
	return 0;
}

int triangle::draw_triangle1 (double scale)
{
	bcolor = COLOR::green;
	SETCOLOR _(m_host->m_pdc,bcolor);
//	writeln (output,"euclid::draw_triangle1:");
	fpoint A1,B1,C1;

	A1 = current[0];
	B1 = current[1];
	C1 = current[2];

	m_host->draw_line (A1,B1);
	m_host->draw_line (B1,C1);
	m_host->draw_line (C1,A1);
	return 0;
}

int triangle::draw_midpoints (double scale)
{
	bcolor = COLOR::cyan;
	SETCOLOR _(m_host->m_pdc,bcolor);
//	writeln (output,"euclid::draw_midpoints:");
	fpoint A1,B1,C1;

	A1 = current[0];
	B1 = current[1];
	C1 = current[2];

	m_host->draw_line (A1,D);
	m_host->draw_line (B1,E);
	m_host->draw_line (C1,F);

	// redraw the segment G1,F to
	// demonstrate that G1 is the centroid
#if 0
	{
		SETCOLOR _(m_pdc,COLOR::blue);
		draw_line (G1,F);
	}
#endif
	// now draw some lines that
	// connect the midpoints to each
	// other - forming 4 new triangles!
	{
		SETCOLOR _(m_host->m_pdc,COLOR::red);
		m_host->draw_line (D,E);
		m_host->draw_line (E,F);
		m_host->draw_line (D,F);
	}
	return 0;
}

int triangle::draw_bisectors (double scale)
{
//	draw_circle (H,J);
//	draw_line (H,J);
	bcolor = COLOR::yellow;
	SETCOLOR _(m_host->m_pdc,bcolor);
	fpoint A1,B1,C1;

	A1 = current[0];
	B1 = current[1];
	C1 = current[2];

	m_host->draw_line (A1,J);
	m_host->draw_line (B1,K);
	m_host->draw_line (C1,L);
	return 0;
}

bool triangle::draw_incircle()
{
	fpoint (*exec0)(const fline&, const fline &);
	fline (*exec1)(const fline&, const fpoint &);
	exec0 = &fpoint::intersect;
	exec1 = &fline::perpendicular;
	
	fpoint A1,B1,C1;
	A1 = current[0];
	B1 = current[1];
	C1 = current[2];
	A1.set_label ("A1");
	B1.set_label ("B1");
	C1.set_label ("C1");

	bool debug_trace=false;
	if (debug_trace==true)
	{
//		write (output,"ROTATION = ",theta,": ");
		write (output,", A1 = (",A1.x,", ",A1.y,"), ");
		write (output,", B1 = (",B1.x,", ",B1.y,"), ");
		writeln (output," C1 = (",C1.x,", ",C1.y,")");
	}
	fpoint::law_of_cosines (A1,B1,C1);	
	fpoint c_center, c_radius;
	fline bisector1 = fline(A1,J);
	fline bisector2 = fline(B1,K);

	c_center = (*exec0) (bisector1,bisector2);
	fline radial = (*exec1)(AB,c_center);
	c_radius = (*exec0) (AB,radial);

	m_host->draw_polygon (c_center,c_radius);
	return true;
}

////////////////////////////////////////////////////////////////////
// 
//	PROP. IX.—Problem.
//	To bisect a given rectilineal angle (BAC).
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

int triangle::generate_bisectors (bool draw)
{
	fpoint U1,U2,U3;
	double d; 
	double dx1,dx2,dx3,dy1,dy2,dy3;
	double L1,L2,L3;
	double c1,c2,c3,s1,s2,s3;
	// first find the sines and cosines
	// of the line segments that make up
	// the triangle
	dx1 = current[1].x-current[0].x;
	dx2 = current[1].x-current[2].x;
	dx3 = current[2].x-current[0].x;
	dy1 = current[1].y-current[0].y;
	dy2 = current[1].y-current[2].y;
	dy3 = current[2].y-current[0].y;

	L1 = sqrt((dx1*dx1)+(dy1*dy1));
	L2 = sqrt((dx2*dx2)+(dy2*dy2));
	L3 = sqrt((dx3*dx3)+(dy3*dy3));

	c1 = dx1*(1.0/L1);
	c2 = dx2*(1.0/L2);
	c3 = dx3*(1.0/L3);
	s1 = dy1*(1.0/L1);
	s2 = dy2*(1.0/L2);
	s3 = dy3*(1.0/L3);

	// now generate three points that
	// represent the directional unit
	// vectors for each line segment
	d = 1.0; // compass distance!
	U1.x = d*c1;
	U1.y = d*s1;
	U2.x = d*c2;
	U2.y = d*s2;
	U3.x = d*c3;
	U3.y = d*s3;
	// now generate the compass
	// points
	fpoint C1,C2,C3,C4,C5,C6;
	C1 = current[0]+U1;
	C4 = current[0]+U3;
	C2 = current[1]-U2;
	C5 = current[1]-U1;
	C3 = current[2]-U3;
	C6 = current[2]+U2;

	bcolor = COLOR::magenta;
	if (draw==true)
	{
		m_host->draw_line (current[0],C1);
		m_host->draw_line (current[1],C2);
		m_host->draw_line (current[2],C3);
		m_host->draw_line (current[0],C4);
		m_host->draw_line (current[1],C5);
		m_host->draw_line (current[2],C6);
		m_host->draw_line (C1,C4);
		m_host->draw_line (C2,C5);
		m_host->draw_line (C3,C6);
	}
	fpoint P0,P1,P2;
	// the directional unit vectors
	// can be used to find the midpoint
	// of a line that besects an
	// isosceles triangle that forms
	// from the base of the "arrow"
	P0 = fpoint::midpoint (C1,C4);
	P1 = fpoint::midpoint (C2,C5);
	P2 = fpoint::midpoint (C3,C6);

	fline AP1,BP2,CP3;
	AP1 = fline (current[0],P0);
	BP2 = fline (current[1],P1);
	CP3 = fline (current[2],P2);

	J = fpoint::intersect (AP1,BC);
	K = fpoint::intersect (BP2,AC);
	L = fpoint::intersect (CP3,AB);

	return 0;
}

fcircle::fcircle ()
{
	m_center = fpoint(0.0,0.0);
	m_radius = -1;
}

bool fcircle::from_triangle(triangle &t)
{
	bool result;
	result = true;
	fline L1,L2;
//	generating the midpoints is what
//	makes sure that we have current
//	versions of the points D,E,F.
	t.generate_midpoints();
	L1 = fline::perpendicular (t.AB,t.F);
	L2 = fline::perpendicular (t.AC,t.E);
	m_center = fpoint::intersect (L1,L2);
	fpoint r1 = m_center-t.current[0];
	m_radius = sqrt(r1.x*r1.x+r1.y*r1.y);

#if 0
	write (output,"fcircle::from_triangle: x = ",m_center.x);
	writeln (output,", y = ",m_center.y,", radius = ",m_radius);
#endif
	return result;
}

////////////////////////////////////////////////////////////////////
// 
//	PROPOSITION ???
//
//	given a point on the circumfernce of a circle and two random
//	lines that are tangent to the circle find the center of the
//	circle and the radius.
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

class tangent_circle
{
public:
	fpoint	m_center;
	float	m_radius;
	void construct (fline L1, fline L2, fpoint CB)
	{
		fline L3, L4;
		float phi, eta, tphi, c1, s1;
		float a, b, c, R1, R2;
		fpoint vertex;
		vertex.fpoint::intersect (L1,L2);
		L3 = fline::bisect1 (L1,L2,false);
		L4 = fline::bisect1 (L1,L2,true);

		tphi = tan(phi);
		c1 = cos (eta);
		s1 = sin (eta);
		c = sqrt(CB.x*CB.x + CB.y*CB.y);
		a = c/(tphi*tphi);
		b = c1+tphi*s1;	
		R1 = (-b-sqrt(b*b-4*a*c))/(2*a);
		R2 = (-b+sqrt(b*b-4*a*c))/(2*a);
	}
};
   
////////////////////////////////////////////////////////////////////
// 
//	PROP. IX.—Problem.
//	To bisect a given rectilineal angle (BAC).
//
//	THE ELEMENTS OF EUCLID - Translated by John Casey
//	Pubished 1887 by Cambridge Press, public domain.
//
////////////////////////////////////////////////////////////////////

#if 0
/*
Fix this buggy C++ code so that it correctly determines
the angle bisectors of any arbitrary triangle, including
triangles containing obtuse angles, or rotated and/or
shifted triangles.  Please minimize the use of conditional
branches if possible in order to take advantage of pipelined
processesing, on the one hand, while also minimizing the
use of trancendental or other computationaly expensive
functions.
*/
// incorrect code - does no genearte the correct
// angle bisectors?

fline fline::bisect1 (const fline &l1,
	const fline &l2, bool flip)
{
	fline l3,result;
	double c1,c2,c3,c4,c5,c6;
	double s1,s2,s3,s4,s5,s6;
	double d1,d2;

	fpoint vertex = fpoint::intersect (l1,l2);
	bool interects = vertex.m_bvalid;

	angle S = angle::add(l1,l2);
	c4 = sqrt (0.5*(1+S.m_c));
	s4 = sqrt (0.5*(1-S.m_c));

	// flip the slope 90 degrees if
	// we end up in the wrong quadrant
	if (flip==false)
	{
		result.a = s4;
		result.b = -c4;
	}
	else
	{
		result.a = c4;
		result.b = s4;
	}
	result.c = -(result.a*vertex.x+result.b*vertex.y);
	result.m_bvalid = true;
	return result;
}

fline fline::bisect1 (const fline &l1,
	 const fline &l2, const fline &l3)
{
	fline l4,result;
//	for each pair of line equations, find the
//	point of interection for rhat pair of lines.
	fpoint vertex1 = fpoint::intersect (l1,l3);
	fpoint vertex2 = fpoint::intersect (l1,l2);
	fpoint vertex3 = fpoint::intersect (l2,l3);
	
//	rhia gives us three line segments
	segment s1 = segment::segment (vertex1,vertex2);
	segment s2 = segment::segment (vertex2,vertex3);
	segment s3 = segment::segment (vertex3,vertex1);

	fpoint p1 = fpoint::distance (s1,1.0);
	fpoint p2 = fpoint::distance (s3,1.0);
	fpoint p3 = fpoint::midpoint (p1,p2);

	result = fline (vertex3,p3);
	return result;
}
#endif


#if 0
int triangle::generate_bisectors ()
{
	bcolor = COLOR::magenta;

#if 0
	L4 = fline::bisect1 (AB,AC,false);
	L5 = fline::bisect1 (AB,BC,true);
	L6 = fline::bisect1 (AC,BC,false); 
#endif
	L4 = fline::bisect1 (AB,AC,BC);
	L5 = fline::bisect1 (AB,BC,AC);
	L6 = fline::bisect1 (AC,BC,AB); 

	D = fpoint::intersect (L4,BC);
	E = fpoint::intersect (L5,AC);
	F = fpoint::intersect (L6,AB);

	H = fpoint::intersect (L5,L6);
	L7 = fline::perpendicular (BC,H);
	J = fpoint::intersect (BC,L7);
	return 0;
}
#endif
