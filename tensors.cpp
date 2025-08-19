
#include "stdafx.h"
#include <math.h>
#include "defines.h"
#include "tensors.h"

using namespace TENSORS;

MATH_TYPE  arccos (MATH_TYPE c)
{
	const MATH_TYPE root_half = sqrt(0.5);
	MATH_TYPE ac, theta, phi;		

//	ASSERT (fabs(c)<=1.0);
	if (fabs(c)>1)
		c=sign(c);

	if (c==0)
		return PI_OVER_TWO;

	if ((ac=fabs(c))>root_half) {
		phi = atan (sqrt(1-c*c)/ac);	
	}
	else {
		phi = PI_OVER_TWO - atan (ac/sqrt(1-c*c));
	}
	theta = (c>0?phi:PI-phi);

	ASSERT (theta<=PI+1e-6);
	ASSERT (fabs (cos(theta)-c)<1e-6);
	return theta;
}

MATH_TYPE _arg (MATH_TYPE _x, MATH_TYPE _y)
{
	MATH_TYPE theta;

	if (abs(_y)<abs(_x))
		theta = atan (abs(_y/_x));
	else
		theta = (MATH_TYPE)PI_OVER_TWO-atan(abs(_x/_y));

	if ((_x<0)&&(_y>0)) // 2nd quadrant
		theta = (MATH_TYPE)(PI-theta);

	else if ((_y<0)&&(_x<0)) // third quadrant
		theta -= (MATH_TYPE)PI;

	else if ((_y<0)&&(_x>0)) // fourth quadrant
		theta = -theta;

	return theta;
}

polar_orientation polar_orientation::from_axis (const _vector &a)
{
	polar_orientation p;
	p.m_theta = _arg (a[0],a[1]);
	p.m_phi = (MATH_TYPE)(PI_OVER_TWO - arccos (a[2]));
//	if (a[2]<0)
//		p.m_phi = -p.m_phi;
	return p;
}

_vector polar_orientation::get_xyz ()
{
	_vector result;	
	MATH_TYPE ct,st,cp,sp;
	ct = cos(m_theta);
	st = sin(m_theta);
	cp = cos(m_phi);
	sp = sin(m_phi);
	result [0] = ct*cp;
	result [1] = st*cp;
	result [2] = sp;
	return result;
}

MATH_TYPE tensor::assert_identity (const tensor &t)
{
	MATH_TYPE error;
	MATH_TYPE max_error = 0;
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++) {
		if (i==j) { 
			error = fabs(t[i][j])-1;
			if (error>max_error) max_error=error;
//			ASSERT (error<1e-1);
		}
		else { 
			error = fabs(t[i][j]);
			if (error>max_error) max_error=error;
//			ASSERT (error<1e-1);
		}
	}
	return max_error;
}

MATH_TYPE tensor::assert_orthogonality (const tensor &t)
{
	MATH_TYPE max_error;
	max_error = assert_identity (t*tensor::transpose(t));
	return max_error;
}

//inline
_vector tensor::operator * (const _vector &X) const
{
	_vector result;
	UINT i, j;
	for (i=0;i<3;i++) {
		result [i]=0;
		for (j=0;j<3;j++)
			result [i]+= A[i][j]*X[j]; }
	return result;
}

tensor tensor::operator + (const tensor &X) const
{
	tensor result;
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++){
		result [i][j]=A[i][j]+X[i][j];
	}
	return result;
}

tensor tensor::operator - (const tensor &X) const
{
	tensor result;
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++) {
		result [i][j]=A[i][j]-X[i][j];
	}
	return result;
}

//inline 
line_segment line_segment::operator+(_vector &v)
{
	line_segment result;
	result.begin = (begin+v);
	result.end = (end+v);
	return result;
}


//inline
line_segment tensor::operator * (const line_segment &b)
{
	line_segment result;
	result.begin = (*this)*b.begin;
	result.end = (*this)*b.end;
	return result;
}

tensor tensor::operator * (MATH_TYPE arg) const
{
	tensor result;
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++)
		result[i][j]=A[i][j]*arg;
	return result;
}

