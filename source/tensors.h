
#include "stdafx.h"
#include <math.h>
//#include "defines.h"

#define MATH_TYPE float


MATH_TYPE arccos (MATH_TYPE c);

#define SQRT_MAGIC_F 0x5f3759df
// 64  Bit float magic number
#define SQRT_MAGIC_D 0x5fe6ec85e7de30da

DWORD GF32 (DWORD p);
short GF16 (short p);

MATH_TYPE _arg (MATH_TYPE _x, MATH_TYPE _y);

inline float invSqrt(const float x)
{
  const float xhalf = 0.5f*x;
  union // get bits for floating value
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
  return u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy
}

inline int sign (MATH_TYPE x)
{
	if (x<0)
		return -1;
	if (x>0)
		return 1;
	else
		return 0;
}

class rectangular_prism;

namespace TENSORS
{
class _vector;
class tensor;

class random
{
public:
	static short GF16 (short p);
	static DWORD GF32 (DWORD p);
	static MATH_TYPE frnd ();
	static MATH_TYPE range (int max);
	static _vector unit__vector ();
	static tensor rotator ();
	static tensor stretch (_vector v);
};

class _point2d
{
public:
	MATH_TYPE x,y;
};

class _vector
{
private:
	MATH_TYPE x [3];

public:
	inline _vector () { };

	inline _vector (MATH_TYPE x0, MATH_TYPE x1, MATH_TYPE x2) {
		x [0] = x0;
		x [1] = x1;
		x [2] = x2;
	};
	inline MATH_TYPE &operator [] (int i) const {
		return *((MATH_TYPE*)&x[i]);	
	};
	inline _vector &operator << (const MATH_TYPE arg) {
		x [0] = arg;
		x [1] = arg;
		x [2] = arg;
		return *this;
	};
#if 0
	_vector &operator = (const _vector &arg) {
		x [0] = arg.x [0];
		x [1] = arg.x [1];
		x [2] = arg,x [2];
		return *this;
	};
#endif
	inline _vector operator + (const _vector &arg) const {
		_vector result;
		result.x [0] = x[0] + arg.x[0];
		result.x [1] = x[1] + arg.x[1];
		result.x [2] = x[2] + arg.x[2];
		return result;
	};
	inline _vector operator - (const _vector &arg) const 
	{
		_vector result;
		result.x [0] = x[0] - arg.x[0];
		result.x [1] = x[1] - arg.x[1];
		result.x [2] = x[2] - arg.x[2];
		return result;
	};
	inline _vector operator - () const 
	{
		_vector result;
		result.x [0] = - x[0];
		result.x [1] = - x[1];
		result.x [2] = - x[2];
		return result;
	};
	inline _vector &operator += (const _vector &arg) 
	{
		x [0] += arg.x [0];
		x [1] += arg.x [1];
		x [2] += arg.x [2];
		return *this;
	};
	inline _vector &operator -= (const _vector &arg)
	{
		x [0] -= arg.x [0];
		x [1] -= arg.x [1];
		x [2] -= arg.x [2];
		return *this;
	};
	inline _vector operator * (const MATH_TYPE arg) const 
	{
		_vector result;
		result.x [0] = x[0]*arg;
		result.x [1] = x[1]*arg;
		result.x [2] = x[2]*arg;
		return result;
	};
	inline _vector &operator *= (const MATH_TYPE arg) 
	{
		_vector result;
		x [0] *=arg;
		x [1] *=arg;
		x [2] *=arg;
		return *this;
	};
	inline _vector operator / (const MATH_TYPE arg) const 
	{
		_vector result;
		result.x [0] = x[0]/arg;
		result.x [1] = x[1]/arg;
		result.x [2] = x[2]/arg;
		return result;
	};
	inline static _vector random (int max) 
	{
		_vector result;
		result.x [0] = (MATH_TYPE)(max - 2*random::range (max));
		result.x [1] = (MATH_TYPE)(max - 2*random::range (max));
		result.x [2] = (MATH_TYPE)(max - 2*random::range (max));
		return result;
	};
	inline static _vector normalize (const _vector &X)
	{
		_vector result;
		MATH_TYPE scale,magnitude = 0;
		for (int i=0;i<3;i++)
			magnitude+=X.x[i]*X.x[i];

		if (magnitude!=0) {
			scale = 1/sqrt (magnitude);
			for (int i=0;i<3;i++)
			result.x [i] = X.x[i]*scale;
		}
		else
			result << 0;
		return result;
	};
	inline _vector cross (const _vector &B) const 
	{
		_vector result;
		result.x [0] = x[1]*B.x[2]-x[2]*B.x[1]; 
		result.x [1] = x[2]*B.x[0]-x[0]*B.x[2];
		result.x [2] = x[0]*B.x[1]-x[1]*B.x[0];
		return result;
	};
	inline MATH_TYPE dot (const _vector &B) const 
	{
		MATH_TYPE result;
		result = x[0]*B.x[0]+x[1]*B.x[1]+x[2]*B.x[2];
		return result;
	};
	inline static MATH_TYPE length (const _vector &A)
	{
		MATH_TYPE result;
		result = sqrt (A.dot (A));
		return result;
	};
	inline static _vector midpoint (const _vector &A, const _vector &B)
	{
		_vector result;
		result = (A+B)*0.5;
		return result;
	}
	inline static _vector spin (const _vector &B);
};

class polar_orientation
{
public:
	MATH_TYPE	m_theta, m_phi;

public:
	polar_orientation (MATH_TYPE theta=0, MATH_TYPE phi=0)
	{ 
		m_theta = theta;
		m_phi = phi;
	};
	static polar_orientation from_axis (const _vector &arg);
	_vector get_xyz ();
};

class euler_orientation: public _vector
{
public:
	static euler_orientation from_axis (const _vector &arg);
};

class point: public _vector
{
public:
	point () {};
	point (MATH_TYPE x0, MATH_TYPE x1, MATH_TYPE x2): _vector (x0,x1,x2) {};

};

class line_segment
{
public:
	_vector begin;
	_vector end;

