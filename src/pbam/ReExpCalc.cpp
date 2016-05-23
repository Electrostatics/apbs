//
//  ReExpCalc.cpp
//  pb_solvers_code
//
//  Created by David Brookes on 10/5/15.
//  Copyright © 2015 David Brookes. All rights reserved.
//

#include "ReExpCalc.h"
#include <iostream>

ReExpCoeffsConstants::ReExpCoeffsConstants(const double &kappa,
                                           const double &lambda, const int &p)
: kappa_(kappa), lambda_(lambda), p_(p),
a_(2*p, 4*p), b_(2*p, 4*p),
alpha_(2*p, 4*p), beta_(2*p, 4*p),
nu_(2*p, 4*p), mu_(2*p, 4*p)
{
  calc_a_and_b();
  calc_alpha_and_beta();
  calc_nu_and_mu();
}

void ReExpCoeffsConstants::calc_a_and_b()
{
  int m, n;
  double sign, a_val, b_val;
  
  //calculate a and b:
  for (n = 0; n < 2*p_; n++)
  {
    for (m = -n; m <= n; m++)
    {
      double nD = (double) n;
      double mD = (double) m;
      a_val = sqrt(((nD+mD+1.) * (nD-mD+1.)) / ((2.*nD+1.)* (2.*nD+3.)));
      if (m < 0)        sign = -1.0;
      else              sign = 1.0;
      b_val = sign * sqrt(((nD-mD-1.) * (nD-mD)) / ((2.*nD-1.) * (2.*nD+1.)));
      
      set_a_val(n, m, a_val);
      set_b_val(n, m, b_val);
    }
  }
}

void ReExpCoeffsConstants::calc_alpha_and_beta()
{
  int m, n;
  double alpha_val, beta_val;
  vector<double> alpha_m, beta_m;
  
  //calculate alpha and beta:
  for (n = 0; n < 2*p_; n++)
  {
    for (m = -n; m <= n; m++)
    {
      double nD = (double) n;
      double mD = (double) m;
      alpha_val = sqrt((nD + mD + 1.0) * (nD - mD + 1.0));
      beta_val  = (pow(lambda_*kappa_, 2.0)*alpha_val);
      beta_val *= (1.0 / ((2.0 * nD + 1.0) * (2.0 * nD + 3.0)));
      set_alpha( n, m, alpha_val);
      set_beta(  n, m, beta_val);
      
    }
  }
}

void ReExpCoeffsConstants::calc_nu_and_mu()
{
  int m, n, sign;
  double nu_val, mu_val;
  
  //calculate alpha and beta:
  for (n = 0; n < 2*p_; n++)
  {
    for (m = -n; m <= n; m++)
    {
      if (m < 0)        sign = -1.0;
      else              sign = 1.0;
      double nD = (double) n;
      double mD = (double) m;
      nu_val = sign * sqrt((nD - mD - 1.) * (nD - mD));
      mu_val = (pow(lambda_*kappa_, 2) * nu_val) / ((2.*nD-1.) * (2.*nD+1.));
      set_nu( n, m, nu_val);
      set_mu( n, m, mu_val);
    }
  }
}

ReExpCoeffs::ReExpCoeffs(int p, Pt v, MyMatrix<cmplx> Ytp,
                               vector<double> besselK,
                               shared_ptr<ReExpCoeffsConstants> _consts,
                               double kappa, double lambda, bool grad)
:p_(p),
v_(v),
Ytp_(Ytp),
besselK_(besselK),
kappa_(kappa),
lambda_(lambda),
grad_(grad),
_consts_(_consts),
R_(2*p, MyMatrix<cmplx> (2*p, 4*p)),
S_(2*p, MyMatrix<double> (2*p, 4*p)),
dSdR_(2*p, MyMatrix<double> (2*p, 4*p)),
dRdTheta_(2*p, MyMatrix<cmplx> (2*p, 4*p)),
prefacSing_(2*p, MyMatrix<double>(p, 2))
{
  if (besselK_.size() < 2 * p_)
  {
    throw BesselSizeException(p_, (int) besselK_.size());
  }
  
  if (Ytp_.get_nrows() < 2 * p_)
  {
    throw SHSizeException(p_, Ytp_.get_nrows());
  }
  
  double sint = sin(v_.theta());
  double sin_eps = 1e-12;

  if (sint < sin_eps)  rSing_ = true;
  else                 rSing_ = false;
 
  calc_r();
  calc_s();
  
  if (grad_)
  {
    if (!rSing_)  calc_dr_dtheta();
    else          calc_dR_pre();
    calc_ds_dr();
  }
}