//inline
tensor tensor::operator * (const tensor &B) const
{
	tensor result;
	UINT i,j;//,k;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++)
	{
		result [i][j] = 
			A[i][0]*B[0][j]+
			A[i][1]*B[1][j]+
			A[i][2]*B[2][j];
	}
	return result;
}

tensor &tensor::operator += (const tensor &B)
{
	tensor result;
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++) {
		A[i][j] += B[i][j];
	}
	return *this;
}

//	constructs a tensor that rotates a _vector about
//	the indicated _vector through an angle theta.

tensor tensor::rotator (const _vector &axis, MATH_TYPE angle)
{
	tensor tBun, result;
	tBun = tensor::bun (axis,axis);
	UINT i,j;
	MATH_TYPE s, c;
	s = sin (angle);
	c = cos (angle);

	for (i=0;i<3;i++)
	for (j=0;j<3;j++) {
		result [i][j] = (1-c)*tBun [i][j] + (i==j? c:0);
	}
	result [0][1] -= axis [2]*s;
	result [0][2] += axis [1]*s;
	result [1][0] += axis [2]*s;
	result [1][2] -= axis [0]*s;
	result [2][0] -= axis [1]*s;
	result [2][1] += axis [0]*s;
	return result;
}

tensor tensor::tilt (const _vector &v)
{
	_vector q;
	tensor result;
	result [2] = v;
	result [0] = _vector::normalize (v.cross (_vector (0,-1,0)));
	result [1] = _vector::normalize (v.cross (_vector(1,0,0)));
	return result;
}

tensor tensor::rotate_x (MATH_TYPE theta)
{
	MATH_TYPE c,s;
	tensor R;
	c = cos (theta);
	s = sin (theta);
	R [0] = _vector (1,0,0);
	R [1] = _vector (0,c,-s);
	R [2] = _vector (0,s,c);
	return R;
}

tensor tensor::rotate_y (MATH_TYPE theta)
{
	MATH_TYPE c,s;
	tensor P;
	c = cos (theta);
	s = sin (theta);
	P [0] = _vector (c,0,s);
	P [1] = _vector (0,1,0);
	P [2] = _vector (-s,0,c);
	return P;
}

tensor tensor::rotate_z (MATH_TYPE theta)
{
	MATH_TYPE c,s;
	tensor Y;
	c = cos (theta);
	s = sin (theta);	
	Y [0] = _vector (c,-s,0);
	Y [1] = _vector (s,c,0);
	Y [2] = _vector (0,0,1);
	return Y;
}

tensor tensor::from_euler_angles (const _vector &v)
{
//	roll about x-axis
//	pitch about y-azis
//	yaw - rotation about z-axis

	MATH_TYPE  roll = v [0];
	MATH_TYPE  pitch = v [1];
	MATH_TYPE  yaw = v [2];

	tensor result; 
	if ((yaw==0)&&(pitch==0)&&(roll==0)) {
		result = tensor::diagonal (1,1,1);
		return result;
	}
	tensor P, Y, R;
	if (roll!=0) {
		R = tensor::rotate_x (roll);
		if ((yaw==0)&&(pitch==0))
			return R;
	}
	if (pitch!=0) {
		P = tensor::rotate_y (pitch);
		if ((yaw==0)&&(roll==0))
			return P;
	}
	if (yaw!=0) {
		Y = tensor::rotate_z (yaw);
		if ((pitch==0)&&(roll==0))
			return Y;
	}
	if ((yaw!=0)&&(pitch!=0)&&(roll!=0)){
		result = Y*P*R;
		return result;
	}
	if (roll==0) {
		result = Y*P;
		return result;
	}
	else if (pitch==0) {
		result = Y*R;
		return result;
	}
//	else if (yaw==0)
	{
		result = P*R;
		return result;
	}
	ASSERT (false);
}

MATH_TYPE tensor::trace (const tensor &T)
{
	MATH_TYPE result;
	result = T[0][0]+T[1][1]+T[2][2];
	return result;
}

tensor tensor::transpose (const tensor &T)
{
	tensor result;	
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++) {
		result [j][i] = T[i][j];
	}
	return result;
}

inline MATH_TYPE tensor::cofactor (int i, int j) const
{
	MATH_TYPE result;
	result = A[i][i]*A[j][j] - A[i][j]*A[j][i];
	return result;
}

