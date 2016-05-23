
//  ReExpCalc.h
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

#ifndef ReExpCalc_h
#define ReExpCalc_h

#include "BesselCalc.h"
#include "SHCalc.h"

using namespace std;

class BesselSizeException
{
protected:
int p_;
int besselSize_;

public:
BesselSizeException(const int p, const int besselSize)
:p_(p), besselSize_(besselSize)
{
}

virtual const char* what() const throw()
{
  ostringstream ss;
  ss << "The bessel vector is the wrong size. It is supposed to be: " <<
       2 * p_ <<" but is:  " << besselSize_ << endl;
  return ss.str().c_str();
}
};


class SHSizeException
{
protected:
  int p_;
  int shSize_;
  
public:
  SHSizeException(const int p, const int shSize)
  :p_(p), shSize_(shSize)
  {
  }
  
  virtual const char* what() const throw()
  {
    ostringstream ss;
    ss << "The spherical harmonics vector is the wrong size." <<
    "It is supposed to be: " << 2 * p_ <<" but is:  " << shSize_ << endl;
    return ss.str().c_str();
  }
};


/*
 Class for pre-computing the constants in the re-expansion coefficne
 */
class ReExpCoeffsConstants
{
protected:
  MyMatrix<double> a_;  // first index is m, second is n
  MyMatrix<double> b_;
  MyMatrix<double> alpha_;
  MyMatrix<double> beta_;
  MyMatrix<double> nu_;
  MyMatrix<double> mu_;
  double lambda_; // uniform scaling factor (section 4.5 of Lotan 2006)
  int p_;
  double kappa_; //from Constants
  
  void calc_a_and_b();
  void calc_alpha_and_beta();
  void calc_nu_and_mu();
  
public:
  
  ReExpCoeffsConstants() { }

  ReExpCoeffsConstants(double const& kappa, double const& lambda, int const &p);
  
  double get_a_val(int n, int m)  { return a_(n, m + 2*p_); }
  double get_b_val(int n, int m)  { return b_(n, m + 2*p_); }
  double get_alpha(int n, int m)  { return alpha_(n, m + 2*p_); }
  double get_beta(int n, int m)   { return  beta_(n, m + 2*p_); }
  double get_nu(int n, int m)     { return nu_(n, m + 2*p_);}
  double get_mu(int n, int m)     { return mu_(n, m + 2*p_);}
  
  void set_a_val( int n, int m, double val) {a_.set_val(n, m + 2*p_, val);}
  void set_b_val(int n, int m, double val)  {b_.set_val(n, m + 2*p_, val);}
  void set_alpha( int n, int m, double val) {alpha_.set_val(n, m + 2*p_, val);}
  void set_beta(int n, int m, double val)   {beta_.set_val(n, m + 2*p_, val);}
  void set_nu( int n, int m, double val)    {nu_.set_val(n, m + 2*p_, val);}
  void set_mu(int n, int m, double val)     {mu_.set_val(n, m + 2*p_, val);}
  
};

/*
 Class representing one entry in the re-expansion coefficient matrix. So if
 that matrix is T (as in Lotan 2006), then this class contains the info
 for one T^(i,j) and its derivatives
 */
class ReExpCoeffs
{
protected:
  int p_; // max value of n when solving for A
  shared_ptr<ReExpCoeffsConstants> _consts_;
  
  /*
   R_ contains rotation coefficients for this entry. R_ has three
   indices: R[n](m, s)
   And the range of each:  0 <= n <  poles
   -n <= m <= n, but -m = conj(+m), so really just [0,p)
   -n <= s <= n
   */
  VecOfMats<cmplx>::type  R_;
  
  /*
   S_ contains translation coefficients for this entry. S_ has three
   indices: S[m](n, l)
   And the range of each:  0 <= n <  poles
   0 <= l <= poles
   -n <= m <= n
   */
  VecOfMats<double>::type S_;
  