void ReExpCoeffs::calc_dR_pre()
{
  int m,n;
  MyVector<double> specialSH (2*p_);
  specialSH = calc_SH_spec(1.0);
//  prefacSing_ = MyVector<MyMatrix<double> > (2*p_);
//  prefacSing_.set_val(0, MyMatrix<double> (p_, 2));
  
  for (n = 1; n < 2*p_-1; n++)
  {
//    prefacSing_.set_val(n, MyMatrix<double> (p_, 2));
    prefacSing_[n](0, 0) = 0.0; prefacSing_[n](0, 1) = specialSH[n];
  }
  
  for (n = 2; n < 2*p_-1; n++)
  {
    set_prefac_dR_val(n-1, 1, 0,(_consts_->get_b_val(n,-1)*prefacSing_[n](0,1)+
                      _consts_->get_a_val(n-1,0))/_consts_->get_b_val(n,0));
    if (n > 2)
      set_prefac_dR_val(n-1,1,1,(_consts_->get_b_val(n,1)/
                               _consts_->get_b_val(n,0))*prefacSing_[n](0,1));
    else
      set_prefac_dR_val(n-1, 1, 1, 0.0);
  }
  
  for (m = 1; m < p_-1; m++)
    for (n = m+2; n < 2*p_-m-1; n++)
    {
      set_prefac_dR_val(n-1,m+1,0, (_consts_->get_b_val(n,m-1)
                         *prefacSing_[n](m,0) + _consts_->get_a_val(n-1,m))/
                         _consts_->get_b_val(n,m));
      if (n > m+2)
        set_prefac_dR_val(n-1,m+1,1, (_consts_->get_b_val(n,m+1)/
                                _consts_->get_b_val(n,m))*prefacSing_[n](m,1));
      else
        set_prefac_dR_val(n-1, m+1, 1, 0.0);
    }
}

MyVector<double> ReExpCoeffs::calc_SH_spec( double val )
{
  int i, l;
  MyVector<double> SH (2*p_);
  MyVector<double> fact_const (4*p_);
  
  fact_const[0] = 1.0;
  for (i = 1; i < 4*p_; i++)
    fact_const[i] = fact_const[i-1]*sqrt((double) i);
  
  SH[0] = 0.0; SH[1] = -1.0; SH[2] = -val * 3.0;
  
  for (l = 3 ; l < 2*p_; l++)
    SH[l] = (val*(2*l-1)*SH[l-1] - l*SH[l-2])/(l-1);
  
  // This part converts the legendre polynomial to a spherical harmonic.
  for (l = 1; l < 2*p_; l++)
    SH[l] *= (-fact_const[l-1]/fact_const[l+1]);
  
  return SH;
}

void ReExpCoeffs::calc_r()
{
  int n, m, s;
  cmplx val;
  cmplx ic = cmplx(0, 1);
  double phi = v_.phi();
  double theta = v_.theta();
  double xi  = M_PI;
  
//  R_ = MyVector<MyMatrix<cmplx> > (2*p_); // n range of 0 to 2p-1 is needed!
  
  for (n = 0; n < 2 * p_-1; n++)
  {
//    R_.set_val(n, MyMatrix<cmplx> (2*p_, 4*p_));//s range: -2p+1 to 2p-1 needed!
    for (s = -n; s <= n; s++)
    {
      val = get_yval(n, -s);
      set_rval(n, 0, s, val);
    }
  }
  
  for (m = 0; m < p_; m++)
  {
    for (n=m+2; n < 2 * p_ - m - 1; n++)
    {
      for (s=-n+1; s < n; s++)
      {
        val = -0.5 * exp(-ic * phi) * (1 + cos(theta)) *
        _consts_->get_b_val(n, s-1) * get_rval(n, m, s-1);
        val += 0.5 * exp(ic * phi) * (1 - cos(theta)) *
        _consts_->get_b_val(n, -s-1) * get_rval(n, m, s+1);
        val -= sin(theta) * _consts_->get_a_val(n-1 , abs(s)) *
        get_rval(n, m, s);
        val *= ( exp( ic * xi ) / _consts_->get_b_val( n, m) );
        set_rval(n-1, m+1, s, val);
      }
    }
  }
}

