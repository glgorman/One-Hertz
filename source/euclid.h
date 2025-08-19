
#include <vector>

#pragma once

class CCameraView;
class angle;
class euclid;
class fline;
class fpoint;
class segment;

class fpoint
{
protected:
	char *m_label;

public:
	bool m_bvalid;
	double x,y;
	fpoint () { m_label=NULL; }
	fpoint (double X, double Y)
	{
		m_label = NULL;
		m_bvalid = true;
		x=X; y=Y;
	}
	bool operator == (const fpoint &pt) const
	{
		bool result;
		if ((pt.x==x)&&(pt.y==y))
			result = true;
		else
			result = false;
		return result;
	}
	fpoint operator + (const fpoint &pt) const
	{
		fpoint result;
		result.x = this->x+pt.x;
		result.y = this->y+pt.y;
		return result;
	}
	fpoint operator - (const fpoint &pt) const
	{
		fpoint result;
		result.x = this->x-pt.x;
		result.y = this->y-pt.y;
		return result;
	}
	static fpoint extend0 (const segment &, double d);
	static fpoint extend1 (const segment &, double d);
	static fpoint intersect (const fline &L1, const fline &L2);
	static double length (const fpoint &p1, const fpoint &p2);
	static fpoint midpoint (const fpoint &p1, const fpoint &p2);
	static fpoint project (const fpoint &p1, const angle &A, double d);
	static fpoint reflect (const fline &, const fpoint &);
	static double *law_of_cosines (const fpoint &p1, const fpoint &p2, const fpoint &p3);	
	void set_label (char *str) { m_label = str; }
	void plot_2d (const euclid &realm);
};

class page_info 
{
friend class euclid;

protected:
	double page_width;
	double page_height;
	double left_margin; 
	double right_margin;
	double top_margin;
	double bottom_margin;
	double x_offset,y_offset;

public:
	void set_margins (double i);
};

class CSignalView;

class euclidian;

class euclid
{
friend class analog_clock;
friend class decagon;
friend class dodecagon;
friend class euclidian;
friend class euclid;
friend class fbox;
friend class fcircle;
friend class fpoint;
friend class ftriangle;
friend class hexagon;
friend class rectangle;
friend class triangle;
friend class triangle_test;

protected:
	vector<euclidian*> m_objects;
	page_info page;
	CRect m_bounds;
	double m_scale;
	float line_width;
	bool m_mode, m_debug;
	bool m_offset;
	int x_dpi, y_dpi;
	COLORREF m_c;
	CDC *m_pdc;
#ifdef ONE_HERTZ
	CSignalView *m_pView;
#else
	CCameraView *m_pView;
#endif

	void draw_line (float x1, float y1, float x2, float y2);
	fpoint m_origin;
	
public:
	euclid(void);
	virtual ~euclid(void);	
	void OnDraw ();
	void init ();
	void draw_border ();
	void draw_grid ();
	void draw_line (const fpoint &p1, const fpoint &p2);
	void draw_polygon (const fpoint &, const fpoint &);
	void set_origin (const fpoint &pt) { m_origin=pt; };
	static int RenderProc (CDC*, CSignalView *);
	static int RenderProc (CDC*, CView *);
};

class analog_clock;
class pentagon;
class hexagon;
class octagon;
class decagon;
class dodecagon;

class euclidian
{
friend class euclid;

protected:
	euclid *m_host;

protected:
	pentagon construct_pentagon (bool draw);
	hexagon construct_hexagon (bool draw);
	octagon construct_octagon (bool draw);
	decagon construct_decagon (bool draw);
	dodecagon construct_dodecagon (bool draw);

public:
	virtual int get_id () { return -1; }
	static bool bind (euclid &p1, euclidian &p2);
	double angle_test ();
	void draw_clock ();
	fpoint bisect (const fpoint &A, const fpoint &B, const fpoint &C);
};

class segment
{
public:
	fpoint p[2];
	segment (const fpoint &A, const fpoint &B)
	{
		p[0] = A;
		p[1] = B;
	}
};

class fline
{
friend class angle;
friend class euclid;
friend class fpoint;

protected:
	double a,b,c;

public:
	bool m_bvalid;

	fline () { m_bvalid = false; };
	fline &operator = (const fline &);
	fline (const fpoint &, const fpoint &);
	fline (const fpoint &, double theta);
	static fline bisect1 (const fline &, const fline &, bool flip);
	static fline bisect1 (const fline &, const fline &, const fline &);
	static fline parallel (const fline &, const fpoint &);
	static fline perpendicular (const fline &, const fpoint &);
};

