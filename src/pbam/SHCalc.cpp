//
//  SHCalc.cpp
//  pb_solvers_code
//
//  Created by David Brookes on 9/25/15.
//  Copyright © 2015 David Brookes. All rights reserved.
//

#include "SHCalc.h"

SHCalcConstants::SHCalcConstants(const int N)
:numVals_(N), legConsts1_(N, N), legConsts2_(N, N),
shConsts_(N, N), dubFac_(N)
{
  vector<double> temp;
  temp.reserve(2 * numVals_);
  temp.push_back(1.0);
  int i, n, m;
  for (i = 1; i < 2 * numVals_; i++)
  {
    temp.push_back(temp[i-1] * sqrt(i));
  }
  
  for (n = 0; n < numVals_; n++)
  {
    for (m = 0; m <= n; m++)
    {
      if ((n-m) != 0 )
      {
        legConsts1_.set_val(n, m, (2*n-1) / (double) (n-m));
        legConsts2_.set_val(n, m, (n+m-1) / (double) (n-m));
      }
      else
      {
        legConsts1_.set_val(n, m, 0.0);
        legConsts2_.set_val(n, m, 0.0);
      }
      shConsts_.set_val(n, m, temp[n-m] / temp[n+m]);
    }
  }
  
  dubFac_[0] = 1.0;
  dubFac_[1] = 1.0;
  for (i = 2; i < numVals_; i++)
  {
    dubFac_[i] = dubFac_[i-1] * (2*i - 1);
  }
  
}

SHCalc::SHCalc(const int num_vals, shared_ptr<SHCalcConstants> _consts)
:numVals_(num_vals), P_( num_vals, num_vals), _consts_(_consts), 
Y_( num_vals, num_vals)
{
  assert (_consts_->get_n() == numVals_);
}

/*
 
 Calculate the Legendre polynomial for the input theta using the
 recursion functions for the polynomials, which are as follows:
 Pl,l (x) = (-1)^l * (2l-1)!! * (1-x^2)^(l/2)                          (1)
 Pl,l+1 (x) = x * (2l+1) * Pl,l(x)                                     (2)
 Pl,m (x) = x * (2l-1)/(l-m) * Pl-1,m(x) - (l+m-1)/(l-m) * Pl-2,m(x)   (3)
 
 This sets the member P_ to the results
 */
void SHCalc::calc_legendre(const double theta)
{
  double x = cos(theta);
  P_.set_val(0, 0, 1.0);  // base value for recursion
  P_.set_val(1, 0, x);
  
  int l, m;
  double val = 0.0;
  for (l = 0; l < numVals_; l++)
  {
    for (m = 0; m <= l; m++)
    {
      if ((l == 0 && m == 0) || (l == 1 && m == 0)) continue; //skip base values
      else if (l == m)
      {
        double dblL = (double) l;
        val = pow(-1.0, dblL) * _consts_->get_dub_fac_val(l)
        * pow(1.0-x*x, dblL/2.0);  // (1) in doc string
      }
      else if (m == l + 1)
      {
        val = x * (2*l + 1) * P_(l, l);  // (2)
      }
      else if (m < l)
      {
        val = _consts_->get_leg_consts1_val(l, m) * x * P_(l-1, m);  // (3)
        val -= _consts_->get_leg_consts2_val(l, m) * P_(l-2, m);  // (3)
      }
      P_.set_val(l, m, val);
    }
  }
}


/*
 Return the results of the legendre calculation for an n, m.
 */
double SHCalc::get_legendre_result( int n, int m )
{
  return P_( n, m);
}

/*
 Calculate the spherical harmonics according to the equation:
 
 Y_(n,m)(theta, phi) = (-1)^m * sqrt((n-m)! / (n + m)!)
 * P_(n,m)(cos(theta)) * exp(i*m*phi)
 where P_(n, m) are the associated Legendre polynomials.
 
 */
void SHCalc::calc_sh(const double theta, const double phi)
{
  calc_legendre(theta);  // first calculate legendre
  cmplx iu (0, 1.0);  // complex unit
  int n, m;
  cmplx val, mcomp;
  double shc;  // constant value
  for (n = 0; n < numVals_; n++)
  {
    for (m = 0; m < numVals_; m++)
    {
      shc = _consts_->get_sh_consts_val(n, m);
      double dblM = (double) m;
      mcomp = complex<double> (dblM, 0.0);
      val = pow(-1.0, dblM) * shc * P_(n, m) * exp(iu * mcomp * phi);
      Y_.set_val(n, m, val);
    }
  }
  
}

/*
 Return the results of the spherical harmonic calculation for an n, m.
 If m is negative, then we return the complex conjugate of the calculated
 value for the positive value of m
 */
cmplx SHCalc::get_result(const int n, const int m)
{
  if (m < 0)
  {
    return conj(Y_(n, -m));  // complex conjugate
  }
  else
  {
    return Y_(n, m);
  }
}