void ReExpCoeffs::calc_s()
{
  int m, n, l;
  double val;
  double r = v_.r();
//  S_ = MyVector<MyMatrix<double> > ( 2 * p_ );
//  for (n = 0; n < 2*p_; n++)
//  {
//    S_.set_val(n, MyMatrix<double> ( 2*p_, 4*p_));
//  }
  
  for (l = 0; l < 2 * p_; l++)
  {
    val = pow((lambda_ / r), l) * (besselK_[l] * exp(-kappa_*r)) / r;
    set_sval( 0, l, 0, val );
    
    // Set S_{n,0}^0 vals
    set_sval( l, 0, 0, pow(-1.0, l) * val );
  }
  
  for (l = 1; l < 2 * p_ - 2; l++)
  {
    val  = _consts_->get_beta(l-1, 0) * get_sval( 0, l-1, 0);
    val += _consts_->get_alpha( l, 0) * get_sval( 0, l+1, 0);
    val *= -1.0 / _consts_->get_alpha(0, 0);
    set_sval( 1, l, 0, val );
  }
  
  for (n = 1; n < p_ - 1; n++)
  {
    for(l = n + 1; l < 2*p_ - n - 2; l++)
    {
      val  = _consts_->get_beta(l-1, 0) * get_sval(  n, l-1, 0);
      val += _consts_->get_beta(n-1, 0) * get_sval(n-1,   l, 0);
      val += _consts_->get_alpha( l, 0) * get_sval(  n, l+1, 0);
      val *= -1.0 / _consts_->get_alpha(n, 0);
      set_sval(n+1, l, 0, val);
    }
  }

  double val2;
  for (m = 1; m < p_ ; m++)
  {
    for (l = m; l < 2*p_ - m - 1; l++)
    {
      val  = _consts_->get_mu(  l,  -m) * get_sval(m-1, l-1, m-1);
      val += _consts_->get_nu(l+1,m-1) * get_sval( m-1, l+1, m-1);
      val *= ( -1.0 / _consts_->get_nu( m, -m) );
      set_sval(m, l, m, val);
    }
    
    for (n = m; n < p_ - 1; n++)
    {
      for (l = n + 1; l < 2*p_ - n - 2; l++)
      {
        val2  = _consts_->get_beta(l-1, m) * get_sval(  n, l-1, m);
        val2 += _consts_->get_beta(n-1, m) * get_sval(n-1,   l, m);
        val2 += _consts_->get_alpha( l, m) * get_sval(  n, l+1, m);
        val2 *= (-1.0 / _consts_->get_alpha(n, m));
        set_sval(n+1, l, m, val2);
      }
    }
  }
  
  // Filling in the rest !
  for (n = 1; n < p_ ; n++)
  {
    for (l = 0; l < p_; l++)
    {
      for (m = -n; m <= n; m++)
      {
        if (n > l) set_sval(n, l, m, pow(-1.0, n+l) * get_sval(l, n, m));
      }
    }
  }

} // end calc_s