	line_segment () {};
	line_segment (const point &p1, const point &p2) { begin = p1; end = p2; };
	line_segment operator+(_vector &v);
};

class plane_equation
{
protected:
	MATH_TYPE  A,B,C;

public:
	MATH_TYPE	D;
	plane_equation () {};
	inline plane_equation (MATH_TYPE a, MATH_TYPE b, MATH_TYPE c, MATH_TYPE d) {A=a;B=b;C=c;D=d;};
	inline plane_equation (const _vector &axis, MATH_TYPE d) {A=axis[0];B=axis[1];C=axis[2];D=d;};
	inline plane_equation (const _vector &axis, const _vector &pt)
	{
		A = axis[0]; B = axis[1]; C = axis[2]; 
		D = -(axis[0]*pt[0]+axis[1]*pt[1]+axis[2]*pt[2]);
	}
	inline _vector normal () const
	{ 
		if (D<0)
			return _vector(A,B,C);
		else
			return _vector(-A,-B,-C);
	};
	void operator << (rectangular_prism &b);
};

class line_equation
{
protected:
	_vector m_pt, m_axis;

public:
	line_equation () {};
	line_equation (const _vector &a, const point &p) { m_axis=a; m_pt=p; };
//	line_equation () {const point &p1, const point &p2} { m_axis = p1-p2; m_pt=p1;};
	line_equation (const plane_equation &p1, const plane_equation &p2)
	{
		MATH_TYPE det_x,det_y,det_z;

		_vector m1 = p1.normal();
		_vector m2 = p2.normal();
		m_axis = m1.cross (m2);

		det_x = m_axis[0];
		det_y = m_axis[1];
		det_z = m_axis[2];

		if (abs(det_x)>abs(det_y)&&abs(det_x)>abs(det_z))
		{
			m_pt[0] = 0;
			m_pt[1] = -(p1.D*m2[2]-p2.D*m1[2])/det_x;
			m_pt[2] = (p1.D*m2[1]-p2.D*m1[1])/det_x;
		}
		else if (abs(det_y)>abs(det_x)&&abs(det_y)>abs(det_z))
		{
			m_pt[0] = (p1.D*m2[2]-p2.D*m1[2])/det_y;
			m_pt[1] = 0;
			m_pt[2] = -(p1.D*m2[0]-p2.D*m1[0])/det_y;	
		}
		else if (det_z!=0)
		{
			m_pt[0]= -(p1.D*m2[1]-p2.D*m1[1])/det_z;
			m_pt[1]= (p1.D*m2[0]-p2.D*m1[0])/det_z;
			m_pt[2]= 0;
		}
		else
		{
			m_pt = _vector(0,0,0);
		}
		m_axis = _vector::normalize (m_axis);
	}
};

class spinor
{
public:
	MATH_TYPE A[2][2];
	MATH_TYPE determinant ();
	spinor inverse ();
};

class tensor
{
private:
	_vector A [3];

public:
	tensor () {};
	tensor (_vector v0, _vector v1, _vector v2)
	{
		A[0]=v0;
		A[1]=v1;
		A[2]=v2;
	}
	tensor inverse (MATH_TYPE q=1) const;
	inline _vector &tensor::operator [] (int row) const {
		return *((_vector*)&(A[row]));
	}
	tensor operator + (const tensor &) const;
	tensor operator - (const tensor &) const;
	_vector operator * (const _vector &) const;
	tensor operator * (const tensor &) const;
	tensor operator * (MATH_TYPE) const;
	tensor &operator += (const tensor &);
	line_segment operator * (const line_segment &b);
	MATH_TYPE cofactor (int,int) const;