class angle
{
public:
	bool m_bvalid;
	double m_c,m_s,theta;
	angle ()
	{
		m_bvalid = false;
	}
	angle (double arg)
	{
		m_c = cos(arg);
		m_s = sin(arg);
		theta = arg;
		m_bvalid = true;
	}
	angle (const fline &L)
	{
		double d;
		d = 1/sqrt(L.a*L.a+L.b*L.b);
		m_c = L.b*d;
		m_s = -L.a*d;
		theta = _arg (m_c,m_s);
		m_bvalid = true;
	}
	static angle add (const fline &L1, const fline &L2)
	{
		angle result;
		angle A1 = angle(L1);
		angle A2 = angle(L2);
		result.m_c = A1.m_c*A2.m_c-A1.m_s*A2.m_s;
		result.m_s = A1.m_s*A2.m_c+A1.m_c*A2.m_s;
		result.m_bvalid = true;
		return result;
	}
	static angle aubtract (const fline &L1, const fline &L2)
	{
		angle result;
		angle A1 = angle(L1);
		angle A2 = angle(L2);
		result.m_c = A1.m_c*A2.m_c+A1.m_s*A2.m_s;
		result.m_s = A1.m_s*A2.m_c-A1.m_c*A2.m_s;
		result.m_bvalid = true;
		return result;
	}
	// assume a positve angle!
	static angle half (const angle &A)
	{
		angle result;
		result.m_c = sqrt (0.5*(1+A.m_c));
		result.m_s = sqrt (0.5*(1-A.m_c));
		result.m_bvalid = true;
		return result;
	}
	void rotate_points (vector<fpoint> &points, const fpoint &G1);
};

class analog_clock: public euclidian
{
protected:
	fpoint m_center;
	fpoint m_minutes [60];
	int m_hour,m_min;
	int m_sec,m_tick;

public:
	void set_time (int,int,int);
	void construct ();
	void on_draw () const;
	void plot_time () const;
};

class box: public euclidian
{
friend class euclidian;

protected:
	fpoint m_pt[4];
	fline x_axis; 
	fline y_axis; 
	fline x_limit;
	fline y_limit;

public:
	box () { m_host=NULL; }
	fpoint center ();
	void construct ();
	void on_draw () const;
};

class decagon: public euclidian
{
friend class euclidian;

protected:
	fpoint m_center;
	fpoint m_points[10];

public:
	decagon () { m_host = NULL; };
	static decagon construct (bool draw);
	void on_draw () const;
};

class dodecagon: public euclidian
{
friend class euclidian;
friend class analog_clock;

protected:
	fpoint m_center;
	fpoint m_points[12];

public:
	dodecagon () { m_host = NULL; };
	static dodecagon construct (bool draw);
	void on_draw (bool,bool) const;
};

class ftriangle: public euclidian
{
public:
};

class fcircle: public euclidian
{
public:
	fpoint		m_center;
	double	m_radius;
	fcircle ();
	bool from_triangle(triangle &);
};

class fbox: public euclidian
{
public:
};

class hexagon: public euclidian
{
friend class euclidian;

protected:
	fpoint m_center;
	fpoint m_points[6];

public:
	hexagon () { m_host = NULL; };
	static hexagon construct (bool draw);
	void on_draw () const;
};

class region: public euclidian
{
public:
};

class pentagon: public euclidian
{
friend class analog_clock;
friend class euclidian;

protected:
	fpoint m_center;
	fpoint m_points[5];

public:
	pentagon ();
	static pentagon construct (bool draw);
};

class rectangle: public euclidian
{
friend class euclid;
friend class euclidian;

protected:
	fpoint m_pt[4];

public:
	rectangle ();
	rectangle *allocate ();
	rectangle &construct (float width, float height);
	void move_to (const fpoint &);
	void on_draw () const;
};

class solar_plot: public euclid
{
	solar_plot ();
	int create_data ();
	int main ();
};

class tessellation
{
public:
	vector<fpoint> m_cloud;
};

class triangle: public euclidian
{
friend class euclidian;
friend class fcircle;
friend class triangle_test;

protected:
	fpoint reference[3];
	fpoint current[3];
	fpoint D,E,F,G1,H,J,K,L;
	fline AB,AC,BC;

	bool draw_aux;
	COLORREF bcolor;

	int draw_triangle0 (double scale);
	int draw_triangle1 (double scale);
	int draw_midpoints (double scale);
	int draw_bisectors (double scale);
	bool draw_incircle();
	
	int rotate_points (double);
	int calculate_lines ();
	int recompute_points ();
	int recompute_lines ();
	int generate_midpoints ();
	int compute_centroid ();
	int generate_bisectors (bool draw);
}; 

class triangle_test
{
friend class euclid;

protected:
	triangle T;
	fcircle circle;

public:
	triangle_test ();
	int create_points ();
	int main ();
};