void ReExpCoeffs::calc_dr_dtheta()
{
  int n, m, s;
  cmplx val;
  cmplx ic = cmplx(0, 1);
  double phi = v_.phi();
  double theta = v_.theta();
  double xi  = M_PI;
//  dRdTheta_ = MyVector<MyMatrix<cmplx> > (2*p_);
  
  for (n = 0; n < 2 * p_; n++)
  {
//    dRdTheta_.set_val(n, MyMatrix<cmplx> (2*p_, 4*p_));
    for (s = 0; s <= n; s++)
    {
      val = s * (cos(theta)/sin(theta)) * get_yval(n, s);
      val -= sqrt((double)(n-s)*(n+s+1)) * exp(-phi*ic) * get_yval(n, s+1);
      set_dr_dtheta_val(n, 0, -s, val);
      set_dr_dtheta_val(n, 0, s, conj(val));
    }
  }
  
  cmplx val1, val2, val3;  // intermediate calculation values
  for (m = 0; m < p_ - 1; m++)
  {
    for (n= m + 2; n < 2 * p_ - m - 1; n++)
    {
      for (s= -n + 1; s < n; s++)
      {
        val1 = sin(theta) * get_rval(n, m, s+1);
        val1 += (1-cos(theta))*get_dr_dtheta_val(n, m, s+1);
        val1 *= 0.5 * exp(ic * phi) * _consts_->get_b_val(n, -s-1);
        
        val2 = sin(theta) * get_rval(n, m, s-1);
        val2 -= (1+cos(theta))*get_dr_dtheta_val(n, m, s-1);
        val2 *= 0.5 * exp(-ic * phi) * _consts_->get_b_val(n, s-1);
        
        val3 = sin(theta) * get_dr_dtheta_val(n, m, s);
        val3 += cos(theta) * get_rval(n, m, s);
        val3 *= _consts_->get_a_val(n-1, abs(s));
        
        val = (exp(ic*xi)/(_consts_->get_b_val(n, m))) * (val1 + val2 - val3);
        
        set_dr_dtheta_val(n-1, m+1, s, val);
      }
    }
  }
} // calc_dr_dtheta

/*
 Same procedure as for calculating s with different first step
 */
void ReExpCoeffs::calc_ds_dr()
{
  int m, n, l;
  double val;
  double r = v_.r();
//  dSdR_ = MyVector<MyMatrix<double> > ( 2 * p_ );
//  for (n = 0; n < 2*p_; n++)
//  {
//    dSdR_.set_val(n, MyMatrix<double> ( 2*p_, 4*p_));
//  }
  
  for (l = 0; l < 2 * p_; l++)
  {
    val = l * besselK_[l] - ((2*l+1) * besselK_[l+1]);
    val *= pow((lambda_ / r), l) * (exp(-kappa_*r) / (r*r));
    set_dsdr_val( 0, l, 0, val );
    set_dsdr_val( l, 0, 0, pow(-1.0, l) * val );
  }
  
  for (l = 1; l < 2 * p_ - 2; l++)
  {
    val  = _consts_->get_beta(l-1, 0) * get_dsdr_val( 0, l-1, 0);
    val += _consts_->get_alpha( l, 0) * get_dsdr_val( 0, l+1, 0);
    val *= -1.0 / _consts_->get_alpha(0, 0);
    set_dsdr_val( 1, l, 0, val );
  }
  
  for (n = 1; n < p_ - 1; n++)
  {
    for(l = n + 1; l < 2*p_ - n - 2; l++)
    {
      val  = _consts_->get_beta(l-1, 0) * get_dsdr_val(  n, l-1, 0);
      val += _consts_->get_beta(n-1, 0) * get_dsdr_val(n-1,   l, 0);
      val += _consts_->get_alpha( l, 0) * get_dsdr_val(  n, l+1, 0);
      val *= -1.0 / _consts_->get_alpha(n, 0);
      set_dsdr_val(n+1, l, 0, val);
    }
  }
  
  double val2;
  for (m = 1; m < p_ ; m++)
  {
    for (l = m; l < 2*p_ - m - 1; l++)
    {
      val  = _consts_->get_mu(  l,  -m) * get_dsdr_val(m-1, l-1, m-1);
      val += _consts_->get_nu(l+1,m-1) * get_dsdr_val( m-1, l+1, m-1);
      val *= ( -1.0 / _consts_->get_nu( m, -m) );
      set_dsdr_val(m, l, m, val);
    }
    
    for (n = m; n < p_ - 1; n++)
    {
      for (l = n + 1; l < 2*p_ - n - 2; l++)
      {
        val2  = _consts_->get_beta(l-1, m) * get_dsdr_val(  n, l-1, m);
        val2 += _consts_->get_beta(n-1, m) * get_dsdr_val(n-1,   l, m);
        val2 += _consts_->get_alpha( l, m) * get_dsdr_val(  n, l+1, m);
        val2 *= (-1.0 / _consts_->get_alpha(n, m));
        set_dsdr_val(n+1, l, m, val2);
      }
    }
  }
  
  // Filling in the rest !
  for (n = 1; n < p_ ; n++)
    for (l = 0; l < p_; l++)
      for (m = -n; m <= n; m++)
        if (n > l) set_dsdr_val(n, l, m, pow(-1.0, n+l) * get_dsdr_val(l, n, m));
  

} // calc_ds_dr