	static MATH_TYPE assert_orthogonality (const tensor &t);
	static MATH_TYPE assert_identity (const tensor &t);
	static MATH_TYPE trace (const tensor &);
	static MATH_TYPE determinant (const tensor &);

	static tensor diagonal (MATH_TYPE);
	static tensor diagonal (MATH_TYPE,MATH_TYPE,MATH_TYPE);
	static tensor diagonal (const _vector &);
	static tensor transpose (const tensor &);

	static tensor rotate_x (MATH_TYPE theta);
	static tensor rotate_y (MATH_TYPE theta);
	static tensor rotate_z (MATH_TYPE theta);
	static tensor rotator (const _vector &axis, MATH_TYPE angle);

	static _vector invariants (const tensor &);
	static _vector axis_of_rotation (const tensor &T);
	static tensor from_euler_angles (const _vector&);
	static tensor bun (const _vector &, const _vector &);
	static tensor tilt (const _vector &);
	
	tensor rt_cauchy () const;
		
};

class cubic_equation
{
public:
	double a,b,c,d;
	cubic_equation ();
	cubic_equation (double r1, double r2, double r3);
};

class cubic_solver
{
private:	
	double q,r,m_radical;
	double A1,A2,A3;	

public:
	double roots [3], im;
	bool	m_bRealRoots;
	cubic_solver (const cubic_equation &, bool force_real=false);
//	void compute_real_roots ();
//	void compute_complex_roots ();
};

class cauchy_green
{
protected:
	MATH_TYPE cube_root_of_determinant_of_f;
	_vector eigenvalues_of_c, eigenvalues_of_u;
	tensor F1, C1, Q;

	bool find_eigenvalues_of_c1();
	bool find_eigen_vector_of_c1 (int j);
	bool find_eigen_vectors ();

public:
	MATH_TYPE determinant_of_f;
	tensor R,U,C,F;
	void main ();
	void set_deformations (const tensor &);
	void solve_cauchy_green ();
};

class quaternion
{
public:
	MATH_TYPE w,x,y,z;

	quaternion operator * (const quaternion &a) const
	{
		quaternion result;
		result.w = w*a.w - x*a.x - y*a.y - z*a.z;
		result.x = w*a.x + x*a.w + y*a.z - z*a.y;
		result.y = w*a.y - x*a.z + y*a.w + z*a.x;
		result.z = w*a.z + x*a.y - y*a.x + z*a.w;
		return result;
	}
};

} // namespace