  /*
   The useful derivatives are S with respect to r and R with respect to
   theta. dR/dPhi is also used, but can be calculated in the getter method
   */
  VecOfMats<cmplx>::type    dRdTheta_;
  VecOfMats<double>::type   dSdR_;

  double kappa_; //from Constants
  double lambda_; // uniform scaling factor (section 4.5 of Lotan 2006)
  
  Pt v_; //computing re-expansion along this vector
  
  bool grad_;
  bool rSing_;

  /*
   Bessel function for this v_. If the bessel function be k_n ( z ) then
   this value should be for n = 2*p_ and z = kappa*r
   */
  vector<double> besselK_;
  
  /*
   Spherical harmonics for this v_:
   */
  MyMatrix<cmplx> Ytp_;
  
  VecOfMats<double>::type prefacSing_; // for singular case
  
  void calc_r();  // calculate all the values for R_
  void calc_s(); // calculate all the values for S_
  void calc_dr_dtheta();
  void calc_ds_dr();

  void calc_dR_pre(); // compute prefactors for singularities
  
public:
  ReExpCoeffs() { };
  
  ReExpCoeffs(int p, Pt v, MyMatrix<cmplx> Ytp, vector<double> besselK_,
              shared_ptr<ReExpCoeffsConstants> _consts, double kappa,
              double lambda, bool grad = false);
  
  MyVector<double> calc_SH_spec( double val ); // for singularities
  
  bool isSingular()  { return rSing_; }  
  Pt get_TVec()       { return v_; }
  
  cmplx get_yval(int n, int s)
  {
    if ( s < 0 ) return conj(Ytp_(n, -s));
    else         return Ytp_(n, s);
  }
  
  cmplx get_rval(int n, int m, int s)
  {
    if ( m < 0 ) return conj(R_[n](-m, -s+2*p_));
    else         return      R_[n]( m,  s+2*p_);
  }
  
  double get_sval(int n, int l, int m)
  {
    if ( m < 0 ) return S_[n](l, -m+2*p_);
    else         return S_[n](l,  m+2*p_);
  }
  
  double get_dsdr_val(int n, int l, int m)
  { 
    if ( m < 0 ) return dSdR_[n](l, -m+2*p_);
    else         return dSdR_[n](l,  m+2*p_);
  }
  
  
  cmplx get_dr_dtheta_val(int n, int m, int s)
  {
    if ( m < 0 ) return conj(dRdTheta_[n](-m, -s+2*p_));
    else         return dRdTheta_[n](m, s+2*p_);
  }
  
  /*
   dR/dPhi is just -i * s * R
   */
  cmplx get_dr_dphi_val(int n, int m, int s)
  {
    cmplx ic = cmplx(0, 1);
    cmplx sc = cmplx(s, 0);
    cmplx drdp = -ic * sc * get_rval(n, m, s);
    return drdp;
  }
  
  double get_prefac_dR_val(int n, int m, int l)
  {
    return prefacSing_[n](m, l);
  }
  
  void print_R();
  void print_dRdtheta();
  void print_dRdphi();
  void print_S();
  void print_dSdr();
  
  void set_rval(int n, int m, int s, cmplx val)
  {
    (&R_[n])->set_val( m, s+2*p_, val);
  }
  
  void set_sval(int n, int l, int m, double val)
  {
    (&S_[n])->set_val(l, m+2*p_, val);
  }
  
  void set_dr_dtheta_val(int n, int m, int s, cmplx val)
  {
    (&dRdTheta_[n])->set_val( m, s+2*p_, val );
  }
  
  void set_dsdr_val(int n, int l, int m, double val)
  {
    (&dSdR_[n])->set_val(l, m+2*p_, val);
  }
  
  void set_prefac_dR_val(int n, int m, int l, double val)
  {
    (&prefacSing_[n])->set_val(m, l, val);
  }
  
};
  

#endif /* ReExpCalc_hpp */