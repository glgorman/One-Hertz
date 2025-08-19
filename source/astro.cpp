
#include "stdafx.h"
#include <ATLComTime.h>
#include <math.h>

#include "defines.h" 
#include "intrinsics.h"
#include "drawstuff.h"
#include "euclid.h"

#define INCLINATION_OF_EARTH_ORBIT	23.439281

using namespace std;
using namespace TENSORS;

#define SIDEREAL (23.0+56.0/60.0+4.091/3600.0)

class ellipse: public euclidian
{
friend class euclidean;
private:

protected:
	fpoint		m_foci[2];
	double		m_area, m_aspect;
	double		m_eccentricity;
	double		m_major, m_minor;
	double		m_offset;

public:
	ellipse ()
	{
		m_major = 0;
		m_minor = 0;
		m_area = 0;
		m_host = NULL;
	}
	void compute_area ();
	void compute_minor ();
	double get_area ();
	double aphelion ();
	double perhelion ();

	void set_eccentricty (MATH_TYPE);
	void set_foci (const fpoint &pt, int i);
	void set_major (MATH_TYPE a);
	void set_minor (MATH_TYPE b);
};

void ellipse::set_major (MATH_TYPE a)
{
	m_major = a;
}

void ellipse::set_eccentricty (MATH_TYPE e)
{
	m_eccentricity = e;
}

void ellipse::compute_area ()
{
	m_area = PI*m_major*m_minor;
	m_aspect = m_major/m_minor;
}

void ellipse::compute_minor ()
{
//	assume that e = sqrt(1-(b*b)/(a*a))  
//	solve for zz = b/a
	double zz = sqrt(1-m_eccentricity*m_eccentricity);
	m_minor = zz*m_major;
	m_offset = 0.5*sqrt (m_major*m_major-m_minor*m_minor);
	m_foci [0] = fpoint (0.0,0.0);
	m_foci [1] = fpoint (m_offset,0.0);
}

void ellipse::set_foci (const fpoint &pt, int i)
{
	m_foci[i] = pt;
}

double ellipse::aphelion ()
{
	double result;
	result = 0.5*m_major+m_offset;
	return result;
}

double ellipse::perhelion ()
{
	double result;
	result = 0.5*m_major-m_offset;
	return result;
}

class celestial_coordinates
{
public:
	MATH_TYPE right_ascension;
	MATH_TYPE declination;
};

solar_elements::solar_elements ()
{
	m_ecliptic = new ellipse;
	m_ecliptic->set_major (2*149.60e6);
	m_ecliptic->set_eccentricty (0.0167086);
	m_ecliptic->compute_minor ();
	m_ecliptic->compute_area ();

	double ap = 0.621371*m_ecliptic->aphelion();
	double pe = 0.621371*m_ecliptic->perhelion();

	set_longitude (119.0);
	set_latitude (39.0);
	set_time ();
		
	R = 3958.8*5280.0*FEET_TO_CM*1.0e-5;
	m_ROTX = tensor::rotate_x (TO_RADIANS(INCLINATION_OF_EARTH_ORBIT));
}

void solar_elements::set_time ()
{
	COleDateTime _dt = COleDateTime::GetCurrentTime();
	m_year = _dt.GetYear ();
	m_month = _dt.GetMonth ();
	m_day = _dt.GetDay ();
	m_hour = _dt.GetHour ();
	m_minute = _dt.GetMinute ();
	m_second = _dt.GetSecond ();
	m_time_zone = -8;
	m_daylight_savings = 0;
}

void solar_elements::set_time (char *str)
{
	COleDateTime _dt = COleDateTime::GetCurrentTime();
	m_year = _dt.GetYear ();
	m_month = _dt.GetMonth ();
	m_day = _dt.GetDay ();

	ASSERT (strlen(str)==6);
	char str_h [4], str_m [4], str_s [4];
	strncpy_s (str_h,4,&str[0],2);
	strncpy_s (str_m,4,&str[2],2);
	strncpy_s (str_s,4,&str[4],2);
	m_hour = atoi(str_h);
	m_minute = atoi(str_m);
	m_second = atoi(str_s);
	m_time_zone = -8;

	if (m_time_zone!=0)
		m_hour = (m_hour+24+m_time_zone)%24;
	m_daylight_savings = 0;
}

