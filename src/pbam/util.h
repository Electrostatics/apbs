//
//  util.h
//  pb_solvers_code
//
/*
 Copyright (c) 2015, Teresa Head-Gordon, Lisa Felberg, Enghui Yap, David Brookes
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of UC Berkeley nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef util_hpp
#define util_hpp

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <complex>
#include <iostream>
#include <iomanip>

#include "MyMatrix.h"

/*
 Right scalar multiplication of a matrix
*/
template<typename T>
MyMatrix<T> operator*(MyMatrix<T> mat, const T& rhs)
{
  MyMatrix<T> result = MyMatrix<T>(mat.get_nrows(), mat.get_ncols());
  int i, j;
  for (i = 0; i < mat.get_nrows(); i++)
  {
    for (j= 0; j < mat.get_ncols(); j++)
    {
        result.set_val(i, j, rhs * mat(i, j));
    }
  }
  return result;
}


/*
 lhs scalar multiplication of a matrix
 */
template<typename T>
MyMatrix<T> operator*(const T& lhs, MyMatrix<T> mat)
{
  MyMatrix<T> result = MyMatrix<T>(mat.get_nrows(), mat.get_ncols());
  int i, j;
  for (i = 0; i < mat.get_nrows(); i++)
  {
    for (j= 0; j < mat.get_ncols(); j++)
    {
        result.set_val(i, j, lhs * mat(i, j));
    }
  }
  return result;
}

/*
 Class for storing three dimensional points in spherical or
 euclidean coordinates
 Should note that THETA = polar angle [ 0 <= THETA <= PI ]
 and PHI = azimuthal angle [ 0 <= PHI <= 2*PI ]
 */
template<typename T>
class Point
{
protected:
  T p1_; // x or r
  T p2_; // y or theta
  T p3_; // z or phi
  bool sph_; // whether or not the point is in spherical coordinates
  
  // convert between coordinatre systems:
  void convert_to_spherical()
  {
    if (sph_)
      return; // do nothing if already spherical
    
    T theta, phi;
    T r = sqrt(p1_*p1_ + p2_*p2_ + p3_*p3_);
    
    if (r < fabs(p3_))
      r = fabs(p3_);
    
    if (r == 0.0)
      theta = 0.0;
    else
      theta = acos(p3_/r);
    
    if ((p1_ == 0.0) && (p2_ == 0.0))
      phi = 0.0;
    else
      phi = atan2(p2_, p1_);
    
    p1_ = r;
    p2_ = theta;
    p3_ = phi;
    sph_ = true;
  }
  
  void convert_to_euclidean()
  {
    if (!sph_)
      return; // do nothing if already euclidean
    
    T x, y, z;
    x = (abs(p1_*sin(p2_)*cos(p3_)) < 1e-15) ? 0 : p1_*sin(p2_)*cos(p3_);
    y = (abs(p1_*sin(p2_)*sin(p3_)) < 1e-15) ? 0 : p1_*sin(p2_)*sin(p3_);
    z = (abs(p1_*cos(p2_)) < 1e-15) ? 0 : p1_*cos(p2_);
    
    p1_ = x;
    p2_ = y;
    p3_ = z;
    sph_ = false;
  }
  
public:
  
  // constructor given three coordinate values and whether those are spherical
  // default is to assum euclidean
  Point(T p1=T(), T p2=T(), T p3=T(), bool sph=false)
  :p1_(p1), p2_(p2), p3_(p3), sph_(sph)
  {
  }
  
  //constructor given a vector
  Point(MyVector<T> vec, bool sph=false)
  :p1_(vec[0]), p2_(vec[1]), p3_(vec[2]), sph_(sph)
  {
  }
  
  Point<T> operator=(Point<T> other)
  {
    if (this != &other) // protect against invalid self-assignment
    {
      p1_  = other.p1_;
      p2_  = other.p2_;
      p3_  = other.p3_;
      sph_ = other.sph_;
    }
    return *this;
  }
  
  T operator[](int i)
  {
    if (i == 0) return p1_;
    else if (i == 1) return p2_;
    else if (i == 2) return p3_;
    else return T();
  }
  
  void set_x(T val) { convert_to_euclidean(); p1_ = val; }
  void set_y(T val) { convert_to_euclidean(); p2_ = val; }
  void set_z(T val) { convert_to_euclidean(); p3_ = val; }
  
  void set_r(T val) { convert_to_spherical(); p1_ = val; }
  void set_theta(T val) { convert_to_spherical(); p2_ = val; }
  void set_phi(T val) { convert_to_spherical(); p3_ = val; }
  
//  
  //arithmetic operators:
  
  //scalar multiplication
  Point<T> operator*(T scalar)
  {
    Point<T> pout;
    if (sph_) convert_to_euclidean(); //for simplicity
    pout.p1_ = p1_ * scalar;
    pout.p2_ = p2_ * scalar;
    pout.p3_ = p3_ * scalar;
    return pout;
  }
  
  Point<T> operator+(Point<T> other)
  {
    Point<T> pout;
    if (sph_) convert_to_euclidean();
    pout.p1_ = p1_ + other.p1_;
    pout.p2_ = p2_ + other.p2_;
    pout.p3_ = p3_ + other.p3_;
    return pout;
  }
  
  Point<T> operator-(Point<T>& other)
  {
    Point<T> pout;
    if (sph_) convert_to_euclidean(); //for simplicity
    pout.p1_ = p1_ - other.p1_;
    pout.p2_ = p2_ - other.p2_;
    pout.p3_ = p3_ - other.p3_;
    return pout;
  }
  