_vector tensor::invariants (const tensor &T)
{
	_vector result;
	result [0] = trace (T);
	result [1] = -(T.cofactor(1,2) + T.cofactor(0,2) + T.cofactor(0,1));
	result [2] = tensor::determinant (T);
	return result;
}

MATH_TYPE tensor::determinant (const tensor &T)
{
	MATH_TYPE d;
	d = T[0][0]*(T[1][1]*T[2][2]-T[1][2]*T[2][1])
	   -T[0][1]*(T[1][0]*T[2][2]-T[1][2]*T[2][0])
	   +T[0][2]*(T[1][0]*T[2][1]-T[1][1]*T[2][0]);

	return d;
}

MATH_TYPE spinor::determinant ()
{
	MATH_TYPE result;
	result = A[0][0]*A[1][1] - A[0][1]*A[1][0];
	return 0;
}

spinor spinor::inverse ()
{
	spinor result;
	MATH_TYPE det = determinant ();
	MATH_TYPE k = 1/det;
	result.A [0][0] = A [1][1]*k;
	result.A [0][1] = -A [1][0]*k;;
	result.A [1][0] = -A [0][1]*k;;
	result.A [1][1] = A [0][0]*k;
	return result;
}

//	construct a diagonal tensor from three
//	parameters.

tensor tensor::diagonal (MATH_TYPE x1)
{
	tensor result;
	result [0] = _vector (x1,0,0);
	result [1] = _vector (0,x1,0);
	result [2] = _vector (0,0,x1);
	return result;
}

tensor tensor::diagonal (MATH_TYPE x1, MATH_TYPE x2, MATH_TYPE x3)
{
	tensor result;
	result [0] = _vector (x1,0,0);
	result [1] = _vector (0,x2,0);
	result [2] = _vector (0,0,x3);
	return result;
}

tensor tensor::diagonal (const _vector &X)
{
	tensor result;
	result [0] = _vector (X[0],0,0);
	result [1] = _vector (0,X[1],0);
	result [2] = _vector (0,0,X[2]);
	return result;
}

tensor tensor::inverse (MATH_TYPE) const
{
	tensor result;
	
	MATH_TYPE cf1,cf2,cf3,cf4,cf5,cf6,cf7,cf8,cf9;
	MATH_TYPE j,k;
	do
	{
		MATH_TYPE a,b,c,d,e,f,g,h,i;
		a=A[0][0];b=A[0][1];c=A[0][2];
		d=A[1][0];e=A[1][1];f=A[1][2];
		g=A[2][0];h=A[2][1];i=A[2][2];
		cf1 = (e*i-f*h);cf2 = -(d*i-f*g);cf3 = (d*h-g*e);
		cf4 = -(b*i-c*h);cf5 = (a*i-c*g);cf6 = -(a*h-b*g);
		cf7 = (b*f-c*e);cf8 = -(a*f-d*c);cf9 = (a*e-d*b);
		j = a*cf1+b*cf2+c*cf3;
		k =(j!=0?1/j:0);
	}
	while (false);
	result [0][0] = cf1*k;
	result [1][0] = cf2*k;
	result [2][0] = cf3*k;
	result [0][1] = cf4*k;
	result [1][1] = cf5*k;
	result [2][1] = cf6*k;
	result [0][2] = cf7*k;
	result [1][2] = cf8*k;
	result [2][2] = cf9*k;

	return result;
}

_vector tensor::axis_of_rotation (const tensor &T)
{
	MATH_TYPE s0, s1, s2;
	_vector axis = _vector(0,0,0);

	s0 = (MATH_TYPE)((T[2][1] - T[1][2])*0.5);
	s1 = (MATH_TYPE)((T[0][2] - T[2][0])*0.5);
	s2 = (MATH_TYPE)((T[1][0] - T[0][1])*0.5);
//	if (s2<0)
//	{
//		s0=-s0;
//		s1=-s1;
//		s2=-s2;
//	}
	axis = _vector::normalize (_vector(s0,s1,s2));
	return axis;
}

tensor tensor::bun (const _vector &a, const _vector &b)
{
	tensor result;
	UINT i,j;
	for (i=0;i<3;i++)
	for (j=0;j<3;j++) {
		result [i][j] = a[i]*b[j];
	}
	return result;
}
 