void solar_elements::set_longitude (double arg)
{
	m_longitude = arg;
}

void solar_elements::set_latitude (double arg)
{
	m_latitude = arg;
}

void solar_elements::debug_test ()
{
	ASSERT (julian_day (1,1,2000)==2451545);
	ASSERT (julian_day (6,19,1987)==2446966);
	ASSERT (julian_day (12,8,2009)==2455174);
	char test_str [256];

	int H,M,M1,S;

	m_month = 7;
	m_day = 6;
	int start_hour = 04;
	int start_minute = 30;
	int end_hour = 04;
	int end_minute = 40;
	for (H=start_hour,M=start_minute;H<=end_hour;H++,M=0)
	{
		M1 = (H==end_hour?end_minute:60);
		for (;M<M1;M++)
		for (S=0;S<60;S++)
		{	
			m_hour = H;
			m_minute = M;
			m_second = S;
			compute_local_aspect();
			sprintf_s (test_str,256,"LOCAL time = %02d:%02d:%02d  AZIMUTH = %f, EL = %f",H,M,S,m_az,m_el);
			writeln (output,test_str);
		}
	}
}

int solar_elements::julian_day  (int M, MATH_TYPE D, int Y)
{
	int mm = (M-14)/12;
	int yy = (Y+4800+mm);
	int JDN1 = (1461*yy)/4;
	int JDN2 = (367*(M-2-12*mm))/12;
	int JDN3 = (3*((yy+100)/100))/4;
	MATH_TYPE JDN = JDN1+JDN2-JDN3+D-32075;
	return JDN;
}

void solar_elements::compute_local_aspect()
{
	double solar_time, ra_base, ecliptic_aspect;
	
//	estimate ecliptic aspect

	m_gmt_seconds = 3600.0*(m_hour-m_daylight_savings-m_time_zone)+60.0*m_minute+m_second;
	solar_time = m_gmt_seconds*(1.0/3600.0);
	m_julian = julian_day (m_month,m_day,m_year) - julian_day (3,20,1900) + solar_time*(1.0/24.0);
	ecliptic_aspect = fmodf(m_julian,365.242191)*(TWO_PI/365.242191);	
	_vector ecliptic_aspect_vector = tensor::rotate_z (ecliptic_aspect)*_vector(1,0,0);
	ra_base = ecliptic_aspect*(24.0/TWO_PI);

	tensor SIDT = tensor::rotate_z ((solar_time-ra_base)*(TWO_PI/24.0)- TO_RADIANS(m_longitude));
	
	_vector local_xyz;
	polar_orientation p;
	p.m_theta = 0.0;// TO_RADIANS(m_longitude);
	p.m_phi = TO_RADIANS(m_latitude);
	local_xyz = p.get_xyz ();

	tensor ROTL = tensor::rotator(
		_vector::normalize (local_xyz.cross(_vector(0,0,1))),p.m_phi-PI_OVER_TWO
		);

//	m_geocentric_solar_vector = m_ROTX*ecliptic_aspect_vector;
//	_vector siderial_solar_vector = SIDT*m_geocentric_solar_vector;
//	_vector local_solar_vector = ROTL*siderial_solar_vector;
	
	tensor GLSUN = ROTL*SIDT*m_ROTX;
	m_local_solar_vector = GLSUN*ecliptic_aspect_vector;
	polar_orientation q = polar_orientation::from_axis (m_local_solar_vector);

	m_el = TO_DEGREES(q.m_phi);
	m_az = fmodf(360.0+TO_DEGREES(q.m_theta),360.0);
}