  // calculate the distance to another point
  double dist(Point<T>& other)
  {
    convert_to_euclidean();
    double d = pow(this->x() - other.x(), 2);
    d += pow(this->y() - other.y(), 2);
    d += pow(this->z() - other.z(), 2);
    d = sqrt(d);
    return d;
  }
  
  Point<T> rotate(MyMatrix<T>& rotmat)
  {
    convert_to_euclidean();
    T x, y, z;
    x = rotmat(0, 0) * p1_ + rotmat(0, 1) * p2_ + rotmat(0, 2) * p3_;
    y = rotmat(1, 0) * p1_ + rotmat(1, 1) * p2_ + rotmat(1, 2) * p3_;
    z = rotmat(2, 0) * p1_ + rotmat(2, 1) * p2_ + rotmat(2, 2) * p3_;
    return Point<T>(x, y, z);
  }
  
  // Getter methods perform necessary conversions:
  const T& x()
  {
    if (sph_) convert_to_euclidean();
    return p1_;
  }
  const T& y()
  {
    if (sph_) convert_to_euclidean();
    return p2_;
  }
  const T& z()
  {
    if (sph_) convert_to_euclidean();
    return p3_;
  }
  const T& r()
  {
    if (!sph_) convert_to_spherical();
    return p1_;
  }
  const T& theta()
  {
    if (!sph_) convert_to_spherical();
    return p2_;
  }
  const T& phi()
  {
    if (!sph_) convert_to_spherical();
    return p3_;
  }
  
  T norm2() { return p1_*p1_ + p2_*p2_ + p3_*p3_; }
  
  T norm() { return sqrt(norm2()); }
  
};

/*
 Class for storing quaternions, which are defined as a real part and a vector
 part
 */
class Quaternion
{
protected:
  double w_;  // real part
  
  //imaginary coefficients:
  double a_;
  double b_;
  double c_;
  
public:
  typedef Quaternion Quat;
  
  /*
   Construct a quaternion given all the coefficients explicitly
   */
  Quaternion(double w=1, double a=0, double b=0, double c=0, bool norm=true):
  w_(w), a_(a), b_(b), c_(c)
  {
    if (norm) normalize();
  }
  
  /*
   Construct a quarernion given an angle and axis of rotation
   */
  Quaternion(double theta, Point<double> axis, bool norm=true)
  {
    w_ = cos(theta/2.0);
    double scal = sin(theta/2.0);
    a_ = (axis.x()/axis.norm()) * scal;
    b_ = (axis.y()/axis.norm()) * scal;
    c_ = (axis.z()/axis.norm()) * scal;
    if (norm) normalize();
  }
  
  /*
   Construct a quarernion from another
   */
  Quaternion(const Quaternion & q)  { *this = q;  }

  /*
   Normalize the quaternion
   */
  void normalize()
  {
    double in_norm = 1.0/sqrt(w_*w_ + a_*a_ + b_*b_  + c_*c_);
    w_ *= in_norm; a_ *= in_norm; b_ *= in_norm; c_ *= in_norm;
  }

  // return parts of quaternion
  double get_w() { return w_; }
  double get_a() { return a_; }
  double get_b() { return b_; }
  double get_c() { return c_; }
  Point<double> get_imag() { return Point <double> ( a_, b_, c_ ); }
  
  // returns the conjugate of this quarternion, negates all complex terms
  Quat conj()  { return Quat(w_, -a_, -b_, -c_); }
  
  /*
   Quaternion = operator
   */
  Quaternion & operator=(const Quaternion & q)
  {
    w_ = q.w_; a_ = q.a_; b_ = q.b_; c_ = q.c_;
    return *this;    
  }
  
  /*
   Right multiply this by another quarternion
   */
  Quat operator*(Quat rhs)
  {
    double t0, t1, t2, t3; // resulting coefficients
    double q0, q1, q2, q3; // this coeffs
    double r0, r1, r2, r3; // rhs coeffs
    
    q0=w_; q1=a_; q2=b_; q3=c_;
    r0=rhs.w_; r1=rhs.a_; r2=rhs.b_; r3=rhs.c_;
    
    t0 = q0*r0 - q1*r1 - q2*r2 - q3*r3;
    t1 = q0*r1 + q1*r0 + q2*r3 - q3*r2;
    t2 = q0*r2 - q1*r3 + q2*r0 + q3*r1;
    t3 = q0*r3 + q1*r2 - q2*r1 + q3*r0;
    
    return Quat(t0, t1, t2, t3, false);
  }
  
  Quat chooseRandom()
  {
    double s = drand48();
    double sig1 = sqrt(1.0-s);
    double sig2 = sqrt(s);
    double t1 = 2.0*drand48()*M_PI;
    double t2 = 2.0*drand48()*M_PI;
    Quat Q(cos(t2)*sig2, sin(t1)*sig1, cos(t1)*sig1, sin(t2)*sig2);
    
    return Q;
  }
  
  /*
   Rotate a point with this quarternion. If the quarternion is q and the point
   is p, then this returns q*p*conj(q)
   */
  Point<double> rotate_point(Point<double> pt)
  {
    //make the point a quarternion with real part=0
    Quat p (0, pt.x(), pt.y(), pt.z(), false);
    Quat q_conj = conj();
    
    Quat result = *this * p;
    result = result * q_conj;
    
    Point<double> pp (result.a_, result.b_, result.c_);
    return pp;
  }
//
};

typedef complex<double> cmplx;
typedef Point<double> Pt;
typedef Quaternion Quat;

#endif /* util_h */