void ReExpCoeffs::print_R()
{
  cout << "This is my R" << endl;
  for (int m = 0; m < 9; m++)
  {
    cout << "\t---m = " << m << "---" << endl;
    for (int n = m; n < 9; n++)
    {
      for (int l = -n; l <= n; l++)
      {
        double r = fabs(get_rval(n, m, l).real())>1e-15 ?
                        get_rval(n, m, l).real() : 0;
        double i = fabs(get_rval(n, m, l).imag())>1e-15 ?
                        get_rval(n, m, l).imag() : 0;
        cout << "(" << r << "," << i << ") | ";
      }
      cout << endl;
    }
  }
  cout << endl;
}

void ReExpCoeffs::print_dRdtheta()
{
  cout << "This is my dRdtheta" << endl;
  for (int m = 0; m < p_; m++)
  {
    cout << "\t---m = " << m << "---" << endl;
    for (int n = m; n < p_; n++)
    {
      for (int l = -n; l <= n; l++)
      {
        double r = fabs(get_dr_dtheta_val(n, m, l).real())>1e-15 ?
                        get_dr_dtheta_val(n, m, l).real() : 0;
        double i = fabs(get_dr_dtheta_val(n, m, l).imag())>1e-15 ?
                        get_dr_dtheta_val(n, m, l).imag() : 0;
        cout << "(" << r << "," << i << ") | ";
      }
      cout << endl;
    }
  }
  cout << endl;
}

void ReExpCoeffs::print_dRdphi()
{
  cout << "This is my dRdphi" << endl;
  for (int m = 0; m < p_; m++)
  {
    cout << "\t---m = " << m << "---" << endl;
    for (int n = m; n < p_; n++)
    {
      for (int l = -n; l <= n; l++)
      {
        double r = fabs(get_dr_dphi_val(n, m, l).real())>1e-15 ?
        get_dr_dphi_val(n, m, l).real() : 0;
        double i = fabs(get_dr_dphi_val(n, m, l).imag())>1e-15 ?
        get_dr_dphi_val(n, m, l).imag() : 0;
        cout << "(" << r << "," << i << ") | ";
      }
      cout << endl;
    }
  }
  cout << endl;
 
}

void ReExpCoeffs::print_S()
{
  cout << "This is my S" << endl;
  for (int m = 0; m < p_; m++)
  {
    cout << "\t---m = " << m << "---" << endl;
    for (int l = m; l < p_; l++)
    {
      for (int n = m; n <= l; n++)
      {
        double r = fabs(get_sval(n, l, m))>1e-15 ? get_sval(n, l, m) : 0;
        cout << r << " | ";
      }
      cout << endl;
    }
  }
  cout << endl;
}

void ReExpCoeffs::print_dSdr()
{
  cout << "This is my dSdr" << endl;
  for (int m = 0; m < p_; m++)
  {
    cout << "\t---m = " << m << "---" << endl;
    for (int l = m; l < p_; l++)
    {
      for (int n = m; n <= l; n++)
      {
        double r = fabs(get_dsdr_val(n,l,m))>1e-15 ? get_dsdr_val(n,l,m) : 0;
        cout << r << " | ";
      }
      cout << endl;
    }
  }
  cout << endl;
}

